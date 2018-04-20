package com.faceunity.fulivenativedemo;

import android.Manifest;
import android.content.pm.PackageManager;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.os.Build;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;

import com.faceunity.beautycontrolview.BeautyControlView;
import com.faceunity.beautycontrolview.entity.Effect;
import com.faceunity.beautycontrolview.entity.Filter;

import java.io.IOException;
import java.util.List;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;
import java.util.regex.Pattern;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class MainActivity extends AppCompatActivity implements Camera.PreviewCallback {
    private static final String TAG = MainActivity.class.getName();

    private FURenderer mFURenderer;

    private GLSurfaceView mGLSurfaceView;
    private GLRenderer glRenderer;
    private SurfaceTexture mSurfaceTexture;

    private List<Effect> mEffects;
    private List<Filter> mBeautyFilters;
    private List<Filter> mFilters;

    private BeautyControlView mFaceunityControlView;
    private ImageView mFaceDetect;

    private Camera mCamera;
    private static final int PREVIEW_BUFFER_COUNT = 3;
    private byte[][] previewCallbackBuffer;
    private int mCameraOrientation = 270;
    private int cameraType = Camera.CameraInfo.CAMERA_FACING_FRONT;
    private int cameraWidth = 1280;
    private int cameraHeight = 720;

    private byte[] mCameraNV21Byte;

    private boolean isHasPermission = false;

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions,
                                           @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        //now i just regard it as CAMERA
        if (grantResults[0] == PackageManager.PERMISSION_GRANTED) {
            isHasPermission = true;
        } else {
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mGLSurfaceView = (GLSurfaceView) findViewById(R.id.gl_surface);
        mFaceunityControlView = (BeautyControlView) findViewById(R.id.faceunity_control);
        mFaceDetect = (ImageView) findViewById(R.id.face_detect);


        mGLSurfaceView.setEGLContextClientVersion(2);
        glRenderer = new GLRenderer();
        mGLSurfaceView.setRenderer(glRenderer);
        mGLSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);

        mFURenderer = new FURenderer();
        mFaceunityControlView.setOnFaceUnityControlListener(mFURenderer);

        checkPermission();

        mEffects = EffectEnum.getEffects();
        mBeautyFilters = FilterEnum.getFiltersByFilterType(Filter.FILTER_TYPE_BEAUTY_FILTER);
        mFilters = FilterEnum.getFiltersByFilterType(Filter.FILTER_TYPE_FILTER);

        mFaceunityControlView.setEffects(mEffects);
        mFaceunityControlView.setBeautyFilters(mBeautyFilters);
        mFaceunityControlView.setFilters(mFilters);

        mGLSurfaceView.onResume();
    }

    @Override
    protected void onResume() {
        super.onResume();
        getCamera(cameraType, cameraWidth, cameraHeight);
        //开启camera
        startCamera();
        mFaceunityControlView.onResume();
    }

    @Override
    protected void onPause() {
        super.onPause();
        releaseCamera();
        mFaceunityControlView.onPause();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        final CountDownLatch count = new CountDownLatch(1);
        mGLSurfaceView.queueEvent(new Runnable() {
            @Override
            public void run() {
                onSurfaceDestroy();
                count.countDown();
            }
        });
        try {
            count.await(1, TimeUnit.SECONDS);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        mGLSurfaceView.onPause();
    }

    @Override
    public void onPreviewFrame(byte[] data, Camera camera) {
        mCameraNV21Byte = data;
        mCamera.addCallbackBuffer(data);
        mGLSurfaceView.requestRender();
    }

    public void switchCamera(View view) {
        if (mCameraNV21Byte == null) return;
        cameraType = (cameraType == Camera.CameraInfo.CAMERA_FACING_FRONT ? Camera.CameraInfo.CAMERA_FACING_BACK : Camera.CameraInfo.CAMERA_FACING_FRONT);
        getCamera(cameraType, cameraWidth, cameraHeight);
        startCamera();
    }

    class GLRenderer implements GLSurfaceView.Renderer {

        @Override
        public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
            cameraTextureId = generateCameraTextureId();
            //开启camera
            startCamera();
            //调用相对应的FURenderer的onSurfaceCreated方法
            mFURenderer.onSurfaceCreated(MainActivity.this);
        }

        @Override
        public void onSurfaceChanged(GL10 gl10, int i, int i1) {

            mFURenderer.onSurfaceChanged(i, i1);
        }

        @Override
        public void onDrawFrame(GL10 gl10) {

            /**
             * 获取camera数据, 更新到texture
             */
            float[] mtx = new float[16];
            try {
                mSurfaceTexture.updateTexImage();
                mSurfaceTexture.getTransformMatrix(mtx);
            } catch (Exception e) {
                e.printStackTrace();
            }

            if (mCameraNV21Byte == null || mCameraNV21Byte.length == 0) {
                Log.e(TAG, "camera nv21 bytes null");
                return;
            }

            mFURenderer.onDrawFrame(mCameraNV21Byte, cameraTextureId, cameraWidth, cameraHeight, mtx);

            mGLSurfaceView.requestRender();
        }
    }

    public void onSurfaceDestroy() {
        if (mSurfaceTexture != null) {
            mSurfaceTexture.release();
        }
        if (cameraTextureId > 0) {
            GLES20.glDeleteTextures(1, new int[]{cameraTextureId}, 0);
            cameraTextureId = 0;
            logErrorGL("Failed to release surface texture");
        }
        mFURenderer.onSurfaceDestroy();
    }

    private void getCamera(int cameraType, int desiredWidth, int desiredHeight) {
        if (!isHasPermission) return;

        if (mCamera != null) {
            releaseCamera();
        }

        int cameraId = 0;

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

        int[] size = CameraUtils.choosePreviewSize(parameters, desiredWidth, desiredHeight);
        cameraWidth = size[0];
        cameraHeight = size[1];

        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                AspectFrameLayout aspectFrameLayout = (AspectFrameLayout) findViewById(R.id.aspect_frame);
                aspectFrameLayout.setAspectRatio(1.0f * cameraHeight / cameraWidth);
            }
        });

        mCamera.setParameters(parameters);
    }

    private void startCamera() {
        if (mCamera == null) return;
        if (cameraTextureId > 0) {
            mFURenderer.switchCamera(cameraType, mCameraOrientation);
            mSurfaceTexture = new SurfaceTexture(cameraTextureId);
            try {
                if (previewCallbackBuffer == null) {
                    previewCallbackBuffer = new byte[PREVIEW_BUFFER_COUNT][cameraWidth * cameraHeight * 3 / 2];
                }
                mCamera.setPreviewCallbackWithBuffer(this);
                for (int i = 0; i < PREVIEW_BUFFER_COUNT; i++)
                    mCamera.addCallbackBuffer(previewCallbackBuffer[i]);
                mCamera.setPreviewTexture(mSurfaceTexture);
                mCamera.startPreview();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    private void releaseCamera() {
        if (mCamera != null) {
            mCameraNV21Byte = null;
            mCamera.stopPreview();
            mCamera.setPreviewCallback(null);
            try {
                mCamera.setPreviewTexture(null);
            } catch (IOException e) {
                e.printStackTrace();
            }
            mCamera.release();
            mCamera = null;
        }
    }

    private void checkPermission() {
        //Meizu behaves wired in requestPermission, just ignore here
        boolean isMeizu = false;
        if (Build.FINGERPRINT.contains("Flyme")
                || Pattern.compile("Flyme", Pattern.CASE_INSENSITIVE).matcher(Build.DISPLAY).find()
                || Build.MANUFACTURER.contains("Meizu")
                || Build.MANUFACTURER.contains("MeiZu")) {
            Log.i(TAG, "the phone is meizu");
            isMeizu = true;
        }
        if (!isMeizu && (ContextCompat.checkSelfPermission(this, Manifest.permission.CAMERA) != PackageManager.PERMISSION_GRANTED ||
                ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED ||
                ContextCompat.checkSelfPermission(this, Manifest.permission.RECORD_AUDIO) != PackageManager.PERMISSION_GRANTED)) {
            Log.e(TAG, "no permission");
            ActivityCompat.requestPermissions(this,
                    new String[]{Manifest.permission.CAMERA,
                            Manifest.permission.WRITE_EXTERNAL_STORAGE,
                            Manifest.permission.RECORD_AUDIO}, 0);
        } else {
            Log.e(TAG, "has permission or is Meizu");
            isHasPermission = true;
        }
    }

    private int cameraTextureId;

    private int generateCameraTextureId() {
        int[] cameraTexture = new int[1];
        GLES20.glGenTextures(1, cameraTexture, 0);
        if (cameraTexture[0] <= 0) return 0;
        GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, cameraTexture[0]);
        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR);
        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR);
        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE);
        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE);
        logErrorGL("Failed to generate camera texture");
        return cameraTexture[0];
    }

    private static void logErrorGL(String log) {
        int error = GLES20.glGetError();
        if (error != 0) Log.d("Unity", "NatCam Error: " + log + " with error " + error);
    }
}
