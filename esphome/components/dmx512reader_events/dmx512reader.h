#pragma once


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_event.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/sensor/sensor.h"
//#include "esphome/components/uart/uart.h"

namespace esphome {
namespace dmx512reader {

static const uint16_t UPDATE_INTERVAL_MS = 500;
static const uint16_t DMX_MAX_CHANNEL = 512;
static const uint16_t DMX_MSG_SIZE = DMX_MAX_CHANNEL + 1;
static const int DMX_BREAK_LEN = 92;
static const int DMX_MAB_LEN = 12;
static const int DMX_MIN_INTERVAL_MS = 23;

enum DMXState { DMX_IDLE, DMX_BREAK, DMX_DATA, DMX_OUTPUT };

class DMX512Sensor;

class DMX512Reader : public Component {
 public:
  DMX512Reader() = default;

  void setup() override;

  void loop() override;

  // void write_array(const uint8_t *data, size_t len) override;

  // bool peek_byte(uint8_t *data) override;

  // bool read_array(uint8_t *data, size_t len) override;

  // int available() override;

  // void flush() override;

  // returns the dmx value for the givven address (values from 1 to 512)
  static uint8_t Read(uint16_t channel);

  // copies the defined channels from the read buffer
  static void ReadAll(uint8_t *data, uint16_t start, size_t size);

  // returns true, when a valid DMX signal was received within the last 500ms
  static uint8_t IsHealthy();

  void dump_config() override;

  // void set_periodic_update(bool update) { periodic_update_ = update; }

  // void set_update_interval(int intvl) { update_interval_ = intvl; }

  float get_setup_priority() const override { return setup_priority::BUS; }

 protected:
 // static uint8_t device_values_[DMX_MSG_SIZE];

  static QueueHandle_t dmx_rx_queue;

  static SemaphoreHandle_t sync_dmx;

  static DMXState dmx_state;

  static uint16_t current_rx_addr;

  static TickType_t last_dmx_packet;

  static uint8_t dmx_data[DMX_MSG_SIZE];

  static void uart_event_task(void *pvParameters);  // event task

  // esphome::uart::UARTComponent *uart_{nullptr};
  // std::vector<uint8_t> rx_buffer_;
  // uint32_t last_dmx512_transmission_{0};

  // void check_logger_conflict() override;

  static uart_port_t uart_num_;
  uart_config_t get_config_();
  SemaphoreHandle_t lock_;

  // bool has_peek_{false};
  // uint8_t peek_byte_;
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
  // void set_dmxsensor_switch(switch_::Switch *dmxsensor_switch) { dmxsensor_switch_ = dmxsensor_switch; }

 protected:
  esp_event_loop_args_t event_task_args = {
      .queue_size = 5, .task_name = "custom event", .task_priority = 5, .task_stack_size = 2048, .task_core_id = 0};
  sensor::Sensor *dmxsensor_{nullptr};
  int channel_;
  bool updated = false;
  //  switch_::Switch *dmxsensor_switch_;
  DMX512Reader *universe_{nullptr};
};

}  // namespace dmx512reader
}  // namespace esphome
