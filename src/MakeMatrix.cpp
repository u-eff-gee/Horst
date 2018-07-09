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
	TString histname = "";
	TString outputfile = "";
};

static char doc[] = "makematrix, Create a response matrix from a series of simulations of the detector response";
static char args_doc[] = "INPUTFILENAME";

static struct argp_option options[] = {
	{"histname", 'n', "HISTNAME", 0, "Name of histogram for detector response", 0},
	{"outputfile", 'o', "OUTPUTFILENAME", 0, "Name of output file", 0},
	{ 0, 0, 0, 0, 0, 0}
};

static int parse_opt(int key, char *arg, struct argp_state *state){
	struct Arguments *arguments = (struct Arguments*) state->input;

	switch (key){
		case ARGP_KEY_ARG: arguments->inputfile = arg; break;
		case 'o': arguments->outputfile = arg; break;
		case 'n': arguments->histname= arg; break;
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

	InputFileReader inputFileReader;
	inputFileReader.readInputFile(arguments.inputfile, filenames, energies);

	TH2F response_matrix("rema", "Response_Matrix", NBINS, 0., (Double_t) (NBINS - 1), NBINS, 0., (Double_t) (NBINS - 1));
	inputFileReader.fillMatrix(filenames, energies, arguments.histname, response_matrix);
	inputFileReader.writeMatrix(response_matrix, arguments.outputfile);

}
