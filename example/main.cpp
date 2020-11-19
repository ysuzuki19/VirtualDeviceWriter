#include <opencv2/opencv.hpp>
#include <chrono>

#include "../VirtualDeviceWriter.hpp"

constexpr int DEVICE_ID_IN = 0;
constexpr int WIDTH = 640;
constexpr int HEIGHT = 480;

constexpr int DEVICE_ID_OUT = 42;

class EventFrequency {
  private:
    std::chrono::system_clock::time_point last_;
    double frequency_;
  public:
    EventFrequency () = default;
    void event () {
      frequency_ = std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::system_clock::now () - last_).count ();
      last_ = std::chrono::system_clock::now ();
    }
    float time () {
      return frequency_;
    }
};

std::string type2str(int type) {
  std::string r;

  uchar depth = type & CV_MAT_DEPTH_MASK;
  uchar chans = 1 + (type >> CV_CN_SHIFT);

  switch ( depth ) {
    case CV_8U:  r = "8U"; break;
    case CV_8S:  r = "8S"; break;
    case CV_16U: r = "16U"; break;
    case CV_16S: r = "16S"; break;
    case CV_32S: r = "32S"; break;
    case CV_32F: r = "32F"; break;
    case CV_64F: r = "64F"; break;
    default:     r = "User"; break;
  }

  r += "C";
  r += (chans+'0');

  return r;
}

int main (int argc, char* argv[])
{
  //cv::VideoCapture cap (DEVICE_ID_IN, cv::CAP_V4L);
  cv::VideoCapture cap (DEVICE_ID_IN);
  cap.set (cv::CAP_PROP_FRAME_WIDTH, WIDTH);
  cap.set (cv::CAP_PROP_FRAME_HEIGHT, HEIGHT);

  if (!cap.isOpened ()) {
    std::cout << "ERROR: input device was not opened" << std::endl;
    return -1;
  }

  cv::Mat frame;
  EventFrequency freq;
  VirtualDeviceWriter pub (DEVICE_ID_OUT, WIDTH, HEIGHT);

  while (cap.read (frame))
  {
    freq.event ();
    //std::cout << freq.time () << std::endl;

    cv::imshow ("Web Camera", frame);

    cv::Mat processed = frame;

    //cv::cvtColor (frame, processed, CV_8S);
    //pub.write (frame);
    pub.write (processed);

    const int key = cv::waitKey (1);
    if (key == 'q') {
      break;
    } else if (key == 's') {
      cv::imwrite ("img.image", frame);
    }
  }
  cv::destroyAllWindows ();
  return 0;
}
