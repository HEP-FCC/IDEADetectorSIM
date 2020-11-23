
#include <iostream>
#include <algorithm>
#include "WaveData.h"

ClassImp(WaveData)

WaveData::WaveData() {
}

WaveData::WaveData(const RunHeader& runHeader):
    _runHeader(runHeader) {
}

WaveData::~WaveData() {
}

const RunHeader* const WaveData::getRunHeader() const {
  return &_runHeader;
}

const WaveData::x742chdata&
WaveData::getChannelDataX742(
    __uint16_t chNumber) const {
  return _chdata_x742.at(chNumber);
}

__uint16_t WaveData::getChannelDataX742(
    __uint16_t chNumber,
    __uint16_t pointNumber) const {
  return _chdata_x742.at(chNumber)[pointNumber];
}

void WaveData::setX742ChannelData(
    const __uint16_t& chNumber,
    const __uint16_t* const chdata) {
  _chdata_x742[chNumber] =
      WaveData::x742chdata(
          _runHeader.getNumberOfX742PointsPerChannel());
  std::copy(chdata,
            chdata +
            _runHeader.getNumberOfX742PointsPerChannel(),
            _chdata_x742[chNumber].begin());
}


void WaveData::setV1190ChannelData(
    const __uint16_t& blockSize,
    const __uint32_t* const chdata) {
  _chdata_v1190.resize(blockSize);
  std::copy(chdata,
            chdata + blockSize,
            _chdata_v1190.begin());
}
const WaveData::v1190data&
WaveData::getV1190Data() const {
  return _chdata_v1190;
}

__uint32_t WaveData::getV1190Data(__uint16_t index) const {
  return _chdata_v1190[index];
}

__uint32_t WaveData::getEventNumber() const {
  return _evnum;
}

void WaveData::setEventNumber(const __uint32_t& evnum) {
  _evnum = evnum;
}

const WaveData::x742map&
WaveData::getX742Data() const {
  return _chdata_x742;
}

WaveData::v1190ConvData
WaveData::convertV1190Data() const {
  v1190ConvData res;
  res.reserve(_chdata_v1190.size());
  for (const auto& data : _chdata_v1190) {
    if ((data & 0xf8000000) != 0x00000000) {
      continue;
    }
    int chan = 0x7F & (data >> 19);
    int time = 0x7FFFF & data;
    res.push_back(v1190ch(chan, time));
  }
  return res;
}
