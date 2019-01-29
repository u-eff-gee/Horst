cmake_minimum_required (VERSION 3.9 FATAL_ERROR)
project (horst)

add_subdirectory("src/")
include_directories("include/")

# horst executable
add_executable(horst src/horst.cpp)
target_link_libraries(horst horst_lib)

# tsroh executable
add_executable(tsroh src/tsroh.cpp)
target_link_libraries(tsroh tsroh_lib)

# makematrix executable
add_executable(makematrix src/MakeMatrix.cpp)
target_link_libraries(makematrix makematrix_lib)

# convert_to_txt executable
add_executable(convert_to_txt src/HistogramToTxt.cpp)

# Test executable
add_executable(create_test_data src/create_test_data.cpp)
target_link_libraries(create_test_data test_lib)

# Different compile options
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra -Wconversion -Wsign-conversion")
set(CMAKE_CXX_FLAGS_RELEASE "-O3") # -ftree_vectorize and -march=native had no effect
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g ${CMAKE_CXX_FLAGS_RELEASE}")

# Compile options
set(N_BINS 12000 CACHE STRING "Set the number of bins (default : 12000)")

configure_file(
	"${PROJECT_SOURCE_DIR}/include/Config.h.in"
	"${PROJECT_SOURCE_DIR}/include/Config.h"
)
configure_file(
	"${PROJECT_SOURCE_DIR}/include/ConfigTest.h.in"
	"${PROJECT_SOURCE_DIR}/include/ConfigTest.h"
)

# ROOT libraries
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT REQUIRED)
include(${ROOT_USE_FILE})
message(STATUS "Using ROOT version ${ROOT_VERSION}")
target_link_libraries(horst ${ROOT_LIBRARIES})
target_link_libraries(tsroh ${ROOT_LIBRARIES})
target_link_libraries(makematrix ${ROOT_LIBRARIES})
target_link_libraries(convert_to_txt ${ROOT_LIBRARIES})
target_link_libraries(create_test_data ${ROOT_LIBRARIES})

# Installing
install(TARGETS horst tsroh makematrix convert_to_txt DESTINATION bin)

# Testing
include(CTest)
add_test(test_bar_escape create_test_data bar escape bar_escape)
add_test(test_tsroh_bar_escape tsroh bar_escape_spectrum.root -m bar_escape_response_matrix.root -b 1 -t spectrum -o tsroh_bar_escape.root)
add_test(test_horst_bar_escape horst tsroh_bar_escape.root -m bar_escape_response_matrix.root -b 10 -L test/bar_escape_limits.txt -t response_spectrum -o horst_bar_escape.root)

add_test(test_normal_escape create_test_data normal escape normal_escape)
add_test(test_tsroh_normal_escape tsroh normal_escape_spectrum.root -m normal_escape_response_matrix.root -b 1 -t spectrum -o tsroh_normal_escape.root)
add_test(test_horst_normal_escape horst tsroh_normal_escape.root -m normal_escape_response_matrix.root -b 10 -L test/normal_escape_limits.txt -t response_spectrum -o horst_normal_escape.root)

add_test(test_bar_efficiency create_test_data bar efficiency bar_efficiency)
add_test(test_tsroh_bar_efficiency tsroh bar_efficiency_spectrum.root -m bar_efficiency_response_matrix.root -b 1 -t spectrum -o tsroh_bar_efficiency.root)
add_test(test_horst_bar_efficiency horst tsroh_bar_efficiency.root -m bar_efficiency_response_matrix.root -b 10 -L test/bar_efficiency_limits.txt -t response_spectrum -o horst_bar_efficiency.root)

add_test(test_normal_efficiency create_test_data normal efficiency normal_efficiency)
add_test(test_tsroh_normal_efficiency tsroh normal_efficiency_spectrum.root -m normal_efficiency_response_matrix.root -b 1 -t spectrum -o tsroh_normal_efficiency.root)
add_test(test_horst_normal_efficiency_mc horst tsroh_normal_efficiency.root -m normal_efficiency_response_matrix.root -b 10 -L test/normal_efficiency_limits.txt -t response_spectrum -o horst_normal_efficiency.root)
