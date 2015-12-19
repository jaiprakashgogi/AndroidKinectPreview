#include "androidkinect.h"

JNIEXPORT jstring JNICALL Java_com_jaiprakashgogi_androidkinect_KinectActivity_callnative
  (JNIEnv *env, jobject){
	return env->NewStringUTF("Hello from JNI !  Compiled with ABI.");
}
