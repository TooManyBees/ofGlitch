#include "toggle.h"

void Toggle::enableThisFrame(bool e) {
	enabled = e;
}

bool Toggle::wasClicked(int mx, int my) {
	return enabled && rect.getLeft() < mx && mx < rect.getRight() && rect.getTop() < my && my < rect.getBottom();
}

bool Toggle::wasPressed(char key) {
	return enabled && key == hotkey;
}

bool Toggle::isOn() {
	return on && enabled;
}

void Toggle::click() {
	on = !on;
	//printf("%s: I'm now %s\n", label.c_str(), on ? "on" : "off");
}

void Toggle::draw() {
	int bg, fg;
	if (!enabled) {
		bg = 128;
		fg = 0;
	}
	else if (on) {
		bg = 255;
		fg = 0;
	}
	else {
		bg = 0;
		fg = 255;
	}
	ofSetColor(bg);
	ofDrawRectangle(rect);
	ofSetColor(fg);
	font->drawString(label, fontPos.x, fontPos.y);
}