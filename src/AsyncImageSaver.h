#pragma once

#include "ofMain.h"

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

class AsyncImageSaver {
public:
  AsyncImageSaver(string _recordingPath) {
    recordingPath = _recordingPath;
    frames = new vector<Frame*>;
    frames->reserve(60);
  }

  ~AsyncImageSaver() {
    if (!detached) {
      for (auto &frame : *frames) {
        delete frame;
      }
      delete frames;
    }
  }

  void push(ofImage* image, int num);

  void save();

private:
  string recordingPath;
  vector<Frame*>* frames;
  bool detached = false;
};

