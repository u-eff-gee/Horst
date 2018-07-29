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

#ifndef RECONSTRUCTOR_H
#define RECONSTRUCTOR_H 1

#include <TROOT.h>
#include <TH1.h>
#include <TH2.h>

class Reconstructor{
public:
	Reconstructor(const UInt_t binning): BINNING(binning){};
	~Reconstructor(){};

	void reconstruct(const TH1F &params, const TH1F &n_simulated_particles, TH1F &reconstructed_spectrum);
	void uncertainty(const TH1F &total_uncertainty, const TH2F &rema, const TH1F &n_simulated_particles, TH1F &reconstruction_uncertainty);

	void addResponse(const TH1F &spectrum, const TH1F &inverse_n_simulated_particles, const TH2F &rema, TH1F &response_spectrum);
	void addResponse(const TH1F &spectrum, const TH1F &inverse_n_simulated_particles, const TH2F &rema, TH1F &response_spectrum, TH1F &response_spectrum_FEP);

	void addRealisticResponse(const TH1F &spectrum, const TH1F &inverse_n_simulated_particles, const TH2F &rema, TH1F &response_spectrum);
	void addRealisticResponse(const TH1F &spectrum, const TH1F &inverse_n_simulated_particles, const TH2F &rema, TH1F &response_spectrum, TH1F &response_spectrum_FEP);

private:
	const UInt_t BINNING;
};

#endif
