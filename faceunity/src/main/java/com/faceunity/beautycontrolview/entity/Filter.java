package com.faceunity.beautycontrolview.entity;

/**
 * 本demo中滤镜的实体类
 * Created by tujh on 2018/2/7.
 */
public class Filter {
    private String name;
    private int iconId;
    private String description;

    public Filter(String name, int iconId, String description) {
        this.name = name;
        this.iconId = iconId;
        this.description = description;
    }

    public String getName() {
        return name;
    }

    public int getIconId() {
        return iconId;
    }

    public String getDescription() {
        return description;
    }

    @Override
    public String toString() {
        return "Filter{" +
                "name='" + name + '\'' +
                ", resId=" + iconId +
                ", description='" + description + '\'' +
                '}';
    }
}
