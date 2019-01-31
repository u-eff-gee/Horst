include_directories("../include/")
add_library(horst_lib FitFunction.cpp MonteCarloUncertainty.cpp Uncertainty.cpp Fitter.cpp InputFileReader.cpp Reconstructor.cpp)
add_library(tsroh_lib FitFunction.cpp Fitter.cpp InputFileReader.cpp Reconstructor.cpp Resolution.cpp)
add_library(makematrix_lib InputFileReader.cpp)
add_library(create_test_data_lib InputFileReader.cpp ResponseMatrixCreator.cpp SpectrumCreator.cpp)

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT REQUIRED)
include(${ROOT_USE_FILE})
