#ifndef FUAPIDEMO_ANDROID_NATIVE_INTERFACE_H
#define FUAPIDEMO_ANDROID_NATIVE_INTERFACE_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/system_properties.h>
#include <GLES2/gl2.h>
#include "CNamaSDK.h"

#define ANDROID_NATIVE_NAMA_RENDER_OPTION_FLIP_X 32
#define ANDROID_NATIVE_NAMA_RENDER_OPTION_FLIP_Y 64

#define FU_ADM_FLAG_TEXTURE_AND_READBACK_BUFFER_OPPOSITE_X 256
#define FU_ADM_FLAG_TEXTURE_AND_READBACK_BUFFER_OPPOSITE_Y 512

#define FU_ADM_FLAG_TEXTURE_AND_READBACK_BUFFER_ROTATE_90 (1 << 10)
#define FU_ADM_FLAG_TEXTURE_AND_READBACK_BUFFER_ROTATE_180 (1 << 11)
#define FU_ADM_FLAG_TEXTURE_AND_READBACK_BUFFER_ROTATE_270 (1 << 12)

#define FU_ADM_FLAG_TEXTURE_ROTATE_90 (1 << 13)
#define FU_ADM_FLAG_TEXTURE_ROTATE_180 (1 << 14)
#define FU_ADM_FLAG_TEXTURE_ROTATE_270 (1 << 15)

#define ANDROID_NATIVE_NAMA_NOCLEAR_CURRENT_FRAMEBUFFER (1 << 16)

#ifdef __cplusplus
extern "C" {
#endif

int fuAndroidNativeSetup(void *v3data_ptr, int v3data_lg, void *authdata_ptr,
                         int authdata_lg);

int fuAndroidNativeSetupLocal(void *v3data_ptr, int v3data_lg,
                              void *authdata_ptr, int authdata_lg,
                              void **offline_bundle_ptr,
                              int *offline_bundle_sz);

int fuAndroidNativeSetupInternalCheck(void *v3data_ptr, int v3data_lg,
                                      void *authdata_ptr, int authdata_lg);

void fuAndroidNativeClearReadbackRelated();

void fuAndroidNativeOnDeviceLost();

void fuAndroidNativeOnDeviceLostSafe();

void fuAndroidNativeDestroyAllItems();

int fuAndroidRenderToTexture(int tex_in, int w, int h, int frame_id,
                             int *p_items, int n_items);

int fuAndroidNativeRenderNV21ImageToTexture(void *img_nv21, int w, int h,
                                            int frame_id, int *p_items,
                                            int n_items);

int fuAndroidNativeCreateItemFromPackage(void *data, int sz);

void fuAndroidNativeDone();

int fuAndroidNativeHasFace();

int fuAndroidNativeAvatarToCurrentFBO(float *pupil_pos, float *expression,
                                      float *rotation, float *rmode,
                                      float *trans, int flags, int w, int h,
                                      int frame_id, int *items, int items_lg,
                                      int isTracking, int orientation);

void fuAndroidNativeDestroyItem(int item);

void fuAndroidNativeDestroyLibData();

int fuAndroidNativeBeautifyImage(int tex_in, int flags, int w, int h,
                                 int frame_id, int *items_ptr, int items_lg);

int fuAndroidNativeItemSetParamd(int item, char *name, double value);

/**
\brief Set an item parameter to a double array
\param item specifies the item
\param name is the parameter name
\param value points to an array of doubles
\param n specifies the number of elements in value
\return zero for failure, non-zero for success
*/
int fuAndroidNativeItemSetParamdv(int item, char *name, double *value, int n);

/**
\brief Set an item parameter to a string value
\param item specifies the item
\param name is the parameter name
\param value is the parameter value to be set
\return zero for failure, non-zero for success
*/
int fuAndroidNativeItemSetParams(int item, char *name, char *value);

/**
\brief Get an item parameter as a double value
\param item specifies the item
\param name is the parameter name
\return double value of the parameter
*/
double fuAndroidNativeItemGetParamd(int item, char *name);

/**
\brief Get an item parameter as a string
\param item specifies the item
\param name is the parameter name
\param buf receives the string value
\param sz is the number of bytes available at buf
\return the length of the string value, or -1 if the parameter is not a string.
*/
int fuAndroidNativeItemGetParams(int item, char *name, char *buf, int sz);

int fuAndroidNativeItemGetParamdv(int item, char *name, double *buf, int n);

int fuAndroidNativeIsTracking();

int fuAndroidNativeSetMaxFaces(int n);

void fuAndroidNativeDisableBoostWithEGLImage();

void fuAndroidNativeReleaseEGLContext();

void fuAndroidNativeCreateEGLContext();

void fuAndroidNativeSetDefaultOrientation(int rmode);

void readbackFromTexture(int readback_w, int readback_h, void *img,
                         void *readback_custom_img, int is_readback_custom,
                         int fuTexId, int frame_id, int flags);

int fuAndroidNativeDualInputToTexture(void *img, GLuint tex_in, int flags,
                                      int w, int h, int frame_id, int *items,
                                      int items_lg, int *masks, int readback_w,
                                      int readback_h, void *readback_custom_img,
                                      int is_readback_custom, int readback_x,
                                      int readback_y);

int fuAndroidNativeDualInputToFBO(void *img, GLuint tex_in, int flags, int w,
                                  int h, int frame_id, int *items, int items_lg,
                                  int *masks, int readback_w, int readback_h,
                                  void *readback_custom_img,
                                  int is_readback_custom, int readback_x,
                                  int readback_y, int orientation);

int fuAndroidNativeRenderToTexture(int tex_in, int w, int h, int frame_id,
                                   int *items, int item_lg, int flags,
                                   void *readback_custom_img,
                                   int readback_custom_w, int readback_custom_h,
                                   int readback_x, int readback_y);

int fuAndroidNativeRenderToNV21Image(void *img, int img_lg, int w, int h,
                                     int frame_id, int *items, int items_lg,
                                     int flags, int readback_w, int readback_h,
                                     void *readback_custom_img,
                                     int is_readback_custom, int readback_x,
                                     int readback_y);

int fuAndroidNativeRenderToNV21ImageMasked(void *img, int img_lg, int w, int h,
                                           int frame_id, int *items,
                                           int items_lg, int flags, int *masks,
                                           int readback_w, int readback_h,
                                           void *readback_custom_img,
                                           int is_readback_custom,
                                           int readback_x, int readback_y);

int fuAndroidNativeRenderToYUVImage(void *y_buffer, void *u_buffer,
                                    void *v_buffer, int ybuffer_stride,
                                    int ubuffer_stride, int vbuffer_stride,
                                    int w, int h, int frame_id, int *items,
                                    int items_lg, int flags);

int fuAndroidNativeAvatarToTexture(float *pupil_pos, float *expression,
                                   float *rotation, float *rmode, float *trans,
                                   int flags, int w, int h, int frame_id,
                                   int *items, int items_lg, int isTracking);

int fuAndroidNativeAvatarToTextureWithTrans(float *trans, float *pupil_pos,
                                            float *expression, float *rotation,
                                            float *rmode, int flags, int w,
                                            int h, int frame_id, int *items,
                                            int items_lg, int isTracking);

int fuAndroidNativeAvatarToImage(float *pupil_pos, float *expression,
                                 float *rotation, float *rmode, float *trans,
                                 int flags, int w, int h, int frame_id,
                                 int *items, int items_lg, int isTracking,
                                 int readback_w, int readback_h,
                                 void *readback_custom_img);

int fuAndroidNativeRenderToI420Image(void *img, int img_lg, int w, int h,
                                     int frame_id, int *items, int items_lg,
                                     int flags, int readback_w, int readback_h,
                                     void *readback_custom_img,
                                     int is_readback_custom, int readback_x,
                                     int readback_y);

int fuAndroidNativeRenderToI420ImageMasked(void *img, int img_lg, int w, int h,
                                           int frame_id, int *items,
                                           int items_lg, int flags, int *masks,
                                           int readback_w, int readback_h,
                                           void *readback_custom_img,
                                           int is_readback_custom,
                                           int readback_x, int readback_y);

int fuAndroidNativeRenderI420ImageToTexture(void *img_i420, int w, int h,
                                            int frame_id, int *p_items,
                                            int n_items);

int fuAndroidNativeRenderToRgbaImage(void *img, int img_lg, int w, int h,
                                     int frame_id, int *items, int items_lg,
                                     int flags, int readback_w, int readback_h,
                                     void *readback_custom_img,
                                     int is_readback_custom, int readback_x,
                                     int readback_y);

int fuAndroidNativeRenderBundlesWithNV21Image(void *img_nv21, int w, int h,
                                              int frame_id, int *p_items,
                                              int n_items);

int fuAndroidNativeRenderBundlesWithAvatarInfo(TAvatarInfo *info, int flags,
                                               int w, int h, int frame_id,
                                               int *items, int items_lg);

int fuAndroidNativeRenderBundlesWithAvatarInfoToCurrentFBO(
        TAvatarInfo *avatar_info, int flags, int w, int h, int frame_id, int *items,
        int items_lg, int orientation);

int fuAndroidNativeRenderBundles(void *img, GLuint tex_in, int flags, int w,
                                 int h, int frame_id, int *items, int items_lg,
                                 int *masks, int readback_w, int readback_h,
                                 void *readback_custom_img,
                                 int is_readback_custom, int readback_x,
                                 int readback_y);

int fuAndroidNativeRenderBundlesSplitView(TAvatarInfo *info, int flags, int w,
                                          int h, int frame_id, int *items,
                                          int items_lg,
                                          TSplitViewInfo *split_view_info);

int fuAndroidNativeAvatarBindItems(int avatar_item, int *p_items, int n_items,
                                   int *p_contracts, int n_contracts);

int fuAndroidNativeAvatarUnbindItems(int avatar_item, int *p_items,
                                     int n_items);

const char *fuAndroidNativGetVersion();

void fuAndroidNativSetQualityTradeoff(float quality);

int fuAndroidNativeBindItems(int item_src, int *p_items, int n_items);

int fuAndroidNativeUnBindItems(int item_src, int *p_items, int n_items);

int fuAndroidNativeUnbindAllItems(int item_src);

const int fuAndroidNativeGetSystemError();

const char *fuAndroidNativeGetSystemErrorString(int code);

int fuAndroidNativeCheckDebugItem(void *data, int sz);

int fuAndroidNativeSetMultiSamples(int sample);

int fuAndroidNativeSetAsyncTrackFace(int mode);

int fuAndroidNativeSetTongueTracking(int mode);

int fuAndroidNativeCreateTexForItem(int item, char *name, void *value,
                                    int width, int height);

int fuAndroidNativeDeleteTexForItem(int item, char *name);

int fuAndroidNativeReleaseNV21Buffer();

#ifdef __cplusplus
}
#endif

#endif  // FUAPIDEMO_ANDROID_NATIVE_INTERFACE_H
