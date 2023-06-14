#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/display/display_buffer.h"

namespace esphome {
namespace hanover_flipdot {

class hanover_flipdot : public PollingComponent, public display::DisplayBuffer, public uart::UARTDevice  {
 public:

  float get_setup_priority() const override { return setup_priority::PROCESSOR; }
  void set_width(int32_t width) { width_ = width; }
  void set_height(int32_t height) { height_ = height; }
  void set_address(int8_t address) { address_ = address; }
  void set_offset(int8_t offset) { offset_ = offset; }

  void initialize();
  void dump_config() override;
  void HOT display();

  void update() override;

  void fill(Color color) override;

  void setup() override {
    this->initialize();
  }
 
  display::DisplayType get_display_type() override {
   return display::DisplayType::DISPLAY_TYPE_BINARY;
  }

 protected:
  void draw_absolute_pixel_internal(int x, int y, Color color) override;

  size_t get_buffer_length_();

  int get_width_internal() override;
  int get_height_internal() override;
  int get_height_rounded();

  int32_t width_;
  int32_t height_;
  int8_t address_;
  int8_t offset_;

  uint8_t *payload{nullptr};
  int roundUp(int numToRound, int multiple);
  uint8_t toAscii(uint8_t val);

};

}  // namespace hanover_flipdot
}  // namespace esphome
