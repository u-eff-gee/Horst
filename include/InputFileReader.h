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

#ifndef INPUTFILEREADER_H
#define INPUTFILEREADER_H 1

#include <TROOT.h>
#include <TH2.h>

#include <vector>

using std::vector;

class InputFileReader{
public:
	InputFileReader(const UInt_t binning):BINNING(binning){};
	~InputFileReader(){};

	void readInputFile(const TString inputfilename, vector<TString> &filenames, vector<Double_t> &energies, vector<Double_t> &n_simulated_particles);

	void fillMatrix(const vector<TString> &filenames, const vector<Double_t> &energies, const vector<Double_t> &n_particles, const TString histname, TH2F &response_matrix, TH1F &n_simulated_particles);

	void writeMatrix(TH2F &response_matrix, TH1F &n_simulated_particles, TString outputfilename) const;
	void readMatrix(TH2F &response_matrix, TH1F &n_simulated_particles, const TString matrixfile);

	void readTxtSpectrum(TH1F &spectrum, const TString spectrumfile);
	
	void readROOTSpectrum(TH1F &spectrum, const TString spectrumfile, const TString spectrumname);

private:
	const UInt_t BINNING;
};

#endif
