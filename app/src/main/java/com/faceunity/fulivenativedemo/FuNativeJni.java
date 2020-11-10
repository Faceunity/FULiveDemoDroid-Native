package com.faceunity.fulivenativedemo;

import android.content.Context;
import android.content.res.AssetManager;

import com.faceunity.beautycontrolview.OnFaceUnityControlListener;
import com.faceunity.beautycontrolview.entity.Sticker;

/**
 * @author Richie on 2020.11.04
 */
public final class FuNativeJni implements OnFaceUnityControlListener {

    public static final int FUAITYPE_DDE = 1;
    public static final int FUAITYPE_BACKGROUNDSEGMENTATION = 1 << 1;
    public static final int FUAITYPE_HAIRSEGMENTATION = 1 << 2;
    public static final int FUAITYPE_HANDGESTURE = 1 << 3;
    public static final int FUAITYPE_TONGUETRACKING = 1 << 4;
    public static final int FUAITYPE_FACELANDMARKS75 = 1 << 5;
    public static final int FUAITYPE_FACELANDMARKS209 = 1 << 6;
    public static final int FUAITYPE_FACELANDMARKS239 = 1 << 7;
    public static final int FUAITYPE_HUMANPOSE2D = 1 << 8;
    public static final int FUAITYPE_BACKGROUNDSEGMENTATION_GREEN = 1 << 9;
    public static final int FUAITYPE_FACEPROCESSOR = 1 << 10;
    public static final int FUAITYPE_FACEPROCESSOR_FACECAPTURE = 1 << 11;
    public static final int FUAITYPE_FACEPROCESSOR_FACECAPTURE_TONGUETRACKING = 1 << 12;
    public static final int FUAITYPE_FACEPROCESSOR_HAIRSEGMENTATION = 1 << 13;
    public static final int FUAITYPE_FACEPROCESSOR_HEADSEGMENTATION = 1 << 14;
    public static final int FUAITYPE_HUMAN_PROCESSOR = 1 << 15;
    public static final int FUAITYPE_HUMAN_PROCESSOR_DETECT = 1 << 16;
    public static final int FUAITYPE_HUMAN_PROCESSOR_2D_SELFIE = 1 << 17;
    public static final int FUAITYPE_HUMAN_PROCESSOR_2D_DANCE = 1 << 18;
    public static final int FUAITYPE_HUMAN_PROCESSOR_3D_SELFIE = 1 << 19;
    public static final int FUAITYPE_HUMAN_PROCESSOR_3D_DANCE = 1 << 20;
    public static final int FUAITYPE_HUMAN_PROCESSOR_SEGMENTATION = 1 << 21;

    static {
        System.loadLibrary("fu_native");
    }

    private final AssetManager mAssetManager;

    public FuNativeJni(Context context) {
        mAssetManager = context.getAssets();
    }

    public static native void setup();

    public static native void loadAiModel(AssetManager assetManager, String filePath, int aiType);

    public static native void releaseAiModel(int aiType);

    public static native void destroy();

    public native void onSurfaceCreated(AssetManager assetManager, String faceBeautyPath);

    public native void onSurfaceChanged(int width, int height);

    public native void onSurfaceDestroyed();

    public native void onCameraChanged(int cameraFacing, int cameraOrientation);

    public native void onDeviceOrientationChanged(int deviceOrientation);

    public native void onDrawFrame(byte[] nv21, int textureId, int width, int height, float[] texMatrix);

    public native void resetStatus();

    private native void onStickerSelected(AssetManager assetManager, String path);

    public void onStickerSelected(Sticker sticker) {
        onStickerSelected(mAssetManager, sticker.getFilePath());
    }

    public native void onFilterLevelSelected(float level);

    public native void onFilterNameSelected(String name);

    public native void onAllBlurLevelSelected(float isAll);

    public native void onBeautySkinTypeSelected(float skinType);

    public native void onBlurLevelSelected(float level);

    public native void onColorLevelSelected(float level);

    public native void onRedLevelSelected(float level);

    public native void onBrightEyesSelected(float level);

    public native void onBeautyTeethSelected(float level);

    public native void onFaceShapeSelected(float faceShape);

    public native void onEnlargeEyeSelected(float level);

    public native void onCheekThinSelected(float level);

    public native void onChinLevelSelected(float level);

    public native void onForeheadLevelSelected(float level);

    public native void onThinNoseLevelSelected(float level);

    public native void onMouthShapeSelected(float level);

}
