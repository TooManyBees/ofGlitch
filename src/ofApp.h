#pragma once

#include "ofMain.h"
#include "openni.h"
#include "OniManager.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp{

	public:
		void parseArgs(int argc, char* argv[]);
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		static const int WIDTH = 640;
		static const int HEIGHT = 480;
		static const int FPS = 30;

		void setupGui();
		void drawGui(ofEventArgs & args);
		void keyPressedInGui(ofKeyEventArgs & args);

	private:
		bool mirror = true;
		bool fullscreen = false;
		OniManager oni_manager;

		ofRectangle canvasSpace;

		ofImage colorFrame;
		ofImage depthFrame;
		ofImage userFrame;
		ofImage glitchBuffer;

		ofShader beglitch;
		ofShader checker;
		ofShader usermask;
		size_t timeCycle;
		size_t timeOffset;
		size_t beatCycle;
		size_t beatOffset;
		float cycle(size_t length, size_t offset);

		bool needsResize;
		void sizeCanvasSpace();

		ofParameterGroup paramsLayers;
		ofParameter<bool> showBuffer;
		ofParameter<bool> showVideo;
		ofParameter<bool> showThreshold;
		ofParameter<bool> showRainbows;
		ofParameterGroup paramsLevels;
		ofParameter<float> levelsRainbow;
		ofParameter<float> levelsThreshold;
		ofParameterGroup paramsChecker;
		ofParameter<bool> showChecker;
		ofParameter<size_t> timeCycleLength;
		ofParameter<size_t> timeCycleOffset;
		ofParameter<size_t> beatCycleLength;
		ofParameter<size_t> beatCycleOffset;
		ofxGuiGroup gui;
};
