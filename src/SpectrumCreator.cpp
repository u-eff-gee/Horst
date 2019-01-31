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

#include "Math/DistFunc.h"
#include "TF1.h"
#include "TFile.h"

#include "ConfigTest.h"
#include "InputFileReader.h"
#include "SpectrumCreator.h"

using std::cout;
using std::endl;
using std::stringstream;

using ROOT::Math::normal_pdf;

void SpectrumCreator::createSpectrum(TH1F &spectrum, const string option, const string outputfile_prefix){

	InputFileReader inputFileReader(1);
	stringstream limitfilename;
	limitfilename << TEST_DIR << outputfile_prefix << "_limits.txt";

	if(option == "bar"){
		createBarSpectrum(spectrum, bar_params);
		vector<UInt_t> limits = {(UInt_t) (bar_params[1] - 1.5*bar_params[2]), (UInt_t) (bar_params[1] + 1.5*bar_params[2])};
		inputFileReader.writeParameters(limits, limitfilename.str());
	} else if(option == "normal"){
		createNormalSpectrum(spectrum, normal_params);
		vector<UInt_t> limits = {(UInt_t) (bar_params[1] - 3.*bar_params[2]), (UInt_t) (bar_params[1] + 3.*bar_params[2])};
		inputFileReader.writeParameters(limits, limitfilename.str());
	} else{
		cout << "Error: SpectrumCreator.cpp: createSpectrum(): Unknown option '" << option << "'. Aborting ..." << endl;
		abort();
	}

	stringstream ofname;
	ofname << outputfile_prefix << "_spectrum.root";
	cout << "Writing test spectrum to '" << ofname.str() << "' ..." << endl;

	stringstream resolutionfilename;
	resolutionfilename << TEST_DIR << outputfile_prefix << "_resolution.txt";
	inputFileReader.writeParameters(resolution_params, resolutionfilename.str());

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
