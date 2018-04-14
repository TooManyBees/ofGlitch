#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main(int argc, char *argv[]){
	ofGLFWWindowSettings settings;
	settings.setGLVersion(3, 2);
	settings.width = 640;
	settings.height = 480;
	shared_ptr<ofAppBaseWindow> mainWindow = ofCreateWindow(settings);

	settings.width = 300;
	settings.height = 500;
	shared_ptr<ofAppBaseWindow> guiWindow = ofCreateWindow(settings);
	guiWindow->setVerticalSync(false);

	shared_ptr<ofApp> mainApp(new ofApp(mainWindow));
	mainApp->parseArgs(argc, argv);
	mainApp->setupGui();

	ofAddListener(guiWindow->events().draw, mainApp.get(), &ofApp::drawGui);
	ofAddListener(guiWindow->events().keyPressed, mainApp.get(), &ofApp::keyPressedInGui);

	ofRunApp(mainWindow, mainApp);
	ofRunMainLoop();
}
