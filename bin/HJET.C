#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <malloc.h>
#include <unistd.h>
#include <math.h>
#include <signal.h>
#include <TH1F.h>
#include <TF1.h>
#include <TLine.h>
#include <TStyle.h>
#include <TGraph.h>
#include "HJET.h"


HJET::HJET(char *fname) : TNamed("HJET", "Hydrogen Jet Online Data")
{
    int i;
    
    DataFile = NULL;
    memset(fQueue, 0, sizeof(fQueue));
    QueueTimer = new TTimer(this, 1000, 1);
    DataThread = NULL;
    StopThread = 0;
    RecNum = 0;
    EventCounter = 0;
    Config = NULL;
    Beam[0] = NULL;
    Beam[1] = NULL;
    memset(Event, 0, sizeof(Event));
    NTEvent = NULL;
    memset(NT, 0, sizeof(NT));
    HQ = new TH1F("HQ", "Time fit quality", 500, 0, 20);
    memset(HAT, 0, sizeof(HAT));
    memset(HM, 0, sizeof(HM));
    memset(HMM, 0, sizeof(HMM));
    memset(HADC, 0, sizeof(HADC));
    memset(HATDET, 0, sizeof(HATDET));
	memset(HATDYB, 0, sizeof(HATDYB));
    memset(HANDET, 0, sizeof(HANDET));

    HSTRIP[0] = new TH1F("HSTRIPY", "Good events in strips distribution", MAXSICHAN, 0, MAXSICHAN);
//    HSTRIP[0]->GetXaxis()->SetTitle("Strip number");
//    HSTRIP[0]->GetYaxis()->SetTitle("Number of events");
    HSTRIP[0]->SetFillColor(kOrange-2);
    HSTRIP[1] = new TH1F("HSTRIPB", "Good events in strips distribution", MAXSICHAN, 0, MAXSICHAN);
//    HSTRIP[1]->GetXaxis()->SetTitle("Strip number");
//    HSTRIP[1]->GetYaxis()->SetTitle("Number of events");
    HSTRIP[1]->SetFillColor(kBlue);		// kBlue = 4
    HSTRIP[2] = new TH1F("HSTRIP", "Good events in strips distribution", MAXSICHAN, 0, MAXSICHAN);
    HSTRIP[2]->GetXaxis()->SetTitle("Strip number");
    HSTRIP[2]->GetYaxis()->SetTitle("Number of events");

    HBUNCH[0] = new TH1F("HBUNCHY", "Good events per bunch crossing, YELLOW beam", 120, 0, 120);
    HBUNCH[0]->SetFillColor(kOrange-2);
    HBUNCH[1] = new TH1F("HBUNCHB", "Good events per bunch crossing, BLUE beam", 120, 0, 120);
    HBUNCH[1]->SetFillColor(kBlue);
    HBUNCH[0]->GetXaxis()->SetTitle("Yellow bunch number");
    HBUNCH[1]->GetXaxis()->SetTitle("Blue bunch number");
    HBUNCH[0]->GetYaxis()->SetTitle("Number of events");
    HBUNCH[1]->GetYaxis()->SetTitle("Number of events");
    HE[0]  = new TH1D("HEYJNL", "Good events energy distribution, Yellow,  Jet negative,  left", 100, 0, 6000);
    HE[1]  = new TH1D("HEYJNR", "Good events energy distribution, Yellow,  Jet negative, right", 100, 0, 6000);
    HE[2]  = new TH1D("HEYJPL", "Good events energy distribution, Yellow,  Jet positive,  left", 100, 0, 6000);
    HE[3]  = new TH1D("HEYJPR", "Good events energy distribution, Yellow,  Jet positive, right", 100, 0, 6000);
    HE[4]  = new TH1D("HEYBNL", "Good events energy distribution, Yellow, Beam negative,  left", 100, 0, 6000);
    HE[5]  = new TH1D("HEYBNR", "Good events energy distribution, Yellow, Beam negative, right", 100, 0, 6000);
    HE[6]  = new TH1D("HEYBPL", "Good events energy distribution, Yellow, Beam positive,  left", 100, 0, 6000);
    HE[7]  = new TH1D("HEYBPR", "Good events energy distribution, Yellow, Beam positive, right", 100, 0, 6000);
    HE[8]  = new TH1D("HEBJNL", "Good events energy distribution,   Blue,  Jet negative,  left", 100, 0, 6000);
    HE[9]  = new TH1D("HEBJNR", "Good events energy distribution,   Blue,  Jet negative, right", 100, 0, 6000);
    HE[10] = new TH1D("HEBJPL", "Good events energy distribution,   Blue,  Jet positive,  left", 100, 0, 6000);
    HE[11] = new TH1D("HEBJPR", "Good events energy distribution,   Blue,  Jet positive, right", 100, 0, 6000);
    HE[12] = new TH1D("HEBBNL", "Good events energy distribution,   Blue, Beam negative,  left", 100, 0, 6000);
    HE[13] = new TH1D("HEBBNR", "Good events energy distribution,   Blue, Beam negative, right", 100, 0, 6000);
    HE[14] = new TH1D("HEBBPL", "Good events energy distribution,   Blue, Beam positive,  left", 100, 0, 6000);
    HE[15] = new TH1D("HEBBPR", "Good events energy distribution,   Blue, Beam positive, right", 100, 0, 6000);
    for(i=0; i<8; i++) {
	HE[i]->SetFillColor(kOrange-2);		// was 2
	HE[i+8]->SetFillColor(kBlue);	// was 4
    }
    for (i=0; i<16; i++) {
	HE[i]->GetXaxis()->SetTitle("E_{kin}, keV");
	HE[i]->GetYaxis()->SetTitle("Number of events");    
    }
    Banana = NULL;
    MassCutMin = NULL;
    MassCutMax = NULL;
    Parabola = NULL;
    Canvas = NULL;
	CanvYB = NULL;
    RootFile = NULL;
    strcpy(DrawWhat, "W1");
    SelectChan = 0;
    GoodData = 1;
    ConfigName = NULL;
//	Cuts
    CutQuality = new HCut(0., 5.0);		// like chi^2
    CutRawAmpl = new HCut(15., 240.0);		// raw ADC counts not to hit overflow
    CutTOF = new HCut(17., 100.0);		// ns
    CutEkin = new HCut(700.0, 5000.0);		// keV
    CutRmass = new HCut(0.55, 1.5);		// GeV - should be proton mass =0.938
    CutMmass = new HCut(0.75, 1.0);		// GeV^2 - should be proton mass squared =0.880
//    TimeCorr = 3.0;
    TimeCorr = 0.0;

    signal(SIGTERM, signalHandler);
    signal(SIGINT,  signalHandler);
    signal(SIGKILL, signalHandler);

    Open(fname);
}

HJET::~HJET(void)
{
    unsigned i;
    if (DataFile != NULL) Open(NULL);
    if (RootFile != NULL) SetFile(NULL);
    delete QueueTimer;
    for(i=0; i<MAXQUEUE; i++) if (fQueue[i] != NULL) free(fQueue);
    for(i=0; i<MAXSICHAN; i++) if (HAT[i] != NULL) delete HAT[i];
    for(i=0; i<MAXSICHAN; i++) if (HM[i] != NULL) delete HM[i];
    for(i=0; i<MAXSICHAN; i++) if (HMM[i] != NULL) delete HMM[i];
    for(i=0; i<MAXSICHAN; i++) if (HADC[i] != NULL) delete HADC[i];
    for(i=0; i<DETECTORS; i++) if (HATDET[i] != NULL) delete HATDET[i];
    for(i=0; i<DETECTORS; i++) if (HATDYB[0][i] != NULL) delete HATDYB[0][i];
    for(i=0; i<DETECTORS; i++) if (HATDYB[1][i] != NULL) delete HATDYB[1][i];
    for(i=0; i<DETECTORS; i++) if (HANDET[i] != NULL) delete HANDET[i];
    for(i=0; i<sizeof(HE)/sizeof(TH1D *); i++) delete HE[i];
    delete HQ;
    delete HBUNCH[0];
    delete HBUNCH[1];
    delete HSTRIP[0];
    delete HSTRIP[1];
	delete HSTRIP[2];
    delete CutQuality;
    delete CutRawAmpl;	
    delete CutTOF;	
    delete CutEkin;	
    delete CutRmass;	
    delete CutMmass;	
    if (Banana != NULL) delete Banana;
    if (MassCutMin != NULL) delete MassCutMin;
    if (MassCutMax != NULL) delete MassCutMax;
    if (Parabola != NULL) delete Parabola;
    
    if (ConfigName != NULL) free(ConfigName);
}

void HJET::Save(char *fname)
{
    TFile *f;
    unsigned i;
    char str[64];
    char *_fname;
    
    if (fname != NULL) {
	_fname = fname;
    } else if (Beam[0] != NULL) {
	sprintf(str, "%d.root", Beam[0]->fillNumberM);
	_fname = str;
    } else {
	_fname = (char *) "hjet_hist.root";
    }
    f = new TFile(_fname, "RECREATE");
    if (!f->IsOpen()) {
	printf("Can not open file %s.\n", _fname);
	return;
    }
    if (DataThread != NULL) DataThread->Lock();
    for(i=0; i<MAXSICHAN; i++) {
	if (HAT[i] != NULL) HAT[i]->Write();
	if (HM[i] != NULL) HM[i]->Write();
	if (HMM[i] != NULL) HMM[i]->Write();
    if (HADC[i] != NULL) HADC[i]->Write();
    }
    for(i=0; i<DETECTORS; i++) {
	if (HATDET[i] != NULL) HATDET[i]->Write();
	if (HANDET[i] != NULL) HANDET[i]->Write();
	if (HATDYB[0][i] != NULL) HATDYB[0][i]->Write();
	if (HATDYB[1][i] != NULL) HATDYB[1][i]->Write();
    }
    for(i=0; i < sizeof(HE)/sizeof(TH1D *); i++) HE[i]->Write();
    HBUNCH[0]->Write();
    HBUNCH[1]->Write();
    HSTRIP[0]->Write();
    HSTRIP[1]->Write();
	HSTRIP[2]->Write();
//    HQ->Write();
    f->Close();
    if (DataThread != NULL) DataThread->UnLock();
}

void HJET::ResetAllHist(void)
{
    unsigned i;
    if (DataThread != NULL) DataThread->Lock();
    for(i=0; i<MAXSICHAN; i++) {
	if (HAT[i] != NULL) HAT[i]->Reset();
	if (HM[i] != NULL) HM[i]->Reset();
	if (HMM[i] != NULL) HMM[i]->Reset();
	if (HADC[i] != NULL) HADC[i]->Reset();
    }
    for(i=0; i<DETECTORS; i++) {
	if (HATDET[i] != NULL) HATDET[i]->Reset();
	if (HANDET[i] != NULL) HANDET[i]->Reset();
	if (HATDYB[0][i] != NULL) HATDYB[0][i]->Reset();
	if (HATDYB[1][i] != NULL) HATDYB[1][i]->Reset();
    }
    for(i=0; i < sizeof(HE)/sizeof(TH1D *); i++) HE[i]->Reset();
    HQ->Reset();
    HBUNCH[0]->Reset();
    HBUNCH[1]->Reset();
    HSTRIP[0]->Reset();
    HSTRIP[1]->Reset();
	HSTRIP[2]->Reset();
    if (DataThread != NULL) DataThread->UnLock();
}

void HJET::Open(char *fname)
{
    int i;
/*	First let's close everything	*/
    if (DataThread != NULL) {
	StopThread = 1;
	DataThread->Join();
	sleep(2);
	DataThread = NULL;
    }
    if (DataFile != NULL) {
	fclose(DataFile);
	DataFile = NULL;
    }
    if (Config != NULL) {
	free(Config);
	Config = NULL;
    }
    for (i=0; i<2; i++) if (Beam[i] != NULL) {
	free(Beam[i]);
	Beam[i] = NULL;
    }
    for(i=0; i<MAXWAVEFORMS; i++) if (Event[i] != NULL) {
	free(Event[i]);
	Event[i] = NULL;
    }
    if (NTEvent != NULL) {
	free(NTEvent);
	NTEvent = NULL;
    }
    if (fname == NULL) return;
/*	Then let's try to open file and start analysis	*/
    DataFile = fopen64(fname, "rb");
    if (DataFile == NULL) {
	printf("Can not open file %s : %m.\n", fname);
	return;
    }
    if (Canvas == NULL) {
	Canvas = new TCanvas("cvHJET", fname, 102,0, 650,480);
    } else {
	Canvas->SetTitle(fname);
    }
/*    
    signal(SIGTERM, signalHandler);
    signal(SIGINT,  signalHandler);
    signal(SIGKILL, signalHandler);
*/
    StopThread = 0;
    DataThread = new TThread(DataThreadFun, (void *)this);
    DataThread->Run();
}

void DrawDummy(Option_t *what)
{
    TH1F *hist;
    char str[100];

    sprintf(str, "No such histogram %s", what);
    hist = new TH1F("cvhist", str, 10, 0, 10);
    hist->DrawCopy();
    delete hist;
}

/* 
	Draw events, histograms etc... what = 
Ann - draw AN-histogram for detector nn=1,...
B   - draw bunch distribution of good events BLUE
Cnn - draw ADC counts histogram for each Si strip
Dnn - draw AT-histogram for detector nn=1,...
Enn - draw energy distribution for polarization combination nn=1,...
Hnn - draw AT-histogram for silicon channel nn=1,... as scatter plot
Mnn - draw missing mass squared for silicon channel nn=1,...
Q   - draw time fit quality
Rnn - draw recoiled mass for silicon channel nn=1,...
S   - draw strip distribution of good events
Wnn - draw waveform nn=1,...
Y   - draw bunch distribution of good events YELLOW

*/
void HJET::Draw(Option_t *what)
{
    TH1F *hist;
    int i, j;
    int wfLen;
    char str[100];
    char bp, jp;
    double massCut_min;
    double massCut_max;
	double factor = 23.58654;	// sqrt(1e6/(2*29.98 cm/ns))
    TLine l;
    TF1 *f;
    
    if (Canvas == NULL) {
	printf("\nNo Canvas !!!\n");
	return;
    }
    Canvas->cd();
    Canvas->SetLogz(0);
    l.SetLineColor(3);
    l.SetLineWidth(2);    
    gStyle->SetOptFit();
    if (DataThread != NULL) DataThread->Lock();
    switch(toupper(what[0])) {
    case 'A':	// Detecor A-angle
	j = (strtol(&what[1], NULL, 0)-1) % DETECTORS;
	if (HANDET[j] != NULL) {
	    HANDET[j]->SetStats(0);
	    HANDET[j]->Draw("box");
	    CutEkin->Draw(1);
	    if (Parabola == NULL) Parabola = new TF1("Parabola", "1840000*sin(x)*sin(x)", -1, 1);
	    Parabola->SetLineColor(3);
	    Parabola->SetLineWidth(2);    
	    Parabola->Draw("same");
	} else {
	    DrawDummy(what);
	}
	break;
    case 'B':	// Blue bunches
	HBUNCH[1]->Draw();
	break;
	case 'C':      // ADC histograms
	j = (strtol(&what[1], NULL, 0)-1) % MAXSICHAN;
    if (HADC[j] != NULL) {
     HADC[j]->Draw();	
    }
	else {
	 DrawDummy(what);
	}
	break;
    case 'D':	// Detector AT
	j = (strtol(&what[1], NULL, 0)-1) % DETECTORS;
	if (HATDET[j] != NULL) {
	    Canvas->SetLogz(1);
	    HATDET[j]->Draw("color");
	    CutTOF->Draw(1);
	    CutEkin->Draw(0);
	    sprintf(str, "%f/sqrt(x)", 22.8*Config->TOFLength);
	    if (Banana == NULL) Banana = new TF1("Banana", str, 0, 6000);
	    Banana->SetLineColor(3);
	    Banana->SetLineWidth(2);    
	    Banana->Draw("same");

// Plot the mass cut ranges...
		CutRmass->Get(&massCut_min, &massCut_max);
		
		sprintf(str, "%f/sqrt(x)", factor*sqrt(massCut_min)*Config->TOFLength);
		if (MassCutMin == NULL) MassCutMin = new TF1("MassCutMin", str, 0, 6000);
		MassCutMin->SetLineColor(kRed);
		MassCutMin->SetLineWidth(2);
		MassCutMin->Draw("same");
		
		sprintf(str, "%f/sqrt(x)", factor*sqrt(massCut_max)*Config->TOFLength);
		if (MassCutMax == NULL) MassCutMax = new TF1("MassCutMax", str, 0, 6000);
		MassCutMax->SetLineColor(kRed);
		MassCutMax->SetLineWidth(2);
		MassCutMax->Draw("same");

	} else {
	    DrawDummy(what);
	}
	break;
    case 'E':	// Energy
	j = (strtol(&what[1], NULL, 0)-1) % (sizeof(HE)/sizeof(TH1D *));
	f = new TF1("fit_fun", "[0]*exp(-0.000001876*[1]*x)", 0, 6000);
	f->SetParName(0, "Constant");
	f->SetParName(1, "Slope, (GeV/c)^{-2}");
	HE[j]->Fit("fit_fun");
	break;

    case 'F':	// Detector AT for Y/B plot
	j = (strtol(&what[1], NULL, 0)-1) % DETECTORS;
    if (CanvYB == NULL) {
	CanvYB = new TCanvas("cvDYB", "Yellow/Blue", 102,0, 650,850);
	CanvYB->Divide(1,2);
    } else {
	CanvYB->SetTitle("Yellow/Blue");
    }
// Draw Yellow half of detector...
	CanvYB->cd(1);
	if (HATDYB[0][j] != NULL) {
	    gPad->SetLogz(1);
	    HATDYB[0][j]->Draw("color");
	    CutTOF->Draw(1);
	    CutEkin->Draw(0);
	    sprintf(str, "%f/sqrt(x)", 22.8*Config->TOFLength);
	    if (Banana == NULL) Banana = new TF1("Banana", str, 0, 6000);
	    Banana->SetLineColor(3);
	    Banana->SetLineWidth(2);    
	    Banana->Draw("same");

// Plot the mass cut ranges...
		CutRmass->Get(&massCut_min, &massCut_max);
		
		sprintf(str, "%f/sqrt(x)", factor*sqrt(massCut_min)*Config->TOFLength);
		if (MassCutMin == NULL) MassCutMin = new TF1("MassCutMin", str, 0, 6000);
		MassCutMin->SetLineColor(kRed);
		MassCutMin->SetLineWidth(2);
		MassCutMin->Draw("same");
		
		sprintf(str, "%f/sqrt(x)", factor*sqrt(massCut_max)*Config->TOFLength);
		if (MassCutMax == NULL) MassCutMax = new TF1("MassCutMax", str, 0, 6000);
		MassCutMax->SetLineColor(kRed);
		MassCutMax->SetLineWidth(2);
		MassCutMax->Draw("same");

	} else {
	    DrawDummy(what);
	}
// Draw Blue half of detector...
	CanvYB->cd(2);
	if (HATDYB[1][j] != NULL) {
	    gPad->SetLogz(1);
	    HATDYB[1][j]->Draw("color");
	    CutTOF->Draw(1);
	    CutEkin->Draw(0);
	    sprintf(str, "%f/sqrt(x)", 22.8*Config->TOFLength);
	    if (Banana == NULL) Banana = new TF1("Banana", str, 0, 6000);
	    Banana->SetLineColor(3);
	    Banana->SetLineWidth(2);    
	    Banana->Draw("same");

// Plot the mass cut ranges...
		CutRmass->Get(&massCut_min, &massCut_max);
		
		sprintf(str, "%f/sqrt(x)", factor*sqrt(massCut_min)*Config->TOFLength);
		if (MassCutMin == NULL) MassCutMin = new TF1("MassCutMin", str, 0, 6000);
		MassCutMin->SetLineColor(kRed);
		MassCutMin->SetLineWidth(2);
		MassCutMin->Draw("same");
		
		sprintf(str, "%f/sqrt(x)", factor*sqrt(massCut_max)*Config->TOFLength);
		if (MassCutMax == NULL) MassCutMax = new TF1("MassCutMax", str, 0, 6000);
		MassCutMax->SetLineColor(kRed);
		MassCutMax->SetLineWidth(2);
		MassCutMax->Draw("same");

	} else {
	    DrawDummy(what);
	}
	CanvYB->cd(0);	// Just to look nicer
	CanvYB->Update();
	break;

    case 'H':	// Strip AT
	j = (strtol(&what[1], NULL, 0)-1) % MAXSICHAN;
	if (HAT[j] != NULL) {
	    Canvas->SetLogz(1);
	    HAT[j]->Draw("color");
	    CutRawAmpl->Draw();
	} else {
	    DrawDummy(what);
	}
	break;
    case 'M':	// Missing mass
	j = (strtol(&what[1], NULL, 0)-1) % MAXSICHAN;
	if (HMM[j] != NULL) {
	    HMM[j]->Draw();
	    CutMmass->Draw();
	    l.DrawLine(0.880, -LARGE_VALUE, 0.880, LARGE_VALUE);
	} else {
	    DrawDummy(what);
	}
	break;
    case 'Q':	// Quality
	HQ->Draw();
	CutQuality->Draw();
	break;
    case 'R':	// Recoiled mass
	j = (strtol(&what[1], NULL, 0)-1) % MAXSICHAN;
	if (HM[j] != NULL) {
	    HM[j]->Draw();
	    CutRmass->Draw();
	    l.DrawLine(0.938, -LARGE_VALUE, 0.938, LARGE_VALUE);
	} else {
	    DrawDummy(what);
	}
	break;
    case 'S':	// Strip distribution
	HSTRIP[2]->Draw();
	HSTRIP[0]->Draw("same");
	HSTRIP[1]->Draw("same");
	break;
    case 'W': // draw raw event #(what[1...]%MAXWAVEFORMS)
	j = (strtol(&what[1], NULL, 0)-1) % MAXWAVEFORMS;
	wfLen = (Config->JetDelay & 0xFF) * 6;	// waveform length in points
	if (Event[j]->bpol > 0) {
	    bp = '+';
	} else if (Event[j]->bpol < 0) {
	    bp = '-';
	} else {
	    bp = '0';
	}
	if (Event[j]->jpol > 0) {
	    jp = '+';
	} else if (Event[j]->jpol < 0) {
	    jp = '-';
	} else {
	    jp = '0';
	}
	sprintf(str, "Si #%d evt %d/%d %s B=%d BPol=%c JPol=%c T=%4.1f A=%4.1f rev=%d Q=%g",
	    Event[j]->chan+1, j+1, MAXWAVEFORMS, 
	    (Event[j]->quality) < 0 ? "OLD" : "NEW",
	    Event[j]->bunch, bp, jp, Event[j]->time, Event[j]->ampl,
	    Event[j]->rev, Event[j]->quality);
	hist = new TH1F("cvhist", str, wfLen, 0, wfLen);
	for(i=0; i<wfLen; i++) hist->SetBinContent(i+1, Event[j]->wave[i]);
	hist->SetStats(0);
	hist->SetMinimum(0);
	hist->SetMaximum(255);
	hist->DrawCopy("*H");
	delete hist;
	break;
    case 'Y':	// yellow bunches
	HBUNCH[0]->Draw();
	break;
    }
    if (DataThread != NULL) DataThread->UnLock();
    Canvas->Update();
}

void HJET::ShowStatistics(void)
{
    if (Beam[0] == NULL) {
	printf("The startup. No beam data record yet. %d records so far.\n", RecNum);
    } else {
	printf("Fill:%d: %d records, Events: total = %d, Good Yellow = %d, Blue = %d.\n", 
	    Beam[0]->fillNumberM, RecNum, EventCounter, 
	    (int)HBUNCH[0]->GetEntries(), (int)HBUNCH[1]->GetEntries());
    }
}

void SQAsymCalc(double A, double B, double C, double D, double *val, double *err)
{
    double terma, termb;
    terma = sqrt(A*B);
    termb = sqrt(C*D);
    
    *val = (terma - termb) / (terma + termb);
    *err = sqrt(A*B*(C+D) + C*D*(A+B)) / ((terma + termb)*(terma + termb));
}

void HJET::PrintRawAsym(void)
{
    FILE *fsummary;
    char *strA;
    char strB[60];

    double yjval, yjerr, ybval, yberr, yval, yerr;	// Physical asymmetries (yellow)
    double ajval, ajerr, abval, aberr, rval, rerr;	// Physical asymmetries
    double bjval, bjerr, bbval, bberr;			// Acceptance asymmetries
    double cjval, cjerr, cbval, cberr;			// Luminosity asymmetries
    double nupleft, nupright, ndnleft, ndnright;
    
    ndnleft = HE[0]->GetEntries();
    ndnright = HE[1]->GetEntries();
    nupleft = HE[2]->GetEntries();
    nupright = HE[3]->GetEntries();
    SQAsymCalc(nupleft, ndnright, ndnleft, nupright, &ajval, &ajerr);
    SQAsymCalc(nupleft, ndnleft, nupright, ndnright, &bjval, &bjerr);
    SQAsymCalc(nupleft, nupright, ndnleft, ndnright, &cjval, &cjerr);
    ndnleft = HE[4]->GetEntries();
    ndnright = HE[5]->GetEntries();
    nupleft = HE[6]->GetEntries();
    nupright = HE[7]->GetEntries();
    SQAsymCalc(nupleft, ndnright, ndnleft, nupright, &abval, &aberr);
    SQAsymCalc(nupleft, ndnleft, nupright, ndnright, &bbval, &bberr);
    SQAsymCalc(nupleft, nupright, ndnleft, ndnright, &cbval, &cberr);
    abval = - abval;	// we measure recoiled partice
    rval = abval/ajval;
    rerr = rval*sqrt((aberr/abval)*(aberr/abval) + (ajerr/ajval)*(ajerr/ajval));
    printf("Yellow beam: A_jet = %7.4f ± %6.4f,  A_beam = %7.4f ± %6.4f,  beam/jet = %7.3f ± %6.3f\n",
	ajval, ajerr, abval, aberr, rval, rerr);
// Copy to save for later...	
	yjval = ajval;
	yjerr = ajerr;
	ybval = abval;
	yberr = aberr;
	yval = rval;
	yerr = rerr;
	
    printf("         A_jet_acc = %7.4f ± %6.4f, A_beam_acc = %7.4f ± %6.4f\n",
	bjval, bjerr, bbval, bberr); 
    printf("         A_jet_lum = %7.4f ± %6.4f, A_beam_lum = %7.4f ± %6.4f\n",
	cjval, cjerr, cbval, cberr);
    ndnleft = HE[8]->GetEntries();
    ndnright = HE[9]->GetEntries();
    nupleft = HE[10]->GetEntries();
    nupright = HE[11]->GetEntries();
    SQAsymCalc(nupleft, ndnright, ndnleft, nupright, &ajval, &ajerr);
    SQAsymCalc(nupleft, ndnleft, nupright, ndnright, &bjval, &bjerr);
    SQAsymCalc(nupleft, nupright, ndnleft, ndnright, &cjval, &cjerr);
    ndnleft = HE[12]->GetEntries();
    ndnright = HE[13]->GetEntries();
    nupleft = HE[14]->GetEntries();
    nupright = HE[15]->GetEntries();
    SQAsymCalc(nupleft, ndnright, ndnleft, nupright, &abval, &aberr);
    SQAsymCalc(nupleft, ndnleft, nupright, ndnright, &bbval, &bberr);
    SQAsymCalc(nupleft, nupright, ndnleft, ndnright, &cbval, &cberr);
    abval = - abval;	// we measure recoiled partice
    rval = abval/ajval;
    rerr = rval*sqrt((aberr/abval)*(aberr/abval) + (ajerr/ajval)*(ajerr/ajval));
    printf("  Blue beam: A_jet = %7.4f ± %6.4f,  A_beam = %7.4f ± %6.4f,  beam/jet = %7.3f ± %6.3f\n",
	ajval, ajerr, abval, aberr, rval, rerr);
    printf("         A_jet_acc = %7.4f ± %6.4f, A_beam_acc = %7.4f ± %6.4f\n",
	bjval, bjerr, bbval, bberr); 
    printf("         A_jet_lum = %7.4f ± %6.4f, A_beam_lum = %7.4f ± %6.4f\n",
	cjval, cjerr, cbval, cberr);
	
// If this is a DAQ job, write some results to a temporary file.
    if (strlen(getenv("HJET_DAQ")) > 0) {
     strA = getenv("HJET_DAQ");
     sprintf(strB,".%s",strA);
     fsummary = fopen(strB,"w");
     fprintf(fsummary,"%8d %8d %7.4f %6.4f %7.4f %6.4f %7.3f %6.3f %7.4f %6.4f %7.4f %6.4f %7.3f %6.3f\n", (int)HBUNCH[0]->GetEntries(), (int)HBUNCH[1]->GetEntries(), yjval, yjerr, ybval, yberr, yval, yerr, ajval, ajerr, abval, aberr, rval, rerr);
     fclose(fsummary);
    }
}

void HJET::PrintCuts(void)
{
    CutQuality->Print("Quality");
    CutRawAmpl->Print("Raw Amplitude");
    CutEkin->Print("Ekin, keV");
    CutTOF->Print("TOF ,ns");
    CutRmass->Print("Recoiled mass, GeV");
    CutMmass->Print("Missing mass squared, GeV^2");
    printf("Time correction %f ns\n", TimeCorr);
}

void HJET::SetConfigName(char *cname)
{
    if (ConfigName != NULL) free(ConfigName);
    if (cname == NULL) {
	ConfigName = NULL;
    } else {
	ConfigName = (char *) malloc(strlen(cname)+1);
	if (ConfigName != NULL) strcpy(ConfigName, cname);
    }
}

/*	Read t0, ecoef and edead from the standard configuration file
    (this overrides valuse in the data stream				*/
void HJET::ReadConfig(char *cname)
{
    FILE *f;
    int i;
    float f1, f2, f3;
    char str[256];
    const char chan[] = "Channel";
    
    if (cname == NULL) return;
    if (Config == NULL) {
	SetConfigName(cname);
	return;
    }
    
    f = fopen(cname, "rt");
    if (f == NULL) {
	printf("Can not open file %s - %m.\n", cname);
	return;
    }
    for(;!feof(f);) {
	fgets(str, sizeof(str), f);
	if(!strncmp(str, chan, sizeof(chan))) continue;
	sscanf(&str[sizeof(chan)-1], "%d=%f %f %f", &i, &f1, &f2, &f3);
	if (i > 0 && i <= MAXSICHAN) {
	    Config->chan[i-1].t0 = f1;
	    Config->chan[i-1].ecoef = f2;
	    Config->chan[i-1].edead = f3;
	}
    }
    fclose(f);
}

/*	Find waveform maximum	*/
float FindMax(unsigned char *wave, int len)
{
    int i;
    unsigned char mx;
    mx = 0;
    for(i=0; i<len; i++) if (mx < wave[i]) mx = wave[i];
    return (float) mx-V10BASELINE;	// subtract baselevel here
}
/*
// Find integral of waveform
float FindIntg(unsigned char *wave, int len)
{
  int i;
  float mx;
  mx = 0.;
  for(i=0; i<len; i++) mx += (float)wave[i];
  return mx;
}
*/

/*	Find time of 1/2 maximum crossing, using line fit	*/
float FindTime(unsigned char *wave, int len, float ampl, float &q)
{
    int i, ib, ie;
    unsigned char mh;
    float a, b, c;
    float x, x2, xy, y, t;
    
    mh  = (unsigned)(ampl/2.) + V10BASELINE;
    for(i=0; i<len; i++) if (wave[i] > mh) break;
    ib = i - LINEPOINTS/2;
    ie = i + LINEPOINTS/2;
    if (ib < 0) {
	ib = 0;
	ie = ib + LINEPOINTS;
    }
    if (ie > len) {
	ie = len;
	ib = ie - LINEPOINTS;
    }
    
    x = 0;
    y = 0;
    x2 = 0;
    xy = 0;
    
    for(i=ib; i<ie; i++) {
	x += i;
	y += wave[i];
	x2 += i*i;
	xy += i*wave[i];
    }
    
    x /= LINEPOINTS;
    y /= LINEPOINTS;
    x2 /= LINEPOINTS;
    xy /= LINEPOINTS;
    
    a = (xy - x*y) / (x2 - x*x);
    b = (y*x2 - x*xy) / (x2 - x*x);
    c = V10BASELINE;	// normalize time start to baseline
    
    q = 0;
    for(i=ib; i<ie; i++) q += (a*i+b - wave[i])*(a*i+b - wave[i]);
    q /= LINEPOINTS - 2;
    
    t = (c - b)/a;
    return t;
}

/*	Create event tree	*/
TTree *HJET::CreateTree(int chan)
{
    TTree *t;
    char strs[10], strl[100];
    sprintf(strs, "HJtree%d", chan+1);
    sprintf(strl, "Hydrogen jet event tree Si%d", chan+1);
    RootFile->cd();		// in root-file
    t = new TTree(strs, strl);
//    t->Branch("Events", &NTEvent->rev, "R/I3:B/I1:BPOL/I1:JPOL/I1:T/F:A/F:Q/F:E/F:TOF/F:M/F:ANG/F:MM/F:W[192]/I");
    t->Branch("Events", &NTEvent->rev, "R/I3:B/I1:BPOL/I1:JPOL/I1:T/F:A/F:Q/F:E/F:TOF/F:M/F:ANG/F:MM/F");
    return t;
}

/*	Create time versus amplitude histogram	*/
TH2F *HJET::CreateHistAT(int chan)
{
    TH2F *h;
    char strs[10], strl[100];
    int amax, tmax;
    
    sprintf(strs, "HAT%d", chan+1);
    sprintf(strl, "Hydrogen jet Time versus Amplitude Si%d", chan+1);
    amax = 256 - V10BASELINE;
    tmax = (2 - Config->CSR.split.B120)*45;	// maximum time: 45/90 for 60/120 bunch mode
//    gROOT->cd();		// in memory
    h = new TH2F(strs, strl, amax, 0, amax, tmax, 0, tmax);
    h->GetXaxis()->SetTitle("Amplitude, WFD units");
    h->GetYaxis()->SetTitle("Time, WFD time units");
    return h;
}

/*	Create recoiled mass histogram	*/
TH1F *HJET::CreateHistM(int chan)
{
    TH1F *h;
    char strs[10], strl[100];
    
    sprintf(strs, "HM%d", chan+1);
    sprintf(strl, "Recoiled mass Si%d", chan+1);
//    gROOT->cd();		// in memory
    h = new TH1F(strs, strl, 100, 0, 5);	// 0-5 GeV
    h->GetXaxis()->SetTitle("GeV");
    h->GetYaxis()->SetTitle("Number of events");
    return h;
}

/*	Create missing mass squared histogram	*/
TH1F *HJET::CreateHistMM(int chan)
{
    TH1F *h;
    char strs[10], strl[100];
    
    sprintf(strs, "HMM%d", chan+1);
    sprintf(strl, "Missing mass squared Si%d", chan+1);
//    gROOT->cd();		// in memory
    h = new TH1F(strs, strl, 100, 0, 2);	// 0-2 GeV
    h->GetXaxis()->SetTitle("GeV^{2}");
    h->GetYaxis()->SetTitle("Number of events");
    return h;
}

/*	Create time versus amplitude histogram	for whole detectors	*/
TH2F *HJET::CreateHistATDet(int det)
{
    TH2F *h;
    char strs[10], strl[100];
    int tmax;
    
    sprintf(strs, "HATDET%d", det+1);
    sprintf(strl, "Hydrogen jet Time versus Amplitude Det%d", det+1);
    tmax = (2 - Config->CSR.split.B120)*45;	// maximum time: 45/90 for 60/120 bunch mode
//    gROOT->cd();		// in memory
    h = new TH2F(strs, strl, 100, 0, 6000, tmax, 0, tmax*Config->WFDTUnit);	// 6000 kev * 107.1 ns
    h->GetXaxis()->SetTitle("E_{kin}, keV");
    h->GetYaxis()->SetTitle("TOF, ns");
    return h;
}

/*	Create time versus amplitude histogram for Y/B stacked plots	*/
TH2F *HJET::CreateHistATYB(int k, int det)
{
    TH2F *h;
    char strs[10], strl[100];
    int tmax;
    
    sprintf(strs, "HAT%s%d", (k == 0) ? "Y" : "B", det+1);
    sprintf(strl, "Hydrogen jet Time versus Amplitude Det%d %s", det+1, (k == 0) ? "Yellow" : "Blue");
    tmax = (2 - Config->CSR.split.B120)*45;	// maximum time: 45/90 for 60/120 bunch mode
//    gROOT->cd();		// in memory
    h = new TH2F(strs, strl, 100, 0, 6000, tmax, 0, tmax*Config->WFDTUnit);	// 6000 kev * 107.1 ns
    h->GetXaxis()->SetTitle("E_{kin}, keV");
    h->GetYaxis()->SetTitle("TOF, ns");
    return h;
}

/*	Create amplitude versus position histogram for whole detectors	*/
TH2F *HJET::CreateHistANDet(int det)
{
    TH2F *h;
    char strs[10], strl[100];
    float angmax;
    
    sprintf(strs, "HANDET%d", det+1);
    sprintf(strl, "Hydrogen jet Energy versus angle Det%d", det+1);
    // angmax = 3.78/Config->TOFLength;
    // h = new TH2F(strs, strl, 17, -angmax, angmax, 50, 0, 6000);
    angmax = 10*0.42/Config->TOFLength;
    h = new TH2F(strs, strl, 40, -angmax, angmax, 50, 0, 6000);
    h->GetXaxis()->SetTitle("rad");
    h->GetYaxis()->SetTitle("E_{kin}, keV");
    h->SetFillColor(4);
    
    return h;
}

/*  Create histogram of ADC counts, i.e., histogram of NTEvent->ampl  */
TH1F *HJET::CreateHistC(int chan)
{
    TH1F *h;
    char strs[10], strl[100];
    
    int nchan = chan + 1;
    int ndet = (int)floor(chan/16) + 1;
    int nstrip = (chan % 16) + 1;
    
    sprintf(strs, "HADC%d", nchan);
    sprintf(strl, "ADC Events Si%d (Det %d Strip %d)", nchan, ndet, nstrip);
//    gROOT->cd();		// in memory
    h = new TH1F(strs, strl, 256, 0, 256);  // Full range
    h->GetXaxis()->SetTitle("ADC Channel");
    h->GetXaxis()->CenterTitle();
    h->GetYaxis()->SetTitle("Number of events");
    h->GetYaxis()->CenterTitle();
    return h;
}

//
//	Emperical fit to energy loss:
//    18.2 + 215/(E+0.405) keV*cm^2/mg for E in MeV in the range 0.1 - 7 MeV 
//    Input for this function is energy in keV
float energy_loss(float energy)
{
    return (18.2 + 215000.0/(energy + 405.0));
}

/*	Fill histogramms and n-tuples	*/
void HJET::FillEvent()
{
    float c = 29.9792458;	// c, cm/ns
    float M = 0.938;	// proton mass, GeV
    float pitch = 0.42;	// strips pitch, cm
    float angcorr = 9.0;// magnetic field correction, cm*MeV/c
    float E;    // beam energy, GeV
    float beta; // Recoil v/c
    int det;
    int indet;
    int i, j, k;

    if (NTEvent->chan < 0 || NTEvent->chan > MAXSICHAN) return;

    det = NTEvent->chan / 16;
    indet = NTEvent->chan % 16;
    E = Beam[(indet > 7) ? 1 : 0]->beamEnergyM;
    NTEvent->ekin = Config->chan[NTEvent->chan].ecoef * NTEvent->ampl;
    NTEvent->ekin += energy_loss(NTEvent->ekin)*Config->chan[NTEvent->chan].edead;	// dead layer correction
    NTEvent->tof = NTEvent->time*Config->WFDTUnit - Config->chan[NTEvent->chan].t0 + TimeCorr;
    beta = Config->TOFLength/(c*NTEvent->tof);
    NTEvent->rmass = 2.0E-6*NTEvent->ekin/(beta*beta);
    NTEvent->angle = ((indet > 7) ? (indet - 6.5) : (indet - 8.5)) * pitch/Config->TOFLength;
    NTEvent->angle += ((det > 2) ? 1. : -1.)*angcorr/(sqrt(2*M*NTEvent->ekin)*Config->TOFLength);// magnetic field
// approximation: not relativistic for recoiled
    NTEvent->mmass2 = M*M - 2.0E-6*(M+E)*NTEvent->ekin + 
	2*sqrt(E*E-M*M)*sqrt(2.0E-6*M*NTEvent->ekin)*fabs(sin(NTEvent->angle));

    DataThread->Lock();
    HQ->Fill(NTEvent->quality);

    if (CutQuality->Cut(NTEvent->quality)) {
	if (HAT[NTEvent->chan] == NULL) HAT[NTEvent->chan] = CreateHistAT(NTEvent->chan);
	HAT[NTEvent->chan]->Fill(NTEvent->ampl, NTEvent->time);

	if (HM[NTEvent->chan] == NULL) HM[NTEvent->chan] = CreateHistM(NTEvent->chan);
	if (CutTOF->Cut(NTEvent->tof) && CutEkin->Cut(NTEvent->ekin) && CutRawAmpl->Cut(NTEvent->ampl)) 
	    HM[NTEvent->chan]->Fill(NTEvent->rmass);

    if (HMM[NTEvent->chan] == NULL) HMM[NTEvent->chan] = CreateHistMM(NTEvent->chan);
	if (CutTOF->Cut(NTEvent->tof) && CutEkin->Cut(NTEvent->ekin) && CutRawAmpl->Cut(NTEvent->ampl) 
	    && CutRmass->Cut(NTEvent->rmass)) HMM[NTEvent->chan]->Fill(NTEvent->mmass2);

    if (HADC[NTEvent->chan] == NULL) HADC[NTEvent->chan] = CreateHistC(NTEvent->chan);
    HADC[NTEvent->chan]->Fill(NTEvent->ampl);
    
    if (HATDET[det] == NULL) HATDET[det] = CreateHistATDet(det);
	HATDET[det]->Fill(NTEvent->ekin, NTEvent->tof);

	k = ((NTEvent->chan/8)%2 == 0) ? 0 : 1;
    if (HATDYB[k][det] == NULL) HATDYB[k][det] = CreateHistATYB(k,det);
	HATDYB[k][det]->Fill(NTEvent->ekin, NTEvent->tof);

    if (HANDET[det] == NULL) HANDET[det] = CreateHistANDet(det);
	if (CutTOF->Cut(NTEvent->tof) && CutEkin->Cut(NTEvent->ekin) && CutRawAmpl->Cut(NTEvent->ampl) && 
	    CutRmass->Cut(NTEvent->rmass) && CutMmass->Cut(NTEvent->mmass2)) 
	    HANDET[det]->Fill(NTEvent->angle, NTEvent->ekin);

	if (CutTOF->Cut(NTEvent->tof) && CutEkin->Cut(NTEvent->ekin) && CutRawAmpl->Cut(NTEvent->ampl) && 
	    CutRmass->Cut(NTEvent->rmass) ) {  // && CutMmass->Cut(NTEvent->mmass2)) {
        k = ((NTEvent->chan/8)%2 == 0) ? 0 : 1;
	    HSTRIP[k]->Fill(NTEvent->chan);		// Yellow or blue strips
		HSTRIP[2]->Fill(NTEvent->chan);		// Both yellow and blue strips
	    HBUNCH[indet/8]->Fill(NTEvent->bunch);
	    if (NTEvent->bpol != 0 && NTEvent->jpol != 0) {
		i = (indet > 7) ? 8 : 0;
		k = ((indet > 7 && det <= 2) || (indet <= 7 && det > 2)) ? 1 : 0;
		j = (NTEvent->jpol > 0) ? 2 : 0;
		HE[i+j+k]->Fill(NTEvent->ekin);
		j = (NTEvent->bpol > 0) ? 2 : 0;
		HE[i+j+k+4]->Fill(NTEvent->ekin);
	    }
	}
    }
/*	This function is called always from DataThread ...*/
    if (RootFile != NULL) {
	if (NT[NTEvent->chan] == NULL) NT[NTEvent->chan] = CreateTree(NTEvent->chan);
	NT[NTEvent->chan]->Fill();
    }
    DataThread->UnLock();
}

Bool_t HJET::HandleTimer(TTimer *)
{
    if (DataFile == NULL && fQueue[0] != NULL) {
	Open(fQueue[0]);
	free(fQueue[0]);
	memmove(fQueue, &fQueue[1], sizeof(fQueue) - sizeof(fQueue[0]));
    }
    if (fQueue[0] == NULL) {
	QueueTimer->TurnOff();
    }
    return kFALSE;
}

void HJET::Queue(char *fname)
{
    int i;
    if (DataThread != NULL) DataThread->Lock();
    for(i=0; i<MAXQUEUE; i++) if (fQueue[i] == NULL) break;
    if (i == MAXQUEUE) {
	printf("Too many (%d) files already in the queue. Try later.\n", MAXQUEUE);
    } else {
	fQueue[i] = (char *)malloc(strlen(fname) + 1);
	if (fQueue[i] == NULL) {
	    printf("Not enough memory.");
	} else {
	    strcpy(fQueue[i], fname);
	}
    }
    if (DataThread != NULL) DataThread->UnLock();
    QueueTimer->TurnOn();
}

void HJET::PrintQueue(void)
{
    int i;
    printf("Processing queue:\n");
    if (DataThread != NULL) DataThread->Lock();
    for(i=0; i<MAXQUEUE; i++) if (fQueue[i] != NULL) printf("%d : %s\n", i+1, fQueue[i]);
    if (DataThread != NULL) DataThread->UnLock();    
}

/*	Open file for n-tuple	*/
void HJET::SetFile(char *fname)
{
    int i;
    if (DataThread != NULL) DataThread->Lock();
    if (RootFile != NULL) {
/*	Save and close n-tuples	*/
	for(i=0; i<MAXSICHAN; i++) if (NT[i] != NULL) {
	    NT[i]->Write();
	    delete NT[i];
	    NT[i] = NULL;
	}

    for(i=0; i < floor(sizeof(HE)/sizeof(TH1D *)); i++) HE[i]->Write();
    HBUNCH[0]->Write();
    HBUNCH[1]->Write();
    HSTRIP[0]->Write();
    HSTRIP[1]->Write();
	HSTRIP[2]->Write();
//    HQ->Write();

	printf("Saving and closing %s\n", RootFile->GetName());
	RootFile->Write();
	RootFile->Close();
	RootFile = NULL;
    }
/*	Try to open new file	*/
	if (strlen(getenv("HJET_DAQ")) > 0) {	// Only if a DAQ run
     if (fname != NULL) {
	  RootFile = new TFile(fname, "RECREATE");
	  if (!RootFile->IsOpen()) {
	    delete RootFile;
	    RootFile = NULL;
	  }
     }
	}
    if (DataThread != NULL) DataThread->UnLock();
}

/*	Get event information	*/
void HJET::ProcessEvent(int chan, longWaveStruct *data)
{
    unsigned char wfLen;
    int bcorr;
    
    wfLen = data->length*6;
    memcpy(NTEvent->wave, data->d, wfLen);
    NTEvent->chan = chan;
    NTEvent->rev = (data->revh << 13) + (data->revm << 5) + data->revl;
    NTEvent->bunch = ((data->bh << 4) + data->bl) % 120;
    NTEvent->jpol = JetPol;
    NTEvent->ampl = FindMax(NTEvent->wave, wfLen);
//    NTEvent->intg = FindIntg(NTEvent->wave, wfLen);
    NTEvent->time = FindTime(NTEvent->wave, wfLen, NTEvent->ampl, NTEvent->quality);
//	Remove unreasonable times
    if (NTEvent->time < 0) NTEvent->time = 0;
    if (NTEvent->time > wfLen) NTEvent->time = wfLen;

    NTEvent->time += (data->trt - data->before)*6;
//	Correct time and bunch number
    if (Config->CSR.split.B120 != 0) {
	if (NTEvent->time < 0) {
	    bcorr = (int)((-NTEvent->time) / 45);
	    bcorr = -bcorr - 1;
	} else {
	    bcorr = (int)(NTEvent->time / 45);	// 45 points per 120-mode bunch crossing
	}
	NTEvent->bunch += bcorr;	
	NTEvent->time -= bcorr*45;	    
    } else {
	if (NTEvent->time < 0) {
	    bcorr = (int)((-NTEvent->time) / 90);	// 90 points per 60-mode bunch crossing
	    bcorr = -bcorr - 1;	    
	} else {
	    bcorr = (int)(NTEvent->time / 90);	// 90 points per 60-mode bunch crossing
	} 
	NTEvent->bunch += 2*bcorr;	// only even bunches in 60-bunch mode
	NTEvent->time -= bcorr*90;
    }
    if (NTEvent->bunch >= 120) {
	NTEvent->bunch -= 120;
	NTEvent->rev++;
    }
    if (NTEvent->bunch < 0) {
	NTEvent->bunch += 120;
	NTEvent->rev--;
    }    
    if ((chan%16) < 8) {	// yellow pattern
	NTEvent->bpol = (Beam[0] == NULL) ? 0 : Beam[0]->polarizationFillPatternS[3*NTEvent->bunch];
    } else {			// blue pattern
	NTEvent->bunch = (NTEvent->bunch + BUNCHCORR) % 120;
	NTEvent->bpol = (Beam[1] == NULL) ? 0 : Beam[1]->polarizationFillPatternS[3*NTEvent->bunch];
    }
}

/*	Read a complete record	
    Return: 	<=0 	error
		1	OK
		2	end of file if queue is not empty
*/
int HJET::RecRead(void *buf, size_t len)
{
    int irc;
    size_t i;
    for(i=0; i<len && StopThread == 0; i+=irc) {
	irc = fread(&((char *)buf)[i], 1, len-i, DataFile);
	if (irc < 0) return irc;
	if (irc == 0 && fQueue[0] != NULL) return 2;
	if (irc == 0) sleep(1);
    }
    return 1;
}

void *DataThreadFun(void *arg)
{
    ReadBufferStruct *rec;
    HJET *hjet;
    recordLongWaveStruct *longWave;
    longWaveStruct *Event;
    int i, k, jj;
    int j = 0;
    unsigned wfLen = 0;
    unsigned wfDelay = 0;
    unsigned subLen;
    int err, evt;
    int errtot;
    const int jet2pol[4] = {0, 1, -1, 0};
    char *str;

    hjet = (HJET *)arg;
    rec = (ReadBufferStruct *)malloc(BSIZE*sizeof(int));
    if (rec == NULL) {
	printf("Not enough memory to start thread.\n");
	return NULL;
    }
    errtot = 0;

    for(;; hjet->RecNum++) {
	i = hjet->RecRead(rec, sizeof(recordHeaderStruct)); 
	if (hjet->StopThread != 0) break;

	if (i == 2) break;	// EOF found - new file was opened
	if (i != 1) {
	    printf("\nHJET-ERR : IO error on input file (head) %m.\n");
	    continue;
	}

	if (rec->header.len > (long) (BSIZE*sizeof(int))) {
	    printf("\nHJET-ERR : Very large record (%d). Skipping...\n", rec->header.len);
	    for (j=sizeof(recordHeaderStruct); j<rec->header.len; j += i) {
		k = rec->header.len - j;
		if (k > (int) (BSIZE*sizeof(int) - sizeof(recordHeaderStruct))) 
		    k = BSIZE*sizeof(int) - sizeof(recordHeaderStruct);
		i = hjet->RecRead(rec->data, k); 
		if (hjet->StopThread != 0 || i != 1) break;
	    }
	    if (hjet->StopThread != 0) break;
	    if (i < 1) printf("\nHJET-ERR : IO error on input file (seek) %m.\n");
	    if (i == 2) break;
	    continue;
	}
	
	i = hjet->RecRead(rec->data, rec->header.len - sizeof(recordHeaderStruct));
	if (hjet->StopThread != 0) break;

	if (i == 2) break;	// EOF found - new file was opened
	if (i != 1) {
	    printf("\nHJET-ERR : IO error on input file (data) %m.\n");
	    continue;
	}

	switch(rec->header.type & REC_TYPEMASK) {
//	RHICCONF - main configuration
	case REC_RHIC_CONF:
	    hjet->Config = (configRhicDataStruct *) malloc(rec->header.len - sizeof(recordHeaderStruct));
	    if (hjet->Config == NULL) {
		printf("\nHJET-FATAL : No memory.\n");
		free(rec);
		fclose(hjet->DataFile);
		hjet->DataFile = NULL;
		return NULL;
	    }
	    memcpy(hjet->Config, rec->data, rec->header.len - sizeof(recordHeaderStruct));
	    wfLen = (hjet->Config->JetDelay & 0xFF) * 6;	// waveform length in points
	    wfDelay = ((hjet->Config->JetDelay >> 8 ) & 0xFF) * 6;// waveform delay in points
//	    printf("len = %d, delay = %d\n", wfLen, wfDelay);
	    for (i=0; i<MAXWAVEFORMS; i++) {
		hjet->Event[i] = (EventStruct *) malloc(sizeof(EventStruct) + wfLen - 1);
		if (hjet->Event[i] == NULL) {
		    printf("\nHJET-FATAL : No memory.\n");
		    free(rec);
		    fclose(hjet->DataFile);
		    hjet->DataFile = NULL;
		    return NULL;
		}
	    }
	    hjet->NTEvent = (EventStruct *) malloc(sizeof(EventStruct) + wfLen - 1);
	    if (hjet->NTEvent == NULL) {
		printf("\nHJET-FATAL : No memory.\n");
		free(rec);
		fclose(hjet->DataFile);
		hjet->DataFile = NULL;
		return NULL;
	    }
	    hjet->ReadConfig(hjet->ConfigName);	// override settings if we need
	    break;
//	BEAM data - we shall take beam polarization pattern from here
	case REC_BEAMADO:
	    k = 0;
	    if (rec->header.type & REC_BLUE) k = 1;
	    hjet->Beam[k] = (beamDataStruct *) malloc(rec->header.len - sizeof(recordHeaderStruct));
	    if (hjet->Beam[k] == NULL) {
		printf("\nHJET-FATAL : No memory.\n");
		free(rec);
		fclose(hjet->DataFile);
		hjet->DataFile = NULL;
		return NULL;
	    }
	    memcpy(hjet->Beam[k], rec->data, rec->header.len - sizeof(recordHeaderStruct));
	    if (k == 0) {
		str = (char *) malloc(strlen(hjet->Canvas->GetTitle())+20);
		if (str != NULL) {
		    sprintf(str, "%s Fill=%d", hjet->Canvas->GetTitle(), hjet->Beam[0]->fillNumberM);
		    hjet->Canvas->SetTitle(str);
		    free(str);
		}
	    }
	    break;
//	END of file - exit the thread
	case REC_END:
	    printf("\nEOF record found. %d events found.", hjet->EventCounter);
	    if (errtot != 0) printf(" %d format errors.", errtot);
	    printf("\n");
	    hjet->ShowStatistics();    // Always call ShowStatistics before PrintRawAsym
	    hjet->PrintRawAsym();
	    free(rec);
	    fclose(hjet->DataFile);
	    hjet->DataFile = NULL;
	    hjet->SetFile();   // Save & Close the ROOT file
	    if (strlen(getenv("HJET_DAQ")) > 0) {	// If not from DAQ, let HJET remain active
		 exit(0);
	    }
        return NULL;
//	Jet polarization changed
	case REC_HJETSWITCH:
	    hjet->JetPol = jet2pol[(((recordHJetSwitchStruct *)rec)->data[0]/JET_PLUS) & 3];
	    for (j=0; j<MAXWAVEFORMS; j++) if (hjet->Event[j] != NULL) hjet->Event[j]->quality = -1;
	    j = 0;
	    break;
//	Jet and beam positions
	case REC_HJPOSADO:
	    memcpy(&hjet->JetPosition, rec->data, rec->header.len - sizeof(recordHeaderStruct));
	    break;	    
//	carbon polarimeter target state
	case REC_HJCARBT:
	    jj = ((recordHJetCarbTargStruct *)rec)->targstat.good;
	    if (hjet->GoodData && !jj) {
		printf("\nHJET-INFO : Carbon polarimeter running. Skipping data following event %d\n", hjet->EventCounter);
	    }
	    if (!(hjet->GoodData) && jj) {
		printf("\nHJET-INFO : Resuming data analysis after polarimeter run, event %d\n", hjet->EventCounter);
	    }
	    hjet->GoodData = jj;
	    break;	    
//	Long waveform - data we analyze here
	case REC_READLONG:
	    err = 0;
	    evt = 0;
	    for (k=0; k < (int)(rec->header.len - sizeof(recordHeaderStruct)); k += subLen) {
		longWave = (recordLongWaveStruct *) &rec->data[k];
		subLen = sizeof(subheadStruct) + 
		    (longWave->subhead.Events + 1)*(sizeof(longWaveStruct) - 1 + wfLen);
		if (k + subLen > rec->header.len - sizeof(recordHeaderStruct)) {
		    printf("\nHJET-WARN : record %d format error %d + %d + 16 %d. Skipping...\n",
			hjet->RecNum, k, subLen, rec->header.len);
		    break;
		}
		for(i=0; i<=(int)longWave->subhead.Events; i++) {
		    Event = (longWaveStruct *) &longWave->data[i*(sizeof(longWaveStruct) - 1 + wfLen)];
		    if (Event->before != wfDelay/6 || Event->length != wfLen/6) {
			err++;
			if (err < 20 && errtot < 100) {
			    printf("\nHJET-WARN : Si %d, event %d(%d) fixed parameters error %d %d %d %d. Skipping...\n",
				longWave->subhead.siNum, i+1, hjet->EventCounter, Event->before, wfDelay/6, 
				Event->length, wfLen/6);
			    printf("Dump: ");
			    for(int ii=0; ii < (int)wfLen + 6; ii++) printf("%2.2X ", 0xFF & ((char *)Event)[ii]);
			    printf("\n");
			}
			continue;
		    }
		    if (hjet->GoodData) {
			hjet->ProcessEvent(longWave->subhead.siNum, Event);
			hjet->FillEvent();
		    }
		    if (j < MAXWAVEFORMS && 
			(hjet->SelectChan == 0 || hjet->SelectChan == hjet->NTEvent->chan + 1)) {
			memcpy(hjet->Event[j], hjet->NTEvent, sizeof(EventStruct) + wfLen - 1);
			j++;
		    }
		    hjet->EventCounter++;
		    evt++;
		}
	    }
	    if (err != 0) printf("Record %d had %d errors for %d good events!\n", hjet->RecNum, err, evt);
	    errtot += err;
	    break;
//		We don't analyze these records
	case REC_BEGIN:	
	case REC_POLADO:
	case REC_TAGADO:
	case REC_READALL:
	case REC_READRAW:
	case REC_READSUB:
	case REC_READAT:
	case REC_WFDV8SCAL:
	case REC_SCALERS:
	case REC_PCTARGET:
	case REC_WCMADO:
	    break;
	default:	// skip any unknown records
//	    printf("\nUnknown record %8.8X encountered in input data file\n", rec->header.type & REC_TYPEMASK);
	    break;
	}
    }
    free(rec);
    fclose(hjet->DataFile);
    hjet->DataFile = NULL;
    hjet->SetFile();   // Save & Close the ROOT file
//    exit(0);
	if (strlen(getenv("HJET_DAQ")) > 0) {	// If not from DAQ, let HJET remain active
	 exit(0);
	}
    return NULL;
}

// Routine to trap Kill signal and close ROOT file.
void signalHandler(int sig)
{
 if (sig == SIGINT || sig == SIGKILL || sig == SIGTERM) {
  printf("Received termination signal %d\n", sig);
 }
 hjet->ShowStatistics();    // Always call ShowStatistics before PrintRawAsym
 hjet->PrintRawAsym();
 hjet->SetFile();
 printf("Closed ROOT file, now terminating\n");
 exit(0);
}
