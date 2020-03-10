#pragma once
#include "ColorPicker.h"

#define LEN 2
#define ARRAY_LEN (LEN*4)

class ColorPickerTrans : public ColorPicker {
public:
    ColorPickerTrans();
    ofFloatColor pick() override;
private:
    const int arrayLen = ARRAY_LEN;
    unsigned int i = 0;
    ofFloatColor colors[ARRAY_LEN];
};
