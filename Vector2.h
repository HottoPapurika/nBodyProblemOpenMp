#pragma once
#include <cmath>

class Vector2
{
public:
	double x, y;

	Vector2 operator+(const Vector2& other) const {
		return { x + other.x,y + other.y };
	}

	Vector2& operator += (const Vector2& other) {
		x += other.x;
		y += other.y;
		return *this;
	}

	Vector2 operator-(const Vector2& other) const {
		return { x - other.x,y - other.y };
	}

	Vector2& operator-=(const Vector2& other) {
		x -= other.x;
		y -= other.y;
		return *this;
	}

	double len() const {
		return std::sqrt(x * x +y * y);
	}

	friend Vector2 operator*(const Vector2& v, double s) {
		return { s * v.x, s * v.y };
	}

	friend Vector2 operator*(double s, const Vector2& v) {
		return { s * v.x, s * v.y };
	}

	friend Vector2 operator/(const Vector2& v, float s) {
		return { v.x / s, v.y / s };
	}
};

