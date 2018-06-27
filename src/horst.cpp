#include <TROOT.h>
#include <TApplication.h>

#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <argp.h>

#include "Config.h"
#include "Fitter.h"
#include "InputFileReader.h"

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

	TH1F params("params", "Parameters", NBINS/BINNING, 0., (Double_t) NBINS - 1);
	TH1F unfolded_spectrum("unfolded_spectrum", "Unfolded_Spectrum", NBINS/BINNING, 0., (Double_t) NBINS - 1);
	fitter.topdown(spectrum, response_matrix, params, unfolded_spectrum, arguments.left/BINNING, arguments.right/BINNING);

	//response_matrix.Draw();
	//spectrum.Draw();
	//unfolded_spectrum.Draw("same");
	params.Draw("same");
	app->Run();
}
