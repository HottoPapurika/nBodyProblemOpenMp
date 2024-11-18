#pragma once

#include "Vector2.h"
#include "Constants.h"
#include <random>

class Body
{
public:

	double mass;
	Vector2 velocity; //	m/s
	Vector2 forces;   //	kg*m/s
	Vector2 position; //	AU

	void move(double dt) {
		Vector2 dv = (forces / mass) * dt;;
		velocity += dv;
		position += (velocity + dv / 2) / TO_METERS * dt; //from AU to m
	}

	static double getForceMagnitudeBetweenTwoBodies(const Body& that, const Body& other) {
		double dist = (that.position - other.position).len() * TO_METERS; //from AU to m

		return (G * that.mass * other.mass) / (dist * dist * dist + SOFTENING* SOFTENING*SOFTENING);
	}


	static std::vector<Body> generate(int n) {
		std::vector<Body> bodies;

		bodies.reserve(n);

		std::uniform_real_distribution<double> randAngle(0.0, 200.0 * PI);

		std::uniform_real_distribution<double> randRadius(INNER_BOUND, SYSTEM_SIZE);

		std::random_device rd;
		std::mt19937 gen(rd());

		double angle;
		double radius;
		double velocity;

		bodies.push_back(Body{ SOLAR_MASS, Vector2{ 0.0, 0.0 }, Vector2{ 0.0, 0.0 }, Vector2{ 0.0, 0.0 } }); //MASSIVE STAR

		for (int index = 1; index < n; ++index) {

			angle = randAngle(gen);  
			radius = sqrt(SYSTEM_SIZE) * sqrt(randRadius(gen));

			auto t = ((G * (SOLAR_MASS + ((radius - INNER_BOUND) / SYSTEM_SIZE) * SOLAR_MASS)));
			velocity = t / (radius * TO_METERS);
			velocity = pow(velocity, 0.5);
			
			auto mass = (SOLAR_MASS) / n;

			bodies.push_back(Body{
				mass,													// mass
				Vector2{ velocity * sin(angle), -velocity * cos(angle)},// velocity
				Vector2{ 0.0, 0.0},										// force
				Vector2{ radius * cos(angle), radius * sin(angle)},		// position
				});             
		}

		return bodies;
	}
};

