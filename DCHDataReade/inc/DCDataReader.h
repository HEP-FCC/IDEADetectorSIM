#ifndef __DCDATAREADER_H
#define __DCDATAREADER_H
#include <string>
#include "TTree.h"

class DCDataReader {
 public:
  explicit DCDataReader(const char* fileName);
  virtual ~DCDataReader();
  void getTree(TTree** tree) const;
 private:
  std::string _fileName;
};
#endif
