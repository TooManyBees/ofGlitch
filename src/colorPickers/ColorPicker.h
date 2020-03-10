#pragma once
#include "ofMain.h"

class ColorPicker {
public:
  virtual ofFloatColor pick() = 0;
};
