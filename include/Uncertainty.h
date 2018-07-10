#ifndef UNCERTAINTY_H
#define UNCERTAINTY_H 1

#include <vector>

#include <TROOT.h>
#include <TH1.h>
#include <TH2.h>

using std::vector;

class Uncertainty{
public:
	Uncertainty(){};
	~Uncertainty(){};

	void getUncertainty(const TH1F &params, const TH1F &spectrum, const TH2F &rema, TH1F &simulation_statistical_uncertainty, TH1F &spectrum_statistical_uncertainty, const Int_t binstart, const Int_t binstop);
	void getTotalUncertainty(vector<TH1F*> &uncertainties, TH1F &total_uncertainty);

};

#endif
