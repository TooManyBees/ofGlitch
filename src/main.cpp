#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main(int argc, char *argv[]){
	ofGLFWWindowSettings settings;
	settings.setGLVersion(3, 2);
	settings.width = 640;
	settings.height = 480;
	ofCreateWindow(settings);
	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofApp* app = new ofApp();
	app->parseArgs(argc, argv);
	ofRunApp(app);
}
