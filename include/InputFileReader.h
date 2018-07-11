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
	InputFileReader(){};
	~InputFileReader(){};

	void readInputFile(const TString inputfilename, vector<TString> &filenames, vector<Double_t> &energies);

	void fillMatrix(const vector<TString> &filenames, const vector<Double_t> &energies, const TString histname, TH2F &response_matrix);

	void writeMatrix(TH2F &response_matrix, TString outputfilename) const;
	void readMatrix(TH2F &response_matrix, const TString matrixfile);

	void readTxtSpectrum(TH1F &spectrum, const TString spectrumfile);
};

#endif
