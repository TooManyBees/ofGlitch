# i named this poorly *again*

A reimplimentation of [glitch](https://github.com/TooManyBees/glitch) written in
[openFrameworks](https://openframeworks.cc) instead of Processing.

It's significantly faster, but that's more because this version uses shaders to
compose the effects rather than arrays of RGB values. (For the life of me I
can not make sense of Processing's shader API, and I'm okay never knowing.)

Built with Visual Studio 2015.

## Setup

Get yourself a camera with a Primesense Carmine (PS1080) SoC (like the *Asus
Xtion* or *XBox Kinect*) and install OpenNI2 and NiTE2. NiTE2 is tragically
proprietary, so it can't be built for targets it doesn't already exist for, like
ARM processors.

Copy three external files/folders into the project's `bin` folder:

* `OpenNI2.dll` from `$openni2_location/Redist`
* `NiTE2.dll` from `$nite2_location/Redist`
* the `NiTE2` folder from `$nite2_location/Redist/NiTE2`

## Usage

Starting the app brings up the output window as well as a second smaller window
with controls on it for tweaking settings. The control window can be closed
without breaking the appm, and all of the controls have their own hotkeys.

### Layers
* Show Video (**V**) Toggles color video stream. When off, the background is
  black.
* Threshold Video (**T**) Applies a threshold to the color video to reduce it to
  single-bit black and white color. Additionally, masks the color stream to
  only display detected users.
* Show Rainbows (**R**) Toggles the rainbows. (But why 🌈)
* Show Buffer (**B**) Instead of displaying the composited video result, display
  only the contents of the buffer that stores the "glitch" effect.

### Levels
* Rainbows (**Up Arrow**, **Down Arrow**) adjusts the strength of the rainbow
  effect.
* Threshold (**Left Arrow**, **Right Arrow**) adjusts the threshold boundary.

### Misc (these don't have their own GUI controls)

* Fullscreen (**F**) Toggles the video window in and out of fullscreen.
* Record (**Return**) Toggles recording frames as bitmaps. Each recording
  "session" has its own separate folder.
