#ifndef CARCTRL_HPP
#define CARCTRL_HPP

#include <CarBoard.hpp>
#include <Simulation.hpp>
#include <ESP8266WiFi.h>

class CarCtrlBase {
	protected:
		CarBoard _car;
		WiFiServer _log_server{23};
		WiFiClient _log_client;
		bool _shutdown = false;

	public:
		Stream & log();
};

class CarCtrlConfig : virtual public CarCtrlBase {
	protected:
		struct Config {
			char header[8] = "CarNode";
			uint8_t version = 1;
			char name[17] = "";
			std::array<uint8_t, 6> adminpass {0, 0, 0, 0, 0, 0};
			int16_t steeringTrim = 0;
			uint16_t throttle_start_fw = 0;
			uint16_t throttle_start_bw = 0;

			Config & operator=(const Config &) = default;
		};
		void setConfig(const Config & c);
		void useDefaultConfig();

	private:
		Config _config;

		void setConfigNameMAC();

	public:
		void setConfigName(String name);
		const char * configName();
		String hostname();
		void setConfigAdminPass(std::array<uint8_t, 6> pass);
		const std::array<uint8_t, 6> configAdminPass() const;
		void setConfigSteeringTrim(int16_t value);
		const int16_t configSteeringTrim() const;
		void setConfigThrottleStart(uint16_t fw, uint16_t bw);
		const uint16_t configThrottleStartFw() const;
		const uint16_t configThrottleStartBw() const;
		void saveConfig();
};

class CarCtrlPilot : virtual public CarCtrlBase {
	private:
		int16_t _angle = 0;
		int16_t _speed = 0;
		bool _speed_down = false;
		bool _pilot_started = false;
		bool _invert_throttle = false;
		bool _invert_steering = false;
		int16_t _speed_max_pos = 8192;
		int16_t _speed_max_neg = -8192;

	protected:
		Simulation _sim;

	public:
		const Simulation & simulation() const { return _sim; }
		void start_pilot(bool on);
		void pilot(int16_t i_speed, int16_t i_angle);
		int16_t speed() { return _speed; }
		int16_t angle() { return _angle; }
		bool speed_down() { return _speed_down; }
		bool pilot_started() { return _pilot_started; }
		void invertSteering(bool on) { _invert_steering = on; }
		void invertThrottle(bool on) { _invert_throttle = on; }
		void limitSpeed(int16_t pos, int16_t neg);
};

class CarCtrlHeadlights : virtual public CarCtrlBase {
	private:
		bool _blink = false;
		uint16_t _headlights_pwr = 0;
		uint16_t _headlights_pwr_set = 0;

	protected:
		void setHeadlightsPower(uint16_t i_pwr);

	public:
		void setBlink(bool on) { _blink = on; }
		bool blinks() const { return _blink; }
		void setHeadlights(uint16_t i_pwr);
		uint16_t headlightsPower() const;
};

class CarCtrlRearlight : virtual public CarCtrlBase {
	public:
		typedef std::array<uint8_t, 3> color_t;

	private:
		color_t _color = {255, 0, 255};
		color_t _color_set = {0, 0, 0};

	protected:
		void setDisplayedColor(const color_t c);

	public:
		void setColor(const color_t c);
		color_t color() const;
		color_t displayed_color() const;
};

class CarCtrlLL : virtual public CarCtrlBase {
	public:
		uint16_t batteryLevel_ADC() const { return _car.batteryLevel_ADC(); }
		int16_t batterySOC() const { return _car.batterySOC(); }

		std::array<int16_t, 3> imu_accelerometerData() const { return _car.imuAccelerometerData(); }
		std::array<int16_t, 3> imu_gyroscopeData() const { return _car.imuGyroscopeData(); }
		uint8_t ir_value() const { return _car.ir_value(); }
};

class CarCtrl : public CarCtrlConfig, public CarCtrlPilot, public CarCtrlHeadlights, public CarCtrlRearlight, public CarCtrlLL {
	public:
		void init();
		void loop();

		void start_engine(bool on);
		void shutdown();
};

#endif
