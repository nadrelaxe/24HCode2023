#ifndef WIFILINK_HPP
#define WIFILINK_HPP

#include <CarCtrl.hpp>
#include <Stream.h>

class WifiLink {
	private:
		CarCtrl & _car;

		uint8_t _last_status;

		void print_status();

	public:
		WifiLink(CarCtrl & car) : _car(car) {}
		void init(const char* ssid, const char* pass);
		void loop();
};

#endif
