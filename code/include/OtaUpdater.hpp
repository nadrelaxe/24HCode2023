#ifndef OTA_UPDATER_HPP
#define OTA_UPDATER_HPP

#include <CarCtrl.hpp>

class OTAUpdater {
	private:
		CarCtrl & _car;

	public:
		OTAUpdater(CarCtrl & car) : _car(car) {}
		void init();
		void loop();
};

#endif
