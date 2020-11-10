#include "android_util.h"

void readAssets(JNIEnv *env, jobject assetManager, const char *file, void **data, int *size) {
    AAssetManager *mgr = AAssetManager_fromJava(env, assetManager);
    if (mgr == NULL) {
        LOGE("readAssets AAssetManager == NULL");
    }

    AAsset *asset = AAssetManager_open(mgr, file, AASSET_MODE_STREAMING);
    if (asset == NULL) {
        LOGE("readAssets asset == NULL");
    }

    off_t length = AAsset_getLength(asset);
    *data = malloc(length);
    *size = AAsset_read(asset, *data, length);
    AAsset_close(asset);
}