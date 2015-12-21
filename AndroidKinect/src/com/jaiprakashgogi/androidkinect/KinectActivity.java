package com.jaiprakashgogi.androidkinect;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.core.CvType;
import org.opencv.core.Mat;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;

public class KinectActivity extends Activity {

	private static final String TAG = "KinectActivity";
	private TextView textView;
	private Button button;
	private Mat rgb, depth;

	private BaseLoaderCallback mLoaderCallback = new BaseLoaderCallback(this) {
		@Override
		public void onManagerConnected(int status) {
			switch (status) {
			case LoaderCallbackInterface.SUCCESS: {
				Log.i(TAG, "OpenCV loaded successfully");
				// Load native library after(!) OpenCV initialization
				System.loadLibrary("androidkinect");
				rgb = new Mat(480, 640, CvType.CV_8UC3 );
				depth = new Mat(480, 640, CvType.CV_8UC1 );
			}
				break;
			default: {
				super.onManagerConnected(status);
			}
				break;
			}
		}
	};

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_kinect);

		textView = (TextView) findViewById(R.id.tv);
		button = (Button) findViewById(R.id.button);
		button.setOnClickListener(new OnClickListener() {

			public void onClick(View v) {
				// TODO Auto-generated method stub
				textView.setText(callnative(rgb.getNativeObjAddr(), depth.getNativeObjAddr()));
			}
		});
	}

	@Override
	protected void onResume() {
		// TODO Auto-generated method stub
		super.onResume();
		Log.i(TAG, "onResume Function");
		if (!OpenCVLoader.initDebug()) {
			Log.d(TAG, "Internal OpenCV library not found. Using OpenCV Manager for initialization");
			OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_3_0_0, this, mLoaderCallback);
		} else {
			Log.d(TAG, "OpenCV library found inside package. Using it!");
			mLoaderCallback.onManagerConnected(LoaderCallbackInterface.SUCCESS);
		}
	}

	@Override
	protected void onPause() {
		// TODO Auto-generated method stub
		super.onPause();
	}

	public native String callnative(long l, long m);

/*	static {
		System.loadLibrary("androidkinect");
	}*/
	
	public void nativecallback() {
		byte[] temp = new byte[rgb.channels()];
		rgb.get(100, 100, temp);
	    Log.e(TAG, "Jai: RGB and depthmap received " + temp[0]);
	}

}
