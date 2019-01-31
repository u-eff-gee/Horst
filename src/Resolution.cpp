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

#include "Math/DistFunc.h"

#include "Config.h"
#include "Resolution.h"

using ROOT::Math::normal_pdf;

void Resolution::gaussianBlur(const TH1F &spectrum, const vector<Double_t> params, TH1F &blurred_spectrum){

	// Correction that is necessary because the integration does not use the full range
	// of the normal distribution which is ]-infinity, infinity[, but
	// [-GAUSSIAN_BLUR_WINDOW*SIGMA, GAUSSIAN_BLUR_WINDOW*SIGMA]
	// where SIGMA is the (energy-dependent) standard deviation of the normal
	// distribution.
	Double_t finite_width_correction = 1./(erf(GAUSSIAN_BLUR_WINDOW));
	Double_t inverse_BINNING = 1./ (Double_t) BINNING;

	Int_t max_bin = (Int_t) NBINS/((Int_t) BINNING);
	Int_t resolution = 1;
	Int_t blur_window_start = 1;
	Int_t blur_window_stop = max_bin;

	Double_t bin_content = 0.;
	
	for(Int_t i = 1; i <= max_bin; ++i){
		blurred_spectrum.SetBinContent(i, 0.);
	}

	for(Int_t i = 1; i <= max_bin; ++i){
		resolution = (Int_t) round(sqrt_resolution_model((Double_t) i*BINNING, params[0], params[1])*inverse_BINNING);
		blur_window_start = i - GAUSSIAN_BLUR_WINDOW*resolution;
		if(blur_window_start < 1){
			blur_window_start = 1;
		}
		blur_window_stop = i + GAUSSIAN_BLUR_WINDOW*resolution;
		if(blur_window_stop > max_bin){
			blur_window_stop = max_bin;
		}

		bin_content = 0.;
		for(Int_t j = blur_window_start; j <= blur_window_stop; ++j){
			bin_content += spectrum.GetBinContent(j)*normal_pdf((Double_t) i-j, resolution, 0.);
		}
		blurred_spectrum.SetBinContent(i, bin_content);
	}

	for(Int_t i = 1; i <= max_bin; ++i){
		blurred_spectrum.SetBinContent(i, blurred_spectrum.GetBinContent(i)*finite_width_correction);
	}
}

Double_t Resolution::sqrt_resolution_model(const Double_t energy, const Double_t p_constant,
		const Double_t p_square_root){
	return p_constant + p_square_root*sqrt(energy);
}
