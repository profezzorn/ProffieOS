#ifndef BLADES_MONOPODWS_IMXRT_H
#define BLADES_MONOPODWS_IMXRT_H

#ifdef ENABLE_WS2811

#include <OctoWS2811.h>

// Any group of digital pins may be used
// Teensy 4.x can use any pin, or arbitrary group of pins!
// default options {2, 14, 7, 8, 6, 20, 21, 5};
const int numPins = 3;               //should be auto generated from config file
byte pinList[numPins] = {14, 15, 6}; //should be auto generated from config file

//this does not work obviously, but something like this should be made to work
// void init_pinlist(){ 
//   if (bladePin != -1) pinList[numPins++] = bladePin;
//   if (blade2Pin != -1) pinList[numPins++] = blade2Pin;
//   if (blade3Pin != -1) pinList[numPins++] = blade3Pin;
//   if (blade4Pin != -1) pinList[numPins++] = blade4Pin;
//   if (blade5Pin != -1) pinList[numPins++] = blade5Pin;
//   if (blade6Pin != -1) pinList[numPins++] = blade6Pin;
// }

// maxLedPerStrip determines the fps acheivable, do not make it larger than the largest blade
// ToDo;
// auto generate maxLedsPerStrip from config file largest blade??
DMAMEM int displayMemory[maxLedsPerStrip * numPins * 3 / 4];//always reserve for  numpins*3 for RGB, /4 4bytes per int?
int drawingMemory[maxLedsPerStrip * numPins * 3 / 4];
const int config_ = WS2811_RGB | WS2811_800kHz; //RGB, ProffieOS already swaps bytes correctly
OctoWS2811 leds(maxLedsPerStrip, displayMemory, drawingMemory, config_, numPins, pinList);
uint32_t frame_sync_time_ = 0;//sync to first blade
uint32_t min_update_time_ = 0;//determined by maxLedsPerStrip * 30us/led + 300us idle time, rounded upward
uint32_t frame_num_ = 0;      //frame number shared between blades


template<Color8::Byteorder BYTEORDER>
class MonopodWSPinBase : public WS2811PIN {
public:
MonopodWSPinBase(int num_leds, int pin, uint32_t frequency,
           uint32_t reset_us, uint32_t t0h_us, uint32_t t1h_us) :
    pin_(pin),  //which pin
    num_leds_(num_leds), //amount of leds for this strip
    frequency_(frequency) { //800000

    for (int j = 0; j < numPins; j++) {
        if (pin == pinList[j]) {
            offset_ = maxLedsPerStrip * j;
            STDOUT.print("pin: ");
            STDOUT.print(pin);
            STDOUT.print(", offset: ");
            STDOUT.print(offset_);
            if (j == 0) {
                is_first_blade_ = true;
                STDOUT.print(", is first blade");
                //determined by maxLedsPerStrip * 30us/led + 300us idle time, rounded upward
                STDOUT.print(", min update time us: ");
                STDOUT.print((maxLedsPerStrip * 24000000.0 / frequency_ + 300));
                STDOUT.print(", min update ms: ");
                //round up to nearest ms interval
                min_update_time_ = ((maxLedsPerStrip * 24000000.0 / frequency_ + 300)/1000)+1;
                STDOUT.print(min_update_time_);
            }
            if (j == numPins-1) {
                is_last_blade_ = true;
                STDOUT.print(", is last blade");
            }
            STDOUT.println();
        }
    }
  }
  
  bool IsReadyForBeginFrame() {
        if (is_first_blade_) {
            return !leds.busy();//sync to 1st blade
        }else{
            return true; // other blades can start filling displayMemory
        }           
  }
  
  bool IsReadyForEndFrame() {
        return millis() - frame_sync_time_ >  min_update_time_;//4ms =250fps, 5ms=200fps, 
  }
  
  void BeginFrame() { //num_bytes(BYTEORDER) allows for RGBw
  
    while (Color8::num_bytes(BYTEORDER) * num_leds_ * numPins + 1 > (int)sizeof(displayMemory)) { //*8 remainder of octolib??
      STDOUT.print("Display memory is not big enough, increase maxLedsPerStrip!");
      num_leds_ /= 2; // just to make sure buffer is large enough in case of incorrect spec
    }
    
    while (!IsReadyForBeginFrame()); // wait for frame to finish
        if (is_first_blade_) {
            // sync to rounde ms time interval, when done to just millis(), fps drops???
			// when done in micros(), jitter occurs on additional blades
			frame_sync_time_ = (millis()/min_update_time_)*min_update_time_; 
            frame_num_ ++;//frame number is now global for all blades
            leds.show();
        }
  }
  
  void EndFrame() {
    
    while (!IsReadyForEndFrame()); // wait for pixels to be transfered
    for (int j = 0; j < num_leds_; j++) {
      Color8 color = color_buffer[j].dither(frame_num_, j); //adjust colors, from common/color.h
                                                             // color_buffer defined in ws28111_blade.h
    // For Teensy 4.x, the pixel data is stored in ordinary RGB format. This is
    // different from Teensy 3.x, where the data was stored as bytes to write directly
    // to the GPIO output register.      
    leds.setPixel(j + offset_, color.getByte(BYTEORDER, 0), color.getByte(BYTEORDER, 1), color.getByte(BYTEORDER, 2));
    }
    return;
  }

  int num_leds() const override { return num_leds_; }
  
  Color8::Byteorder get_byteorder() const override { return BYTEORDER; }
  
  void Enable(bool on) override {
    pinMode(pin_, on ? OUTPUT : INPUT_ANALOG); // set actual pin to output
  }

  uint8_t  pin_;
  bool     is_first_blade_;
  bool     is_last_blade_;
  uint32_t offset_;
  int      num_leds_;
  uint32_t start_micros_;
  uint32_t frequency_;
  uint32_t min_update_time_;//determined by maxLedsPerStrip * 30us/led + 300us idle time, rounded upward
};


//actual function for config file
template<int LEDS, int PIN, Color8::Byteorder BYTEORDER, int frequency=800000, int reset_us=300, int t1h=294, int t0h=892>
class MonopodWSPin : public MonopodWSPinBase<BYTEORDER> {
public:
  MonopodWSPin() : MonopodWSPinBase<BYTEORDER>(LEDS, PIN, frequency, reset_us, t0h, t1h) {
  }
};

#endif
#endif
