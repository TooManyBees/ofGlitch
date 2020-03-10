#include "ColorPickerTrans.h"

ColorPickerTrans::ColorPickerTrans() {
    for (int i = 0; i < arrayLen; i++) {
        switch (i / LEN) {
            case 0:
                colors[i] = ofFloatColor((float)0x55 / 255.0f, (float)0xcd / 255.0f, (float)0xfc / 255.0f);
                break;
            case 1:
                colors[i] = ofFloatColor((float)0xf7 / 255.0f, (float)0xa8 / 255.0f, (float)0xb8 / 255.0f);
                break;
            case 2:
                colors[i] = ofFloatColor(1.0, 1.0, 1.0);
                break;
            case 3:
                colors[i] = ofFloatColor((float)0xf7 / 255.0f, (float)0xa8 / 255.0f, (float)0xb8 / 255.0f);
                break;
        }
    }
}

ofFloatColor ColorPickerTrans::pick() {
  ofFloatColor color = colors[i];
  i += 1;
  if (i >= arrayLen) {
    i = 0;
  }
  return color;
}
