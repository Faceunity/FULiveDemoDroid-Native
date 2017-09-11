package com.faceunity.fulivenativedemo;

import android.content.res.AssetManager;

/**
 * Created by tujh on 2017/8/25.
 */

public class FURenderer implements FaceunityControlView.OnViewEventListener {

    static {
        System.loadLibrary("faceunity-native");
    }

    public FURenderer() {
        resetStatus();
    }

    /**
     * 需要GL环境，初始化了faceunity相关的数据。
     *
     * @param manager 底层需要AssetManager用于从assets资源文件夹中读取v3.mp3、face_beautification.mp3以及道具bundle
     */
    public native void onSurfaceCreated(AssetManager manager);

    /**
     * 需要GL环境,应在SurfaceView大小改变时被调用
     *
     * @param w
     * @param h
     */
    public native void onSurfaceChanged(int w, int h);

    /**
     * 需要GL环境，接收每帧图像纹理与byte[]数据，绘制画面
     * @param img 图像byte[]数据
     * @param textureId 图像纹理
     * @param weight
     * @param height
     * @param mtx 画面方向旋转需要的矩阵
     */
    public native void onDrawFrame(byte[] img, int textureId, int weight, int height, float[] mtx);

    /**
     * 需要GL环境，SurfaceView销毁时被调用，释放faceunity相关的资源
     */
    public native void onSurfaceDestory();

    /**
     * 需要GL环境，切换摄像头时重置一些native数据
     */
    public native void switchCamera();

    /**
     * 不需要GL环境，重置美颜、滤镜以及道具数据
     */
    public native void resetStatus();

    @Override
    public native int onEffectItemSelected(AssetManager assetManager, String effectItemName);

    @Override
    public native void onFilterSelected(String filterName);

    @Override
    public native void onBlurLevelSelected(int level);

    @Override
    public native void onColorLevelSelected(int progress, int max);

    @Override
    public native void onCheekThinSelected(int progress, int max);

    @Override
    public native void onEnlargeEyeSelected(int progress, int max);

    @Override
    public native void onFaceShapeSelected(int faceShape);

    @Override
    public native void onFaceShapeLevelSelected(int progress, int max);

    @Override
    public native void onRedLevelSelected(int progress, int max);

}
