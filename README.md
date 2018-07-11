# Horst
## HIGS original reconstruction spectrum tool

# Table of Contents

 1. [Introduction](#introduction)
 2. [Prerequisites](#prerequisites)
 3. [Installation](#installation)
 4. [Usage](#usage)

    4.1 [Horst](#usage_horst)

    4.2 [MakeMatrix](#usage_makematrix)

    4.3 [convert_to_txt](#usage_convert_to_txt)

 5. [License](#license)
 6. [References](#references)

## 1 Introduction <a name="introduction"></a>

In particle detection experiments, the real particle spectrum will distorted by the detector response function. `Horst` is a tool to reconstruct the original spectrum using a simulated detector response. It is designed to be applied to gamma-ray spectroscopy at the High-Intensity Gamma-Ray Source (HIGS) [[1]](#ref-higs), using response functions simulated with the Geant4 [[2]](#ref-g4_1) [[3]](#ref-g4_2) [[4]](#ref-g4_3) toolkit.

## 2 Prerequisites <a name="prerequisites"></a>

* C++11
* [ROOT 6](https://root.cern.ch/)

## 3 Installation <a name="installation"></a>

To install `Horst`, clone the repository by typing:

```
$ git clone https://github.com/uga-uga/Horst.git
```

After that enter the newly create `Horst/` directory and execute `make`:

```
$ cd Horst/
$ make
```

This should create three executables:

 * `horst`: The executable of the main program
 * `makematrix`: A tool to create a detector response matrix from a set of monoenergetic Geant4 simulations
 * `convert_to_txt`: A tool to convert the ROOT output file to text files

The Makefile also supports the commands `make clean` and `make debug`. The latter activates the `-g` compiler option, switches off optimization and prints more warnings (most of the warnings you will see are actually caused by ROOT, not by `Horst`).

## 4 Usage <a name="usage"></a>

### 4.1 Horst <a name="usage_horst"></a>

In order to use `Horst`, two things are needed:

 * `spectrum.txt`: An experimental spectrum from which the original spectrum should be reconstructed (single-column file, no text header)
 * `matrix.root`: A simulated detector response matrix (ROOT file with an `NBINSxNBINS` TH2F histogram)

Both the matrix and the spectrum need to have the same binning (examples: 1 keV per 1 bin). The spectrum should have at least as many bins as the number of rows/columns in the response matrix. All higher bins will simply be ignored.
For the reconstruction procedure, both can be rebinned to reduce the computing time which depends approximately exponentially on the number of bins.
The integer number of bins `NBINS` of the `NBINSxNBINS` matrix and the integer rebinning factor `BINNING` (`BINNING`==1 means no rebinning, `BINNING` = 2 means two bins are merged into one ...) can be hard-coded in `Horst/include/Config.h`, which will communicate the values to `Horst` and the auxiliary programs. After changing anything in `Config.h`, do not forget to `make` the code again.

The most simple usage of `Horst` is:

```
$ ./horst spectrum.txt -m matrix.root
```

Most probably, however, the user will at least want to restrict the energy range of the reconstruction. To give the left limit `E_LOW` and the right limit `E_UP` to `Horst`, type:

```
$ ./horst spectrum.txt -m matrix.root -l E_LOW -r E_UP
```

There are further options available to create interactive plots, set the name of the output file and make the program more verbose.

`Horst` will first try to reconstruct the spectrum using a simple TopDown algorithm. The result from the TopDown fit will be used as start parameters for a constrained fit of the original spectrum. After that, `Horst` creates a ROOT output file in `Horst/output/` which contains several TH1F histograms with `NBINS/BINNING` bins:

 * `spectrum`: The rebinned input spectrum
 * `spectrum_reconstructed`: The reconstructed original spectrum
 * `*_fit`: The fitted function
 * `*_params`: The fit parameters
 * `*_FEP`: The full-energy peaks of the fitted function (= the parameters multiplied with the corresponding full-energy peaks of the simulation)
 * `*_simulation_uncertainty`: The uncertainty which is caused by the statistical uncertainty of the Geant4 Monte-Carlo simulation
 * `*_fit_uncertainty`: The uncertainty of the fitting procedure determined by a Chi^2 test. Does not exist for TopDown, because the solution is unambiguous.
 * `*_statistical_uncertainty`: The uncertainty which comes from the statistics in the input spectrum.
 * `*_reconstruction_uncertainty`: The uncertainty of the reconstruction, which is a combination of all three uncertainties above.
 * `*_total_uncertainty`: The total uncertainty of the fit (Note that the fit and the reconstructed spectrum are different things)

### 4.2 MakeMatrix <a name="usage_makematrix"></a>

`MakeMatrix` creates a detector response matrix out of a set of simulated detector response files. The script needs two things:

 * `file*.root`: A set of ROOT files that contain the detector response as TH1F histograms. All of them must have the same binning and the histograms must have the same name `histname` in each ROOT file.
 * `input.txt`: A text file that indicates which file corresponds to which simulated energy, for example:

```
file0.root  3000
file1.root  3100
...
```

Each line contains one combination of file name and energy, separated by any whitespace.

To use `MakeMatrix`, type:

```
$ ./makematrix input.txt -n histname
```

The script will then go through the files and arrange them in an `NBINSxNBINS` matrix. If a simulation for a specific energy is missing, the closest simulated energy will be taken.

`MakeMatrix` also has an option to set the name of the output file.

### 4.3 convert_to_txt <a name="usage_convert_to_txt"></a>

This script converts all the TH1F histograms in the output file `output.root` to text files by typing:

```
$ ./convert_to_txt output.root
```

The text file names will be the name of the histogram plus the name of the ROOT file (without ".root") and a suffix. For each histogram, there will be a two-column (energy vs. counts) file and a single-column file (only counts). For the single-column files, a calibration file will be created that contains the parameters for a linear energy calibration `ENERGY = a + b*BIN` in the format

```
spectrum.tv: a b

```

## 5 License <a name="license"></a>

Copyright (C) 2018

U. Gayer (gayer.udo@gmail.com)

This code is distributed under the terms of the GNU General Public License. See the COPYING file for more information.

## 6 References <a name="references"></a>
<a name="ref-higs">[1]</a> H. R. Weller *et al.*, “Research opportunities at the upgraded HIγS facility”, Prog. Part. Nucl. Phys. **62.1**, 257 (2009). [`doi:10.1016/j.ppnp.2008.07.001`](https://doi.org/10.1016/j.ppnp.2008.07.001).
<a name="ref-g4_1">[2]</a> S. Agostinelli *et al.*, “GEANT4 - a simulation toolkit”, Nucl. Inst. Meth. A **506.3**, 250 (2003). [`doi:10.1016/S0168-9002(03)01368-8`](https://doi.org/10.1016/S0168-9002(03)01368-8).  
<a name="ref-g4_2">[3]</a> J. Allison *et al.*, “GEANT4 developments and applications”, IEEE Transactions on Nuclear Science, **53.1**, 270 (2006). [`doi:10.1109/TNS.2006.869826`](https://doi.org/10.1109/TNS.2006.869826).  
<a name="ref-g4_3">[4]</a> J. Allison *et al.*, “Recent developments in GEANT4”, Nucl. Inst. Meth. A **835**, 186 (2016). [`doi:10.1016/j.nima.2016.06.125`](https://doi.org/10.1016/j.nima.2016.06.125).  

