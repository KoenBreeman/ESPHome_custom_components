#include "esphome/core/log.h"
#include "m5stack4relay.h"
#include <bitset>

namespace esphome {
namespace m5stack4relay {

static const char *const TAG = "m5stack_4relay.switch";

void M5Relay4Control::dump_config() { ESP_LOGCONFIG(TAG, "M5Stack 4relay switch"); }

void M5Relay4Control::setup() {
  write_byte(UNIT_4RELAY_CONFIG_REG, this->sync_mode_);
  write_byte(UNIT_4RELAY_RELAY_REG, 0);

  ESP_LOGCONFIG(TAG, "Setting up M5STACK4RELAY (0x%02X)...", this->address_);
  uint8_t component_status = get_relay_states_();
  ESP_LOGD(TAG, "Setup Status 0x%02X", component_status);
  has_been_setup_ = true;
}

uint8_t M5Relay4Control::get_relay_states_() {
  uint8_t result;
  this->read_byte(UNIT_4RELAY_RELAY_REG, &result);
  return result;
}

bool M5Relay4Control::set_relay(RelayBit bit, bool state) {
  uint8_t org = get_relay_states_();

  if (state) {
    org |= (1u << (uint8_t) bit);
  } else {
    org &= ~(1u << (uint8_t) bit);
  }

  return write_byte(UNIT_4RELAY_RELAY_REG, org);
}

void M5Relay4Control::set_sync_mode(bool mode) {
  this->sync_mode_ = mode;
  if (has_been_setup_) {
    write_byte(UNIT_4RELAY_CONFIG_REG, this->sync_mode_);
  }
}

void M5Relay4Switch::dump_config() {
  LOG_SWITCH("", "M5Stack relay 4 Switch", this);
  if (!this->interlock_.empty()) {
    ESP_LOGCONFIG(TAG, "  Interlocks:");
    for (auto *lock : this->interlock_) {
      if (lock == this)
        continue;
      ESP_LOGCONFIG(TAG, "    %s", lock->get_name().c_str());
    }
  }
}

void M5Relay4Switch::set_interlock(const std::vector<Switch *> &interlock) { this->interlock_ = interlock; }

void M5Relay4Switch::setup() {
  ESP_LOGCONFIG(TAG, "Setting up M5Stack relay 4 Switch '%s'...", this->name_.c_str());

  bool initial_state = this->get_initial_state_with_restore_mode().value_or(false);

  // write state before setup
  if (initial_state) {
    this->turn_on();
  } else {
    this->turn_off();
  }
}

void M5Relay4Switch::write_state(bool state) {
  if (state != this->inverted_) {
    // Turning ON, check interlocking

    bool found = false;
    for (auto *lock : this->interlock_) {
      if (lock == this)
        continue;

      if (lock->state) {
        lock->turn_off();
        found = true;
      }
    }
    if (found && this->interlock_wait_time_ != 0) {
      this->set_timeout("interlock", this->interlock_wait_time_, [this, state] {
        // Don't write directly, call the function again
        // (some other switch may have changed state while we were waiting)
        this->write_state(state);
      });
      return;
    }
  } else if (this->interlock_wait_time_ != 0) {
    // If we are switched off during the interlock wait time, cancel any pending
    // re-activations
    this->cancel_timeout("interlock");
  }

  if (!this->parent_->set_relay(this->bit_, state)) {
    publish_state(false);
  } else {
    publish_state(state);
  }
}

}  // namespace m5stack4relay
}  // namespace esphome
