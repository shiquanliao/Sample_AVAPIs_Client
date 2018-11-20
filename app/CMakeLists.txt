# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)


find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

set(distribution_DIR ${CMAKE_SOURCE_DIR}/../../../../libs)
include_directories(libs/include)

add_library( avutil
             SHARED
             IMPORTED )
set_target_properties( avutil
                       PROPERTIES IMPORTED_LOCATION
                       ../../../../libs/${ANDROID_ABI}/libavutil.so )

add_library( swresample
             SHARED
             IMPORTED )
set_target_properties( swresample
                       PROPERTIES IMPORTED_LOCATION
                       ../../../../libs/${ANDROID_ABI}/libswresample.so )
add_library( avcodec
             SHARED
             IMPORTED )
set_target_properties( avcodec
                       PROPERTIES IMPORTED_LOCATION
                       ../../../../libs/${ANDROID_ABI}/libavcodec.so )
add_library( avfilter
             SHARED
             IMPORTED)
set_target_properties( avfilter
                       PROPERTIES IMPORTED_LOCATION
                       ../../../../libs/${ANDROID_ABI}/libavfilter.so )
add_library( swscale
             SHARED
             IMPORTED)
set_target_properties( swscale
                       PROPERTIES IMPORTED_LOCATION
                       ../../../../libs/${ANDROID_ABI}/libswscale.so )

add_library( avformat
             SHARED
             IMPORTED)
set_target_properties( avformat
                       PROPERTIES IMPORTED_LOCATION
                       ../../../../libs/${ANDROID_ABI}/libavformat.so )


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

add_library( native-lib
             SHARED
             src/main/cpp/native-lib.cpp
             src/main/cpp/IDemux.cpp
             src/main/cpp/FFDemux.cpp
             src/main/cpp/XData.cpp
             src/main/cpp/XLog.cpp
             src/main/cpp/XThread.cpp
             src/main/cpp/IObserver.cpp
             src/main/cpp/IDecode.cpp
             src/main/cpp/FFDecode.cpp
             src/main/cpp/XParameter.cpp
             src/main/cpp/IVideoView.cpp
             src/main/cpp/GLVideoView.cpp
             src/main/cpp/XTexture.cpp
             src/main/cpp/XShader.cpp
             src/main/cpp/XEGL.cpp
             src/main/cpp/FFResample.cpp
             src/main/cpp/IResample.cpp
             src/main/cpp/IAudioPlay.cpp
             src/main/cpp/SLAudioPlay.cpp
             src/main/cpp/IPlayer.cpp
             src/main/cpp/IPlayerBuilder.cpp
             src/main/cpp/FFPlayerBuilder.cpp
             src/main/cpp/IPlayProxy.cpp
             src/main/cpp/FFPushStream.cpp
             src/main/cpp/XListener.cpp
             src/main/cpp/IPushStream.cpp

            )

target_link_libraries( # Specifies the target library.
                       native-lib
                       GLESv2 EGL
                       OpenSLES
                       android
                       avutil avformat avcodec swscale  swresample avfilter

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib} )