#ifndef __RUN_HEADER_H
#define __RUN_HEADER_H
#include "TObject.h"
#if __cplusplus < 201402L
#include <stdint.h>

typedef uint8_t __uint8_t;
typedef uint16_t __uint16_t;
typedef uint32_t __uint32_t;
typedef uint64_t __uint64_t;
#else
#include <stdlib.h>
#endif

class RunHeader: public TObject {
 public:
  RunHeader();
  RunHeader(
      __uint8_t ndevs,
      __uint16_t nuberOfX742Channels,
      __uint32_t runnum,
      __uint64_t beginTime);
  RunHeader(const RunHeader& runHeader);
  virtual ~RunHeader();
  __uint8_t getNumberOfDevicesInRun() const;
  __uint16_t getNumberOfX742PointsPerChannel() const;
  __uint16_t getNumberOfX742Channels() const;
  __uint32_t getRunNumber() const;
  __uint64_t getBeginTime() const;
 private:
  __uint8_t _ndevs;
  const __uint16_t _nuberOfX742PointsPerChannel;
  __uint16_t _nuberOfX742Channels;
  __uint32_t _runnum;
  __uint64_t _beginTime;
  ClassDef(RunHeader, 1)
};
#endif
