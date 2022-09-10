#pragma once

#include "ofMain.h"
#include "ofxMidi.h"
#include "OpenNI.h"
#include "OniManager.h"
#include "ofxGui.h"
#include "AsyncImageSaver.h"
#include "GifSaver.h"
#include "GlitchEffect.h"
#include "RainbowClassic.h"
#include "RainbowNew.h"

#define GIFS

enum Levels {
	Rainbow,
	Threshold,
	User,
	Expansion,
	ExpansionX,
	ExpansionY,
};

struct DisplayParam {
	Levels param;
	uint64_t frameNumber;
};

class ofApp : public ofBaseApp, public ofxMidiListener {

	public:
		ofApp(shared_ptr<ofAppBaseWindow> _mainWindow, shared_ptr<ofAppBaseWindow> _guiWindow) : mainWindow(_mainWindow), guiWindow(_guiWindow) {};
		void parseArgs(int argc, char* argv[]);
		void setup();
		void update();
		void draw();
		void exit();

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
		static const int FPS = 20;

		void setupGui();
		void drawGui(ofEventArgs & args);
		void keyPressedInGui(ofKeyEventArgs & args);

		void drawTempGui();

		void quit(ofEventArgs & args);

		void newMidiMessage(ofxMidiMessage& eventArgs);
		bool midiConnected = false;
		ofxMidiIn midiIn;

	private:
		shared_ptr<ofAppBaseWindow> mainWindow;
		shared_ptr<ofAppBaseWindow> guiWindow;
		bool mirror = true;
		bool startFullscreen = false;
		float backPlane = 10000;
		bool useHistogram = false;
		OniManager oni_manager;

		ofFbo canvas;
		ofRectangle canvasSpace = ofRectangle(0, 0, WIDTH, HEIGHT);
		ofRectangle projectionSpace = ofRectangle(0, 0, WIDTH, HEIGHT);

		ofImage colorFrame;
		ofImage depthFrame;
		ofImage userFrame;

		GlitchEffect* glitchEffect;
		size_t glitchIndex = 0;
		vector<GlitchEffect*> glitches;
		void reloadShaders();

#ifdef ENABLE_CHECKER
		ofShader checker;
#endif
		ofShader usermask;
		size_t timeCycle;
		size_t timeOffset;
		size_t beatCycle;
		size_t beatOffset;
		float cycle(size_t length, size_t offset);
		float cyclePerMinute(float rpm, float offset);

		bool needsResize;
		void sizeProjectionSpace();

		bool recording;
		string recordingPath;
		static const unsigned int MAX_RECORDING_FRAMES = 20 * FPS;
		unsigned int numRecordingFrames = 0;
#ifdef GIFS
		GifSaver* imgSaver;
#else
		AsyncImageSaver* imgSaver;
#endif
		void startRecording();
		void endRecording();

		ofParameterGroup paramsLayers;
		ofParameter<bool> showBuffer;
		ofParameter<bool> showVideo;
		ofParameter<bool> showThreshold;
		ofParameter<bool> showRainbows;
		ofParameterGroup paramsLevels;
		ofParameter<float> levelsRainbow;
		ofParameter<float> levelsThreshold;
		ofParameter<float> levelsUser;
		ofParameterGroup paramsExpansion;
		ofParameter<float> levelsExpansion;
		ofParameter<float> levelsExpansionX;
		ofParameter<float> levelsExpansionY;
#ifdef ENABLE_CHECKER
		ofParameterGroup paramsChecker;
		ofParameter<bool> checkerEnabled;
		ofParameter<bool> checkerOutside;
		ofParameter<float> checkerAmplitude;
		ofParameter<float> checkerRPM;
		ofParameter<float> checkerRevolutionOffset;
		ofParameter<float> checkerBPM;
		ofParameter<float> checkerBeatOffset;
#endif
		ofxGuiGroup gui;
		ofxGuiGroup tempGui;
		vector<DisplayParam> renderParamsThisFrame;
		void renderParamThisFrame(Levels param);
};
