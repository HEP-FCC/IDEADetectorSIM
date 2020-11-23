/******************************************************************************
*
* CAEN SpA - Front End Division
* Via Vetraia, 11 - 55049 - Viareggio ITALY
* +390594388398 - www.caen.it
*
***************************************************************************//**
* \note TERMS OF USE:
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License as published by the Free Software
* Foundation. This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. The user relies on the
* software, documentation and results solely at his own risk.
*
*  Description:
*  -----------------------------------------------------------------------------
*  This is a demo program that can be used with any model of the CAEN's
*  digitizer family. The purpose of WaveDump is to configure the digitizer,
*  start the acquisition, read the data and write them into output files
*  and/or plot the waveforms using 'gnuplot' as an external plotting tool.
*  The configuration of the digitizer (registers setting) is done by means of
*  a configuration file that contains a list of parameters.
*  This program uses the CAENDigitizer library which is then based on the
*  CAENComm library for the access to the devices through any type of physical
*  channel (VME, Optical Link, USB, etc...). The CAENComm support the following
*  communication paths:
*  PCI => A2818 => OpticalLink => Digitizer (any type)
*  PCI => V2718 => VME => Digitizer (only VME models)
*  USB => Digitizer (only Desktop or NIM models)
*  USB => V1718 => VME => Digitizer (only VME models)
*  If you have want to sue a VME digitizer with a different VME controller
*  you must provide the functions of the CAENComm library.
*
*  -----------------------------------------------------------------------------
*  Syntax: WaveDump [ConfigFile]
*  Default config file is "WaveDumpConfig.txt"
******************************************************************************/

#define HAVE_CAEN_BRD


#define WaveDump_Release        "3.8.2"
#define WaveDump_Release_Date   "Semptember 2017"
#define DBG_TIME
#include <stdlib.h>
#include <unistd.h>
#include <CAENDigitizer.h>
#include <CAENDigitizerType.h>
#include "DCHDAQ.h"
#include "WDconfig.h"
#include "WDplot.h"
#include "fft.h"
#include "keyb.h"
#include "X742CorrectionRoutines.h"
#define MAX_FILE_SIZE 1073741824

#ifdef HAVE_CAEN_BRD
#include <CAENVMEBridge.h>
#endif

#define clear() printf("\033[H\033[J")


int X742Chs[] = {
  0, 1, 2, 3, 5, 6, 7,
  8, 9, 10,11, 12, 13, 14, 15,
  16, 17,18, 21, 23
};

int grON[] = {0,0};

#define MAX_ALLOW_DEV 10
uint8_t numberOfDevices = 1;
uint8_t boardIds[MAX_ALLOW_DEV];
uint8_t deviceTypes[MAX_ALLOW_DEV];
uint8_t channelsPerBoard[MAX_ALLOW_DEV];


char LAM_ON;
int eor = 0;
int runON = 0;

extern int dc_file[MAX_CH];
extern int thr_file[MAX_CH];
int cal_ok[MAX_CH] = {0};

/**
 * Error messages
 */
typedef enum  {
    ERR_NONE = 0,
    ERR_CONF_FILE_NOT_FOUND,
    ERR_DGZ_OPEN,
    ERR_BOARD_INFO_READ,
    ERR_INVALID_BOARD_TYPE,
    ERR_DGZ_PROGRAM,
    ERR_MALLOC,
    ERR_RESTART,
    ERR_INTERRUPT,
    ERR_READOUT,
    ERR_EVENT_BUILD,
    ERR_HISTO_MALLOC,
    ERR_UNHANDLED_BOARD,
    ERR_OUTFILE_WRITE,
    ERR_OVERTEMP,
    ERR_DUMMY_LAST,
} ERROR_CODES;
static char ErrMsg[ERR_DUMMY_LAST][100] = {
  /**
   * ERR_NONE
   */
  "No Error",
  /**
   * ERR_CONF_FILE_NOT_FOUND
   */
  "Configuration File not found",
  /**
   * ERR_DGZ_OPEN
   */
  "Can't open the digitizer",
  /**
   * ERR_BOARD_INFO_READ
   */
  "Can't read the Board Info",
  /**
   * ERR_INVALID_BOARD_TYPE
   */
  "Can't run WaveDump for this digitizer",
  /**
   * ERR_DGZ_PROGRAM
   */
  "Can't program the digitizer",
  /**
   * ERR_MALLOC
   */
  "Can't allocate the memory for the readout buffer",
  /**
   * ERR_RESTART
   */
  "Restarting Error",
  /**
   * ERR_INTERRUPT
   */
  "Interrupt Error",
  /**
   * ERR_READOUT
   */
  "Readout Error",
  /**
   * ERR_EVENT_BUILD
   */
  "Event Build Error",
  /**
   * ERR_HISTO_MALLOC
   */
  "Can't allocate the memory fro the histograms",
  /**
   * ERR_UNHANDLED_BOARD
   */
  "Unhandled board type",
  /**
   * ERR_OUTFILE_WRITE
   */
  "Output file write error",
  /**
   * ERR_OVERTEMP
   */
  "Over Temperature"
};


#ifndef max
#define max(a, b) ((a > b) ? a : b)
#endif

static CAEN_DGTZ_IRQMode_t INTERRUPT_MODE = CAEN_DGTZ_IRQ_MODE_ROAK;

/* ###########################################################################
*  Functions
*  ########################################################################### */
/*! \fn      static long get_time()
*   \brief   Get time in milliseconds
*
*   \return  time in msec
*/
static long get_time() {
  long time_ms;
#ifdef WIN32
  struct _timeb timebuffer;
  _ftime(&timebuffer);
  time_ms = (long) timebuffer.time * 1000 +
      (long) timebuffer.millitm;
#else
  struct timeval t1;
  struct timezone tz;
  gettimeofday(&t1, &tz);
  time_ms = (t1.tv_sec) * 1000 + t1.tv_usec / 1000;
#endif
  return time_ms;
}

struct CAENDGTZvars {
  int handle;
  WaveDumpConfig_t WDcfg;
  WaveDumpRun_t WDrun;
  CAEN_DGTZ_ErrorCode ret;
  ERROR_CODES ErrCode;
  int i;
  int ch;
  int Nb;
  int Ne;
  uint32_t BufferSize;
  uint32_t NumEvents;
  char *buffer;
  char *EventPtr;
  char ConfigFileName[100];
  int isVMEDevice;
  uint64_t PrevRateTime;
  CAEN_DGTZ_BoardInfo_t BoardInfo;
  CAEN_DGTZ_EventInfo_t EventInfo;
  /**
   * generic event struct with 16 bit data (10, 12, 14 and 16 bit digitizers
   */
  CAEN_DGTZ_UINT16_EVENT_t *Event16;
  /**
   * generic event struct with 8 bit data (only for 8 bit digitizers)
   */
  CAEN_DGTZ_UINT8_EVENT_t *Event8;
  /**
   * custom event struct with 8 bit data (only for 8 bit digitizers)
   */
  CAEN_DGTZ_X742_EVENT_t *Event742;
  WDPlot_t *PlotVar;
  FILE *f_ini;
  CAEN_DGTZ_DRS4Correction_t X742Tables[MAX_X742_GROUP_SIZE];
  /**
   * Init to the bigger positive number
   */
  int ReloadCfgStatus;
} dgtzVars;

uint64_t prevRateTime;


void printError(const ERROR_CODES* errCode) {
  if (errCode) {
    printf("\a%s\n", ErrMsg[*errCode]);
#ifndef WIN32
    printf("Press a key to quit\n");
    getch();
#endif
  }
}

int GetMoreBoardInfo() {
  int ret;
  switch (dgtzVars.BoardInfo.FamilyCode) {
    CAEN_DGTZ_DRS4Frequency_t freq;
    case CAEN_DGTZ_XX724_FAMILY_CODE:
    case CAEN_DGTZ_XX781_FAMILY_CODE:
    case CAEN_DGTZ_XX780_FAMILY_CODE:
      dgtzVars.WDcfg.Nbit = 14;
      dgtzVars.WDcfg.Ts = 10.0;
      break;
    case CAEN_DGTZ_XX720_FAMILY_CODE:
      dgtzVars.WDcfg.Nbit = 12;
      dgtzVars.WDcfg.Ts = 4.0;
      break;
    case CAEN_DGTZ_XX721_FAMILY_CODE:
      dgtzVars.WDcfg.Nbit = 8;
      dgtzVars.WDcfg.Ts = 2.0;
      break;
    case CAEN_DGTZ_XX731_FAMILY_CODE:
      dgtzVars.WDcfg.Nbit = 8;
      dgtzVars.WDcfg.Ts = 2.0;
      break;
    case CAEN_DGTZ_XX751_FAMILY_CODE:
      dgtzVars.WDcfg.Nbit = 10;
      dgtzVars.WDcfg.Ts = 1.0;
      break;
    case CAEN_DGTZ_XX761_FAMILY_CODE:
      dgtzVars.WDcfg.Nbit = 10;
      dgtzVars.WDcfg.Ts = 0.25;
      break;
    case CAEN_DGTZ_XX740_FAMILY_CODE:
      dgtzVars.WDcfg.Nbit = 12;
      dgtzVars.WDcfg.Ts = 16.0; break;
    case CAEN_DGTZ_XX725_FAMILY_CODE:
      dgtzVars.WDcfg.Nbit = 14;
      dgtzVars.WDcfg.Ts = 4.0;
      break;
    case CAEN_DGTZ_XX730_FAMILY_CODE:
      dgtzVars.WDcfg.Nbit = 14;
      dgtzVars.WDcfg.Ts = 2.0;
      break;
    case CAEN_DGTZ_XX742_FAMILY_CODE:
      dgtzVars.WDcfg.Nbit = 12;
      ret = CAEN_DGTZ_GetDRS4SamplingFrequency(
          dgtzVars.handle, &freq);
      if (ret != CAEN_DGTZ_Success) {
        return CAEN_DGTZ_CommError;
      }
      switch (freq) {
        case CAEN_DGTZ_DRS4_1GHz:
          dgtzVars.WDcfg.Ts = 1.0;
          break;
        case CAEN_DGTZ_DRS4_2_5GHz:
          dgtzVars.WDcfg.Ts = (float) 0.4;
          break;
        case CAEN_DGTZ_DRS4_5GHz:
          dgtzVars.WDcfg.Ts = (float)0.2;
          break;
        case CAEN_DGTZ_DRS4_750MHz:
          dgtzVars.WDcfg.Ts = (float) (1.0 / 750.0) * 1000.0;
          break;
      }
      switch (dgtzVars.BoardInfo.FormFactor) {
        case CAEN_DGTZ_VME64X_FORM_FACTOR:
          dgtzVars.WDcfg.MaxGroupNumber = 4;
          break;
        case CAEN_DGTZ_DESKTOP_FORM_FACTOR:
        case CAEN_DGTZ_NIM_FORM_FACTOR:
        default:
          dgtzVars.WDcfg.MaxGroupNumber = 2;
          break;
      }
      break;
    default: return -1;
  }
  if ((dgtzVars.BoardInfo.FamilyCode == CAEN_DGTZ_XX751_FAMILY_CODE ||
       dgtzVars.BoardInfo.FamilyCode == CAEN_DGTZ_XX731_FAMILY_CODE) &&
      dgtzVars.WDcfg.DesMode) {
    dgtzVars.WDcfg.Ts /= 2;
  }
  switch (dgtzVars.BoardInfo.FamilyCode) {
    case CAEN_DGTZ_XX724_FAMILY_CODE:
    case CAEN_DGTZ_XX781_FAMILY_CODE:
    case CAEN_DGTZ_XX780_FAMILY_CODE:
    case CAEN_DGTZ_XX720_FAMILY_CODE:
    case CAEN_DGTZ_XX721_FAMILY_CODE:
    case CAEN_DGTZ_XX751_FAMILY_CODE:
    case CAEN_DGTZ_XX761_FAMILY_CODE:
    case CAEN_DGTZ_XX731_FAMILY_CODE:
      switch (dgtzVars.BoardInfo.FormFactor) {
        case CAEN_DGTZ_VME64_FORM_FACTOR:
        case CAEN_DGTZ_VME64X_FORM_FACTOR:
          dgtzVars.WDcfg.Nch = 8;
          break;
        case CAEN_DGTZ_DESKTOP_FORM_FACTOR:
        case CAEN_DGTZ_NIM_FORM_FACTOR:
          dgtzVars.WDcfg.Nch = 4;
          break;
      }
      break;
    case CAEN_DGTZ_XX725_FAMILY_CODE:
    case CAEN_DGTZ_XX730_FAMILY_CODE:
      switch (dgtzVars.BoardInfo.FormFactor) {
        case CAEN_DGTZ_VME64_FORM_FACTOR:
        case CAEN_DGTZ_VME64X_FORM_FACTOR:
          dgtzVars.WDcfg.Nch = 16;
          break;
        case CAEN_DGTZ_DESKTOP_FORM_FACTOR:
        case CAEN_DGTZ_NIM_FORM_FACTOR:
          dgtzVars.WDcfg.Nch = 8;
          break;
      }
      break;
    case CAEN_DGTZ_XX740_FAMILY_CODE:
      switch (dgtzVars.BoardInfo.FormFactor) {
        case CAEN_DGTZ_VME64_FORM_FACTOR:
        case CAEN_DGTZ_VME64X_FORM_FACTOR:
          dgtzVars.WDcfg.Nch = 64;
          break;
        case CAEN_DGTZ_DESKTOP_FORM_FACTOR:
        case CAEN_DGTZ_NIM_FORM_FACTOR:
          dgtzVars.WDcfg.Nch = 32;
          break;
      }
      break;
    case CAEN_DGTZ_XX742_FAMILY_CODE:
      switch (dgtzVars.BoardInfo.FormFactor) {
        case CAEN_DGTZ_VME64_FORM_FACTOR:
        case CAEN_DGTZ_VME64X_FORM_FACTOR:
          dgtzVars.WDcfg.Nch = 36;
          break;
        case CAEN_DGTZ_DESKTOP_FORM_FACTOR:
        case CAEN_DGTZ_NIM_FORM_FACTOR:
          dgtzVars.WDcfg.Nch = 16;
          break;
      }
      break;
    default:
      return -1;
  }
  return 0;
}

int WriteRegisterBitmask(int32_t handle, uint32_t address,
                         uint32_t data, uint32_t mask) {
    int32_t ret = CAEN_DGTZ_Success;
    uint32_t d32 = 0xFFFFFFFF;
    ret = CAEN_DGTZ_ReadRegister(handle, address, &d32);
    if (ret != CAEN_DGTZ_Success) {
      return ret;
    }
    data &= mask;
    d32 &= ~mask;
    d32 |= data;
    ret = CAEN_DGTZ_WriteRegister(handle, address, d32);
    return ret;
}

int programDigitizer() {
  int i, j, ret = 0;
  /**
   *reset the digitizer
   */ 
  ret |= CAEN_DGTZ_Reset(dgtzVars.handle);
  if (ret != 0) {
    printf("Error: Unable to reset digitizer.\nPlease reset "
           "digitizer manually then restart the program\n");
    return -1;
  }
  /**
   * Set the waveform test bit for debugging
   */ 
  if (dgtzVars.WDcfg.TestPattern)
    ret |= CAEN_DGTZ_WriteRegister(
        dgtzVars.handle,
        CAEN_DGTZ_BROAD_CH_CONFIGBIT_SET_ADD, 1 << 3);
  /**
   * custom setting for X742 boards
   */
  if (dgtzVars.BoardInfo.FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE) {
    ret |= CAEN_DGTZ_SetFastTriggerDigitizing(
        dgtzVars.handle, dgtzVars.WDcfg.FastTriggerEnabled);
    ret |= CAEN_DGTZ_SetFastTriggerMode(
        dgtzVars.handle, dgtzVars.WDcfg.FastTriggerMode);
  }
  if (dgtzVars.BoardInfo.FamilyCode == CAEN_DGTZ_XX751_FAMILY_CODE ||
      dgtzVars.BoardInfo.FamilyCode == CAEN_DGTZ_XX731_FAMILY_CODE) {
    ret |= CAEN_DGTZ_SetDESMode(
        dgtzVars.handle, dgtzVars.WDcfg.DesMode);
  }
  ret |= CAEN_DGTZ_SetRecordLength(
      dgtzVars.handle, dgtzVars.WDcfg.RecordLength);
  ret |= CAEN_DGTZ_GetRecordLength(
      dgtzVars.handle, &dgtzVars.WDcfg.RecordLength);
  if (dgtzVars.BoardInfo.FamilyCode == CAEN_DGTZ_XX740_FAMILY_CODE ||
      dgtzVars.BoardInfo.FamilyCode == CAEN_DGTZ_XX724_FAMILY_CODE) {
    ret |= CAEN_DGTZ_SetDecimationFactor(
        dgtzVars.handle, dgtzVars.WDcfg.DecimationFactor);
  }
  ret |= CAEN_DGTZ_SetPostTriggerSize(
      dgtzVars.handle, dgtzVars.WDcfg.PostTrigger);
  if (dgtzVars.BoardInfo.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE) {
    uint32_t pt;
    ret |= CAEN_DGTZ_GetPostTriggerSize(
        dgtzVars.handle, &pt);
    dgtzVars.WDcfg.PostTrigger = pt;
  }
  ret |= CAEN_DGTZ_SetIOLevel(dgtzVars.handle,
                              dgtzVars.WDcfg.FPIOtype);
  if (dgtzVars.WDcfg.InterruptNumEvents > 0) {
    /**
     * Interrupt handling
     */
    ret |= CAEN_DGTZ_SetInterruptConfig(
        dgtzVars.handle, CAEN_DGTZ_ENABLE,
        VME_INTERRUPT_LEVEL, VME_INTERRUPT_STATUS_ID,
        (uint16_t) dgtzVars.WDcfg.InterruptNumEvents,
        INTERRUPT_MODE);
    if (ret != CAEN_DGTZ_Success) {
      printf("\nError configuring interrupts. "
             "Interrupts disabled\n\n");
      dgtzVars.WDcfg.InterruptNumEvents = 0;
    }
  }
  ret |= CAEN_DGTZ_SetMaxNumEventsBLT(
      dgtzVars.handle, dgtzVars.WDcfg.NumEvents);
  ret |= CAEN_DGTZ_SetAcquisitionMode(
      dgtzVars.handle, CAEN_DGTZ_SW_CONTROLLED);
  ret |= CAEN_DGTZ_SetExtTriggerInputMode(
      dgtzVars.handle, dgtzVars.WDcfg.ExtTriggerMode);
  if (dgtzVars.BoardInfo.FamilyCode ==
      CAEN_DGTZ_XX740_FAMILY_CODE ||
        dgtzVars.BoardInfo.FamilyCode ==
      CAEN_DGTZ_XX742_FAMILY_CODE) {
    ret |= CAEN_DGTZ_SetGroupEnableMask(
        dgtzVars.handle, dgtzVars.WDcfg.EnableMask);
    for (i = 0; i < dgtzVars.WDcfg.Nch / 8; ++i) {
      if (dgtzVars.WDcfg.EnableMask & (1 << i)) {
        if (dgtzVars.BoardInfo.FamilyCode ==
            CAEN_DGTZ_XX742_FAMILY_CODE) {
          for (j = 0; j < 8; ++j) {
            if (dgtzVars.WDcfg.DCoffsetGrpCh[i][j] != -1) {
              ret |= CAEN_DGTZ_SetChannelDCOffset(
                  dgtzVars.handle,
                  i * 8 + j,
                  dgtzVars.WDcfg.DCoffsetGrpCh[i][j]);
            } else {
              ret |= CAEN_DGTZ_SetChannelDCOffset(
                  dgtzVars.handle,
                  i * 8 + j,
                  dgtzVars.WDcfg.DCoffset[i]);
            }
          }
        } else {
          ret |= CAEN_DGTZ_SetGroupDCOffset(
              dgtzVars.handle, i, dgtzVars.WDcfg.DCoffset[i]);
          ret |= CAEN_DGTZ_SetGroupSelfTrigger(
              dgtzVars.handle, dgtzVars.WDcfg.ChannelTriggerMode[i],
              1 << i);
          ret |= CAEN_DGTZ_SetGroupTriggerThreshold(
              dgtzVars.handle, i, dgtzVars.WDcfg.Threshold[i]);
          ret |= CAEN_DGTZ_SetChannelGroupMask(
              dgtzVars.handle, i, dgtzVars.WDcfg.GroupTrgEnableMask[i]);
        }
        ret |= CAEN_DGTZ_SetTriggerPolarity(
            dgtzVars.handle, i, dgtzVars.WDcfg.PulsePolarity[i]);
        /**
         * TriggerEdge
         */
      }
    }
  } else {
    ret |= CAEN_DGTZ_SetChannelEnableMask(
        dgtzVars.handle, dgtzVars.WDcfg.EnableMask);
    for (i = 0; i < dgtzVars.WDcfg.Nch; i++) {
      if (dgtzVars.WDcfg.EnableMask & (1 << i)) {
        ret |= CAEN_DGTZ_SetChannelDCOffset(
            dgtzVars.handle, i, dgtzVars.WDcfg.DCoffset[i]);
        if (dgtzVars.BoardInfo.FamilyCode != CAEN_DGTZ_XX730_FAMILY_CODE &&
            dgtzVars.BoardInfo.FamilyCode != CAEN_DGTZ_XX725_FAMILY_CODE)
          ret |= CAEN_DGTZ_SetChannelSelfTrigger(
              dgtzVars.handle,
              dgtzVars.WDcfg.ChannelTriggerMode[i],
              1 << i);
        ret |= CAEN_DGTZ_SetChannelTriggerThreshold(
            dgtzVars.handle, i, dgtzVars.WDcfg.Threshold[i]);
        ret |= CAEN_DGTZ_SetTriggerPolarity(
            dgtzVars.handle, i, dgtzVars.WDcfg.PulsePolarity[i]);
        /**
         * TriggerEdge
         */
      }
    }
    if (dgtzVars.BoardInfo.FamilyCode == CAEN_DGTZ_XX730_FAMILY_CODE ||
        dgtzVars.BoardInfo.FamilyCode == CAEN_DGTZ_XX725_FAMILY_CODE) {
      /**
       * channel pair settings for x730 boards
       */
      for (i = 0; i < dgtzVars.WDcfg.Nch; i += 2) {
        if (dgtzVars.WDcfg.EnableMask & (0x3 << i)) {
          CAEN_DGTZ_TriggerMode_t mode =
              dgtzVars.WDcfg.ChannelTriggerMode[i];
          uint32_t pair_chmask = 0;
          /**
           * Build mode and relevant channelmask. The behaviour is that,
           if the triggermode of one channel of the pair is DISABLED,
           this channel doesn't take part to the trigger generation.
           Otherwise, if both are different from DISABLED, the one of
           the even channel is used.
          */
          if (dgtzVars.WDcfg.ChannelTriggerMode[i] !=
              CAEN_DGTZ_TRGMODE_DISABLED) {
            if (dgtzVars.WDcfg.ChannelTriggerMode[i + 1] ==
                CAEN_DGTZ_TRGMODE_DISABLED) {
              pair_chmask = (0x1 << i);
            } else {
              pair_chmask = (0x3 << i);
            }
          } else {
            mode = dgtzVars.WDcfg.ChannelTriggerMode[i + 1];
            pair_chmask = (0x2 << i);
          }
          pair_chmask &= dgtzVars.WDcfg.EnableMask;
          ret |= CAEN_DGTZ_SetChannelSelfTrigger(
              dgtzVars.handle, mode, pair_chmask);
        }
      }
    }
  }
  if (dgtzVars.BoardInfo.FamilyCode ==
      CAEN_DGTZ_XX742_FAMILY_CODE) {
    for (i = 0; i < dgtzVars.WDcfg.Nch / 8; ++i) {
      ret |= CAEN_DGTZ_SetDRS4SamplingFrequency(
          dgtzVars.handle, dgtzVars.WDcfg.DRS4Frequency);
      ret |= CAEN_DGTZ_SetGroupFastTriggerDCOffset(
          dgtzVars.handle, i, dgtzVars.WDcfg.FTDCoffset[i]);
      ret |= CAEN_DGTZ_SetGroupFastTriggerThreshold(
          dgtzVars.handle, i, dgtzVars.WDcfg.FTThreshold[i]);
    }
  }
  /**
   * execute generic write commands
   */
  for (i = 0; i < dgtzVars.WDcfg.GWn; ++i) {
    ret |= WriteRegisterBitmask(
        dgtzVars.handle, dgtzVars.WDcfg.GWaddr[i],
        dgtzVars.WDcfg.GWdata[i], dgtzVars.WDcfg.GWmask[i]);
  }
  if (ret) {
    printf("Warning: errors found during the "
           "programming of the digitizer.\n"
           "Some settings may not be executed\n");
  }
  return 0;
}


struct RUNFILEvagrs {
  uint32_t maxNumOfEvs;
  uint32_t isMaxFileSize;
  uint32_t runNumber;
  uint32_t eventNumber;
  uint32_t isStarted;
  uint32_t isFirstS;
  char fileNameTemplate[100];
  uint64_t beginRunTime;
  FILE* fout;
} runVars;

void initRunVars() {
  runVars.maxNumOfEvs = 50000;
  runVars.isMaxFileSize = 1;
  runVars.fout = NULL;
  runVars.runNumber = 0;
  runVars.eventNumber = 0;
  runVars.isStarted = 0;
  runVars.isFirstS = 0;
}

void setRunNumber() {
  printf("Please insert the run number: ");
  scanf("%d", &runVars.runNumber);
  printf("%d\n", runVars.runNumber);
  runVars.eventNumber = 0;
}

void setFileNameTemplate(const char* fileNameTemplate) {
  strcpy(runVars.fileNameTemplate,
         fileNameTemplate);
}

void setBeginRunTime() {
  runVars.beginRunTime = get_time();
}

int openRunFile() {
  char fname[100];
  sprintf(
      fname,
      runVars.fileNameTemplate,
      runVars.runNumber);
  if (access(fname, F_OK) != -1) {
    printf("File %s already exists.", fname);
    return 1;
  }
  runVars.fout = fopen(fname, "a");
  if (runVars.fout == NULL) {
    return 1;
  }
  dgtzVars.WDrun.ContinuousWrite = 1;
  return 0;
}

void setNextRunNumber() {
  runVars.runNumber++;
  runVars.eventNumber = 0;
}

void closeRunFile() {
  fclose(runVars.fout);
  runVars.fout = NULL;
}

int appendRunHeader() {
  /**
   * Write a run number
   */
  int output = fwrite(&runVars.runNumber,
                      sizeof(uint32_t),
                      1, runVars.fout);
  if (output != 1) {
    return 1;
  }
  /**
   * Write begin time
   */
  output = fwrite(
      &runVars.beginRunTime,
      sizeof(uint64_t),
      1, runVars.fout);
  if (output != 1) {
    return 1;
  }

  output = fwrite(
        &numberOfDevices,
        sizeof(uint8_t),
        1, runVars.fout);
  if (output != 1) {
	  return 1;
  }
  
  output = fwrite(
		  channelsPerBoard,
		  sizeof(uint8_t),
		  numberOfDevices,
		  runVars.fout);
  if (output != numberOfDevices) {
    return 1;
  }
  return 0;
}

int appendEventHeader() {
	/**
	 * Write a event number
	 */
	int output = fwrite(
			&runVars.eventNumber,
			sizeof(uint32_t),
			1, runVars.fout);
	if (output != 1) {
		return 1;
	}
	/**
	 * Write a boards ids
	 */
	output = fwrite(
			boardIds,
			sizeof(uint8_t),
			numberOfDevices,
			runVars.fout);
	if (output != numberOfDevices) {
		return 1;
	}
	output = fwrite(
			deviceTypes,
			sizeof(uint8_t),
			numberOfDevices,
			runVars.fout);
	if (output != numberOfDevices) {
		return 1;
	}
	return 0;
}

int appendChannelHeader(int absCh) {
  /**
   * Write a channel number
   */
  uint16_t chnum = (uint16_t) absCh;
  int output = fwrite(
      &chnum, sizeof(uint16_t),
      1, runVars.fout);
  if (output != 1) {
    closeRunFile();
    return 1;
  }
  return 0;
}

int appendChannelData(
    int gr, int ch) {
  /**
   * Write a channel data
   */
  int chSize = dgtzVars.Event742->DataGroup[gr].ChSize[ch];
  uint16_t dataArray[chSize];
  int k;
  for (k = 0; k < chSize; ++k) {
    dataArray[k] = (uint16_t) (dgtzVars.Event742->DataGroup[gr].DataChannel[ch])[k];
  }
  int output = fwrite(dataArray,
  		      sizeof(uint16_t), chSize, runVars.fout);
  if (output != chSize) {
    /**
     * error writing to file
     */
    closeRunFile();
    return 1;
  }
  return 0;
}

void goToNextEnabledGroup() {
  if (dgtzVars.WDcfg.EnableMask &&
      dgtzVars.WDcfg.Nch > 8) {
    int orgPlotIndex = dgtzVars.WDrun.GroupPlotIndex;
    do {
      dgtzVars.WDrun.GroupPlotIndex =
          (++dgtzVars.WDrun.GroupPlotIndex) % (dgtzVars.WDcfg.Nch / 8);
    } while (!((1 << dgtzVars.WDrun.GroupPlotIndex) & dgtzVars.WDcfg.EnableMask));
    if(dgtzVars.WDrun.GroupPlotIndex != orgPlotIndex) {
      printf("Plot group set to %d\n", dgtzVars.WDrun.GroupPlotIndex);
    }
  }
  ClearPlot();
}

int32_t boardSupportsCalibration() {
  return
      dgtzVars.BoardInfo.FamilyCode == CAEN_DGTZ_XX761_FAMILY_CODE ||
      dgtzVars.BoardInfo.FamilyCode == CAEN_DGTZ_XX751_FAMILY_CODE ||
      dgtzVars.BoardInfo.FamilyCode == CAEN_DGTZ_XX730_FAMILY_CODE ||
      dgtzVars.BoardInfo.FamilyCode == CAEN_DGTZ_XX725_FAMILY_CODE;
}

int32_t boardSupportsTemperatureRead() {
    return
        dgtzVars.BoardInfo.FamilyCode == CAEN_DGTZ_XX751_FAMILY_CODE ||
        dgtzVars.BoardInfo.FamilyCode == CAEN_DGTZ_XX730_FAMILY_CODE ||
        dgtzVars.BoardInfo.FamilyCode == CAEN_DGTZ_XX725_FAMILY_CODE;
}

void calibrate() {
  printf("\n");
  if (boardSupportsCalibration()) {
    if (dgtzVars.WDrun.AcqRun == 0) {
      int32_t ret = CAEN_DGTZ_Calibrate(dgtzVars.handle);
      if (ret == CAEN_DGTZ_Success) {
        printf("ADC Calibration successfully executed.\n");
      } else {
        printf("ADC Calibration failed. CAENDigitizer ERR %d\n", ret);
      }
      printf("\n");
    } else {
      printf("Can't run ADC calibration while acquisition is running.\n");
    }
  } else {
    printf("ADC Calibration not needed for this board family.\n");
  }
}

void Calibrate_XX740_DC_Offset() {
  float cal = 1;
  float offset = 0;
  int i = 0, acq = 0, k = 0, p = 0;
  CAEN_DGTZ_ErrorCode ret;
  CAEN_DGTZ_AcqMode_t mem_mode;
  uint32_t  AllocatedSize;
  ERROR_CODES ErrCode = ERR_NONE;
  uint32_t BufferSize;
  CAEN_DGTZ_EventInfo_t EventInfo;
  char *buffer = NULL;
  char *EventPtr = NULL;
  CAEN_DGTZ_UINT16_EVENT_t *Event16 = NULL;
  /**
   * chosen value stored
   */
  ret = CAEN_DGTZ_GetAcquisitionMode(
      dgtzVars.handle, &mem_mode);
  if (ret) {
    printf("Error trying to read acq mode!!\n");
  }
  ret = CAEN_DGTZ_SetAcquisitionMode(
      dgtzVars.handle, CAEN_DGTZ_SW_CONTROLLED);
  if (ret) {
    printf("Error trying to set acq mode!!\n");
  }
  ret = CAEN_DGTZ_SetExtTriggerInputMode(
      dgtzVars.handle, CAEN_DGTZ_TRGMODE_DISABLED);
  if (ret) {
    printf("Error trying to set ext trigger!!\n");
  }
  ret = CAEN_DGTZ_SetPostTriggerSize(
      dgtzVars.handle, 0);
  if (ret) {
    printf("Error trying to set post trigger!!\n");
  }
  /**
   * malloc
   */
  ret = CAEN_DGTZ_MallocReadoutBuffer(
      dgtzVars.handle, &buffer, &AllocatedSize);
  if (ret) {
    ErrCode = ERR_MALLOC;
    printError(&ErrCode);
    exit(0);
  }
  ret = CAEN_DGTZ_AllocateEvent(
      dgtzVars.handle,
      (void**) &Event16);
  if (ret != CAEN_DGTZ_Success) {
    ErrCode = ERR_MALLOC;
    printError(&ErrCode);
    exit(0);
  }
  /**
   * test values (%)
   */
  uint32_t dc[NPOINTS] = {25, 75};
  for (i = 0; i < dgtzVars.WDcfg.Nch / 8; ++i) {
    float avg_value[NPOINTS] = {0};
    if (dgtzVars.WDcfg.EnableMask & (1 << i) &&
        dgtzVars.WDcfg.Version_used[i] == 1) {	
      printf("Group %d DAC calibration...\n", i);
      ret = CAEN_DGTZ_SetGroupSelfTrigger(
          dgtzVars.handle, CAEN_DGTZ_TRGMODE_DISABLED, 1 << i);
      if (ret) {
        printf("Warning: error disabling group %d self trigger\n", i);
      }
      ret = CAEN_DGTZ_SWStartAcquisition(dgtzVars.handle);
      if (ret) {
        printf("Warning: error starting X740 acq\n");
        printError(&ErrCode);
        exit(0);
      }
      cal_ok[i] = Calibration_is_possible(i);
      if (cal_ok[i]) {
        for (p = 0; p < NPOINTS; p++) {
          ret = CAEN_DGTZ_SetGroupDCOffset(
              dgtzVars.handle,
              (uint32_t) i,
              (uint32_t) ((float)(abs(dc[p] - 100)) * 655.35));
          if (ret) {
            printf("Warning: error setting group %d test offset\n", i);
          }
#ifdef _WIN32
          Sleep(200);
#else
          usleep(200000);
#endif
          ret = CAEN_DGTZ_SWStartAcquisition(dgtzVars.handle);
          if (ret) {
            printf("Warning: error starting X740 acq\n");
            printError(&ErrCode);
            exit(0);
          }
          int value[NACQS] = {0};
          for (acq = 0; acq < NACQS; acq++) {
            CAEN_DGTZ_SendSWtrigger(dgtzVars.handle);
            ret = CAEN_DGTZ_ReadData(
                dgtzVars.handle,
                CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT,
                buffer, &BufferSize);
            if (ret) {
              ErrCode = ERR_READOUT;
              printError(&ErrCode);
              exit(0);
            }
            ret = CAEN_DGTZ_GetEventInfo(
                dgtzVars.handle, buffer,
                BufferSize, 0, &EventInfo, &EventPtr);
            if (ret) {
              ErrCode = ERR_EVENT_BUILD;
              printError(&ErrCode);
              exit(0);
            }
            /**
             * decode the event
             */
            ret = CAEN_DGTZ_DecodeEvent(
                dgtzVars.handle,
                EventPtr,
                (void**) &Event16);
            if (ret) {
              ErrCode = ERR_EVENT_BUILD;
              printError(&ErrCode);
              exit(0);
            }
            for (k = 1; k < 7; ++k) {
              /**
               * mean over 6 samples
               */
              value[acq] += (int) Event16->DataChannel[i * 8][k];
            }
            value[acq] = value[acq] / 6;
          }  // for acq
          /**
           * check for clean baselines
           */
          int max = 0;
          int mpp = 0;
          int size = (int) pow(2, (double) dgtzVars.BoardInfo.ADC_NBits);
          int *freq = calloc(size, sizeof(int));
          for (k = 0; k < NACQS; ++k) {
            if (value[k] > 0 && value[k] < size) {
              freq[value[k]]++;
              if (freq[value[k]] > max) {
                max = freq[value[k]];
                mpp = value[k];
              }
            }
          }
          free(freq);
          int ok = 0;
          for (k = 0; k < NACQS; k++) {
            if (value[k] == mpp ||
                value[k] == (mpp + 1) ||
                value[k] == (mpp - 1)) {
              avg_value[p] = avg_value[p] + (float) value[k];
              ok++;
            }
          }
          avg_value[p] = (avg_value[p] / (float) ok) * 100. / (float) size;
        }  // for p
        cal = ((float) (avg_value[1] - avg_value[0]) / (float) (dc[1] - dc[0]));
        offset = (float) (dc[1] * avg_value[0] - dc[0] * avg_value[1]) / (float) (dc[1] - dc[0]);
      }  // close if cal ok
      if (dgtzVars.WDcfg.PulsePolarity[i] == CAEN_DGTZ_PulsePolarityPositive) {
        dgtzVars.WDcfg.DCoffset[i] =
            (uint32_t) ((float) (fabs(((float) dc_file[i] - offset) / cal - 100.)) * 655.35);
        if (dgtzVars.WDcfg.DCoffset[i] > 65535) {
          dgtzVars.WDcfg.DCoffset[i] = 65535;
        }
        if (dgtzVars.WDcfg.DCoffset[i] < 0) {
          dgtzVars.WDcfg.DCoffset[i] = 0;
        }
      } else {
        if (dgtzVars.WDcfg.PulsePolarity[i] ==
            CAEN_DGTZ_PulsePolarityNegative) {
          dgtzVars.WDcfg.DCoffset[i] =
              (uint32_t)((float)(fabs(((fabs(dc_file[i] - 100.) - offset) / cal) - 100.)) * (655.35));
        }
        if (dgtzVars.WDcfg.DCoffset[i] < 0) {
          dgtzVars.WDcfg.DCoffset[i] = 0;
        }
        if (dgtzVars.WDcfg.DCoffset[i] > 65535) {
          dgtzVars.WDcfg.DCoffset[i] = 65535;
        }  
      }
      ret = CAEN_DGTZ_SetGroupDCOffset(
          dgtzVars.handle, (uint32_t) i, dgtzVars.WDcfg.DCoffset[i]);
      if (ret) {
        printf("Warning: error setting group %d calibrated offset\n", i);
      }
#ifdef _WIN32
      Sleep(200);
#else
      usleep(200000);
#endif
    }
  }
  /**
   * printf("DAC Calibration ready\n");
   */
  CAEN_DGTZ_SWStopAcquisition(dgtzVars.handle);
  /**
   * free events e buffer
   */
  CAEN_DGTZ_FreeReadoutBuffer(&buffer);
  CAEN_DGTZ_FreeEvent(dgtzVars.handle,
                      (void**) &Event16);
  Set_correct_XX740_Threshold();
  ret |= CAEN_DGTZ_SetDecimationFactor(
      dgtzVars.handle, dgtzVars.WDcfg.DecimationFactor);
  ret |= CAEN_DGTZ_SetPostTriggerSize(
      dgtzVars.handle, dgtzVars.WDcfg.PostTrigger);
  ret |= CAEN_DGTZ_SetPostTriggerSize(
      dgtzVars.handle, dgtzVars.WDcfg.PostTrigger);
  ret |= CAEN_DGTZ_SetAcquisitionMode(dgtzVars.handle, mem_mode);
  ret |= CAEN_DGTZ_SetExtTriggerInputMode(
      dgtzVars.handle, dgtzVars.WDcfg.ExtTriggerMode);
  if (ret) {
    printf("Warning: error setting recorded parameters\n");
  }
  printError(&ErrCode);
  exit(0);
}

void Set_correct_XX740_Threshold() {
  int i = 0, k = 0;
  CAEN_DGTZ_ErrorCode ret;
  uint32_t  AllocatedSize;
  ERROR_CODES ErrCode = ERR_NONE;
  uint32_t BufferSize;
  CAEN_DGTZ_EventInfo_t EventInfo;
  char *buffer = NULL;
  char *EventPtr = NULL;
  CAEN_DGTZ_UINT16_EVENT_t *Event16 = NULL;
  /**
   * malloc
   */
  ret = CAEN_DGTZ_MallocReadoutBuffer(
      dgtzVars.handle, &buffer, &AllocatedSize);
  if (ret) {
    ErrCode = ERR_MALLOC;
    printError(&ErrCode);
    exit(0);
  }
  ret = CAEN_DGTZ_AllocateEvent(dgtzVars.handle, (void**)&Event16);	
  if (ret != CAEN_DGTZ_Success) {
    ErrCode = ERR_MALLOC;
    printError(&ErrCode);
    exit(0);
  }
  CAEN_DGTZ_SWStartAcquisition(dgtzVars.handle);
  for (i = 0; i < dgtzVars.WDcfg.Nch / 8; ++i) {
    if (dgtzVars.WDcfg.EnableMask & (1 << i) && dgtzVars.WDcfg.Version_used[i] == 1) {
      if (cal_ok[i]) {
        int baseline = 0;
        CAEN_DGTZ_SendSWtrigger(dgtzVars.handle);
        ret = CAEN_DGTZ_ReadData(
            dgtzVars.handle,
            CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT,
            buffer,
            &BufferSize);
        if (ret) {
          ErrCode = ERR_READOUT;
          printError(&ErrCode);
          exit(0);
        }
        ret = CAEN_DGTZ_GetEventInfo(
            dgtzVars.handle, buffer, BufferSize, 0, &EventInfo, &EventPtr);
        if (ret) {
          ErrCode = ERR_EVENT_BUILD;
          printError(&ErrCode);
          exit(0);
        }
        ret = CAEN_DGTZ_DecodeEvent(
            dgtzVars.handle,
            EventPtr,
            (void**) &Event16);
        if (ret) {
          ErrCode = ERR_EVENT_BUILD;
          printError(&ErrCode);
          exit(0);
        }
        /**
         * mean over 10 samples
         */
        for (k = 1; k < 11; ++k) {
          baseline += (int) Event16->DataChannel[i*8][k];
        }
        baseline = (baseline / 10);
        if (dgtzVars.WDcfg.PulsePolarity[i] ==
            CAEN_DGTZ_PulsePolarityPositive) {
          dgtzVars.WDcfg.Threshold[i] = (uint32_t)baseline + thr_file[i];
        } else  {
          if (dgtzVars.WDcfg.PulsePolarity[i] ==
              CAEN_DGTZ_PulsePolarityNegative) {
            dgtzVars.WDcfg.Threshold[i] = (uint32_t)baseline - thr_file[i];
          }
        }
        if (dgtzVars.WDcfg.Threshold[i] < 0) {
          dgtzVars.WDcfg.Threshold[i] = 0;
        }
        int size = (int)pow(2, (double)dgtzVars.BoardInfo.ADC_NBits);
        if (dgtzVars.WDcfg.Threshold[i] > (uint32_t)size) dgtzVars.WDcfg.Threshold[i] = size;
        // if cal ok i
      } else {
        dgtzVars.WDcfg.Threshold[i] = thr_file[i];
      }
      ret = CAEN_DGTZ_SetGroupTriggerThreshold(
          dgtzVars.handle, i, dgtzVars.WDcfg.Threshold[i]);
      if (ret) {
        printf("Warning: error setting ch %d corrected threshold\n", i);
      }
      ret |= CAEN_DGTZ_SetGroupSelfTrigger(
          dgtzVars.handle,
          dgtzVars.WDcfg.ChannelTriggerMode[i],
          1 << i);
      ret |= CAEN_DGTZ_SetTriggerPolarity(
          dgtzVars.handle, i,
          dgtzVars.WDcfg.PulsePolarity[i]);  // TriggerEdge
      ret |= CAEN_DGTZ_SetChannelGroupMask(
          dgtzVars.handle, i,
          dgtzVars.WDcfg.GroupTrgEnableMask[i]);
    }
  }
  CAEN_DGTZ_SWStopAcquisition(dgtzVars.handle);
  CAEN_DGTZ_FreeReadoutBuffer(&buffer);
  CAEN_DGTZ_FreeEvent(dgtzVars.handle,
                      (void**) &Event16);
        printError(&ErrCode);
        exit(0);
}

int Calibration_is_possible(int ch) {
  int res;
  int i = 0;
  CAEN_DGTZ_ErrorCode ret;
  uint32_t  AllocatedSize;
  ERROR_CODES ErrCode = ERR_NONE;
  uint32_t BufferSize;
  CAEN_DGTZ_EventInfo_t EventInfo;
  char *buffer = NULL;
  char *EventPtr = NULL;
  CAEN_DGTZ_UINT16_EVENT_t *Event16 = NULL;
  CAEN_DGTZ_UINT8_EVENT_t *Event8 = NULL;
  /**
   * malloc
   */
  ret = CAEN_DGTZ_MallocReadoutBuffer(
      dgtzVars.handle, &buffer, &AllocatedSize);
  if (ret) {
    ErrCode = ERR_MALLOC;
    printError(&ErrCode);
    exit(0);
  }
  if (dgtzVars.WDcfg.Nbit == 8) {
    ret = CAEN_DGTZ_AllocateEvent(dgtzVars.handle,
                                  (void**)&Event8);
  } else {
    ret = CAEN_DGTZ_AllocateEvent(dgtzVars.handle,
                                  (void**)&Event16);
  }
  if (ret != CAEN_DGTZ_Success) {
    ErrCode = ERR_MALLOC;
    printError(&ErrCode);
    exit(0);
  }
  int freq_max = 0;
  int freq_min = 0;
  int freq_bl = 0;
  int real_bl;
  int max = 0, min = 10000000;
  int pmax = 0,pmin=0;
  /**
   * test value
   */
  uint32_t dc_test = 32767;
  int size = (int) pow(
      2, (double) dgtzVars.BoardInfo.ADC_NBits);
  float expected_bl = 0.5 * (float) size;
  if(dgtzVars.BoardInfo.FamilyCode == CAEN_DGTZ_XX740_FAMILY_CODE) {
    ret = CAEN_DGTZ_SetGroupDCOffset(
        dgtzVars.handle, (uint32_t)ch, (uint32_t)(dc_test));
  } else {
    ret = CAEN_DGTZ_SetChannelDCOffset(
        dgtzVars.handle, (uint32_t)ch, (uint32_t)(dc_test));
  }
  if (ret) {
    printf("Warning: error setting ch %d test offset\n", ch);
  }
#ifdef _WIN32
  Sleep(500);
#else
  usleep(500000);
#endif

  ret = CAEN_DGTZ_SWStartAcquisition(dgtzVars.handle);
  if (ret) {
    printf("Warning: error starting acq\n");
    printError(&ErrCode);
    exit(0);
  }
  CAEN_DGTZ_SendSWtrigger(dgtzVars.handle);
  ret = CAEN_DGTZ_ReadData(
      dgtzVars.handle,
      CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT,
      buffer, &BufferSize);
  if (ret) {
    ErrCode = ERR_READOUT;
    printError(&ErrCode);
    exit(0);
  }
  ret = CAEN_DGTZ_GetEventInfo(dgtzVars.handle, buffer,
                               BufferSize, 0, &EventInfo,
                               &EventPtr);
  if (ret) {
    ErrCode = ERR_EVENT_BUILD;
    printError(&ErrCode);
    exit(0);
  }
  /**
   * decode the event
   */
  if (dgtzVars.WDcfg.Nbit == 8) {
    ret = CAEN_DGTZ_DecodeEvent(
        dgtzVars.handle, EventPtr,
        (void**)&Event8);
  } else {
    ret = CAEN_DGTZ_DecodeEvent(
        dgtzVars.handle,
        EventPtr,
        (void**) &Event16);
  }
  if (ret) {
    ErrCode = ERR_EVENT_BUILD;
    printError(&ErrCode);
    exit(0);
  }
  int *value = calloc(
      dgtzVars.WDcfg.RecordLength,
      sizeof(int));
  /**
   * scan all samples, look for max and min
   */
  for (i = 2; i < dgtzVars.WDcfg.RecordLength - 2; ++i) {
    if (dgtzVars.WDcfg.Nbit == 8) {
      value[i] = (int) Event8->DataChannel[ch][i];
    } else {
      if (dgtzVars.BoardInfo.FamilyCode == CAEN_DGTZ_XX740_FAMILY_CODE) {
        value[i] = (int) Event16->DataChannel[ch * 8][i];
      } else {
        value[i] = (int) Event16->DataChannel[ch][i];
      }
      if (value[i] > max &&
          value[i] < size - 10) {
        max = value[i];
        pmax = i;
      }
      if (value[i] < min &&
          value[i]>10) {
        min = value[i];
        pmin = i;
      }
    }
  } // close for i
  /**
   * scan all samples
   */
  for (i = 2; i < dgtzVars.WDcfg.RecordLength-2; ++i) {
    if (value[i] > min - 10 &&
        value[i] < min + 10) {
      freq_min++;
    }
    if (value[i]> max - 10 &&
        value[i] < max + 10) {
      freq_max++;
    }
  }
  free(value);
  if (dgtzVars.WDcfg.PulsePolarity[ch] ==
      CAEN_DGTZ_PulsePolarityPositive) {
    freq_bl = freq_min; real_bl = min;
  } else {
    if (dgtzVars.WDcfg.PulsePolarity[ch] ==
        CAEN_DGTZ_PulsePolarityNegative) {
      freq_bl = freq_max;
      real_bl = max;
    }
  }
  if (freq_bl < dgtzVars.WDcfg.RecordLength * 0.6 ||
      fabs((float) real_bl -
           (float) expected_bl) /
      expected_bl > 0.25) {
    if (dgtzVars.BoardInfo.FamilyCode == CAEN_DGTZ_XX740_FAMILY_CODE) {
      printf("DAC calibration failed for group %d.\n "
             "The group trigger threshold will be absolute.\n "
             "Disconnect the input signal from channel %d and "
             "restart acquisition to calibrate.\n", ch, ch * 8);
    } else {
      printf("DAC calibration failed for channel %d.\n"
             "The channel trigger threshold will be absolute.\n"
             "Disconnect the input signal and restart acquisition "
             "to calibrate.\n", ch);
    }
    res = 0;
#ifdef _WIN32
    Sleep(500);
#else
    usleep(500000);
#endif
  } else {
    res = 1;
  }
  CAEN_DGTZ_SWStopAcquisition(dgtzVars.handle);
  /**
   * free events e buffer
   */
  CAEN_DGTZ_FreeReadoutBuffer(&buffer);
  if (dgtzVars.WDcfg.Nbit == 8) {
    CAEN_DGTZ_FreeEvent(dgtzVars.handle,
                        (void**) &Event8);
  } else {
    CAEN_DGTZ_FreeEvent(dgtzVars.handle,
                        (void**) &Event16);
  }
  return res;
}
  
void Calibrate_DC_Offset() {
  float cal = 1;
  float offset = 0;
  int i = 0, k = 0, p = 0, acq = 0, ch = 0;
  CAEN_DGTZ_ErrorCode ret;
  CAEN_DGTZ_AcqMode_t mem_mode;
  uint32_t  AllocatedSize;
  ERROR_CODES ErrCode = ERR_NONE;
  uint32_t BufferSize;
  CAEN_DGTZ_EventInfo_t EventInfo;
  char *buffer = NULL;
  char *EventPtr = NULL;
  CAEN_DGTZ_UINT16_EVENT_t *Event16 = NULL;
  CAEN_DGTZ_UINT8_EVENT_t *Event8 = NULL;
  
  ret = CAEN_DGTZ_GetAcquisitionMode(
      dgtzVars.handle, &mem_mode);  // chosen value stored
  if (ret) {
    printf("Error trying to read acq mode!!\n");
  }
  ret = CAEN_DGTZ_SetAcquisitionMode(dgtzVars.handle, CAEN_DGTZ_SW_CONTROLLED);
  if (ret) {
    printf("Error trying to set acq mode!!\n");
  }
  ret = CAEN_DGTZ_SetExtTriggerInputMode(
      dgtzVars.handle, CAEN_DGTZ_TRGMODE_DISABLED);
  if (ret) {
    printf("Error trying to set ext trigger!!\n");
  }
  ret = CAEN_DGTZ_SetPostTriggerSize(dgtzVars.handle, 0);
  if (ret) {
    printf("Error trying to set post trigger!!\n");
  }
  if (dgtzVars.BoardInfo.FamilyCode ==
      CAEN_DGTZ_XX740_FAMILY_CODE ||
      dgtzVars.BoardInfo.FamilyCode ==
      CAEN_DGTZ_XX724_FAMILY_CODE) {
    ret = CAEN_DGTZ_SetDecimationFactor(
        dgtzVars.handle, 1);
    if (ret) {
      printf("Error trying to set decimation factor!!\n");
    }
  }
  /**
   * malloc
   */
  ret = CAEN_DGTZ_MallocReadoutBuffer(
      dgtzVars.handle, &buffer, &AllocatedSize);
  if (ret) {
    ErrCode = ERR_MALLOC;
    printError(&ErrCode);
    exit(0);
  }
  if (dgtzVars.WDcfg.Nbit == 8) {
    ret = CAEN_DGTZ_AllocateEvent(
        dgtzVars.handle, (void**) &Event8);
  } else {
    ret = CAEN_DGTZ_AllocateEvent(
        dgtzVars.handle, (void**) &Event16);		
  }
  if (ret != CAEN_DGTZ_Success) {
    ErrCode = ERR_MALLOC;
    printError(&ErrCode);
    exit(0);
  }
  float avg_value[NPOINTS] = {0};
  /**
   * test values (%)
   */
  uint32_t dc[NPOINTS] = {25, 75};
  for (ch = 0; ch < (int32_t) dgtzVars.BoardInfo.Channels; ++ch) {
    if (dgtzVars.WDcfg.EnableMask &
        (1 << ch) &&
        dgtzVars.WDcfg.Version_used[ch] ==1) {
      printf("Starting channel %d DAC calibration...\n", ch);
      ret = CAEN_DGTZ_SetChannelSelfTrigger(
          dgtzVars.handle, CAEN_DGTZ_TRGMODE_DISABLED,
          1 << ch);
      if (ret) {
        printf("Warning: error disabling ch %d self trigger\n", ch);
      }
      cal_ok[ch] = Calibration_is_possible(ch);
      if (cal_ok[ch]) {
        for (p = 0; p < NPOINTS; p++) {
          ret = CAEN_DGTZ_SetChannelDCOffset(
              dgtzVars.handle, (uint32_t) ch, (uint32_t)((float)(abs(dc[p] - 100))*(655.35)));
          if (ret)
            printf("Warning: error setting ch %d test offset\n", ch);
#ifdef _WIN32
          Sleep(200);
#else
          usleep(200000);
#endif
          
          ret = CAEN_DGTZ_SWStartAcquisition(dgtzVars.handle);
          if (ret)
          {
            printf("Warning: error starting acq\n");
            printError(&ErrCode);
            exit(0);
          }
          
          CAEN_DGTZ_ClearData(dgtzVars.handle);
          int value[NACQS] = { 0 };
          for (acq = 0; acq < NACQS; acq++)
          {
            CAEN_DGTZ_SendSWtrigger(dgtzVars.handle);
            
            ret = CAEN_DGTZ_ReadData(dgtzVars.handle, CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT, buffer, &BufferSize);
            if (ret) {
              ErrCode = ERR_READOUT;
              printError(&ErrCode);
              exit(0);
            }
            
            ret = CAEN_DGTZ_GetEventInfo(dgtzVars.handle, buffer, BufferSize, 0, &EventInfo, &EventPtr);
            if (ret) {
              ErrCode = ERR_EVENT_BUILD;
              printError(&ErrCode);
              exit(0);
            }
            // decode the event //
            if (dgtzVars.WDcfg.Nbit == 8)
              ret = CAEN_DGTZ_DecodeEvent(dgtzVars.handle, EventPtr, (void**)&Event8);
            else
              ret = CAEN_DGTZ_DecodeEvent(dgtzVars.handle, EventPtr, (void**)&Event16);
            
            if (ret) {
              ErrCode = ERR_EVENT_BUILD;
              printError(&ErrCode);
              exit(0);
            }
            
            
            for (i = 1; i < 7; i++) //mean over 6 samples
            {
              if (dgtzVars.WDcfg.Nbit == 8)
                value[acq] += (int)(Event8->DataChannel[ch][i]);
              else
                value[acq] += (int)(Event16->DataChannel[ch][i]);
            }
            value[acq] = (value[acq] / 6);
            
          }//for acq
          
          ///check for clean baselines
          int max = 0;
          int mpp = 0;
          int size = (int)pow(2, (double)dgtzVars.BoardInfo.ADC_NBits);
          int *freq = calloc(size, sizeof(int));
          
          for (k = 0; k < NACQS; k++)
          {
            if (value[k] > 0 && value[k] < size)
            {
              freq[value[k]]++;
              if (freq[value[k]] > max) { max = freq[value[k]]; mpp = value[k]; }
            }
          }
          
          free(freq);
          int ok = 0;
          for (k = 0; k < NACQS; k++)
          {
            if (value[k] == mpp || value[k] == (mpp + 1) || value[k] == (mpp - 1))
            {
              avg_value[p] = avg_value[p] + (float)value[k]; ok++;
            }
          }
          avg_value[p] = (avg_value[p] / (float)ok)*100. / (float)size;
          
          CAEN_DGTZ_SWStopAcquisition(dgtzVars.handle);
        }//close for p
        cal = ((float)(avg_value[1] - avg_value[0]) / (float)(dc[1] - dc[0]));
        offset = (float)(dc[1] * avg_value[0] - dc[0] * avg_value[1]) / (float)(dc[1] - dc[0]);
        //printf("Cal %f   offset %f\n", cal, offset);
        
        
      }///close if calibration is possible
      
      
      if (dgtzVars.WDcfg.PulsePolarity[ch] == CAEN_DGTZ_PulsePolarityPositive)
      {
        dgtzVars.WDcfg.DCoffset[ch] = (uint32_t)((float)(fabs(( ((float)dc_file[ch] - offset )/ cal ) - 100.))*(655.35));
        if (dgtzVars.WDcfg.DCoffset[ch] > 65535) dgtzVars.WDcfg.DCoffset[ch] = 65535;
        if (dgtzVars.WDcfg.DCoffset[ch] < 0) dgtzVars.WDcfg.DCoffset[ch] = 0;
      }
      else
        if (dgtzVars.WDcfg.PulsePolarity[ch] == CAEN_DGTZ_PulsePolarityNegative)
        {
          dgtzVars.WDcfg.DCoffset[ch] = (uint32_t)((float)(fabs(( (fabs(dc_file[ch] - 100.) - offset) / cal ) - 100.))*(655.35));
          if (dgtzVars.WDcfg.DCoffset[ch] < 0) dgtzVars.WDcfg.DCoffset[ch] = 0;
          if (dgtzVars.WDcfg.DCoffset[ch] > 65535) dgtzVars.WDcfg.DCoffset[ch] = 65535;
          
        }
      
      ret = CAEN_DGTZ_SetChannelDCOffset(dgtzVars.handle, (uint32_t)ch, dgtzVars.WDcfg.DCoffset[ch]);
      if (ret)
        printf("Warning: error setting ch %d offset\n", ch);
#ifdef _WIN32
      Sleep(200);
#else
      usleep(200000);
#endif
    }//if ch enabled
    
  }//loop ch
  
  //printf("DAC Calibration ready\n");
  
  //CAEN_DGTZ_SWStopAcquisition(dgtzVars.handle);  
  
  ///free events e buffer
  CAEN_DGTZ_FreeReadoutBuffer(&buffer);
  if (dgtzVars.WDcfg.Nbit == 8) {
    CAEN_DGTZ_FreeEvent(dgtzVars.handle, (void**)&Event8);
  } else {
    CAEN_DGTZ_FreeEvent(dgtzVars.handle, (void**)&Event16);
  }
  Set_correct_Threshold();  
  if (dgtzVars.BoardInfo.FamilyCode == CAEN_DGTZ_XX724_FAMILY_CODE) {
    ret |= CAEN_DGTZ_SetDecimationFactor(dgtzVars.handle, dgtzVars.WDcfg.DecimationFactor);
  }
  ret |= CAEN_DGTZ_SetPostTriggerSize(dgtzVars.handle, dgtzVars.WDcfg.PostTrigger);
  ret |= CAEN_DGTZ_SetAcquisitionMode(dgtzVars.handle, mem_mode);
  ret |= CAEN_DGTZ_SetExtTriggerInputMode(dgtzVars.handle, dgtzVars.WDcfg.ExtTriggerMode);
  if (dgtzVars.BoardInfo.FamilyCode == CAEN_DGTZ_XX740_FAMILY_CODE || dgtzVars.BoardInfo.FamilyCode == CAEN_DGTZ_XX724_FAMILY_CODE) 
    ret |= CAEN_DGTZ_SetDecimationFactor(dgtzVars.handle, dgtzVars.WDcfg.DecimationFactor);
  
  if (ret)
    printf("Warning: error setting some parameters\n");
  
  printError(&ErrCode);
  exit(0);
}

void Set_correct_Threshold() {
  int i = 0,ch=0;
  CAEN_DGTZ_ErrorCode ret;
  uint32_t  AllocatedSize;
  ERROR_CODES ErrCode = ERR_NONE;
  uint32_t BufferSize;
  CAEN_DGTZ_EventInfo_t EventInfo;
  char *buffer = NULL;
  char *EventPtr = NULL;
  CAEN_DGTZ_UINT16_EVENT_t *Event16 = NULL;
  CAEN_DGTZ_UINT8_EVENT_t *Event8 = NULL;
  /**
   * malloc
   */
  ret = CAEN_DGTZ_MallocReadoutBuffer(
      dgtzVars.handle, &buffer, &AllocatedSize);
  if (ret) {
    ErrCode = ERR_MALLOC;
    printError(&ErrCode);
    exit(0);
  }

  if (dgtzVars.WDcfg.Nbit == 8) {
    ret = CAEN_DGTZ_AllocateEvent(
        dgtzVars.handle, (void**) &Event8);
  } else {
    ret = CAEN_DGTZ_AllocateEvent(
        dgtzVars.handle, (void**) &Event16);	
  }
  if (ret != CAEN_DGTZ_Success) {
    ErrCode = ERR_MALLOC;
    printError(&ErrCode);
    exit(0);
  }
  uint32_t mask;
  CAEN_DGTZ_GetChannelEnableMask(
      dgtzVars.handle, &mask);
  CAEN_DGTZ_SWStartAcquisition(dgtzVars.handle);
  for (ch = 0; ch < (int32_t)dgtzVars.BoardInfo.Channels; ++ch) {
    if (dgtzVars.WDcfg.EnableMask & (1 << ch) &&
        dgtzVars.WDcfg.Version_used[ch] == 1) {
      if (cal_ok[ch]) {
        int baseline = 0;
        CAEN_DGTZ_SendSWtrigger(dgtzVars.handle);
        ret = CAEN_DGTZ_ReadData(
            dgtzVars.handle,
            CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT,
            buffer, &BufferSize);
        if (ret) {
          ErrCode = ERR_READOUT;
          printError(&ErrCode);
          exit(0);
        }
        ret = CAEN_DGTZ_GetEventInfo(
            dgtzVars.handle, buffer,
            BufferSize, 0, &EventInfo, &EventPtr);
        if (ret) {
          ErrCode = ERR_EVENT_BUILD;
          printError(&ErrCode);
          exit(0);
        }
        /**
         * decode the event
         */
        if (dgtzVars.WDcfg.Nbit == 8) {
          ret = CAEN_DGTZ_DecodeEvent(
              dgtzVars.handle,
              EventPtr,
              (void**) &Event8);
        } else {
          ret = CAEN_DGTZ_DecodeEvent(
              dgtzVars.handle,
              EventPtr,
              (void**) &Event16);
        }
        if (ret) {
          ErrCode = ERR_EVENT_BUILD;
          printError(&ErrCode);
          exit(0);
        }
        /**
         * mean over 10 samples
         */
        for (i = 1; i < 11; i++) {
          if (dgtzVars.WDcfg.Nbit == 8) {
            baseline += (int) Event8->DataChannel[ch][i];
          } else {
            baseline += (int) Event16->DataChannel[ch][i];
          }
        }
        baseline = baseline / 10;
        if (dgtzVars.WDcfg.PulsePolarity[ch] ==
            CAEN_DGTZ_PulsePolarityPositive) {
          dgtzVars.WDcfg.Threshold[ch] = (uint32_t)baseline + thr_file[ch];
        } else {
          if (dgtzVars.WDcfg.PulsePolarity[ch] ==
              CAEN_DGTZ_PulsePolarityNegative) {
            dgtzVars.WDcfg.Threshold[ch] = (uint32_t) baseline - thr_file[ch];
          }
        }
        if (dgtzVars.WDcfg.Threshold[ch] < 0) {
          dgtzVars.WDcfg.Threshold[ch] = 0;
        }
        int size = (int) pow(2, (double) dgtzVars.BoardInfo.ADC_NBits);
        if (dgtzVars.WDcfg.Threshold[ch] > (uint32_t)size) {
          dgtzVars.WDcfg.Threshold[ch] = size;
        }
      } else {
        dgtzVars.WDcfg.Threshold[ch] = thr_file[ch];
      }
      ret = CAEN_DGTZ_SetChannelTriggerThreshold(
          dgtzVars.handle, ch, dgtzVars.WDcfg.Threshold[ch]);
      if (ret) {
        printf("Warning: error setting ch %d corrected threshold\n", ch);
      }
      ret |= CAEN_DGTZ_SetChannelSelfTrigger(
          dgtzVars.handle, dgtzVars.WDcfg.ChannelTriggerMode[ch], 1 << ch);
    }
  }
  CAEN_DGTZ_SWStopAcquisition(dgtzVars.handle);
  CAEN_DGTZ_FreeReadoutBuffer(&buffer);
  if (dgtzVars.WDcfg.Nbit == 8) {
    CAEN_DGTZ_FreeEvent(dgtzVars.handle, (void**) &Event8);
  } else {
    CAEN_DGTZ_FreeEvent(dgtzVars.handle, (void**) &Event16);
  }
  printError(&ErrCode);
  exit(0);
}

void startDataAcquisition() {
  // Avoid calibration for X731 (it is done automatically when
  // switching DES mode enablement.
  if (dgtzVars.BoardInfo.FamilyCode == CAEN_DGTZ_XX740_FAMILY_CODE) {
    // XX740 specific
    Calibrate_XX740_DC_Offset();
  } else if (dgtzVars.BoardInfo.FamilyCode !=
             CAEN_DGTZ_XX742_FAMILY_CODE) {
    // XX742 not considered
    Calibrate_DC_Offset();
  }
  if (dgtzVars.BoardInfo.FamilyCode == CAEN_DGTZ_XX730_FAMILY_CODE ||
      dgtzVars.BoardInfo.FamilyCode == CAEN_DGTZ_XX725_FAMILY_CODE) {
    dgtzVars.WDrun.GroupPlotSwitch = 0;
  }
  printf("Acquisition started\n");
  //my change
  #ifdef HAVE_CAEN_BRD
  //                busyON();
  //                usleep(10);
  //                CAEN_DGTZ_ClearData(handle);
                  usleep(10);
  //                busyOFF();
                  DisableGlobalVeto();
  #endif
  //--------
  CAEN_DGTZ_SWStartAcquisition(dgtzVars.handle);
  dgtzVars.WDrun.AcqRun = 1;
}

void stopDataAcquisition() {
  printf("Acquisition stopped\n");
  CAEN_DGTZ_SWStopAcquisition(dgtzVars.handle);
  dgtzVars.WDrun.AcqRun = 0;
  //my change
  #ifdef HAVE_CAEN_BRD
  EnableGlobalVeto();
  #endif
  //--------
  printf("Toal number of events: %d\n",
         runVars.eventNumber);
}

void checkKeyboardCommands() {
  int c = 0;
  if (!kbhit()) {
    return;
  }
  c = getch();
  if (c < '9' &&
      c >= '0') {
    int ch = c - '0';
    if (dgtzVars.BoardInfo.FamilyCode == CAEN_DGTZ_XX740_FAMILY_CODE ||
        dgtzVars.BoardInfo.FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE) {
      if (dgtzVars.BoardInfo.FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE &&
          dgtzVars.WDcfg.FastTriggerEnabled == 0 && ch == 8) {
        dgtzVars.WDrun.ChannelPlotMask = dgtzVars.WDrun.ChannelPlotMask;
      } else {
        dgtzVars.WDrun.ChannelPlotMask ^= (1 << ch);
      }
      if (dgtzVars.BoardInfo.FamilyCode == CAEN_DGTZ_XX740_FAMILY_CODE &&
          ch == 8) {
        printf("Channel %d belongs to a different group\n",
               ch + dgtzVars.WDrun.GroupPlotIndex * 8);
      } else {
        if (dgtzVars.WDrun.ChannelPlotMask & (1 << ch)) {
          printf("Channel %d enabled for plotting\n",
                 ch + dgtzVars.WDrun.GroupPlotIndex * 8);
        } else {
          printf("Channel %d disabled for plotting\n",
                 ch + dgtzVars.WDrun.GroupPlotIndex * 8);
        }
      }
    } else if (dgtzVars.BoardInfo.FamilyCode == CAEN_DGTZ_XX730_FAMILY_CODE ||
               dgtzVars.BoardInfo.FamilyCode == CAEN_DGTZ_XX725_FAMILY_CODE &&
               dgtzVars.WDcfg.Nch > 8) {
      ch = ch + 8 * dgtzVars.WDrun.GroupPlotSwitch;
      if (ch != 8 &&
          dgtzVars.WDcfg.EnableMask & (1 << ch)) {
        dgtzVars.WDrun.ChannelPlotMask ^= (1 << ch);
        if (dgtzVars.WDrun.ChannelPlotMask & (1 << ch)) {
          printf("Channel %d enabled for plotting\n", ch);
        } else {
          printf("Channel %d disabled for plotting\n", ch);
        }
      } else {
        printf("Channel %d not enabled for acquisition\n", ch);
      }
    } else {
      dgtzVars.WDrun.ChannelPlotMask ^= (1 << ch);
      if (dgtzVars.WDrun.ChannelPlotMask & (1 << ch)) {
        printf("Channel %d enabled for plotting\n", ch);
      } else {
        printf("Channel %d disabled for plotting\n", ch);
      }
    }
  } else {
    switch (c) {
      case 'g' :
        /**
         *for boards with >8 channels
         */
        if (dgtzVars.BoardInfo.FamilyCode == CAEN_DGTZ_XX730_FAMILY_CODE ||
            dgtzVars.BoardInfo.FamilyCode == CAEN_DGTZ_XX725_FAMILY_CODE &&
            dgtzVars.WDcfg.Nch > 8) {
          if (dgtzVars.WDrun.GroupPlotSwitch == 0) {
            dgtzVars.WDrun.GroupPlotSwitch = 1;
            printf("Channel group set to %d: use numbers 0-7 for channels 8-15\n",
                   dgtzVars.WDrun.GroupPlotSwitch);
          } else if (dgtzVars.WDrun.GroupPlotSwitch == 1) {
            dgtzVars.WDrun.GroupPlotSwitch = 0;
            printf("Channel group set to %d: use numbers 0-7 for channels 0-7\n",
                   dgtzVars.WDrun.GroupPlotSwitch);
          }
        } else {
          /**
           * Update the group plot index
           */
          if (dgtzVars.WDcfg.EnableMask &&
              dgtzVars.WDcfg.Nch > 8) {
            goToNextEnabledGroup();
          }
        }
        break;
      case 'q' :
        dgtzVars.WDrun.Quit = 1;
        break;
      case 'R' :
        dgtzVars.WDrun.Restart = 1;
        break;
      case 't' :
        if (!dgtzVars.WDrun.ContinuousTrigger) {
          CAEN_DGTZ_SendSWtrigger(dgtzVars.handle);
          printf("Single Software Trigger issued\n");
        }
        break;
      case 'T' :
        dgtzVars.WDrun.ContinuousTrigger ^= 1;
        if (dgtzVars.WDrun.ContinuousTrigger) {
          printf("Continuous trigger is enabled\n");
        } else {
          printf("Continuous trigger is disabled\n");
        }
        break;
      case 'P' :
        if (dgtzVars.WDrun.ChannelPlotMask == 0) {
          printf("No channel enabled for plotting\n");
        } else {
          dgtzVars.WDrun.ContinuousPlot ^= 1;
        }
        break;
      case 'p' :
        if (dgtzVars.WDrun.ChannelPlotMask == 0) {
          printf("No channel enabled for plotting\n");
        } else {
          dgtzVars.WDrun.SinglePlot = 1;
        }
        break;
      case 'f' :
        if (dgtzVars.WDrun.PlotType == PLOT_FFT) {
          dgtzVars.WDrun.PlotType = PLOT_WAVEFORMS;
        } else {
          dgtzVars.WDrun.PlotType = PLOT_FFT;
        }
        dgtzVars.WDrun.SetPlotOptions = 1;
        break;
      case 'h' :
        if (dgtzVars.WDrun.PlotType == PLOT_HISTOGRAM) {
          dgtzVars.WDrun.PlotType = PLOT_WAVEFORMS;
        } else {
          dgtzVars.WDrun.PlotType = PLOT_HISTOGRAM;
        }
        dgtzVars.WDrun.RunHisto = (dgtzVars.WDrun.PlotType == PLOT_HISTOGRAM);
        dgtzVars.WDrun.SetPlotOptions = 1;
        break;
      case 'w' :
        if (!dgtzVars.WDrun.ContinuousWrite) {
          dgtzVars.WDrun.SingleWrite = 1;
        }
        break;
      case 'W' :
        dgtzVars.WDrun.ContinuousWrite ^= 1;
        break;
      case 's' :
	if (dgtzVars.WDrun.AcqRun == 1) {
	  stopDataAcquisition();
	} else {
	  if (runVars.isFirstS == 0 &&
	      runVars.isStarted == 0) {
	    setRunNumber();
	    runVars.isFirstS = 1;
	  }
	  if (runVars.isStarted == 0) {
	  setBeginRunTime();
	  if (!openRunFile() ) {
	      appendRunHeader();//FIXX
	      runVars.isStarted = 1;
	      if (dgtzVars.WDrun.AcqRun == 0) {
		startDataAcquisition();
	      }
	    } else {
	      printf("Acquisition not started for problem during the opening of the data file\n");
	    }
	  } else {
	    startDataAcquisition();
	  }
	}
        break;
      case 'n':
         if (dgtzVars.WDrun.AcqRun) {
           printf("Acquisition stopped\n");
           CAEN_DGTZ_SWStopAcquisition(dgtzVars.handle);
           dgtzVars.WDrun.AcqRun = 0;
         }
        closeRunFile();
        dgtzVars.WDrun.Restart = 1;
        setRunNumber();
        runVars.isStarted = 0;
        break;
    case 'e':
      if (dgtzVars.WDrun.AcqRun == 0) {
        runVars.isMaxFileSize = !runVars.isMaxFileSize;
        if (!runVars.isMaxFileSize) {
          printf("Number of events to be collected: ");
          scanf("%d", &runVars.maxNumOfEvs);
          printf("%d\n", runVars.maxNumOfEvs);
        }
      }
      break;
    case 'm' :
        if (boardSupportsTemperatureRead()) {
          if (dgtzVars.WDrun.AcqRun == 0) {
            int32_t ch;
            for (ch = 0; ch < (int32_t) dgtzVars.BoardInfo.Channels; ch++) {
              uint32_t temp;
              int32_t ret = CAEN_DGTZ_ReadTemperature(
                  dgtzVars.handle, ch, &temp);
              printf("CH%02d: ", ch);
              if (ret == CAEN_DGTZ_Success) {
                printf("%u C\n", temp);
              } else {
                printf("CAENDigitizer ERR %d\n", ret);
              }
            }
            printf("\n");
          } else {
            printf("Can't run temperature monitor "
                   "while acquisition is running.\n");
          }
        } else {
          printf("Board Family doesn't support ADC Temperature Monitor.\n");
        }
        break;
      case 'c' :
        calibrate();
        break;
      case ' ' :
        printf("\n                            Bindkey help                                \n");
        printf("--------------------------------------------------------------------------\n");;
        printf("  [q]   Quit\n");
        printf("  [R]   Reload configuration file and restart\n");
        printf("  [n]   Set new run number\n");
	printf("  [e]   Start a new run when the maximum file size / number of events will be reached\n");
        printf("  [s]   Start/Stop acquisition\n");
        printf("  [t]   Send a software trigger (single shot)\n");
        printf("  [T]   Enable/Disable continuous software trigger\n");
        printf("  [w]   Write one event to output file\n");
        printf("  [W]   Enable/Disable continuous writing to output file\n");
        printf("  [p]   Plot one event\n");
        printf("  [P]   Enable/Disable continuous plot\n");
        printf("  [f]   Toggle between FFT and Waveform plot\n");
        printf("  [h]   Toggle between Histogram and Waveform plot\n");
        printf("  [g]   Change the index of the group to plot (XX740 family)\n");
        printf("  [m]   Single ADC temperature monitor (XX751/30/25 only)\n");
        printf("  [c]   ADC Calibration (XX751/30/25 only)\n");
        printf(" [0-7]  Enable/Disable one channel on the plot\n");
        printf("        For x740 family this is the plotted group's relative channel index\n");
        printf("[SPACE] This help\n");
        printf("--------------------------------------------------------------------------\n");
        printf("Press a key to continue\n");
        getch();
        break;
      default:
        break;
    }
  }
}

int writeOutputFilesx742() {
  if ((runVars.isMaxFileSize &&
       ftell(runVars.fout) > MAX_FILE_SIZE) ||
      (!runVars.isMaxFileSize &&
       runVars.eventNumber >= runVars.maxNumOfEvs)) {
    stopDataAcquisition();
    closeRunFile();
    setNextRunNumber();
    setBeginRunTime();
    openRunFile();
    appendRunHeader();
    runVars.isStarted = 1;
    startDataAcquisition();
  }
  appendEventHeader();
  runVars.eventNumber++;
  if (dgtzVars.WDcfg.FastTriggerEnabled) {
	  if (grON[0]) {
		  if (appendChannelHeader(32)) {
			  return 1;
		  }
		  /**
		   * Write channel data for x742
		   */
		  int igr;
		  for (igr=0; igr<2; ++igr) {
			  if (dgtzVars.Event742->GrPresent[igr]) {
				  if (appendChannelData(igr, 8)) {
					  return 1;
				  }
				  break;
			  }
		  }
	  }
	  if (grON[1]) {
		  if (appendChannelHeader(33)) {
			  return 1;
		  }
		  /**
		   * Write channel data for x742
		   */
		  int igr;
		  for (igr=2; igr<4; ++igr) {
			  if (dgtzVars.Event742->GrPresent[igr]) {
				  if (appendChannelData(igr, 8)) {
					  return 1;
				  }
				  break;
			  }
		  }
	  }
  }
  int gr, ch, k;
  int numberOfChs = sizeof(X742Chs) / sizeof(int);
  for (k = 0; k < numberOfChs; ++k) {
    gr = X742Chs[k] / 8;
    ch = X742Chs[k] % 8;
    /**
     * Write the Channel Header
     */
    if (appendChannelHeader(X742Chs[k])) {
      return 1;
    }
    /**
     * Write channel data for x742
     */
    if (appendChannelData(gr, ch)) {
      return 1;
    }
  }
  return 0;
}

void getConfigFileName(int argc,
                       char *argv[]) {
  if (argc > 1) {
    strcpy(dgtzVars.ConfigFileName, argv[1]);
  } else {
    strcpy(dgtzVars.ConfigFileName, DEFAULT_CONFIG_FILE);
  }
}

int parseConfigFile(/*CAENDGTZ_vars *dgtzVars*/) {
  memset(&dgtzVars.WDrun, 0, sizeof(dgtzVars.WDrun));
  memset(&dgtzVars.WDcfg, 0, sizeof(dgtzVars.WDcfg));
  printf("Opening Configuration File %s\n", dgtzVars.ConfigFileName);
  FILE *f_ini = fopen(dgtzVars.ConfigFileName, "r");
  if (f_ini == NULL) {
    ERROR_CODES errCode = ERR_CONF_FILE_NOT_FOUND;
    printError(&errCode);
    return 1;
  }
  ParseConfigFile(f_ini, &dgtzVars.WDcfg);
  fclose(f_ini);
  return 0;
}

int readBoardInfo()
{
  dgtzVars.isVMEDevice = dgtzVars.WDcfg.BaseAddress ? 1 : 0;
  dgtzVars.ret =
      CAEN_DGTZ_OpenDigitizer(dgtzVars.WDcfg.LinkType,
    		  dgtzVars.WDcfg.LinkNum,
			  dgtzVars.WDcfg.ConetNode,
			  dgtzVars.WDcfg.BaseAddress,
			  &dgtzVars.handle);
  if (dgtzVars.ret) {
    ERROR_CODES errCode = ERR_DGZ_OPEN;
    printError(&errCode);
    return 1;
  }
  dgtzVars.ret = CAEN_DGTZ_GetInfo(dgtzVars.handle, &dgtzVars.BoardInfo);
  if (dgtzVars.ret) {
    ERROR_CODES errCode = ERR_BOARD_INFO_READ;
    printError(&errCode);
    return 2;
  }
  printf("Connected to CAEN Digitizer Model %s\n", dgtzVars.BoardInfo.ModelName);
  printf("ROC FPGA Release is %s\n", dgtzVars.BoardInfo.ROC_FirmwareRel);
  printf("AMC FPGA Release is %s\n", dgtzVars.BoardInfo.AMC_FirmwareRel);
  return 0;
}

int checkFrimwareRevision() {
  int majorNumber;
  sscanf(dgtzVars.BoardInfo.AMC_FirmwareRel, "%d", &majorNumber);
    if (majorNumber >= 128) {
      printf("This digitizer has a DPP firmware\n");
      ERROR_CODES errCode = ERR_INVALID_BOARD_TYPE;
      printError(&errCode);
      return 2;
    }
    return 0;
}

int parseConfigFile_x742() {
  if (dgtzVars.BoardInfo.FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE) {
    printf("\nWARNING: using configuration file "
           "WaveDumpConfig_X742.txt specific for "
           "Board model X742.\nEdit this file if "
           "you want to modify the default settings.\n ");
#ifdef LINUX 
    strcpy(dgtzVars.ConfigFileName, "/etc/wavedump/WaveDumpConfig_X742.txt");
#else
    strcpy(dgtzVars.ConfigFileName, "WaveDumpConfig_X742.txt");
#endif
    FILE *f_ini = fopen(dgtzVars.ConfigFileName, "r");
    if (f_ini == NULL) {
      ERROR_CODES errCode = ERR_CONF_FILE_NOT_FOUND;
      printError(&errCode);
      return 2;
    }
    ParseConfigFile(f_ini, &dgtzVars.WDcfg);
    fclose(f_ini);
  }
  return 0;
}

int getNumOfBoardChsBitsGroups() {
  dgtzVars.ret =
      GetMoreBoardInfo(dgtzVars.handle, dgtzVars.BoardInfo, &dgtzVars.WDcfg);
  if (dgtzVars.ret) {
    ERROR_CODES errCode = ERR_INVALID_BOARD_TYPE;
    printError(&errCode);
    return 2;
  }
  return 0;
}

void performCalibration() {
  if (dgtzVars.WDcfg.StartupCalibration) {
    calibrate(dgtzVars.handle, &dgtzVars.WDrun, dgtzVars.BoardInfo);
  }
}

void maskNotAvailableChs() {
  if (dgtzVars.BoardInfo.FamilyCode != CAEN_DGTZ_XX740_FAMILY_CODE &&
      dgtzVars.BoardInfo.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE) {
    dgtzVars.WDcfg.EnableMask &= (1 << dgtzVars.WDcfg.Nch) - 1;
  } else {
    dgtzVars.WDcfg.EnableMask &= (1 << (dgtzVars.WDcfg.Nch / 8)) - 1;
  }
  if (dgtzVars.BoardInfo.FamilyCode == CAEN_DGTZ_XX751_FAMILY_CODE &&
      dgtzVars.WDcfg.DesMode) {
    dgtzVars.WDcfg.EnableMask &= 0xAA;
  }
  if (dgtzVars.BoardInfo.FamilyCode == CAEN_DGTZ_XX731_FAMILY_CODE &&
      dgtzVars.WDcfg.DesMode) {
    dgtzVars.WDcfg.EnableMask &= 0x55;
  }
}

void setPlotMask() {
  if (dgtzVars.BoardInfo.FamilyCode != CAEN_DGTZ_XX740_FAMILY_CODE &&
      dgtzVars.BoardInfo.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE) {
        dgtzVars.WDrun.ChannelPlotMask = dgtzVars.WDcfg.EnableMask;
    } else {
        dgtzVars.WDrun.ChannelPlotMask = (dgtzVars.WDcfg.FastTriggerEnabled == 0) ? 0xFF: 0x1FF;
    }
  if (dgtzVars.BoardInfo.FamilyCode == CAEN_DGTZ_XX730_FAMILY_CODE ||
      dgtzVars.BoardInfo.FamilyCode == CAEN_DGTZ_XX725_FAMILY_CODE) {
    dgtzVars.WDrun.GroupPlotSwitch = 0;
  }
}

int programTheDigitizer() {
  dgtzVars.ret = programDigitizer();
  if (dgtzVars.ret) {
    ERROR_CODES errCode = ERR_DGZ_PROGRAM;
    printError(&errCode);
    return 2;
  }
  return 0;
}

void selectNextGroup() {
  if (dgtzVars.WDcfg.EnableMask &&
      dgtzVars.BoardInfo.FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE) {
    if ((dgtzVars.WDcfg.EnableMask >> dgtzVars.WDrun.GroupPlotIndex) & 0x1 == 0) {
      goToNextEnabledGroup();
    }
  }
}

int disableAutoCorrections() {
  dgtzVars.ret =
      CAEN_DGTZ_DisableDRS4Correction(dgtzVars.handle);
  if (dgtzVars.ret != CAEN_DGTZ_Success) {
    ERROR_CODES errCode = ERR_NONE;
    printError(&errCode);
    return 2;
  }
  return 0;
}

int loadCorrTabFromFlash() {
  dgtzVars.ret =
      CAEN_DGTZ_GetCorrectionTables(dgtzVars.handle,
                                    dgtzVars.WDcfg.DRS4Frequency,
                                    (void*) (dgtzVars.X742Tables));
  if (dgtzVars.ret != CAEN_DGTZ_Success) {
    ERROR_CODES errCode = ERR_NONE;
    printError(&errCode);
    return 2;
  }
  return 0;
}

void setCustomTables(uint32_t* groupMask) {
  uint32_t gr;
  *groupMask = dgtzVars.WDcfg.UseManualTables;
  for (gr = 0; gr < dgtzVars.WDcfg.MaxGroupNumber; ++gr) {
    if (((*groupMask >> gr) & 0x1) == 0) {
      continue;
    }
    LoadCorrectionTable(dgtzVars.WDcfg.TablesFilenames[gr], &(dgtzVars.X742Tables[gr]));
  }
}

void saveTablesFromFlash(uint32_t* groupMask) {
  *groupMask = (~(*groupMask)) & ((0x1 << dgtzVars.WDcfg.MaxGroupNumber) - 1);
  SaveCorrectionTables("X742Table", *groupMask, dgtzVars.X742Tables);
}

int useAutoCorrections() {
  dgtzVars.ret =
      CAEN_DGTZ_LoadDRS4CorrectionData(dgtzVars.handle, dgtzVars.WDcfg.DRS4Frequency);
  if (dgtzVars.ret != CAEN_DGTZ_Success) {
    ERROR_CODES errCode = ERR_NONE;
    printError(&errCode);
    return 2;
  }
  dgtzVars.ret = CAEN_DGTZ_EnableDRS4Correction(dgtzVars.handle);
  if (dgtzVars.ret != CAEN_DGTZ_Success) {
    ERROR_CODES errCode = ERR_NONE;
    printError(&errCode);
    return 2;
  }
  return 0;
}

int reloadCorrTables() {
  dgtzVars.ret = CAEN_DGTZ_GetInfo(dgtzVars.handle, &dgtzVars.BoardInfo);
  if (dgtzVars.ret) {
    ERROR_CODES errCode = ERR_BOARD_INFO_READ;
    printError(&errCode);
    return 2;
  }
  dgtzVars.ret = GetMoreBoardInfo(dgtzVars.handle, dgtzVars.BoardInfo, &dgtzVars.WDcfg);
  if (dgtzVars.ret) {
    ERROR_CODES errCode = ERR_INVALID_BOARD_TYPE;
    printError(&errCode);
    return 2;
  }
  if (dgtzVars.BoardInfo.FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE &&
      ((dgtzVars.ReloadCfgStatus) & (0x1 << CFGRELOAD_CORRTABLES_BIT))) {
    if (dgtzVars.WDcfg.useCorrections != -1) {
      uint32_t groupMask = 0;
      // Use Manual Corrections
      // Disable Automatic Corrections
      if ( disableAutoCorrections() ) { return 2; }
      // Load the Correction Tables from the Digitizer flash
      if ( loadCorrTabFromFlash() ) { return 2; }
      // The user wants to use some custom tables
      if (dgtzVars.WDcfg.UseManualTables != -1) {
        setCustomTables(&groupMask);
      }
      // Save to file the Tables read from flash
      saveTablesFromFlash(&groupMask);
    } else {
      // Use Automatic Corrections
      if ( useAutoCorrections() ) { return 2; }
    }
  }
  return 0;
}

int allocateMemory() {
  uint32_t allocatedSize;
  if (dgtzVars.WDcfg.Nbit == 8) {
    dgtzVars.ret = CAEN_DGTZ_AllocateEvent(
        dgtzVars.handle, (void**) &dgtzVars.Event8);
  } else {
    if (dgtzVars.BoardInfo.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE) {
    	dgtzVars.ret = CAEN_DGTZ_AllocateEvent(
          dgtzVars.handle, (void**) &dgtzVars.Event16);
    } else {
    	dgtzVars.ret = CAEN_DGTZ_AllocateEvent(
          dgtzVars.handle, (void**) &dgtzVars.Event742);
    }
  }
  if (dgtzVars.ret != CAEN_DGTZ_Success) {
    ERROR_CODES errCode = ERR_MALLOC;
    printError(&errCode);
    return 2;
  }
  /**
   * WARNING: This malloc must be done after the digitizer programming
   */
  dgtzVars.ret = CAEN_DGTZ_MallocReadoutBuffer(
		  dgtzVars.handle, &dgtzVars.buffer, &allocatedSize);
  if (dgtzVars.ret) {
    ERROR_CODES errCode = ERR_MALLOC;
    printError(&errCode);
    return 2;
  }
  return 0;
}

void startStopAquisition() {
  if (dgtzVars.WDrun.Restart && dgtzVars.WDrun.AcqRun) {
    if (dgtzVars.BoardInfo.FamilyCode == CAEN_DGTZ_XX740_FAMILY_CODE) {
      /**
       * XX740 specific
       */
      Calibrate_XX740_DC_Offset();
    } else {
      if (dgtzVars.BoardInfo.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE) {
        /**
         * XX742 not considered
         */
        Calibrate_DC_Offset();
      }
    }
    CAEN_DGTZ_SWStartAcquisition(dgtzVars.handle);
  } else {
    printf("[s] start/stop the acquisition, [q] quit, [SPACE] help\n");
  }
  dgtzVars.WDrun.Restart = 0;
  prevRateTime = get_time();
}

void closeDev() {
	CAEN_DGTZ_CloseDigitizer(dgtzVars.handle);
}

void quit(int ext) {
	if (ext ==1 ) { exit(1); }
	closeDev();
	if (ext ==2 ) { exit(2); }
	exit(0);
}

int initDGTZ(int argc, char* argv[]) {
	printf("\n");
  	printf("**************************************************************\n");
 	 printf("                        Wave Dump %s\n", WaveDump_Release);
  	printf("**************************************************************\n");
  	getConfigFileName(argc, argv);
	/**
	* Open and parse configuration file
	*/
  	if ( parseConfigFile() ) {
          return 1;
	}
	/**
	* Open the digitizer and read the board information.
	*/
	int stat = readBoardInfo();
	if (stat) {
          return stat;
	}
	/**
	* Check firmware rivision (DPP firmwares cannot be used with WaveDump).
	*/
	if(checkFrimwareRevision()) {
          return 2;
	}
	/**
  	* Check if model x742 is in use.
	*/
	if (parseConfigFile_x742()) {
          return 2;
	}
  	/**
	* Get Number of Channels, Number of bits, Number of Groups of the board.
	*/
	if (getNumOfBoardChsBitsGroups()) {
          return 2;
	}
	/**
	* Perform calibration (if needed).
	*/
	performCalibration();
	/**
	* Restart.
	*/ 
	if (confDGTZ()) {
		return 2;
	}

	return 0;
}

void frontend_init(int argc, char* argv[]) {
  int stat = initDGTZ(argc, argv);
  if (stat) {
    quit(stat);
  }
#ifdef HAVE_CAEN_BRD
  vmeHandle = dgtzVars.BoardInfo.VMEHandle;
  initBridge(1);
#endif
  LAM_ON = 0;
#ifdef HAVE_V1190
  LAM_ON |= 1;
#endif
  LAM_ON |= 64;
}

void frontend_end() {

  /**
   * Stop the acquisition.
   */
  CAEN_DGTZ_SWStopAcquisition(dgtzVars.handle);
  /**
   * Close the plotter.
   */
  if (dgtzVars.PlotVar) {
    ClosePlotter();
  }
  /**
   * Close the output files and free histograms.
   */
  if (runVars.fout) {
    closeRunFile();
  }
  int ch;
  for (ch = 0; ch < dgtzVars.WDcfg.Nch; ++ch) {
    if ((dgtzVars.WDrun.Histogram)[ch]) {
      free((dgtzVars.WDrun.Histogram)[ch]);
    }
  }
  /**
   * Close the device and free the buffers.
   */
  if(dgtzVars.Event8) {
    CAEN_DGTZ_FreeEvent(dgtzVars.handle, (void**) &dgtzVars.Event8);
  }
  if(dgtzVars.Event16) {
    CAEN_DGTZ_FreeEvent(dgtzVars.handle, (void**) &dgtzVars.Event16);
  }
  CAEN_DGTZ_FreeReadoutBuffer(&dgtzVars.buffer);
//  CAEN_DGTZ_CloseDigitizer(dgtzVars.handle);
}

void myConfV1742() {
	  //My Chaneges in the Board Config
	  if ( (dgtzVars.BoardInfo.FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE) ) {
	    dgtzVars.ret = CAEN_DGTZ_WriteRegister(dgtzVars.handle,0x8110,0xC000000F);//  V1742_FP_TRIGGER_OUT_EN_MASK
	    uint32_t acqReg=0;
	    dgtzVars.ret |= CAEN_DGTZ_ReadRegister(dgtzVars.handle,0x8000,&acqReg);//  V1742_GROUP_CONFIG
	    //  std::cerr<<"acqReg bf: "<<std::hex<<acqReg<<std::endl;
	    acqReg |= 0x20001950;
	    //acqReg |= 0x20029950;
	    //  std::cerr<<"acqReg af: "<<std::hex<<acqReg<<std::endl;
	    dgtzVars.ret |= CAEN_DGTZ_WriteRegister(dgtzVars.handle,0x8000,acqReg);//  V1742_GROUP_CONFIG

	    dgtzVars.ret |= CAEN_DGTZ_ReadRegister(dgtzVars.handle,0x811C,&acqReg);//  1742_FP_IO_CONTROL
	    //  std::cerr<<"FqReg bf: "<<std::hex<<acqReg<<std::endl;
	    acqReg &= ~(0x38000);
	    //  std::cerr<<"FpReg af: "<<std::hex<<acqReg<<std::endl;
	    dgtzVars.ret |= CAEN_DGTZ_WriteRegister(dgtzVars.handle,0x811C,acqReg);//  1742_FP_IO_CONTROL

	  }
}

int confDGTZ() {
	/**
	* mask the channels not available for this model
	*/
	maskNotAvailableChs();
	/**
	* Set plot mask
	*/
	setPlotMask();
	/**
	* Program the digitizer.
	*/
	if (programTheDigitizer()) {
		return 2;
	}
	/**
	* Select the next enabled group for plotting
	*/
	selectNextGroup();
	if ((dgtzVars.ReloadCfgStatus) > 0) {
	/**
	* Reload Correction Tables if changed
	*/
	if (reloadCorrTables()) {
		return 2;
	}
  }

  myConfV1742();

  /**
   * Allocate memory for the event data and readout buffer
   */
  if ( allocateMemory() ) { return 2; }
  return 0;
}

int restart() {
if ( confDGTZ() ) { return 2; }

  startStopAquisition();
    return 0;
}

int restartCall() {
    CAEN_DGTZ_SWStopAcquisition(dgtzVars.handle);
    CAEN_DGTZ_FreeReadoutBuffer(&dgtzVars.buffer);
    ClosePlotter();
    dgtzVars.PlotVar = NULL;
    if (dgtzVars.WDcfg.Nbit == 8) {
      CAEN_DGTZ_FreeEvent(dgtzVars.handle, (void**) &dgtzVars.Event8);
    } else {
      if (dgtzVars.BoardInfo.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE) {
        CAEN_DGTZ_FreeEvent(dgtzVars.handle, (void**) &dgtzVars.Event16);
      } else {
        CAEN_DGTZ_FreeEvent(dgtzVars.handle, (void**) &dgtzVars.Event742);
      }
    }
    FILE *f_ini = fopen(dgtzVars.ConfigFileName, "r");
    dgtzVars.ReloadCfgStatus = ParseConfigFile(f_ini, &dgtzVars.WDcfg);
    fclose(f_ini);

    if (restart()) {
	return 2;
	}
    return 0;
}

void interruptTimeout() {
  /**
   * Calculate throughput and trigger rate (every second).
  */
  dgtzVars.Nb += dgtzVars.BufferSize;
  dgtzVars.Ne += dgtzVars.NumEvents;
  uint64_t currentTime = get_time();
  uint64_t elapsedTime = currentTime - prevRateTime;
  if (elapsedTime > 1000) {
    if (dgtzVars.Nb == 0) {
      if (dgtzVars.ret == CAEN_DGTZ_Timeout) {
        printf("Timeout...\n");
      } else {
        printf("No data...\n");
      }
    } else {
      clear();
      printf("Run: %d\n"
             "Total number of events: %d\n"
             "Reading at %.2f MB/s (Trg Rate: %.2f Hz)\n",
             runVars.runNumber,
             runVars.eventNumber,
             (float) dgtzVars.Nb / ((float) elapsedTime * 1048.576f),
             (float) dgtzVars.Ne * 1000.0f / (float) elapsedTime);
    }
    dgtzVars.Nb = 0;
    dgtzVars.Ne = 0;
    prevRateTime = currentTime;
  }
}

void plotWaveforms() {
  if ((dgtzVars.WDrun.ContinuousPlot ||
       dgtzVars.WDrun.SinglePlot) &&
      !IsPlotterBusy()) {
    int nTraces;
    if (dgtzVars.BoardInfo.FamilyCode ==
        CAEN_DGTZ_XX740_FAMILY_CODE) {
      nTraces = 8;
    } else {
      nTraces = dgtzVars.WDcfg.Nch;
    }
    if (dgtzVars.BoardInfo.FamilyCode ==
        CAEN_DGTZ_XX742_FAMILY_CODE) {
      nTraces = 9;
    }
    if (dgtzVars.PlotVar == NULL) {
      int traceLength = max(
          dgtzVars.WDcfg.RecordLength,
          (uint32_t)(1 << dgtzVars.WDcfg.Nbit));
      dgtzVars.PlotVar = OpenPlotter(
          dgtzVars.WDcfg.GnuPlotPath,
          nTraces, traceLength);
      dgtzVars.WDrun.SetPlotOptions = 1;
    }
    if (dgtzVars.PlotVar == NULL) {
       printf("Can't open the plotter\n");
       dgtzVars.WDrun.ContinuousPlot = 0;
       dgtzVars.WDrun.SinglePlot = 0;
     } else {
       int Tn = 0;
       if (dgtzVars.WDrun.SetPlotOptions) {
         if ((dgtzVars.WDrun.PlotType == PLOT_WAVEFORMS) &&
             (dgtzVars.BoardInfo.FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE)) {
           strcpy(dgtzVars.PlotVar->Title, "Waveform");
           dgtzVars.PlotVar->Xscale = dgtzVars.WDcfg.Ts;
           strcpy(dgtzVars.PlotVar->Xlabel, "ns");
           strcpy(dgtzVars.PlotVar->Ylabel, "ADC counts");
           dgtzVars.PlotVar->Yautoscale = 0;
           dgtzVars.PlotVar->Ymin = 0;
           dgtzVars.PlotVar->Ymax = (float)(1 << dgtzVars.WDcfg.Nbit);
           dgtzVars.PlotVar->Xautoscale = 1;
         } else if (dgtzVars.WDrun.PlotType == PLOT_WAVEFORMS) {
           strcpy(dgtzVars.PlotVar->Title, "Waveform");
           dgtzVars.PlotVar->Xscale = dgtzVars.WDcfg.Ts *
               dgtzVars.WDcfg.DecimationFactor / 1000;
           strcpy(dgtzVars.PlotVar->Xlabel, "us");
           strcpy(dgtzVars.PlotVar->Ylabel, "ADC counts");
           dgtzVars.PlotVar->Yautoscale = 0;
           dgtzVars.PlotVar->Ymin = 0;
           dgtzVars.PlotVar->Ymax = (float)(1 << dgtzVars.WDcfg.Nbit);
           dgtzVars.PlotVar->Xautoscale = 1;
         }  else if (dgtzVars.WDrun.PlotType == PLOT_FFT) {
           strcpy(dgtzVars.PlotVar->Title, "FFT");
           strcpy(dgtzVars.PlotVar->Xlabel, "MHz");
           strcpy(dgtzVars.PlotVar->Ylabel, "dB");
           dgtzVars.PlotVar->Yautoscale = 1;
           dgtzVars.PlotVar->Ymin = -160;
           dgtzVars.PlotVar->Ymax = 0;
           dgtzVars.PlotVar->Xautoscale = 1;
         } else if (dgtzVars.WDrun.PlotType == PLOT_HISTOGRAM) {
           dgtzVars.PlotVar->Xscale = 1.0;
           strcpy(dgtzVars.PlotVar->Title, "Histogram");
           strcpy(dgtzVars.PlotVar->Xlabel, "ADC channels");
           strcpy(dgtzVars.PlotVar->Ylabel, "Counts");
           dgtzVars.PlotVar->Yautoscale = 1;
           dgtzVars.PlotVar->Xautoscale = 1;
         }
         SetPlotOptions();
         dgtzVars.WDrun.SetPlotOptions = 0;
       }
       int ch;
       for (ch = 0; ch < nTraces; ++ch) {
         int absCh = dgtzVars.WDrun.GroupPlotIndex * 8 + ch;  
         if (!((dgtzVars.WDrun.ChannelPlotMask >> ch) & 1)) {
           continue;
         }
         if ((dgtzVars.BoardInfo.FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE) &&
             ((ch != 0) && (absCh % 8) == 0)) {
           sprintf(dgtzVars.PlotVar->TraceName[Tn],
                   "TR %d", (int)((absCh - 1) / 16));
         } else {
           sprintf(dgtzVars.PlotVar->TraceName[Tn],
                   "CH %d", absCh);
         }
         if (dgtzVars.WDrun.PlotType == PLOT_WAVEFORMS) {
           if (dgtzVars.WDcfg.Nbit == 8) {
             dgtzVars.PlotVar->TraceSize[Tn] = dgtzVars.Event8->ChSize[absCh];
             memcpy(dgtzVars.PlotVar->TraceData[Tn],
                    dgtzVars.Event8->DataChannel[absCh],
                    dgtzVars.Event8->ChSize[absCh]);
             dgtzVars.PlotVar->DataType = PLOT_DATA_UINT8;
           } else if (dgtzVars.BoardInfo.FamilyCode ==
                      CAEN_DGTZ_XX742_FAMILY_CODE) {
             if (dgtzVars.Event742->GrPresent[dgtzVars.WDrun.GroupPlotIndex]) { 
               dgtzVars.PlotVar->TraceSize[Tn] =
                   dgtzVars.Event742->DataGroup[dgtzVars.WDrun.GroupPlotIndex].ChSize[ch];
               memcpy(dgtzVars.PlotVar->TraceData[Tn],
                      dgtzVars.Event742->DataGroup[dgtzVars.WDrun.GroupPlotIndex].DataChannel[ch],
                      dgtzVars.Event742->DataGroup[dgtzVars.WDrun.GroupPlotIndex].ChSize[ch] * sizeof(float));
               dgtzVars.PlotVar->DataType = PLOT_DATA_FLOAT;
             }
           } else {
             dgtzVars.PlotVar->TraceSize[Tn] = dgtzVars.Event16->ChSize[absCh];
             memcpy(dgtzVars.PlotVar->TraceData[Tn],
                    dgtzVars.Event16->DataChannel[absCh],
                    dgtzVars.Event16->ChSize[absCh] * 2);
             dgtzVars.PlotVar->DataType = PLOT_DATA_UINT16;
           }
         } else if (dgtzVars.WDrun.PlotType == PLOT_FFT) {
           int FFTns;
           dgtzVars.PlotVar->DataType = PLOT_DATA_DOUBLE;
           if (dgtzVars.WDcfg.Nbit == 8) {
             FFTns = FFT(dgtzVars.Event8->DataChannel[absCh],
                         dgtzVars.PlotVar->TraceData[Tn],
                         dgtzVars.Event8->ChSize[absCh],
                         HANNING_FFT_WINDOW,
                         SAMPLETYPE_UINT8);
           } else if (dgtzVars.BoardInfo.FamilyCode ==
                      CAEN_DGTZ_XX742_FAMILY_CODE) {
             FFTns = FFT(dgtzVars.Event742->DataGroup[dgtzVars.WDrun.GroupPlotIndex].DataChannel[ch],
                         dgtzVars.PlotVar->TraceData[Tn],
                         dgtzVars.Event742->DataGroup[dgtzVars.WDrun.GroupPlotIndex].ChSize[ch],
                         HANNING_FFT_WINDOW, SAMPLETYPE_FLOAT);
           } else {
             FFTns = FFT(dgtzVars.Event16->DataChannel[absCh],
                         dgtzVars.PlotVar->TraceData[Tn],
                         dgtzVars.Event16->ChSize[absCh],
                         HANNING_FFT_WINDOW, SAMPLETYPE_UINT16);
           }
           dgtzVars.PlotVar->Xscale = (1000 / dgtzVars.WDcfg.Ts) / (2 * FFTns);
           dgtzVars.PlotVar->TraceSize[Tn] = FFTns;
         } else if (dgtzVars.WDrun.PlotType == PLOT_HISTOGRAM) {
           dgtzVars.PlotVar->DataType = PLOT_DATA_UINT32;
           strcpy(dgtzVars.PlotVar->Title, "Histogram");
           dgtzVars.PlotVar->TraceSize[Tn] = 1 << dgtzVars.WDcfg.Nbit;
           memcpy(dgtzVars.PlotVar->TraceData[Tn],
                  dgtzVars.WDrun.Histogram[absCh],
                  (uint64_t)(1 << dgtzVars.WDcfg.Nbit) * sizeof(uint32_t));
         }
         Tn++;
         if (Tn >= MAX_NUM_TRACES) {
           break;
         }
       }
       dgtzVars.PlotVar->NumTraces = Tn;
       if (PlotWaveforms() < 0) {
         dgtzVars.WDrun.ContinuousPlot = 0;
         printf("Plot Error\n");
       }
       dgtzVars.WDrun.SinglePlot = 0;
    }
  }
}

int writeEventDataToFile() {
  if (dgtzVars.WDrun.ContinuousWrite || dgtzVars.WDrun.SingleWrite) {
    /**
     * Note: use a thread here to allow parallel readout and file writing
     */
    if (dgtzVars.BoardInfo.FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE) {
      dgtzVars.ret = writeOutputFilesx742();
    }
    if (dgtzVars.ret) {
      ERROR_CODES errCode = ERR_OUTFILE_WRITE;
      printError(&errCode);
      return 2;
    }
    if (dgtzVars.WDrun.SingleWrite) {
      printf("Single Event saved to output files\n");
      dgtzVars.WDrun.SingleWrite = 0;
    }
  }
  return 0;
}

void updateHistograms() {
  int ch;
  for (ch = 0; ch < dgtzVars.WDcfg.Nch; ++ch) {
    int chmask =
        ((dgtzVars.BoardInfo.FamilyCode ==
          CAEN_DGTZ_XX740_FAMILY_CODE) ||
         (dgtzVars.BoardInfo.FamilyCode ==
          CAEN_DGTZ_XX742_FAMILY_CODE) )? (ch / 8) : ch;
    if (!(dgtzVars.EventInfo.ChannelMask & (1 << chmask))) {
      continue;
    }
    if (dgtzVars.WDrun.Histogram[ch] == NULL) {
      dgtzVars.WDrun.Histogram[ch] = malloc(
          (uint64_t)(1 << dgtzVars.WDcfg.Nbit) *
          sizeof(uint32_t));
      if (dgtzVars.WDrun.Histogram[ch] == NULL) {
        ERROR_CODES errCode = ERR_HISTO_MALLOC;
        printError(&errCode);
        exit(0);
      }
      memset(dgtzVars.WDrun.Histogram[ch], 0,
             (uint64_t)(1 << dgtzVars.WDcfg.Nbit) *
             sizeof(uint32_t));
    }
    if (dgtzVars.WDcfg.Nbit == 8) {
      int i;
      for (i = 0; i < (int) dgtzVars.Event8->ChSize[ch]; ++i) {
        dgtzVars.WDrun.Histogram[ch][dgtzVars.Event8->DataChannel[ch][i]]++;
      }
    } else {
      if (dgtzVars.BoardInfo.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE) {
        int i;
        for (i = 0; i < (int) dgtzVars.Event16->ChSize[ch]; ++i) {
          dgtzVars.WDrun.Histogram[ch][dgtzVars.Event16->DataChannel[ch][i]]++;
        }
      } else {
        printf("Can't build samples histogram for this board: it has float samples.\n");
        dgtzVars.WDrun.RunHisto = 0;
        dgtzVars.WDrun.PlotType = PLOT_WAVEFORMS;
        break;
      }
    }
  }
}

int decodeTheEvent() {
  if (dgtzVars.WDcfg.Nbit == 8) {
    dgtzVars.ret = CAEN_DGTZ_DecodeEvent(
        dgtzVars.handle,
        dgtzVars.EventPtr,
        (void**) &dgtzVars.Event8);
  } else {
    if (dgtzVars.BoardInfo.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE) {
      dgtzVars.ret = CAEN_DGTZ_DecodeEvent(
          dgtzVars.handle,
          dgtzVars.EventPtr,
          (void**) dgtzVars.Event16);
    } else {
      dgtzVars.ret = CAEN_DGTZ_DecodeEvent(
          dgtzVars.handle,
          dgtzVars.EventPtr,
          (void**) &dgtzVars.Event742);
      /**
       * if manual corrections
       */
      if (dgtzVars.WDcfg.useCorrections != -1) {
        uint32_t gr;
        for (gr = 0; gr < dgtzVars.WDcfg.MaxGroupNumber; ++gr) {
          if (((dgtzVars.WDcfg.EnableMask >> gr) & 0x1) == 0) {
            continue;
          }
          ApplyDataCorrection(
              &(dgtzVars.X742Tables[gr]),
              dgtzVars.WDcfg.DRS4Frequency,
              dgtzVars.WDcfg.useCorrections,
              &(dgtzVars.Event742->DataGroup[gr]));
        }
      }
    }
  }
  if (dgtzVars.ret) {
    ERROR_CODES errCode = ERR_EVENT_BUILD;
    printError(&errCode);
    return 2;
  }
  return 0;
}

int getEventFromTheReadOutBuffer(int evNumber) {
  dgtzVars.ret = CAEN_DGTZ_GetEventInfo(
      dgtzVars.handle, dgtzVars.buffer, dgtzVars.BufferSize,
      evNumber, &dgtzVars.EventInfo, &dgtzVars.EventPtr);
  if (dgtzVars.ret) {
    ERROR_CODES errCode = ERR_EVENT_BUILD;
    printError(&errCode);
    return 2;
  }
  return 0;
}

int analyzeData() {
  int i;
  for (i = 0; i < (int) dgtzVars.NumEvents; ++i) {
    /**
     * Get one event from the readout buffer
     */
    if (getEventFromTheReadOutBuffer(i)) {
      return 2;
    }
    /**
     * decode the event
     */
    if (decodeTheEvent()) {
      return 2;
    }
    /**
     * Update Histograms
     */
    if (dgtzVars.WDrun.RunHisto) {
      updateHistograms();
    }
    /**
     * Write Event data to file
     */
    if (writeEventDataToFile()) {
      return 2;
    }
    /**
     * Plot Waveforms
     */
    plotWaveforms();
  }
  return 0;
}

int readDataFromTheBoard() {
  dgtzVars.ret = CAEN_DGTZ_ReadData(
      dgtzVars.handle,
      CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT,
      dgtzVars.buffer, &dgtzVars.BufferSize);
  if (dgtzVars.ret) {
    ERROR_CODES errCode = ERR_READOUT;
    printError(&errCode);
    return 2;
  }
  dgtzVars.NumEvents = 0;
  if (dgtzVars.BufferSize != 0) {
    dgtzVars.ret = CAEN_DGTZ_GetNumEvents(
        dgtzVars.handle, dgtzVars.buffer, dgtzVars.BufferSize, &dgtzVars.NumEvents);
    if (dgtzVars.ret) {
      ERROR_CODES errCode = ERR_READOUT;
      printError(&errCode);
      return 2;
    }
  } else {
    uint32_t lstatus;
    dgtzVars.ret = CAEN_DGTZ_ReadRegister(
        dgtzVars.handle,
        CAEN_DGTZ_ACQ_STATUS_ADD,
        &lstatus);
    if (dgtzVars.ret) {
      printf("Warning: Failure reading reg:%x (%d)\n",
             CAEN_DGTZ_ACQ_STATUS_ADD, dgtzVars.ret);
    } else {
      if (lstatus & (0x1 << 19)) {
        ERROR_CODES errCode = ERR_OVERTEMP;
        printError(&errCode);
        return 2;
      }
    }
  }
  return 0;
}

int readingV1742() {
  /**
   * Read data from the board
   */
  if (readDataFromTheBoard()) { return 2;}
    /**
     * Interrupt timeout
     */
    interruptTimeout();
    /**
     * Analyze data
     */
    if (analyzeData()) { return 2; }

    return 0;
}

int lamV1742(char *lam){

  //Check data ready
  //    ChkTime = getMicrotime();
  uint32_t status=0;
  //    *dataReady=0;
  //    ++nCheck;
  dgtzVars.ret = CAEN_DGTZ_ReadRegister(dgtzVars.handle,CAEN_DGTZ_VME_STATUS_ADD,&status);
  //        if (iEv>0) printf("Time beteewn Chk lam: %d\n", (ChkTime - PChkTime));
  if (dgtzVars.ret) {
    printf("V1742: Error during checking lam: %s\n",ErrMsg[ERR_READOUT]);
    //ERROR_CODES ErrCode = ERR_READOUT;
    return 2;
  } else {
    *lam |= ( ((status & 0x1)!=0) << 6 );
    //busyON();
  }
  //      ret = CAEN_DGTZ_ReadRegister(handle,CAEN_DGTZ_ACQ_STATUS_ADD,&status); /* read status register */
    //      if (ret) {
    //        ErrCode = ERR_READOUT;
    //        goto QuitProgram;
    //      } else {
    //        dataReady = ((status & 0x8)!=0); /* 4th bit is data ready */
    //      }

    return 0;
}

int LAM(char *lam){
//std::cerr<<"Start LAM "<<std::endl;
    int lam1 = (*lam) & 0x1;
    int lam2 = ((*lam) >> 1) & 0x1;
    int lam3 = ((*lam) >> 2) & 0x1;
    int lam4 = ((*lam) >> 3) & 0x1;
    int lam5 = ((*lam) >> 4) & 0x1;
    int lam6 = ((*lam) >> 5) & 0x1;
    int lam7 = ((*lam) >> 6) & 0x1;

    if(!lam7) {
    	if (lamV1742(lam)) {
          return 2;
        }
    }

    return 0;

}

int poll_event(/*int count=1,*/ int test) {
//	char LAM_ON = 0;

//	int i=0 ;
//	for ( ; i<count ; i++){
//		printf("lam checking\n");

		//	#ifdef HAVE_OSC
		//	    for (int iosc=0; iosc<NOSC; ++iosc) { OSCLam[iosc]=false; }
		//	#endif

		char lam = 0;
		if (dgtzVars.WDrun.AcqRun==1) {
			if ( LAM(&lam) ) { return 2; }
		}
//		printf("------- lam %d\n",(int)lam);

		int checklam = 0;

		if(lam) {

			//printf("check lam\n");
//#ifdef HAVE_CAEN_BRD
////			printf("busy ON\n");
//			busyON();
//#endif
			//loop while all lam = 1

			//#ifdef HAVE_PIX
			//      while(lam != LAM_ON && pix_running ){
			//#else
			while(lam != LAM_ON && checklam < 15 ){

				//#endif
				usleep(1000);//1000
				++checklam;
				printf("secondary Lam check\n");
				if (LAM(&lam)) {
                                  return 2;
                                }
				//if (checklam%200) {
				//printf("------- %d lam %d\n",checklam,(int)lam);
				//	  printf("data ready 0 = %d , dat aready 1 = %d \n",pix_data_ready[0],pix_data_ready[1]);
				//}
				// printf("------- %d lam %d\n",checklam,(int)lam);

			}

			//printf("done check lam\n");

			//printf("N checklam %d\n",checklam);
			//    }  //end lam

			int lam1 = lam & 0x1;
			int lam2 = (lam >> 1) & 0x1;
			int lam3 = (lam >> 2) & 0x1;
			int lam4 = (lam >> 3) & 0x1;
			int lam5 = (lam >> 4) & 0x1;
			int lam6 = (lam >> 5) & 0x1;
			int lam7 = (lam >> 6) & 0x1;

			//if(lam1) printf("\n%d %d %d %d %d %d\n",lam1,lam2,lam3,lam4,lam5,lam6);

			//if(lam1 && lam2 && lam3 && lam4 && lam5 && lam6){
			if(lam == LAM_ON){

				if (!test){

					// #ifdef HAVE_CAEN_BRD
					// 	//SET OUT_0 to 0 (busy) through OUTPUT REGISTER
					// 	data = 0x40;
					// 	CAENVME_WriteRegister(gVmeHandle,cvOutRegClear,data);
					// #endif

					/*MEMCACHE
	#ifdef HAVE_PIX
		for(Int_t ii=0;ii<NPIX;ii++) utime_fit_old[ii]=utime_fit[ii];
	#endif
					 */
//					printf("LAM FOUND!\n");
					return 1;
				}
				//  printf("%d %d %d\n",lam1,lam2,lam3);
				return 4;
			}

			if(!eor){

				printf("LAM not correct, Clear full devices\n");
//#ifdef HAVE_V1190
//				if(lam1) {
//					v1190_SoftClear(gVmeHandle,gTdcBase);
//				}
//#endif

//	#ifdef HAVE_DRS
				//	        if(lam4) {
				//	          for (int j=drs->GetNumberOfBoards()-1 ; j>=0 ; j--)
				//	            drs->GetBoard(j)->StartDomino();
				//	        }
				//	#endif


				if(lam7) {
					//	v1742_SoftClear(gVme,gDrsBase);
					printf("V1742: cleaning due to wrong lam\n");
					dgtzVars.ret = CAEN_DGTZ_ClearData(dgtzVars.handle);
					if (dgtzVars.ret) {
//						printf("V1742: Error during cleaning: "<<ErrMsg[ERR_READOUT]<<std::endl;
					} else { usleep(50); }

				}

				//#ifdef HAVE_CAEN_BRD
				if(!test && !eor){
					////        data = 0x40;
					//        CAENVME_ReadRegister(gVmeHandle,cvOutRegSet,&data);
					//        data |= 0x40;
					//        CAENVME_WriteRegister(gVmeHandle,cvOutRegSet,data);
					printf("LAM not correct, release busy signal\n");
					//          busyOFF();
				}
				//#endif

				return 3;

			}

		}  //end lam

//		/////TO BE CHECKED!!!!
//		usleep(10);
//
//	}
	return 0;
}

int readOut_loop() {

  /**
   * Readout loop.
   */
	int outStat=0;
	int ii=0;
  while(!(dgtzVars.WDrun.Quit)) {
    /**
     * Check for keyboard commands (key pressed).
     */
    checkKeyboardCommands();
    if (dgtzVars.WDrun.Restart) {
      if ( restartCall() ) { return 2; }
      continue;
    }
    if ((dgtzVars.WDrun.AcqRun) == 0) {
      continue;
    }
    char lam = 0;
    if (lamV1742(&lam)) {
      return 2;
    }
    if (lam) {
#ifdef HAVE_CAEN_BRD
//    	busyOFF();
    	busyON();
#endif
      if (readingV1742()) {
        return 2;
      }
#ifdef HAVE_CAEN_BRD
    	busyOFF();
#endif
    }
  }
  return 0;
}

void chekDevicesON(){

	deviceTypes[0]=0;
	boardIds[0]=dgtzVars.EventInfo.BoardId;

	int gr=0, k;
	int numberOfChs = sizeof(X742Chs)/sizeof(int);
	int trgChannels=0;
	for (k = 0; k < numberOfChs; ++k) {
	  gr = X742Chs[k] / 8;
	  if (gr==0 || gr ==1) { if (!grON[0]) { grON[0]=1; ++trgChannels; } }
	  if (gr==2 || gr ==2) { if (!grON[1]) { grON[1]=1; ++trgChannels;} }
	}
	channelsPerBoard[0]=numberOfChs+trgChannels;

#ifdef HAVE_V1190
	++numberOfDevices;
	deviceTypes[1]=1;
	boardIds[1]=1;
#endif

}

/**
 * Main function.
 */
int main(int argc, char *argv[]) {
  initRunVars();
  setFileNameTemplate("dch_run_%d.dat");
  dgtzVars.ret = CAEN_DGTZ_Success;
  dgtzVars.handle = -1;
  dgtzVars.ErrCode= ERR_NONE;
  dgtzVars.Nb=0;
  dgtzVars.Ne=0;
  dgtzVars.buffer = NULL;
  dgtzVars.EventPtr = NULL;
  dgtzVars.isVMEDevice = 0;
  /**
   * generic event struct with 16 bit data (10, 12, 14 and 16 bit digitizers
   */
  dgtzVars.Event16 = NULL;
  /**
   * generic event struct with 8 bit data (only for 8 bit digitizers)
   */
  dgtzVars.Event8 = NULL;
  /**
   * custom event struct with 8 bit data (only for 8 bit digitizers)
   */
  dgtzVars.Event742 = NULL;
  dgtzVars.PlotVar = NULL;
  /**
   * Init to the bigger positive number
   */
  dgtzVars.ReloadCfgStatus = 0x7FFFFFFF;
  /**
   * Frontend init.
   */
  frontend_init(argc, argv);
/* #ifdef HAVE_CAEN_BRD */
/*    busyOFF(); */
/*    DisableGlobalVeto(); */
/* #endif */
/* frontend_end(); */
/*  quit(0); */

  chekDevicesON();

#ifdef HAVE_CAEN_BRD
    EnableGlobalVeto();
#endif
    printf("front end ready\n");
    printf("[s] start/stop the acquisition, [q] quit, [SPACE] help\n");
  /**
   * Readout loop
   */
  int stat = readOut_loop();
  if (stat) { quit(2); }
  /**
   * Frontend end.
   */
  frontend_end();
  quit(0);
}
