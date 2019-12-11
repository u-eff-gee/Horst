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

#include <TH2.h>
#include <TMatrixDSym.h>
#include <TROOT.h>

#include <vector>

using std::vector;

class InputFileReader{
public:
	InputFileReader():BINNING(1){};
	InputFileReader(const UInt_t binning):BINNING(binning){};
	~InputFileReader(){};

	void readInputFile(const TString inputfilename, vector<TString> &filenames, vector<Double_t> &energies, vector<Double_t> &n_simulated_particles);

	void fillMatrix(const vector<TString> &filenames, const vector<Double_t> &energies, const vector<Double_t> &n_particles, const TString histname, TH2F &response_matrix, TH1F &n_simulated_particles);
	void fillMatrixWeighted(const vector<TString> &filenames, const vector<Double_t> &energies, const vector<Double_t> &n_particles, const TString histname, TH2F &response_matrix, TH1F &n_simulated_particles, Int_t i, Int_t simulation, Double_t weight);
	void updateMatrix(const vector<TString> &old_filenames, const vector<Double_t> &old_energies, const vector<Double_t> &old_n_particles, const TH2F &old_response_matrix, const vector<TString> &new_filenames, const vector<Double_t> &new_energies, const vector<Double_t> &new_n_particles, const TString histname, TH2F &response_matrix, TH1F &n_simulated_particles);

	void writeCorrelationMatrix(TMatrixDSym &correlation_matrix, TString outputfilename) const;
	void writeMatrix(TH2F &response_matrix, TH1F &n_simulated_particles, TString outputfilename) const;
	void readMatrix(TH2F &response_matrix, TH1F &n_simulated_particles, const TString matrixfile);
	// Alternative version of readMatrix() which does not read n_simulated_particles
	void readMatrix(TH2F &response_matrix, const TString matrixfile);

	void readTxtSpectrum(TH1F &spectrum, const TString spectrumfile);
	
	void readROOTSpectrum(TH1F &spectrum, const TString spectrumfile, const TString spectrumname);

	void readDoubleParameters(vector<Double_t> &params, const TString inputfilename);
	void readUnsignedIntParameters(vector<UInt_t> &params, const TString inputfilename);

	// Having a template function that can process either a UInt_t or a Double_t
	// would be nicer, but this caused the cmake linking to fail.
	// For now, circumvented this problem by overloading InputFileReader::writeParameters()
	void writeParameters(const vector<UInt_t> &params, TString outputfilename) const;
	void writeParameters(const vector<Double_t> &params, const TString outputfilename) const ;

private:
	const UInt_t BINNING;
};

#endif
