#include <Max17261.hpp>

#include <Wire.h>

#define MAX17261_STATUS_BOOT     4
#define MAX17261_STATUS_POR      2
#define MAX17261_STATUS_MODELCFG 1
#define MAX17261_STATUS_RUN      0

Max17261::Max17261()
{
  _status = MAX17261_STATUS_BOOT;
}

bool Max17261::begin(
    uint16_t designCapacity,
    uint16_t iChgTerm,
    uint16_t vEmpty,
    uint16_t modelCFG)
{
  _designCapacity = designCapacity;
  _iChgTerm = iChgTerm;
  _vEmpty = vEmpty;
  _modelCFG = modelCFG;

  _status = MAX17261_STATUS_RUN;
  return true;
}

void Max17261::process()
{
  switch(_status) {
    case MAX17261_STATUS_BOOT:
      // Nothing to do, user should run begin()
      break;
    case MAX17261_STATUS_RUN:
      // Step 0
      if (statusPOR()) { _status = MAX17261_STATUS_POR; }
      break;
    case MAX17261_STATUS_POR:
      // Step 1
      if(startupOperationsCompleted()) {
        // Step 2
        _hibcfg = read(MAX1726X_HIBCFG_REG); //Store original HibCFG value
        write(MAX1726X_COMMAND_REG, 0x0090); // Exit Hibernate Mode step 1
        write(MAX1726X_HIBCFG_REG, 0x0000);  // Exit Hibernate Mode step 2
        write(MAX1726X_COMMAND_REG, 0x0000); // Exit Hibernate Mode step 3

        // Step 2.1
        write(MAX1726X_DESIGNCAP_REG, _designCapacity); // Design capacity
        write(MAX1726X_ICHGTERM_REG, _iChgTerm);        // Charge Termination Current (cf. End-of-charge detection)
        write(MAX1726X_VEMPTY_REG, _vEmpty);            // Empty and recovery voltages

        _status = MAX17261_STATUS_MODELCFG;
      }
      break;
    case MAX17261_STATUS_MODELCFG:
      if(modelCfgRefreshed()) {
        // Step 2.1 (final stage)
        write(MAX1726X_HIBCFG_REG, _hibcfg); // Restore Original HibCFG value

        // Step 3
        uint16_t status_reg = read(MAX1726X_STATUS_REG); // Read Status
        writeAndVerify(MAX1726X_STATUS_REG, status_reg & 0xFFFD); // Write and Verify Status with POR bit Cleared

        _status = MAX17261_STATUS_RUN;
      }
      break;
  }
}

uint16_t Max17261::read(const uint8_t reg)
{
  Wire.beginTransmission(MAX1726X_I2C_ADDR);
  Wire.write(reg);
  Wire.endTransmission(false);
  
  Wire.requestFrom(MAX1726X_I2C_ADDR, 2);
  uint16_t value = Wire.read();
  value |= (uint16_t)Wire.read() << 8;
  return value;
}

bool Max17261::write(const uint8_t reg, const uint16_t value)
{
  Wire.beginTransmission(MAX1726X_I2C_ADDR);
  Wire.write(reg);
  Wire.write(value & 0xFF);
  Wire.write((value >> 8) & 0xFF);
  return Wire.endTransmission();
}

bool Max17261::writeAndVerify(const uint8_t reg, const uint16_t value)
{
  int attempt = 0;
  uint16_t valueRead;
  do {
    write(reg, value);
    delay(1); // NOTE: ATM, this is acceptable even in a non-blocking context
    valueRead = read(reg);
  } while((value != valueRead) && (attempt++ < 3));

  return(attempt<3);
}

bool Max17261::statusPOR()
{
  return(read(MAX1726X_STATUS_REG) & 0x0002);
}

bool Max17261::startupOperationsCompleted()
{
  bool dnr = read(MAX1726X_FSTAT_REG) & 0x0001;
  return(!dnr);
}

bool Max17261::modelCfgRefreshed()
{
  return(!(read(MAX1726X_MODELCFG_REG) & 0x8000));
}

int16_t Max17261::readRemainingCapacity()
{
  if(_status != MAX17261_STATUS_RUN) {
    return -_status;
  }
  return((int16_t)(read(MAX1726X_REPCAP_REG) >> 1)); // WARN: Depends on RSense
}

int16_t Max17261::readStateOfCharge()
{
  if(_status != MAX17261_STATUS_RUN) {
    return -_status;
  }
  return((int16_t)read(MAX1726X_REPSOC_REG));
}
