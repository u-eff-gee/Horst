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

#include <vector>

#include "Math/DistFunc.h"

#include "Config.h"
#include "MonteCarloUncertainty.h"

#define USE_POISSON 1

using std::vector;

using ROOT::Math::normal_cdf;
using ROOT::Math::normal_quantile;

void MonteCarloUncertainty::getSpectrumUncertainty(TH1F &mc_reconstruction_mean, TH1F &mc_spectrum_uncertainty, const vector<TH1F> &mc_reconstructed_spectra, const Int_t binstart, const Int_t binstop){
	const UInt_t nrandom = (UInt_t) mc_reconstructed_spectra.size();

	vector<Double_t> values;
	for(UInt_t i = 1; i <= NBINS / BINNING; ++i){
		if(i < (UInt_t) binstart || i > (UInt_t) binstop){
			mc_reconstruction_mean.SetBinContent((Int_t) i, 0.);
			mc_spectrum_uncertainty.SetBinContent((Int_t) i, 0.);
		} else{
			values = vector<Double_t>(nrandom, 0.);
			for(UInt_t j = 0; j < nrandom; ++j){
				values[j] = mc_reconstructed_spectra[j].GetBinContent((Int_t) i);
			}

			Double_t mean = get_mean(values);
			mc_reconstruction_mean.SetBinContent((Int_t) i, mean);
			mc_spectrum_uncertainty.SetBinContent((Int_t) i, get_stdev(values, mean));
		}
	}
}

void MonteCarloUncertainty::apply_fluctuations(TH1F &modified_spectrum, const TH1F &spectrum, const Int_t binstart, const Int_t binstop){
	// The content of each bin in a measured spectrum is a random sample from a distribution.
	// The experiment is assumed to be a statistical counting experiment of uncorrelated events, where the underlying distribution is a Poissonian distribution P(lambda) with mean value lambda.
	// To simulate the influence of counting statistics on the measured spectrum, assume that the actually measured value of a bin is the mean value lambda of the Poissonian distribution, and sample a new value from it.
	// 
	// Instead of sampling from a discrete Poissonian distribution, which is the 'true' distribution for a counting experiment, random numbers can also be obtained from a continuous normal distribution, because for large numbers of events per bin (N >~ 10), the Poissonian distribution converges to a normal distribution with mean value lambda and variance lambda.
	// Since the normal distribution is defined over the whole range of real numbers, one needs to make sure that only positive numbers are return.
	// The Poissonian distribution does this by default.
	// In the history of 'horst', the normal distribution was used first.
	// However, it was decided to switch to the more general Poissonian distribution.
	// The old implementation is kept here and it can be switched on using the preprocessor variable USE_POISSON
	Double_t mu = 0.;
#ifndef USE_POISSON
	Double_t sigma = 0.;
#endif

	for(Int_t i = 1; i <= (Int_t) NBINS / (Int_t) BINNING; ++i){

		mu = spectrum.GetBinContent(i);
		
		if(i < binstart || i > binstop){
			modified_spectrum.SetBinContent(i, mu);
		}
		if(mu == 0.){
			modified_spectrum.SetBinContent(i, 0.);
		} else{
#ifndef USE_POISSON
			sigma = sqrt(mu);
			modified_spectrum.SetBinContent(i, get_positive_random_normal(mu, sigma));
#endif
#ifdef USE_POISSON
			modified_spectrum.SetBinContent(i, get_random_poisson((Int_t) round(mu)));
#endif
		}
	}
}

void MonteCarloUncertainty::apply_fluctuations(TH2F &modified_response_matrix, const TH2F &response_matrix, const Int_t binstart, const Int_t binstop){
	Double_t mu = 0.;

	for(Int_t i = binstart; i <= binstop; ++i){
		for(Int_t j = binstart; j <= binstop; ++j){
			mu = response_matrix.GetBinContent(i, j);
			if(mu == 0.){
				modified_response_matrix.SetBinContent(i, j, 0.);
			} else{
				modified_response_matrix.SetBinContent(i, j, get_random_poisson((Int_t) round(mu)));
			}
		}
	}
}

Double_t MonteCarloUncertainty::get_positive_random_normal(Double_t mu, Double_t sigma) const {
	return normal_quantile(random_generator->Uniform(normal_cdf(-mu/sigma), 1.), sigma) + mu;
}

Double_t MonteCarloUncertainty::get_random_poisson(Int_t mean) const {
	return random_generator->Poisson(mean);
}

Double_t MonteCarloUncertainty::get_mean(vector<Double_t> &values) const {
	Double_t sum = 0.;
	for(auto v: values)
		sum += v;

	return sum / (Double_t) values.size();
}

Double_t MonteCarloUncertainty::get_stdev(vector<Double_t> &values, Double_t mean) const {
	Double_t sum = 0.;
	for(auto v: values)
		sum += (v - mean)*(v - mean);

	return sqrt(sum / (Double_t) values.size());
}
