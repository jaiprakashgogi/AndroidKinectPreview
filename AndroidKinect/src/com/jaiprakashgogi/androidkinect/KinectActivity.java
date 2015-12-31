package com.jaiprakashgogi.androidkinect;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.android.Utils;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Scalar;
import org.opencv.imgproc.Imgproc;

import android.app.Activity;
import android.graphics.Bitmap;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;

public class KinectActivity extends Activity {

	private static final String TAG = "KinectActivity";
	private TextView textView;
	private Button button;
	private Mat rgb, depth, tmp;
	private ImageView imrgb;
	private int count = 0;
	private Handler staticHandler;
	private boolean kinectstatus = false;
	private Bitmap rgbBitmap, depthBitmap;
	

	private BaseLoaderCallback mLoaderCallback = new BaseLoaderCallback(this) {
		@Override
		public void onManagerConnected(int status) {
			switch (status) {
			case LoaderCallbackInterface.SUCCESS: {
				Log.i(TAG, "OpenCV loaded successfully");
				// Load native library after(!) OpenCV initialization
				System.loadLibrary("androidkinect");
				rgb = new Mat(480, 640, CvType.CV_8UC3);
				depth = new Mat(480, 640, CvType.CV_8UC1);
				tmp = new Mat (480, 640, CvType.CV_8U, new Scalar(4));
				rgbBitmap = Bitmap.createBitmap(rgb.cols(), rgb.rows(),Bitmap.Config.ARGB_8888);
				depthBitmap = Bitmap.createBitmap(rgb.cols(), rgb.rows(),Bitmap.Config.ARGB_8888);
				
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
			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub
				if (kinectstatus == false) {
					textView.setText(callnative(rgb.getNativeObjAddr(), depth.getNativeObjAddr(), true));
					kinectstatus = true;
				} else {
					textView.setText(callnative(rgb.getNativeObjAddr(), depth.getNativeObjAddr(), false));
					kinectstatus = false;
				}
			}
		});
		imrgb = (ImageView) findViewById(R.id.imageView1);
		Handler handler = new Handler() {
			@Override
			public void handleMessage(Message msg) {
				Log.i(TAG, "Message received " + msg.what);
				textView.setText("Jai: " + msg.what);
				if(imrgb != null){
					imrgb.setImageBitmap(depthBitmap);
					imrgb.invalidate();
				}
			}
		};
		staticHandler = handler;
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

	@Override
	protected void onDestroy() {
		// TODO Auto-generated method stub
		super.onDestroy();
		staticHandler = null;
	}

	public native String callnative(long l, long m, boolean status);

	/*
	 * static { System.loadLibrary("androidkinect"); }
	 */

	public void nativecallback() {
		/*
		 * byte[] temp = new byte[rgb.channels()]; rgb.get(100, 100, temp);
		 * Log.e(TAG, "Jai: RGB and depthmap received " + temp[0]);
		 * textView.setText("Pixel value is " + temp[0]); Bitmap img =
		 * Bitmap.createBitmap(rgb.cols(), rgb.rows(),Bitmap.Config.ARGB_8888);
		 * Utils.matToBitmap(rgb, img); if(imrgb != null){
		 * imrgb.setImageBitmap(img); }
		 */
		/*
		 * textView.setText("Callback received:" + count );
		 * textView.invalidate(); count++;
		 */
		// staticHandler.sendEmptyMessage(count);
		(new Thread(new Runnable() {
			@Override
			public void run() {
				Utils.matToBitmap(rgb, rgbBitmap);
				Imgproc.cvtColor(depth, tmp, Imgproc.COLOR_GRAY2RGBA, 4);
				Utils.matToBitmap(tmp, depthBitmap);
				staticHandler.sendEmptyMessage(count);
			}
		})).start();
		Log.i(TAG, "Jai frame update request");
		count++;
	}

}
