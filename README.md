# VirtualDeviceWriter
C++ dummy video writer by using V4l2loopback and OpenCV

# Dependencies
- OpenCV
- v4l2loopback

```bash
$ sudo apt install libopencv-dev
$ sudo apt install v4l2loopback-dkms
$ sudo modprobe v4l2loopback video_nr=42 exclusive_caps=1
```

# run example

```bash
$ git clone https://github.com/ysuzuki19/VirtualDeviceWriter
$ cd VirtualDeviceWriter/example
$ mkdir build
$ cd build
$ cmake ..
$ make
$ ./project
```
