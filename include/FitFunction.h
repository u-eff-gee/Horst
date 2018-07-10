#ifndef FITFUNCTION_H
#define FITFUNCTION_H 1

#include <TH2.h>

class FitFunction{
	public:
		FitFunction(const TH2F &rema, Int_t binstart, Int_t binstop): 
			response_matrix(rema),
			bin_start(binstart),
			bin_stop(binstop)
	{};
		~FitFunction(){};
		Double_t operator()(Double_t *x, Double_t *p);
		Double_t getSimulationStatisticalUncertainty(const Int_t bin, const TH1F &params);
		Double_t getSpectrumStatisticalUncertainty(const Int_t bin, const TH1F &params, const TH1F &spectrum);

	private:
		TH2F response_matrix;
		Int_t bin_start;
		Int_t bin_stop;
};

#endif
