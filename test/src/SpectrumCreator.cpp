#include <iostream>

#include "TF1.h"
#include "TFile.h"

#include "ConfigTest.h"
#include "SpectrumCreator.h"

using std::cout;
using std::endl;

void SpectrumCreator::createSpectrum(TH1F &spectrum, const string option){
	if(option == "bar"){
		createBarSpectrum(spectrum, bar_params);
	} else{
		cout << "Error: SpectrumCreator.cpp: createSpectrum(): Unknown option '" << option << "'. Aborting ..." << endl;
		abort();
	}

	cout << "Writing test spectrum to 'test_spectrum.root' ..." << endl;

	TFile spectrumfile("test_spectrum.root", "RECREATE");
	spectrum.Write();
	spectrumfile.Close();
}

void SpectrumCreator::createBarSpectrum(TH1F &spectrum, const vector<Double_t> &params){
	for(Int_t i = 1; i <= NBINS; ++i){
		if(i >= params[1] - params[2] && i <= params[1] + params[2])
			spectrum.SetBinContent(i, 1.);
		else
			spectrum.SetBinContent(i, 0.);
	}

	TF1 *constant_function = new TF1("const", "[&](double *x, double *p){ return 1.; }", 0., NBINS, 0);

	// Normalize spectrum to 1
	spectrum.Multiply(constant_function, 1./spectrum.Integral());
	// Multiply spectrum with the desired number of events
	spectrum.Multiply(constant_function, params[0]);
}
