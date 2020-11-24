void GenerateSeeds(Int_t nseed) {

ofstream fInput(Form("SeedsGenerated_%d.lst",nseed),ios::out);

Long_t xxx, yyy;

for (int i=0;i<nseed;i++) {
  xxx = 1.e9*gRandom->Uniform();
  yyy = 1.e9*gRandom->Uniform();
  while (xxx>yyy) {
    yyy = 1.e9*gRandom->Uniform();
  }
  if (xxx>yyy) {cerr<<"wrong sequence"<<endl;}
  fInput<<setfill('0')<<setw(9)<<xxx<<"\t"<<setfill('0')<<setw(9)<<yyy<<endl;
}

fInput.close();

}
