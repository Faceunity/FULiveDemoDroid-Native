package com.faceunity.beautycontrolview.entity;

/**
 * 本demo中道具的实体类
 * Created by tujh on 2018/2/7.
 */
public class Sticker {
    private int iconId;
    private String filePath;
    private String description;

    public Sticker(int iconId, String filePath, String description) {
        this.iconId = iconId;
        this.filePath = filePath;
        this.description = description;
    }

    public int getIconId() {
        return iconId;
    }

    public String getFilePath() {
        return filePath;
    }

    public String getDescription() {
        return description;
    }

    @Override
    public String toString() {
        return "Sticker{" +
                "iconId=" + iconId +
                ", filePath='" + filePath + '\'' +
                ", description='" + description + '\'' +
                '}';
    }
}
