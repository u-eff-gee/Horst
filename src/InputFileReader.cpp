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

#include <TFile.h>
#include <TH1.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Config.h"
#include "InputFileReader.h"

using std::cout;
using std::endl;
using std::ifstream;
using std::string;
using std::stringstream;

void InputFileReader::readInputFile(const TString inputfilename, vector<TString> &filenames, vector<Double_t> &energies, vector<Double_t> &n_simulated_particles){
	
	cout << "> Reading input file " << inputfilename << " ..." << endl;

	ifstream file;	
	file.open(inputfilename);
	string line, energy, filename, n_particles;
	stringstream sst;

	if(file.is_open()){
		while(getline(file, line)){
			sst.str(line);
			sst >> filename >> energy >> n_particles;
			filenames.push_back(filename);
			energies.push_back(atof(energy.c_str()));
			n_simulated_particles.push_back(atof(n_particles.c_str()));
			sst.clear();
		}

	} else{
		cout << "Error: File " << inputfilename << " could not be opened." << endl;
	}
}

void InputFileReader::fillMatrix(const vector<TString> &filenames, const vector<Double_t> &energies, const vector<Double_t> &n_particles, const TString histname, TH2F &response_matrix, TH1F &n_simulated_particles){
	cout << "> Creating matrix ..." << endl;

	Double_t min_dist = (Double_t) NBINS;
	Double_t dist;
	Int_t best_simulation = 0;
	Int_t n_energies = (Int_t) energies.size();
	Int_t simulation_shift = 0;

	for(Int_t i = 1; i <= (Int_t) NBINS; ++i){
		// Find best simulation for energy bin
		min_dist = (Double_t) NBINS;
		best_simulation = 0;

		for(Int_t j = 0; j < n_energies; ++j){
			dist = energies[(long unsigned int) j] - (Double_t) i;	
			if(fabs(dist) < fabs(min_dist)){
				min_dist = dist;
				best_simulation = j;
			}
		}

		cout << "Bin: " << i << " keV, using " << filenames[(long unsigned int) best_simulation] << " ( " << energies[(long unsigned int) best_simulation] << " )" << endl;

		// Fill row of matrix with best simulation
		TFile *inputFile = new TFile(filenames[(long unsigned int) best_simulation]);
		TH1F *hist = nullptr;

		if(gDirectory->FindKey(histname)){
			hist = (TH1F*) gDirectory->Get(histname);
		} else{
			cout << __FILE__ << ":" << __FUNCTION__ << "():" << __LINE__ << ": Error: No TH1F object called '" << histname << "' found in '" << filenames[(long unsigned int) best_simulation] << "'. Aborting ..." << endl; 
			abort();
		}

		simulation_shift = (Int_t) min_dist;	
		for(Int_t j = 1; j <= (Int_t) NBINS; ++j){
			if(j + simulation_shift < (Int_t) NBINS && (j + simulation_shift) >= 0){
				response_matrix.SetBinContent(i, j, hist->GetBinContent(j + simulation_shift));
			}
		}

		// Fill number of simulated particles into TH1F
		n_simulated_particles.SetBinContent(i, n_particles[best_simulation]);

		inputFile->Close();
	}
}

void InputFileReader::writeMatrix(TH2F &response_matrix, TH1F &n_simulated_particles, TString outputfilename) const {
	TFile *outputFile = new TFile(outputfilename, "RECREATE");	

	response_matrix.Write();
	n_simulated_particles.Write();
	outputFile->Close();

	cout << "> Wrote matrix to file " << outputfilename << endl;
}

void InputFileReader::readMatrix(TH2F &response_matrix, TH1F &n_simulated_particles, const TString matrixfile){

	TFile *inputFile = new TFile(matrixfile); 
	TH2F *rema = nullptr;

	if(gDirectory->FindKey("rema")){
		rema = (TH2F*) gDirectory->Get("rema");
	} else{
		cout << __FILE__ << ":" << __FUNCTION__ << "():" << __LINE__ << ": Error: No TH2F object called 'rema' found in '" << matrixfile << "'. Aborting ..." << endl;
		abort();
	}

	for(Int_t i = 1; i <= (Int_t) NBINS; ++i){
		for(Int_t j = 1; j <= (Int_t) NBINS; ++j){
			response_matrix.SetBinContent(i, j, rema->GetBinContent(i, j));
		}
	}

	TH1F *n_particles = nullptr;
	
	if(gDirectory->FindKey("n_simulated_particles")){
		n_particles = (TH1F*) gDirectory->Get("n_simulated_particles");
	} else{
		cout << __FILE__ << ":" << __FUNCTION__ << "():" << __LINE__ << ": Error: No TH1F object called 'n_particles' found in '" << matrixfile << "'. Aborting ..." << endl;
		abort();
	}
	for(Int_t i = 1; i <= (Int_t) NBINS; ++i){
		n_simulated_particles.SetBinContent(i, n_particles->GetBinContent(i));
	}

	inputFile->Close();
}

void InputFileReader::readTxtSpectrum(TH1F &spectrum, const TString spectrumfile){
	
	ifstream file;	
	file.open(spectrumfile);
	string line = "";
	Int_t index = 1;

	if(file.is_open()){
		while(getline(file, line)){
			spectrum.SetBinContent(index, atof(line.c_str()));
			++index;
		}
	}
}

void InputFileReader::readROOTSpectrum(TH1F &spectrum, const TString spectrumfile, const TString spectrumname){
	
	TFile file;
	file.Open(spectrumfile, "READ");

	TH1F *spec= (TH1F*) gDirectory->Get(spectrumname);

	for(Int_t i = 0; i <= (Int_t) NBINS/((Int_t) BINNING); ++i){
		spectrum.SetBinContent(i, spec->GetBinContent(i));
	}
}
