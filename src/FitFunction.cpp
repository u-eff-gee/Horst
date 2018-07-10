#include "FitFunction.h"
#include "Config.h"

#include <iostream>

using std::cout;
using std::endl;

Double_t FitFunction::operator()(Double_t *x, Double_t *p){
	Int_t bin = (Int_t) round(x[0]/(Double_t) BINNING);
	Double_t bin_content = 0.;

	for(Int_t i = bin_stop; i >= bin; --i){
		bin_content += p[i]*response_matrix.GetBinContent(i, bin);
	}

	return bin_content;
}

Double_t FitFunction::getSimulationStatisticalUncertainty(const Int_t bin, const TH1F &params) {
	Double_t bin_content = 0.;

	for(Int_t i = bin_stop; i > bin; --i){
		bin_content += params.GetBinContent(i)*response_matrix.GetBinContent(i, bin);
	}

	return sqrt(bin_content);
}

Double_t FitFunction::getSpectrumStatisticalUncertainty(const Int_t bin, const TH1F &params, const TH1F &spectrum) {
	Double_t bin_content = 0.;
	Double_t spectrum_bin_content = 1.;

	for(Int_t i = bin_stop; i > bin; --i){
		spectrum_bin_content = spectrum.GetBinContent(i);
		if(spectrum_bin_content > 0.){	// Ignore bins with negative values (should not be in the original spectrum anyway) or zero content.
			bin_content += params.GetBinContent(i)*1./spectrum_bin_content*response_matrix.GetBinContent(i, bin)*response_matrix.GetBinContent(i, bin);
		}
	}

	return sqrt(bin_content);
}