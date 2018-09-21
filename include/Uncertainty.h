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

#ifndef UNCERTAINTY_H
#define UNCERTAINTY_H 1

#include <vector>

#include <TROOT.h>
#include <TH1.h>
#include <TH2.h>

using std::vector;

class Uncertainty{
public:
	Uncertainty(const UInt_t binning): BINNING(binning){};
	~Uncertainty(){};

	void getUncertainty(const TH1F &params, const TH2F &rema, TH1F &simulation_statistical_uncertainty, const Int_t binstart, const Int_t binstop); // Version of Uncertainty::getUncertainty() which does not calculate the statistical uncertainty of the spectrum.
	void getUncertainty(const TH1F &params, const TH1F &spectrum, const TH2F &rema, TH1F &simulation_statistical_uncertainty, TH1F &spectrum_statistical_uncertainty, const Int_t binstart, const Int_t binstop);

	void getTotalUncertainty(vector<TH1F*> &uncertainties, TH1F &total_uncertainty);

	void getLowerAndUpperLimit(const TH1F &spectrum, const TH1F &uncertainty, TH1F &uncertainty_low, TH1F &uncertainty_up, Bool_t no_zeros);

private:
	const UInt_t BINNING;
};

#endif
