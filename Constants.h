#pragma once

constexpr double PI = 3.14159265358979323846; // pi
constexpr double TO_METERS = 1.496e11; // Meters in an AU(Astronomical Unit)

constexpr double SYSTEM_SIZE = 3.5; // Farthest particles in AU
constexpr double INNER_BOUND = 0.3; // Closest bodies to center in AU
constexpr double SOFTENING = (0.015 * TO_METERS);

constexpr int width = 256; //width of gif
constexpr int height = 256; //height of gif
constexpr double coeff_w = width / (SYSTEM_SIZE * 5); //To convert coordinates to gif pixels
constexpr double coeff_h = height / (SYSTEM_SIZE * 5); //To convert coordinates to gif pixels

constexpr double SOLAR_MASS = 2.0e30; // in kg
constexpr double G = 6.67408e-11; // The gravitational constant
constexpr double TIME_STEP = 24 * 60 * 60; // one day in sec