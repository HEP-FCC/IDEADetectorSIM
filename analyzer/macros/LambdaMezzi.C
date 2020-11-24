void DrawWave() {

TFile f("MCHits00001.root");

TTree *tr = (TTree *)f.Get("MCHits");

Int_t nEvents = tr->GetEntries();

cout<<"Number of events Stored --> "<<nEvents<<endl;

TClonesArray *hits = new TClonesArray("GMCDCHHit");
tr->SetBranchAddress("DCHits",&hits);

 
for (int ie=0;ie<nEvents;ie++) {

  tr->GetEntry(ie);  //ti ritorna l'evento ie

  //il numero di hit lo ottienei tramite
  Int_t nhits = hits->GetEntries();
  
  //ciclo sugli hit
  for (int ih=0;ih<nhits;ih++) {

    //prendiamo l'hit ih
    GMCDCHHit *ahit = (GMCDCHHit *)hits->At(ih); 
    //numero di clusters
    Int_t nclust = ahit->GetfNrClusters();
    
    //ci metti il ciclo sulle distanze di drift ed il calcolo dei vari lambda
  
  }

}
 
}
