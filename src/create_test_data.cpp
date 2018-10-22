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

#include <iostream>

#include <TROOT.h>

#include "ConfigTest.h"
#include "ResponseMatrixCreator.h"
#include "SpectrumCreator.h"

using std::cout;
using std::endl;

int main(int argc, char* argv[]){

	if(argc == 1 || argc > 4){
		cout << "Error: create_test_data.cpp: main(): Function takes exactly three arguments which define the models for the spectrum and the response function, and the name prefix of the output files. Aborting ..." << endl;
		abort();
	}

	TH1F spectrum("spectrum", "Spectrum", NBINS, 0., (Double_t) NBINS - 1.);
	TH1F n_simulated_particles("n_simulated_particles", "Initial simulated particles", NBINS, 0., (Double_t) (NBINS - 1));
	TH2F response_matrix("rema", "Response_Matrix", NBINS, 0., (Double_t) (NBINS - 1), NBINS, 0., (Double_t) (NBINS - 1));
	
	cout << "Creating test spectrum with option '" << argv[1] << "' ..." << endl;
	SpectrumCreator spectrumCreator;
	spectrumCreator.createSpectrum(spectrum, argv[1], argv[3]);

	cout << "Creating test reponse matrix with option '" << argv[2] << "' ..." << endl;
	ResponseMatrixCreator responseMatrixCreator;
	responseMatrixCreator.createResponseMatrix(response_matrix, n_simulated_particles, argv[2], argv[3]);

}
