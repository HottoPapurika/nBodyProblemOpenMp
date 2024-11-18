#include <iostream>

#include <iomanip>

#include "Updaters.h"
#include "Constants.h"
#include "Simulation.h"


int main(int argc, char* argv[])
{

	if (argc == 1) {
		Simulation simulation;
		simulation.randomGenerateBodies(1000);
		simulation.drawSimulation(noDataRace, "result.gif");
	}
	else if (argc == 2) {
		void (*update)(std::vector<Body>&);
		if (strcmp(argv[1], "naive") == 0) {
			update = singleThread;
		}
		else if (strcmp(argv[1], "noDataRace") == 0) {
			update = noDataRace;
		}
		else {
			std::cout << "\nERROR: There is no such method (try: singleThread, noDataRace)";
			return -1;
		}

		int max_threads = omp_get_max_threads();

		std::cout << "num threads:";
		for (int i = 1; i <= max_threads; i++) {
			std::cout << std::setw(8) << i;
		}
		std::cout << "\nTIME:" << std::setw(9) << ' ';
		double t = 0;
		for (int i = 1; i <= max_threads; i++) {
			omp_set_num_threads(i);
			for (int j = 0; j < 10; j++) {
				Simulation simulation;
				simulation.randomGenerateBodies(1000);
				t += simulation.simulate(update);
			}
			t /= 10;
			std::cout << std::setw(8) << std::setprecision(3) << t;
		}

	}
	else {
		std::cout << "\n\nERROR: TOO MANY PARAMETERS";
		return -1;
	}
}