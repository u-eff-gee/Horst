#include "FitFunction.h"
#include "Config.h"

#include <iostream>

using std::cout;
using std::endl;

Double_t FitFunction::operator()(Double_t *x, Double_t *p){
	Int_t bin = (Int_t) round(x[0]/(Double_t) BINNING);
	//cout << "Called for " << x[0] << " <> " << bin << endl;
	Double_t bin_content = 0.;

	for(Int_t i = bin_stop; i >= bin; --i){
		bin_content += p[i]*response_matrix.GetBinContent(i, bin);	
	}

	return bin_content;
}
