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
