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
