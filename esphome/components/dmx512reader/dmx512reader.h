#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/uart/uart.h"

static const uint16_t UPDATE_INTERVAL_MS = 500;
static const uint16_t DMX_MAX_CHANNEL = 512;
static const uint16_t DMX_MSG_SIZE = DMX_MAX_CHANNEL + 1;
static const int DMX_BREAK_LEN = 92;
static const int DMX_MAB_LEN = 12;
static const int DMX_MIN_INTERVAL_MS = 23;

namespace esphome {
namespace dmx512reader {

class DMX512Sensor;

class DMX512Reader : public Component, public uart::UARTDevice {
 public:
  DMX512Reader() = default;

  uint8_t device_values_[DMX_MSG_SIZE];

  //void set_uart_parent(esphome::uart::UARTComponent *parent) { this->uart_ = parent; }

  void setup() override;

  void update() override;

  void dump_config() override;

  //virtual void send_break() = 0;

  void set_enable_pin(GPIOPin *pin_enable) { pin_enable_ = pin_enable; }

  void set_uart_tx_pin(InternalGPIOPin *tx_pin) { tx_pin_ = tx_pin; }

  void set_periodic_update(bool update) { periodic_update_ = update; }

  void set_update_interval(int intvl) { update_interval_ = intvl; }

  //virtual void set_uart_num(int num) = 0;

  float get_setup_priority() const override { return setup_priority::BUS; }

 protected:
  //esphome::uart::UARTComponent *uart_{nullptr};
  std::vector<uint8_t> rx_buffer_;
  uint32_t last_dmx512_transmission_{0};

  int uart_idx_{0};
  InternalGPIOPin *tx_pin_{nullptr};
  int update_interval_{UPDATE_INTERVAL_MS};
  int mab_len_{DMX_MAB_LEN};
  int break_len_{DMX_BREAK_LEN};
  uint16_t max_chan_{0};
  bool periodic_update_{true};
  uint32_t last_update_{0};
  GPIOPin *pin_enable_{nullptr};
};

class DMX512Sensor : public PollingComponent {
 public:
  /// Set up the pins and check connection.
  void setup() override;
  void dump_config() override;
  void update() override;
  float get_setup_priority() const override;

  void set_universe(DMX512Reader *universe) { this->universe_ = universe; }
  void set_dmxsensor(sensor::Sensor *dmxsensor) { dmxsensor_ = dmxsensor; }
  void set_dmxsensor_channel(int channel) { channel_ = channel; }
  //void set_dmxsensor_switch(switch_::Switch *dmxsensor_switch) { dmxsensor_switch_ = dmxsensor_switch; }

 protected:
  sensor::Sensor *dmxsensor_{nullptr};
  int channel_;
//  switch_::Switch *dmxsensor_switch_;
  DMX512Reader *universe_{nullptr};
};

}  // namespace dmx512reader
}  // namespace esphome
