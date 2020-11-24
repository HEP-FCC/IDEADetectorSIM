
void PlotPhotonConvStudy(Int_t nRun=1, Int_t frtRun=0){

	TFile outF("PHCVHistos.root","recreate");

	TH2D* HResidualUnbiased;
	TH2D* HResidualBiased;
	TH2D* HMomentum;
	TH2D* HMomRes_El;
	TH2D* HMomRes_Ps;

	TH1F* HConvHitPerShell;
	TH1F* HGammaPerShell;
	TH1F* HThetaGamma;
	TH1F* HConvProbPerShell;
	TH1F* HConvEnergy;
	TH1F* HPairEnergy;
	TH1F* HPairEnergyRad;
	TH1F* HPairEnergySct;
	TH2D* HPairEnergyRadDiff;
	TH2D* HPairEnergySctDiff;
	TH1F* HPairThetaRad;
	TH1F* HThetaRad_El;
	TH1F* HThetaRad_Ps;
	TH1F* HPairPhiRad;
	TH1F* HPhiRad_El;
	TH1F* HPhiRad_Ps;
	TH1F* HPairThetaSct;
	TH1F* HPairPhiSct;
	TH2D* HEnergyOut_El;
	TH2D* HElossInRadPerShell_El;
	TH2D* HElossInSctPerShell_El;
	TH2D* HEnergyOut_Ps;
	TH2D* HElossInRadPerShell_Ps;
	TH2D* HElossInSctPerShell_Ps;
	TH1F* HElHits;
	TH2D* HElHitsDiff;
	TH1F* HPsHits;
	TH2D* HPsHitsDiff;
	TH1F* HPairEnergyRec;
	TH2D* HPairEnergyRecDiff;

	TH1F* HRecPairEnergy;
	TH2D* HRecPairEnergyDiff;
	TH1F* HRecPairEnergyS;
	TH2D* HRecPairEnergySDiff;
	TH2D* HRecPair2EnergyDiff;
	TH2D* HRecPair2EnergySDiff;
	TH2D* HRecPair2EnergyDiff_1;
	TH2D* HRecPair2EnergySDiff_1;
	TH1F* HRecPairEnergyDebg;
	TH2D* HPairEnergyHit0Debg;
	TH2D* HPairEnergyScntDebg;
	TH2D* HPairVertXYResHt0ScDebg;
	TH2D* HPairVertZResHt0ScDebg;
	TH2D* HPairMomReasHt0ScDebg;

	for (int ifl=frtRun; ifl<frtRun+nRun; ++ifl) {
		TFile *tFl = TFile::Open(Form("histos%05d.root",ifl));
		if (tFl) {
			TH2D* tHResidualUnbiased = (TH2D*) tFl->Get("histos/PhotonConvStudyHistos/HResidualUnbiased");
			TH2D* tHResidualBiased = (TH2D*) tFl->Get("histos/PhotonConvStudyHistos/HResidualBiased");
			TH2D* tHMomentum = (TH2D*) tFl->Get("histos/PhotonConvStudyHistos/HMomentum");
			TH2D* tHMomRes_El = (TH2D*) tFl->Get("histos/PhotonConvStudyHistos/HMomRes_El");
			TH2D* tHMomRes_Ps = (TH2D*) tFl->Get("histos/PhotonConvStudyHistos/HMomRes_Ps");

			TH1F* tHConvHitPerShell = (TH1F*) tFl->Get("histos/PhotonConvStudyHistos/HConvHitPerShell");
			TH1F* tHGammaPerShell = (TH1F*) tFl->Get("histos/PhotonConvStudyHistos/HGammaPerShell");
			TH1F* tHThetaGamma = (TH1F*) tFl->Get("histos/PhotonConvStudyHistos/HThetaGamma");
			TH1F* tHConvProbPerShell = (TH1F*) tFl->Get("histos/PhotonConvStudyHistos/HConvProbPerShell");
			TH1F* tHConvEnergy = (TH1F*) tFl->Get("histos/PhotonConvStudyHistos/HConvEnergy");
			TH1F* tHPairEnergy = (TH1F*) tFl->Get("histos/PhotonConvStudyHistos/HPairEnergy");
			TH1F* tHPairEnergyRad = (TH1F*) tFl->Get("histos/PhotonConvStudyHistos/HPairEnergyRad");
			TH1F* tHPairEnergySct = (TH1F*) tFl->Get("histos/PhotonConvStudyHistos/HPairEnergySct");
			TH2D* tHPairEnergyRadDiff = (TH2D*) tFl->Get("histos/PhotonConvStudyHistos/HPairEnergyRadDiff");
			TH2D* tHPairEnergySctDiff = (TH2D*) tFl->Get("histos/PhotonConvStudyHistos/HPairEnergySctDiff");
			TH1F* tHPairThetaRad = (TH1F*) tFl->Get("histos/PhotonConvStudyHistos/HPairThetaRad");
			TH1F* tHThetaRad_El = (TH1F*) tFl->Get("histos/PhotonConvStudyHistos/HThetaRad_El");
			TH1F* tHThetaRad_Ps = (TH1F*) tFl->Get("histos/PhotonConvStudyHistos/HThetaRad_Ps");
			TH1F* tHPairPhiRad = (TH1F*) tFl->Get("histos/PhotonConvStudyHistos/HPairPhiRad");
			TH1F* tHPhiRad_El = (TH1F*) tFl->Get("histos/PhotonConvStudyHistos/HPhiRad_El");
			TH1F* tHPhiRad_Ps = (TH1F*) tFl->Get("histos/PhotonConvStudyHistos/HPhiRad_Ps");
			TH1F* tHPairThetaSct = (TH1F*) tFl->Get("histos/PhotonConvStudyHistos/HPairThetaSct");
			TH1F* tHPairPhiSct = (TH1F*) tFl->Get("histos/PhotonConvStudyHistos/HPairPhiSct");
			TH2D* tHEnergyOut_El = (TH2D*) tFl->Get("histos/PhotonConvStudyHistos/HEnergyOut_El");
			TH2D* tHElossInRadPerShell_El = (TH2D*) tFl->Get("histos/PhotonConvStudyHistos/HElossInRadPerShell_El");
			TH2D* tHElossInSctPerShell_El = (TH2D*) tFl->Get("histos/PhotonConvStudyHistos/HElossInSctPerShell_El");
			TH2D* tHEnergyOut_Ps = (TH2D*) tFl->Get("histos/PhotonConvStudyHistos/HEnergyOut_Ps");
			TH2D* tHElossInRadPerShell_Ps = (TH2D*) tFl->Get("histos/PhotonConvStudyHistos/HElossInRadPerShell_Ps");
			TH2D* tHElossInSctPerShell_Ps = (TH2D*) tFl->Get("histos/PhotonConvStudyHistos/HElossInSctPerShell_Ps");
			TH1F* tHElHits = (TH1F*) tFl->Get("histos/PhotonConvStudyHistos/HElHits");
			TH2D* tHElHitsDiff = (TH2D*) tFl->Get("histos/PhotonConvStudyHistos/HElHitsDiff");
			TH1F* tHPsHits = (TH1F*) tFl->Get("histos/PhotonConvStudyHistos/HPsHits");
			TH2D* tHPsHitsDiff = (TH2D*) tFl->Get("histos/PhotonConvStudyHistos/HPsHitsDiff");
			TH1F* tHPairEnergyRec = (TH1F*) tFl->Get("histos/PhotonConvStudyHistos/HPairEnergyRec");
			TH2D* tHPairEnergyRecDiff = (TH2D*) tFl->Get("histos/PhotonConvStudyHistos/HPairEnergyRecDiff");

			TH1F* tHRecPairEnergy = (TH1F*) tFl->Get("histos/PhotonConvStudyHistos/HRecPairEnergy");
			TH2D* tHRecPairEnergyDiff = (TH2D*) tFl->Get("histos/PhotonConvStudyHistos/HRecPairEnergyDiff");
			TH1F* tHRecPairEnergyS = (TH1F*) tFl->Get("histos/PhotonConvStudyHistos/HRecPairEnergyS");
			TH2D* tHRecPairEnergySDiff = (TH2D*) tFl->Get("histos/PhotonConvStudyHistos/HRecPairEnergySDiff");
			TH2D* tHRecPair2EnergyDiff = (TH2D*) tFl->Get("histos/PhotonConvStudyHistos/HRecPair2EnergyDiff");
			TH2D* tHRecPair2EnergySDiff = (TH2D*) tFl->Get("histos/PhotonConvStudyHistos/HRecPair2EnergySDiff");
			TH2D* tHRecPair2EnergyDiff_1 = (TH2D*) tFl->Get("histos/PhotonConvStudyHistos/HRecPair2EnergyDiff_1");
			TH2D* tHRecPair2EnergySDiff_1 = (TH2D*) tFl->Get("histos/PhotonConvStudyHistos/HRecPair2EnergySDiff_1");
			TH1F* tHRecPairEnergyDebg = (TH1F*) tFl->Get("histos/PhotonConvStudyHistos/HRecPairEnergyDebg");
			TH2D* tHPairEnergyHit0Debg = (TH2D*) tFl->Get("histos/PhotonConvStudyHistos/HPairEnergyHit0Debg");
			TH2D* tHPairEnergyScntDebg = (TH2D*) tFl->Get("histos/PhotonConvStudyHistos/HPairEnergyScntDebg");
			TH2D* tHPairVertXYResHt0ScDebg = (TH2D*) tFl->Get("histos/PhotonConvStudyHistos/HPairVertXYResHt0ScDebg");
			TH2D* tHPairVertZResHt0ScDebg = (TH2D*) tFl->Get("histos/PhotonConvStudyHistos/HPairVertZResHt0ScDebg");
			TH2D* tHPairMomReasHt0ScDebg = (TH2D*) tFl->Get("histos/PhotonConvStudyHistos/HPairMomReasHt0ScDebg");

			if (ifl==frtRun) {
				outF.cd();
				HResidualUnbiased = (TH2D*) tHResidualUnbiased->Clone();
				HResidualBiased = (TH2D*) tHResidualBiased->Clone();
				HMomentum = (TH2D*) tHMomentum->Clone();
				HMomRes_El = (TH2D*) tHMomRes_El->Clone();
				HMomRes_Ps = (TH2D*) tHMomRes_Ps->Clone();

				HConvHitPerShell = (TH1F*) tHConvHitPerShell->Clone();
				HGammaPerShell = (TH1F*) tHGammaPerShell->Clone();
				HThetaGamma = (TH1F*) tHThetaGamma->Clone();
				HConvProbPerShell = (TH1F*) tHConvHitPerShell->Clone();
				HConvEnergy = (TH1F*) tHConvEnergy->Clone();
				HPairEnergy = (TH1F*) tHPairEnergy->Clone();
				HPairEnergyRad = (TH1F*) tHPairEnergyRad->Clone();
				HPairEnergySct = (TH1F*) tHPairEnergySct->Clone();
				HPairEnergyRadDiff = (TH2D*) tHPairEnergyRadDiff->Clone();
				HPairEnergySctDiff = (TH2D*) tHPairEnergySctDiff->Clone();
				HPairThetaRad = (TH1F*) tHPairThetaRad->Clone();
				HThetaRad_El = (TH1F*) tHThetaRad_El->Clone();
				HThetaRad_Ps = (TH1F*) tHThetaRad_Ps->Clone();
				HPairPhiRad = (TH1F*) tHPairPhiRad->Clone();
				HPhiRad_El = (TH1F*) tHPhiRad_El->Clone();
				HPhiRad_Ps = (TH1F*) tHPhiRad_Ps->Clone();
				HPairThetaSct = (TH1F*) tHPairThetaSct->Clone();
				HPairPhiSct = (TH1F*) tHPairPhiSct->Clone();
				HEnergyOut_El = (TH2D*) tHEnergyOut_El->Clone();
				HElossInRadPerShell_El = (TH2D*) tHElossInRadPerShell_El->Clone();
				HElossInSctPerShell_El = (TH2D*) tHElossInSctPerShell_El->Clone();
				HEnergyOut_Ps = (TH2D*) tHEnergyOut_Ps->Clone();
				HElossInRadPerShell_Ps = (TH2D*) tHElossInRadPerShell_Ps->Clone();
				HElossInSctPerShell_Ps = (TH2D*) tHElossInSctPerShell_Ps->Clone();
				HElHits = (TH1F*) tHElHits->Clone();
				HElHitsDiff = (TH2D*) tHElHitsDiff->Clone();
				HPsHits = (TH1F*) tHPsHits->Clone();
				HPsHitsDiff = (TH2D*) tHPsHitsDiff->Clone();
				HPairEnergyRec = (TH1F*) tHPairEnergyRec->Clone();
				HPairEnergyRecDiff = (TH2D*) tHPairEnergyRecDiff->Clone();
				HConvProbPerShell->SetTitle(((TH1F*) tHConvProbPerShell)->GetTitle());
				HConvProbPerShell->SetYTitle(((TH1F*) tHConvProbPerShell)->GetYaxis()->GetTitle());
				HConvProbPerShell->SetName(((TH1F*) tHConvProbPerShell)->GetName());

				HRecPairEnergy = (TH1F*) tHRecPairEnergy->Clone();
				HRecPairEnergyDiff = (TH2D*) tHRecPairEnergyDiff->Clone();
				HRecPairEnergyS = (TH1F*) tHRecPairEnergyS->Clone();
				HRecPairEnergySDiff = (TH2D*) tHRecPairEnergySDiff->Clone();
				HRecPair2EnergyDiff = (TH2D*) tHRecPair2EnergyDiff->Clone();
				HRecPair2EnergySDiff = (TH2D*) tHRecPair2EnergySDiff->Clone();
				HRecPair2EnergyDiff_1 = (TH2D*) tHRecPair2EnergyDiff_1->Clone();
				HRecPair2EnergySDiff_1 = (TH2D*) tHRecPair2EnergySDiff_1->Clone();
				HRecPairEnergyDebg = (TH1F*) tHRecPairEnergyDebg->Clone();
				HPairEnergyHit0Debg = (TH2D*) tHPairEnergyHit0Debg->Clone();
				HPairEnergyScntDebg = (TH2D*) tHPairEnergyScntDebg->Clone();
				HPairVertXYResHt0ScDebg = (TH2D*) tHPairVertXYResHt0ScDebg->Clone();
				HPairVertZResHt0ScDebg = (TH2D*) tHPairVertZResHt0ScDebg->Clone();
				HPairMomReasHt0ScDebg = (TH2D*) tHPairMomReasHt0ScDebg->Clone();

			} else {
				HResidualUnbiased->Add(tHResidualUnbiased);
				HResidualBiased->Add(tHResidualBiased);
				HMomentum->Add(tHMomentum);
				HMomRes_El->Add(tHMomRes_El);
				HMomRes_Ps->Add(tHMomRes_Ps);

				HConvHitPerShell->Add(tHConvHitPerShell);
				HGammaPerShell->Add(tHGammaPerShell);
				HThetaGamma->Add(tHThetaGamma);
				HConvProbPerShell->Add(tHConvHitPerShell);
				HConvEnergy->Add(tHConvEnergy);
				HPairEnergy->Add(tHPairEnergy);
				HPairEnergyRad->Add(tHPairEnergyRad);
				HPairEnergySct->Add(tHPairEnergySct);
				HPairEnergyRadDiff->Add(tHPairEnergyRadDiff);
				HPairEnergySctDiff->Add(tHPairEnergySctDiff);
				HPairThetaRad->Add(tHPairThetaRad);
				HThetaRad_El->Add(tHThetaRad_El);
				HThetaRad_Ps->Add(tHThetaRad_Ps);
				HPairPhiRad->Add(tHPairPhiRad);
				HPhiRad_El->Add(tHPhiRad_El);
				HPhiRad_Ps->Add(tHPhiRad_Ps);
				HPairThetaSct->Add(tHPairThetaSct);
				HPairPhiSct->Add(tHPairPhiSct);
				HEnergyOut_El->Add(tHEnergyOut_El);
				HElossInRadPerShell_El->Add(tHElossInRadPerShell_El);
				HElossInSctPerShell_El->Add(tHElossInSctPerShell_El);
				HEnergyOut_Ps->Add(tHEnergyOut_Ps);
				HElossInRadPerShell_Ps->Add(tHElossInRadPerShell_Ps);
				HElossInSctPerShell_Ps->Add(tHElossInSctPerShell_Ps);
				HElHits->Add(tHElHits);
				HElHitsDiff->Add(tHElHitsDiff);
				HPsHits->Add(tHPsHits);
				HPsHitsDiff->Add(tHPsHitsDiff);
				HPairEnergyRec->Add(tHPairEnergyRec);
				HPairEnergyRecDiff->Add(tHPairEnergyRecDiff);

				HRecPairEnergy->Add(tHRecPairEnergy);
				HRecPairEnergyDiff->Add(tHRecPairEnergyDiff);
				HRecPairEnergyS->Add(tHRecPairEnergyS);
				HRecPairEnergySDiff->Add(tHRecPairEnergySDiff);
				HRecPair2EnergyDiff->Add(tHRecPair2EnergyDiff);
				HRecPair2EnergySDiff->Add(tHRecPair2EnergySDiff);
				HRecPair2EnergyDiff_1->Add(tHRecPair2EnergyDiff_1);
				HRecPair2EnergySDiff_1->Add(tHRecPair2EnergySDiff_1);
				HRecPairEnergyDebg->Add(tHRecPairEnergyDebg);
				HPairEnergyHit0Debg->Add(tHPairEnergyHit0Debg);
				HPairEnergyScntDebg->Add(tHPairEnergyScntDebg);
				HPairVertXYResHt0ScDebg->Add(tHPairVertXYResHt0ScDebg);
				HPairVertZResHt0ScDebg->Add(tHPairVertZResHt0ScDebg);
				HPairMomReasHt0ScDebg->Add(tHPairMomReasHt0ScDebg);

			}
			tFl->Close();
		}
	}
	HConvProbPerShell->Divide(HGammaPerShell);
	outF.cd();
	HResidualUnbiased->Write();
	HResidualBiased->Write();
	HMomentum->Write();
	HMomRes_El->Write();
	HMomRes_Ps->Write();

	HConvHitPerShell->Write();
	HGammaPerShell->Write();
	HThetaGamma->Write();
	HConvProbPerShell->Write();
	HConvEnergy->Write();
	HPairEnergy->Write();
	HPairEnergyRad->Write();
	HPairEnergySct->Write();
	HPairEnergyRadDiff->Write();
	HPairEnergySctDiff->Write();
	HPairThetaRad->Write();
	HThetaRad_El->Write();
	HThetaRad_Ps->Write();
	HPairPhiRad->Write();
	HPhiRad_El->Write();
	HPhiRad_Ps->Write();
	HPairThetaSct->Write();
	HPairPhiSct->Write();
	HEnergyOut_El->Write();
	HElossInRadPerShell_El->Write();
	HElossInSctPerShell_El->Write();
	HEnergyOut_Ps->Write();
	HElossInRadPerShell_Ps->Write();
	HElossInSctPerShell_Ps->Write();
	HElHits->Write();
	HElHitsDiff->Write();
	HPsHits->Write();
	HPsHitsDiff->Write();
	HPairEnergyRec->Write();
	HPairEnergyRecDiff->Write();

	HRecPairEnergy->Write();
	HRecPairEnergyDiff->Write();
	HRecPairEnergyS->Write();
	HRecPairEnergySDiff->Write();
	HRecPair2EnergyDiff->Write();
	HRecPair2EnergySDiff->Write();
	HRecPair2EnergyDiff_1->Write();
	HRecPair2EnergySDiff_1->Write();
	HRecPairEnergyDebg->Write();
	HPairEnergyHit0Debg->Write();
	HPairEnergyScntDebg->Write();
	HPairVertXYResHt0ScDebg->Write();
	HPairVertZResHt0ScDebg->Write();
	HPairMomReasHt0ScDebg->Write();

	outF.Close();

}
