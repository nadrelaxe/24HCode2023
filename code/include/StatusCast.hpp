#ifndef STATUSCAST_HPP
#define STATUSCAST_HPP

#include <WiFiUdp.h>
#include <CarCtrl.hpp>

class StatusCast {
	private:
		typedef enum : uint8_t {
			STATUS_NO_REPORT,
			STATUS_RSSI,
			STATUS_IR,
			STATUS_SIMULATION,
			STATUS_HEADLIGHTS,
			STATUS_COLOR,
			STATUS_BATTERY,
			STATUS_IMU,
			STATUS_PILOT,
		} status_t;

		CarCtrl & _car;
		WiFiUDP _udp;
		unsigned long _last_msg = 0;
		uint8_t _last_msg_id = 0;

	public:
		StatusCast(CarCtrl & car) : _car(car) {}
		void init();
		void loop();
		void send();
};

#endif
