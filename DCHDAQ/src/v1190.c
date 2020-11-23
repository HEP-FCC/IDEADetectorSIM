/*********************************************************************

  $Id: v1190.c 269 2013-10-31 17:22:20Z tassielli $
  $Author:  $
  $Revision:  $

  Created by:   Giovanni F. Tassielli
  Based on v1190B.c created by Pierre-Andre Amaudruz

  Contents:     V1190A/B 128/64ch. TDC

*********************************************************************/
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
//#include <iostream>
//#include <bitset>

#include "v1190.h"
#include <CAENVMElib.h>
#include <CAENVMEtypes.h>

/**
data modes
*/
#define MVME_DMODE_D8                 1   /**< D8 */
#define MVME_DMODE_D16                2   /**< D16 */
#define MVME_DMODE_D32                3   /**< D32 */
#define MVME_DMODE_D64                4   /**< D64 */
#define MVME_BLT_NONE                 1   /**< normal programmed IO */
#define MVME_BLT_BLT32                2   /**< 32-bit block transfer */
#define MVME_BLT_MBLT64               3   /**< multiplexed 64-bit block transfer */

/* vme bus address modifiers */

#define MVME_AM_A32_SB     (0x0F)      /**< A32 Extended Supervisory Block */
//#define MVME_AM_A32_SP     (0x0E)      /**< A32 Extended Supervisory Program */
//#define MVME_AM_A32_SD     (0x0D)      /**< A32 Extended Supervisory Data */
//#define MVME_AM_A32_NB     (0x0B)      /**< A32 Extended Non-Privileged Block */
//#define MVME_AM_A32_NP     (0x0A)      /**< A32 Extended Non-Privileged Program */
//#define MVME_AM_A32_ND     (0x09)      /**< A32 Extended Non-Privileged Data */
//#define MVME_AM_A32_SMBLT  (0x0C)      /**< A32 Multiplexed Block Transfer (D64) */
#define MVME_AM_A32_NMBLT  (0x08)      /**< A32 Multiplexed Block Transfer (D64) */

#define MVME_AM_A24_SB     (0x3F)      /**< A24 Standard Supervisory Block Transfer      */
//#define MVME_AM_A24_SP     (0x3E)      /**< A24 Standard Supervisory Program Access      */
//#define MVME_AM_A24_SD     (0x3D)      /**< A24 Standard Supervisory Data Access         */
//#define MVME_AM_A24_NB     (0x3B)      /**< A24 Standard Non-Privileged Block Transfer   */
//#define MVME_AM_A24_NP     (0x3A)      /**< A24 Standard Non-Privileged Program Access   */
//#define MVME_AM_A24_ND     (0x39)      /**< A24 Standard Non-Privileged Data Access      */
//#define MVME_AM_A24_SMBLT  (0x3C)      /**< A24 Multiplexed Block Transfer (D64) */
#define MVME_AM_A24_NMBLT  (0x38)      /**< A24 Multiplexed Block Transfer (D64) */


MVME_INTERFACE mvme;

void initMVMVE() {
    mvme.am=cvA32_S_DATA;
    mvme.dmode = MVME_DMODE_D32;
    mvme.blt_mode=MVME_BLT_BLT32;
}

int mvme_write_value(int *hvme, mvme_addr_t vme_addr, unsigned int value)
{
//   int status=0;
//   status = CAENVME_WriteCycle(*hvme, vme_addr, &value, cvA32_S_DATA, cvD32);
//
//   if (status != cvSuccess) status=0;
//   else status=1;
//
//   return status;


   int status=0, n;

   if (mvme.dmode == MVME_DMODE_D8)
      n = 1;
   else if (mvme.dmode == MVME_DMODE_D16)
      n = 2;
   else
      n = 4;

   /* D8 */
   if (mvme.dmode == MVME_DMODE_D8)
      status = CAENVME_WriteCycle(*hvme, vme_addr, &value, (CVAddressModifier)mvme.am, cvD8);
   /* D16 */
   else if (mvme.dmode == MVME_DMODE_D16)
      status = CAENVME_WriteCycle(*hvme, vme_addr, &value, (CVAddressModifier)mvme.am, cvD16);
   /* D32 */
   else if (mvme.dmode == MVME_DMODE_D32)
      status = CAENVME_WriteCycle(*hvme, vme_addr, &value, (CVAddressModifier)mvme.am, cvD32);

   if (status != cvSuccess)
      n = 0;

   return n;

}

/*------------------------------------------------------------------*/

int mvme_read(int *hvme, void *dst, mvme_addr_t vme_addr, mvme_size_t n_bytes)
{
   mvme_size_t i;
   int status=0, n;

   n = 0;

   /* D8 */
   if ((mvme.dmode == MVME_DMODE_D8) ) {
      for (i=0 ; i<n_bytes ; i++)
         status = CAENVME_ReadCycle(*hvme, vme_addr, (dst+i), (CVAddressModifier)mvme.am, cvD8);
      n = n_bytes;
   /* D16 */
   } else if (mvme.dmode == MVME_DMODE_D16) {
         /* normal I/O */
//         if (mvme.blt_mode == MVME_BLT_NONE) {
            for (i=0 ; i<(n_bytes>>1) ; i++)
               status = CAENVME_WriteCycle(*hvme, vme_addr, (dst+(i<<1)), (CVAddressModifier)mvme.am, cvD16);
            n = n_bytes;
//         /* FIFO BLT */
//         } else if ((mvme.blt_mode == MVME_BLT_BLT32FIFO) || (mvme.blt_mode == MVME_BLT_MBLT64FIFO))
//            status = CAENVME_FIFOBLTWriteCycle(hvme, vme_addr, src, n_bytes, (CVAddressModifier)mvme.am, cvD16, &n);
   } else if (mvme.dmode == MVME_DMODE_D32) {
      /* normal I/O */
//      if (mvme.blt_mode == MVME_BLT_NONE) {
         for (i=0 ; i<(n_bytes>>2) ; i++)
            status = CAENVME_ReadCycle(*hvme, vme_addr, (dst+(i<<2)), (CVAddressModifier)mvme.am, cvD32);
         n = n_bytes;
//      /* FIFO BLT */
//      } else if (mvme.blt_mode == MVME_BLT_BLT32FIFO)
//         status = CAENVME_FIFOBLTReadCycle(hvme, vme_addr, dst, n_bytes, (CVAddressModifier)mvme.am, cvD32, &n);
//      /* BLT */
//      else
//         status = CAENVME_BLTReadCycle(hvme, vme_addr, dst, n_bytes, (CVAddressModifier)mvme.am, cvD32, &n);
   }

   if ((status != cvSuccess) && (status != cvBusError))
      n = 0;

   return n;
}
/*------------------------------------------------------------------*/

unsigned int mvme_read_value(int *hvme, mvme_addr_t vme_addr)
{
   unsigned int data;
   int status=0;

   data = 0;

   /* D8 */
   if (mvme.dmode == MVME_DMODE_D8)
      status = CAENVME_ReadCycle(*hvme, vme_addr, &data, (CVAddressModifier)mvme.am, cvD8);
   /* D16 */
   else if (mvme.dmode == MVME_DMODE_D16)
      status = CAENVME_ReadCycle(*hvme, vme_addr, &data, (CVAddressModifier)mvme.am, cvD16);
   /* D32 */
   else if (mvme.dmode == MVME_DMODE_D32)
      status = CAENVME_ReadCycle(*hvme, vme_addr, &data, (CVAddressModifier)mvme.am, cvD32);

   return data;
}



///*****************************************************************/
//WORD v1190_Read16(int *hvme, DWORD base, int offset)
//{
//  mvme_set_am(mvme, MVME_AM_A24);
//  mvme_set_dmode(mvme, MVME_DMODE_D16);
//  return mvme_read_value(mvme, base+offset);
//}
//
//DWORD v1190_Read32(int *hvme, DWORD base, int offset)
//{
//  mvme_set_am(mvme, MVME_AM_A24);
//  mvme_set_dmode(mvme, MVME_DMODE_D32);
//  return mvme_read_value(mvme, base+offset);
//}
//
//void v1190_Write16(int *hvme, DWORD base, int offset, WORD value)
//{
//  mvme_set_am(mvme, MVME_AM_A24);
//  mvme_set_dmode(mvme, MVME_DMODE_D16);
//  mvme_write_value(mvme, base+offset, value);
//}
//
//int v1190_dummyWrite16(int *hvme, DWORD base, WORD value)
//{
//    int cmode, nbytes;
//
//    mvme_get_dmode(mvme, &cmode);
//    mvme_set_dmode(mvme, MVME_DMODE_D16);
//    nbytes = mvme_write_value(mvme, base+0x1204, value);
//
//    mvme_set_dmode(mvme, cmode);
//    return nbytes;
//
//}

/*****************************************************************/
/**
Read Data buffer for single event (check delimiters)
0x4... and 0xC...
@param *mvme VME structure
@param  base Module base address
@param *pdest destination pointer address
@param *nentry number of entries requested and returned.
@return
*/
int v1190_EventRead(int *hvme, DWORD base, DWORD *pdest, int *nentry)
{
  int cmode;
  DWORD hdata;
  *nentry = 0;

  cmode=mvme.dmode;
  mvme.dmode=MVME_DMODE_D32;

  if (v1190_DataReady(hvme, base)) {
      do {
          hdata = mvme_read_value(hvme, base);
          //std::cout << std::bitset<32>(hdata)<<std::endl;
      } while ( ((hdata & V1190_DATA_FLAG) != V1190_DATA_FLAG_GLHD) );

      pdest[*nentry] = hdata;
      ++(*nentry);
      DWORD flagHdata=0x00000000;
      do {
          hdata = mvme_read_value(hvme, base);
          //std::cout << std::bitset<32>(hdata)<<std::endl;
          flagHdata = hdata & V1190_DATA_FLAG;
          if ( (flagHdata == V1190_DATA_FLAG_DATA) || (flagHdata == V1190_DATA_FLAG_TDHD) || (flagHdata == V1190_DATA_FLAG_TDTR) ) {
              pdest[*nentry] = hdata;
              ++(*nentry);
          }
      } while ( (flagHdata != V1190_DATA_FLAG_GLTR) );
      pdest[*nentry] = hdata;
      ++(*nentry);
  }
  mvme.dmode= cmode;
  return *nentry;

/*
  header = *pbase & 0xF8000000;//0xFF000000;

  switch (header) {
  case 0x40000000:  // Global Header
    break;
  case 0x08000000:  // TDC Header
    break;
  case 0x00000000:  // Data
    break;
  case 0x18000000:  // TDC Trailer
    break;
  case 0x20000000:  // TDC Error
    break;
  case 0x80000000:  // Global Trailer
    break;
  case 0xC0000000:  // Filler
    break;
  case 0x88000000:  // Global Trigger Time Tag
    break;
  }

  return *nentry;
*/
}

/*****************************************************************/
/**
Read data buffer for nentry data.
@param *mvme VME structure
@param  base Module base address
@param *pdest destination pointer address
@param *nentry number of entries requested and returned.
@return
*/
int v1190_DataRead(int *hvme, DWORD base, DWORD *pdest, int nentry)
{
  int cmode, status;

    cmode=mvme.dmode;
    mvme.dmode = MVME_DMODE_D32;
    mvme.blt_mode=MVME_BLT_BLT32;

    status = mvme_read(hvme, pdest, base, sizeof(DWORD) * nentry);

    mvme.dmode = cmode;
    return nentry;

    /*
      for (i=0 ; i<nentry ; i++) {
      if (!v1190_DataReady(mvme, base))
      break;
      pdest[i] = mvme_read_value(mvme, base);
      }
      mvme_set_dmode(mvme, cmode);
      return i;
    */
}

///*****************************************************************/
//int v1190_GeoWrite(int *hvme, DWORD base, int geo)
//{
//  int cmode, data;
//
//  mvme_get_dmode(mvme, &cmode);
//  mvme_set_dmode(mvme, MVME_DMODE_D16);
//  mvme_write_value(mvme, base+V1190_GEO_REG_RW, (geo & 0x1F));
//  data = mvme_read_value(mvme, base+V1190_GEO_REG_RW);
//  mvme_set_dmode(mvme, cmode);
//
//  return (int) (data & 0x1F);
//}

///*****************************************************************/
//void v1190_SoftReset(int *hvme, DWORD base)
//{
//  int cmode;
//
//  mvme_get_dmode(mvme, &cmode);
//  mvme_set_dmode(mvme, MVME_DMODE_D16);
//  mvme_write_value(mvme, base+V1190_MODULE_RESET_WO, 0);
//  mvme_set_dmode(mvme, cmode);
//}

/*****************************************************************/
void v1190_SoftClear(int *hvme, DWORD base)
{
  int cmode;

  cmode=mvme.dmode;
  mvme.dmode = MVME_DMODE_D16;
  mvme_write_value(hvme, base+V1190_SOFT_CLEAR_WO, 0);
  mvme.dmode=cmode;
}

///*****************************************************************/
//void v1190_SoftTrigger(int *hvme, DWORD base)
//{
//  int cmode;
//
//  mvme_get_dmode(mvme, &cmode);
//  mvme_set_dmode(mvme, MVME_DMODE_D16);
//  mvme_write_value(mvme, base+V1190_SOFT_TRIGGER_WO, 0);
//  mvme_set_dmode(mvme, cmode);
//}

/*****************************************************************/
int  v1190_DataReady(int *hvme, DWORD base)
{
  int cmode, data;

  cmode=mvme.dmode;
  mvme.dmode = MVME_DMODE_D16;

  data = mvme_read_value(hvme, base+V1190_SR_RO);

  mvme.dmode=cmode;
  return (data & V1190_DATA_READY);

  //return data;
}

/*****************************************************************/
int  v1190_EvtStored(int *hvme, DWORD base)
{
  int cmode, data;

  cmode=mvme.dmode;
  mvme.dmode = MVME_DMODE_D16;

  data = mvme_read_value(hvme, base+V1190_EVT_STORED_RO);
  mvme.dmode=cmode;
  return (data);
}

///*****************************************************************/
//int  v1190_EvtCounter(MVME_INTERFACE *mvme, DWORD base)
//{
//  int cmode, data;
//
//  mvme_get_dmode(mvme, &cmode);
//  mvme_set_dmode(mvme, MVME_DMODE_D32);
//  data = mvme_read_value(mvme, base+V1190_EVT_CNT_RO);
//  mvme_set_dmode(mvme, cmode);
//  return (data);
//}

///*****************************************************************/
//void v1190_TdcIdList(MVME_INTERFACE *mvme, DWORD base, V1190Type type)
//{
//  int  cmode, i, code;
//  DWORD value;
//
//  mvme_get_dmode(mvme, &cmode);
//  mvme_set_dmode(mvme, MVME_DMODE_D16);
//
//  int nBlock = 2;
//  if (type==V1190A) {nBlock=4;}
//  for (i=0; i<nBlock ; i++) {
//    code   = V1190_MICRO_TDCID | (i & 0x0F);
//    value  = v1190_MicroWrite(mvme, base, code);
//    value  = v1190_MicroRead(mvme, base);
//    value = (v1190_MicroRead(mvme, base) << 16) | value;
//    //    printf("Received :code: 0x%04x  0x%08lx\n", code, value);
//  }
//  mvme_set_dmode(mvme, cmode);
//}

///*****************************************************************/
//int  v1190_ResolutionRead(MVME_INTERFACE *mvme, DWORD base, V1190Type type)
//{
//  int   cmode, value, edmode, expo;
//  WORD le, width;
//  le=width=0x0;
//
//  mvme_get_dmode(mvme, &cmode);
//  mvme_set_dmode(mvme, MVME_DMODE_D16);
//
//    // printf("Received RR :code: 0x%04x  0x%08x\n", code, value);
//  value = v1190_MicroWrite(mvme, base, (WORD) (0x2300));
//  edmode = v1190_MicroRead(mvme, base);
//  value = v1190_MicroWrite(mvme, base, V1190_RESOLUTION_RO);
//  value = v1190_MicroRead(mvme, base);
//
//  if (edmode==(int)V1190_ED_Pair) {
//      le = value & 0x7;
//      width = value>>8;
//      width = width & 0xF;
//      printf("L Edge Detection resolution of %f ps, Width Detection resolution of %f ps\n",(pow(2,le)*100.0),(pow(2,width)*100.0));
//  }
//  else if (edmode==(int)V1190_ED_TRandLE) {
//      le = le & 0x7;
//      width = value>>8;
//      width = width & 0x7;
//      printf("L Edge Detection resolution of %f ps, T Edge Detection resolution of %f ps\n",(pow(2,le)*100.0),(pow(2,width)*100.0));
//  }
//  else {
//      le = value & 0x3;
//      if (le==1 || le==2) {expo = 2-le;}
//      else {expo = 3;}
//      printf("Edge Detection resolution of %f ps\n",(pow(2,expo)*100.0));
//  }
//
//  mvme_set_dmode(mvme, cmode);
//  return value;
//}

/*****************************************************************/
void v1190_EdgResolutionSet(int *hvme, DWORD base, V1190_EDGE_RESOL le)
{
  int   cmode, value;

  cmode=mvme.dmode;
  mvme.dmode = MVME_DMODE_D16;

  if ((le == V1190_ED_Res_100PS) ||
      (le == V1190_ED_Res_200PS) ||
      (le == V1190_ED_Res_800PS)) {
    //printf("le:%x\n", le);
    value = v1190_MicroWrite(hvme, base, V1190_EDGE_RESOLUTION_WO);
    value = v1190_MicroWrite(hvme, base, (WORD)le);
  } else {
    printf("Wrong Leading Edge Resolution -> Set at default (100ps)\n");
    value = v1190_MicroWrite(hvme, base, V1190_EDGE_RESOLUTION_WO);
    value = v1190_MicroWrite(hvme, base, V1190_ED_Res_100PS);
  }
  mvme.dmode=cmode;
}

///*****************************************************************/
//void v1190_LEWResolutionSet(MVME_INTERFACE *mvme, DWORD base, WORD le, WORD width)
//{
//  //printf("Not yet implemented\n");
//  int   cmode, value;
//  WORD setVal=0x0;
//
//  mvme_get_dmode(mvme, &cmode);
//  mvme_set_dmode(mvme, MVME_DMODE_D16);
//  value = v1190_MicroWrite(mvme, base, (WORD) (0x2300));
//  value = v1190_MicroRead(mvme, base);
//  if (value==(int)V1190_ED_Pair) {
//      le = le & 0x7;
//      width = width & 0xF;
//      if (width>13) {width=13; printf("Wrong width resolution selected, set to max (800ns)!\n");}
//      setVal = le+(width<<8);
//  }
//  else if (value==(int)V1190_ED_TRandLE) {
//      le = le & 0x7;
//      width = width & 0x7;
//      setVal = le+(width<<8);
//  }
//  else {
//      printf("Edge Detection configuration is set to a single edge detection mode -> Use v1190_EdgResolutionSet instead\n");
//      mvme_set_dmode(mvme, cmode);
//      return;
//  }
//
//  value = v1190_MicroWrite(mvme, base, V1190_LEW_RESOLUTION_WO);
//  value = v1190_MicroWrite(mvme, base, setVal);
//  mvme_set_dmode(mvme, cmode);
//}

///*****************************************************************/
//void v1190_AcqModeRead(MVME_INTERFACE *mvme, DWORD base)
//{
//  int   cmode, value;
//
//  mvme_get_dmode(mvme, &cmode);
//  mvme_set_dmode(mvme, MVME_DMODE_D16);
//
//  value = v1190_MicroWrite(mvme, base, V1190_ACQ_MODE_RO);
//  value = v1190_MicroRead(mvme, base);
//  //  printf("Received AR :code: 0x%04x  0x%08x\n", V1190_ACQ_MODE_RO, value);
//  mvme_set_dmode(mvme, cmode);
//}

/*****************************************************************/
void v1190_TriggerMatchingSet(int *hvme, DWORD base)
{
  int   cmode, value;

  cmode=mvme.dmode;
  mvme.dmode = MVME_DMODE_D16;

  value = v1190_MicroWrite(hvme, base, V1190_TRIGGER_MATCH_WO);
  printf("Received MS :code: 0x%04x  0x%08x\n", V1190_TRIGGER_MATCH_WO, value);

  mvme.dmode=cmode;
}

/*****************************************************************/
void v1190_SubtractTriggerTimeON(int *hvme, DWORD base)
{
  int   cmode, value;

  cmode=mvme.dmode;
  mvme.dmode = MVME_DMODE_D16;

  value = v1190_MicroWrite(hvme, base, (WORD) 0x1400);

  mvme.dmode=cmode;
}

///*****************************************************************/
//void v1190_SubtractTriggerTimeOFF(MVME_INTERFACE *mvme, DWORD base)
//{
//  int   cmode, value;
//
//  mvme_get_dmode(mvme, &cmode);
//  mvme_set_dmode(mvme, MVME_DMODE_D16);
//
//  value = v1190_MicroWrite(mvme, base, (WORD) 0x1500);
//
//  mvme_set_dmode(mvme, cmode);
//}

///*****************************************************************/
//void v1190_ContinuousSet(MVME_INTERFACE *mvme, DWORD base)
//{
//  int   cmode, value;
//
//  mvme_get_dmode(mvme, &cmode);
//  mvme_set_dmode(mvme, MVME_DMODE_D16);
//
//  value = v1190_MicroWrite(mvme, base, V1190_CONTINUOUS_WO);
//  //  printf("Received CS :code: 0x%04x  0x%08x\n", V1190_CONTINUOUS_WO, value);
//
//  mvme_set_dmode(mvme, cmode);
//}

/*****************************************************************/
/**
Set the width of the matching Window. The width parameter should be
in the range of 1 to 4095 (0xFFF). Example 0x14 == 500ns.
@param *mvme VME structure
@param  base Module base address
@param width window width in ns units
@return
*/
void v1190_WidthSet(int *hvme, DWORD base, WORD width)
{
  int   cmode, value;

  cmode=mvme.dmode;
  mvme.dmode = MVME_DMODE_D16;

  //  v1190_MicroFlush(hvme, base);
  value = v1190_MicroWrite(hvme, base, V1190_WINDOW_WIDTH_WO);
  value = v1190_MicroWrite(hvme, base, (WORD) (width&0x0FFF));
  //  printf("Received WS :code: 0x%04x  0x%08x\n", V1190_WINDOW_WIDTH_WO, value);

  mvme.dmode=cmode;
}

/*****************************************************************/
void v1190_WidthSet_ns(int *hvme, DWORD base, unsigned int width)
{
    if (width>102375) { width=102375; }
    WORD widthVal = width/V1190_NS_PER_TDC_COUNT;
    v1190_WidthSet(hvme,base,widthVal);
}

/*****************************************************************/
/**
Set the offset of the matching window with respect to the trigger.
The offset parameter should be in 25ns units. The range is
from -2048(0x800) to +40(0x28). Example 0xFE8 == -600ns.
@param *mvme VME structure
@param  base Module base address
@param  offset offset in ns units
*/
void v1190_OffsetSet(int *hvme, DWORD base, WORD offset)
{
  int   cmode, value;
  //printf("Calling method v1190_OffsetSet\n");
  cmode=mvme.dmode;
  mvme.dmode = MVME_DMODE_D16;

  //  v1190_MicroFlush(mvme, base);
  value = v1190_MicroWrite(hvme, base, V1190_WINDOW_OFFSET_WO);
  value = v1190_MicroWrite(hvme, base, offset);
  //  printf("Received OS :code: 0x%04x  0x%08x\n", V1190_WINDOW_OFFSET_WO, value);

  mvme.dmode=cmode;
}

/*****************************************************************/
void v1190_OffsetSet_ns(int *hvme, DWORD base, int offset) {
    WORD offVal=0x0000;
    if (offset<-51200) {offVal=0x800;}
    else if (offset>1000) {offVal=0x28;}
    else { offVal=(4096+offset/((int) V1190_NS_PER_TDC_COUNT))&4095; }
    v1190_OffsetSet(hvme, base, offVal);

}

/*****************************************************************/
void v1190_SetEdgeDetection(int *hvme, DWORD base, V1190_EDGE_MODE ed)
{
  int   cmode, value;
  //printf("Calling method v1190_SetEdgeDetection\n");

  cmode=mvme.dmode;
  mvme.dmode = MVME_DMODE_D16;

  if ((ed == V1190_ED_Pair) ||
      (ed == V1190_ED_Trailing) ||
      (ed == V1190_ED_Leading) ||
      (ed == V1190_ED_TRandLE)) {
    value = v1190_MicroWrite(hvme, base, V1190_EDGE_DETECTION_WO);
    WORD edVal = ed;
    value = v1190_MicroWrite(hvme, base, edVal);
  } else {
    //printf("Wrong Edge Detection configuration -> Set to Pair\n");
    value = v1190_MicroWrite(hvme, base, V1190_EDGE_DETECTION_WO);
    value = v1190_MicroWrite(hvme, base, (WORD)V1190_ED_Pair);
  }
  mvme.dmode=cmode;
}

/*****************************************************************/
int v1190_MicroWrite(int *hvme, DWORD base, WORD data)
{
  int cmode, i;

  cmode=mvme.dmode;
  mvme.dmode = MVME_DMODE_D16;

  for (i=0; i<5000; i++)
  {
    WORD microHS = mvme_read_value(hvme, base+V1190_MICRO_HAND_RO);
    //printf("read microHS: %i\n",microHS);
    if ( (microHS & V1190_MICRO_WR_OK) ) {
      data &= 0xFFFF;
      //printf("writing OPCODE: 0x%04x\n",data);
      mvme_write_value(hvme, base+V1190_MICRO_RW, data);
      mvme.dmode=cmode;
      return 1;
    }
    if (udelay(500)==-1) {udelay(500);}
  }

  printf("v1190_MicroWrite: Micro not ready for writing!\n");
  mvme.dmode=cmode;
  return -1;
}

/*****************************************************************/
int v1190_MicroRead(int *hvme, const DWORD base)
{
  int cmode, i;
  int reg=-1;

  cmode=mvme.dmode;
  mvme.dmode = MVME_DMODE_D16;
  for (i=500; i>0; i--) {
    WORD  microHS = mvme_read_value(hvme, base+V1190_MICRO_HAND_RO);
    if (microHS & V1190_MICRO_RD_OK) {
      reg = mvme_read_value(hvme, base+V1190_MICRO_RW);
      //printf("i:%d microHS:%d 0x%04x\n", i, microHS, reg);
      mvme.dmode=cmode;
      return (reg);
    }
    if (udelay(500)==-1) {udelay(500);}
  };
  mvme.dmode=cmode;
  return -1;
}

///*****************************************************************/
//int v1190_MicroFlush(MVME_INTERFACE *mvme, const DWORD base)
//{
//  int cmode, count = 0;
//
//  mvme_get_dmode(mvme, &cmode);
//  mvme_set_dmode(mvme, MVME_DMODE_D16);
//
//  while (1)
//    {
//      int data = v1190_MicroRead(mvme, base);
//      printf("microData[%d]: 0x%04x\n",count,data);
//      if (data < 0)
//  break;
//      count++;
//    }
//  mvme_set_dmode(mvme, cmode);
//  return count;
//}

/*****************************************************************/
/**
Sets all the necessary paramters for a given configuration.
The configuration is provided by the mode argument.
Add your own configuration in the case statement. Let me know
your setting if you want to include it in the distribution.
@param *mvme VME structure
@param  base Module base address
@param mode  Configuration mode number
@param *nentry number of entries requested and returned.
@return MVME_SUCCESS
*/
//int  v1190_Setup(MVME_INTERFACE *mvme, DWORD base, int mode)
//{
//  WORD code, value;
//  int      cmode, status = -1;
//
//  mvme_get_dmode(mvme, &cmode);
//  mvme_set_dmode(mvme, MVME_DMODE_D16);
//
////  v1190_MicroFlush(mvme, base);
//  switch (mode) {
//  case 0x1:
//    printf("Trigger Matching Setup (mode:%d)\n", mode);
//    printf("Default setting + Width : 800ns, Offset : -400ns\n");
//    printf("Time subtract, Leading Edge only\n");
//    code = 0x0000;  // Trigger matching Flag
//    if ((status = v1190_MicroWrite(mvme, base, code)) < 0)
//      return status;
//    code = 0x1000;  // Width
//    value = v1190_MicroWrite(mvme, base, code);
//    value = v1190_MicroWrite(mvme, base, 0x20);   // Width : 800ns
//    code = 0x1100;  // Offset
//    value = v1190_MicroWrite(mvme, base, code);
//    value = v1190_MicroWrite(mvme, base, 0xfe8);  // offset: -400ns
//    code = 0x1500;  // Subtraction flag
//    value = v1190_MicroWrite(mvme, base, code);
//    code = 0x2100;  // Leading Edge Detection
//    value = v1190_MicroWrite(mvme, base, code);
//    break;
//  case 0x2:
//    code = 0x0500;  // Default configuration
//    value = v1190_MicroWrite(mvme, base, code);
//    break;
//  default:
//    printf("Unknown setup mode\n");
//    mvme_set_dmode(mvme, cmode);
//    return -1;
//  }
//  v1190_Status(mvme, base);
//  mvme_set_dmode(mvme, cmode);
//  return 0;
//}

/*****************************************************************/
/**
Read and display the curent status of the TDC.
@param *mvme VME structure
@param  base Module base address
@return MVME_SUCCESS, MicroCode error
*/
int v1190_Status(int *hvme, DWORD base, V1190Type type)
{
  WORD  i, code, pair=0;
  int   cmode, value;
  int ns = V1190_NS_PER_TDC_COUNT; // ns per tdc count

  cmode=mvme.dmode;
  mvme.dmode = MVME_DMODE_D16;

  //-------------------------------------------------
  printf("\n");
  printf("V1190 TDC at VME ");
  if (mvme.am>=MVME_AM_A32_NMBLT && mvme.am<=MVME_AM_A32_SB) { printf("A32"); }
  if (mvme.am>=MVME_AM_A24_NMBLT && mvme.am<=MVME_AM_A24_SB) { printf("A24"); }
  printf(" 0x%06x:\n", base);
  printf("\n--- Trigger Section [0x1600]:\n");
  code = 0x1600;
  if ((value = v1190_MicroWrite(hvme, base, code)) < 0)
    return -value;

  value = v1190_MicroRead(hvme, base);
  printf("  Match Window width       : 0x%04x, %d ns\n", value, value*ns);
  value = v1190_MicroRead(hvme, base);
  printf("  Window offset            : 0x%04x, %d ns\n", value, ((value&0x0FFF)-4096)*ns);
  value = v1190_MicroRead(hvme, base);
  printf("  Extra Search Window Width: 0x%04x, %d ns\n", value, value*ns);
  value = v1190_MicroRead(hvme, base);
  printf("  Reject Margin            : 0x%04x, %d ns\n", value, value*ns);
  value = v1190_MicroRead(hvme, base);
  printf("  Trigger Time subtraction : %s\n",(value & 0x1) ? "y" : "n");

  //-------------------------------------------------
  printf("\n--- Edge Detection & Resolution Section[0x2300/26/29]:\n");
  code = 0x2300;
  value = v1190_MicroWrite(hvme, base, code);
  pair = value = v1190_MicroRead(hvme, base);
  printf("  Edge Detection (1:T/2:L/3:TL)           : 0x%02x\n", (value&0x3));
  code = 0x2600;
  value = v1190_MicroWrite(hvme, base, code);
  value = v1190_MicroRead(hvme, base);
  if (pair==0x3) {
    value = v1190_MicroRead(hvme, base);
    printf("  Leading Edge Resolution (see table)     : 0x%02x\n", (value&0x3));
    printf("  Pulse Width Resolution (see table)      : 0x%02x\n", ((value>>8)&0xF));
  } else {
    printf("  Resolution [ps] (0:800/1:200/2:100)     : 0x%02x\n", (value&0x3));
  }
  code = 0x2900;
  value = v1190_MicroWrite(hvme, base, code);
  value = v1190_MicroRead(hvme, base);
  printf("  Dead Time between hit [~ns](5/10/30/100): 0x%02x\n", (value&0x3));

  //-------------------------------------------------
  printf("\n--- Readout Section[0x3200/34/3a/3c]:\n");
  code = 0x3200;
  value = v1190_MicroWrite(hvme, base, code);
  value = v1190_MicroRead(hvme, base);
  printf("  Header/Trailer                            : %s\n",(value & 0x1) ? "y" : "n");
  code = 0x3400;
  value = v1190_MicroWrite(hvme, base, code);
  value = v1190_MicroRead(hvme, base);
  value = v1190_MicroWrite(hvme, base, code);
  value = v1190_MicroRead(hvme, base);
  printf("  Max #hits per event 2^(n-1) (>128:no limit) : n=%i\n", value&0xF);
  code = 0x3a00;
  value = v1190_MicroWrite(hvme, base, code);
  value = v1190_MicroRead(hvme, base);
  printf("  Internal TDC error type (see doc)         : 0x%04x\n", (value&0x7FF));
  code = 0x3c00;
  value = v1190_MicroWrite(hvme, base, code);
  value = v1190_MicroRead(hvme, base);
  printf("  Effective size of readout Fifo 2^(n+1)      : n=%i\n", (value&0xF));

  //-------------------------------------------------
  printf("\n--- Channel Enable Section[0x4500/47/49]:\n");

  int nBlock = 2;
  if (type==V1190A) {nBlock=4;}

  code = 0x4500;
  value = v1190_MicroWrite(hvme, base, code);
  for (i=0; i<2*nBlock ; i++) {
    value = v1190_MicroRead(hvme, base);
    printf("  Read Enable Pattern [  %i..%i ] : 0x%04x\n",(16*i),(16*(i+1)-1), value);
  }
  code = 0x4700;
  for (i=0; i<nBlock ; i++) {
    value = v1190_MicroWrite(hvme, base, code+i);
    value = v1190_MicroRead(hvme, base);
    value = (v1190_MicroRead(hvme, base)<<16) | value;
    printf("  Read Enable Pattern 32 (%i) : 0x%08x\n",i, value);
  }

  //-------------------------------------------------
  printf("\n--- Adjust Section[0x5100/60]:\n");
  code = 0x5100;
  value = v1190_MicroWrite(hvme, base, code);
  value = v1190_MicroRead(hvme, base);
  printf("  Coarse Counter Offset: 0x%04x\n", (value&0x7FF));
  value = v1190_MicroRead(hvme, base);
  printf("  Fine   Counter Offset: 0x%04x\n", (value&0x1F));
  printf("\nMiscellaneous Section:\n");
  for (i=0; i<nBlock ; i++) {
    code = 0x6000 | (i & 0x0F);
    value = v1190_MicroWrite(hvme, base, code);
    value = v1190_MicroRead(hvme, base);
    value = (v1190_MicroRead(hvme, base) << 16) | value;
    printf("  TDC ID(%i)  0x%08x  [code:0x%04x]\n", i, value, code);
  }

  //-------------------------------------------------
  printf("\n--- Bus Error Enable? : %s\n",(v1190_SatusBusError(hvme, base)) ? "y" : "n");

  mvme.dmode=cmode;
  return 0;
}

//void v1190_SetDuoubleHitsRes(MVME_INTERFACE *mvme, DWORD base, V1190_HITS_DEAD_TIME resol){
//    int cmode = 0;
//    mvme_get_dmode(mvme, &cmode);
//    mvme_set_dmode(mvme, MVME_DMODE_D16);
//    v1190_MicroWrite(mvme, base, (WORD) (0x2800));
//    v1190_MicroWrite(mvme, base, (WORD) resol);
//    mvme_set_dmode(mvme, cmode);
//}

void v1190_SetDuoubleHitsRes_ns(int *hvme, DWORD base, int resol){
    int cmode = 0;
    WORD value = 0x0000;

    cmode=mvme.dmode;
    mvme.dmode = MVME_DMODE_D16;

    switch (resol) {
    case 5:
        value = 0;
        break;
    case 10:
        value = 1;
        break;
    case 30:
        value = 2;
        break;
    case 100:
        value = 3;
        break;
    default:
        printf("Unknown resolution value, set it to default (5ns)!\n");
        value = 0;
        break;
    }

    v1190_MicroWrite(hvme, base, (WORD) (0x2800));
    v1190_MicroWrite(hvme, base, value);
    mvme.dmode=cmode;
}

void v1190_SetMaxNOfHitsPerEve(int *hvme, DWORD base, V1190_MAXHITS_PEREVENT maxHits){
    int cmode = 0;
    cmode=mvme.dmode;
    mvme.dmode = MVME_DMODE_D16;
    v1190_MicroWrite(hvme, base, (WORD) (0x3300));
    v1190_MicroWrite(hvme, base, (WORD) maxHits);
    mvme.dmode=cmode;
}

//void v1190_SetMaxNOfHitsPerEve_absn(MVME_INTERFACE *mvme, DWORD base, int maxHits){
//    int cmode = 0;
//    WORD value = 0x0000;
//
//    mvme_get_dmode(mvme, &cmode);
//    mvme_set_dmode(mvme, MVME_DMODE_D16);
//
//    switch (maxHits) {
//    case -1:
//        value = 9;
//        break;
//    case 0:
//        value = 0;
//        break;
//    case 1:
//        value = 1;
//        break;
//    case 2:
//        value = 2;
//        break;
//    case 4:
//        value = 3;
//        break;
//    case 8:
//        value = 4;
//        break;
//    case 16:
//        value = 5;
//        break;
//    case 32:
//        value = 6;
//        break;
//    case 64:
//        value = 7;
//        break;
//    case 128:
//        value = 8;
//        break;
//    default:
//        printf("Unknown max number of hits value, set it to default (unlimited)!\n");
//        value = 9;
//        break;
//    }
//    value=5;
//    v1190_MicroWrite(mvme, base, (WORD) (0x3300));
//    v1190_MicroWrite(mvme, base, value);
//    mvme_set_dmode(mvme, cmode);
//}

//void v1190_SetEffFIFO(MVME_INTERFACE *mvme, DWORD base, V1190_FIFO_SIZE maxWords) {
//    int cmode = 0;
//    mvme_get_dmode(mvme, &cmode);
//    mvme_set_dmode(mvme, MVME_DMODE_D16);
//    v1190_MicroWrite(mvme, base, (WORD) (0x3B00));
//    v1190_MicroWrite(mvme, base, (WORD) maxWords);
//    mvme_set_dmode(mvme, cmode);
//}

//void v1190_SetEffFIFO_absn(MVME_INTERFACE *mvme, DWORD base, int maxWords) {
//    int cmode = 0;
//    WORD value = 0x0000;
//
//    mvme_get_dmode(mvme, &cmode);
//    mvme_set_dmode(mvme, MVME_DMODE_D16);
//
//    switch (maxWords) {
//    case -1:
//        value = 7;
//        break;
//    case 2:
//        value = 0;
//        break;
//    case 4:
//        value = 1;
//        break;
//    case 8:
//        value = 2;
//        break;
//    case 16:
//        value = 3;
//        break;
//    case 32:
//        value = 4;
//        break;
//    case 64:
//        value = 5;
//        break;
//    case 128:
//        value = 6;
//        break;
//    case 256:
//        value = 7;
//        break;
//    default:
//        printf("Unknown effective size of readout FIFO value, set it to default (256 words)!\n");
//        value = 7;
//        break;
//    }
//
//    v1190_MicroWrite(mvme, base, (WORD) (0x3B00));
//    v1190_MicroWrite(mvme, base, value);
//    mvme_set_dmode(mvme, cmode);
//}

void v1190_EnableERRORMark(int *hvme, DWORD base) {
    int cmode = 0;
    cmode=mvme.dmode;
    mvme.dmode = MVME_DMODE_D16;
    v1190_MicroWrite(hvme, base, (WORD) (0x3500));
    mvme.dmode=cmode;
}

//void v1190_DisableERRORMark(MVME_INTERFACE *mvme, DWORD base){
//    int cmode = 0;
//    mvme_get_dmode(mvme, &cmode);
//    mvme_set_dmode(mvme, MVME_DMODE_D16);
//    v1190_MicroWrite(mvme, base, (WORD) (0x3600));
//    mvme_set_dmode(mvme, cmode);
//}

void v1190_EnableHeaderAndTrailer(int *hvme, DWORD base) {
    int cmode = 0;
    cmode=mvme.dmode;
    mvme.dmode = MVME_DMODE_D16;
    v1190_MicroWrite(hvme, base, (WORD) (0x3000));
    mvme.dmode=cmode;
}

//void v1190_DisableHeaderAndTrailer(MVME_INTERFACE *mvme, DWORD base){
//    int cmode = 0;
//    mvme_get_dmode(mvme, &cmode);
//    mvme_set_dmode(mvme, MVME_DMODE_D16);
//    v1190_MicroWrite(mvme, base, (WORD) (0x3100));
//    mvme_set_dmode(mvme, cmode);
//}

//int v1190_StatusHeaderAndTrailer(MVME_INTERFACE *mvme, DWORD base){
//    int cmode = 0, value;
//    mvme_get_dmode(mvme, &cmode);
//    mvme_set_dmode(mvme, MVME_DMODE_D16);
//    v1190_MicroWrite(mvme, base, (WORD) (0x3200));
//    value = v1190_MicroRead(mvme, base);
//    mvme_set_dmode(mvme, cmode);
//    value &=0x1;
//    return value;
//}

//void v1190_EnableBusError(MVME_INTERFACE *mvme, DWORD base) {
//    int cmode = 0;
//    mvme_get_dmode(mvme, &cmode);
//    mvme_set_dmode(mvme, MVME_DMODE_D16);
//    WORD CtrlReg = mvme_read_value(mvme, base+V1190_REG_BASE);
//    if (!(CtrlReg & 0x0001)) {
//        CtrlReg+=0x0001;
//        mvme_write_value(mvme, base+V1190_REG_BASE, CtrlReg);
//    }
//    mvme_set_dmode(mvme, cmode);
//}

//void v1190_DisableBusError(MVME_INTERFACE *mvme, DWORD base){
//    int cmode = 0;
//    mvme_get_dmode(mvme, &cmode);
//    mvme_set_dmode(mvme, MVME_DMODE_D16);
//    WORD CtrlReg = mvme_read_value(mvme, base+V1190_REG_BASE);
//    if ((CtrlReg & 0x0001)) {
//        CtrlReg-=0x0001;
//        mvme_write_value(mvme, base+V1190_REG_BASE, CtrlReg);
//    }
//    mvme_set_dmode(mvme, cmode);
//}

int v1190_SatusBusError(int *hvme, DWORD base){
    int cmode = 0;
    cmode=mvme.dmode;
    mvme.dmode = MVME_DMODE_D16;
    WORD CtrlReg = mvme_read_value(hvme, base+V1190_REG_BASE);
    CtrlReg &= 0x0001;
    mvme.dmode=cmode;
    return CtrlReg;
}

/*****************************************************************/
#ifndef HAVE_UDELAY
// this is the VMIC version of udelay()
int udelay(int usec)
{
    struct timespec tim;//, tim2;
    tim.tv_sec = 0;
    tim.tv_nsec = usec*1000;

    if(nanosleep(&tim , NULL/*&tim2*/) < 0 )
    {
        //printf("Nano sleep system call failed \n");
        return -1;
    }
    return 1;
    //  int i, j, k = 0;
    //  for (i=0; i<133; i++)
    //    for (j=0; j<usec; j++)
    //      k += (k+1)*j*k*(j+1);
    //  return k;
}
#endif

/********************************************************************/
///*-PAA- For test purpose only */
//#ifdef MAIN_ENABLE
//int main () {
//
//  MVME_INTERFACE *myvme;
//
//  DWORD VMEIO_BASE = 0x780000;
//  DWORD V1190_BASE = 0xF10000;
//  int status, csr, i;
//  DWORD    cnt, array[10000];
//
//
//  // Test under vmic
//  status = mvme_open(&myvme, 0);
//
//  // Set am to A24 non-privileged Data
//  mvme_set_am(myvme, MVME_AM_A24_ND);
//
//  // Set dmode to D16
//  mvme_set_dmode(myvme, MVME_DMODE_D16);
//
//  // Get Firmware revision
//  csr = mvme_read_value(myvme, V1190_BASE+V1190_FIRM_REV_RO);
//  printf("Firmware revision: 0x%x\n", csr);
//
//  // Print Current status
//  v1190_Status(myvme, V1190_BASE);
//
//  // Set mode 1
//  // v1190_Setup(myvme, V1190_BASE, 1);
//
//  csr = v1190_DataReady(myvme, V1190_BASE);
//  printf("Data Ready: 0x%x\n", csr);
//
//  // Read Event Counter
//  cnt = v1190_EvtCounter(myvme, V1190_BASE);
//  printf("Event counter: 0x%lx\n", cnt);
//
//  memset(array, 0, sizeof(array));
//
//  // Set dmode to D32
//  mvme_set_dmode(myvme, MVME_DMODE_D32);
//
//  // Set 0x3 in pulse mode for timing purpose
//  mvme_write_value(myvme, VMEIO_BASE+0x8, 0xF);
//
//  // Write pulse for timing purpose
//  mvme_write_value(myvme, VMEIO_BASE+0xc, 0x2);
//
//   mvme_set_blt(myvme, MVME_BLT_BLT32);
//
//  // Read Data
//  v1190_DataRead(myvme, V1190_BASE, array, 500);
//
//  // Write pulse for timing purpose
//  mvme_write_value(myvme, VMEIO_BASE+0xc, 0x8);
//
//  for (i=0;i<12;i++)
//    printf("Data[%i]=0x%lx\n", i, array[i]);
//
//  memset(array, 0, sizeof(array));
//
//  // Event Data
//  /*
//  status = 10;
//  v1190_EventRead(myvme, V1190_BASE, array, &status);
//  printf("count: 0x%x\n", status);
//  for (i=0;i<12;i++)
//    printf("Data[%i]=0x%x\n", i, array[i]);
//  */
//
//  status = mvme_close(myvme);
//  return 0;
//}
//#endif

