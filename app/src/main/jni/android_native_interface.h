#ifndef FUAPIDEMO_ANDROID_NATIVE_INTERFACE_H
#define FUAPIDEMO_ANDROID_NATIVE_INTERFACE_H

int fuAndroidNativeSetup(void *v3data_ptr, int v3data_lg, void *authdata_ptr, int authdata_lg);

void fuAndroidNativeOnDeviceLost();

void fuAndroidNativeDestroyAllItems();

int fuAndroidRenderToTexture(int tex_in, int w, int h, int frame_id, int *p_items, int n_items);

int
fuAndroidNativeRenderNV21ImageToTexture(void *img_nv21, int w, int h, int frame_id, int *p_items,
                                        int n_items);

int fuAndroidNativeCreateItemFromPackage(void *data, int sz);

void fuAndroidNativeDone();

void fuAndroidNativeDestroyItem(int item);

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

int fuAndroidNativeIsTracking();

int fuAndroidNativeSetMaxFaces(int n);

void fuAndroidNativeDisableBoostWithEGLImage();

void fuAndroidNativeReleaseEGLContext();

void fuAndroidNativeCreateEGLContext();

int fuAndroidNativeDualInputToTexture(void *img, int tex_in, int flags, int w, int h, int frame_id,
                                      int *items, int items_lg, int *masks, int readback_w,
                                      int readback_h, void *readback_custom_img,
                                      int is_readback_custom);

int fuAndroidNativeRenderToNV21Image(void *img, int img_lg, int w, int h, int frame_id, int *items,
                                     int items_lg, int flags, int readback_w, int readback_h,
                                     void *readback_custom_img, int is_readback_custom);

int
fuAndroidNativeAvatarToTexture(float *pupil_pos, float *expression, float *rotation, float *rmode,
                               int flags, int w, int h, int frame_id, int *items, int items_lg,
                               int isTracking);

int fuAndroidNativeAvatarToImage(float *pupil_pos, float *expression, float *rotation, float *rmode,
                                 int flags, int w, int h, int frame_id, int *items, int items_lg,
                                 int isTracking, int readback_w, int readback_h,
                                 void *readback_custom_img);

int fuAndroidNativeRenderToI420Image(void *img, int img_lg, int w, int h, int frame_id, int *items,
                                     int items_lg, int flags, int readback_w, int readback_h,
                                     void *readback_custom_img, int is_readback_custom);

int
fuAndroidNativeRenderI420ImageToTexture(void *img_i420, int w, int h, int frame_id, int *p_items,
                                        int n_items);

int fuAndroidNativeRenderToRgbaImage(void *img, int img_lg, int w, int h, int frame_id, int *items,
                                     int items_lg, int flags, int readback_w, int readback_h,
                                     void *readback_custom_img, int is_readback_custom);

int fuAndroidNativeAvatarBindItems(int avatar_item, int *p_items, int n_items, int *p_contracts,
                                   int n_contracts);

int fuAndroidNativeAvatarUnbindItems(int avatar_item, int *p_items, int n_items);

char *fuAndroidNativGetVersion();

int fuAndroidNativeBindItems(int item_src, int *p_items, int n_items);

int fuAndroidNativeUnbindAllItems(int item_src);

void fuOnCameraChange();

#endif //FUAPIDEMO_ANDROID_NATIVE_INTERFACE_H


