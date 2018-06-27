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
};

#endif
