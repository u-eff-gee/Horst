#include <iostream>

#include <TF1.h>

#include "Config.h"
#include "FitFunction.h"
#include "Fitter.h"

using std::cout;
using std::endl;

void Fitter::topdown(const TH1F &spectrum, const TH2F &rema, TH1F &params, TH1F &unfolded_spectrum, Int_t binstart, Int_t binstop){

	cout << "> Unfold spectrum using top-down algorithm ..." << endl;

	Double_t parameter = 0.;
	for(Int_t i = 1; i <= (Int_t) NBINS/((Int_t) BINNING); ++i){
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

void Fitter::fit(TH1F &spectrum, const TH2F &rema, const TH1F &start_params, TH1F &params, TH1F &fit, Int_t binstart, Int_t binstop){

	cout << "> Fitting spectrum ..." << endl;

	FitFunction fitFunction(rema, binstart, binstop);
	TF1 fitf("fitf", fitFunction, 0., (Double_t) NBINS-1, NBINS/BINNING);

	for(Int_t i = 1; i <= start_params.GetNbinsX(); ++i){
//		if(i < binstart || i > binstop){
//			fitf.FixParameter(i, 0.);
//		} else{
			fitf.SetParameter(i, start_params.GetBinContent(i));
			fitf.SetParLimits(i, 0., 1.);
//		}

	}

	spectrum.Fit("fitf", "", "", binstart*BINNING, binstop*BINNING);

	for(Int_t i = 1; i <= start_params.GetNbinsX(); ++i){
		params.SetBinContent(i, fitf.GetParameter(i-1));
	}
}
