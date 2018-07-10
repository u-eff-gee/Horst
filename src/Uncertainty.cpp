#include "Config.h"
#include "FitFunction.h"
#include "Uncertainty.h"

void Uncertainty::getUncertainty(const TH1F &params, const TH1F &spectrum, const TH2F &rema,TH1F &simulation_statistical_uncertainty, TH1F &spectrum_statistical_uncertainty, const Int_t binstart, const Int_t binstop){
	FitFunction fitFunction(rema, binstart, binstop);

	for(Int_t i = 1; i <= (Int_t) NBINS/ (Int_t) BINNING; ++i){
		if(i < binstart || i > binstop){
			simulation_statistical_uncertainty.SetBinContent(i, 0.);
			spectrum_statistical_uncertainty.SetBinContent(i, 0.);
		} else{
			simulation_statistical_uncertainty.SetBinContent(i, fitFunction.getSimulationStatisticalUncertainty(i, params));
			spectrum_statistical_uncertainty.SetBinContent(i, fitFunction.getSpectrumStatisticalUncertainty(i, params, spectrum));
		}
	}
}

void Uncertainty::getTotalUncertainty(vector<TH1F*> &uncertainties, TH1F &total_uncertainty){
	Double_t bin_content = 0.;

	for(Int_t i = 1; i <= (Int_t) NBINS/ (Int_t) BINNING; ++i){
		bin_content = 0.;
		for(auto unc : uncertainties){
			bin_content += unc->GetBinContent(i)*unc->GetBinContent(i);
		}
		total_uncertainty.SetBinContent(i, sqrt(bin_content));
	}
}
