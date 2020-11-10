package com.faceunity.fulivenativedemo;

import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;

import com.faceunity.beautycontrolview.BeautyControlView;

import java.io.IOException;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class MainActivity extends AppCompatActivity implements Camera.PreviewCallback, LifeCycleSensorManager.OnAccelerometerChangedListener {
    private static final String TAG = "MainActivity";

    private FuNativeJni mFuNativeJni;

    private GLSurfaceView mGlSurfaceView;
    private SurfaceTexture mSurfaceTexture;
    private ImageView mFaceDetect;

    private Camera mCamera;
    private int mCameraTextureId;
    private static final int PREVIEW_BUFFER_COUNT = 3;
    private byte[][] mPreviewCallbackBuffer;
    private int mCameraOrientation = 270;
    private int mCameraFacing = Camera.CameraInfo.CAMERA_FACING_FRONT;
    private int mCameraWidth = 1280;
    private int mCameraHeight = 720;
    private byte[] mCameraNV21Byte;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mGlSurfaceView = (GLSurfaceView) findViewById(R.id.gl_surface);
        BeautyControlView faceunityControlView = (BeautyControlView) findViewById(R.id.faceunity_control);
        mFaceDetect = (ImageView) findViewById(R.id.face_detect);

        mGlSurfaceView.setEGLContextClientVersion(GlUtil.getSupportGLVersion(this));
        GLRenderer glRenderer = new GLRenderer();
        mGlSurfaceView.setRenderer(glRenderer);
        mGlSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
        mGlSurfaceView.setKeepScreenOn(true);

        mFuNativeJni = new FuNativeJni(this);
        faceunityControlView.setOnFaceUnityControlListener(mFuNativeJni);
        faceunityControlView.setStickers(StickerEnum.getStickers());
        faceunityControlView.setFilters(FilterEnum.getFilters());

        LifeCycleSensorManager lifeCycleSensorManager = new LifeCycleSensorManager(this, getLifecycle());
        lifeCycleSensorManager.setOnAccelerometerChangedListener(this);
    }

    @Override
    protected void onResume() {
        super.onResume();
        mGlSurfaceView.onResume();
        openCamera(mCameraFacing);
        startCamera();
    }

    @Override
    protected void onPause() {
        super.onPause();
        final CountDownLatch count = new CountDownLatch(1);
        mGlSurfaceView.queueEvent(new Runnable() {
            @Override
            public void run() {
                onSurfaceDestroy();
                count.countDown();
            }
        });
        try {
            count.await(500, TimeUnit.MILLISECONDS);
        } catch (InterruptedException e) {
            // ignored
        }
        mGlSurfaceView.onPause();
        releaseCamera();
    }

    @Override
    public void onPreviewFrame(byte[] data, Camera camera) {
        mCameraNV21Byte = data;
        mCamera.addCallbackBuffer(data);
        mGlSurfaceView.requestRender();
    }

    public void switchCamera(View view) {
        mCameraFacing = mCameraFacing == Camera.CameraInfo.CAMERA_FACING_FRONT ? Camera.CameraInfo.CAMERA_FACING_BACK : Camera.CameraInfo.CAMERA_FACING_FRONT;
        openCamera(mCameraFacing);
        startCamera();
    }

    @Override
    public void onAccelerometerChanged(float x, float y, float z) {
        if (Math.abs(x) > 3 || Math.abs(y) > 3) {
            if (Math.abs(x) > Math.abs(y)) {
                mFuNativeJni.onDeviceOrientationChanged(x > 0 ? 0 : 180);
            } else {
                mFuNativeJni.onDeviceOrientationChanged(y > 0 ? 90 : 270);
            }
        }
    }

    private class GLRenderer implements GLSurfaceView.Renderer {
        private final float[] mTexMatrix = new float[16];

        @Override
        public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
            Log.d(TAG, "onSurfaceCreated: ");
            LimitFpsUtil.averageFrameRate(30);
            mCameraTextureId = GlUtil.createTextureObject(GLES11Ext.GL_TEXTURE_EXTERNAL_OES);
            startCamera();
            mFuNativeJni.onSurfaceCreated(getAssets(), "graphics/face_beautification.bundle");
        }

        @Override
        public void onSurfaceChanged(GL10 gl10, int i, int i1) {
            Log.d(TAG, "onSurfaceChanged: w " + i + ", h " + i1);
            mFuNativeJni.onSurfaceChanged(i, i1);
        }

        @Override
        public void onDrawFrame(GL10 gl10) {
            if (mSurfaceTexture == null) {
                return;
            }

            try {
                mSurfaceTexture.updateTexImage();
                mSurfaceTexture.getTransformMatrix(mTexMatrix);
            } catch (Exception e) {
                Log.e(TAG, "onDrawFrame: ", e);
            }

            if (mCameraNV21Byte == null) {
                return;
            }

            mFuNativeJni.onDrawFrame(mCameraNV21Byte, mCameraTextureId, mCameraWidth, mCameraHeight, mTexMatrix);
            mGlSurfaceView.requestRender();
            LimitFpsUtil.limitFrameRate();
        }
    }

    private void onSurfaceDestroy() {
        Log.d(TAG, "onSurfaceDestroy: ");
        if (mSurfaceTexture != null) {
            mSurfaceTexture.release();
            mSurfaceTexture = null;
        }
        if (mCameraTextureId > 0) {
            GLES20.glDeleteTextures(1, new int[]{mCameraTextureId}, 0);
            mCameraTextureId = 0;
        }
        mFuNativeJni.onSurfaceDestroyed();
    }

    private void openCamera(int cameraType) {
        Log.d(TAG, "openCamera: " + cameraType);
        if (mCamera != null) {
            releaseCamera();
        }
        int cameraId = Camera.CameraInfo.CAMERA_FACING_FRONT;
        Camera.CameraInfo info = new Camera.CameraInfo();
        int numCameras = Camera.getNumberOfCameras();
        for (int i = 0; i < numCameras; i++) {
            Camera.getCameraInfo(i, info);
            if (info.facing == cameraType) {
                cameraId = i;
                mCamera = Camera.open(i);
            }
        }
        mCameraOrientation = CameraUtils.getCameraOrientation(cameraId);
        CameraUtils.setCameraDisplayOrientation(this, cameraId, mCamera);
        Camera.Parameters parameters = mCamera.getParameters();
        CameraUtils.setFocusModes(parameters);
        int[] size = CameraUtils.choosePreviewSize(parameters, mCameraWidth, mCameraHeight);
        mCameraWidth = size[0];
        mCameraHeight = size[1];
        mCamera.setParameters(parameters);

        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                AspectFrameLayout aspectFrameLayout = (AspectFrameLayout) findViewById(R.id.aspect_frame);
                aspectFrameLayout.setAspectRatio(1.0f * mCameraHeight / mCameraWidth);
            }
        });
    }

    private void startCamera() {
        if (mCamera == null) {
            return;
        }
        if (mCameraTextureId <= 0) {
            return;
        }
        Log.d(TAG, "startCamera: ");
        mFuNativeJni.onCameraChanged(mCameraFacing, mCameraOrientation);
        mSurfaceTexture = new SurfaceTexture(mCameraTextureId);
        try {
            if (mPreviewCallbackBuffer == null) {
                mPreviewCallbackBuffer = new byte[PREVIEW_BUFFER_COUNT][mCameraWidth * mCameraHeight * 3 / 2];
            }
            mCamera.setPreviewCallbackWithBuffer(this);
            for (int i = 0; i < PREVIEW_BUFFER_COUNT; i++) {
                mCamera.addCallbackBuffer(mPreviewCallbackBuffer[i]);
            }
            mCamera.setPreviewTexture(mSurfaceTexture);
            mCamera.startPreview();
        } catch (Exception e) {
            Log.e(TAG, "startCamera: ", e);
        }
    }

    private void releaseCamera() {
        Log.d(TAG, "releaseCamera: ");
        if (mCamera != null) {
            mCameraNV21Byte = null;
            mCamera.stopPreview();
            mCamera.setPreviewCallback(null);
            try {
                mCamera.setPreviewTexture(null);
            } catch (IOException e) {
                Log.e(TAG, "releaseCamera: ", e);
            }
            mCamera.release();
            mCamera = null;
        }
    }

}
