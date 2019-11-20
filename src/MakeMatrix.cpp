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

#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <argp.h>

#include "Config.h"
#include "InputFileReader.h"

using std::cout;
using std::endl;
using std::vector;
using std::stringstream;

struct Arguments{
	TString inputfile = "";
	TString old_inputfile = "";
	TString histname = "hpge0";
	TString outputfile = "output.root";

	Bool_t update = false;
};

static char doc[] = "makematrix, Create a response matrix from a series of simulations of the detector response";
static char args_doc[] = "INPUTFILENAME";

static struct argp_option options[] = {
	{"histname", 'n', "HISTNAME", 0, "Name of histogram for detector response (default: 'hpge0')", 0},
	{"outputfile", 'o', "OUTPUTFILENAME", 0, "Name of output file (default: 'output.root')", 0},
	{"old_inputfile", 'u', "OLD_INPUTFILENAME", 0, "Add new response simulations to an existing matrix. The previous input file must be given as a reference, so that 'makematrix' knows how to add the new simulations.", 0},
	{ 0, 0, 0, 0, 0, 0 }
};

static int parse_opt(int key, char *arg, struct argp_state *state){
	struct Arguments *arguments = (struct Arguments*) state->input;

	switch (key){
		case ARGP_KEY_ARG: arguments->inputfile = arg; break;
		case 'o': arguments->outputfile = arg; break;
		case 'n': arguments->histname= arg; break;
		case 'u': arguments->update=true; arguments->old_inputfile=arg; break;
		case ARGP_KEY_END:
			if(state->arg_num == 0){
				argp_usage(state);
			}
			break;
		default: return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0};

int main(int argc, char* argv[]){

	Arguments arguments;
	argp_parse(&argp, argc, argv, 0, 0, &arguments);

	vector<TString> filenames;
	vector<Double_t> energies;
	vector<Double_t> n_simulated_particles;
	
	vector<TString> old_filenames;
	vector<Double_t> old_energies;
	vector<Double_t> old_n_simulated_particles;

	// Best approach would be to copy the energy calibration (i.e. first bin center and bin width) of the supplied response 
	// spectra (just take the first one for example assuming that all are equal, otherwise it would be havoc anyway) to the
	// response matrix. I.e. in TH2F constructor use xlow=ylow=hist->GetBinLowEdge(1) 
	// and xup=yup=hist->GetBinLowEdge(1)+hist->GetBinWidth(0)*NBINS with hist being a response spectrum.
	// Or let the user set this via a CMake switch or command line option.
	// Ideally, the energy calibration should be equal for all histograms (the user has to guarantee that input matrix and 
	// spectrum are binned equally), while for HORST the energy calibration does not matter (only bin numbers are relevant)
	// this produces nicer (i.e. energy calibrated) output.
	TH2F response_matrix("rema", "Response_Matrix", NBINS, 0., (Double_t) NBINS, NBINS, 0., (Double_t) NBINS);
	TH2F old_response_matrix("old_rema", "Response_Matrix", NBINS, 0., (Double_t) NBINS, NBINS, 0., (Double_t) NBINS);
	TH1F n_particles("n_simulated_particles", "Initial simulated particles", NBINS, 0., (Double_t) NBINS);

	InputFileReader inputFileReader(1);

	if(arguments.update){
		stringstream old_matrixfile_name;
		old_matrixfile_name << "old_" << arguments.outputfile;
		inputFileReader.readMatrix(old_response_matrix, old_matrixfile_name.str());

		inputFileReader.readInputFile(arguments.old_inputfile, old_filenames, old_energies, old_n_simulated_particles);
		inputFileReader.readInputFile(arguments.inputfile, filenames, energies, n_simulated_particles);

		inputFileReader.updateMatrix(old_filenames, old_energies, old_n_simulated_particles, old_response_matrix, filenames, energies, n_simulated_particles, arguments.histname, response_matrix, n_particles);
		inputFileReader.writeMatrix(response_matrix, n_particles, arguments.outputfile);

	} else{
		inputFileReader.readInputFile(arguments.inputfile, filenames, energies, n_simulated_particles);
		inputFileReader.fillMatrix(filenames, energies, n_simulated_particles, arguments.histname, response_matrix, n_particles);
		inputFileReader.writeMatrix(response_matrix, n_particles, arguments.outputfile);
	}
}
