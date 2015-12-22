#define OPENCV 1
#include "androidkinect.h"
#if OPENCV
#include "libfreenect.hpp"
//#include <iostream>
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
#endif

/*jclass cls;
jmethodID mid;
JNIEnv *env;
jobject obj;*/
static JavaVM* jvm = 0;
static jobject activity = 0; // GlobalRef
#if OPENCV
Mat depthMat;
Mat rgbMat;
bool die(false);

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

#endif

void sendCallback(JNIEnv* env, jmethodID methodID){
    env->CallVoidMethod(activity, methodID);
    __android_log_print(ANDROID_LOG_DEBUG, "threadFunction", "Not Detaching");
}

void *KinectPreview(void *) {
    JNIEnv* env;
    __android_log_print(ANDROID_LOG_DEBUG, "threadFunction", "Attaching");
    jvm->AttachCurrentThread(&env, NULL);
    jclass clazz = env->GetObjectClass(activity);
    jmethodID methodID = env->GetMethodID(clazz, "nativecallback", "()V");
	Freenect::Freenect freenect;
	MyFreenectDevice & device = freenect.createDevice<MyFreenectDevice>(0);
	device.startVideo();
	device.startDepth();
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Thread started");
	while (!die) {
		device.getVideo(rgbMat);
		device.getDepth(depthMat);
		//env->CallVoidMethod(obj,mid);
		sendCallback(env, methodID);
		__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Frames received");
	}
	device.stopVideo();
	device.stopDepth();
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Thread stopped");
	delete (&device);
	delete (&freenect);
	pthread_exit(NULL);
}

JNIEXPORT jstring JNICALL Java_com_jaiprakashgogi_androidkinect_KinectActivity_callnative(
		JNIEnv *env, jobject thiz, jlong rgb, jlong depth, jboolean status) {
    env->GetJavaVM(&jvm);
    activity = env->NewGlobalRef(thiz);
/*	obj = Obj;
	env = Env;
	cls = env->GetObjectClass(obj);
	mid = env->GetMethodID(cls, "nativecallback", "()V");
	if (mid == 0)
		return env->NewStringUTF("Mid is zero.");*/
#if OPENCV
	depthMat = *(Mat *) depth;
	rgbMat = *(Mat *) rgb;

	Mat ownMat(Size(640, 480), CV_8UC3, Scalar(0));
#endif

	pthread_t threads;
	if (status) {
		int rc = pthread_create(&threads, NULL, KinectPreview, NULL);
		if (rc) {
			__android_log_print(ANDROID_LOG_VERBOSE, APPNAME,
					"Unable to create Thread");
		}
		__android_log_print(ANDROID_LOG_VERBOSE, APPNAME,
				"Jai startVideo done");
		return env->NewStringUTF("Preview Started..");
	} else {
		die = true;
		//pthread_exit(NULL);
		__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Stopping thread");
		return env->NewStringUTF("Preview Stopped..");
	}

}
