#include "ofMain.h"

class Toggle {
public:
	Toggle(const std::string l, const char c, std::string d, ofPoint p, ofTrueTypeFont* f, bool startOn = false) :
		hotkey(c), on(startOn), enabled(true), rect(p, 20, 20), font(f)
	{
		label.assign(l);
		description.assign(d);

		float w = font->stringWidth(label);
		fontPos = ofPoint(
			rect.getCenter().x - w / 2.0,
			rect.getBottom() - 2.0
		);
	}

	void enableThisFrame(bool e);
	bool wasClicked(int mx, int my);
	bool wasPressed(char key);
	bool isOn();
	void click();
	void draw();

private:
	bool on;
	bool enabled;
	std::string label;
	char hotkey;
	std::string description;
	ofRectangle rect;
	ofTrueTypeFont* font;
	ofPoint fontPos;
};