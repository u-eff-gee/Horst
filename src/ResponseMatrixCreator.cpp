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

#include <iostream>
#include <sstream>

#include "TFile.h"

#include "ConfigTest.h"
#include "ResponseMatrixCreator.h"

using std::cout;
using std::endl;
using std::stringstream;

void ResponseMatrixCreator::createResponseMatrix(TH2F &response_matrix, TH1F &n_simulated_particles, const string option, const string outputfile_prefix){
	if(option == "escape"){
		createResponseMatrixWithEscapePeaks(response_matrix, n_simulated_particles, escape_params);
	} 
	else if(option == "efficiency"){
		createResponseMatrixWithEfficiency(response_matrix, n_simulated_particles, efficiency_params);
	}else{
		cout << "Error: ResponseMatrixCreator.cpp: createResponseMatrix(): Unknown option '" << option << "'. Aborting ..." << endl;
		abort();
	}

	stringstream ofname;
	ofname << outputfile_prefix << "_response_matrix.root";
	cout << "Writing test response_matrix to '" << ofname.str() << "' ..." << endl;

	TFile response_matrixfile(ofname.str().c_str(), "RECREATE");
	n_simulated_particles.Write();
	response_matrix.Write();
	response_matrixfile.Close();
}

void ResponseMatrixCreator::createResponseMatrixWithEscapePeaks(TH2F &response_matrix, TH1F &n_simulated_particles, vector<Double_t> params){
	// Fill n_simulated_particles
	for(Int_t i = 1; i <= NBINS; ++i)
		n_simulated_particles.SetBinContent(i, params[0]);

	// Create spectrum with response, assuming that the response is independent of the energy.
	TH1F response_spectrum("response_spectrum", "Response Spectrum", NBINS, 0., (Double_t) (NBINS - 1));

	for(Int_t i = 1; i <= NBINS; ++i)
		response_spectrum.SetBinContent(i, 0.);

	// Set full-energy peak
	response_spectrum.SetBinContent(NBINS, params[1]*params[0]);
	// Set single-escape peak
	response_spectrum.SetBinContent((Int_t) (NBINS - params[3]), params[2]*params[0]);
	// Set double-escape peak
	response_spectrum.SetBinContent((Int_t) (NBINS - params[5]), params[4]*params[0]);

	for(Int_t i = 1; i <= NBINS; ++i){
		for(Int_t j = 1; j <= i; ++j){
			if(j <= i){
				response_matrix.SetBinContent(i, j, response_spectrum.GetBinContent(NBINS - i + j));
			} else{
				response_matrix.SetBinContent(i, j, 0.);
			}
		}
	}
}

void ResponseMatrixCreator::createResponseMatrixWithEfficiency(TH2F &response_matrix, TH1F &n_simulated_particles, vector<Double_t> params){
	// Fill n_simulated_particles
	for(Int_t i = 1; i <= NBINS; ++i)
		n_simulated_particles.SetBinContent(i, params[0]);

	Double_t inverse_NBINS = 1./ ((Double_t) NBINS);
	// Simple and unrealistic model for the efficiency epsilon:
	//
	//              | 1, if i even
	// epsilon(E) = | 
	//              | (E_0 - E)/E_0, else
	//
	// where E_0 is the maximum energy in the spectrum. I.e. the efficiency varies from
	// epsilon = 1 at the left edge of the spectrum to epsilon = 0 at the right edge.
	for(Int_t i = 1; i <= NBINS; ++i){
		for(Int_t j = 1; j <= i; ++j){
			if(j == i){
				if(j < params[1]){
					response_matrix.SetBinContent(i, j, params[0]*params[2]);
				} else{
					response_matrix.SetBinContent(i, j, params[0]*params[3]);
				}
			} else{
				response_matrix.SetBinContent(i, j, 0.);
			}
		}
	}
}
