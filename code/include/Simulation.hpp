#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <cstdint>

class Simulation {
	private:
		unsigned long _last_update {0};

		int16_t _last_throttle {0};
		int16_t _last_steering {0};

		float _x {0};
		float _y {0};
		float _theta {0};

	public:
		void init();
		void loop();
		void update();

		void pilot(int16_t throttle, int16_t steering) {
			_last_throttle = throttle;
			_last_steering = steering;
		}

		int16_t x() const;
		int16_t y() const;
		int16_t theta() const;
};

#endif
