#include <iostream>

#include <iomanip>

#include "Updaters.h"
#include "Constants.h"
#include "Simulation.h"
#include <omp.h>

template<typename T>
void test(T& update) {
	int max_threads = omp_get_max_threads();

	std::cout << "num threads:";
	for (int i = 1; i <= max_threads; i++) {
		std::cout << std::setw(8) << i;
	}
	std::cout << "\nTIME:" << std::setw(9) << ' ';
	double t = 0;
	for (int i = 12; i <= max_threads; i++) {
		omp_set_num_threads(i);
		for (int j = 0; j < 10; j++) {
			Simulation simulation;
			simulation.randomGenerateBodies(3500);
			t += simulation.simulate(update);
		}
		t /= 10;
		std::cout << std::setw(8) << std::setprecision(3) << t;
	}
}


int main(int argc, char* argv[])
{

	if (argc == 1) {
		Simulation simulation;
		simulation.randomGenerateBodies(2000);
		OptimizedTriangleLocalForces triangleLocalForces;
		simulation.drawSimulation(squareNoDataRace, "result.gif");
	}
	else if (argc == 2) {
		void (*update)(std::vector<Body>&) = nullptr;

		if (strcmp(argv[1], "singleThread") == 0) {
			test(singleThread);
		}
		else if (strcmp(argv[1], "squareNoDataRace") == 0) {
			test(squareNoDataRace);
		}
		else if (strcmp(argv[1], "triangleAtomic") == 0) {
			test(triangleAtomic);
		}
		else if (strcmp(argv[1], "triangleCritical") == 0) {
			test(triangleCritical);
		}
		else if (strcmp(argv[1], "triangleLocalForces") == 0) {
			TriangleLocalForces tlf;
			test(tlf);
		}
		else if (strcmp(argv[1], "optimizedTriangleLocalForces") == 0) {
			OptimizedTriangleLocalForces otlf;
			test(otlf);
		}
		else {
			std::cout << "\nERROR: There is no such method (try: singleThread, noDataRace, triangleAtomic)";
			return -1;
		}

	}
	else {
		std::cout << "\n\nERROR: TOO MANY PARAMETERS";
		return -1;
	}
}