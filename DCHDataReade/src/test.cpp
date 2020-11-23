
#include <iostream>
#include <string>
#include <regex>
#include "TFile.h"
#include "TTree.h"
#include "DCDataReader.h"

void test(const char* filename) {
  auto fl = TFile::Open(filename, "read");
  auto tree = (TTree*) fl->Get("data");
  std::cout << "number of entries: " <<
      tree->GetEntries() << std::endl;
  fl->Close();
}

int main(int argc, char *argv[]) {
  if (argc == 1) {
    std::cout <<
        "You need to set file(s) name" <<
        std::endl;
  }
  for (int i = 1; i < argc; ++i) {
    bool isMatched =
        std::regex_match(
            argv[i],
            std::regex(".*dch_run_(.[0-9]+).dat"));
    if (isMatched) {
#if __cplusplus < 201402L
      std::string inFn=argv[i];
      std::string repStr="run_$1.root";
      std::string filename = std::regex_replace(
          inFn,
          std::regex("dch_run_(.[0-9]+).dat"),
          repStr);
#else
      std::string filename = std::regex_replace(
          argv[i],
          std::regex("dch_run_(.[0-9]+).dat"),
          "run_$1.root");
#endif
      auto fl = TFile::Open(filename.c_str(), "recreate");
      DCDataReader reader(argv[i]);
      TTree* tree;
      reader.getTree(&tree);
      fl->cd();
      tree->Write();
      fl->Close();
      delete tree;
      test(filename.c_str());
    } else {
      std::cout << "The file " <<
          argv[i] <<
          " doesn't match the template dch_run_[n]dat, "
          "where [n] is integer number." << std::endl;
    }
  }
  return 0;
}

