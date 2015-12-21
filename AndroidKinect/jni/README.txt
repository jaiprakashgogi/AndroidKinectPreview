To create the native header file:

javah -o androidkinect.h -bootclasspath /home/jprakash/jai/android/android-sdk-linux/platforms/android-23/android.jar  -classpath ../bin/classes/ com.jaiprakashgogi.androidkinect.KinectActivity

For callback functions:
jprakash@jai-ubuntu:~/workspace/AndroidKinectPreview/AndroidKinect/bin/classes/com/jaiprakashgogi/androidkinect$ javap -s -p KinectActivity.class
