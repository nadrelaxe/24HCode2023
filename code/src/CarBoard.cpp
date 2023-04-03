#include <CarBoard.hpp>
#include <ImuLSM6DS3.hpp>
#include <Max17261.hpp>

#include <ESP8266WiFi.h>
#include <NeoPixelBus.h>
#include <IRrecv.h>

namespace {
	constexpr const int PIN_DEBUG_RX = 3;
	constexpr const int PIN_DEBUG_TX = 1;
	constexpr const int PIN_THROTTLE = 14;
	constexpr const int PIN_STEERING = 0;
	constexpr const int PIN_HEADLIGHTS = 16;
	constexpr const int PIN_IR = 12;

	SoftwareSerial debugSerial(PIN_DEBUG_RX, PIN_DEBUG_TX);
	NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart1800KbpsMethod> strip(/* length */ 1);
	ImuLSM6DS3 imu(Wire);
	IRrecv ir_recv(PIN_IR);
	decode_results ir_result;
	Max17261 battery;
};

void CarBoard::init() {
	_throttleServo.attach(PIN_THROTTLE);
	setThrottle(0);

	_steeringServo.attach(PIN_STEERING);
	setSteering(0);

	pinMode(PIN_HEADLIGHTS, OUTPUT);
	setHeadlights(0);

	EEPROM.begin(256);

	Serial.begin(115200);
	Serial.swap(); // Change uart mux
	U0C0 |= BIT(UCTXI); // Invert TX signal

	debugSerial.begin(76800);
	debugSerial.println("CarInSitu");

	strip.Begin();
	setColor(0, 0, 0);

	_batt_adc = analogRead(0);

	Wire.begin();
	imu.init();
	ir_recv.enableIRIn();

	// Init battery

	// 800 (400mAh on 10mΩ)
	// 900 (450mAh on 10mΩ)
	// 1200 (600mAh on 10mΩ)
	const uint16_t designCapacity = 900;
	// FIXME: Tune this value according to our application
	const uint16_t iChgTerm = 0x0640; // (250mA on 10mΩ)
	// VE: Empty Voltage Target, during load
	// VR: Recovery voltage
	const uint16_t vEmpty = 0xB961; // VE/VR: 0xAA61 → 3.4V/3.88V (0xA561 → 3.3V/3.88V (default))
	// In typical cases, if charge voltage > 4,275 then 0x8400 else 0x8000
	// FIXME: Tune this value according to our charge voltage
	const uint16_t modelCFG = 0x8000;
	battery.begin(
			designCapacity,
			iChgTerm,
			vEmpty,
			modelCFG
		     );
}

void CarBoard::loop() {
	unsigned long now = millis();
	if (now - _batt_adc_time > 200) {
		_batt_adc_time = now;
		_batt_adc = analogRead(0);
	}
	if (now - _imu_sample_time >= 10) {
		_imu_sample_time = now;
		auto imu2car_coord = [](auto vec) {
			std::swap(vec[0], vec[1]);
			vec[0] *= -1;
			return vec;
		};
		if (imu.accelerometerDataReady()) _imu_xl = imu2car_coord(imu.readAccelerometer());
		if (imu.gyroscopeDataReady()) _imu_g = imu2car_coord(imu.readGyroscope());
	}
	if (ir_recv.decode(&ir_result)) {
		if (ir_result.decode_type == RC6 && ir_result.bits == 12) {
			uint16_t value = ir_result.value;
			if ((value & 0xf00) == 0xa00) {
				_ir_value = value;
				_ir_time = now;
			}
		}
		ir_recv.resume();
	}
	if (now - _ir_time >= 200) _ir_value = 0;
}

std::array<uint8_t, 6> CarBoard::mac() const {
	std::array<uint8_t, 6> ret;
	WiFi.macAddress(ret.data());
	return ret;
}

Stream & CarBoard::debug_serial() const {
	return debugSerial;
}

void CarBoard::setSteering(int16_t i_angle) {
	const int16_t value = map(i_angle, -32768, 32767, _steering_left, _steering_right);
	_steeringServo.writeMicroseconds(value);
}

void CarBoard::setThrottle(int16_t i_speed) {
	static int16_t i_current_speed = 0;
	const int16_t value = (i_speed > 0) ? map(i_speed, 0, 32767, 1500-_throttle_start_fw, 1000) :
	                      (i_speed < 0) ? map(i_speed, -32768, 0, 2000, 1500+_throttle_start_bw) :
	                      1500;

	// Workaround: Brushed motors can have difficulties to start to rotate
	// To workaround this inertial effect, we overshoot during a short time, then set desired value
	// FIXME: Implement this in a non-blocking way
	if((i_current_speed == 0) && (i_speed != 0)) {
		const int16_t i_value = i_speed > 0 ? 1000 : 2000;
		_throttleServo.writeMicroseconds(i_value);
		delay(20);
	}
	i_current_speed = i_speed;

	_throttleServo.writeMicroseconds(value);
}

void CarBoard::setHeadlights(uint16_t i_pwr) {
	const uint8_t value = map(i_pwr, 0, 65535, 0, 255);
	analogWrite(PIN_HEADLIGHTS, value);
}

void CarBoard::setColor(uint8_t r, uint8_t g, uint8_t b) {
	strip.SetPixelColor(0, RgbColor(r, g, b));
	strip.Show();
}

void CarBoard::setSteeringTrim(int16_t v) {
	const int16_t trim = std::max(static_cast<int16_t>(-450), std::min(v, static_cast<int16_t>(450)));
	_steering_left = 2000 + (trim < 0 ? trim : 0);
	_steering_right = 1000 + (trim > 0 ? trim : 0);
}

void CarBoard::setThrottleStart(uint16_t fw, uint16_t bw) {
	_throttle_start_fw = std::min(fw, static_cast<uint16_t>(450));
	_throttle_start_bw = std::min(bw, static_cast<uint16_t>(450));
}

uint16_t CarBoard::batteryLevel_ADC() const {
	return _batt_adc;
}

int16_t CarBoard::batterySOC() const {
	return battery.readStateOfCharge();
}
