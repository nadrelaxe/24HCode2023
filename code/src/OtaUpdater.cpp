#include <OtaUpdater.hpp>
#include <ArduinoOTA.h>

void OTAUpdater::init() {
	auto & log = _car.log();
	ArduinoOTA.setHostname(_car.hostname().c_str());
	ArduinoOTA.setPasswordHash("6f67154c3512b8de72af2e635be08f17");
	ArduinoOTA.setRebootOnSuccess(true);

	ArduinoOTA.onStart([this, &log]() {
		_car.shutdown();
		log.print("OTA: Start updating ");
		log.println(ArduinoOTA.getCommand() == U_FLASH ? "sketch" : "filesystem");
	});
	ArduinoOTA.onEnd([&log]() {
		log.println("OTA: End");
	});
	ArduinoOTA.onProgress([&log](unsigned int progress, unsigned int total) {
		log.printf("OTA: Progress: %u%%\r", (progress / (total / 100)));
	});
	ArduinoOTA.onError([&log](ota_error_t error) {
		log.printf("OTA: Error[%u]: ", error);
		log.println([](ota_error_t error){
			switch(error) {
				case OTA_AUTH_ERROR: return "Auth Failed";
				case OTA_BEGIN_ERROR: return "Begin Failed";
				case OTA_CONNECT_ERROR: return "Connect Failed";
				case OTA_RECEIVE_ERROR: return "Receive Failed";
				case OTA_END_ERROR: return "End Failed";
			}
			return "?";
		}(error));
	});

	ArduinoOTA.begin();
	log.println("OTA: Ready");
}

void OTAUpdater::loop() {
	ArduinoOTA.handle();
}

