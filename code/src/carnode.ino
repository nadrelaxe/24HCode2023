#include <CarCtrl.hpp>
#include <Mailbox.hpp>
#include <OtaUpdater.hpp>
#include <StatusCast.hpp>
#include <WifiLink.hpp>

CarCtrl car;
WifiLink wifilink(car);
OTAUpdater ota(car);
Mailbox mailbox(car);
StatusCast status_caster(car);

const char wifi_ssid[] = "CarInSitu";
const char wifi_passwd[] = "Roulez jeunesse !";

void setup() {
  car.init();
  wifilink.init(wifi_ssid, wifi_passwd);
  ota.init();
  mailbox.init();
  status_caster.init();
}

void loop() {
  car.loop();
  wifilink.loop();
  ota.loop();
  mailbox.loop();
  status_caster.loop();
}
