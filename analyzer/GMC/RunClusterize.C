void RunClusterize(Int_t nMode=1) {

  if (nMode == 1) {
    gROOT->ProcessLine(".L ../macros/Clusterize.C++");

    ClusterRaggr(20000);
  }

  if (nMode == 2) {
    gROOT->ProcessLine(".L ../macros/ClusterEfficiency.C++");
    ClusterEff(28000,160.,0);
  }

  if (nMode == 3) {
    gROOT->ProcessLine(".L ../macros/ClusterEfficiency.C++");
    ClusterEff(28000,160.,1);
  }

  if (nMode == 4) {
    gROOT->ProcessLine(".L ../macros/PeakAnalysis_v2.C++");
    PeakAnalysis(800,10000,1,3.,1.1);
  }

  if (nMode == 5) {
    gROOT->ProcessLine(".L ../macros/PeakFinder.C++");
    PeakAnalysis(800,10000,1,3.,1.1);
  }

}
