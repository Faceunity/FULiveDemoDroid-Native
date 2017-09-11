# FuLiveNativeDemoDroid(Android)

## 概述

FuLiveNativeDemoDroid 是集成了 Faceunity 面部跟踪和虚拟道具功能基于 android NDK 环境开发的demo。该demo在java中实现布局以及camera对数据的采集，并在native层实现faceunity的基本功能以及画面的绘制。

## SDK使用介绍

 - Faceunity SDK的使用方法请参看 [**Faceunity/FULiveDemoDroid**][1]

## 集成方法

添加库文件 libnama.so 、v3.mp3 、face_beautification.mp3 以及一系列道具 bundle 。faceunity 的控制部分界面主要集成在 GLSurfaceView 与 FaceunityControlView 中。而在 FURenderer 中实现了一系列 native 方法来完成 demo 的核心功能。

### Java层

Java层主要实现了camera 的管理以及界面部分 。camera的管理主要几种在 CameraUtils 类中，界面主要有 GLSurfaceView 来显示画面和 FaceunityControlView 来控制画面两部分组成。FaceunityControlView 集中了 FaceunityControlView.OnViewEventListener 接口的一系列控制操作与 FURenderer 中的 native 代码一一对应，而 GLSurfaceView 主要实现了 GLSurfaceView.Renderer 接口，用于管理GLSurfaceView各个生命周期。

#### onSurfaceCreated

```
//获取glThreadI
glThreadId = Thread.currentThread().getId();

//初始化camera
getCamera(cameraType, cameraWidth, cameraHeight);
//开启camera
startCamera();
//调用相对应的FURenderer的onSurfaceCreated方法
mFURenderer.onSurfaceCreated(getAssets(), authpack.A());
```

#### onSurfaceChanged

在onSurfaceChanged中仅需要完成对FURenderer.onSurfaceChanged的调用。

```
mFURenderer.onSurfaceChanged(i, i1);
```

#### onDrawFrame

驱动SurfaceTexture获取纹理，并完成对FURenderer.onDrawFrame的调用。

```
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
```

#### onSurfaceDestory

在Java层的 onSurfaceDestory 主要寄生于 activity 的 onPause 中。

```
mGLSurfaceView.queueEvent(new Runnable() {
    @Override
    public void run() {
        mFURenderer.onSurfaceDestory();
    }
});
releaseCamera();
```

### Native层

Native 层只有一个核心类 FURenderer，作为 Java 层与 Native 层的桥梁。FURenderer 中有一系列用于处理 faceunity 以及 opengl 绘制工作的 native 方法（对应的 native 代码都在faceunity_renderer.c中具体实现），这些方法中包括用于响应 FaceunityControlView 的事件的 FaceunityControlView.OnViewEventListener 接口实现以及一系列的 native 方法,如下：

#### onSurfaceCreated

```
/**
 * 需要GL环境，初始化了faceunity相关的数据。
 *
 * @param manager 底层需要AssetManager用于从assets资源文件夹中读取v3.mp3、face_beautification.mp3以及道具bundle
 * @param authpack 证书数据，用于验证faceunity鉴权。
 */
public native void onSurfaceCreated(AssetManager manager, byte[] authpack);
```

相对应的native代码

```
//初始化opengl绘画需要的program
createProgram();

jbyte *authdata_ptr = (*env)->GetByteArrayElements(env, authpack_, 0);
jsize authdata_lg = (*env)->GetArrayLength(env, authpack_);

//读取assets中的v3.mp3数据，并初始化
void *v3;
int v3Size;
readAssets(env, manager, "v3.mp3", &v3, &v3Size);
fuAndroidNativeSetup(v3, v3Size, authdata_ptr, authdata_lg);

//读取assets中的face_beautification.mp3数据，并初始化
void *beautification;
int beautificationSize;
readAssets(env, manager, "face_beautification.mp3", &beautification, &beautificationSize);
itemsArray[0] = fuAndroidNativeCreateItemFromPackage(beautification, beautificationSize);

//判断当前显示的道具状态，并加载相应的道具
createEffect(env, manager, mEffectName);

(*env)->ReleaseByteArrayElements(env, authpack_, authdata_ptr, 0);
```

#### onSurfaceChanged

```
/**
 * 需要GL环境,应在SurfaceView大小改变时被调用
 * 
 * @param w
 * @param h
 */
public native void onSurfaceChanged(int w, int h);
```

相对应的native代码

```
glViewport(0, 0, wight, height);
```

#### onDrawFrame

```
/**
 * 需要GL环境，接收每帧图像纹理与byte[]数据，绘制画面
 * @param img 图像byte[]数据
 * @param textureId 图像纹理
 * @param weight 
 * @param height
 * @param mtx 画面方向旋转需要的矩阵
 */
public native void onDrawFrame(byte[] img, int textureId, int weight, int height, float[] mtx);
```

相对应的native代码

```
jbyte *img = (*env)->GetByteArrayElements(env, img_, NULL);
jfloat *mtx = (*env)->GetFloatArrayElements(env, mtx_, NULL);

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
```

#### onSurfaceDestory

```
/**
 * 需要GL环境，SurfaceView销毁时被调用，释放faceunity相关的资源
 */
public native void onSurfaceDestory();
```

相对应的native代码

```
fuAndroidNativeDestroyItem(itemsArray[1]);
itemsArray[1] = itemsArray[1] = 0;
fuAndroidNativeDestroyItem(itemsArray[0]);
itemsArray[0] = itemsArray[0] = 0;
fuAndroidNativeDestroyAllItems();
fuAndroidNativeOnDeviceLost();
```

#### switchCamera

```
/**
 * 需要GL环境，切换摄像头时重置一些native数据
 */
public native void switchCamera();
```

相对应的native代码

```
fuOnCameraChange();
```

#### resetStatus

```
/**
 * 不需要GL环境，重置美颜、滤镜以及道具数据
 */
public native void resetStatus();
```

相对应的native代码

```
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
```

#### 道具加载

为了提高加载效率，采用异步加载的方式。在java层管理异步线程以及加载消息，在native实现具体加载过程。

##### 道具加载java层的异步管理

CreateItemHandler

```
class CreateItemHandler extends Handler {

    public static final int CREATE_ITEM_WHAT = 1;

    public CreateItemHandler(Looper looper) {
        super(looper);
    }

    @Override
    public void handleMessage(Message msg) {
        if (msg.what == CREATE_ITEM_WHAT)
            mOnViewEventListener.onEffectItemSelected(mContext.getAssets(), 
                        EffectAndFilterSelectAdapter.EFFECT_ITEM_FILE_NAME[msg.arg1]);
    }
}
```

加载道具异步线程的初始化

```
mCreateItemThread = new HandlerThread("CreateItemThread");
mCreateItemThread.start();
mCreateItemHandler = new CreateItemHandler(mCreateItemThread.getLooper());
```

切换道具事件触发时，发送给CreateItemThread进行处理代码

```
mCreateItemHandler.removeMessages(CreateItemHandler.CREATE_ITEM_WHAT);
mCreateItemHandler.sendMessage(Message.obtain(mCreateItemHandler,
            CreateItemHandler.CREATE_ITEM_WHAT, itemPosition, 0));
```

##### 道具加载native代码

```
void createEffect(JNIEnv *env, jobject assetManager, char *name) {

    if (strncmp("none", name, 4) == 0) {
        fuAndroidNativeDestroyItem(itemsArray[1]);
        itemsArray[1] = 0;
    } else {
        int temp = itemsArray[1];
        void *effectData;
        int effectSize;
        readAssets(env, assetManager, name, &effectData, &effectSize);
        itemsArray[1] = fuAndroidNativeCreateItemFromPackage(effectData, effectSize);
        if (temp != 0) {
            fuAndroidNativeDestroyItem(temp);
        }
    }
}
```



  [1]: https://github.com/Faceunity/FULiveDemoDroid