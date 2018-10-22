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
	} else{
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
