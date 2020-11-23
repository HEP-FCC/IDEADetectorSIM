#include "RunHeader.h"

ClassImp(RunHeader)

RunHeader::RunHeader():
_nuberOfX742PointsPerChannel(1024) {
}

RunHeader::RunHeader(
    __uint8_t ndevs,
    __uint16_t nuberOfX742Channels,
    __uint32_t runnum,
    __uint64_t beginTime):
    _ndevs(ndevs),
    _nuberOfX742PointsPerChannel(1024),
    _nuberOfX742Channels(nuberOfX742Channels),
    _runnum(runnum),
    _beginTime(beginTime) {
}

RunHeader::RunHeader(const RunHeader& runHeader):
    _ndevs(runHeader.getNumberOfDevicesInRun()),
    _nuberOfX742PointsPerChannel(1024), _nuberOfX742Channels(runHeader.getNumberOfX742Channels()),
    _runnum(runHeader.getRunNumber()),
    _beginTime(runHeader.getBeginTime()) {
}

RunHeader::~RunHeader() {
}

__uint8_t RunHeader::getNumberOfDevicesInRun() const {
  return _ndevs;
}

__uint16_t RunHeader::getNumberOfX742PointsPerChannel() const {
  return _nuberOfX742PointsPerChannel;
}

__uint16_t RunHeader::getNumberOfX742Channels() const {
  return _nuberOfX742Channels;
}

__uint32_t RunHeader::getRunNumber() const {
  return _runnum;
}

__uint64_t RunHeader::getBeginTime() const {
  return _beginTime;
}
