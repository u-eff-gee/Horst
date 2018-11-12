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

#include <TF1.h>
#include <TH1.h>
#include <TH2.h>
#include <TMatrixDSym.h>
#include <TROOT.h>

#include "FitFunction.h"

class Fitter{
public:
	Fitter(const TH2F &rema, const UInt_t binning, Int_t binstart, Int_t binstop):fitFunction("rema_fit", rema, binning, binstart, binstop), BINNING(binning), chi2(-1.){ fitf = new TF1("fitf", fitFunction, 0., (Double_t) NBINS-1, (Int_t) NBINS/ (Int_t) BINNING); };
	~Fitter(){};

	void topdown(const TH1F &spectrum, const TH2F &rema, TH1F &params, Int_t binstart, Int_t binstop);
	void fit(TH1F &spectrum, const TH2F &rema, const TH1F &start_params, TH1F &params, Int_t binstart, Int_t binstop); // Version of Fitter::fit() which does not return uncertainty and does not print output
	void fit(TH1F &spectrum, const TH2F &rema, const TH1F &start_params, TH1F &params, TH1F &fit_uncertainty, Int_t binstart, Int_t binstop, const Bool_t verbose, const Bool_t correlation, TMatrixDSym &correlation_matrix);
	void fittedFEP(const TH1F &params, const TH2F &rema, TH1F &fitted_FEP);
	void fittedSpectrum(const TH1F &params, const TH2F &rema, TH1F &fitted_spectrum);
	void remove_negative(TH1F &hist);
	void print_fitresult() const;

private:
	const UInt_t BINNING;
	FitFunction fitFunction;
	TF1 *fitf;
	Double_t chi2;
};

#endif
