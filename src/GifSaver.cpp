#include "GifSaver.h";

void GifSaver::push(ofPixels* pixels) {
	frames->push_back(pixels);
}

void processFrame(unsigned char* frame, unsigned int width, unsigned int height, FIMULTIBITMAP *multi) {
	// FIXME: I'm ignoring the whole little-endian bs because this is just for my own hardware
	FIBITMAP* bitmap = FreeImage_ConvertFromRawBits(frame, width, height, width * 4, 32, 0, 0, 0, true);
	FIBITMAP* quantizedBitmap = FreeImage_ColorQuantizeEx(bitmap, FIQ_WUQUANT);

	FreeImage_SetMetadata(FIMD_ANIMATION, quantizedBitmap, NULL, NULL);
	FITAG *tag = FreeImage_CreateTag();
	DWORD frameDuration = (DWORD)(1000.f / 30.f);
	if (tag) {
		FreeImage_SetTagKey(tag, "FrameTime");
		FreeImage_SetTagType(tag, FIDT_LONG);
		FreeImage_SetTagCount(tag, 1);
		FreeImage_SetTagLength(tag, 4);
		FreeImage_SetTagValue(tag, &frameDuration);
		FreeImage_SetMetadata(FIMD_ANIMATION, quantizedBitmap, FreeImage_GetTagKey(tag), tag);
		FreeImage_DeleteTag(tag);
	}

	FreeImage_AppendPage(multi, quantizedBitmap);
	if (bitmap != NULL) FreeImage_Unload(bitmap);
	if (quantizedBitmap != NULL) FreeImage_Unload(quantizedBitmap);
	delete frame;
}

void asyncSave(vector<ofPixels*>* frames, unsigned int width, unsigned int height, string path) {
	string ppath = ofToDataPath(path);
	cout << ppath << endl;
	FIMULTIBITMAP *multi = FreeImage_OpenMultiBitmap(FIF_GIF, ppath.c_str(), TRUE, FALSE);
	for (auto frame : *frames) {
		processFrame(frame->getData(), width, height, multi);
	}

	FreeImage_CloseMultiBitmap(multi);
}

void GifSaver::save() {
	std::thread t(asyncSave, frames, width, height, recordingPath);
	t.detach();
	detached = true;
}