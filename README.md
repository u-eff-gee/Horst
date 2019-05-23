# horst
## histogram original reconstruction spectrum tool

# Table of Contents

 1. [Introduction](#introduction)
 2. [Prerequisites](#prerequisites)
 3. [Installation](#installation)

    3.1 [Testing](#testing)

    3.2 [Documentation](#documentation)

 4. [Usage](#usage)

    4.1 [Horst](#usage_horst)

    4.2 [Tsroh](#usage_tsroh)

    4.3 [MakeMatrix](#usage_makematrix)

    4.4 [convert_to_txt](#usage_convert_to_txt)

 5. [Output](#output)
 6. [License](#license)
 7. [References](#references)

## 1 Introduction <a name="introduction"></a>

In particle detection experiments, the real particle spectrum will be distorted by the detector response function. `Horst` is a tool to reconstruct the original spectrum using a simulated detector response. It was originally designed to be applied to gamma-ray spectroscopy at the High-Intensity Gamma-Ray Source (HIGS) [[1]](#ref-higs), using response functions simulated with the Geant4 [[2]](#ref-g4_1) [[3]](#ref-g4_2) [[4]](#ref-g4_3) toolkit.

## 2 Prerequisites <a name="prerequisites"></a>

* [CMake](https://cmake.org/) (> 3.9)
* C++11 (minimum)
* [ROOT 6](https://root.cern.ch/)
* LaTeX (to build the documentation)

## 3 Installation <a name="installation"></a>

To install `Horst` and all auxiliary programs, clone the repository by typing:

```
$ git clone https://github.com/uga-uga/horst.git
```

This will create a `horst/` directory containing the source code. Its path will be referred to as `HORST_SOURCE_DIR` in the following.
After that, create a build directory and build `horst` there using `cmake`:

```
$ mkdir horst-build
$ cd horst-build/
$ cmake HORST_SOURCE_DIR
```

The procedure above is the default build. To change the build type (`RELEASE`, `DEBUG` and `RELEASEWITHDEBUGINFO` are supported) or change the installation directory, use the `-DCMAKE_BUILD_TYPE` and `-DCMAKE_INSTALL_PREFIX` build variables, for example:

```
$ cmake -DCMAKE_BUILD_TYPE=DEBUG -DCMAKE_INSTALL_PREFIX=/opt/ HORST_SOURCE_DIR
```

To change the maximum number of bins `NBINS` of the input spectra and matrices, set the variable `N_BINS` to a different value:

```
$ cmake -DN_BINS=NBINS
```

The default value is 12000. This value should match the number of bins of the response matrix to ensure correct reading. Furthermore, it influences the performance of the tests, since the test parameters are all defined relative to `NBINS`. Note that this option is different from the  `-b` command-line option (see also [4 Usage](#usage)).
After that, compile and install the code in the build directory by executing

```
$ make
$ make install
```

The `make` step should create five executable binaries:

 * `horst`: The executable of the main program
 * `tsroh`: A tool that does the opposite of `Horst`, distorting a spectrum with a simulated detector response
 * `makematrix`: A tool to create a detector response matrix from a set of monoenergetic Geant4 simulations
 * `convert_to_txt`: A tool to convert the ROOT output file to text files
 * `create_test_data`: A driver to generate artificial spectra and response matrices for unit testing

The Makefile also supports the command `make clean` to remove all files which were created in the compilation step.

### 3.1 Testing <a name="testing"></a>

After completing the steps above, a self-test of the code can be run by executing

```
make test
```

in the build directory. This will run `horst` with selected examples, trying to use all of its functionality. The tests will report on their success or failure and create the usual `horst` output.
Note that successful tests only mean that the code runs without runtime errors. **They do not guarantee that the physics is correct!** In order to check the physics, compare the output:
Each test starts by creating an artificial spectrum and response matrix. After that, `tsroh` is used to distort the spectrum with a detector response. At the end, `horst` is used to recreate the original spectrum. Compare the input/output of all three steps to see whether, and if, how well, the spectrum reconstruction works.
User-defined artificial response functions and spectra can be hard-coded as member functions of the corresponding classes `SpectrumCreator` and `ResponseMatrixCreator`.

### 3.2 Documentation <a name="documentation"></a>

`Horst` includes a documentation file, which describes the basics of how the reconstruction procedure is implemented and what assumptions go into it. It also includes detailed descriptions of the command-line options and the output file. It can be built by going to the `doc/` directory and executing `make`:

```
$ cd horst/doc/
$ make
```

This Makefile contains a `make clean` option as well.

## 4 Usage <a name="usage"></a>

Detector response matrices for different collimator diameters and offsets of the zero-degree detector at HIGS from the beam axis can be found at [the IKP cloud](https://ikpcloud.ikp.physik.tu-darmstadt.de/index.php/s/WuoQzKVqZyYjjHI).

### 4.1 Horst <a name="usage_horst"></a>

In order to use `Horst`, two things are needed (the files can have arbitrary name, `spectrum.txt` and `matrix.root` are just for reference in this README):

 * `spectrum.txt`: An experimental spectrum with `NBINS` bins from which the original spectrum should be reconstructed (single-column file, no text header OR a ROOT file containing a TH1F histogram with the name 'SPECTRUM', using the `-t SPECTRUM` option)
 * `matrix.root`: A simulated detector response matrix (ROOT file with an `NBINSxNBINS` TH2F histogram called 'rema' and a TH1F histogram called 'n_simulated_particles' containing the response matrix, and the number of simulated primary particles, respectively)

The spectrum and the response matrix need to have the same binning (example: 1 bin corresponds to an energy range of 1 keV). During runtime, they can be rebinned simultaneously with the factor given by the `-b` command line option. For the reconstruction procedure, rebinning is an important measure to reduce the computing time which depends approximately exponentially on the number of bins.
The rebinning factor `BINNING` (`BINNING`==1 means no rebinning, `BINNING` = 2 means two bins are merged into one ...) can be set via the `-b BINNING` command-line option. By default, `BINNING` == 10 is used in `Horst`.

The most simple usage of `Horst` is:

```
$ horst spectrum.txt -m matrix.root
```

Most probably, however, the user will at least want to restrict the energy range of the reconstruction. To give the left limit `E_LOW` and the right limit `E_UP` to `Horst`, type:

```
$ horst spectrum.txt -m matrix.root -l E_LOW -r E_UP
```

There are more options available that:

 * change the binning factor
 * enable a better uncertainty estimate using a Monte-Carlo method
 * create interactive plots
 * set the name of the output file
 * write the correlation matrix of the fit
 * change the verbosity of `horst`

To see a short description of the options, type

```
$ horst --help
```

or refer to the command line option reference in the [documentation](#documentation).

`Horst` will first try to reconstruct the original spectrum using a simple TopDown algorithm. The result of the TopDown fit will be used as start parameters for a constrained fit of the original spectrum. After that, `Horst` creates a ROOT output file `OUTPUTFILE` which contains a lot TH1F histograms. For more information about the output, see the [Output](#output) section.

### 4.2 Tsroh <a name="usage_tsroh"></a>

`Tsroh`, *Transfer spectroscopic response on histogram* is `Horst` in reverse. The program has the same requirements for the input as `Horst`, but it takes the spectrum and applies the simulated detector response to it using the TopDown algorithm.

To see a short description of the command-line options of `tsroh`, type

```
$ tsroh --help
```
or refer to the command line option reference in the [documentation](#documentation).

Similar to `horst`, `tsroh` also create a ROOT output file which contains several TH1F histograms.

### 4.2 MakeMatrix <a name="usage_makematrix"></a>

`MakeMatrix` creates a detector response matrix with the name `MATRIXFILE` out of a set of simulated detector response files. The script needs two things:

 * `file*.root`: A set of ROOT files that contain the detector response to monoenergetic particles as TH1F histograms. All of them must have the same binning and the histograms must have the same name `HISTNAME` in each ROOT file.
 * `input.txt`: A text file that indicates which file corresponds to which simulated energy, and how many particles were simulated for each file, for example:

```
file0.root  3000  1e8
file1.root  3100  1e8
...
```

Each line contains one combination of file name, energy and particle number, separated by any whitespace.

To use `MakeMatrix`, type:

```
$ makematrix input.txt -n HISTNAME -o MATRIXFILE
```

In the example above, the `-o` command line option was used to set the name of the output file.
The script will then go through the files and arrange them in an `NBINSxNBINS` matrix. If a simulation for a specific energy is missing, the closest simulated energy will be taken. The closest simulation will be shifted to match the desired energy. The output file will contain the response matrix as a `TH2F` histogram `rema` and a `TH1F` histogram `n_simulated_particles` which indicates the number of particles simulated for each energy.

`MakeMatrix` can also add new simulations to an existing 'old' response matrix file using the `-u` option. For this, the following input is needed:

  * `old_MATRIXFILE`: The old response matrix file which should be updated. `MATRIXFILE` will be the name set by the `-o` command line option, and the old file **must** be called `old_MATRIXFILE`.
 * `old_input.txt`: The input file that was used to create the old response matrix. This is needed to distinguish between rows in the matrix which were 'true' simulation output and others which were simply filled up by shifting the simulations.
 * `input.txt`: The new input file, following the same convention as described above.
 * `file*.root`: The new simulations, following the same convention as described above.

The new response matrix file `MATRIXFILE` will contain a combination of old and new simulations. To add even more simulations to this one, combine `old_input.txt` and `input.txt` and use this as the 'new' `old_input.txt`.

A valid call of `MakeMatrix` to update an existing response matrix would look like:

```
$ makematrix input.txt -n HISTNAME -o MATRIXFILE -u old_input.txt
```

### 4.3 convert_to_txt <a name="usage_convert_to_txt"></a>

This convenience script converts all the TH1F histograms in an output file `OUTPUTFILE` to text files by typing:

```
$ convert_to_txt OUTPUTFILE BINNING
```

Here, `BINNING` is the binning factor of the histogram introduced in [4.1 Horst](#usage_horst). The text file names will be the name of the histogram plus the name of the ROOT file (without ".root") and a suffix. For each histogram, there will be a two-column (energy vs. counts) file and a single-column file (only counts). For the single-column files, a calibration file will be created that contains the parameters for a linear energy calibration `ENERGY = a + b*BIN` in the format

```
spectrum1.tv: a1 b1
spectrum2.tv: a2 b2
...
```
This is why the binning factor is needed.

## 5 Output <a name="output"></a>

`horst` creates an output file that contains TH1F histograms with `NBINS/BINNING` bins which can be accessed by their name (`TH1F::GetName()`). First of all, it contains the possibly rebinned original spectrum `spectrum`.

Besides the spectrum, the top-level directory also contains the reconstructed spectrum with an uncertainty estimate. If the Monte-Carlo uncertainty estimation was used, it will both contain the reconstructed spectrum from a single fit and the average of all reconstructed spectra from the Monte-Carlo method.

More output can be found in up to three subdirectories, which correspond to the three reconstruction steps that `horst` can do:

 * `topdown`: Output from the TopDown fit
 * `fit`: Output from a single fit using Gaussian uncertainty estimation
 * `monte_carlo`: Output from several fits of Monte-Carlo generated spectra. If the `-w` command line option was used, every single Monte-Carlo realization is stored. If not, only the average.

For each reconstruction step, the output is similar. Different reconstruction steps can be distinguished by the following key words:

 * `*reconstruct*`: The spectrum recorded by a perfect detector with 100% efficiency.
 * `*FEP*`: The spectrum recorded by a detector with an efficiency of less than 100%, but perfect energy reconstruction (i.e. if a particle is detected, its complete energy is be recorded).
 * `*params*`: Similar to `*reconstruct*`, but normalized to the number of simulated primary particles (these are the actual fit parameters).

For the categories above, uncertainty estimates can be given. `horst` decomposes the uncertainty estimates into the contributions by different sources of uncertainty. Depending on whether it makes sense to give that uncertainty in a specific case, the following are possible:

 * `*algorithm_uncertainty*`: The uncertainty from the minimization algorithm
 * `*spectrum_uncertainty*`: The influence of the statistical uncertainty of the input spectrum
 * `*simulation_uncertainty*`: The influence of the statistical uncertainty of the simulated response
 * `*total_uncertainty*`: The total uncertainty

For more information about the output, refer to the [documentation](#documentation)

## 6 License <a name="license"></a>

Copyright (C) 2018

U. Gayer (gayer.udo@gmail.com)

This code is distributed under the terms of the GNU General Public License. See the COPYING file for more information.

## 7 References <a name="references"></a>

<a name="ref-higs">[1]</a> H. R. Weller *et al.*, “Research opportunities at the upgraded HIγS facility”, Prog. Part. Nucl. Phys. **62.1**, 257 (2009). [`doi:10.1016/j.ppnp.2008.07.001`](https://doi.org/10.1016/j.ppnp.2008.07.001).

<a name="ref-g4_1">[2]</a> S. Agostinelli *et al.*, “GEANT4 - a simulation toolkit”, Nucl. Inst. Meth. A **506.3**, 250 (2003). [`doi:10.1016/S0168-9002(03)01368-8`](https://doi.org/10.1016/S0168-9002(03)01368-8).  

<a name="ref-g4_2">[3]</a> J. Allison *et al.*, “GEANT4 developments and applications”, IEEE Transactions on Nuclear Science, **53.1**, 270 (2006). [`doi:10.1109/TNS.2006.869826`](https://doi.org/10.1109/TNS.2006.869826).  

<a name="ref-g4_3">[4]</a> J. Allison *et al.*, “Recent developments in GEANT4”, Nucl. Inst. Meth. A **835**, 186 (2016). [`doi:10.1016/j.nima.2016.06.125`](https://doi.org/10.1016/j.nima.2016.06.125).  

