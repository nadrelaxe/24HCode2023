#ifndef MAX17261_H
#define Max17261_H

#include <Max1726x.hpp>

class Max17261 {
public:
  Max17261();

  bool begin(
    uint16_t designCapacity,
    uint16_t iChgTerm,
    uint16_t vEmpty,
    uint16_t modelCFG);
  void process();

  int16_t readRemainingCapacity();
  int16_t readStateOfCharge();
  uint16_t read(const uint8_t reg);
private:
  bool write(const uint8_t reg, const uint16_t value);
  bool writeAndVerify(const uint8_t reg, const uint16_t value);
  bool statusPOR();
  bool startupOperationsCompleted();
  bool modelCfgRefreshed();

  uint16_t _designCapacity;
  uint16_t _iChgTerm;
  uint16_t _vEmpty;
  uint16_t _modelCFG;

  uint8_t _status;
  uint16_t _hibcfg;
};

#endif // MAX17261_H
