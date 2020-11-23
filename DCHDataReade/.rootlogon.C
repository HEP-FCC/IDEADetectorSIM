{
  gSystem->AddIncludePath("-I WaveData/inc");
  gSystem->AddIncludePath("-I WaveData/llinkdef_inc");
  gSystem->Load("WaveData/lib/libWaveData.so");
}
