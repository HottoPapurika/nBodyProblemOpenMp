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

void noDataRace(std::vector<Body>& bodies) {
#pragma omp parallel for
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