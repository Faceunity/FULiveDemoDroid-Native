package com.faceunity.fulivenativedemo;

import android.app.Application;

/**
 * Created by tujh on 2018/3/30.
 */
public class FUApplication extends Application {

    @Override
    public void onCreate() {
        super.onCreate();

        FURenderer.initFURenderer(getAssets());
    }

}
