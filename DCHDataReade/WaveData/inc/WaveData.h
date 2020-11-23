
#include <utility>
#include <vector>
#if __cplusplus < 201402L
#include <map>
#else
#include <unordered_map>
#endif
#include "TObject.h"
#include "RunHeader.h"

class WaveData: public TObject {
 public:
  typedef std::vector<__uint16_t> x742chdata;
#if __cplusplus < 201402L
  typedef std::map<__uint16_t,
      x742chdata > x742map;
#else
  typedef std::unordered_map<__uint16_t,
      x742chdata > x742map;
#endif
  typedef std::vector<__uint32_t> v1190data;
  typedef std::pair<int, int> v1190ch;
  typedef std::vector<v1190ch > v1190ConvData;
  WaveData();
  explicit WaveData(const RunHeader& runHeader);
  virtual ~WaveData();
  const RunHeader* const getRunHeader() const;
  const x742map& getX742Data() const;
  const x742chdata& getChannelDataX742(
      __uint16_t chNumber) const;
  __uint16_t getChannelDataX742(__uint16_t chNumber,
                                __uint16_t pointNumber) const;
  const v1190data& getV1190Data() const;
  __uint32_t getV1190Data(__uint16_t index) const;
  v1190ConvData convertV1190Data() const;
  __uint32_t getEventNumber() const;
  void setX742ChannelData(
      const __uint16_t& chNumber,
      const __uint16_t* const chdata);
  void setV1190ChannelData(
      const __uint16_t& blockSize,
      const __uint32_t* const chdata);
  void setEventNumber(const __uint32_t& evnum);
 private:
  RunHeader _runHeader;
  x742map _chdata_x742;
  v1190data _chdata_v1190;
  __uint32_t _evnum;
  ClassDef(WaveData, 1)
};
