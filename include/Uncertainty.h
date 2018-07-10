#ifndef UNCERTAINTY_H
#define UNCERTAINTY_H 1

#include <TROOT.h>
#include <TH1.h>
#include <TH2.h>

class Uncertainty{
public:
	Uncertainty(){};
	~Uncertainty(){};

	void getUncertainty(const TH1F &params, const TH1F &spectrum, const TH2F &rema, TH1F &simulation_statistical_uncertainty, TH1F &spectrum_statistical_uncertainty, const Int_t binstart, const Int_t binstop);

};

#endif
