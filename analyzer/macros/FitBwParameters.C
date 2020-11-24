void FitBwParameters() {

  Float_t BWidth[3] = {0.3, 0.6, 0.8};
  Float_t TauUp[3] = {0.2413, 0.1505, 0.1236};
  Float_t TauDw1[3] = {1.175, 0.7051, 0.5709};
  Float_t TauDw2[3] = {9.831, 5.716, 4.432};
  Float_t Fraction[3] = {0.5049, 0.4909, 0.4766};


  TGraph *gr_up = new TGraph(3,BWidth,TauUp);
  TGraph *gr_dw1 = new TGraph(3,BWidth,TauDw1);
  TGraph *gr_dw2 = new TGraph(3,BWidth,TauDw2);
  TGraph *gr_frac = new TGraph(3,BWidth,Fraction);


  gr_up->SetMarkerStyle(22);
  gr_up->SetMarkerColor(1);
  gr_up->SetTitle("#tau_{up}; BW [GHz]; [ns]");

  gr_dw1->SetMarkerStyle(23);
  gr_dw1->SetMarkerColor(2);
  gr_dw1->SetTitle("#tau_{dw1}; BW [GHz]; [ns]");

  gr_dw2->SetMarkerStyle(29);
  gr_dw2->SetMarkerColor(3);
  gr_dw2->SetTitle("#tau_{dw2}; BW [GHz]; [ns]");

  gr_frac->SetMarkerStyle(33);
  gr_frac->SetMarkerColor(4);
  gr_frac->SetTitle("Mixing Fraction; BW [GHz]; [%]");


  TMultiGraph *t_gr = new TMultiGraph();
  t_gr->Add(gr_up);
  t_gr->Add(gr_dw1);
  t_gr->Add(gr_dw2);
  t_gr->Add(gr_frac);


  TCanvas *myC = new TCanvas();
  t_gr->Draw("ap");

  TCanvas *myC1 = new TCanvas();
  myC1->Divide(2,2);
  myC1->cd(1);
  gr_up->Draw("ap");
  myC1->cd(2);
  gr_dw1->Draw("ap");
  myC1->cd(3);
  gr_dw2->Draw("ap");
  myC1->cd(4);
  gr_frac->Draw("ap");


}
