/*
    This file is part of Horst.

    Horst is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Horst is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Horst.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <TROOT.h>
#include <TApplication.h>
#include <TStyle.h>
#include <TFile.h>
#include <TCanvas.h>

#include <argp.h>
#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <time.h>

#include "Config.h"
#include "InputFileReader.h"
#include "Reconstructor.h"
#include "Fitter.h"

using std::cout;
using std::endl;
using std::vector;
using std::stringstream;

struct Arguments{
	UInt_t binning = 10;
	TString spectrumfile = "";
	TString spectrumname = "";
	TString matrixfile = "";
	TString outputfile = "output.root";
	Bool_t interactive_mode = false;
	Bool_t statistics = false;
	Bool_t tfile = false;
};

static char doc[] = "Tsroh, Transfer spectroscopic response on histogram";
static char args_doc[] = "INPUTFILENAME";

static struct argp_option options[] = {
	{"binning", 'b', "arguments.binning", 0, "Rebinning factor for input histograms (default: 10)", 0},
	{"matrixfile", 'm', "MATRIXFILENAME", 0, "Name of file that contains the response matrix", 0},
	{"outputfile", 'o', "OUTPUTFILENAME", 0, "Name of output file", 0},
	{"interactive_mode", 'i', 0, 0, "Interactive mode (show results in ROOT application, switched off by default)", 0},
	{"statistics", 's', 0, 0, "Add statistical fluctuations to response (switched off by default)", 0},
	{"tfile", 't', "SPECTRUM", 0, "Read SPECTRUM from a ROOT file called INPUTFILENAME, instead from a text file.", 0},
	{ 0, 0, 0, 0, 0, 0}
};

static int parse_opt(int key, char *arg, struct argp_state *state){
	struct Arguments *arguments = (struct Arguments*) state->input;

	switch (key){
		case ARGP_KEY_ARG: arguments->spectrumfile = arg; break;
		case 'b': arguments->binning= atoi(arg); break;
		case 'm': arguments->matrixfile= arg; break;
		case 'o': arguments->outputfile = arg; break;
		case 'i': arguments->interactive_mode= true; break;
		case 's': arguments->statistics= true; break;
		case 't': arguments->tfile = true; arguments->spectrumname = arg; break;
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

	/************ Initialize auxiliary classes *************/

	InputFileReader inputFileReader(arguments.binning);
	Reconstructor reconstructor(arguments.binning);
	Fitter fitter(arguments.binning);

	/************ Initialize histograms *************/

	// Input
	TH1F spectrum("spectrum", "Input Spectrum", NBINS, 0., (Double_t) NBINS - 1);
	TH1F n_simulated_particles("n_simulated_particles", "Number of simulated particles per bin", NBINS, 0., (Double_t) NBINS - 1);
	TH1F inverse_n_simulated_particles("inverse_n_simulated_particles", "1 / Number of simulated particles per bin", NBINS, 0., (Double_t) NBINS - 1);
	TH2F response_matrix("rema", "Response_Matrix", NBINS, 0., (Double_t) (NBINS - 1), NBINS, 0., (Double_t) (NBINS - 1));

	// Output
	
	TH1F response_spectrum("response_spectrum", "Spectrum with Response", NBINS/arguments.binning, 0., (Double_t) NBINS - 1); 
	TH1F response_spectrum_FEP("response_spectrum_FEP", "Spectrum with Response, normalized to FEP", NBINS/arguments.binning, 0., (Double_t) NBINS - 1); 

	/************ Start ROOT application *************/

	TApplication *app = nullptr;
	if(arguments.interactive_mode){
		argc = 1; // Prevent name clashes with options of TApplication
		app = new TApplication("Reconstruction", &argc, argv);
	}

	/************ Read and rebin spectrum and response matrix *************/

	cout << "> Reading spectrum file " << arguments.spectrumfile << " ..." << endl;
	if(arguments.tfile)
		inputFileReader.readROOTSpectrum(spectrum, arguments.spectrumfile, arguments.spectrumname);
	else{
		inputFileReader.readTxtSpectrum(spectrum, arguments.spectrumfile);
		spectrum.Rebin(arguments.binning);
	}

	cout << "> Reading matrix file " << arguments.matrixfile << " ..." << endl;
	inputFileReader.readMatrix(response_matrix, n_simulated_particles, arguments.matrixfile);
	response_matrix.Rebin2D(arguments.binning, arguments.binning);
	n_simulated_particles.Rebin(arguments.binning);

	for(Int_t i = 0; i <= (Int_t) NBINS/(Int_t) arguments.binning; ++i)
		inverse_n_simulated_particles.SetBinContent(i, 1./n_simulated_particles.GetBinContent(i));

	/************ Add response to experimental spectrum *************/

	cout << "> Adding response to spectrum ..." << endl;
	if(arguments.statistics){
		reconstructor.addRealisticResponse(spectrum, inverse_n_simulated_particles, response_matrix, response_spectrum, response_spectrum_FEP);
		fitter.remove_negative(response_spectrum);
	} else{
		reconstructor.addResponse(spectrum, inverse_n_simulated_particles, response_matrix, response_spectrum, response_spectrum_FEP);
	}

	/************ Plot results *************/

	TCanvas c1("c1", "Plots", 4);
	if(arguments.interactive_mode){
		cout << "> Creating plots ..." << endl;

		c1.Divide(1, 2);

		c1.cd(1);
		response_spectrum_FEP.SetLineColor(kBlack);
		response_spectrum_FEP.Draw();
		spectrum.SetLineColor(kGreen);
		spectrum.Draw("same");

		c1.cd(2);
		response_spectrum.SetLineColor(kBlack);
		response_spectrum.Draw();
	}

	/************ Write results to file *************/

	cout << "> Writing output file " << OUTPUT_DIR << arguments.outputfile << " ..." << endl;

	stringstream outputfilename;
	outputfilename << OUTPUT_DIR << arguments.outputfile;

	TFile outputfile(outputfilename.str().c_str(), "RECREATE");
	spectrum.Write();
	response_spectrum_FEP.Write();
	response_spectrum.Write();

	outputfile.Close();

	if(arguments.interactive_mode){
		cout << "> Starting interactive plot ..." << endl;
		app->Run();
	}

	time(&stop);
	cout << "> Execution time: " << stop - start << " seconds" << endl;
}
