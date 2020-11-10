//
// Created by Jiahua Tu on 2018/4/19.
//
#ifndef FULIVENATIVEDEMO_RENDERER_H
#define FULIVENATIVEDEMO_RENDERER_H

#include <jni.h>
#include <string.h>
#include <stdlib.h>
#include "gl_utils.h"
#include "android_util.h"
#include "include/android_native_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *
 * @param auth
 * @param size
 * @return 0 for success, 1 for inited
 */
int setup(void *auth, int size);

void createSticker(JNIEnv *env, jobject assetsManager, const char *dataPath);

void onSurfaceCreated(JNIEnv *env, jobject assetsManager, const char *dataPath);

void onSurfaceChanged(int x, int y, int width, int height);

void onDrawFrame(void *img, int textureId, int width, int height, float *texMatrix);

void onSurfaceDestroy();

void onCameraChanged(int cameraFacing, int cameraOrientation);

void onDeviceOrientationChanged(int deviceOrientation);

void resetStatus();

void onFilterLevelSelected(float progress);

void onFilterSelected(char *name);

void onAllBlurLevelSelected(float isAll);

void onBeautySkinTypeSelected(float skinType);

void onBlurLevelSelected(float level);

void onColorLevelSelected(float progress);

void onRedLevelSelected(float progress);

void onBrightEyesSelected(float progress);

void onBeautyTeethSelected(float progress);

void onFaceShapeSelected(float faceShape);

void onEnlargeEyeSelected(float progress);

void onCheekThinSelected(float progress);

void onChinLevelSelected(float progress);

void onForeheadLevelSelected(float progress);

void onThinNoseLevelSelected(float progress);

void onMouthShapeSelected(float progress);

#ifdef __cplusplus
}
#endif
#endif //FULIVENATIVEDEMO_RENDERER_H
