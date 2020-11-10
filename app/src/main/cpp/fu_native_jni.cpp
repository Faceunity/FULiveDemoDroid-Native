/* Header for class com_faceunity_fulivenativedemo_FuNativeJni */

#include "authpack.h"
#include "android_util.h"
#include "faceunity_renderer.h"
#include "android_native_interface.h"

#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_faceunity_fulivenativedemo_FuNativeJni
 * Method:    setup
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_faceunity_fulivenativedemo_FuNativeJni_setup
        (JNIEnv *env, jclass type) {
    fuSetLogLevel(FU_LOG_LEVEL_WARN);
    fuAndroidNativeCreateEGLContext();
    setup(g_auth_package, sizeof(g_auth_package) / sizeof(*g_auth_package));
    LOGI("setup. CNamaSDK version: %s", fuGetVersion());
    fuAndroidNativeReleaseEGLContext();
}
/*
 * Class:     com_faceunity_fulivenativedemo_FuNativeJni
 * Method:    destroy
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_faceunity_fulivenativedemo_FuNativeJni_destroy
        (JNIEnv *env, jclass type) {
    fuDestroyLibData();
    LOGD("fuDestroyLibData");
}

/*
 * Class:     com_faceunity_fulivenativedemo_FuNativeJni
 * Method:    loadAiModel
 * Signature: (Ljava/lang/Object;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_faceunity_fulivenativedemo_FuNativeJni_loadAiModel
        (JNIEnv *env, jclass type, jobject assetsManager, jstring filePath, jint aiType) {
    jboolean isCopy = JNI_FALSE;
    const char *dataPath = env->GetStringUTFChars(filePath, &isCopy);
    void *dataPtr;
    int dataSize;
    readAssets(env, assetsManager, dataPath, &dataPtr, &dataSize);
    int ret = fuLoadAIModelFromPackage(dataPtr, dataSize, (FUAITYPE) aiType);
    free(dataPtr);
    env->ReleaseStringUTFChars(filePath, dataPath);
    LOGD("fuLoadAIModel filePath: %s, aiType: %d, result: %d", dataPath, aiType, ret);
}

/*
 * Class:     com_faceunity_fulivenativedemo_FuNativeJni
 * Method:    releaseAiModel
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_faceunity_fulivenativedemo_FuNativeJni_releaseAiModel
        (JNIEnv *env, jclass type, jint aiType) {
    int ret = 0;
    if (fuIsAIModelLoaded((FUAITYPE) aiType)) {
        ret = fuReleaseAIModel((FUAITYPE) aiType);
    }
    LOGD("fuReleaseAIModel result: %d", ret);
}

/*
 * Class:     com_faceunity_fulivenativedemo_FuNativeJni
 * Method:    onSurfaceCreated
 * Signature: (Ljava/lang/Object;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_faceunity_fulivenativedemo_FuNativeJni_onSurfaceCreated
        (JNIEnv *env, jobject obj, jobject assetsManager, jstring faceBeautyPath) {
    jboolean isCopy = JNI_FALSE;
    const char *dataPath = env->GetStringUTFChars(faceBeautyPath, &isCopy);
    onSurfaceCreated(env, assetsManager, dataPath);
    env->ReleaseStringUTFChars(faceBeautyPath, dataPath);
}

/*
 * Class:     com_faceunity_fulivenativedemo_FuNativeJni
 * Method:    onSurfaceChanged
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_com_faceunity_fulivenativedemo_FuNativeJni_onSurfaceChanged
        (JNIEnv *env, jobject obj, jint width, jint height) {
    onSurfaceChanged(0, 0, width, height);
}

/*
 * Class:     com_faceunity_fulivenativedemo_FuNativeJni
 * Method:    onSurfaceDestroyed
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_faceunity_fulivenativedemo_FuNativeJni_onSurfaceDestroyed
        (JNIEnv *env, jobject obj) {
    onSurfaceDestroy();
}

/*
 * Class:     com_faceunity_fulivenativedemo_FuNativeJni
 * Method:    onCameraChanged
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_com_faceunity_fulivenativedemo_FuNativeJni_onCameraChanged
        (JNIEnv *env, jobject obj, jint cameraFacing, jint cameraOrientation) {
    onCameraChanged(cameraFacing, cameraOrientation);
}

/*
 * Class:     com_faceunity_fulivenativedemo_FuNativeJni
 * Method:    onDeviceOrientationChanged
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_faceunity_fulivenativedemo_FuNativeJni_onDeviceOrientationChanged
        (JNIEnv *env, jobject obj, jint deviceOrientation) {
    onDeviceOrientationChanged(deviceOrientation);
}

/*
 * Class:     com_faceunity_fulivenativedemo_FuNativeJni
 * Method:    onDrawFrame
 * Signature: ([BIII[F)V
 */
JNIEXPORT void JNICALL Java_com_faceunity_fulivenativedemo_FuNativeJni_onDrawFrame
        (JNIEnv *env, jobject obj, jbyteArray nv21, jint texId, jint width, jint height,
         jfloatArray texMatrix) {
    jboolean isCopy = JNI_FALSE;
    jbyte *img = env->GetByteArrayElements(nv21, &isCopy);
    jfloat *mtx = env->GetFloatArrayElements(texMatrix, &isCopy);

    onDrawFrame(img, texId, width, height, mtx);

    env->ReleaseByteArrayElements(nv21, img, JNI_ABORT);
    env->ReleaseFloatArrayElements(texMatrix, mtx, JNI_ABORT);
}

/*
 * Class:     com_faceunity_fulivenativedemo_FuNativeJni
 * Method:    resetStatus
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_faceunity_fulivenativedemo_FuNativeJni_resetStatus
        (JNIEnv *env, jobject obj) {
    resetStatus();
}

/*
 * Class:     com_faceunity_fulivenativedemo_FuNativeJni
 * Method:    onStickerSelected
 * Signature: (Ljava/lang/Object;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_faceunity_fulivenativedemo_FuNativeJni_onStickerSelected
        (JNIEnv *env, jobject obj, jobject assetsManager, jstring path) {
    jboolean isCopy = JNI_FALSE;
    const char *dataPath = env->GetStringUTFChars(path, &isCopy);
    createSticker(env, assetsManager, dataPath);
    env->ReleaseStringUTFChars(path, dataPath);
}

/*
 * Class:     com_faceunity_fulivenativedemo_FuNativeJni
 * Method:    onFilterLevelSelected
 * Signature: (F)V
 */
JNIEXPORT void JNICALL Java_com_faceunity_fulivenativedemo_FuNativeJni_onFilterLevelSelected
        (JNIEnv *env, jobject obj, jfloat level) {
    onFilterLevelSelected(level);
}

/*
 * Class:     com_faceunity_fulivenativedemo_FuNativeJni
 * Method:    onFilterNameSelected
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_faceunity_fulivenativedemo_FuNativeJni_onFilterNameSelected
        (JNIEnv *env, jobject obj, jstring name) {
    jboolean isCopy = JNI_FALSE;
    char *cName = const_cast<char *>(env->GetStringUTFChars(name, &isCopy));
    onFilterSelected(cName);
    env->ReleaseStringUTFChars(name, cName);
}

/*
 * Class:     com_faceunity_fulivenativedemo_FuNativeJni
 * Method:    onAllBlurLevelSelected
 * Signature: (F)V
 */
JNIEXPORT void JNICALL Java_com_faceunity_fulivenativedemo_FuNativeJni_onAllBlurLevelSelected
        (JNIEnv *env, jobject obj, jfloat level) {
    onAllBlurLevelSelected(level);
}

/*
 * Class:     com_faceunity_fulivenativedemo_FuNativeJni
 * Method:    onBeautySkinTypeSelected
 * Signature: (F)V
 */
JNIEXPORT void JNICALL Java_com_faceunity_fulivenativedemo_FuNativeJni_onBeautySkinTypeSelected
        (JNIEnv *env, jobject obj, jfloat type) {
    onBeautySkinTypeSelected(type);
}

/*
 * Class:     com_faceunity_fulivenativedemo_FuNativeJni
 * Method:    onBlurLevelSelected
 * Signature: (F)V
 */
JNIEXPORT void JNICALL Java_com_faceunity_fulivenativedemo_FuNativeJni_onBlurLevelSelected
        (JNIEnv *env, jobject obj, jfloat level) {
    onBlurLevelSelected(level);
}

/*
 * Class:     com_faceunity_fulivenativedemo_FuNativeJni
 * Method:    onColorLevelSelected
 * Signature: (F)V
 */
JNIEXPORT void JNICALL Java_com_faceunity_fulivenativedemo_FuNativeJni_onColorLevelSelected
        (JNIEnv *env, jobject obj, jfloat level) {
    onColorLevelSelected(level);
}

/*
 * Class:     com_faceunity_fulivenativedemo_FuNativeJni
 * Method:    onRedLevelSelected
 * Signature: (F)V
 */
JNIEXPORT void JNICALL Java_com_faceunity_fulivenativedemo_FuNativeJni_onRedLevelSelected
        (JNIEnv *env, jobject obj, jfloat level) {
    onRedLevelSelected(level);
}

/*
 * Class:     com_faceunity_fulivenativedemo_FuNativeJni
 * Method:    onBrightEyesSelected
 * Signature: (F)V
 */
JNIEXPORT void JNICALL Java_com_faceunity_fulivenativedemo_FuNativeJni_onBrightEyesSelected
        (JNIEnv *env, jobject obj, jfloat level) {
    onBrightEyesSelected(level);
}

/*
 * Class:     com_faceunity_fulivenativedemo_FuNativeJni
 * Method:    onBeautyTeethSelected
 * Signature: (F)V
 */
JNIEXPORT void JNICALL Java_com_faceunity_fulivenativedemo_FuNativeJni_onBeautyTeethSelected
        (JNIEnv *env, jobject obj, jfloat level) {
    onBeautyTeethSelected(level);
}

/*
 * Class:     com_faceunity_fulivenativedemo_FuNativeJni
 * Method:    onFaceShapeSelected
 * Signature: (F)V
 */
JNIEXPORT void JNICALL Java_com_faceunity_fulivenativedemo_FuNativeJni_onFaceShapeSelected
        (JNIEnv *env, jobject obj, jfloat level) {
    onFaceShapeSelected(level);
}

/*
 * Class:     com_faceunity_fulivenativedemo_FuNativeJni
 * Method:    onEnlargeEyeSelected
 * Signature: (F)V
 */
JNIEXPORT void JNICALL Java_com_faceunity_fulivenativedemo_FuNativeJni_onEnlargeEyeSelected
        (JNIEnv *env, jobject obj, jfloat level) {
    onEnlargeEyeSelected(level);
}

/*
 * Class:     com_faceunity_fulivenativedemo_FuNativeJni
 * Method:    onCheekThinSelected
 * Signature: (F)V
 */
JNIEXPORT void JNICALL Java_com_faceunity_fulivenativedemo_FuNativeJni_onCheekThinSelected
        (JNIEnv *env, jobject obj, jfloat level) {
    onCheekThinSelected(level);
}

/*
 * Class:     com_faceunity_fulivenativedemo_FuNativeJni
 * Method:    onChinLevelSelected
 * Signature: (F)V
 */
JNIEXPORT void JNICALL Java_com_faceunity_fulivenativedemo_FuNativeJni_onChinLevelSelected
        (JNIEnv *env, jobject obj, jfloat level) {
    onChinLevelSelected(level);
}

/*
 * Class:     com_faceunity_fulivenativedemo_FuNativeJni
 * Method:    onForeheadLevelSelected
 * Signature: (F)V
 */
JNIEXPORT void JNICALL Java_com_faceunity_fulivenativedemo_FuNativeJni_onForeheadLevelSelected
        (JNIEnv *env, jobject obj, jfloat level) {
    onForeheadLevelSelected(level);
}

/*
 * Class:     com_faceunity_fulivenativedemo_FuNativeJni
 * Method:    onThinNoseLevelSelected
 * Signature: (F)V
 */
JNIEXPORT void JNICALL Java_com_faceunity_fulivenativedemo_FuNativeJni_onThinNoseLevelSelected
        (JNIEnv *env, jobject obj, jfloat level) {
    onThinNoseLevelSelected(level);
}

/*
 * Class:     com_faceunity_fulivenativedemo_FuNativeJni
 * Method:    onMouthShapeSelected
 * Signature: (F)V
 */
JNIEXPORT void JNICALL Java_com_faceunity_fulivenativedemo_FuNativeJni_onMouthShapeSelected
        (JNIEnv *env, jobject obj, jfloat level) {
    onMouthShapeSelected(level);
}

#ifdef __cplusplus
}
#endif
