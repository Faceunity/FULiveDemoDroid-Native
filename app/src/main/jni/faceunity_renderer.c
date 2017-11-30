#include <jni.h>
#include <pthread.h>

#include "authpack.h"
#include "GlUtils.h"
#include "android_native_interface.h"
#include "android_util.h"

volatile int isSetupComplete = 0;

volatile int frame_id = 0;

volatile float mFacebeautyColorLevel = 0.2f;
volatile float mFacebeautyBlurLevel = 6.0f;
volatile float mFacebeautyCheeckThin = 1.0f;
volatile float mFacebeautyEnlargeEye = 0.5f;
volatile float mFacebeautyRedLevel = 0.5f;
volatile int mFaceShape = 3;
volatile float mFaceShapeLevel = 0.5f;

volatile char *mFilterName = "nature";
volatile char *mEffectName = "none";

volatile int itemsArray[] = {0, 0};

int mCameraType;

int mIsTracking = -1;

int createEffect(JNIEnv *env, jobject assetManager, char *name) {

    if (strncmp("none", name, 4) == 0) {
        fuAndroidNativeDestroyItem(itemsArray[1]);
        itemsArray[1] = 0;
    } else {
        int temp = itemsArray[1];
        void *effectData;
        int effectSize;
        readAssets(env, assetManager, name, &effectData, &effectSize);
        if (isSetupComplete == 0) {
            free(effectData);
            return 0;
        }
        itemsArray[1] = fuAndroidNativeCreateItemFromPackage(effectData, effectSize);
        fuAndroidNativeItemSetParamd(itemsArray[1], "isAndroid", 1.0);
        fuAndroidNativeItemSetParamd(itemsArray[1], "rotationAngle", mCameraType == 1 ? 90 : 270);
        if (temp != 0) {
            fuAndroidNativeDestroyItem(temp);
        }
        free(effectData);
    }
    return 1;
}

JNIEXPORT void JNICALL
Java_com_faceunity_fulivenativedemo_FURenderer_onSurfaceCreated(JNIEnv *env, jobject instance,
                                                                jobject manager) {
    //初始化opengl绘画需要的program
    createProgram();

    //读取assets中的v3.mp3数据，并初始化
    void *v3;
    int v3Size;
    readAssets(env, manager, "v3.mp3", &v3, &v3Size);
    fuAndroidNativeSetup(v3, v3Size, g_auth_package, sizeof(g_auth_package));

    //读取assets中的face_beautification.mp3数据，并初始化
    void *beautification;
    int beautificationSize;
    readAssets(env, manager, "face_beautification.mp3", &beautification, &beautificationSize);
    itemsArray[0] = fuAndroidNativeCreateItemFromPackage(beautification, beautificationSize);

    isSetupComplete = 1;

    free(v3);
    free(beautification);

    LOGE("version %s ", fuAndroidNativGetVersion());

    //判断当前显示的道具状态，并加载相应的道具
    createEffect(env, manager, mEffectName);
}

JNIEXPORT void JNICALL
Java_com_faceunity_fulivenativedemo_FURenderer_onSurfaceChanged(JNIEnv *env, jobject instance,
                                                                jint wight, jint height) {
    glViewport(0, 0, wight, height);
}

JNIEXPORT void JNICALL
Java_com_faceunity_fulivenativedemo_FURenderer_onDrawFrame(JNIEnv *env, jobject instance,
                                                           jbyteArray img_, jint textureId,
                                                           jint weight, jint height,
                                                           jfloatArray mtx_) {

    jbyte *img = (*env)->GetByteArrayElements(env, img_, NULL);
    jfloat *mtx = (*env)->GetFloatArrayElements(env, mtx_, NULL);

    int isTracking = fuAndroidNativeIsTracking();
    if (mIsTracking != isTracking) {
        LOGE("isTracking %d ", isTracking);
        mIsTracking = isTracking;
    }

    //设置一系列美颜参数
    fuAndroidNativeItemSetParamd(itemsArray[0], "color_level", mFacebeautyColorLevel);
    fuAndroidNativeItemSetParamd(itemsArray[0], "blur_level", mFacebeautyBlurLevel);
    fuAndroidNativeItemSetParamd(itemsArray[0], "cheek_thinning", mFacebeautyCheeckThin);
    fuAndroidNativeItemSetParamd(itemsArray[0], "eye_enlarging", mFacebeautyEnlargeEye);
    fuAndroidNativeItemSetParamd(itemsArray[0], "red_level", mFacebeautyRedLevel);
    fuAndroidNativeItemSetParamd(itemsArray[0], "face_shape", mFaceShape);
    fuAndroidNativeItemSetParamd(itemsArray[0], "face_shape_level", mFaceShapeLevel);

    //设置滤镜
    fuAndroidNativeItemSetParams(itemsArray[0], "filter_name", mFilterName);

    int texture = fuAndroidNativeDualInputToTexture(img, (GLuint) textureId, 1, weight, height,
                                                    frame_id++,
                                                    itemsArray, 2, NULL, weight, height, NULL, 0);

    //把处理完的纹理绘制到屏幕上
    drawFrame(texture, mtx);

    (*env)->ReleaseByteArrayElements(env, img_, img, 0);
    (*env)->ReleaseFloatArrayElements(env, mtx_, mtx, 0);
}

JNIEXPORT void JNICALL
Java_com_faceunity_fulivenativedemo_FURenderer_onSurfaceDestory(JNIEnv *env, jobject instance) {
    fuAndroidNativeDestroyItem(itemsArray[1]);
    itemsArray[1] = itemsArray[1] = 0;
    fuAndroidNativeDestroyItem(itemsArray[0]);
    itemsArray[0] = itemsArray[0] = 0;
    fuAndroidNativeDestroyAllItems();
    fuAndroidNativeOnDeviceLost();

    isSetupComplete = 0;
}

JNIEXPORT void JNICALL
Java_com_faceunity_fulivenativedemo_FURenderer_switchCamera(JNIEnv *env, jobject instance,
                                                            jint cameraType) {
    mCameraType = cameraType;
    fuOnCameraChange();
}


JNIEXPORT void JNICALL
Java_com_faceunity_fulivenativedemo_FURenderer_resetStatus(JNIEnv *env, jobject instance) {

    frame_id = 0;

    mFacebeautyColorLevel = 0.2f;
    mFacebeautyBlurLevel = 6.0f;
    mFacebeautyCheeckThin = 1.0f;
    mFacebeautyEnlargeEye = 0.5f;
    mFacebeautyRedLevel = 0.5f;
    mFaceShape = 3;
    mFaceShapeLevel = 0.5f;

    mFilterName = "nature";
    mEffectName = "none";

    itemsArray[0] = itemsArray[1] = 0;

    mIsTracking = -1;
}

JNIEXPORT int JNICALL
Java_com_faceunity_fulivenativedemo_FURenderer_onEffectItemSelected(JNIEnv *env, jobject instance,
                                                                    jobject assetManager,
                                                                    jstring effectItemName_) {
    const char *effectItemName = (*env)->GetStringUTFChars(env, effectItemName_, 0);
    mEffectName = (char *) effectItemName;
    return createEffect(env, assetManager, mEffectName);
}

JNIEXPORT jobject JNICALL
Java_com_faceunity_fulivenativedemo_FURenderer_onFilterSelected(JNIEnv *env, jobject instance,
                                                                jstring filterName_) {
    const char *filterName = (*env)->GetStringUTFChars(env, filterName_, 0);
    mFilterName = (char *) filterName;
}

JNIEXPORT jobject JNICALL
Java_com_faceunity_fulivenativedemo_FURenderer_onBlurLevelSelected(JNIEnv *env, jobject instance,
                                                                   jint level) {
    switch (level) {
        case 0:
            mFacebeautyBlurLevel = 0;
            break;
        case 1:
            mFacebeautyBlurLevel = 1.0f;
            break;
        case 2:
            mFacebeautyBlurLevel = 2.0f;
            break;
        case 3:
            mFacebeautyBlurLevel = 3.0f;
            break;
        case 4:
            mFacebeautyBlurLevel = 4.0f;
            break;
        case 5:
            mFacebeautyBlurLevel = 5.0f;
            break;
        case 6:
            mFacebeautyBlurLevel = 6.0f;
            break;
        default:
            break;
    }
}

JNIEXPORT jobject JNICALL
Java_com_faceunity_fulivenativedemo_FURenderer_onColorLevelSelected(JNIEnv *env, jobject instance,
                                                                    jint progress, jint max) {
    mFacebeautyColorLevel = 1.0f * progress / max;
}

JNIEXPORT jobject JNICALL
Java_com_faceunity_fulivenativedemo_FURenderer_onCheekThinSelected(JNIEnv *env, jobject instance,
                                                                   jint progress, jint max) {
    mFacebeautyCheeckThin = 1.0f * progress / max;
}

JNIEXPORT jobject JNICALL
Java_com_faceunity_fulivenativedemo_FURenderer_onEnlargeEyeSelected(JNIEnv *env, jobject instance,
                                                                    jint progress, jint max) {
    mFacebeautyEnlargeEye = 1.0f * progress / max;
}

JNIEXPORT jobject JNICALL
Java_com_faceunity_fulivenativedemo_FURenderer_onFaceShapeSelected(JNIEnv *env, jobject instance,
                                                                   jint faceShape) {
    mFaceShape = faceShape;
}

JNIEXPORT jobject JNICALL
Java_com_faceunity_fulivenativedemo_FURenderer_onFaceShapeLevelSelected(JNIEnv *env,
                                                                        jobject instance,
                                                                        jint progress, jint max) {
    mFaceShapeLevel = (1.0f * progress) / max;
}

JNIEXPORT jobject JNICALL
Java_com_faceunity_fulivenativedemo_FURenderer_onRedLevelSelected(JNIEnv *env, jobject instance,
                                                                  jint progress, jint max) {
    mFacebeautyRedLevel = 1.0f * progress / max;
}
