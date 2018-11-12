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
#include <vector>

#include <TFitResult.h>

#include "Config.h"
#include "Fitter.h"

using std::cout;
using std::endl;
using std::vector;

void Fitter::topdown(const TH1F &spectrum, const TH2F &rema, TH1F &params, Int_t binstart, Int_t binstop){

	TH1F topdown_unfolded_spectrum("topdown_unfolded_spectrum", "Unfolded_Spectrum_TopDown", (Int_t) NBINS/ (Int_t) BINNING, 0., (Double_t) NBINS - 1);

	Double_t parameter = 0.;
	for(Int_t i = 0; i <= (Int_t) NBINS/((Int_t) BINNING); ++i){
		topdown_unfolded_spectrum.SetBinContent(i, spectrum.GetBinContent(i));
		params.SetBinContent(i, 0.);
	}

	for(Int_t i = binstop; i >= binstart; --i){
		parameter = topdown_unfolded_spectrum.GetBinContent(i)/rema.GetBinContent(i, i);
		params.SetBinContent(i, parameter);

		for(Int_t j = binstop - 1; j >= 0; --j){
			topdown_unfolded_spectrum.SetBinContent(j, topdown_unfolded_spectrum.GetBinContent(j) - parameter*rema.GetBinContent(i, j));
		}
	}
}

void Fitter::fit(TH1F &spectrum, const TH2F &rema, const TH1F &start_params, TH1F &params, TH1F &fit_uncertainty, Int_t binstart, Int_t binstop, const Bool_t verbose, const Bool_t correlation, TMatrixDSym &correlation_matrix){

	fitFunction.setResponseMatrix(rema);

	Double_t fit_upper_limit = 10.*start_params.GetMaximum();

	for(Int_t i = 0; i <= start_params.GetNbinsX(); ++i){
		if(i < binstart || i >= binstop){
			fitf->FixParameter(i, 0.);
		} else{
			fitf->SetParameter(i, start_params.GetBinContent(i));
			fitf->SetParLimits(i, 0., fit_upper_limit);
		}
	}

	TFitResultPtr fit_result;
	if(verbose){
		if(correlation){
			fit_result = spectrum.Fit("fitf", "S0N", "", (UInt_t) binstart*BINNING, (UInt_t) binstop*BINNING);
			correlation_matrix = fit_result->GetCorrelationMatrix();
		} else{
			spectrum.Fit("fitf", "0N", "", (UInt_t) binstart*BINNING, (UInt_t) binstop*BINNING);
		}
	} else{
		if(correlation){
			fit_result = spectrum.Fit("fitf", "S0QN", "", (UInt_t) binstart*BINNING, (UInt_t) binstop*BINNING);
			correlation_matrix = fit_result->GetCorrelationMatrix();
		} else{
			spectrum.Fit("fitf", "0QN", "", (UInt_t) binstart*BINNING, (UInt_t) binstop*BINNING);
		}
	}

	chi2 = fitf->GetChisquare();

	for(Int_t i = 1; i <= start_params.GetNbinsX(); ++i){
		params.SetBinContent(i, fitf->GetParameter(i-1));
		fit_uncertainty.SetBinContent(i, fitf->GetParError(i-1));
	}
}

void Fitter::fit(TH1F &spectrum, const TH2F &rema, const TH1F &start_params, TH1F &params, Int_t binstart, Int_t binstop){

	fitFunction.setResponseMatrix(rema);

	Double_t fit_upper_limit = 10.*start_params.GetMaximum();

	for(Int_t i = 0; i <= start_params.GetNbinsX(); ++i){
		if(i < binstart || i >= binstop){
			fitf->FixParameter(i, 0.);
		} else{
			fitf->SetParameter(i, start_params.GetBinContent(i));
			fitf->SetParLimits(i, 0., fit_upper_limit);
		}

	}

	spectrum.Fit("fitf", "0QN", "", (UInt_t) binstart*BINNING, (UInt_t) binstop*BINNING);

	for(Int_t i = 1; i <= start_params.GetNbinsX(); ++i){
		params.SetBinContent(i, fitf->GetParameter(i-1));
	}
}

void Fitter::fittedFEP(const TH1F &params, const TH2F &rema, TH1F &fitted_FEP){
	for(Int_t i = 1; i <= (Int_t) NBINS/ (Int_t) BINNING; ++i){
		fitted_FEP.SetBinContent(i, params.GetBinContent(i)*rema.GetBinContent(i, i));
	}
}

void Fitter::fittedSpectrum(const TH1F &params, const TH2F &rema, TH1F &fitted_spectrum){

	vector<Double_t> parameters((long unsigned int) NBINS/ (long unsigned int) BINNING + 1);
	for(Int_t i = 1; i <= (Int_t) NBINS/ (Int_t) BINNING; ++i)
		parameters[(long unsigned int) i] = params.GetBinContent(i);

	Double_t bin = 0.;
	for(Int_t i = 1; i <= (Int_t) NBINS/ (Int_t) BINNING; ++i){
		bin = (Double_t) i*BINNING;
		fitted_spectrum.SetBinContent(i, fitFunction(&bin, &parameters[0]));
	}
}

void Fitter::remove_negative(TH1F &hist){

	for(Int_t i = 1; i <= hist.GetNbinsX(); ++i){
		if(hist.GetBinContent(i) < 0.){
			hist.SetBinContent(i, 0.);
		}
	}
}

void Fitter::print_fitresult() const {
	cout << "> Fit result: Chi^2 = " << chi2 << " (care is to be taken with the interpretation of this value) " << endl;
}
