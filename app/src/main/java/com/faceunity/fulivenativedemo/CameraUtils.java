package com.faceunity.fulivenativedemo;

import android.app.Activity;
import android.hardware.Camera;
import android.util.Log;
import android.view.Surface;

import java.util.List;

/**
 * Camera-related utility functions.
 */
@SuppressWarnings("deprecation")
public abstract class CameraUtils {
    private static final String TAG = "CameraUtils";

    public static void setCameraDisplayOrientation(Activity activity, int cameraId, Camera camera) {
        Camera.CameraInfo info = new Camera.CameraInfo();
        Camera.getCameraInfo(cameraId, info);
        int rotation = activity.getWindowManager().getDefaultDisplay().getRotation();
        int degrees = 0;
        switch (rotation) {
            case Surface.ROTATION_0:
                degrees = 0;
                break;
            case Surface.ROTATION_90:
                degrees = 90;
                break;
            case Surface.ROTATION_180:
                degrees = 180;
                break;
            case Surface.ROTATION_270:
                degrees = 270;
                break;
        }

        int result;
        if (info.facing == Camera.CameraInfo.CAMERA_FACING_FRONT) {
            result = (info.orientation + degrees) % 360;
            result = (360 - result) % 360;  // compensate the mirror
        } else {  // back-facing
            result = (info.orientation - degrees + 360) % 360;
        }
        camera.setDisplayOrientation(result);
    }

    /**
     * 设置对焦，会影响camera吞吐速率
     */
    public static void setFocusModes(Camera.Parameters parameters) {
        List<String> focusModes = parameters.getSupportedFocusModes();
        if (focusModes.contains(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO))
            parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO);
    }

    /**
     * 设置fps
     */
    public static void chooseFramerate(Camera.Parameters parameters, float frameRate) {
        int framerate = (int) (frameRate * 1000);
        List<int[]> rates = parameters.getSupportedPreviewFpsRange();
        int[] bestFramerate = rates.get(0);
        for (int i = 0; i < rates.size(); i++) {
            int[] rate = rates.get(i);
            Log.e(TAG, "supported preview pfs min " + rate[0] + " max " + rate[1]);
            int curDelta = Math.abs(rate[1] - framerate);
            int bestDelta = Math.abs(bestFramerate[1] - framerate);
            if (curDelta < bestDelta) {
                bestFramerate = rate;
            } else if (curDelta == bestDelta) {
                bestFramerate = bestFramerate[0] < rate[0] ? rate : bestFramerate;
            }
        }
        Log.e(TAG, "closet framerate min " + bestFramerate[0] + " max " + bestFramerate[1]);
        parameters.setPreviewFpsRange(bestFramerate[0], bestFramerate[1]);
    }

    /**
     * Attempts to find a preview size that matches the provided width and height (which
     * specify the dimensions of the encoded video).  If it fails to find a match it just
     * uses the default preview size for video.
     * <p>
     * TODO: should do a best-fit match, e.g.
     * https://github.com/commonsguy/cwac-camera/blob/master/camera/src/com/commonsware/cwac/camera/CameraUtils.java
     */
    public static int[] choosePreviewSize(Camera.Parameters parms, int width, int height) {
        // We should make sure that the requested MPEG size is less than the preferred
        // size, and has the same aspect ratio.
        Camera.Size ppsfv = parms.getPreferredPreviewSizeForVideo();
        if (ppsfv != null) {
            Log.d(TAG, "Camera preferred preview size for video is " + ppsfv.width + "x" + ppsfv.height);
        }

        //for (Camera.Size size : parms.getSupportedPreviewSizes()) {
        //    Log.d(TAG, "supported: " + size.width + "x" + size.height);
        //}

        for (Camera.Size size : parms.getSupportedPreviewSizes()) {
            if (size.width == width && size.height == height) {
                parms.setPreviewSize(width, height);
                return new int[]{width, height};
            }
        }

        Log.w(TAG, "Unable to set preview size to " + width + "x" + height);
        if (ppsfv != null) {
            parms.setPreviewSize(ppsfv.width, ppsfv.height);
            return new int[]{ppsfv.width, ppsfv.height};
        }
        // else use whatever the default size is
        return new int[]{0, 0};
    }

}
