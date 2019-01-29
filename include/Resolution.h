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

#ifndef RESOLUTION_H
#define RESOLUTION_H 1

#include<vector>

#include <TROOT.h>
#include <TH1.h>

using std::vector;

class Resolution{
public:
	Resolution(const UInt_t binning): BINNING(binning){};
	~Resolution(){};

	void gaussianBlur(const TH1F &spectrum, const vector<Double_t> params, TH1F &blurred_spectrum); 

private:
	Double_t sqrt_resolution_model(const Double_t energy, const Double_t p_constant, const Double_t p_square_root);
	const UInt_t BINNING;
};

#endif
