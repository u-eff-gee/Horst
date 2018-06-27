#include "FitFunction.h"
#include "Config.h"

Double_t FitFunction::operator()(Double_t *x, Double_t *p){
	Int_t bin = round(x[0]/BINNING);
	Double_t bin_content = 0.;

	for(Int_t i = bin_stop; i >= bin; --i){
		bin_content += p[i]*response_matrix.GetBinContent(i, bin);	
	}

	return bin_content;
}
