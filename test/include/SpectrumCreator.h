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

#ifndef SPECTRUMCREATOR_H
#define SPECTRUMCREATOR_H 1

#include <string>
#include <vector>

#include "TH1.h"
#include "TH2.h"

using std::string;
using std::vector;

class SpectrumCreator{
	
public:
	SpectrumCreator(){}
	~SpectrumCreator(){}

	void createSpectrum(TH1F &spectrum, const string option);

	void createBarSpectrum(TH1F &spectrum, const vector<Double_t> &params);
	void createNormalSpectrum(TH1F &spectrum, const vector<Double_t> &params);
};

#endif
