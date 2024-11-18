#pragma once

#include <vector>

#include <omp.h>

#include "Body.h"
#include "Constants.h"

void singleThread(std::vector<Body>& bodies) {
	for (int i = 0; i < bodies.size(); i++) {
		bodies[i].forces = Vector2{ 0,0 };
	}

	for (int i = 0; i < bodies.size(); i++) {
		for (int j = i + 1; j < bodies.size(); j++) {
			double forceMagnitude = Body::getForceMagnitudeBetweenTwoBodies(bodies[i], bodies[j]);
			Vector2 dir = (bodies[j].position - bodies[i].position) * TO_METERS;
			bodies[i].forces += forceMagnitude * dir;
			bodies[j].forces -= forceMagnitude * dir;
		}
	}

	for (auto& body : bodies) {
		body.move(TIME_STEP);
	}
}

void triangleCritical(std::vector<Body>& bodies) {

#pragma omp parallel for
	for (int i = 0; i < bodies.size(); i++) {
		bodies[i].forces = Vector2{ 0,0 };
	}

#pragma omp parallel for schedule(dynamic, 6)
	for (int i = 0; i < bodies.size(); i++) {
		for (int j = i + 1; j < bodies.size(); j++) {
			double forceMagnitude = Body::getForceMagnitudeBetweenTwoBodies(bodies[i], bodies[j]);
			Vector2 dir = (bodies[j].position - bodies[i].position) * TO_METERS;

#pragma omp critical
			{
				bodies[i].forces += forceMagnitude * dir;
				bodies[j].forces -= forceMagnitude * dir;
			}
		}
	}

#pragma omp parallel for
	for (int i = 0; i < bodies.size(); i++) {
		bodies[i].move(TIME_STEP);
	}
}

void triangleAtomic(std::vector<Body>& bodies) {

#pragma omp parallel for
	for (int i = 0; i < bodies.size(); i++) {
		bodies[i].forces = Vector2{ 0,0 };
	}

#pragma omp parallel for schedule(dynamic, 6)
	for (int i = 0; i < bodies.size(); i++) {
		for (int j = i + 1; j < bodies.size(); j++) {
			double forceMagnitude = Body::getForceMagnitudeBetweenTwoBodies(bodies[i], bodies[j]);
			Vector2 dir = (bodies[j].position - bodies[i].position) * TO_METERS;

#pragma omp atomic
			bodies[i].forces.x += forceMagnitude * dir.x;
#pragma omp atomic
			bodies[i].forces.y += forceMagnitude * dir.y;
#pragma omp atomic
			bodies[j].forces.x -= forceMagnitude * dir.x;
#pragma omp atomic
			bodies[j].forces.y -= forceMagnitude * dir.y;
		}
	}

#pragma omp parallel for
	for (int i = 0; i < bodies.size(); i++) {
		bodies[i].move(TIME_STEP);
	}
}

void squareNoDataRace(std::vector<Body>& bodies) {
#pragma omp parallel for schedule(dynamic, 6)
	for (int i = 0; i < bodies.size(); i++) {
		bodies[i].forces = Vector2{ 0,0 };
		for (int j = 0; j < bodies.size(); j++) {
			if (i == j) continue;

			double forceMagnitude = Body::getForceMagnitudeBetweenTwoBodies(bodies[i], bodies[j]);
			Vector2 dir = (bodies[j].position - bodies[i].position) * TO_METERS;
			bodies[i].forces += forceMagnitude * dir;
		}
	}
#pragma omp parallel for
	for (int i = 0; i < bodies.size(); i++) {
		bodies[i].move(TIME_STEP);
	}
}

class TriangleLocalForces {
public:
	std::vector<std::vector<Vector2>> localForces;

	void operator()(std::vector<Body>& bodies) {
		if (localForces.size() != omp_get_max_threads()) {
			localForces = std::vector<std::vector<Vector2>>(omp_get_max_threads());
		}
		if (localForces[0].size() != bodies.size()) {
			for (auto& lf : localForces) {
				lf = std::vector<Vector2>(bodies.size());
			}
		}
#pragma omp parallel 
		for (int i = 0; i < bodies.size(); i++) {
			localForces[omp_get_thread_num()][i] = { 0,0 };
		}
#pragma omp parallel for schedule(dynamic, 6)
		for (int i = 0; i < bodies.size(); i++) {
			for (int j = i + 1; j < bodies.size(); j++) {
				double forceMagnitude = Body::getForceMagnitudeBetweenTwoBodies(bodies[i], bodies[j]);
				Vector2 dir = (bodies[j].position - bodies[i].position) * TO_METERS;
				localForces[omp_get_thread_num()][i] += forceMagnitude * dir;
				localForces[omp_get_thread_num()][j] -= forceMagnitude * dir;
			}
		}
		for (int i = 0; i < bodies.size(); i++) {
			bodies[i].forces = { 0,0 };
			for (int j = 0; j < localForces.size(); j++) {
				bodies[i].forces += localForces[j][i];
			}
		}

#pragma omp parallel for
		for (int i = 0; i < bodies.size(); i++) {
			bodies[i].move(TIME_STEP);
		}
	}
};

class OptimizedTriangleLocalForces {
public:
	std::vector<std::vector<Vector2>> localForces;
	std::vector<std::pair<int, int>> indexesToPair;

	void operator()(std::vector<Body>& bodies) {
		if (localForces.size() != omp_get_max_threads()) {
			localForces = std::vector<std::vector<Vector2>>(omp_get_max_threads());
		}
		if (localForces[0].size() != bodies.size()) {
			for (auto& lf : localForces) {
				lf = std::vector<Vector2>(bodies.size());
			}
			indexesToPair.clear();
			for (int i = 0; i < bodies.size(); i++) {
				for (int j = i + 1; j < bodies.size(); j++) {
					indexesToPair.push_back({ i,j });
				}
			}
		}
#pragma omp parallel 
		for (int i = 0; i < bodies.size(); i++) {
			localForces[omp_get_thread_num()][i] = { 0,0 };
		}


#pragma omp parallel for
		for (int k = 0; k < indexesToPair.size(); k++) {
			double forceMagnitude = Body::getForceMagnitudeBetweenTwoBodies(bodies[indexesToPair[k].first], bodies[indexesToPair[k].second]);
			Vector2 dir = (bodies[indexesToPair[k].second].position - bodies[indexesToPair[k].first].position) * TO_METERS;
			localForces[omp_get_thread_num()][indexesToPair[k].first] += forceMagnitude * dir;
			localForces[omp_get_thread_num()][indexesToPair[k].second] -= forceMagnitude * dir;
		}
#pragma omp parallel for
		for (int i = 0; i < bodies.size(); i++) {
			bodies[i].forces = { 0,0 };
			for (int j = 0; j < localForces.size(); j++) {
				bodies[i].forces += localForces[j][i];
			}
		}

#pragma omp parallel for
		for (int i = 0; i < bodies.size(); i++) {
			bodies[i].move(TIME_STEP);
		}
	}
};