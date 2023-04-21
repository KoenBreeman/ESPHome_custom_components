#include "dmx512reader.h"
#include "esphome/core/log.h"

#ifdef USE_LOGGER
#include "esphome/components/logger/logger.h"
#endif

namespace esphome {
namespace dmx512reader {

#define DMX_SERIAL_INPUT_PIN GPIO_NUM_22   // pin for dmx rx
#define DMX_SERIAL_OUTPUT_PIN GPIO_NUM_19  // pin for dmx tx

#define DMX_UART_NUM UART_NUM_  // dmx uart

#define HEALTHY_TIME 500  // timeout in ms

#define BUF_SIZE 1024  //  buffer size for rx events

#define DMX_CORE 1  // select the core the rx/tx thread should run on

#define DMX_IGNORE_THREADSAFETY 0  // set to 1 to disable all threadsafe mechanisms

static const char *TAG = "dmx512reader";

QueueHandle_t DMX512Reader::dmx_rx_queue;

SemaphoreHandle_t DMX512Reader::sync_dmx;

DMXState DMX512Reader::dmx_state = DMX_IDLE;

uint16_t DMX512Reader::current_rx_addr = 0;

TickType_t DMX512Reader::last_dmx_packet = 0;

uint8_t DMX512Reader::dmx_data[513];

uart_port_t DMX512Reader::uart_num_ = DMX_UART_NUM;

void DMX512Reader::dump_config() {
  ESP_LOGCONFIG(TAG, "Setting up DMX512 Reader...");
  ESP_LOGCONFIG(TAG, "  Number: %u", this->uart_num_);
  this->setup();
  // LOG_PIN("  TX Pin: ", tx_pin_);
  // LOG_PIN("  RX Pin: ", rx_pin_);
  // if (this->rx_pin_ != nullptr) {
  //  ESP_LOGCONFIG(TAG, "  RX Buffer Size: %u", this->rx_buffer_size_);
  //}
  // ESP_LOGCONFIG(TAG, "  Baud Rate: %u baud", this->baud_rate_);
  // ESP_LOGCONFIG(TAG, "  Data Bits: %u", this->data_bits_);
  // ESP_LOGCONFIG(TAG, "  Parity: %s", LOG_STR_ARG(parity_to_str(this->parity_)));
  // ESP_LOGCONFIG(TAG, "  Stop bits: %u", this->stop_bits_);
}

void DMX512Reader::loop() {
  bool update = false;
  update = true;
  //// An update needs to be triggered periodically, if the
  //// periodic update option is set
  // if ((((millis() - this->last_update_) > this->update_interval_) && this->periodic_update_)) {
  //  // ESP_LOGD(TAG, "Update UART read array");
  //  // Force the refresh rate to be within the spec
  //  if ((millis() - this->last_update_) > DMX_MIN_INTERVAL_MS) {
  //    update = true;
  //  }
  //}
  //if (update) {
  //  if (!this->available()) {
  //    return;
  //  }
  //  delay(16);
  //  ESP_LOGD(TAG, "number of bytes available: %d", this->available());

  //  // uint8_t tmpByte;
  //  // while (this->available()) {
  //  //  this->read_byte(&tmpByte);
  //  //  ESP_LOGD(TAG, "byte : %d", tmpByte);
  //  //}
  //  // if (!(this->read_array(this->device_values_, DMX_MSG_SIZE))) {
  //  if (!(this->read_array(this->device_values_, this->available()))) {
  //    ESP_LOGW(TAG, "Junk on wire. Throwing away partial message");
  //  } else {
  //    for (int i = 0; i < DMX_MSG_SIZE; i++)
  //      ESP_LOGD(TAG, "byte %d: %d", i, this->device_values_[i]);
  //  }

  //  // this->last_update_ = millis();
  //}
}

uint8_t DMX512Reader::IsHealthy() {
  // get timestamp of last received packet
#ifndef DMX_IGNORE_THREADSAFETY
  xSemaphoreTake(sync_dmx, portMAX_DELAY);
#endif
  TickType_t dmx_timeout = last_dmx_packet;
#ifndef DMX_IGNORE_THREADSAFETY
  xSemaphoreGive(sync_dmx);
#endif
  // check if elapsed time < defined timeout
  if (xTaskGetTickCount() - dmx_timeout < HEALTHY_TIME / portTICK_PERIOD_MS) {
    return 1;
  }
  return 0;
}

uint8_t DMX512Reader::Read(uint16_t channel) {
  // restrict acces to dmx array to valid values
  if (channel < 1 || channel > 512) {
    return 0;
  }

  // take data threadsafe from array and return
#ifndef DMX_IGNORE_THREADSAFETY
  xSemaphoreTake(sync_dmx, portMAX_DELAY);
#endif
  uint8_t tmp_dmx = dmx_data[channel];
#ifndef DMX_IGNORE_THREADSAFETY
  xSemaphoreGive(sync_dmx);
#endif
  return tmp_dmx;
}

void DMX512Reader::ReadAll(uint8_t *data, uint16_t start, size_t size) {
  // restrict acces to dmx array to valid values
  if (start < 1 || start > DMX_MAX_CHANNEL || start + size > DMX_MSG_SIZE) {
    return;
  }
#ifndef DMX_IGNORE_THREADSAFETY
  xSemaphoreTake(sync_dmx, portMAX_DELAY);
#endif
  memcpy(data, (uint8_t *) dmx_data + start, size);
#ifndef DMX_IGNORE_THREADSAFETY
  xSemaphoreGive(sync_dmx);
#endif
}

void DMX512Reader::setup() {
  // for (int i = 0; i < DMX_MSG_SIZE; i++)
  //  this->device_values_[i] = 0;

  static uint8_t next_uart_num = 0;
  //#ifdef USE_LOGGER
  //  if (logger::global_logger->get_uart_num() == next_uart_num)
  //    next_uart_num++;
  //#endif
  if (next_uart_num >= UART_NUM_MAX) {
    ESP_LOGW(TAG, "Maximum number of UART components created already.");
    this->mark_failed();
    return;
  }
  //this->uart_num_ = next_uart_num++;
  ESP_LOGCONFIG(TAG, "Setting up UART %u...", this->uart_num_);

  uart_config_t uart_config = this->get_config_();
  esp_err_t err = uart_param_config(DMX_UART_NUM, &uart_config);

  ESP_LOGCONFIG(TAG, "UART config %u...", &uart_config);
  if (err != ESP_OK) {
    ESP_LOGW(TAG, "uart_param_config failed: %s", esp_err_to_name(err));
    this->mark_failed();
    return;
  }

  // Set pins for UART
  err = uart_set_pin(DMX_UART_NUM, DMX_SERIAL_OUTPUT_PIN, DMX_SERIAL_INPUT_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
  if (err != ESP_OK) {
    ESP_LOGW(TAG, "uart set pin failed: %s", esp_err_to_name(err));
    this->mark_failed();
    return;
  }

  if (uart_is_driver_installed(DMX_UART_NUM))
    uart_driver_delete(DMX_UART_NUM);

  // install queue
  err = uart_driver_install(DMX_UART_NUM, BUF_SIZE * 2, BUF_SIZE * 2, 20, &dmx_rx_queue, 0);
  if (err != ESP_OK) {
    ESP_LOGW(TAG, "uart_driver_install failed: %s", esp_err_to_name(err));
    this->mark_failed();
    return;
  }

  // install queue
  // uart_driver_install(DMX_UART_NUM, BUF_SIZE * 2, BUF_SIZE * 2, 20, &dmx_rx_queue, 0);

  // create mutex for syncronisation
  sync_dmx = xSemaphoreCreateMutex();

  dmx_state = DMX_IDLE;

  // create receive task
  xTaskCreatePinnedToCore(DMX512Reader::uart_event_task, "uart_event_task", 2048, NULL, 0, NULL, DMX_CORE);
}

void DMX512Reader::uart_event_task(void *pvParameters) {
  uart_event_t event;
  uint8_t *dtmp = (uint8_t *) malloc(BUF_SIZE);
  for (;;) {
    // wait for data in the dmx_queue
    if (xQueueReceive(dmx_rx_queue, (void *) &event, (portTickType) portMAX_DELAY)) {
      bzero(dtmp, BUF_SIZE);
      switch (event.type) {
        case UART_DATA:
          // read the received data
          uart_read_bytes(DMX_UART_NUM, dtmp, event.size, portMAX_DELAY);
          // check if break detected
          if (dmx_state == DMX_BREAK) {
            // if not 0, then RDM or custom protocol
            if (dtmp[0] == 0) {
              dmx_state = DMX_DATA;
              // reset dmx adress to 0
              current_rx_addr = 0;
#ifndef DMX_IGNORE_THREADSAFETY
              xSemaphoreTake(sync_dmx, portMAX_DELAY);
#endif
              // store received timestamp
              last_dmx_packet = xTaskGetTickCount();
#ifndef DMX_IGNORE_THREADSAFETY
              xSemaphoreGive(sync_dmx);
#endif
            }
          }
          // check if in data receive mode
          if (dmx_state == DMX_DATA) {
#ifndef DMX_IGNORE_THREADSAFETY
            xSemaphoreTake(sync_dmx, portMAX_DELAY);
#endif
            //ESP_LOGD(TAG, "UART Data received");
            // copy received bytes to dmx data array
            for (int i = 0; i < event.size; i++) {
              if (current_rx_addr < 513) {
                dmx_data[current_rx_addr++] = dtmp[i];
                //ESP_LOGD(TAG, "Address %d: %d", current_rx_addr, dtmp[i]);
              }
            }
#ifndef DMX_IGNORE_THREADSAFETY
            xSemaphoreGive(sync_dmx);
#endif
          }
          break;
        case UART_BREAK:
          // break detected
          // clear queue und flush received bytes
          uart_flush_input(DMX_UART_NUM);
          xQueueReset(dmx_rx_queue);
          dmx_state = DMX_BREAK;
          break;
        case UART_FRAME_ERR:
        case UART_PARITY_ERR:
        case UART_BUFFER_FULL:
        case UART_FIFO_OVF:
        default:
          // error recevied, going to idle mode
          uart_flush_input(DMX_UART_NUM);
          xQueueReset(dmx_rx_queue);
          dmx_state = DMX_IDLE;
          break;
      }
    }
  }
}

uart_config_t DMX512Reader::get_config_() {
  uart_config_t uart_config;
  uart_config.baud_rate = 250000;
  uart_config.data_bits = UART_DATA_8_BITS;
  uart_config.parity = UART_PARITY_DISABLE;
  uart_config.stop_bits = UART_STOP_BITS_2;
  uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;

  return uart_config;
}
//
// void DMX512Reader::write_array(const uint8_t *data, size_t len) {
//  xSemaphoreTake(this->lock_, portMAX_DELAY);
//  uart_write_bytes(this->uart_num_, data, len);
//  xSemaphoreGive(this->lock_);
//}
//
// bool DMX512Reader::peek_byte(uint8_t *data) {
//  if (!this->check_read_timeout_())
//    return false;
//  xSemaphoreTake(this->lock_, portMAX_DELAY);
//  if (this->has_peek_) {
//    *data = this->peek_byte_;
//  } else {
//    int len = uart_read_bytes(this->uart_num_, data, 1, 20 / portTICK_RATE_MS);
//    if (len == 0) {
//      *data = 0;
//    } else {
//      this->has_peek_ = true;
//      this->peek_byte_ = *data;
//    }
//  }
//  xSemaphoreGive(this->lock_);
//  return true;
//}
//
// bool DMX512Reader::read_array(uint8_t *data, size_t len) {
//  size_t length_to_read = len;
//  if (!this->check_read_timeout_(len))
//    return false;
//  xSemaphoreTake(this->lock_, portMAX_DELAY);
//  if (this->has_peek_) {
//    length_to_read--;
//    *data = this->peek_byte_;
//    data++;
//    this->has_peek_ = false;
//  }
//  if (length_to_read > 0)
//    uart_read_bytes(this->uart_num_, data, length_to_read, 20 / portTICK_RATE_MS);
//  xSemaphoreGive(this->lock_);
//
//  return true;
//}
//
// int DMX512Reader::available() { return 0; }
//
// void DMX512Reader::flush() {}

void DMX512Sensor::dump_config() {}

float DMX512Sensor::get_setup_priority() const { return setup_priority::AFTER_CONNECTION; }

void DMX512Sensor::setup() {}

void DMX512Sensor::update() {
  // uint8_t sensorvalue = this->universe_->device_values_[this->channel_];
  uint8_t sensorvalue = this->universe_->Read(this->channel_);
  ESP_LOGD(TAG, "Channel '%f': value '%f'", this->channel_, sensorvalue);
  this->dmxsensor_->publish_state((float) sensorvalue);
}

}  // namespace dmx512reader
}  // namespace esphome
