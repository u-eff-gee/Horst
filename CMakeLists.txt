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

# Different compile options
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra -Wconversion -Wsign-conversion")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g ${CMAKE_CXX_FLAGS_RELEASE}")

# ROOT libraries
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT REQUIRED)
include(${ROOT_USE_FILE})
message(STATUS "Using ROOT version ${ROOT_VERSION}")
target_link_libraries(horst ${ROOT_LIBRARIES})
target_link_libraries(tsroh ${ROOT_LIBRARIES})
target_link_libraries(makematrix ${ROOT_LIBRARIES})
target_link_libraries(convert_to_txt ${ROOT_LIBRARIES})
