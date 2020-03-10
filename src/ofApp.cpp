#include "ofApp.h"

void ofApp::parseArgs(int argc, char* argv[]) {
	for (int i = 0; i < argc; i++) {
		if (strcmp(argv[i], "--fullscreen") == 0) {
			startFullscreen = true;
		}
		else if (strcmp(argv[i], "--no-mirror") == 0) {
			mirror = false;
		}
		else if (strcmp(argv[i], "--depth") == 0) {
			if (i < argc-1 && strstr(argv[i+1], "-") == NULL) {
				int parsed;
				int result = sscanf(argv[i+1], "%d", &parsed);
				if (result) {
					backPlane = (float)min(10000, parsed);
				}
			}
		}
		else if (strcmp(argv[i], "--histogram") == 0) {
			useHistogram = true;
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
			cout << "--depth N\tSet max depth to some value N lower than 10000." << endl;
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
	if (oni_manager.setup(WIDTH, HEIGHT, 30, backPlane, mirror, useHistogram)) {
		cout << "Setup device and streams.\n" << endl;
	}
	else {
		cerr << openni::OpenNI::getExtendedError() << endl;
		return std::exit(1);
	}
	colorFrame.allocate(WIDTH, HEIGHT, OF_IMAGE_COLOR);
	depthFrame.allocate(WIDTH, HEIGHT, OF_IMAGE_GRAYSCALE);
	depthFrame.setColor(0);

	userFrame.allocate(WIDTH, HEIGHT, OF_IMAGE_GRAYSCALE);
	userFrame.setColor(0);

	canvas.allocate(WIDTH, HEIGHT, GL_RGBA);

	usermask.load("identity.vert", "usermask.frag");
#ifdef ENABLE_CHECKER
	checker.load("identity.vert", "check.frag");
#endif

	mainWindow->setFullscreen(startFullscreen);
	needsResize = true;
	ofBackground(0);

	glitchEffect.init(WIDTH, HEIGHT);
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

//    ofFloatColor color;
//    color.setHsb(ofRandom(1.0), 1.0, 1.0);
    ofFloatColor color = colorPicker.pick();
	glitchEffect.update(depthFrame, color, levelsRainbow);

	oni_manager.getUserFrame(&userFrame);

	if (needsResize) sizeProjectionSpace();

	if (recording) {
		numRecordingFrames += 1;
		if (numRecordingFrames > MAX_RECORDING_FRAMES) {
			endRecording();
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	if (showBuffer) {
		ofSetColor(255);
		glitchEffect.drawBuffer(projectionSpace);
		return;
	}

	canvas.begin();
	ofClear(0);

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
		glitchEffect.draw(canvasSpace);
		checker.end();
	}
#endif

	// Draw the glitch!
	if (showRainbows) {
		glitchEffect.draw(canvasSpace);
	}

	if (recording) {
#ifdef GIFS
		ofPixels* pixels = new ofPixels;
		pixels->allocate(WIDTH, HEIGHT, OF_IMAGE_COLOR_ALPHA);
		canvas.readToPixels(*pixels);
		imgSaver->push(pixels);
#else
		ofImage* img = new ofImage;
		img->setUseTexture(false);
		img->allocate(WIDTH, HEIGHT, OF_IMAGE_COLOR);
		canvas.readToPixels(img->getPixelsRef());
		imgSaver->push(img, ofGetFrameNum());
#endif
	}
	canvas.end();

	canvas.draw(projectionSpace);

	if (recording) {
		ofSetColor(255, 0, 0);
		ofDrawCircle(20, 20, 5);
		ofSetColor(255);
	}
}

void ofApp::startRecording() {
#ifdef GIFS
	string filename = ofGetTimestampString("%F_%H-%M-%S.gif");
	string path = ofFilePath::join("screenshots", filename);
	imgSaver = new GifSaver(WIDTH, HEIGHT, path);
#else
	string timestamp = ofGetTimestampString("%F_%H-%M-%S");
	string path = ofFilePath::addTrailingSlash(ofFilePath::join("screenshots", timestamp));
	ofFilePath::createEnclosingDirectory(path);
	imgSaver = new AsyncImageSaver(path);
#endif
	recording = true;
}

void ofApp::endRecording() {
	imgSaver->save();
	delete imgSaver;
	recording = false;
	numRecordingFrames = 0;
}

void ofApp::drawGui(ofEventArgs & args) {
	gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch (key) {
	case ' ':
		glitchEffect.markShadersDirty();
		{
			usermask.load("identity.vert", "usermask.frag");
			GLint err = glGetError();
			if (err != GL_NO_ERROR) {
				ofLogNotice() << "Shader 'usermask.frag' failed to compile:" << endl << err << endl;
			}
		}
		break;
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
			endRecording();
		} else {
			startRecording();
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

void ofApp::sizeProjectionSpace() {
	ofRectangle window = ofGetWindowRect();
	if (window.width > 0 && window.height > 0) {
		projectionSpace.scaleTo(window, OF_SCALEMODE_FIT);
		projectionSpace.alignTo(window, OF_ALIGN_HORZ_CENTER, OF_ALIGN_VERT_CENTER);
	}
	needsResize = false;
}

#ifdef ENABLE_CHECKER
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
#endif
