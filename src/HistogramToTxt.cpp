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

#include <TROOT.h>
#include <TH1.h>
#include <TFile.h>

#include <iostream>
#include <fstream>
#include <sstream>

#include "Config.h"

using namespace std;

void convertDirectory(TDirectory* f, TString filename,
        TString calibrationfilename, TString prefix,
        UInt_t BINNING) {
	ofstream of;
	stringstream outputfilename;
    stringstream prefixstream;
	TH1* hist;
    f->cd();
	Int_t nkeys = f->GetNkeys();
	for(Int_t i = 0; i < nkeys; i++){
        if (f->GetListOfKeys()->At(i)->IsFolder()) {
            TDirectory* dir = (TDirectory*)f->GetDirectory(
                f->GetListOfKeys()->At(i)->GetName());
            prefixstream << prefix << f->GetListOfKeys()->At(i)->GetName() << "_";
            convertDirectory(dir,
                    filename, calibrationfilename, (TString)prefixstream.str(), BINNING);
            f->cd();
            prefixstream.str("");
            continue;
        }
		TString histogramname = f->GetListOfKeys()->At(i)->GetName();

		cout << "Converting histogram " << histogramname << " ..." << endl;

		// Write two-column spectrum
		outputfilename << prefix << histogramname << "_" << filename(0, filename.Length() - 5) << ".txt"; 
		of.open(outputfilename.str().c_str());
	
		hist = (TH1*) f->Get(histogramname);

		for(Int_t j = 0; j <= (Int_t) NBINS/ (Int_t) BINNING; j++){
			of << hist->GetBinCenter(j) << "\t" << hist->GetBinContent(j) << endl;
		}

		of.close();
		cout << "Output file " << outputfilename.str() << " created." << endl;
		outputfilename.str("");

		// Write single-column spectrum with calibration file
		outputfilename << prefix << histogramname << "_" << filename(0, filename.Length() - 5) << ".tv"; 
		of.open(outputfilename.str().c_str());
	
		hist = (TH1*) f->Get(histogramname);

		for(Int_t j = 0; j <= (Int_t) NBINS/ (Int_t) BINNING; j++){
			of << hist->GetBinContent(j) << endl;
		}

		of.close();
		cout << "Output file " << outputfilename.str() << " created." << endl;

		if(i == 0) {
		} else {
        of.open(calibrationfilename, std::ios_base::app);
		}
		of << outputfilename.str() << ":\t" << 0. << "\t" << BINNING << endl;
		of.close();

		cout << "Calibration file " << calibrationfilename << " created." << endl;

		outputfilename.str("");
	}
}

int main(int argc, char* argv[]){

	if(argc != 3){
		cout << "Error: Script needs exactily two arguments. The first is the name of the ROOT file that contains the histograms. The second is the binning factor of the histograms. Aborting ..." << endl;
		abort();
	}
	TString filename = argv[1];
	UInt_t BINNING = (UInt_t) atoi(argv[2]);

	// Open TFile
	TFile *f = new TFile(filename);

	Int_t nkeys = f->GetNkeys();
		
	if(nkeys < 1){
		cout << "Error: File does not contain any object to read from." << endl;
		return 0;
	}

	// Loop over all keys (hopefully all of them are TH1 histograms) and write their content to separate output files
	stringstream calibrationfilename;
	calibrationfilename << filename(0, filename.Length() - 5) << ".cal";

    ofstream of;
    of.open(calibrationfilename.str().c_str(), std::ios::out);
    of.close();

    convertDirectory(f, filename, calibrationfilename.str(), (TString)"", BINNING);

	return 0;
}
