#include <cstring>
#include <string>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <ctime>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <chrono>
#include "config.h"

using namespace std;
using namespace cv;

Config conf;

template <typename T>
class Queue
{
 public:
 
  T pop()
  {
    std::unique_lock<std::mutex> mlock(mutex_);
    while (queue_.empty())
    {
      cond_.wait(mlock);
    }
    auto item = queue_.front();
    queue_.pop();
    return item;
  }
  
  int size(){
	  return queue_.size();
  }
  
  bool empty(){
	  //cout << queue_.size() << endl;
	  return queue_.empty();
  }
 
  void pop(T& item)
  {
    std::unique_lock<std::mutex> mlock(mutex_);
    while (queue_.empty())
    {
      cond_.wait(mlock);
    }
    item = queue_.front();
    queue_.pop();
  }
 
  void push(const T& item)
  {
    std::unique_lock<std::mutex> mlock(mutex_);
    queue_.push(item);
	//cout << "PUSH: " << queue_.size() << endl;
    mlock.unlock();
    cond_.notify_one();
  }
 
  void push(T&& item)
  {
    std::unique_lock<std::mutex> mlock(mutex_);
    queue_.push(std::move(item));
	//cout << "PUSH: " << queue_.size() << endl;
    mlock.unlock();
    cond_.notify_one();
  }
 
 private:
  std::queue<T> queue_;
  std::mutex mutex_;
  std::condition_variable cond_;
};

Queue<Mat> q;
const string name = "Test";
bool stop = false;

double getTime(clock_t start, clock_t end) {
	return double(end - start) / CLOCKS_PER_SEC;
}





void show() {
	int tt = 1000 / conf.getFps() + 1;
	while (!stop || !q.empty()){
		Mat img = q.pop();
		imshow(name, img);
		//cout << tt << endl;
		//this_thread::sleep_for(std::chrono::milliseconds(50));		
		//cout << stop << " " << q.empty() << endl;
		if (waitKey(tt) >= 0)
			break;
	}
}

void getVideoOutput(VideoCapture cap, VideoWriter& output){	
	Size S = Size(conf.getWidth(), conf.getHeight());
	int ex = static_cast<int>(cap.get(CV_CAP_PROP_FOURCC));

    output.open(conf.getOutput(), ex, cap.get(CV_CAP_PROP_FPS), S, true);
}

void killThread(){
	
}

void proc() {
	VideoCapture cap;
	//cap.open("C:\\Users\\XuanDuc\\Desktop\\VIDEO0042.mp4");
	cap.open(conf.getVideo());	
	//cout << conf.getVideo() << endl;
	//cout << cap.get(CV_CAP_PROP_FPS) << endl;
	//namedWindow("video capture");
	cap.set(CV_CAP_PROP_FRAME_WIDTH , conf.getWidth());
	cap.set(CV_CAP_PROP_FRAME_HEIGHT , conf.getHeight());
	if (!cap.isOpened())
		return ;

	VideoWriter output;
	if (conf.isOutput())
		getVideoOutput(cap, output);

	Mat img, preImg;
	Detector* detector = conf.getDetector();
	Tracker* tracker = conf.getTracker();

	vector<Rect> found_filtered;
	for(int step = 0; true; step++) {
		bool skipFrame = false;
		clock_t start = clock();
		bool ok = cap.read(img);
		if (!ok) {
			//cout << "STOP" << endl;
			stop = true;
			q.push(preImg);	
			break;
		}
	
		resize(img, img, Size(conf.getWidth(), conf.getHeight()), 0, 0, INTER_CUBIC);
		//cout << img.cols << " " << img.rows << endl;
		if (!img.data)
			continue;

		if (step%conf.getStep() == 0)
			found_filtered = detector->detect(img);
		else if ((step%conf.getStep()) % 2 == 1) {
			if (step%conf.getStep() == 1)
				tracker->startDetect();
			found_filtered = tracker->detect(preImg, img, found_filtered);
		} else {
			skipFrame = true;
		}
		
		
		for (size_t i = 0; i < found_filtered.size(); i++) {			
			Rect r = found_filtered[i];
			cout << r.tl() << " " << r.width << " " << r.height << endl;
			r.x += cvRound(r.width * 0.1);
			r.width = cvRound(r.width * 0.8);
			r.y += cvRound(r.height * 0.06);
			r.height = cvRound(r.height * 0.9);
			rectangle(img, r.tl(), r.br(), cv::Scalar(0, 255, 0), 2);
		}
		//resize(img, img, Size(cap.get(CV_CAP_PROP_FRAME_WIDTH), cap.get(CV_CAP_PROP_FRAME_HEIGHT)), 0, 0, INTER_CUBIC);
		//imshow("video capture", img);
		//if (waitKey(50) >= 0) break;
		if (output.isOpened())
			output.write(img.clone());
		q.push(img.clone());
		clock_t end = clock();
		cout << getTime(start, end) << endl;		

		if (!skipFrame) preImg = img;		
	}
	delete detector;	
}

void help(){
	string message = "Options:\n";
	message += "--video=<video file name>\n";
	message += "--width=<width of resized video>\n";
	message += "--height=<height of resized video>\n";
	message += "--detector=<detector>\n";
	message += "--tracker=<tracker>\n";
	message += "--output=<output file>\n";

	cout << message;
}

//createsamples -vec samples.vec -w 30 -h 73
int main(int argc, const char * argv[]) {
	for(int i = 1; i < argc; ++i){
		if (strcmp(argv[i], "--help") == 0){
			help();
			return 0;
		}
		else 
			conf.parseArgument(argv[i]);
	}

	conf.init();
	//conf.debug();
	namedWindow(name);

	thread pr (proc);	
	show();
	
	pr.join();
	return 0;
}