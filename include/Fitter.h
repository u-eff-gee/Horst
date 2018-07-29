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

#ifndef FITTER_H
#define FITTER_H 1

#include <TROOT.h>
#include <TH1.h>
#include <TH2.h>

class Fitter{
public:
	Fitter(const UInt_t binning):BINNING(binning){};
	~Fitter(){};

	void topdown(const TH1F &spectrum, const TH2F &rema, TH1F &params, Int_t binstart, Int_t binstop);
	void fit(TH1F &spectrum, const TH2F &rema, const TH1F &start_params, TH1F &params, TH1F &fit_uncertainty, Int_t binstart, Int_t binstop, const Bool_t verbose);
	void fittedFEP(const TH1F &params, const TH2F &rema, TH1F &fitted_FEP);
	void fittedSpectrum(const TH1F &params, const TH2F &rema, TH1F &fitted_spectrum);
	void remove_negative(TH1F &hist);

private:
	const UInt_t BINNING;
};

#endif
