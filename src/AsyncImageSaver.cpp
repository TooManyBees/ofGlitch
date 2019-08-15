#include "AsyncImageSaver.h"

void writeAll(string recordingPath, vector<Frame*>* frames) {
  for (auto &frame : *frames) {
    string recordingFilename = ofFilePath::join(recordingPath, ofToString(frame->frameNumber) + ".bmp");
    frame->save(recordingFilename);
    delete frame;
  }
  delete frames;
}

void AsyncImageSaver::push(ofImage* image, int num) {
  frames->push_back(new Frame(image, num));
}

void AsyncImageSaver::save() {
  std::thread t(writeAll, recordingPath, frames);
  t.detach();
  detached = true;
}
