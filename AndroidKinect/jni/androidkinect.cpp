#define OPENCV 1
#include "androidkinect.h"
#if OPENCV
#include "libfreenect.hpp"
#include <iostream>
#include <vector>
#include <cmath>
#include <pthread.h>
#include <opencv2/opencv.hpp>
#include <cxcore.h>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <android/log.h>

#define APPNAME "androidkinectnative"

using namespace cv;
using namespace std;

int main_kinect(void);
#endif

jclass cls;
jmethodID mid;
JNIEnv *env;
jobject obj;
Mat depthMat;
Mat rgbMat;

JNIEXPORT jstring JNICALL Java_com_jaiprakashgogi_androidkinect_KinectActivity_callnative(
		JNIEnv *Env, jobject Obj, jlong rgb, jlong depth) {
	obj = Obj;
	env = Env;
	cls = env->GetObjectClass(obj);
	mid = env->GetMethodID(cls, "nativecallback", "()V");
	if (mid == 0)
		return env->NewStringUTF("Mid is zero.");
	//env->CallVoidMethod(obj, mid, 8850);
	depthMat = *(Mat *) depth;
	rgbMat = *(Mat *) rgb;
	main_kinect();
	return env->NewStringUTF("Hello from JNI !  Compiled with ABI.");
}

#if OPENCV
class myMutex {
public:
	myMutex() {
		pthread_mutex_init(&m_mutex, NULL);
	}
	void lock() {
		pthread_mutex_lock(&m_mutex);
	}
	void unlock() {
		pthread_mutex_unlock(&m_mutex);
	}
private:
	pthread_mutex_t m_mutex;
};

class MyFreenectDevice: public Freenect::FreenectDevice {
public:
	MyFreenectDevice(freenect_context *_ctx, int _index) :
	Freenect::FreenectDevice(_ctx, _index), m_buffer_depth(
			FREENECT_DEPTH_11BIT), m_buffer_rgb(FREENECT_VIDEO_RGB), m_gamma(
			2048), m_new_rgb_frame(false), m_new_depth_frame(false), depthMat(
			Size(640, 480), CV_16UC1), rgbMat(Size(640, 480), CV_8UC3,
			Scalar(0)), ownMat(Size(640, 480), CV_8UC3, Scalar(0)) {

		for (unsigned int i = 0; i < 2048; i++) {
			float v = i / 2048.0;
			v = std::pow(v, 3) * 6;
			m_gamma[i] = v * 6 * 256;
		}
	}

	// Do not call directly even in child
	void VideoCallback(void* _rgb, uint32_t timestamp) {
		std::cout << "RGB callback" << std::endl;
		m_rgb_mutex.lock();
		uint8_t* rgb = static_cast<uint8_t*>(_rgb);
		rgbMat.data = rgb;
		m_new_rgb_frame = true;
		m_rgb_mutex.unlock();
	}
	;

	// Do not call directly even in child
	void DepthCallback(void* _depth, uint32_t timestamp) {
		std::cout << "Depth callback" << std::endl;
		m_depth_mutex.lock();
		uint16_t* depth = static_cast<uint16_t*>(_depth);
		depthMat.data = (uchar*) depth;
		m_new_depth_frame = true;
		m_depth_mutex.unlock();
	}

	bool getVideo(Mat& output) {
		m_rgb_mutex.lock();
		if (m_new_rgb_frame) {
			cv::cvtColor(rgbMat, output, CV_RGB2BGR);
			m_new_rgb_frame = false;
			m_rgb_mutex.unlock();
			return true;
		} else {
			m_rgb_mutex.unlock();
			return false;
		}
	}

	bool getDepth(Mat& output) {
		m_depth_mutex.lock();
		if (m_new_depth_frame) {
			depthMat.copyTo(output);
			m_new_depth_frame = false;
			m_depth_mutex.unlock();
			return true;
		} else {
			m_depth_mutex.unlock();
			return false;
		}
	}
private:
	std::vector<uint8_t> m_buffer_depth;
	std::vector<uint8_t> m_buffer_rgb;
	std::vector<uint16_t> m_gamma;
	Mat depthMat;
	Mat rgbMat;
	Mat ownMat;
	myMutex m_rgb_mutex;
	myMutex m_depth_mutex;
	bool m_new_rgb_frame;
	bool m_new_depth_frame;
};

int main_kinect(void) {
	bool die(false);
	string filename("/sdcard/DCIM/Camera/snapshot");
	string suffix(".png");
	int i_snap(0), iter(0);

	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Jai start");
	//Mat depthMat(Size(640, 480), CV_16UC1);
	Mat depthf(Size(640, 480), CV_8UC1);
	//Mat rgbMat(Size(640, 480), CV_8UC3, Scalar(0));
	Mat ownMat(Size(640, 480), CV_8UC3, Scalar(0));

	// The next two lines must be changed as Freenect::Freenect
	// isn't a template but the method createDevice:
	// Freenect::Freenect<MyFreenectDevice> freenect;
	// MyFreenectDevice& device = freenect.createDevice(0);
	// by these two lines:

	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME,
			"Jai about to initialize Freenect");

	Freenect::Freenect freenect;
	MyFreenectDevice& device = freenect.createDevice<MyFreenectDevice>(0);

	device.startVideo();
	device.startDepth();
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Jai startVideo done");
	while (!die) {
		device.getVideo(rgbMat);
		device.getDepth(depthMat);
		depthMat.convertTo(depthf, CV_8UC1, 255.0 / 2048.0);
		env->CallVoidMethod(obj, mid);
		/*			std::ostringstream file;
		 file << filename << i_snap << suffix;
		 for(int y = 100; y< 110; y++)
		 for(int x = 100; x< 110; x++)
		 __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Jai (%d, %d) = %d", y,x, rgbMat.at<uchar>(y,x));
		 cv::imwrite("/sdcard/DCIM/Camera/snapshot.jpg",rgbMat);*/
		__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Jai id %i", i_snap);
		i_snap++;
		if (iter >= 1000)
		break;
		iter++;
	}
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Jai closing");
	device.stopVideo();
	device.stopDepth();
	return 0;
}

#endif
