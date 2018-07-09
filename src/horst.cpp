#include <TROOT.h>
#include <TApplication.h>
#include <TF1.h>
#include <TStyle.h>
#include <TFile.h>
#include <TCanvas.h>

#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <argp.h>
#include <time.h>

#include "Config.h"
#include "Fitter.h"
#include "InputFileReader.h"

using std::cout;
using std::endl;
using std::vector;

struct Arguments{
	TString spectrumfile = "";
	TString matrixfile = "";
	TString outputfile = "output.root";
	UInt_t left = 0;
	UInt_t right = NBINS;
	Bool_t interactive_mode = false;
	Bool_t plot = false;
};

static char doc[] = "horst, HIGS original reconstruction spectrum tool";
static char args_doc[] = "INPUTFILENAME";

static struct argp_option options[] = {
	{"matrixfile", 'm', "MATRIXFILENAME", 0, "Name of file that contains the response matrix", 0},
	{"outputfile", 'o', "OUTPUTFILENAME", 0, "Name of output file", 0},
	{"left", 'l', "LEFT", 0, "Left limit of fit range", 0},
	{"right", 'r', "RIGHT", 0, "Right limit of fit range", 0},
	{"interactive_mode", 'i', 0, 0, "Interactive mode (show results in ROOT application, switched off by default)", 0},
	{"plot", 'p', 0, 0, "Plot results in pdf files (switched off by default)", 0},
	{ 0, 0, 0, 0, 0, 0}
};

static int parse_opt(int key, char *arg, struct argp_state *state){
	struct Arguments *arguments = (struct Arguments*) state->input;

	switch (key){
		case ARGP_KEY_ARG: arguments->spectrumfile = arg; break;
		case 'o': arguments->outputfile = arg; break;
		case 'm': arguments->matrixfile= arg; break;
		case 'l': arguments->left= (UInt_t) atoi(arg); break;
		case 'r': arguments->right= (UInt_t) atoi(arg); break;
		case 'i': arguments->interactive_mode= true; break;
		case 'p': arguments->plot = true; break;
		case ARGP_KEY_END:
			if(state->arg_num == 0){
				argp_usage(state);
			}
			if(arguments->matrixfile == ""){
				cout << "Error: No matrix file given. Aborting ..." << endl;
				abort();
			}
			break;
		default: return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0};

int main(int argc, char* argv[]){

	time_t start, stop;
	time(&start);

	/************ Read command-line arguments  *************/

	Arguments arguments;
	argp_parse(&argp, argc, argv, 0, 0, &arguments);

	/************ Start ROOT application *************/

	TApplication *app;
	if(arguments.interactive_mode){
		app = new TApplication("Response_Fit", &argc, argv);
	}

	/************ Read and rebin spectrum and response matrix *************/

	InputFileReader inputFileReader;

	TH1F spectrum("spec", "Original Spectrum", NBINS, 0., (Double_t) NBINS - 1);
	inputFileReader.readTxtSpectrum(spectrum, arguments.spectrumfile);
	spectrum.Rebin(BINNING);

	TH2F response_matrix("rema", "Response_Matrix", NBINS, 0., (Double_t) (NBINS - 1), NBINS, 0., (Double_t) (NBINS - 1));
	inputFileReader.readMatrix(response_matrix, arguments.matrixfile);
	response_matrix.Rebin2D(BINNING, BINNING);

	/************ Use Top-Down unfolding to get start parameters *************/

	TH1F start_params("start_params", "Start_Parameters", NBINS/BINNING, 0., (Double_t) NBINS - 1);
	TH1F unfolded_spectrum_topdown("unfolded_spectrum_topdown", "Unfolded_Spectrum_TopDown", (Int_t) NBINS/BINNING, 0., (Double_t) NBINS - 1);

	Fitter fitter;
	fitter.topdown(spectrum, response_matrix, start_params, unfolded_spectrum_topdown, (Int_t) arguments.left/ (Int_t) BINNING, (Int_t) arguments.right/ (Int_t) BINNING);

	TH1F topdown_FEP("topdown_FEP", "TopDown FEP", NBINS/BINNING, 0., (Double_t) NBINS - 1); 
	fitter.fittedFEP(start_params, response_matrix, topdown_FEP);
	TH1F topdown_fit("topdown_fit", "TopDown Fit", NBINS/BINNING, 0., (Double_t) NBINS - 1); 
	fitter.fittedSpectrum(start_params, response_matrix, topdown_fit);

	fitter.remove_negative(start_params);

	/************ Fit *************/

	TH1F params("params", "Parameters", NBINS/BINNING, 0., (Double_t) NBINS - 1);
	TH1F fit("fit", "Fit", (Int_t) NBINS/BINNING, 0., (Double_t) NBINS - 1);

	fitter.fit(spectrum, response_matrix, start_params, params, fit, (Int_t) arguments.left/ (Int_t) BINNING, (Int_t) arguments.right/ (Int_t) BINNING);

	TH1F chi2_FEP("chi2_FEP", "Chi2 FEP", NBINS/BINNING, 0., (Double_t) NBINS - 1); 
	fitter.fittedFEP(params, response_matrix, chi2_FEP);
	TH1F chi2_fit("chi2_fit", "Chi2 Fit", NBINS/BINNING, 0., (Double_t) NBINS - 1); 
	fitter.fittedSpectrum(params, response_matrix, chi2_fit);

	/************ Plot results *************/

	TCanvas c1("c1", "Plots", 4);
	c1.Divide(2, 2);

	c1.cd(1);
	spectrum.SetLineColor(kBlack);
	spectrum.Draw();

	c1.cd(2);
	topdown_fit.SetLineColor(kRed);
	topdown_fit.Draw();
	topdown_FEP.SetLineColor(kGreen);
	topdown_FEP.Draw("same");
	spectrum.SetLineColor(kBlack);
	spectrum.Draw("same");

	c1.cd(3);
	chi2_fit.SetLineColor(kRed);
	chi2_fit.Draw();
	chi2_FEP.SetLineColor(kGreen);
	chi2_FEP.Draw("same");
	spectrum.SetLineColor(kBlack);
	spectrum.Draw("same");

	/************ Write results to file *************/

	TFile outputfile(arguments.outputfile, "RECREATE");
	spectrum.Write();
	start_params.Write();
	topdown_FEP.Write();
	topdown_fit.Write();
	params.Write();
	chi2_FEP.Write();
	chi2_fit.Write();
	outputfile.Close();

	if(arguments.interactive_mode){
		app->Run();
	}

	time(&stop);
	cout << "> Execution time: " << stop - start << " seconds" << endl;
}
