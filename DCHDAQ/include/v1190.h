/*********************************************************************

  $Id: v1190.h 270 2013-10-31 17:26:02Z tassielli $
  $Author:  $
  $Revision:  $

  Created by:   Giovanni F. Tassielli
  Based on v1190B.h created by Pierre-Andre Amaudruz

  Contents:     V1190A/B 128/64ch. TDC

*********************************************************************/

//#include "mvmestd.h"

#ifndef V1190B_INCLUDE_H
#define V1190B_INCLUDE_H

/*---- interface structure -----------------------------------------*/
typedef struct {
   int  am;                  /**< Address modifier */
   int  dmode;               /**< Data mode (D8,D16,D32,D64) */
   int  blt_mode;            /**< Block transfer mode */
} MVME_INTERFACE;

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int mvme_addr_t;
typedef unsigned int mvme_size_t;
typedef unsigned short int WORD;
typedef unsigned int DWORD;


typedef enum { V1190B=0, V1190A=1 } V1190Type;

/* V1190 parameters */
//#define  V1190_MAX_CHANNELS      (DWORD) 64
#define  V1190_NS_PER_TDC_COUNT  (DWORD) 25
/* V1190 Base address */
#define  V1190_REG_BASE          (DWORD) (0x1000)
#define  V1190_CR_RW             (DWORD) (0x1000)
#define  V1190_SR_RO             (DWORD) (0x1002)
#define  V1190_DATA_READY        (DWORD) (0x0001)
#define  V1190_ALMOST_FULL       (DWORD) (0x0002)
#define  V1190_FULL              (DWORD) (0x0004)
#define  V1190_TRIGGER_MATCH     (DWORD) (0x0008)
#define  V1190_HEADER_ENABLE     (DWORD) (0x0010)
#define  V1190_GEO_REG_RW        (DWORD) (0x001E)
#define  V1190_MODULE_RESET_WO   (DWORD) (0x1014)
#define  V1190_SOFT_CLEAR_WO     (DWORD) (0x1016)
#define  V1190_SOFT_EVT_RESET_WO (DWORD) (0x1018)
#define  V1190_SOFT_TRIGGER_WO   (DWORD) (0x101A)
#define  V1190_EVT_CNT_RO        (DWORD) (0x101C)
#define  V1190_EVT_STORED_RO     (DWORD) (0x1020)
#define  V1190_FIRM_REV_RO       (DWORD) (0x1026)
#define  V1190_MICRO_HAND_RO     (DWORD) (0x1030)
#define  V1190_MICRO_RW          (DWORD) (0x102E)
/* Micro code IDs */
#define  V1190_WINDOW_WIDTH_WO   (WORD) (0x1000)
#define  V1190_WINDOW_OFFSET_WO  (WORD) (0x1100)
#define  V1190_MICRO_WR_OK       (WORD) (0x0001)
#define  V1190_MICRO_RD_OK       (WORD) (0x0002)
#define  V1190_MICRO_TDCID       (WORD) (0x6000)
#define  V1190_EDGE_DETECTION_WO (WORD) (0x2200)
#define  V1190_EDGE_RESOLUTION_WO (WORD) (0x2400)
#define  V1190_LEW_RESOLUTION_WO (WORD) (0x2500)
#define  V1190_RESOLUTION_RO     (WORD) (0x2600)
#define  V1190_TRIGGER_MATCH_WO  (WORD) (0x0000)
#define  V1190_CONTINUOUS_WO     (WORD) (0x0100)
#define  V1190_ACQ_MODE_RO       (WORD) (0x0200)
/* V1190 Data Flags */
#define  V1190_DATA_FLAG         (DWORD) (0xf8000000)
#define  V1190_DATA_FLAG_GLHD    (DWORD) (0x40000000)
#define  V1190_DATA_FLAG_GLTR    (DWORD) (0x80000000)
#define  V1190_DATA_FLAG_GLTT    (DWORD) (0x88000000)
#define  V1190_DATA_FLAG_DATA    (DWORD) (0x00000000)
#define  V1190_DATA_FLAG_TDHD    (DWORD) (0x08000000)
#define  V1190_DATA_FLAG_TDTR    (DWORD) (0x18000000)
#define  V1190_DATA_FLAG_TDER    (DWORD) (0x20000000)
#define  V1190_DATA_FLAG_FLLR    (DWORD) (0xC0000000)

/* V1190 Setting parameters */
typedef enum { V1190_ED_Pair=0, V1190_ED_Trailing=1, V1190_ED_Leading=2, V1190_ED_TRandLE=3 } V1190_EDGE_MODE;
typedef enum { V1190_ED_Res_800PS=0, V1190_ED_Res_200PS=1, V1190_ED_Res_100PS=2 } V1190_EDGE_RESOL;
typedef enum { V1190_DT_5NS= 0, V1190_DT_10NS= 1, V1190_DT_30NS= 2, V1190_DT_100NS= 3 } V1190_HITS_DEAD_TIME;
typedef enum
{
    V1190_HE_0                              = 0,            /*!< 0 hits per event. */
    V1190_HE_1                              = 1,            /*!< 1 hits per event. */
    V1190_HE_2                              = 2,            /*!< 2 hits per event. */
    V1190_HE_4                              = 3,            /*!< 4 hits per event. */
    V1190_HE_8                              = 4,            /*!< 8 hits per event. */
    V1190_HE_16                             = 5,            /*!< 16 hits per event. */
    V1190_HE_32                             = 6,            /*!< 32 hits per event. */
    V1190_HE_64                             = 7,            /*!< 64 hits per event. */
    V1190_HE_128                            = 8,            /*!< 128 hits per event. */
    V1190_HE_NO_LIMIT                       = 9,            /*!< no limits to hits per event. */
} V1190_MAXHITS_PEREVENT;
typedef enum
{
    V1190_FS_2                              = 0,            /*!< 2 words. */
    V1190_FS_4                              = 1,            /*!< 4 words. */
    V1190_FS_8                              = 2,            /*!< 8 words. */
    V1190_FS_16                             = 3,            /*!< 16 words. */
    V1190_FS_32                             = 4,            /*!< 32 words. */
    V1190_FS_64                             = 5,            /*!< 64 words. */
    V1190_FS_128                            = 6,            /*!< 128 words. */
    V1190_FS_256                            = 7,            /*!< 256 words. */
} V1190_FIFO_SIZE;

void initMVMVE();
int  udelay(int usec);
//WORD v1190_Read16(int *hvme, DWORD base, int offset);
//DWORD v1190_Read32(int *hvme, DWORD base, int offset);
//void v1190_Write16(int *hvme, DWORD base, int offset, WORD value);
//int v1190_Write16(int *hvme, DWORD base, int offset, WORD data);
//int v1190_dummyWrite16(int *hvme, DWORD base, WORD value=0x0);
int  v1190_EventRead(int *hvme, DWORD base, DWORD *pdest, int *nentry);
int  v1190_DataRead(int *hvme, DWORD base, DWORD *pdest, int nentry);
//void v1190_SoftReset(int *hvme, DWORD base);
void v1190_SoftClear(int *hvme, DWORD base);
//void v1190_SoftTrigger(int *hvme, DWORD base);
void v1190_DataReset(int *hvme, DWORD base);
int  v1190_DataReady(int *hvme, DWORD base);
//int  v1190_EvtCounter(int *hvme, DWORD base);
int  v1190_EvtStored(int *hvme, DWORD base);
int  v1190_MicroCheck(int *hvme, const DWORD base, int what);
int  v1190_MicroWrite(int *hvme, DWORD base, WORD data);
//int  v1190_MicroRead(int *hvme, const DWORD base);
//int  v1190_MicroFlush(int *hvme, const DWORD base);
//void v1190_TdcIdList(int *hvme, DWORD base, V1190Type type=V1190B);
//int  v1190_ResolutionRead(int *hvme, DWORD base, V1190Type type=V1190B);
//void v1190_EdgResolutionSet(int *hvme, DWORD base, V1190_EDGE_RESOL le=V1190_ED_Res_100PS);
void v1190_EdgResolutionSet(int *hvme, DWORD base, V1190_EDGE_RESOL le);
//void v1190_LEWResolutionSet(int *hvme, DWORD base, WORD le, WORD width);
//void v1190_AcqModeRead(int *hvme, DWORD base);
void v1190_TriggerMatchingSet(int *hvme, DWORD base);
void v1190_SubtractTriggerTimeON(int *hvme, DWORD base);
//void v1190_SubtractTriggerTimeOFF(int *hvme, DWORD base);
//void v1190_ContinuousSet(int *hvme, DWORD base);
void v1190_WidthSet(int *hvme, DWORD base, WORD width);
void v1190_WidthSet_ns(int *hvme, DWORD base, unsigned int width);
void v1190_OffsetSet(int *hvme, DWORD base, WORD offset);
void v1190_OffsetSet_ns(int *hvme, DWORD base, int offset);
//int  v1190_GeoWrite(int *hvme, DWORD base, int geo);
//int  v1190_Setup(int *hvme, DWORD base, int mode);
//int  v1190_Status(int *hvme, DWORD base, V1190Type type=V1190B);
int  v1190_Status(int *hvme, DWORD base, V1190Type type);
//void v1190_SetEdgeDetection(int *hvme, DWORD base, V1190_EDGE_MODE ed=V1190_ED_Trailing);
void v1190_SetEdgeDetection(int *hvme, DWORD base, V1190_EDGE_MODE ed);
//void v1190_SetDuoubleHitsRes(int *hvme, DWORD base, V1190_HITS_DEAD_TIME resol=V1190_DT_5NS);
//void v1190_SetDuoubleHitsRes_ns(int *hvme, DWORD base, int resol=5);
void v1190_SetDuoubleHitsRes_ns(int *hvme, DWORD base, int resol);
//void v1190_SetMaxNOfHitsPerEve(int *hvme, DWORD base, V1190_MAXHITS_PEREVENT maxHits=V1190_HE_NO_LIMIT);
void v1190_SetMaxNOfHitsPerEve(int *hvme, DWORD base, V1190_MAXHITS_PEREVENT maxHits);
//void v1190_SetMaxNOfHitsPerEve_absn(int *hvme, DWORD base, int maxHits=-1);
//void v1190_SetEffFIFO(int *hvme, DWORD base, V1190_FIFO_SIZE maxWords=V1190_FS_256);
//void v1190_SetEffFIFO_absn(int *hvme, DWORD base, int maxWords=-1);
void v1190_EnableERRORMark(int *hvme, DWORD base);
//void v1190_DisableERRORMark(int *hvme, DWORD base);
void v1190_EnableHeaderAndTrailer(int *hvme, DWORD base);
//void v1190_DisableHeaderAndTrailer(int *hvme, DWORD base);
//int  v1190_SatusHeaderAndTrailer(int *hvme, DWORD base);
//void v1190_EnableBusError(int *hvme, DWORD base);
//void v1190_DisableBusError(int *hvme, DWORD base);
int  v1190_SatusBusError(int *hvme, DWORD base);

#ifdef __cplusplus
}
#endif
#endif // V1190B_INCLUDE_H

