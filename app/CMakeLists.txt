# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

cmake_minimum_required(VERSION 3.4.1)

include_directories(src/main/cpp/include)
link_directories(src/main/jniLibs/${CMAKE_ANDROID_ARCH_ABI})
link_libraries(CNamaSDK.so fuai.so)

add_library(
        fu_native
        SHARED
        src/main/cpp/authpack.h
        src/main/cpp/android_util.h
        src/main/cpp/android_util.c
        src/main/cpp/gl_utils.h
        src/main/cpp/gl_utils.c
        src/main/cpp/faceunity_renderer.h
        src/main/cpp/faceunity_renderer.c
        src/main/cpp/fu_native_jni.cpp
)

find_library(log-lib log)
find_library(android-lib android)
find_library(gles-lib GLESv2)

target_link_libraries(
        fu_native
        CNamaSDK
        fuai
        ${log-lib}
        ${android-lib}
        ${gles-lib}
)
