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
import android.widget.Toast;

import java.io.IOException;
import java.util.regex.Pattern;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class MainActivity extends AppCompatActivity implements Camera.PreviewCallback {
    private static final String TAG = MainActivity.class.getName();

    private FURenderer mFURenderer;

    private GLSurfaceView mGLSurfaceView;
    private GLRenderer glRenderer;
    private long glThreadId;
    private SurfaceTexture mSurfaceTexture;

    private FaceunityControlView mFaceunityControlView;
    private ImageView mFaceDetect;

    private Camera mCamera;
    private int cameraType = Camera.CameraInfo.CAMERA_FACING_FRONT;
    private int cameraWidth = 1280;
    private int cameraHeight = 720;

    private byte[] mCameraNV21Byte;

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions,
                                           @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        //now i just regard it as CAMERA
        if (grantResults[0] == PackageManager.PERMISSION_GRANTED) {
            Toast.makeText(this, "已拥有权限，请再次点击",
                    Toast.LENGTH_SHORT).show();
        } else {
            Toast.makeText(this, "you must permit the camera permission!",
                    Toast.LENGTH_SHORT).show();
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mGLSurfaceView = (GLSurfaceView) findViewById(R.id.gl_surface);
        mFaceunityControlView = (FaceunityControlView) findViewById(R.id.faceunity_control);
        mFaceDetect = (ImageView) findViewById(R.id.face_detect);


        mGLSurfaceView.setEGLContextClientVersion(2);
        glRenderer = new GLRenderer();
        mGLSurfaceView.setRenderer(glRenderer);
        mGLSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);

        mFURenderer = new FURenderer();
        mFaceunityControlView.setOnViewEventListener(mFURenderer);

        checkPermission();
    }

    @Override
    protected void onResume() {
        super.onResume();
        mGLSurfaceView.onResume();
    }

    @Override
    protected void onPause() {
        super.onPause();
        mGLSurfaceView.queueEvent(new Runnable() {
            @Override
            public void run() {
                mFURenderer.onSurfaceDestory();
            }
        });
        mGLSurfaceView.onPause();
        releaseCamera();
    }

    @Override
    public void onPreviewFrame(byte[] data, Camera camera) {
        mCameraNV21Byte = data;
    }

    public void switchCamera(View view) {
        cameraType = (cameraType == Camera.CameraInfo.CAMERA_FACING_FRONT ? Camera.CameraInfo.CAMERA_FACING_BACK : Camera.CameraInfo.CAMERA_FACING_FRONT);
        getCamera(cameraType, cameraWidth, cameraHeight);
        startCamera();
    }

    class GLRenderer implements GLSurfaceView.Renderer {

        @Override
        public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
            //获取glThreadI
            glThreadId = Thread.currentThread().getId();

            //初始化camera
            getCamera(cameraType, cameraWidth, cameraHeight);
            //开启camera
            startCamera();
            //调用相对应的FURenderer的onSurfaceCreated方法
            mFURenderer.onSurfaceCreated(getAssets());
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

            mFURenderer.onDrawFrame(mCameraNV21Byte, getCameraTexture(), cameraWidth, cameraHeight, mtx);
        }
    }

    private void getCamera(int cameraType, int desiredWidth, int desiredHeight) {
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

        CameraUtils.setCameraDisplayOrientation(this, cameraId, mCamera);

        Camera.Parameters parameters = mCamera.getParameters();

        CameraUtils.setFocusModes(parameters);

        CameraUtils.chooseFramerate(parameters, 30);

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
        if (Thread.currentThread().getId() == glThreadId) {
            mFURenderer.switchCamera();
            releaseCameraTexture();
            mSurfaceTexture = new SurfaceTexture(getCameraTexture());
            try {
                mCamera.setPreviewCallback(this);
                mCamera.setPreviewTexture(mSurfaceTexture);
                mCamera.startPreview();
            } catch (Exception e) {
                e.printStackTrace();
            }
        } else {
            mGLSurfaceView.queueEvent(new Runnable() {
                @Override
                public void run() {
                    startCamera();
                }
            });
        }
    }

    private void releaseCamera() {
        if (mCamera != null) {
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
        }
    }

    private int cameraTexture[];

    public int getCameraTexture() {
        if (cameraTexture != null) return cameraTexture[0];
        if (!generateCameraTexture()) logErrorGL("Camera texture generation failed");
        return cameraTexture[0];
    }

    private boolean generateCameraTexture() {
        cameraTexture = new int[1];
        GLES20.glGenTextures(1, cameraTexture, 0);
        if (cameraTexture[0] <= 0) return false;
        GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, cameraTexture[0]);
        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR);
        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR);
        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE);
        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE);
        logErrorGL("Failed to generate camera texture");
        return true;
    }

    public void releaseCameraTexture() {
        if (cameraTexture == null) return;
        GLES20.glDeleteTextures(1, cameraTexture, 0);
        logErrorGL("Failed to release surface texture");
        cameraTexture = null;
    }

    private static void logErrorGL(String log) {
        int error = GLES20.glGetError();
        if (error != 0) Log.d("Unity", "NatCam Error: " + log + " with error " + error);
    }
}
