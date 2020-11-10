package com.faceunity.fulivenativedemo;

import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v7.app.AppCompatActivity;
import android.widget.Toast;

/**
 * 开屏页
 *
 * @author Richie on 2019.12.20
 */
public class SplashActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        boolean ret = PermissionUtils.checkNeededPermission(this);
        handlePermissionResult(ret);
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (grantResults.length > 0) {
            boolean ret = true;
            for (int grantResult : grantResults) {
                if (grantResult == PackageManager.PERMISSION_DENIED) {
                    ret = false;
                    break;
                }
            }
            handlePermissionResult(ret);
        }
    }

    private void handlePermissionResult(boolean ret) {
        if (ret) {
            Intent intent = new Intent(this, MainActivity.class);
            startActivity(intent);
            finish();
        } else {
            Toast.makeText(this, R.string.toast_permission, Toast.LENGTH_SHORT).show();
        }
    }
}
