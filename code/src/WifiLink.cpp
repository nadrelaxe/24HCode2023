#include <WifiLink.hpp>

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

void WifiLink::init(const char* ssid, const char* pass) {
	_last_status = WL_NO_SHIELD;
	_car.setBlink(true);
	WiFi.begin(ssid, pass);

	String hostname = _car.hostname();
	_car.log().print("Hostname: ");
	_car.log().println(hostname);
	MDNS.begin(hostname);
	MDNS.addService("carnode", "udp", 4210);
}

void WifiLink::loop() {
	uint8_t status = WiFi.status();
	if (status != _last_status) {
		_last_status = status;
		print_status();
		_car.setBlink(status != WL_CONNECTED);
	}
	MDNS.update();
}

void WifiLink::print_status() {
	_car.log().print("Wifi status: ");
	_car.log().println([]{
		switch (WiFi.status()) {
			case WL_CONNECTED: return "Connected to a WiFi network";
			case WL_NO_SHIELD: return "No WiFi shield is present";
			case WL_IDLE_STATUS: return "Idle";
			case WL_NO_SSID_AVAIL: return "No SSID are available";
			case WL_SCAN_COMPLETED: return "Scan networks is completed";
			case WL_CONNECT_FAILED: return "Connection fails for all the attempts";
			case WL_CONNECTION_LOST: return "Connection is lost";
			case WL_WRONG_PASSWORD: return "Wrong password";
			case WL_DISCONNECTED: return "Disconnected";
		}
		return "";
	}());
}

