void ClusterSize() {

TH1F *fClusterDistr = new TH1F("fClusterDistr","ClusterDistrution Hist",100,0.5,100.5);

   fClusterDistr->SetBinContent(1,76.600);
   fClusterDistr->SetBinContent(2,12.500);
   fClusterDistr->SetBinContent(3,4.600);
   fClusterDistr->SetBinContent(4,2.000);
   fClusterDistr->SetBinContent(5,1.200);
   fClusterDistr->SetBinContent(6,0.750);
   fClusterDistr->SetBinContent(7,0.500);
   fClusterDistr->SetBinContent(8,0.3360);
   fClusterDistr->SetBinContent(9,0.250);
   fClusterDistr->SetBinContent(10,0.190);
   fClusterDistr->SetBinContent(11,0.140);
   fClusterDistr->SetBinContent(12,0.100);
   fClusterDistr->SetBinContent(13,0.080);
   fClusterDistr->SetBinContent(14,0.060);
   fClusterDistr->SetBinContent(15,0.048);
   fClusterDistr->SetBinContent(16,0.043);
   fClusterDistr->SetBinContent(17,0.038);
   fClusterDistr->SetBinContent(18,0.034);
   fClusterDistr->SetBinContent(19,0.030);

   for (int i=20;i<101;i++) {
     fClusterDistr->SetBinContent(i,10.9/(i*i));
     //     cout<<"stampo il settaggio dei bin  "<<i<<"   "<<10.9/(i*i)<<endl;
   }
   fClusterDistr->Scale(100./fClusterDistr->Integral());


   TH1F *pippo = new TH1F("0000","Estraione",100,0.5,100.5);

   Double_t val;
   for (int i=0;i<1000;i++) {
     val = fClusterDistr->GetRandom();
     pippo->Fill(val);
     cout<<"Estrazione "<<i<<"   "<<val<<"  "<<TMath::Nint(val)<<endl;

   }


   TCanvas *mC1 = new TCanvas();
   fClusterDistr->DrawCopy();
   TCanvas *mC2 = new TCanvas();
   pippo->DrawCopy();
}
