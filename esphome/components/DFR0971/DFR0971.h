#pragma once
#include "esphome/core/component.h"
#include "esphome/components/output/float-output.h"
#include "esphome/core/hal.h"
#include "esphome/components/i2c/i2c.h"

#define GP8302_CONFIG_CURRENT_REG 0x02
#define OUTPUT_RANGE 0X01

namespace esphome {
namespace DFR0971 {
/**
 * @enum eOutPutRange_t
 * @brief Analog voltage output range select
 */
enum class eOutPutRange_t {
  eOutputRange5V = 0X00,
  eOutputRange10V = 0X11,
};

enum class ChannelSelect : uint8_t {
			Channel1 = 0,
			Channel2 = 1,
			Both = 2
		}

class DFR0971 : public i2c::I2CDevice, public Component {
 public:
  void setup() override;
  void dump_config() override;
};

class DFR0971Output : public output::output, public Component, public Parented<DFR0971> {
 public:
  DFR0971Output(ChannelBit channel_) : channel(channel_) {}

  void set_max_power(float max_power) override;
  void set_level(float state) override;

 protected:
  ChannelSelect channel{ChannelSelect::Both};
  uint16_t voltage = 0;
}  // namespace DFR0971
}  // namespace esphome
