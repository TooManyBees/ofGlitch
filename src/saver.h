#pragma once

#include "ofMain.h"
#include <thread>

class Frame {
public:
  Frame(ofImage* img, int num): image(img), frameNumber(num) {};
  ~Frame() {
    delete image;
  }
  void save(string name) {
    image->save(name);
  }
  ofImage* image;
  int frameNumber;
};

void writeAll(string recordingPath, vector<Frame>* frames) {
  for (auto frame : *frames) {
    string recordingFilename = ofFilePath::join(recordingPath, ofToString(frame.frameNumber) + ".bmp");
    frame.save(recordingFilename);
  }
  delete frames;
}

class Saver {
public:
  Saver(string _recordingPath) {
    recordingPath = _recordingPath;
    frames = new std::vector<Frame>;
    frames->reserve(60);
  }

  ~Saver() {
    if (!detached) {
      delete frames;
    }
  }

  void push(ofImage* image, int num) {
    frames->push_back(Frame(image, num));
  }
  void save() {
    std::thread t(writeAll, recordingPath, frames);
    t.detach();
    detached = true;
  }

private:
  string recordingPath;
  vector<Frame>* frames;
  bool detached = false;
};

