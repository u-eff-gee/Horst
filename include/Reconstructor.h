#ifndef RECONSTRUCTOR_H
#define RECONSTRUCTOR_H 1

#include <TROOT.h>
#include <TH1.h>
#include <TH2.h>

class Reconstructor{
public:
	Reconstructor(){};
	~Reconstructor(){};

	void reconstruct(const TH1F &params, const TH2F &rema, TH1F &reconstructed_spectrum);
	void uncertainty(const TH1F &total_uncertainty, const TH2F &rema, TH1F &reconstruction_uncertainty);
};

#endif
