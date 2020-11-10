package com.faceunity.fulivenativedemo;


import com.faceunity.beautycontrolview.entity.Filter;

import java.util.ArrayList;

/**
 * Created by tujh on 2018/1/30.
 */
public enum FilterEnum {
    /**
     * 滤镜
     */
    origin("origin", R.drawable.demo_icon_cancel, "原图"),
    nature1("ziran1", R.drawable.demo_icon_natural_1, "自然 1"),
    zhiganhui1("zhiganhui1", R.drawable.demo_icon_texture_gray1, "质感灰 1"),
    bailiang1("bailiang1", R.drawable.demo_icon_bailiang1, "白亮 1"),
    fennen1("fennen1", R.drawable.demo_icon_fennen1, "粉嫩 1"),
    lengsediao1("lengsediao1", R.drawable.demo_icon_lengsediao1, "冷色调 1");

    private String name;
    private int resId;
    private String description;

    FilterEnum(String name, int resId, String description) {
        this.name = name;
        this.resId = resId;
        this.description = description;
    }

    public Filter create() {
        return new Filter(name, resId, description);
    }

    public static ArrayList<Filter> getFilters() {
        FilterEnum[] values = FilterEnum.values();
        ArrayList<Filter> filters = new ArrayList<>(values.length);
        for (FilterEnum f : values) {
            filters.add(f.create());
        }
        return filters;
    }
}
