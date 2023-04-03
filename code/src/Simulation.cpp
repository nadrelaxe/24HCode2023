#include <Simulation.hpp>

#include <cmath>
#include <Arduino.h>

void Simulation::init() {
}

void Simulation::loop() {
	unsigned long now = millis();
	if (now - _last_update <= 5) {
		_last_update = now;
		update();
	}
}

void Simulation::update() {
	// Not accurate, but interesting enough
	const constexpr float Kt = 0.005;
	const constexpr float Ks = M_PI/6;
	const constexpr float Ka = 6;
	const constexpr float dt = 0.05;

	const float v = _last_throttle / 32768.0f * Kt;
	const float s = _last_steering / 32768.0f * Ks;
	const float x = _x + v * std::cos(_theta) * dt;
	const float y = _y + v * std::sin(_theta) * dt;
	const float theta = _theta + v * Ka * std::tan(s) * dt;

	auto limit_rot = [](float v, float min, float max) {
		float itv = max - min;
		while (v < min) v += itv;
		while (v > max) v -= itv;
		return v;
	};

	_x = limit_rot(x, -1, 1);
	_y = limit_rot(y, -1, 1);
	_theta = limit_rot(theta, -M_PI, M_PI);
}

int16_t Simulation::x() const {
	return _x * 32767;
}

int16_t Simulation::y() const {
	return _y * 32767;
}

int16_t Simulation::theta() const {
	return _theta / M_PI * 32767;
}
