#include <TROOT.h>
#include <TApplication.h>
#include <TF1.h>
#include <TStyle.h>
#include <TFile.h>

#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <argp.h>

#include "Config.h"
#include "Fitter.h"
#include "InputFileReader.h"
#include "FitFunction.h"

using std::cout;
using std::endl;
using std::vector;

struct Arguments{
	TString spectrumfile = "";
	TString matrixfile = "";
	TString outputfile = "output.root";
	UInt_t left = 0;
	UInt_t right = NBINS;
};

static char doc[] = "horst, HIGS original reconstruction spectrum tool";
static char args_doc[] = "INPUTFILENAME";

static struct argp_option options[] = {
	{"matrixfile", 'm', "MATRIXFILENAME", 0, "Name of file that contains the response matrix", 0},
	{"outputfile", 'o', "OUTPUTFILENAME", 0, "Name of output file", 0},
	{"left", 'l', "LEFT", 0, "Left limit of fit range", 0},
	{"right", 'r', "RIGHT", 0, "Right limit of fit range", 0},
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
		case ARGP_KEY_END:
			if(state->arg_num == 0){
				argp_usage(state);
			}
			break;
		default: return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0};

int main(int argc, char* argv[]){

	/************ Read command-line arguments  *************/

	Arguments arguments;
	argp_parse(&argp, argc, argv, 0, 0, &arguments);

	/************ Start ROOT application *************/

	TApplication *app;
	app = new TApplication("Response_Fit", &argc, argv);

	/************ Read and rebin spectrum and response matrix *************/

	InputFileReader inputFileReader;

	TH1F spectrum("spec", "Spectrum", NBINS, 0., (Double_t) NBINS - 1);
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

	TH1F topdown_fit("topdown_fit", "TopDown_Fit", NBINS/BINNING, 0., (Double_t) NBINS - 1); 
	for(Int_t i = 1; i <= (Int_t) NBINS; ++i){
		topdown_fit.SetBinContent(i, start_params.GetBinContent(i)*response_matrix.GetBinContent(i, i));
		if(start_params.GetBinContent(i) <= 0.){
			start_params.SetBinContent(i, 0.);
		}
	}

	TH1F fit_function_test("fit_function_test", "FF_Test", NBINS/BINNING, 0., (Double_t) NBINS - 1); 
	vector<Double_t> fit_params(NBINS/BINNING);
	for(Int_t i = 1; i <= start_params.GetNbinsX(); ++i){
		fit_params[i] = start_params.GetBinContent(i);
	}
	FitFunction fitFunction(response_matrix, (Int_t) arguments.left/ (Int_t) BINNING, (Int_t) arguments.right/ (Int_t) BINNING);
	Double_t bin = 0.;
	for(Int_t i = 1; i <= (Int_t) NBINS/BINNING; ++i){
		bin = (Double_t) i*BINNING;
		fit_function_test.SetBinContent(i, fitFunction(&bin, &fit_params[0]));
	}
	

	/************ Fit *************/

	TH1F params("params", "Parameters", NBINS/BINNING, 0., (Double_t) NBINS - 1);
	TH1F fit("fit", "Fit", (Int_t) NBINS/BINNING, 0., (Double_t) NBINS - 1);

	fitter.fit(spectrum, response_matrix, start_params, params, fit, (Int_t) arguments.left/ (Int_t) BINNING, (Int_t) arguments.right/ (Int_t) BINNING);

	TH1F chi2_fit("chi2_fit", "Chi2_Fit", NBINS/BINNING, 0., (Double_t) NBINS - 1); for(Int_t i = 1; i <= (Int_t) NBINS; ++i){
		chi2_fit.SetBinContent(i, params.GetBinContent(i)*response_matrix.GetBinContent(i, i));
	}

	/************ Plot results *************/

	//response_matrix.Draw();
	gStyle->SetHistLineColor(kBlack);
	spectrum.UseCurrentStyle();
	spectrum.Draw();
	gStyle->SetHistLineColor(kGreen);
	fit_function_test.UseCurrentStyle();
	fit_function_test.Draw("same");
//	gStyle->SetHistLineColor(kGreen);
//	unfolded_spectrum_topdown.UseCurrentStyle();
//	unfolded_spectrum_topdown.Draw("same");
//	gStyle->SetHistLineColor(kBlue);
//	start_params.UseCurrentStyle();
//	start_params.Draw();
//	gStyle->SetHistLineColor(kBlue);
//	topdown_fit.UseCurrentStyle();
//	topdown_fit.Draw("same");
//	gStyle->SetHistLineColor(kRed);
//	params.UseCurrentStyle();
//	params.Draw("same");
	gStyle->SetHistLineColor(kAzure);
	chi2_fit.UseCurrentStyle();
	chi2_fit.Draw("same");

	/************ Write results to file *************/

	TFile outputfile(arguments.outputfile, "RECREATE");
	unfolded_spectrum_topdown.Write();
	spectrum.Write();
	params.Write();
	outputfile.Close();

	app->Run();
}
