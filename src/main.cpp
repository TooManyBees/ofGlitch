#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main(int argc, char *argv[]){
	ofGLFWWindowSettings settings;
	settings.setGLVersion(3, 2);
	settings.setSize(640, 480);
	shared_ptr<ofAppBaseWindow> mainWindow = ofCreateWindow(settings);

	settings.setSize(250, 400);
	//settings.decorated = false;
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
