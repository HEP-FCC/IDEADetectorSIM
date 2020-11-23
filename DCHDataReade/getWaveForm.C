TGraph* getWaveForm(int channel, int entry, const char* filename) {
  auto gr = new TGraph();
  auto fl = TFile::Open(filename, "read");
  auto tree = static_cast<TTree*>(fl->Get("data"));
  int n = tree->GetEntries();
  WaveData* wd = new WaveData();
  tree->SetBranchAddress("x", &wd);
  tree->GetEntry(entry);
 
  for (auto point : wd->getX742Data()) {
    if (point.first == channel) {
       for (int i = 0; i < 1024; ++i) {
         gr->SetPoint(i, (double) i, (double) point.second[i]);
       }
      break;
    }
  }
  gr->SetMarkerStyle(7);
  fl->Close();
  delete wd;
  return gr;
}
