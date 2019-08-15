#include "ofApp.h"

void ofApp::parseArgs(int argc, char* argv[]) {
	for (int i = 0; i < argc; i++) {
		if (strcmp(argv[i], "--fullscreen") == 0) {
			startFullscreen = true;
		}
		else if (strcmp(argv[i], "--no-mirror") == 0) {
			mirror = false;
		}
		else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
#ifdef WIN32
			char* term = strrchr(argv[0], '\\');
#else
			char* term = strrchr(argv[0], '/');
#endif
			if (term == NULL) term = argv[0];
			cout << "Usage: " << term+1 << " [options]" << endl;
			cout << "Options:" << endl;
			cout << "--help, -h\tPrint this message and exit." << endl;
			cout << "--fullscreen\tStart application in full screen." << endl;
			cout << "--no-mirror\tTurn off mirroring." << endl;
			cout << "--vampire\tHack to make vampires appear on screen (may impact performance).";
			std::exit(0);
		}
	}
}

//--------------------------------------------------------------
void ofApp::setup(){
	mainWindow->setVerticalSync(true);
	ofSetFrameRate(FPS);
	if (oni_manager.setup(WIDTH, HEIGHT, FPS, mirror)) {
		cout << "Setup device and streams.\n" << endl;
	}
	else {
		cerr << openni::OpenNI::getExtendedError() << endl;
		return std::exit(1);
	}
	colorFrame.allocate(WIDTH, HEIGHT, OF_IMAGE_COLOR);
	depthFrame.allocate(WIDTH, HEIGHT, OF_IMAGE_GRAYSCALE);
	depthFrame.setColor(0);

	glitchBuffer.allocate(WIDTH, HEIGHT, OF_IMAGE_GRAYSCALE);
	glitchBuffer.setColor(0);

	userFrame.allocate(WIDTH, HEIGHT, OF_IMAGE_GRAYSCALE);
	userFrame.setColor(0);

	usermask.load("identity.vert", "usermask.frag");
#ifdef ENABLE_CHECKER
	checker.load("identity.vert", "check.frag");
#endif
	beglitch.load("identity.vert", "beglitch.frag");

	mainWindow->setFullscreen(startFullscreen);
	needsResize = true;
}

void ofApp::setupGui() {
	paramsLayers.setName("Layers");
	paramsLayers.add(showBuffer.set("Show buffer", false));
	paramsLayers.add(showVideo.set("Show video", true));
	paramsLayers.add(showThreshold.set("Threshold video", true));
	paramsLayers.add(showRainbows.set("Show Rainbows", true));

	paramsLevels.setName("Levels");
	paramsLevels.add(levelsRainbow.set("Rainbows", 0.6, 0.0, 1.0));
	paramsLevels.add(levelsThreshold.set("Threshold", 0.35, 0.0, 1.0));

#ifdef ENABLE_CHECKER
	paramsChecker.setName("Checker pattern");
	paramsChecker.add(checkerEnabled.set("Enabled", false));
	paramsChecker.add(checkerOutside.set("Paint outside users", true));
	paramsChecker.add(checkerRPM.set("RPM", 3.0, 0.0, 25.0));
	paramsChecker.add(checkerRevolutionOffset.set("Rev offset", 0.0, 0.0, 2.0));
	paramsChecker.add(checkerAmplitude.set("Beat Amplitude", 0.0, 0.0, 5.0));
	paramsChecker.add(checkerBPM.set("BPM", 130.0, 0.0, 200.0));
	paramsChecker.add(checkerBeatOffset.set("Beat offset", 0.0, 0.0, 10.0));
#endif

	gui.setup();
	gui.add(paramsLayers);
	gui.add(paramsLevels);
#ifdef ENABLE_CHECKER
	gui.add(paramsChecker);
#endif

	recording = false;
}

unsigned char mysteryDiff(unsigned char a, unsigned char b) {
	unsigned char c = a > b ? a - b : b - a;
	return c > (b / 2) ? c - 1 : c;
}

unsigned char funDiff(int a, int b) {
	int c = a > b ? a - b : b - a;
	c = a - (((c - a) * 255) >> 8);
	return c;
}

unsigned char underflowDiff(unsigned char a, unsigned char b) {
	unsigned int c = a > b ? a - b : b - a;
	return c - 1; // underflow intended
}

//--------------------------------------------------------------
void ofApp::update(){
	oni_manager.getColorFrame(&colorFrame);
	oni_manager.getDepthFrame(&depthFrame);

	ofPixels & pGlitch = glitchBuffer.getPixels();
	const ofPixels pDepth = depthFrame.getPixels();
	for (int i = 0; i < WIDTH * HEIGHT; i++) {
		unsigned char dd = mysteryDiff(pGlitch[i], pDepth[i]);
		pGlitch[i] = dd;
	}
	glitchBuffer.update();
	oni_manager.getUserFrame(&userFrame);

	if (needsResize) sizeCanvasSpace();
}

//--------------------------------------------------------------
void ofApp::draw(){
	if (showBuffer) {
		ofSetColor(255);
		glitchBuffer.draw(canvasSpace);
		return;
	}

	ofBackground(0);

	if (showVideo) {
		// Draw the color frame, optionally masked and thresholded
		if (showThreshold) {
			usermask.begin();
			usermask.setUniformTexture("usermask", userFrame.getTexture(), 1);
			usermask.setUniform1f("threshold", levelsThreshold);
		}
		colorFrame.draw(canvasSpace);
		if (showThreshold) usermask.end();
	}

#ifdef ENABLE_CHECKER
	if (checkerEnabled) {
		checker.begin();
		checker.setUniform1f("outside", checkerOutside ? 1.0 : 0.0);
		checker.setUniform1f("timeCycle", cyclePerMinute(checkerRPM, checkerRevolutionOffset));
		checker.setUniform1f("beatCycle", cyclePerMinute(checkerBPM, checkerBeatOffset));
		checker.setUniform1f("size", 10);
		checker.setUniform1f("amplitude", checkerAmplitude);
		checker.setUniform2f("resolution", WIDTH, HEIGHT);
		checker.setUniformTexture("usermask", userFrame.getTexture(), 1);
		glitchBuffer.draw(canvasSpace);
		checker.end();
	}
#endif

	// Draw the glitch!
	if (showRainbows) {
		beglitch.begin();
		beglitch.setUniform1f("threshold", levelsRainbow);
		ofFloatColor color;
		color.setHsb(ofRandom(1.0), 1.0, 1.0);
		beglitch.setUniform3f("color", color.r, color.g, color.b);
		glitchBuffer.draw(canvasSpace);
		beglitch.end();
	}

	if (recording) {
		// string recordingFilename = ofFilePath::join(recordingPath, ofToString(ofGetFrameNum()) + ".bmp");
		// ofSaveScreen(recordingFilename);
		ofImage* img = new ofImage;
		img->allocate(WIDTH, HEIGHT, OF_IMAGE_COLOR);
		img->grabScreen(0, 0, WIDTH, HEIGHT);
		imgSaver->push(img, ofGetFrameNum());
	}
}

void ofApp::drawGui(ofEventArgs & args) {
	gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch (key) {
	case 'u':
		// if ui window closed, reopen and reattach listeners
		break;
	case 'f':
		mainWindow->toggleFullscreen();
		break;
	case 'b':
		showBuffer = !showBuffer;
		break;
	case 'v':
		showVideo = !showVideo;
		break;
	case 't':
		showThreshold = !showThreshold;
		break;
#ifdef ENABLE_CHECKER
	case 'c':
		checkerEnabled = !checkerEnabled;
		break;
#endif
	case 'r':
		showRainbows = !showRainbows;
		break;
	case OF_KEY_UP:
		levelsRainbow -= 0.05;
		break;
	case OF_KEY_DOWN:
		levelsRainbow += 0.05;
		break;
	case OF_KEY_LEFT:
		levelsThreshold += 0.025;
		break;
	case OF_KEY_RIGHT:
		levelsThreshold -= 0.025;
		break;
	case OF_KEY_RETURN:
		if (recording) {
			imgSaver->save();
			delete imgSaver;
			recording = false;
		} else {
			string timestamp = ofGetTimestampString("%F_%H-%M-%S");
			string path = ofFilePath::addTrailingSlash(ofFilePath::join("screenshots", timestamp));
			ofFilePath::createEnclosingDirectory(path);
			imgSaver = new Saver(path);
			recording = true;
		}
		break;
	}
}

void ofApp::keyPressedInGui(ofKeyEventArgs & args) {
	keyPressed(args.key);
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
	needsResize = true;
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

void ofApp::sizeCanvasSpace() {
	canvasSpace.scaleTo(ofGetWindowRect(), OF_SCALEMODE_FIT);
	canvasSpace.alignTo(ofGetWindowRect(), OF_ALIGN_HORZ_CENTER, OF_ALIGN_VERT_CENTER);
	needsResize = false;
}

float ofApp::cycle(size_t length, size_t offset) {
	if (length == 0) return 0.0;
	float fLength = (float)length;
	float fCycle = (float)((ofGetElapsedTimeMillis() + offset) % length);
	return fCycle / fLength;
}

float ofApp::cyclePerMinute(float rpm, float offset) {
	if (rpm == 0.0) return 0.0;
	float length = 60.0 * 1000.0 / rpm;
	size_t offsetMillis = (size_t)(offset * 1000);
	float fCycle = (float)((ofGetElapsedTimeMillis() + offsetMillis) % (size_t)length);
	return fCycle / length;
}