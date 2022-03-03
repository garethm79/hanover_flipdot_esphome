#include "hanover_flipdot.h"
#include "esphome/core/log.h"


namespace esphome {
namespace hanover_flipdot {

static const char *const TAG = "hanover_flipdot";

void hanover_flipdot::initialize() {
    ESP_LOGD("custom","hanover_flipdot::initialize", this);
  this->init_internal_(this->get_buffer_length_());

}

int hanover_flipdot::roundUp(int numToRound, int multiple)
{
    if (multiple == 0)
        return numToRound;

    int remainder = numToRound % multiple;
    if (remainder == 0)
        return numToRound;

    return numToRound + multiple - remainder;
}

uint8_t hanover_flipdot::toAscii(uint8_t val)
{
  if( val >= 10 ) {
    return val - 10 + 'A';
  }
  else {
    return val + '0';
  }
}

int hanover_flipdot::get_width_internal() { return this->width_;  } 
int hanover_flipdot::get_height_internal() { return this->height_; } 

int hanover_flipdot::get_height_rounded() { return roundUp(this->get_height_internal(), 8); } 


size_t hanover_flipdot::get_buffer_length_() {
  return size_t(this->get_width_internal()) * size_t(this->get_height_rounded()) / 8u;
}

void HOT hanover_flipdot::display() {
  ESP_LOGD("custom","hanover_flipdot::display");
    
    
  static uint8_t header [] = {
      0x02, 0x31, 0x30, 0x00, 0x00 
  };

  static uint8_t footer [] = {
      0x03, 0x00, 0x00  
  };

  uint8_t *payload{nullptr};
  payload = new uint8_t[get_buffer_length_() * 2]; 

    for (int i=0; i < get_buffer_length_(); i++) {
      int b = buffer_[i];

      // set payload ascii values
      payload[i*2] = toAscii((b >> 4) & 0x0F);
      payload[i*2+1]= toAscii(b & 0x0F);

    }

  // header
  header[2] = toAscii(address_); // display address
  header[3] = toAscii((get_buffer_length_() >> 4) & 0x0F);
  header[4] = toAscii(get_buffer_length_() & 0x0F);

  // Calculate crc
  uint8_t crc = 0;
  for( int8_t t = 1; t < sizeof header; t++) crc += header[t];
    for (int i=0; i < get_buffer_length_() * 2; i++) {
       crc += payload[i];
    }

  crc += footer[0];
  crc &= 0xFF;
  crc ^= 255;
  crc += 1;
  footer[1] = toAscii((crc >> 4) & 0x0F);
  footer[2] = toAscii(crc & 0x0F);

// write to serial

this->write_array(header, sizeof header);
this->write_array(payload, get_buffer_length_() * 2);
this->write_array(footer, sizeof footer);


}

void HOT hanover_flipdot::draw_absolute_pixel_internal(int esphome_x, int esphome_y, Color color) {

int x = esphome_x;
int y = esphome_y + offset_;

  //ESP_LOGD("custom","pixel: %d", x);
  //ESP_LOGD("custom","pixel: %d", y);
  //ESP_LOGD("custom","pixel: %d", color);

  int bytepos = (((get_height_rounded() * x) + y) / 8u) ;
  //int bitval = 1 << (y % 8);
  int bitpos = y % 8;
  int bitval =  1 << bitpos;
 
 //printf("pixel %d/%d \n", x, y);
  //printf("byteval before: %d \n", buffer[bytepos]);

  if (color.is_on())
    buffer_[bytepos] |=  bitval;
  else
    buffer_[bytepos] &= ~bitval;

  //printf("bytepos: %d \n", bytepos);
  //printf("bitval: %d = %d \n", y, bitval);

  //printf("byteval after: %d \n\n", buffer[bytepos]);
}

void hanover_flipdot::dump_config() {
  ESP_LOGD("custom","hanover_flipdot::dump_config", this);

  LOG_DISPLAY("", "hanover_flipdot", this);
  
  //LOG_PIN("  DC Pin: ", this->dc_pin_);

  LOG_UPDATE_INTERVAL(this);
}

void hanover_flipdot::update() {
  //ESP_LOGD("custom","hanover_flipdot::update", this);
  this->do_update_();
  this->display();
}

void hanover_flipdot::fill(Color color) {

  uint8_t fill = color.is_on() ? 0xFF : 0x00;
  for (uint8_t i = 0; i < this->get_buffer_length_(); i++)
  {
    this->buffer_[i] = fill;
  }
}
//




}  // namespace hanover_flipdot
}  // namespace esphome
