{
//	Load(compile) libraries
    gSystem->Load("$ROOTSYS/lib/libThread.so");
    gROOT->ProcessLine(".L HCut.C+O");
    gROOT->ProcessLine(".L HJET.C+O");
    HJET *hjet = new HJET();
//	Create tool bar
    TControlBar *bar = new TControlBar("vertical", "HJET analysis");
    bar->AddButton("Waveforms", "hjet->DrawSet(\"W1\");", "Draw to waveforms.");
    bar->AddButton("AT-hist", "hjet->DrawSet(\"H1\");", "Draw AT-histograms.");
    bar->AddButton("ADC-hist", "hjet->DrawSet(\"C1\");", "Draw ADC histograms.");
    bar->AddButton("Recoiled Mass", "hjet->DrawSet(\"R1\");", "Draw recoiled mass histograms.");
    bar->AddButton("Missing Mass", "hjet->DrawSet(\"M1\");", "Draw missing mass squared histograms.");
    bar->AddButton("ATDET", "hjet->DrawSet(\"D1\");", "Draw Ekin-TOF histograms for whole detectotrs.");
    bar->AddButton("ATDET Y/B", "hjet->DrawSet(\"F1\");", "Draw Ekin-TOF histograms for half detectotrs.");
    bar->AddButton("APOS", "hjet->DrawSet(\"A1\");", "Draw Ekin-angle histograms.");
    bar->AddButton("Yellow", "hjet->DrawSet(\"Y1\");", "Draw Yellow good event bunch distribution.");
    bar->AddButton("Blue", "hjet->DrawSet(\"B1\");", "Draw Blue good event bunch distribution.");
    bar->AddButton("Strip", "hjet->DrawSet(\"S1\");", "Draw good event strip distribution.");
    bar->AddButton("Energy", "hjet->DrawSet(\"E1\");", "Draw good event energy distribution.");
    bar->AddButton("Quality", "hjet->DrawSet(\"Q1\");", "Draw time fit quality.");
    bar->AddSeparator();
    bar->AddButton("Next", "hjet->Next();", "Draw next.");
    bar->AddButton("Next 8", "hjet->Next(8);", "Draw next half of a detector.");
    bar->AddButton("Previous", "hjet->Prev();", "Draw previous.");
    bar->AddButton("Prev. 8", "hjet->Prev(8);", "Draw previous half of a detector.");
    bar->AddButton("Info", "hjet->PrintCuts(); hjet->ShowStatistics(); hjet->PrintRawAsym();", 
	"Print cuts, statistics and raw asymmetries");
    bar->AddSeparator();
    bar->AddButton("Reset", "hjet->ResetAllHist();", "Reset all histograms.");
    bar->AddButton("Close", "hjet->SetFile();", "Close Root file with n-tuples.");
    bar->AddButton("Save", "hjet->Save();", "Save all histograms to root file.");
    bar->AddSeparator();
    bar->AddButton("Exit", "hjet->SetFile(); gApplication->Terminate();", "Exit from root.");
    bar->Show();
//	Set parameters
    hjet->TimeCorr = 3.0;
    hjet->CutQuality->Set(0., 5.0);		// like chi^2
    hjet->CutRawAmpl->Set(15., 240.0);		// raw ADC counts not to hit overflow
    hjet->CutTOF->Set(17., 70.0);		// ns
    hjet->CutEkin->Set(1200.0, 4600.0);		// keV
    hjet->CutRmass->Set(0.55, 1.5);		// GeV - should be proton mass =0.938
    hjet->CutMmass->Set(0.70, 1.0);		// GeV^2 - should be proton mass squared =0.880
//	Start data processing
    hjet->SetConfigName(getenv("HJET_CALIB"));
    hjet->SetFile(getenv("HJET_ROOTFILE"));
    hjet->Open(getenv("HJET_DATAFILE"));
}
