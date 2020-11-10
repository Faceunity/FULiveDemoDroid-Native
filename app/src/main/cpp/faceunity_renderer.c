//
// Created by Richie Tu on 2020/11/06.
//

#include "faceunity_renderer.h"

#define STICKER_PATH_NONE "none"
#define FILTER_NAME_ORIGIN "origin"

#define CAMERA_FACING_FRONT 1
#define CAMERA_FACING_BACK 0

#define ITEM_ARRAYS_INDEX_FACE_BEAUTY  0
#define ITEM_ARRAYS_INDEX_STICKER  1

int mFrameId = 0;

// 贴纸文件路径
char mStickerPath[4096] = STICKER_PATH_NONE;

//美颜和滤镜的默认参数
volatile int mIsNeedUpdateParam = 1;
float mFilterLevel = 0.4f;//滤镜强度
char mFilterName[16] = FILTER_NAME_ORIGIN;

float mFaceBeautyABlurLevel = 1.0f;//精准磨皮
float mFaceBeautyType = 0.0f;//美肤类型
float mFaceBeautyBlurLevel = 0.7f;//磨皮
float mFaceBeautyColorLevel = 0.3f;//美白
float mFaceBeautyRedLevel = 0.3f;//红润
float mBrightEyesLevel = 0.0f;//亮眼
float mBeautyTeethLevel = 0.0f;//美牙

float mFaceBeautyFaceShape = 4.0f;//脸型
float mFaceShapeLevel = 1.0f;//程度
float mFaceBeautyEnlargeEye = 0.4f;//大眼
float mFaceBeautyCheekThin = 0.f;//瘦脸
float mChinLevel = 0.3f;//下巴
float mForeheadLevel = 0.3f;//额头
float mThinNoseLevel = 0.5f;//瘦鼻
float mMouthShape = 0.4f;//嘴形

// 句柄数组
int itemsArray[] = {0, 0};

int mCameraFacing = CAMERA_FACING_FRONT;
int mCameraOrientation = 270;
int mDeviceOrientation = 90;
int mRotMode = FU_ROTATION_MODE_90;

void updateStickerParams() {
    int itemHandle = itemsArray[ITEM_ARRAYS_INDEX_STICKER];
    if (itemHandle > 0) {
        fuAndroidNativeItemSetParamd(itemHandle, "isAndroid", 1.0);
        fuAndroidNativeItemSetParamd(itemHandle, "rotationMode", mRotMode);
        fuAndroidNativeItemSetParamd(itemHandle, "rotationAngle", mRotMode * 90);
    }
}

void updateRotationMode() {
    int rotMode = FU_ROTATION_MODE_0;
    int deviceOrientation = mDeviceOrientation;
    int cameraFacing = mCameraFacing;
    int cameraOrientation = mCameraOrientation;
    if (cameraOrientation == 270) {
        if (cameraFacing == CAMERA_FACING_FRONT) {
            rotMode = deviceOrientation / 90;
        } else {
            if (deviceOrientation == 90) {
                rotMode = FU_ROTATION_MODE_270;
            } else if (deviceOrientation == 270) {
                rotMode = FU_ROTATION_MODE_90;
            } else {
                rotMode = deviceOrientation / 90;
            }
        }
    } else if (cameraOrientation == 90) {
        if (cameraFacing == CAMERA_FACING_BACK) {
            if (deviceOrientation == 90) {
                rotMode = FU_ROTATION_MODE_270;
            } else if (deviceOrientation == 270) {
                rotMode = FU_ROTATION_MODE_90;
            } else {
                rotMode = deviceOrientation / 90;
            }
        } else {
            if (deviceOrientation == 0) {
                rotMode = FU_ROTATION_MODE_180;
            } else if (deviceOrientation == 90) {
                rotMode = FU_ROTATION_MODE_270;
            } else if (deviceOrientation == 270) {
                rotMode = FU_ROTATION_MODE_90;
            }
        }
    }
    mRotMode = rotMode;
}

void onDeviceChanged() {
    updateRotationMode();
    fuOnCameraChange();
    fuSetDefaultRotationMode(mRotMode);
    updateStickerParams();
}

void createSticker(JNIEnv *env, jobject assetsManager, const char *dataPath) {
    if (itemsArray[ITEM_ARRAYS_INDEX_STICKER] > 0) {
        fuAndroidNativeDestroyItem(itemsArray[ITEM_ARRAYS_INDEX_STICKER]);
        itemsArray[ITEM_ARRAYS_INDEX_STICKER] = 0;
    }
    void *dataPtr = NULL;
    int dataSize = 0;
    if (strncmp(STICKER_PATH_NONE, dataPath, sizeof(STICKER_PATH_NONE)) != 0) {
        readAssets(env, assetsManager, dataPath, &dataPtr, &dataSize);
    }
    strcpy(mStickerPath, dataPath);
    if (dataPtr) {
        itemsArray[ITEM_ARRAYS_INDEX_STICKER] = fuAndroidNativeCreateItemFromPackage(dataPtr,
                                                                                     dataSize);
        updateStickerParams();
        free(dataPtr);
    }
}

int setup(void *auth, int size) {
    if (fuIsLibraryInit()) {
        return 1;
    } else {
        fuAndroidNativeSetup(NULL, 0, auth, size);
        return 0;
    }
}

void onSurfaceCreated(JNIEnv *env, jobject assetsManager, const char *dataPath) {
    mFrameId = 0;
    createProgram();

    // 创建道具句柄可以异步执行
    void *dataPtr = NULL;
    int dataSize;
    readAssets(env, assetsManager, dataPath, &dataPtr, &dataSize);
    if (dataPtr) {
        itemsArray[ITEM_ARRAYS_INDEX_FACE_BEAUTY] = fuAndroidNativeCreateItemFromPackage(dataPtr,
                                                                                         dataSize);
        free(dataPtr);
    }
    mIsNeedUpdateParam = 1;
    fuSetMaxFaces(4);
    onDeviceChanged();

    createSticker(env, assetsManager, mStickerPath);
}

void onSurfaceChanged(int x, int y, int width, int height) {
    glViewport(x, y, width, height);
}

void onDrawFrame(void *img, int textureId, int width, int height, float *texMatrix) {
    int systemError = fuAndroidNativeGetSystemError();
    if (systemError != 0) {
        LOGE("system error %d %s", systemError, fuAndroidNativeGetSystemErrorString(systemError));
    }

    //修改美颜参数
    if (mIsNeedUpdateParam && itemsArray[ITEM_ARRAYS_INDEX_FACE_BEAUTY] > 0) {
        int item = itemsArray[ITEM_ARRAYS_INDEX_FACE_BEAUTY];
        //filter_level 滤镜强度 范围0~1 SDK默认为 1
        fuAndroidNativeItemSetParamd(item, "filter_level",
                                     mFilterLevel);
        //filter_name 滤镜
        fuAndroidNativeItemSetParams(item, "filter_name",
                                     mFilterName);

        //skin_detect 精准美肤 0:关闭 1:开启 SDK默认为 0
        fuAndroidNativeItemSetParamd(item, "skin_detect",
                                     mFaceBeautyABlurLevel);
        //heavy_blur 美肤类型 0:清晰美肤 1:朦胧美肤 SDK默认为 0
        fuAndroidNativeItemSetParamd(item, "heavy_blur",
                                     mFaceBeautyType);
        //blur_level 磨皮 范围0~6 SDK默认为 6
        fuAndroidNativeItemSetParamd(item, "blur_level",
                                     6.0f * mFaceBeautyBlurLevel);
        //color_level 美白 范围0~1 SDK默认为 1
        fuAndroidNativeItemSetParamd(item, "color_level",
                                     mFaceBeautyColorLevel);
        //red_level 红润 范围0~1 SDK默认为 1
        fuAndroidNativeItemSetParamd(item, "red_level",
                                     mFaceBeautyRedLevel);
        //eye_bright 亮眼 范围0~1 SDK默认为 0
        fuAndroidNativeItemSetParamd(item, "eye_bright",
                                     mBrightEyesLevel);
        //tooth_whiten 美牙 范围0~1 SDK默认为 0
        fuAndroidNativeItemSetParamd(item, "tooth_whiten",
                                     mBeautyTeethLevel);

        //face_shape_level 美型程度 范围0~1 SDK默认为1
        fuAndroidNativeItemSetParamd(item, "face_shape_level",
                                     mFaceShapeLevel);
        //face_shape 脸型 0：女神 1：网红 2：自然 3：默认 SDK默认为 3
        fuAndroidNativeItemSetParamd(item, "face_shape",
                                     mFaceBeautyFaceShape);
        //eye_enlarging 大眼 范围0~1 SDK默认为 0
        fuAndroidNativeItemSetParamd(item, "eye_enlarging",
                                     mFaceBeautyEnlargeEye);
        //cheek_thinning 瘦脸 范围0~1 SDK默认为 0
        fuAndroidNativeItemSetParamd(item, "cheek_thinning",
                                     mFaceBeautyCheekThin);
        //intensity_chin 下巴 范围0~1 SDK默认为 0.5    大于0.5变大，小于0.5变小
        fuAndroidNativeItemSetParamd(item, "intensity_chin",
                                     mChinLevel);
        //intensity_forehead 额头 范围0~1 SDK默认为 0.5    大于0.5变大，小于0.5变小
        fuAndroidNativeItemSetParamd(item,
                                     "intensity_forehead", mForeheadLevel);
        //intensity_nose 鼻子 范围0~1 SDK默认为 0
        fuAndroidNativeItemSetParamd(item, "intensity_nose",
                                     mThinNoseLevel);
        //intensity_mouth 嘴型 范围0~1 SDK默认为 0.5   大于0.5变大，小于0.5变小
        fuAndroidNativeItemSetParamd(item, "intensity_mouth",
                                     mMouthShape);
        mIsNeedUpdateParam = 0;
    }

    int flags = FU_ADM_FLAG_EXTERNAL_OES_TEXTURE;
    if (mCameraFacing == CAMERA_FACING_BACK) {
        flags |= ANDROID_NATIVE_NAMA_RENDER_OPTION_FLIP_X;
    }
    int texture = fuAndroidNativeDualInputToTexture(img, (GLuint) textureId, flags, width, height,
                                                    ++mFrameId, itemsArray,
                                                    sizeof(itemsArray) / sizeof(*itemsArray),
                                                    NULL, 0, 0, NULL, 0, 0, 0);
    drawFrame(texture, texMatrix);
}

void onSurfaceDestroy() {
    releaseProgram();
    for (int i = 0, s = sizeof(itemsArray) / sizeof(*itemsArray); i < s; ++i) {
        itemsArray[i] = 0;
    }
    fuAndroidNativeDestroyAllItems();
    fuAndroidNativeDone();
    fuAndroidNativeOnDeviceLost();
}

void onCameraChanged(int cameraFacing, int cameraOrientation) {
    if (mCameraFacing != cameraFacing || mCameraOrientation != cameraOrientation) {
        LOGD("onCameraChanged facing: %s, orientation: %d",
             cameraFacing == CAMERA_FACING_BACK ? "back" : "front", cameraOrientation);
        mCameraFacing = cameraFacing;
        mCameraOrientation = cameraOrientation;
        onDeviceChanged();
    }
}

void onDeviceOrientationChanged(int deviceOrientation) {
    if (mDeviceOrientation != deviceOrientation) {
        LOGD("onDeviceOrientationChanged %d", deviceOrientation);
        mDeviceOrientation = deviceOrientation;
        onDeviceChanged();
    }
}

void resetStatus() {
    mFrameId = 0;
    mIsNeedUpdateParam = 1;

    mFilterLevel = 0.4f;//滤镜强度
    strcpy(mFilterName, FILTER_NAME_ORIGIN);//滤镜名称

    mFaceBeautyABlurLevel = 1.0f;//精准磨皮
    mFaceBeautyType = 0.0f;//美肤类型
    mFaceBeautyBlurLevel = 0.7f;//磨皮
    mFaceBeautyColorLevel = 0.3f;//美白
    mFaceBeautyRedLevel = 0.3f;//红润
    mBrightEyesLevel = 0.0f;//亮眼
    mBeautyTeethLevel = 0.0f;//美牙

    mFaceBeautyFaceShape = 3.0f;//脸型
    mFaceShapeLevel = 1.0f;//程度
    mFaceBeautyEnlargeEye = 0.4f;//大眼
    mFaceBeautyCheekThin = 0.f;//瘦脸
    mChinLevel = 0.3f;//下巴
    mForeheadLevel = 0.3f;//额头
    mThinNoseLevel = 0.5f;//瘦鼻
    mMouthShape = 0.4f;//嘴形

    strcpy(mStickerPath, STICKER_PATH_NONE);
}

void onFilterLevelSelected(float progress) {
    mFilterLevel = progress;
    mIsNeedUpdateParam = 1;
}

void onFilterSelected(char *name) {
    strcpy(mFilterName, name);
    mIsNeedUpdateParam = 1;
}

void onAllBlurLevelSelected(float isAll) {
    mFaceBeautyABlurLevel = isAll;
    mIsNeedUpdateParam = 1;
}

void onBeautySkinTypeSelected(float skinType) {
    mFaceBeautyType = skinType;
    mIsNeedUpdateParam = 1;
}

void onBlurLevelSelected(float level) {
    mFaceBeautyBlurLevel = level;
    mIsNeedUpdateParam = 1;
}

void onColorLevelSelected(float progress) {
    mFaceBeautyColorLevel = progress;
    mIsNeedUpdateParam = 1;
}

void onRedLevelSelected(float progress) {
    mFaceBeautyRedLevel = progress;
    mIsNeedUpdateParam = 1;
}

void onBrightEyesSelected(float progress) {
    mBrightEyesLevel = progress;
    mIsNeedUpdateParam = 1;
}

void onBeautyTeethSelected(float progress) {
    mBeautyTeethLevel = progress;
    mIsNeedUpdateParam = 1;
}

void onFaceShapeSelected(float faceShape) {
    mFaceBeautyFaceShape = faceShape;
    mIsNeedUpdateParam = 1;
}

void onEnlargeEyeSelected(float progress) {
    mFaceBeautyEnlargeEye = progress;
    mIsNeedUpdateParam = 1;
}

void onCheekThinSelected(float progress) {
    mFaceBeautyCheekThin = progress;
    mIsNeedUpdateParam = 1;
}

void onChinLevelSelected(float progress) {
    mChinLevel = progress;
    mIsNeedUpdateParam = 1;
}

void onForeheadLevelSelected(float progress) {
    mForeheadLevel = progress;
    mIsNeedUpdateParam = 1;
}

void onThinNoseLevelSelected(float progress) {
    mThinNoseLevel = progress;
    mIsNeedUpdateParam = 1;
}

void onMouthShapeSelected(float progress) {
    mMouthShape = progress;
    mIsNeedUpdateParam = 1;
}
