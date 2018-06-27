#include <iostream>

#include "Fitter.h"
#include "Config.h"

using std::cout;
using std::endl;

void Fitter::topdown(const TH1F &spectrum, const TH2F &rema, TH1F &params, TH1F &unfolded_spectrum, Int_t binstart, Int_t binstop){

	cout << "> Unfold spectrum using top-down algorithm ..." << endl;

	Double_t parameter = 0.;
	for(Int_t i = 0; i < (Int_t) NBINS/((Int_t) BINNING); ++i){
		unfolded_spectrum.SetBinContent(i, spectrum.GetBinContent(i));
		params.SetBinContent(i, 0.);
	}

	for(Int_t i = binstop; i >= binstart; --i){
		parameter = unfolded_spectrum.GetBinContent(i)/rema.GetBinContent(i, i);
		params.SetBinContent(i, parameter);

		for(Int_t j = binstop - 1; j >= 0; --j){
			unfolded_spectrum.SetBinContent(j, unfolded_spectrum.GetBinContent(j) - parameter*rema.GetBinContent(i, j));
		}
	}
}
