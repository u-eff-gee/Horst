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

#ifndef RESPONSEMATRIXCREATOR_H
#define RESPONSEMATRIXCREATOR_H 1

#include <string>

#include "TH1.h"
#include "TH2.h"

using std::string;

class ResponseMatrixCreator{
	
public:
	ResponseMatrixCreator(){}
	~ResponseMatrixCreator(){}

	void createResponseMatrix(TH2F &response_matrix, TH1F &n_simulated_particles, const string option, const string outputfile_prefix);

	void createResponseMatrixWithEscapePeaks(TH2F &response_matrix, TH1F & n_simulated_particles, const vector<Double_t> params);

	void createResponseMatrixWithEfficiency(TH2F &response_matrix, TH1F &n_simulated_particles, vector<Double_t> params);
};

#endif
