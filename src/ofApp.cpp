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
	beglitch.load("identity.vert", "beglitch.frag");

	uiFont.load("AnonymousProBold.ttf", 18) || uiFont.load(OF_TTF_MONO, 18);
	statsFont.load("AnonymousPro.ttf", 12) || statsFont.load(OF_TTF_MONO, 12);
	if (!uiFont.isLoaded() || !statsFont.isLoaded()) {
		cerr << "Couldn't load fonts." << endl;
		return std::exit(1);
	}

	toggleBuffer = new Toggle("B", 'b', "draw buffer", ofPoint(10, 10), &uiFont);
	toggleVideo = new Toggle("V", 'v', "draw video", ofPoint(10, 35), &uiFont, true);
	toggleThreshold = new Toggle("T", 't', "threshold video", ofPoint(10, 60), &uiFont, true);
	toggleRainbows = new Toggle("R", 'r', "rainbows", ofPoint(10, 85), &uiFont, true);

	ui.push_back(toggleBuffer);
	ui.push_back(toggleVideo);
	ui.push_back(toggleThreshold);
	ui.push_back(toggleRainbows);

	ofSetFullscreen(fullscreen);
	videoThreshold = 0.35;
	rainbowThreshold = 0.6;
	needsResize = true;
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

	updateUi();

	if (needsResize) sizeCanvasSpace();
}

//--------------------------------------------------------------
void ofApp::draw(){
	if (toggleBuffer->isOn()) {
		ofSetColor(255);
		glitchBuffer.draw(canvasSpace);
		if (displayUi) drawUi();
		return;
	}

	ofBackground(0);

	if (toggleVideo->isOn()) {
		// Draw the color frame, optionally masked and thresholded
		if (toggleThreshold->isOn()) {
			usermask.begin();
			usermask.setUniformTexture("usermask", userFrame.getTexture(), 1);
			usermask.setUniform1f("threshold", videoThreshold);
		}
		colorFrame.draw(canvasSpace);
		if (toggleThreshold->isOn()) usermask.end();
	}

	// Draw the glitch!
	if (toggleRainbows->isOn()) {
		beglitch.begin();
		beglitch.setUniform1f("threshold", rainbowThreshold);
		ofFloatColor color;
		color.setHsb(ofRandom(1.0), 1.0, 1.0);
		beglitch.setUniform3f("color", color.r, color.g, color.b);
		glitchBuffer.draw(canvasSpace);
		beglitch.end();
	}

	if (displayUi) drawUi();
}

void ofApp::updateUi() {
	toggleVideo->enableThisFrame(!toggleBuffer->isOn());
	toggleThreshold->enableThisFrame(toggleVideo->isOn());
	toggleRainbows->enableThisFrame(!toggleBuffer->isOn());
}

void ofApp::drawUi() {
	for (Toggle* elem : ui) {
		elem->draw();
	}
	ofSetColor(255);
	sprintf(statsString, "Rainbows: %.2f\nFaces: %.2f", rainbowThreshold, videoThreshold);
	statsFont.drawString(statsString, 5.0, ofGetHeight() - statsFont.stringHeight(statsString));
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch (key) {
	case 'u':
		displayUi = !displayUi;
		break;
	case 'f':
		fullscreen = !fullscreen;
		ofSetFullscreen(fullscreen);
		break;
	case OF_KEY_UP:
		rainbowThreshold = ofClamp(rainbowThreshold - 0.05, 0.0, 1.0);
		break;
	case OF_KEY_DOWN:
		rainbowThreshold = ofClamp(rainbowThreshold + 0.05, 0.0, 1.0);
		break;
	case OF_KEY_LEFT:
		videoThreshold = ofClamp(videoThreshold + 0.025, 0.0, 1.0);
		break;
	case OF_KEY_RIGHT:
		videoThreshold = ofClamp(videoThreshold - 0.025, 0.0, 1.0);
		break;
	default:
		for (Toggle* elem : ui) {
			if (elem->wasPressed(key)) {
				elem->click();
				return;
			}
		}
	}
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
	for (Toggle* elem : ui) {
		if (elem->wasClicked(x, y)) {
			elem->click();
			return;
		}
	}
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