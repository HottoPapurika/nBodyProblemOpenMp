#include <iostream>

#include <iomanip>

#include "Updaters.h"
#include "Constants.h"
#include "Simulation.h"
#include <omp.h>
#include "cmdparser.h"

template<typename T>
void test(T& update, int minThreads, int maxThreads, int numBodies, int numFrames) {
	std::cout << "num threads:";
	for (int i = minThreads; i <= maxThreads; i++) {
		std::cout << std::setw(8) << i;
	}
	std::cout << "\nTIME:" << std::setw(9) << ' ';
	double t = 0;
	for (int i = minThreads; i <= maxThreads; i++) {
		omp_set_num_threads(i);
		for (int j = 0; j < 10; j++) {
			Simulation simulation;
			simulation.randomGenerateBodies(numBodies);
			t += simulation.simulate(update, numFrames);
		}
		t /= 10;
		std::cout << std::setw(8) << std::setprecision(3) << t;
	}
}

template<typename T>
void drawGif(T& update, int numThreads, int numBodies, int numFrames, std::string outputFileName) {
	omp_set_num_threads(numThreads);

	Simulation simulation;
	simulation.randomGenerateBodies(numBodies);
	simulation.drawSimulation(update, numFrames, outputFileName);
}

void testAll(int minNumThreads, int maxNumThreads, int numBodies, int numFrames) {
	TriangleLocalForces triangleLocalForces;
	OptimizedTriangleLocalForces optimizedTriangleLocalForces;

	std::cout << "\ntriangleLocalForces:\n";
	test(triangleLocalForces, minNumThreads, maxNumThreads, numBodies, numFrames);
	std::cout << "\noptimizedTriangleLocalForces:\n";
	test(optimizedTriangleLocalForces, minNumThreads, maxNumThreads, numBodies, numFrames);
	std::cout << "\nsquareNoDataRace:\n";
	test(squareNoDataRace, minNumThreads, maxNumThreads, numBodies, numFrames);
	std::cout << "\ntriangleAtomic:\n";
	test(triangleAtomic, minNumThreads, maxNumThreads, numBodies, numFrames);
}

void configure_parser(cli::Parser& parser) {
	parser.set_optional<std::string>("m", "method", "triangleLocalForces", "The name of the simulation update method. (singleThread, squareNoDataRace, triangleAtomic, triangleLocalForces, optimizedTriangleLocalForces, all))");
	parser.set_optional<bool>("g", "gif", false, "Make gif.");
	parser.set_optional<bool>("t", "test", false, "Starts time testing of the selected method.");
	parser.set_optional<int>("n", "numThreads", 12, "The number of threads used in the simulation. (default: 12)");
	parser.set_optional<int>("min", "minThreads", 1, "The minimum number of iterations used in testing time of the selected method.");
	parser.set_optional<int>("max", "maxThreads", 12, "The maximum number of iterations used in testing time of the selected method.");
	parser.set_optional<int>("f", "frames", 1000, "Number of frames (simulation iterations).");
	parser.set_optional<int>("nb", "numBodies", 1000, "The number of bodies involved in the simulation.");
	parser.set_optional<std::string>("o", "output", "result", "Output file name (without file extension).");
}


int main(int argc, char* argv[])
{
	cli::Parser parser(argc, argv);
	configure_parser(parser);
	parser.run_and_exit_if_error();

	if (parser.get<bool>("g") && parser.get<bool>("t")) {
		std::cout << "You can only draw a gif and test the method separately, remove one of the parameters.";
		return -1;
	}

	if (!parser.get<bool>("g") && !parser.get<bool>("t")) {
		std::cout << "There must be one of the parameters: test or gif. Set one of the gif or test parameters, depending on what you want.";
		return -1;
	}


	std::string method = parser.get<std::string>("m");
	std::string outputFileName = parser.get<std::string>("o");

	bool isGifDraw = parser.get<bool>("g");
	bool isTimeTest = parser.get<bool>("t");

	if (isGifDraw) {
		outputFileName += ".gif";
	}
	if (isTimeTest) {
		outputFileName += ".txt";
	}

	int minNumThreads = parser.get<int>("min");
	int maxNumThreads = parser.get<int>("max");
	int numThreads = parser.get<int>("n");
	int numFrames = parser.get<int>("f");
	int numBodies = parser.get<int>("nb");

	if (method == "singleThread") {
		if (isTimeTest) test(singleThread, minNumThreads, maxNumThreads, numBodies, numFrames);
		if (isGifDraw) {
			drawGif(singleThread, numThreads, numBodies, numFrames, outputFileName);
		}
	}
	else if (method == "squareNoDataRace") {
		if (isTimeTest) test(squareNoDataRace, minNumThreads, maxNumThreads, numBodies, numFrames);
		if (isGifDraw) {
			drawGif(squareNoDataRace, numThreads, numBodies, numFrames, outputFileName);
		}
	}
	else if (method == "triangleAtomic") {
		if (isTimeTest) test(triangleAtomic, minNumThreads, maxNumThreads, numBodies, numFrames);
		if (isGifDraw) {
			drawGif(triangleAtomic, numThreads, numBodies, numFrames, outputFileName);
		}
	}
	else if (method == "triangleLocalForces") {
		TriangleLocalForces tlf;
		if (isTimeTest) {
			test(tlf, minNumThreads, maxNumThreads, numBodies, numFrames);
		}
		if (isGifDraw) {
			drawGif(tlf, numThreads, numBodies, numFrames, outputFileName);
		}
	}
	else if (method == "optimizedTriangleLocalForces") {
		OptimizedTriangleLocalForces otlf;
		if (isTimeTest) {
			test(otlf, minNumThreads, maxNumThreads, numBodies, numFrames);
		}
		if (isGifDraw) {
			drawGif(otlf, numThreads, numBodies, numFrames, outputFileName);
		}
	}
	else if (method == "all") {
		if (isGifDraw) {
			std::cout << "The method rendering function is not available for the all parameter";
			return -1;
		}
		if (isTimeTest) testAll(minNumThreads, maxNumThreads, numBodies, numFrames);
		return 0;
	}
	else {
		std::cout << "\nERROR: There is no such method (try: singleThread, squareNoDataRace, triangleAtomic, triangleLocalForces, optimizedTriangleLocalForces, all)";
		return -1;
	}
}