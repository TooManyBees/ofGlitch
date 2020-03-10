#pragma once
#include "ColorPicker.h"

class ColorPickerRandom : public ColorPicker {
public:
    ofFloatColor pick() override;
};
