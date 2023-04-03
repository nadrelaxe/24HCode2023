#include <Arduino.h>
#include <Wire.h>

class ImuLSM6DS3 {
  private:
    TwoWire & _wire;
    uint8_t _address = 0x6A;
    bool _found = false;

  protected:
    uint8_t readRegister(uint8_t address);
    void readRegisters(uint8_t address, uint8_t * data, size_t length);
    void writeRegister(uint8_t address, uint8_t value);

  public:
    typedef std::array<int16_t, 3> data_t;

    ImuLSM6DS3(TwoWire& wire) : _wire(wire) {}
    ImuLSM6DS3(TwoWire& wire, uint8_t address) : _wire(wire), _address(address) {}

    void init();

    data_t readAccelerometer();
    bool accelerometerDataReady();

    data_t readGyroscope();
    bool gyroscopeDataReady();

};
