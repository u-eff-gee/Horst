#include <TROOT.h>
#include <TApplication.h>
#include <TF1.h>

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
	TString outputfile = "";
	Int_t left = 0;
	Int_t right = NBINS;
};

static char doc[] = "horst, HIGS original reconstruction spectrum tool";
static char args_doc[] = "INPUTFILENAME";

static struct argp_option options[] = {
	{"matrixfile", 'm', "MATRIXFILENAME", 0, "Name of file that contains the response matrix"},
	{"outputfile", 'o', "OUTPUTFILENAME", 0, "Name of output file"},
	{"left", 'l', "LEFT", 0, "Left limit of fit range"},
	{"right", 'r', "RIGHT", 0, "Right limit of fit range"},
	{ 0, 0, 0, 0, 0}
};

static int parse_opt(int key, char *arg, struct argp_state *state){
	struct Arguments *arguments = (struct Arguments*) state->input;

	switch (key){
		case ARGP_KEY_ARG: arguments->spectrumfile = arg; break;
		case 'o': arguments->outputfile = arg; break;
		case 'm': arguments->matrixfile= arg; break;
		case 'l': arguments->left= atoi(arg); break;
		case 'r': arguments->right= atoi(arg); break;
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

	Arguments arguments;
	argp_parse(&argp, argc, argv, 0, 0, &arguments);

	TApplication *app;
	app = new TApplication("Response_Fit", &argc, argv);

	InputFileReader inputFileReader;
	Fitter fitter;

	TH1F spectrum("spec", "Spectrum", NBINS, 0., (Double_t) NBINS - 1);
	inputFileReader.readTxtSpectrum(spectrum, arguments.spectrumfile);
	spectrum.Rebin(BINNING);

	TH2F response_matrix("rema", "Response_Matrix", NBINS, 0., (Double_t) (NBINS - 1), NBINS, 0., (Double_t) (NBINS - 1));
	inputFileReader.readMatrix(response_matrix, arguments.matrixfile);
	response_matrix.Rebin2D(BINNING, BINNING);

	TH1F start_params("start_params", "Start_Parameters", NBINS/BINNING, 0., (Double_t) NBINS - 1);
	TH1F unfolded_spectrum_topdown("unfolded_spectrum_topdown", "Unfolded_Spectrum_TopDown", (Int_t) NBINS/BINNING, 0., (Double_t) NBINS - 1);
	fitter.topdown(spectrum, response_matrix, start_params, unfolded_spectrum_topdown, arguments.left/BINNING, arguments.right/BINNING);

	TH1F params("params", "Parameters", NBINS/BINNING, 0., (Double_t) NBINS - 1);

	for(Int_t i = 0; i < start_params.GetNbinsX(); ++i)
		params.SetBinContent(i, start_params.GetBinContent(i));

	FitFunction fitFunction(response_matrix, (Int_t) arguments.left/BINNING, (Int_t) arguments.right/BINNING);
	TF1 fitf("fitf", fitFunction, 0., (Double_t) NBINS-1, NBINS/BINNING);

	for(Int_t i = 0; i < start_params.GetNbinsX(); ++i){
		if(i < arguments.left/BINNING || i > arguments.right/BINNING){
			fitf.FixParameter(i, 0.);
		}
		fitf.SetParameter(i, start_params.GetBinContent(i));
	}

	cout << "> Fitting spectrum ..." << endl;
	spectrum.Fit("fitf");

	//response_matrix.Draw();
	//spectrum.Draw();
	//unfolded_spectrum.Draw("same");
	//start_params.Draw("same");
	app->Run();
}
