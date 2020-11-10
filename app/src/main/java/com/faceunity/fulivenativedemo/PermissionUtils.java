package com.faceunity.fulivenativedemo;

import android.Manifest;
import android.app.Activity;
import android.content.pm.PackageManager;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;

import java.util.ArrayList;
import java.util.List;

/**
 * 权限
 */
public final class PermissionUtils {

    private PermissionUtils() {
    }

    public static boolean checkNeededPermission(Activity activity) {
        return checkPermission(activity, new String[]{
                Manifest.permission.CAMERA
        });
    }

    public static boolean checkPermission(Activity activity, String[] permissions) {
        List<String> permissionList = new ArrayList<>(2);
        for (String p : permissions) {
            if (ContextCompat.checkSelfPermission(activity, p) != PackageManager.PERMISSION_GRANTED) {
                permissionList.add(p);
            }
        }
        if (permissionList.isEmpty()) {
            return true;
        }
        String[] requestPermissions = new String[permissionList.size()];
        permissionList.toArray(requestPermissions);
        ActivityCompat.requestPermissions(activity, requestPermissions, 0);
        return false;
    }

}
