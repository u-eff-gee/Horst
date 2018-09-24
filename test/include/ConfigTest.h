#include <vector>

using std::vector;

const int NBINS = 12000;

// Parameters for the spectrum models:

const vector<double> bar_params = {1e5, 9000., 200.};

// Parameters for the response matrix models:

const vector<double> escape_params = {1e5, 0.05, 0.03, 511., 0.02, 1022.};
