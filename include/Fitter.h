#ifndef FITTER_H
#define FITTER_H 1

#include <TROOT.h>
#include <TH1.h>
#include <TH2.h>

class Fitter{
public:
	Fitter(){};
	~Fitter(){};

	void topdown(const TH1F &spectrum, const TH2F &rema, TH1F &params, TH1F &unfolded_spectrum, Int_t binstart, Int_t binstop);
	void fit(TH1F &spectrum, const TH2F &rema, const TH1F &start_params, TH1F &params, TH1F &fit_uncertainty, Int_t binstart, Int_t binstop);
	void fittedFEP(const TH1F &params, const TH2F &rema, TH1F &fitted_FEP);
	void fittedSpectrum(const TH1F &params, const TH2F &rema, TH1F &fitted_spectrum);
	void remove_negative(TH1F &hist);
};

#endif
