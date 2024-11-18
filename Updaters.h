#pragma once

#include <vector>

#include <omp.h>

#include "Body.h"
#include "Constants.h"

void singleThread(std::vector<Body>& bodies) {
	for (int i = 0; i < bodies.size(); i++) {
		bodies[i].forces = Vector2{ 0,0 };
		for (int j = 0; j < bodies.size(); j++) {
			if (i == j) continue;

			double forceMagnitude = Body::getForceMagnitudeBetweenTwoBodies(bodies[i], bodies[j]);
			Vector2 dir = (bodies[j].position - bodies[i].position) * TO_METERS;
			bodies[i].forces += forceMagnitude * dir;
		}

		bodies[i].moveBody(TIME_STEP);
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

		bodies[i].moveBody(TIME_STEP);
	}
}