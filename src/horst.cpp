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

#include <TApplication.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TFile.h>
#include <TMatrixDSym.h>
#include <TROOT.h>
#include <TStyle.h>

#include <argp.h>
#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <time.h>

#include "Config.h"
#include "Fitter.h"
#include "InputFileReader.h"
#include "MonteCarloUncertainty.h"
#include "Reconstructor.h"
#include "Uncertainty.h"

using std::cout;
using std::endl;
using std::vector;
using std::stringstream;

struct Arguments{
	UInt_t binning = 10;
	TString spectrumfile = "";
	TString spectrumname = "";
	TString matrixfile = "";
	UInt_t uncertainty_mc = 1;
	UInt_t seed = 1;
	Bool_t use_mc = false;
	Bool_t use_mc_fast = false;
	Bool_t write_mc = false;
	Bool_t write_mc_only = false;
	TString correlation_matrix_filename = "correlation_matrix.txt";
	TString outputfile = "output.root";
	UInt_t left = 0;
	UInt_t right = NBINS;
	Bool_t interactive_mode = false;
	Bool_t tfile = false;
	Bool_t verbose = false;
	Bool_t correlation = false;
};

static char doc[] = "Horst, Histogram original reconstruction spectrum tool";
static char args_doc[] = "INPUTFILENAME";

static struct argp_option options[] = {
	{"binning", 'b', "BINNING", 0, "a) Without '-t' option: Rebinning factor for input spectrum and response matrix (default: 10)\nb) With '-t' option   : Rebinning factor for response matrix (default: 10)", 0},
	{"matrixfile", 'm', "MATRIXFILENAME", 0, "Name of file that contains the response matrix", 0},
	{"uncertainty_mc", 'u', "NRANDOM", 0, "Determine uncertainty using a Monte-Carlo (MC) method to include correlations. NRANDOM is the number of MC iterations (default: 10).", 0},
	{"uncertainty_mc_fast", 'U', "NRANDOM", 0, "Similar to '-u' option, but do not vary the response matrix when estimating the statistical uncertainty. This option can be used to disentangle the statistical uncertainty introduced by the input spectrum and the response matrix. Overrides the '-u' option if both are set.", 0},
	{"write_mc", 'w', 0, 0, "Write MC-generated spectra and reconstructed spectra. This option is ignored if '-u' option is not used.", 0},
	{"write_mc_only", 'W', 0, 0, "Similar to '-w' option, but does not evaluate MC results afterwards (i.e. leaves calculation of mean value and uncertainties to the user). The advantage compared to the '-w' option is that horst does not have to keep all MC spectra in memory until the end of the program execution, potentially saving a lot of RAM. MC spectra are dumped to file immediately.", 0},
	{"outputfile", 'o', "OUTPUTFILENAME", 0, "Name of output file", 0},
	{"left", 'l', "LEFT", 0, "Left limit of fit range", 0},
	{"right", 'r', "RIGHT", 0, "Right limit of fit range", 0},
	{"interactive_mode", 'i', 0, 0, "Interactive mode (show results in ROOT application, switched off by default)", 0},
	{"tfile", 't', "SPECTRUM", 0, "Select SPECTRUM from a ROOT file called INPUTFILENAME, instead of a text file."
	" Spectrum must be an object of TH1F.", 0},
	{"correlation", 'c', "CORRELATIONFILENAME", 0, "Write the correlation matrix of the fit to the specified output file. If the '-u' option is used, only one correlation matrix will be written, although NRANDOM fits are executed.", 0},
	{"seed", 's', "SEED", 0, "Set the random number seed (default: 1)", 0},
	{"verbose", 'v', 0, 0, "Enable ROOT to print verbose information about the fitting process", 0},
	{ 0, 0, 0, 0, 0, 0}
};

static int parse_opt(int key, char *arg, struct argp_state *state){
	struct Arguments *arguments = (struct Arguments*) state->input;

	switch (key){
		case ARGP_KEY_ARG: arguments->spectrumfile = arg; break;
		case 'b': arguments->binning= (UInt_t) atoi(arg); break;
		case 'm': arguments->matrixfile= arg; break;
		case 'u': arguments->use_mc = true; arguments->uncertainty_mc = (UInt_t) atoi(arg); break;
		case 'U': arguments->use_mc_fast = true; arguments->use_mc = true; arguments->uncertainty_mc = (UInt_t) atoi(arg); break;
		case 'w': arguments->write_mc = true; break;
		case 'W': arguments->write_mc = true; arguments->write_mc_only = true; break;
		case 'o': arguments->outputfile = arg; break;
		case 'l': arguments->left= (UInt_t) atoi(arg); break;
		case 'r': arguments->right= (UInt_t) atoi(arg); break;
		case 'i': arguments->interactive_mode= true; break;
		case 't': arguments->tfile = true; arguments->spectrumname = arg; break;
		case 'c': arguments->correlation = true; arguments->correlation_matrix_filename = arg; break;
		case 's': arguments->seed = (UInt_t) atoi(arg); break;
		case 'v': arguments->verbose = true; break;
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

	const Int_t nbins = (Int_t) NBINS / (Int_t) arguments.binning;
	const Double_t max_bin = (Double_t) NBINS - 1.;
	const Int_t binstart = (Int_t) arguments.left / (Int_t) arguments.binning; 
	const Int_t binstop = (Int_t) arguments.right / (Int_t) arguments.binning; 

	InputFileReader inputFileReader(arguments.binning);
	TMatrixDSym correlation_matrix(nbins);
	Reconstructor reconstructor(arguments.binning);
	MonteCarloUncertainty monteCarloUncertainty(arguments.binning, arguments.seed);
	Uncertainty uncertainty(arguments.binning);

	/************ Initialize histograms *************/

	// Input
	TH1F spectrum = TH1F("spectrum", "Input Spectrum",  (Int_t) NBINS, 0., max_bin);
	TH1F n_simulated_particles("n_simulated_particles", "Number of simulated particles per bin", NBINS, 0., max_bin);
	TH2F response_matrix("rema", "Response_Matrix", NBINS, 0., (Double_t) (NBINS - 1), NBINS, 0., max_bin);

	// TopDown algorithm

	TH1F topdown_params("topdown_params", "TopDown Parameters", nbins, 0., max_bin);
	TH1F topdown_fit("topdown_fit", "TopDown Fit",  nbins, 0., max_bin); 
	TH1F topdown_simulation_uncertainty("topdown_simulation_uncertainty", "TopDown Simulation Uncertainty", nbins, 0., max_bin); 
	TH1F topdown_spectrum_uncertainty("topdown_spectrum_uncertainty", "TopDown Spectrum Uncertainty", nbins, 0., max_bin); 
	TH1F topdown_total_uncertainty("topdown_total_uncertainty", "TopDown Total Uncertainty", nbins, 0., max_bin);
	TH1F topdown_FEP("topdown_FEP", "TopDown FEP", nbins, 0., max_bin); 
	TH1F topdown_spectrum_reconstructed("topdown_spectrum_reconstructed", "TopDown Spectrum Reconstructed", nbins, 0., max_bin); 

	// Fit
	TH1F fit_params("fit_params", "Fit Parameters", nbins, 0., max_bin);
	TH1F fit_result("fit_result", "Fit Result", nbins, 0., max_bin); 
	TH1F fit_algorithm_uncertainty("fit_algorithm_uncertainty", "Fit Algorithm Uncertainty", nbins, 0., max_bin);
	TH1F fit_algorithm_FEP_uncertainty("fit_algorithm_FEP_uncertainty", "Fit Algorithm FEP Uncertainty", nbins, 0., max_bin);
	TH1F fit_algorithm_reconstruction_uncertainty("fit_algorithm_reconstruction_uncertainty", "Fit Algorithm Reconstruction Uncertainty", nbins, 0., max_bin);
	TH1F fit_simulation_uncertainty("fit_simulation_uncertainty", "Fit Simulation Uncertainty", nbins, 0., max_bin);
	TH1F fit_spectrum_uncertainty("fit_spectrum_uncertainty", "Spectrum Uncertainty", nbins, 0., max_bin);
	TH1F fit_total_uncertainty("fit_total_uncertainty", "Total Uncertainty", nbins, 0., max_bin);

	TH1F fit_FEP("fit_FEP", "Fit FEP", nbins, 0., max_bin); 

	TH1F spectrum_reconstructed("spectrum_reconstructed", "Reconstructed Spectrum", nbins, 0., max_bin); 
	TH1F reconstruction_uncertainty("reconstruction_uncertainty", "Reconstruction Uncertainty", nbins, 0., max_bin);
	TH1F reconstruction_uncertainty_low("reconstruction_uncertainty_low", "Reconstruction Uncertainty lower Limit", nbins, 0., max_bin);
	TH1F reconstruction_uncertainty_up("reconstruction_uncertainty_up", "Reconstruction Uncertainty upper Limit", nbins, 0., max_bin);

	// Monte-Carlo Uncertainty
	vector<TH1F*> mc_spectra;
	vector<TH1F*> mc_fit_params_samples;
	vector<TH1F*> mc_FEP_samples;
	vector<TH1F*> mc_reconstruction_samples;

	TH2F mc_matrix;
	TH1F mc_fit_params, mc_fit_params_mean, mc_fit_params_uncertainty, mc_fit_total_uncertainty;
	TH1F mc_fit_FEP, mc_fit_FEP_uncertainty;
	TH1F mc_FEP_uncertainty_low, mc_FEP_uncertainty_up;
	TH1F mc_spectrum_reconstructed, mc_reconstruction_uncertainty;
	TH1F mc_reconstruction_uncertainty_low, mc_reconstruction_uncertainty_up;

	/************ Start ROOT application *************/

	TApplication *app = nullptr;
	if(arguments.interactive_mode){
		argc = 1; // Prevent name clashes with options of TApplication
		app = new TApplication("Reconstruction", &argc, argv);
	}

	/************ Read and rebin spectrum and response matrix *************/
	/************ + initialize Fitter using the response matrix ***********/

	cout << "> Reading spectrum file " << arguments.spectrumfile << " ..." << endl;
	if(arguments.tfile){
		inputFileReader.readROOTSpectrum(spectrum, arguments.spectrumfile, arguments.spectrumname);
	} else{
		inputFileReader.readTxtSpectrum(spectrum, arguments.spectrumfile);
	}
	spectrum.Rebin( (Int_t) arguments.binning);

	cout << "> Reading matrix file " << arguments.matrixfile << " ..." << endl;
	inputFileReader.readMatrix(response_matrix, n_simulated_particles, arguments.matrixfile);
	cout << "> Rebinning response matrix ..." << endl;
	response_matrix.Rebin2D((Int_t) arguments.binning, (Int_t) arguments.binning);
	n_simulated_particles.Rebin((Int_t) arguments.binning);

	Fitter fitter(response_matrix, arguments.binning, binstart, binstop);

	/************ Create output file *****************/

	cout << "> Opening output file " << arguments.outputfile << " ..." << endl;

	stringstream outputfilename;
	outputfilename << arguments.outputfile;

	TFile *outputfile = new TFile(outputfilename.str().c_str(), "RECREATE");

	TDirectory *td_mc = nullptr;
	TDirectory *td_mc_spectra = nullptr;
	TDirectory *td_mc_fit_parameters = nullptr;
	TDirectory *td_mc_FEP = nullptr;
	TDirectory *td_mc_reconstructed = nullptr;

	outputfile->Close();

	/************ Use Top-Down unfolding to get start parameters *************/

	cout << "> Unfold spectrum using top-down algorithm ..." << endl;
	fitter.topdown(spectrum, response_matrix, topdown_params, binstart, binstop);

	fitter.fittedFEP(topdown_params, response_matrix, topdown_FEP);
	fitter.fittedSpectrum(topdown_params, response_matrix, topdown_fit);

	reconstructor.reconstruct(topdown_params, n_simulated_particles, topdown_spectrum_reconstructed);

	uncertainty.getUncertainty(topdown_params, spectrum, response_matrix, topdown_simulation_uncertainty, topdown_spectrum_uncertainty, binstart, binstop);

	vector<TH1F*> topdown_uncertainties(2);
	topdown_uncertainties[0] = &topdown_simulation_uncertainty;
	topdown_uncertainties[1] = &topdown_spectrum_uncertainty;
	uncertainty.getTotalUncertainty(topdown_uncertainties, topdown_total_uncertainty);

	fitter.remove_negative(topdown_params);

	/************ Fit *************/

	cout << "> Fit spectrum using TopDown parameters as start parameters ..." << endl;

	fitter.fit(spectrum, response_matrix, topdown_params, fit_params, fit_algorithm_uncertainty, binstart, binstop, arguments.verbose, arguments.correlation, correlation_matrix);

	fitter.print_fitresult();

	fitter.fittedFEP(fit_params, response_matrix, fit_FEP);
	fitter.fittedSpectrum(fit_params, response_matrix, fit_result);

	reconstructor.reconstruct(fit_params, n_simulated_particles, spectrum_reconstructed);

	if(arguments.use_mc){
		// Create directories in TFile for MC output

		outputfile = new TFile(outputfilename.str().c_str(), "UPDATE");
		td_mc = outputfile->mkdir("monte_carlo");
		td_mc_spectra = td_mc->mkdir("spectra");
		td_mc_fit_parameters = td_mc->mkdir("fit_parameters");
		td_mc_FEP = td_mc->mkdir("fep");
		td_mc_reconstructed = td_mc->mkdir("reconstructed");
		outputfile->Close();

		cout << "> Using Monte-Carlo algorithm to determine fit uncertainty (NRANDOM == " << arguments.uncertainty_mc << ")" << endl;

		stringstream histname("");
		if(!arguments.use_mc_fast){
			mc_matrix = TH2F("modified_response_matrix", "MC ResponseMatrix", nbins, 0., max_bin, nbins, 0., max_bin);
		}

		mc_fit_params = TH1F ("mc_fit_params", "MC Fit Parameters", nbins, 0., max_bin);

		// If the '-W' option is used, always write new MC spectra to the 0-th entry of the
		// vectors mc_spectra, mc_FEP_samples and mc_reconstruction_samples to save memory
		if(arguments.write_mc_only){
			mc_spectra.push_back(new TH1F(histname.str().c_str(), histname.str().c_str(), nbins,  0., max_bin));
			mc_FEP_samples.push_back(new TH1F(histname.str().c_str(), histname.str().c_str(), nbins, 0., max_bin));
			mc_reconstruction_samples.push_back(new TH1F(histname.str().c_str(), histname.str().c_str(), nbins, 0., max_bin));
			mc_fit_params_samples.push_back(new TH1F(histname.str().c_str(), histname.str().c_str(), nbins, 0., max_bin));
		}

		UInt_t j = 0;

		for(UInt_t i = 0; i < arguments.uncertainty_mc; ++i){

			// If '-W' option is used, do not push back new entries, but work with the 0-th element
			if(arguments.write_mc_only){
				histname << "mc_spectrum_" << i;
				mc_spectra[0] = new TH1F(histname.str().c_str(), histname.str().c_str(), nbins,  0., max_bin);
				histname.str("");
				histname << "mc_FEP_" << i;
				mc_FEP_samples[0] = new TH1F(histname.str().c_str(), histname.str().c_str(), nbins, 0., max_bin);

				histname.str("");
				histname << "mc_reconstructed_spectrum_" << i;
				mc_reconstruction_samples[0] = new TH1F(histname.str().c_str(), histname.str().c_str(), nbins, 0., max_bin);
			} else{
				histname << "mc_spectrum_" << i;
				mc_spectra.push_back(new TH1F(histname.str().c_str(), histname.str().c_str(), nbins,  0., max_bin));
				histname.str("");
				histname << "mc_FEP_" << i;
				mc_FEP_samples.push_back(new TH1F(histname.str().c_str(), histname.str().c_str(), nbins, 0., max_bin));

				histname.str("");
				histname << "mc_reconstructed_spectrum_" << i;
				mc_reconstruction_samples.push_back(new TH1F(histname.str().c_str(), histname.str().c_str(), nbins, 0., max_bin));

				j = i;
			}

			monteCarloUncertainty.apply_fluctuations(*mc_spectra[j], spectrum, nbins, binstop);

			histname.str("");
			histname << "mc_fit_params_" << i;
			mc_fit_params = TH1F(histname.str().c_str(), histname.str().c_str(), nbins,  0., max_bin);

			if(arguments.use_mc_fast){
				fitter.fit(*mc_spectra[j], response_matrix, fit_params, mc_fit_params, binstart, binstop);
			} else{
				monteCarloUncertainty.apply_fluctuations(mc_matrix, response_matrix, binstart, binstop);
				fitter.fit(*mc_spectra[j], mc_matrix, fit_params, mc_fit_params, binstart, binstop);
			}

			if(arguments.write_mc_only){
				mc_fit_params_samples[0] = &mc_fit_params;
			}
			else{
				mc_fit_params_samples.push_back(&mc_fit_params);
			}

			reconstructor.reconstruct(mc_fit_params, n_simulated_particles, *mc_reconstruction_samples[j]);
			fitter.fittedFEP(mc_fit_params, response_matrix, *mc_FEP_samples[j]);
			histname.str("");

			if(i % MC_UPDATE_INTERVAL == 0 && i > 0)
				cout << "\t> Processed " << i << " Monte-Carlo iterations" << endl;

			if(arguments.write_mc){
				outputfile = new TFile(outputfilename.str().c_str(), "UPDATE");

				td_mc_spectra = (TDirectory*) outputfile->Get("monte_carlo/spectra");
				td_mc_spectra->cd();
				mc_spectra[j]->Write();

				td_mc_fit_parameters = (TDirectory*) outputfile->Get("monte_carlo/fit_parameters");
				td_mc_fit_parameters->cd();
				mc_fit_params_samples[j]->Write();

				td_mc_FEP = (TDirectory*) outputfile->Get("monte_carlo/fep");
				td_mc_FEP->cd();
				mc_FEP_samples[j]->Write();

				td_mc_reconstructed = (TDirectory*) outputfile->Get("monte_carlo/reconstructed");
				td_mc_reconstructed->cd();
				mc_reconstruction_samples[j]->Write();

				outputfile->Close();
			}
		}
		cout << "\t> Processed " << arguments.uncertainty_mc << " Monte-Carlo iterations" << endl;
	}
	/************ Uncertainties *************/

	vector<TH1F*> uncertainties;

	// Uncertainty of Monte-Carlo method
	if(arguments.use_mc && !arguments.write_mc_only){

		cout << "> Evaluating Monte-Carlo results ..." << endl;

		mc_fit_params_mean = TH1F("mc_fit_params_mean", "MC Fit Parameters", nbins, 0., max_bin);
		mc_fit_params_uncertainty = TH1F("mc_fit_params_uncertainty", "MC Fit Parameters Uncertainty", nbins, 0., max_bin);
		mc_fit_total_uncertainty = TH1F("mc_fit_total_uncertainty", "MC Fit Total Uncertainty", nbins, 0., max_bin);

		mc_fit_FEP = TH1F("mc_fit_FEP", "MC Fit FEP", nbins, 0., max_bin);
		mc_fit_FEP_uncertainty = TH1F("mc_fit_FEP_uncertainty", "MC Fit FEP Uncertainty", nbins, 0., max_bin);
		mc_FEP_uncertainty_low = TH1F("mc_FEP_uncertainty_low", "MC Fit FEP Uncertainty lower Limit", nbins, 0., max_bin);
		mc_FEP_uncertainty_up = TH1F("mc_FEP_uncertainty_up", "MC Fit FEP Uncertainty upper Limit", nbins, 0., max_bin);

		mc_spectrum_reconstructed = TH1F("mc_spectrum_reconstructed", "MC Reconstructed Spectrum", nbins, 0., max_bin);
		mc_reconstruction_uncertainty = TH1F("mc_reconstruction_uncertainty", "MC Reconstruction Uncertainty", nbins, 0., max_bin);
		mc_reconstruction_uncertainty_low = TH1F("mc_reconstruction_uncertainty_low", "MC Reconstruction Uncertainty lower Limit", nbins, 0., max_bin);
		mc_reconstruction_uncertainty_up = TH1F("mc_reconstruction_uncertainty_up", "MC Reconstruction Uncertainty upper Limit", nbins, 0., max_bin);

		monteCarloUncertainty.evaluateMeanAndStd(mc_fit_params_mean, mc_fit_params_uncertainty, mc_fit_params_samples, binstart, binstop);

		// Use the fit uncertainty from a single fit as an estimate for the uncertainty
		// of the fitting algorithm
		uncertainties.push_back(&fit_algorithm_uncertainty);
		uncertainties.push_back(&mc_fit_params_uncertainty);
		uncertainty.getTotalUncertainty(uncertainties, mc_fit_total_uncertainty);

		fitter.fittedFEP(mc_fit_params_mean, response_matrix, mc_fit_FEP);
		fitter.fittedFEP(mc_fit_params_uncertainty, response_matrix, mc_fit_FEP_uncertainty);
		uncertainty.getLowerAndUpperLimit(mc_fit_FEP, mc_fit_FEP_uncertainty, mc_FEP_uncertainty_low, mc_FEP_uncertainty_up, true);

		reconstructor.reconstruct(mc_fit_params_mean, n_simulated_particles, mc_spectrum_reconstructed);
		reconstructor.reconstruct(mc_fit_total_uncertainty, n_simulated_particles, mc_reconstruction_uncertainty);
		uncertainty.getLowerAndUpperLimit(mc_spectrum_reconstructed, mc_reconstruction_uncertainty, mc_reconstruction_uncertainty_low, mc_reconstruction_uncertainty_up, true);

	}

	// Uncertainty of single fit

	uncertainty.getUncertainty(fit_params, spectrum, response_matrix, fit_simulation_uncertainty, fit_spectrum_uncertainty, binstart, binstop);
	fitter.fittedFEP(fit_algorithm_uncertainty, response_matrix, fit_algorithm_FEP_uncertainty);
	uncertainties.push_back(&fit_algorithm_FEP_uncertainty);
	uncertainties.push_back(&fit_simulation_uncertainty);
	uncertainties.push_back(&fit_spectrum_uncertainty);
	uncertainty.getTotalUncertainty(uncertainties, fit_total_uncertainty);

	reconstructor.uncertainty(fit_total_uncertainty, response_matrix, n_simulated_particles, reconstruction_uncertainty);

	uncertainty.getLowerAndUpperLimit(spectrum_reconstructed, reconstruction_uncertainty, reconstruction_uncertainty_low, reconstruction_uncertainty_up, true);

	/************ Plot results *************/

	TCanvas c1("c1", "Plots", 4);
	if(arguments.interactive_mode){
		cout << "> Creating plots ..." << endl;

		c1.Divide(2, 2, (Float_t) 0.01, (Float_t) 0.01);

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
		fit_result.SetLineColor(kRed);
		fit_result.Draw();
		fit_FEP.SetLineColor(kGreen);
		fit_FEP.Draw("same");
		spectrum.SetLineColor(kBlack);
		spectrum.Draw("same");

		c1.cd(4);
		spectrum_reconstructed.SetLineColor(kBlack); 
		spectrum_reconstructed.SetLineWidth(2); 
	       	spectrum_reconstructed.Draw();
		reconstruction_uncertainty_up.SetFillColor(kGray); 
		reconstruction_uncertainty_up.SetLineColor(kBlack); 
		reconstruction_uncertainty_up.Draw("same"); 
		reconstruction_uncertainty_low.SetLineColor(kBlack); 
		reconstruction_uncertainty_low.SetFillColor(10); 
		reconstruction_uncertainty_low.Draw("same"); 
		// Draw spectrum_reconstructed twice. Once first in the canvas so that it determines the title of the canvas. Second at the end so that it is on top of everything.
		spectrum_reconstructed.SetLineColor(kBlack); 
		spectrum_reconstructed.SetLineWidth(2); 
		spectrum_reconstructed.Draw("same");
	}

	/************ Write results to file *************/

	// Write (rebinned) original spectrum
	outputfile = new TFile(outputfilename.str().c_str(), "UPDATE");

	spectrum.Write();
	spectrum_reconstructed.Write();
	reconstruction_uncertainty.Write();
	reconstruction_uncertainty_low.Write();
	reconstruction_uncertainty_up.Write();
	n_simulated_particles.Write();

	// Write TopDown results
	TDirectory *td_topdown = outputfile->mkdir("topdown");
	td_topdown->cd();

	topdown_params.Write();
	topdown_FEP.Write();
	topdown_fit.Write();
	topdown_simulation_uncertainty.Write();
	topdown_spectrum_uncertainty.Write();
	topdown_total_uncertainty.Write();
	topdown_spectrum_reconstructed.Write();
	outputfile->cd();

	// Write fit results
	TDirectory * td_fit = outputfile->mkdir("fit");
	td_fit->cd();

	fit_params.Write();
	fit_algorithm_uncertainty.Write();
	fit_algorithm_FEP_uncertainty.Write();
	fit_algorithm_reconstruction_uncertainty.Write();
	fit_simulation_uncertainty.Write();
	fit_spectrum_uncertainty.Write();
	fit_total_uncertainty.Write();

	fit_FEP.Write();
	fit_result.Write();
	
	// Write Monte-Carlo results (if Monte-Carlo uncertainty determination is activated)
	if(arguments.use_mc){
		td_mc = (TDirectory*) outputfile->Get("monte_carlo");
		td_mc->cd();

		mc_fit_params_mean.Write();
		mc_fit_params_uncertainty.Write();
		fit_algorithm_uncertainty.Write();
		fitter.fittedFEP(fit_algorithm_uncertainty, response_matrix, fit_algorithm_FEP_uncertainty);
		reconstructor.reconstruct(fit_algorithm_uncertainty, n_simulated_particles, fit_algorithm_reconstruction_uncertainty);
		mc_fit_total_uncertainty.Write();

		mc_fit_FEP.Write();
		mc_fit_FEP_uncertainty.Write();
		mc_FEP_uncertainty_low.Write();
		mc_FEP_uncertainty_up.Write();

		if(!arguments.write_mc_only){
			mc_spectrum_reconstructed.Write();
			mc_reconstruction_uncertainty.Write();
			mc_reconstruction_uncertainty_low.Write();
			mc_reconstruction_uncertainty_up.Write();
		}
	}

	outputfile->Close();

	cout << "> Wrote output file " << arguments.outputfile << " ..." << endl;

	outputfilename.str("");
	outputfilename << arguments.correlation_matrix_filename;

	if(arguments.correlation){
		cout << "> Writing correlation matrix to output file " << outputfilename.str() << " ..." << endl;
		inputFileReader.writeCorrelationMatrix(correlation_matrix, outputfilename.str().c_str());
	}

	time(&stop);
	cout << "> Execution time: " << stop - start << " seconds" << endl;

	if(arguments.interactive_mode){
		cout << "> Starting interactive plot ..." << endl;
		app->Run();
	}
}
