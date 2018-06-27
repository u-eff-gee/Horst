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

void InputFileReader::readInputFile(const TString inputfilename, vector<TString> &filenames, vector<Double_t> &energies){

	cout << "> Reading input file " << inputfilename << " ..." << endl;

	ifstream file;	
	file.open(inputfilename);
	string line, energy, filename;
	stringstream sst;

	if(file.is_open()){
		while(getline(file, line)){
			sst.str(line);
			sst >> filename >> energy;
			filenames.push_back(filename);
			energies.push_back(atof(energy.c_str()));
			sst.clear();
		}

	} else{
		cout << "Error: File " << inputfilename << " could not be opened." << endl;
	}
}

void InputFileReader::fillMatrix(const vector<TString> &filenames, const vector<Double_t> &energies, const TString histname, TH2F &response_matrix){
	cout << "> Creating matrix ..." << endl;

	Double_t min_dist = (Double_t) NBINS;
	Double_t dist;
	Int_t best_simulation = 0;
	Int_t n_energies = (Int_t) energies.size();
	Int_t simulation_shift = 0;

	for(Int_t i = 0; i < (Int_t) NBINS; ++i){
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
		TH1F *hist = (TH1F*) gDirectory->Get(histname);
		simulation_shift = (Int_t) min_dist;	
		for(Int_t j = 0; j < (Int_t) NBINS; ++j){
			if(j + simulation_shift < (Int_t) NBINS && (j + simulation_shift) >= 0){
				response_matrix.SetBinContent(i, j, hist->GetBinContent(j + simulation_shift));
			}
		}

		inputFile->Close();
	}
}

void InputFileReader::writeMatrix(TH2F &response_matrix, TString outputfilename) const {
	TFile *outputFile = new TFile(outputfilename, "RECREATE");	

	response_matrix.Write();
	outputFile->Close();

	cout << "> Wrote matrix to file " << outputfilename << endl;
}
