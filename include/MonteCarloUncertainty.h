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

#ifndef MONTECARLOUNCERTAINTY_H
#define MONTECARLOUNCERTAINTY_H 1

#include <vector>

#include <TH1.h>
#include <TH2.h>
#include <TROOT.h>
#include <TRandom3.h>

using std::vector;

class MonteCarloUncertainty{
public:
	MonteCarloUncertainty(const UInt_t binning): BINNING(binning) { random_generator = new TRandom3(); };
	~MonteCarloUncertainty(){ delete random_generator; };

	void apply_fluctuations(TH1F &modified_spectrum, const TH1F &spectrum, const Int_t binstart, const Int_t binstop);
	void apply_fluctuations(TH2F &modified_response_matrix, const TH2F &response_matrix, const Int_t binstart, const Int_t binstop);
	void getSpectrumUncertainty(TH1F &mc_reconstruction_mean, TH1F &mc_spectrum_uncertainty, const vector<TH1F> &mc_reconstructed_spectra, const Int_t binstart, const Int_t binstop);

private:
	Double_t get_positive_random_normal(Double_t mu, Double_t sigma) const;	
	Double_t get_random_poisson(Int_t mean) const;	
	Double_t get_mean(vector<Double_t> &values) const;
	Double_t get_stdev(vector<Double_t> &values, Double_t mean) const;

	TRandom3 *random_generator;
	const UInt_t BINNING;
};

#endif
