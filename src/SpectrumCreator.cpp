#include <iostream>
#include <sstream>

#include "Math/DistFunc.h"
#include "TF1.h"
#include "TFile.h"

#include "ConfigTest.h"
#include "SpectrumCreator.h"

using std::cout;
using std::endl;
using std::stringstream;

using ROOT::Math::normal_pdf;

void SpectrumCreator::createSpectrum(TH1F &spectrum, const string option, const string outputfile_prefix){
	if(option == "bar"){
		createBarSpectrum(spectrum, bar_params);
	} else if(option == "normal"){
		createNormalSpectrum(spectrum, normal_params);
	} else{
		cout << "Error: SpectrumCreator.cpp: createSpectrum(): Unknown option '" << option << "'. Aborting ..." << endl;
		abort();
	}

	stringstream ofname;
	ofname << outputfile_prefix << "_spectrum.root";
	cout << "Writing test spectrum to '" << ofname.str() << "' ..." << endl;

	TFile spectrumfile(ofname.str().c_str(), "RECREATE");
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

void SpectrumCreator::createNormalSpectrum(TH1F &spectrum, const vector<Double_t> &params){
	for(Int_t i = 1; i <= NBINS; ++i){
		// To avoid entering arbitrarily small numbers
		if(i < params[1] - 4.*params[2] && i > params[1] + 4.*params[2]){
			spectrum.SetBinContent(i, 0.);
		} else{
			spectrum.SetBinContent(i, normal_pdf((Double_t) i, params[2], params[1]));
		}
	}

	TF1 *constant_function = new TF1("const", "[&](double *x, double *p){ return 1.; }", 0., NBINS, 0);

	// Normalize spectrum to 1
	spectrum.Multiply(constant_function, 1./spectrum.Integral());
	// Multiply spectrum with the desired number of events
	spectrum.Multiply(constant_function, params[0]);
}
