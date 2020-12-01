#ifndef VIRTUAL_DEVICE_WRITER
#define VIRTUAL_DEVICE_WRITER

#pragma once
#include <opencv2/opencv.hpp>

#include <chrono>

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>

class VirtualDeviceWriter {
  private:
    int fdwr_;
    size_t frame_size_;
    struct v4l2_capability vid_caps_;
    struct v4l2_format vid_format_;

    std::vector<unsigned char> jpeg_img_;

    void setDevice (int device_id) {
      std::string dev_name = "/dev/video" + std::to_string (device_id);

      fdwr_ = ::open (dev_name.c_str (), O_RDWR);
      if(fdwr_ < 0) {
        std::cerr << "ERROR: could not open output device!\n" << ::strerror(errno);
      }

      if (::ioctl (fdwr_, VIDIOC_QUERYCAP, &vid_caps_) < 0) {
        std::cerr << "ERROR: unable to get video format!\n" << ::strerror(errno);
      }

      ::memset (&vid_format_, 0, sizeof(vid_format_));

      //if (::ioctl (fdwr_, VIDIOC_G_FMT, &vid_format_) < 0) {
        //std::cerr << "ERROR: unable to get video format!\n" << ::strerror(errno);
      //}
    }

    void setSize (std::size_t width, std::size_t height) {
      frame_size_ = width * height * 3;
      int linewidth = width * 3;

      vid_format_.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
      vid_format_.fmt.pix.width = width;
      vid_format_.fmt.pix.height = height;
      vid_format_.fmt.pix.sizeimage = frame_size_;
      vid_format_.fmt.pix.bytesperline = linewidth;
      vid_format_.fmt.pix.pixelformat = V4L2_PIX_FMT_JPEG;
      vid_format_.fmt.pix.colorspace = V4L2_COLORSPACE_JPEG;
      vid_format_.fmt.pix.field = V4L2_FIELD_NONE;

      if (::ioctl(fdwr_, VIDIOC_S_FMT, &vid_format_) < 0) {
        std::cerr << "ERROR: unable to set video format!\n" << ::strerror(errno);
      }
    }

    void init (int device_id, std::size_t width, std::size_t height) {
      this->setDevice (device_id);
      this->setSize (width, height);
    }

  public:
    VirtualDeviceWriter (int device_id, std::size_t width, std::size_t height) {
      this->init (device_id, width, height);
    }

    void write (cv::Mat & img) {
      //FIXME:
      //cv::cvtColor (img, img, cv::COLOR_BGR2RGB);
      //cv::cvtColor (img, img, cv::COLOR_BGR2RGB);

      cv::imencode (".jpeg", img, jpeg_img_);

      size_t written = ::write(fdwr_, jpeg_img_.data (), frame_size_);

      if (written < 0) {
        std::cerr << "ERROR: could not write to output device!\n";
        ::close(fdwr_);
      }
    }
};

#endif
