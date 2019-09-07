#ifndef STYLES_BYTEORDER_H
#define STYLES_BYTEORDER_H

// Usage: ByteOrderStyle<BYTEORDER, COLOR>
// BYTEORDER: Color8::RGB, or one of the other byte orders
// COLOR: COLOR
// return value: COLOR

// This shuffles the RGB values around. It's meant to be used
// when you are mixing different kind of pixels on one string.
// While it's not recommended to do so, this should make it
// easier to compensate by re-ordering the channels.

template<Color8::Byteorder byteorder, class COLOR>
class ByteOrderStyle {
public:
  void run(BladeBase* base) {
    color_.run(base);
    order_ = Color8::combine_byteorder(Color8::invert_byteorder(base->get_byteorder()), byteorder);
  }
  OverDriveColor getColor(int led) {
    Color16 tmp;
    OverDriveColor c = color_.getColor(led);
    tmp.r = c.c.getShort(order_, 2);
    tmp.g = c.c.getShort(order_, 1);
    tmp.b = c.c.getShort(order_, 0);
    c.c = tmp;
    return c;
  }
  
  COLOR color_;
  Color8::Byteorder order_;
};


#endif
