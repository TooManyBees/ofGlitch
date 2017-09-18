#include "OniManager.h"

//OniManager::~OniManager() {
//	if (colorStream.isValid()) colorStream.destroy();
//	if (depthStream.isValid()) depthStream.destroy();
//	if (userTracker.isValid()) userTracker.destroy();
//	if (device.isValid()) device.close();
//
//	//nite::NiTE::shutdown();
//	openni::OpenNI::shutdown();
//}

bool OniManager::setup(int w, int h, int fps, bool mirror) {
	//colorFrameTimestamp = 0;
	//userFrameTimestamp = 0;

	openni::Status ok = openni::STATUS_OK;
	if (openni::OpenNI::initialize() != ok) return false;

	if (device.open(openni::ANY_DEVICE) != ok) return false;

	if (depthStream.create(device, openni::SENSOR_DEPTH) == ok) {
		openni::VideoMode vm = depthStream.getVideoMode();
		vm.setFps(fps);
		vm.setResolution(w, h);
		if (depthStream.setVideoMode(vm) != ok) return false;
		if (depthStream.setMirroringEnabled(mirror) != ok) return false;
	}
	else return false;

	if (nite::NiTE::initialize() != ok) return false;
	if (userTracker.create(&device) != ok) return false;

	if (colorStream.create(device, openni::SENSOR_COLOR) == ok) {
		openni::VideoMode vm = colorStream.getVideoMode();
		vm.setFps(fps);
		vm.setResolution(w, h);
		colorStream.setVideoMode(vm);
		if (colorStream.setMirroringEnabled(mirror) != ok) return false;
		if (colorStream.start() != ok) return false;
	}
	else return false;

	device.setImageRegistrationMode(openni::IMAGE_REGISTRATION_DEPTH_TO_COLOR);

	pDepthMap = new unsigned char[w * h];

	return true;
}

void OniManager::getDepthFrame(ofImage* image) {
	userTracker.readFrame(&userFrame);
	if (!userFrame.isValid()) {
		printf("err :(");
		return;
	}
	openni::VideoFrameRef depthFrame = userFrame.getDepthFrame();

	if (depthFrame.isValid()) {
		histogram(depthHistogram, depthFrame);
		int frameWidth = depthFrame.getWidth();
		int frameHeight = depthFrame.getHeight();
		memset(pDepthMap, 0, frameWidth * frameHeight);

		const openni::DepthPixel* pDepthRow = (const openni::DepthPixel*)depthFrame.getData();
		unsigned char* pMap = pDepthMap + depthFrame.getCropOriginY() * frameWidth;
		int rowSize = depthFrame.getStrideInBytes() / sizeof(openni::DepthPixel);

		for (int y = 0; y < frameHeight; ++y)
		{
			const openni::DepthPixel* pDepth = pDepthRow;
			unsigned char* pTex = pMap + depthFrame.getCropOriginX();

			for (int x = 0; x < frameWidth; ++x, ++pDepth, ++pTex)
			{
				if (*pDepth != 0)
				{
					int nHistValue = depthHistogram[*pDepth];
					*pTex = nHistValue;
				}
			}

			pDepthRow += rowSize;
			pMap += 640;
		}

		image->setFromPixels(pDepthMap, frameWidth, frameHeight, OF_IMAGE_GRAYSCALE);
	}
}

//bool OniManager::updateUserFrame() {
//	userTracker.readFrame(&userFrame);
//	uint64_t newTimestamp = userFrame.getTimestamp();
//	if (userFrame.isValid() && newTimestamp != userFrameTimestamp) {
//		userFrameTimestamp = newTimestamp;
//      return true;
//	}
//  return false;
//}

void OniManager::getColorFrame(ofImage* image) {
	colorStream.readFrame(&colorFrame);
	//updateColorFrame();
	if (colorFrame.isValid()) {
		const openni::RGB888Pixel* pColor = (const openni::RGB888Pixel*)colorFrame.getData();
		image->setFromPixels((const unsigned char*)pColor, colorFrame.getWidth(), colorFrame.getHeight(), OF_IMAGE_COLOR);
	}
}

//bool OniManager::updateColorFrame() {
//	colorStream.readFrame(&colorFrame);
//	uint64_t newTimestamp = colorFrame.getTimestamp();
//	if (colorFrame.isValid() && newTimestamp != colorFrameTimestamp) {
//		colorFrameTimestamp = newTimestamp;
//	}
//}

void OniManager::histogram(float *pHistogram, openni::VideoFrameRef& frame) {
	memset(pHistogram, 0, MAX_DEPTH * sizeof(float));
	const openni::DepthPixel* pDepth = (const openni::DepthPixel*)frame.getData();
	int restOfRow = frame.getStrideInBytes() / sizeof(openni::DepthPixel) - frame.getWidth();
	int height = frame.getHeight();
	int width = frame.getWidth();

	unsigned int nNumberOfPoints = 0;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++, pDepth++) {
			if (*pDepth != 0) {
				pHistogram[*pDepth]++;
				nNumberOfPoints++;
			}

		}
		pDepth += restOfRow;
	}

	for (int nIndex = 1; nIndex < MAX_DEPTH; nIndex++) {
		pHistogram[nIndex] += pHistogram[nIndex - 1];
	}
	if (nNumberOfPoints) {
		for (int nIndex = 1; nIndex < MAX_DEPTH; nIndex++) {
			pHistogram[nIndex] = (float)(unsigned int)(256 * (1.0f - (pHistogram[nIndex] / nNumberOfPoints)));
		}
	}
}

void OniManager::getUserMask(bool *users) {
	if (userFrame.isValid()) {
		nite::UserMap userMap = userFrame.getUserMap();
		int length = userMap.getWidth() * userMap.getHeight();
		const nite::UserId* px = userMap.getPixels();
		for (int i = 0; i < length; i++) {
			users[i] = px[i] > 0;
		}
	}
}

void OniManager::getUserFrame(ofImage* image) {
	if (userFrame.isValid()) {
		nite::UserMap userMap = userFrame.getUserMap();
		int length = userMap.getWidth() * userMap.getHeight();
		const short* userPx = userMap.getPixels();
		ofPixels & pixels = image->getPixelsRef();
		for (int i = 0; i < length; i++) {
			pixels[i] = userPx[i] > 0 ? 255 : 0;
		}
		image->setFromPixels(pixels);
	}
}
