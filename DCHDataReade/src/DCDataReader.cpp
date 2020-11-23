
#include <stdio.h>
#include "DCDataReader.h"
#include "RunHeader.h"
#include "WaveData.h"

DCDataReader::DCDataReader(const char* fileName):
    _fileName(fileName) {
}

DCDataReader::~DCDataReader() {
}

void DCDataReader::getTree(TTree** tree) const {
  *tree = new TTree("data", "");
  __uint32_t runnum;
  __uint32_t evnum;
  __uint64_t btime;
  __uint8_t ndevs;
  __uint16_t chnum;
  __uint8_t chpdev[2];
  __uint8_t devids[2];
  __uint8_t devtypes[2];
  FILE* fl = fopen(_fileName.c_str(), "r");
  fread(&runnum, sizeof(__uint32_t), 1, fl);
  printf("runnum: %d\n", runnum);
  fread(&btime, sizeof(__uint64_t), 1, fl);
  printf("begin time: %ld\n", btime);
  fread(&ndevs, sizeof(__uint8_t), 1, fl);
  printf("ndevs: %d\n", ndevs);
  fread(chpdev, sizeof(__uint8_t), ndevs, fl);
  if (ndevs == 1) {
    printf("chpdev[0]: %d\n", chpdev[0]);
  }
  if (ndevs == 2) {
     printf("chpdev[0]: %d\n", chpdev[0]);
     printf("chpdev[1]: %d\n", chpdev[0]);
  }
  RunHeader runHeader(
      ndevs, chpdev[0],
      runnum, btime);
  int comp = chpdev[0];
  int nch = comp;
  if (ndevs == 2) {
    nch++;
  }
  WaveData waveData(runHeader);
  (*tree)->Branch("x", &waveData, 64000);
  int loop;
  __uint16_t nX742PointsPerCh =
      runHeader.getNumberOfX742PointsPerChannel();
  __uint16_t* chdata0 =
      new __uint16_t[nX742PointsPerCh];
  __uint32_t* chdata1;
  while (true) {
    loop = fread(&evnum, sizeof(__uint32_t), 1, fl);
    if (evnum%1000==0) printf("evnum: %d\n", evnum);
    if (!loop) {
      break;
    }
    waveData.setEventNumber(evnum);
    fread(devids, sizeof(__uint8_t), ndevs, fl);
    fread(devtypes, sizeof(__uint8_t), ndevs, fl);
    for (int k = 0; k < ndevs; ++k) {
      //printf("devids[%d]: %d\n"
      //       "devtypes[%d]: %d\n",
      //       k, devids[k], k, devtypes[k]);
    }
    for (int i = 0; i < nch; ++i) {
      fread(&chnum, sizeof(__uint16_t), 1, fl);
       if (i < comp) {
         //printf("[%d] 1st chnum: %d\n", i, chnum);
        fread(chdata0,
              sizeof(__uint16_t),
              nX742PointsPerCh, fl);
        waveData.setX742ChannelData(
            chnum, chdata0);
       } else {
         __uint32_t tmp;
         __uint8_t tdevids[2];
         __uint8_t tdevtypes[2];
         fseek(fl, -sizeof(__uint16_t), SEEK_CUR);
         fread(&tmp, sizeof(__uint32_t), 1, fl);
         fread(&tdevids, sizeof(__uint8_t), 2, fl);
         fread(&tdevtypes, sizeof(__uint8_t), 2, fl);
         bool coin = (tmp == evnum + 1 &&
                      tdevids[0] == 0 &&
                      tdevids[1] == 1 &&
                      tdevtypes[0] == 0 &&
                      tdevtypes[1] == 1);
         if (coin) {
           fseek(fl, -sizeof(__uint32_t) - 4 * sizeof(__uint8_t), SEEK_CUR);
           //printf("[%d] 2nd chnum: %d\n", i, 0);
         } else {
           fseek(fl, -sizeof(__uint16_t) - 4 * sizeof(__uint8_t), SEEK_CUR);
         }
         if (!coin && chnum != 0) {
           //printf("[%d] 2nd chnum: %d\n", i, chnum);
           chdata1 = new __uint32_t[chnum];
           fread(chdata1,
                 sizeof(__uint32_t),
                 chnum, fl);
           waveData.setV1190ChannelData(
               chnum, chdata1);
           delete [] chdata1;
         }
       }
    }
    (*tree)->Fill();
  }
  delete [] chdata0;
  (*tree)->SetDirectory(0);
  fclose(fl);
}
