#pragma once

#include <vector>
#include <omp.h>

#include "Body.h"
#include <chrono>
#include <iostream>
#include "NbodyDrawer.h"

class Simulation
{
public:
	std::vector<Body> bodies{};

	void randomGenerateBodies(int N) {
		bodies = Body::generate(N);
	}

	template<typename F>
	double simulate(F& update) {

		auto start = std::chrono::system_clock::now();

		for (int i = 0; i < 100; i++) {
			update(bodies);
		}
		auto finish = std::chrono::system_clock::now();
		return static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count()) / 1000;
	}

	template<typename F>
	void drawSimulation(F& update, std::string outputFile) {

		NbodyDrawer drawer(bodies, outputFile);
		for (int i = 0; i < 1000; i++) {
			update(bodies);
			drawer.update();
		}
		drawer.write();
	}

};


