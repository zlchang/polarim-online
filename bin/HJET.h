/*
    Class definition for online Hjet data analysis
    I. Alekseev & D. Svirida, 2006.
*/

#ifndef HJET_H
#define HJET_H

#include <stdio.h>
#include <TNamed.h>
#include <TThread.h>
#include <TCanvas.h>
#include <TTree.h>
#include <TH1F.h>
#include <TH1D.h>
#include <TH2F.h>
#include <TFile.h>
#include <TTimer.h>
#include "../include/rhicpol.h"
#include "../include/rpoldata.h"
//#include "rhicpol_old.h"
//#include "rpoldata_old.h"
#include "HCut.h"

#define MAXWAVEFORMS	100
#define MAXSICHAN	100
#define MAXQUEUE	100
#define LINEPOINTS	6
#define DETECTORS	6
#define BUNCHCORR	40 //RUN6 80, RUN8 40 Hiromi Feb. 25, 2008
#define LARGE_VALUE	(1.0E50)

typedef struct {
    int chan;			// Channel number
    int rev;			// revolution number
    int bunch;			// bunch number
    int bpol;			// bunch polarization (-1, 0, +1)
    int jpol;			// jet polarization (-1, 0, +1)
    float time;			// reconstructed time
    float ampl;			// reconstructed amplitude
    float quality;		// event quality
    float ekin;			// recoil kinetic energy, keV
    float tof;			// time of flight, ns
    float rmass;		// recoil mass, GeV 
    float angle;		// pi/2 - <scattering angle>, radians
    float mmass2;		// missing mass squared, GeV
//    float intg;         // Integral of waveform
    unsigned char wave[1];	// actual length is defined elsewhere
} EventStruct;

class HJET : public TNamed {
private:
    FILE *DataFile;		// file we are processing
    TThread *DataThread;	// data drocessing thread
    configRhicDataStruct *Config;	// configuration
    beamDataStruct *Beam[2];	// beam data for both beams
    int JetPol;			// current jet polarization
    int StopThread;		// flag to stop data processing thread
    int RecNum;			// current record number
    int EventCounter;		// current event counter
    TCanvas *Canvas;		// our window
	TCanvas *CanvYB;			// for Y/B stacked plot
    TFile *RootFile;		// file to write output n-tuple.
    EventStruct *Event[MAXWAVEFORMS];	// Waveforms to be shown
    EventStruct *NTEvent;	// event to be saved in n-tuple.
    char DrawWhat[10];		// What to draw on buttons
    int SelectChan;		// channel to select waveforms
    int GoodData;		// true if no carbon polarimeter interference happens
    char *ConfigName;		// name of calibration file 
    TF1  *Banana;		// Banana function for HATDET histograms
    TF1  *MassCutMin;   // Mass cut function for HATDET histograms
    TF1  *MassCutMax;   // Mass cut function for HATDET histograms
    TF1  *Parabola;		// Parabola function for HANDET histograms
    char *fQueue[MAXQUEUE];	// Queue of files to be processed
    TTimer *QueueTimer;		// Timer to move through the queue

    void ProcessEvent(int chan, longWaveStruct *data);	// Extract one event
    void FillEvent(void);	// Fill histograms for one event
    
    TTree *CreateTree(int chan);	// Create N-tuple for a strip 
    TH2F  *CreateHistAT(int chan);	// Create AT-histogram for a strip
    TH1F  *CreateHistM(int chan);	// Create recoiled mass histogram for a strip
    TH1F  *CreateHistMM(int chan);	// Create missing mass histogram for a strip
    TH1F  *CreateHistC(int chan);	// Create ADC histogram for a strip
    TH2F  *CreateHistATDet(int det);	// Create Ekin-TOF histogram for the whole detector
    TH2F  *CreateHistATYB(int k, int det);	// Create Ekin-TOF histogram for each half detector
    TH2F  *CreateHistANDet(int det);	// Create Angle-Ekin histogram for the whole detector
    int RecRead(void *buf, size_t len);	// Read one record
    Bool_t HandleTimer(TTimer *timer);	// Handle timer events

public:
    jetPositionStruct JetPosition;	// jet and beam positions
//	Histograms etc
    TTree *NT[MAXSICHAN];	// event n-tuples
    TH1F  *HQ;			// time fit quality
    TH2F  *HAT[MAXSICHAN];	// time versus amplitude histogram per silicon
    TH1F  *HM[MAXSICHAN];	// recoiled mass
    TH1F  *HMM[MAXSICHAN];	// missing mass
    TH1F  *HADC[MAXSICHAN]; // ADC histogram 
    TH2F  *HATDET[DETECTORS];	// time versus amplitude histogram per detector
    TH2F  *HATDYB[2][DETECTORS];	// time versus amplitude histogram per detector half
    TH2F  *HANDET[DETECTORS];	// amplitude versus position per detector
    TH1F  *HSTRIP[3];		// good events per strip
    TH1F  *HBUNCH[2];		// good events per bunch crossing yellow, blue
    TH1D  *HE[16];		// good events energy distribution polarization sorted
//	Cuts for plots
    HCut  *CutQuality;		// required quality for filling histograms
    HCut  *CutRawAmpl;		// Cut on the raw amplitude
    HCut  *CutTOF;		// Cut on time of flight
    HCut  *CutEkin;		// Cut on the recoiled kinetic energy
    HCut  *CutRmass;		// cut on the recoiled mass
    HCut  *CutMmass;		// cut on the missing mass
    float TimeCorr;		// some time correction

    HJET(char *fname = NULL);	// Create analysis. Attach to file fname if fname != NULL
    ~HJET(void);		// Destruct analysis
    void Open(char *fname);	// Attach file fname. Detach current file if attached 
    void Queue(char *fname);	// Put file in queue for processing
    void ShowStatistics(void);	// Print statistice
    virtual void Draw(Option_t *what);	// Draw histogram
    void SetFile(char *fname = NULL);	// Open root file. if NULL - just close
    void ResetAllHist(void);	// Reset all histogramms
    void ReadConfig(char *cname);	// Override t0, ecoef and edead from the file
    void SetConfigName(char *cname);	// Set file to override t0, ecoef and edead for all further open's
    void PrintCuts(void);	// Print information about current cuts
    void PrintRawAsym(void);	// Print raw asymmetries
    void PrintQueue(void);	// Print file queue for processing
    void Save(char *fname = NULL);	// Save histograms to the file fname

//	Inline functions
    int IsOK(void) {		// Check if there is datafile attached
	return (DataFile != NULL);
    };
    void Next(int incr = 1) {	// Draw next histogram
	int i = strtol(&DrawWhat[1], NULL, 0);
	sprintf(&DrawWhat[1], "%d", i+incr);
	Draw(DrawWhat);
    };
    void Prev(int decr = 1) {	// Draw previous histogram
	int i = strtol(&DrawWhat[1], NULL, 0);
	if (i>decr) sprintf(&DrawWhat[1], "%d", i-decr);
	Draw(DrawWhat);
    };
    void DrawSet(char what) {	// Set what to draw
	DrawWhat[0] = what;
	Draw(DrawWhat);
    };
    void DrawSet(char *what) {	// Set what to draw
	strncpy(DrawWhat, what, sizeof(DrawWhat));
	Draw(DrawWhat);
    };
    void SelectChannel(int chan) {	// Select channel to collect waveforms
	SelectChan = chan; 
    }

    ClassDef(HJET, 1);
    
    friend void *DataThreadFun(void *arg);
    friend void signalHandler(int sig);    // Trap Kill command
};
void signalHandler(int sig);    // Trap Kill command
void *DataThreadFun(void *arg);		// function toprocess the data

HJET *hjet;
#endif /* HJET_H */
