void RunMacros(TString fname="",Int_t mode=0) {

  if (mode == 0) {

    if (fname.IsNull()) {
      printf("Error the input file is not set!!!");
      return;
    }
 
    gROOT->ProcessLine(".L ../macros/PeakAnalysis_v1.C++");

    PeakAnalysis_v1(fname,0,0,0.1,1.1);

  }

  if (mode == 1) {

    gROOT->ProcessLine(".L ../macros/EfficiencyOnNoise.C++");

    EfficiencyOnNoise(fname);

  }

}
