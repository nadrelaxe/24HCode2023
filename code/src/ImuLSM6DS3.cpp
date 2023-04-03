#include <ImuLSM6DS3.hpp>

namespace {
	const constexpr uint8_t WHO_AM_I = 0x0F;

	const constexpr uint8_t CTRL1_XL = 0x10;
	const constexpr uint8_t CTRL2_G = 0x11;

	const constexpr uint8_t STATUS_REG = 0x1E;

	const constexpr uint8_t OUTX_L_G = 0x22;
	const constexpr uint8_t OUTX_L_XL = 0x28;
}

uint8_t ImuLSM6DS3::readRegister(uint8_t address) {
	uint8_t ret;
	readRegisters(address, &ret, 1);
	return ret;
}

void ImuLSM6DS3::readRegisters(uint8_t address, uint8_t * data, size_t length) {
	_wire.beginTransmission(_address);
	_wire.write(address);
	_wire.endTransmission(false);
	_wire.requestFrom(_address, length);
	for (size_t i = 0; i < length; i++) *data++ = _wire.read();
}

void ImuLSM6DS3::writeRegister(uint8_t address, uint8_t value) {
	_wire.beginTransmission(_address);
	_wire.write(address);
	_wire.write(value);
	_wire.endTransmission();
}

void ImuLSM6DS3::init() {
	_wire.begin();
	uint8_t who = readRegister(WHO_AM_I);
	_found = (who == 0x69 || who == 0x6A);

	if (_found) {
		writeRegister(CTRL1_XL,
			(4 /* ODR: 104Hz */ << 4) |
			(2 /* FullScale ±4g */ << 2) |
			(3 /* Bandwidth 50Hz */ << 0)
		);
		writeRegister(CTRL2_G,
			(4 /* ODR: 104Hz */ << 4) |
			(2 /* FullScale 500dps */ << 2) |
			(0 /* FS_125 disabled */ << 1)
		);
	}
}

ImuLSM6DS3::data_t ImuLSM6DS3::readAccelerometer() {
	if (!_found) return {};
	data_t ret;
	readRegisters(OUTX_L_XL, reinterpret_cast<uint8_t*>(ret.data()), 6);
	return ret;
}

bool ImuLSM6DS3::accelerometerDataReady() {
	if (!_found) return false;
	return (readRegister(STATUS_REG) & 0x01) != 0;
}

ImuLSM6DS3::data_t ImuLSM6DS3::readGyroscope() {
	if (!_found) return {};
	data_t ret;
	readRegisters(OUTX_L_G, reinterpret_cast<uint8_t*>(ret.data()), 6);
	return ret;
}

bool ImuLSM6DS3::gyroscopeDataReady() {
	if (!_found) return false;
	return (readRegister(STATUS_REG) & 0x02) != 0;
}

