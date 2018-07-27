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

#include "Config.h"
#include "Reconstructor.h"

void Reconstructor::reconstruct(const TH1F &params, const TH1F &n_simulated_particles, TH1F &reconstructed_spectrum){

	for(Int_t i = 1; i <= (Int_t) NBINS/((Int_t) BINNING); ++i){
		reconstructed_spectrum.SetBinContent(i, params.GetBinContent(i)*n_simulated_particles.GetBinContent(i));
	}
}

void Reconstructor::uncertainty(const TH1F &total_uncertainty, const TH2F &rema, const TH1F &n_simulated_particles, TH1F &reconstruction_uncertainty){

	for(Int_t i = 1; i <= (Int_t) NBINS/((Int_t) BINNING); ++i){
		reconstruction_uncertainty.SetBinContent(i, total_uncertainty.GetBinContent(i)*n_simulated_particles.GetBinContent(i)/rema.GetBinContent(i, i));
	}
}
