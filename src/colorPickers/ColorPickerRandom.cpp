#include "ColorPickerRandom.h"

ofFloatColor ColorPickerRandom::pick() {
  ofFloatColor color;
  color.setHsb(ofRandom(1.0), 1.0, 1.0);
  return color;
}
