#include "Config.h"
#include "Reconstructor.h"

void Reconstructor::reconstruct(const TH1F &params, const TH2F &rema, TH1F &reconstructed_spectrum){
	Int_t nbins_y = rema.GetNbinsY();

	for(Int_t i = 1; i <= (Int_t) NBINS/((Int_t) BINNING); ++i){
		reconstructed_spectrum.SetBinContent(i, params.GetBinContent(i)*rema.Integral(i, i, 1, nbins_y));
	}
}

void Reconstructor::uncertainty(const TH1F &total_uncertainty, const TH2F &rema, TH1F &reconstruction_uncertainty){
	Int_t nbins_y = rema.GetNbinsY();

	for(Int_t i = 1; i <= (Int_t) NBINS/((Int_t) BINNING); ++i){
		reconstruction_uncertainty.SetBinContent(i, total_uncertainty.GetBinContent(i)*rema.Integral(i, i, 1, nbins_y)/rema.GetBinContent(i, i));
	}
}
