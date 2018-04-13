#include "ofApp.h"

void ofApp::parseArgs(int argc, char* argv[]) {
	for (int i = 0; i < argc; i++) {
		if (strcmp(argv[i], "--fullscreen") == 0) {
			fullscreen = true;
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
	ofSetFrameRate(FPS);
	ofSetVerticalSync(true);
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
	checker.load("identity.vert", "check.frag");
	beglitch.load("identity.vert", "beglitch.frag");

	ofSetFullscreen(fullscreen);
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

	paramsChecker.setName("Checker pattern");
	paramsChecker.add(showChecker.set("Enabled", false));
	paramsChecker.add(timeCycleLength.set("Spin duration (ms)", 8000, 0, 20000));
	paramsChecker.add(timeCycleOffset.set("Spin offset (ms)", 0, 0, 20000));
	paramsChecker.add(beatCycleLength.set("Beat duration (ms)", 2000, 0, 20000));
	paramsChecker.add(beatCycleOffset.set("Beat offset (ms)", 0, 0, 20000));

	gui.setup();
	gui.add(paramsLayers);
	gui.add(paramsLevels);
	gui.add(paramsChecker);
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

	//updateUi();

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

	if (showChecker) {
		checker.begin();
		checker.setUniform1f("outside", 1.0);
		checker.setUniform1f("timeCycle", cycle(timeCycleLength, timeCycleOffset));
		checker.setUniform1f("beatCycle", cycle(beatCycleLength, beatCycleOffset));
		checker.setUniform1f("size", 10);
		checker.setUniform2f("resolution", (float)ofGetWidth(), (float)ofGetHeight());
		checker.setUniformTexture("usermask", userFrame.getTexture(), 1);
		glitchBuffer.draw(canvasSpace);
		checker.end();
	}

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
		fullscreen = !fullscreen;
		ofSetFullscreen(fullscreen);
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
	case 'c':
		showChecker = !showChecker;
		break;
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
	// Figure out where to draw the image on screen
	float scaleWidthRatio = (float)ofGetWidth() / (float)WIDTH;
	float scaleHeightRatio = (float)ofGetHeight() / (float)HEIGHT;
	float posX, posY, scaleHeight, scaleWidth;
	if (HEIGHT * scaleWidthRatio <= ofGetHeight()) {
		// if scaling to max still fits the height
		scaleWidth = ofGetWidth();
		scaleHeight = HEIGHT * scaleWidthRatio;
		posX = 0;
		posY = (ofGetHeight() - scaleHeight) / 2;
	}
	else {
		scaleWidth = WIDTH * scaleHeightRatio;
		scaleHeight = ofGetHeight();
		posX = (ofGetWidth() - scaleWidth) / 2;
		posY = 0;
	}
	canvasSpace.x = posX;
	canvasSpace.y = posY;
	canvasSpace.width = scaleWidth;
	canvasSpace.height = scaleHeight;
	printf("Drawing at (%.2f, %.2f): %.2f x %.2f\n", canvasSpace.x, canvasSpace.y, canvasSpace.width, canvasSpace.height);
	needsResize = false;
}

float ofApp::cycle(size_t length, size_t offset) {
	float fLength = (float)length;
	float fCycle = (float)(ofGetElapsedTimeMillis() % length + offset);
	return fCycle / fLength;
}
