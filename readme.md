
![TabbyEngine](/resources/logo/Tabby_Engine_Logo_And_Name_1024X256.png?raw=true "TabbyEngine")


Simple game engine written in C++.
This engines main focus is compatibility for most new and old platforms.
Engine is mainly for 2D games but has 3d elements. Further 3D support planned.

In this engine you mostly do everything by just code.

Engine architecture is based on Hazel engine.

External Dependencies
-------------------
- Bundled with engine
  - box2d 3.0
  - entt
  - ImGui
  - glad2
  - sdl2 
  - glm
  - imguizmo
  - spdlog
  - stb_image
  - yaml-cpp
  - msdf-atlas-gen
  - fastgltf
  - tracy profiler
  - OpenAL-Soft 
  - gtk4 (just on Linux) 

Supported Graphics Apis
-----------------------
- OpenGL 3.3
- OpenGL 3.0 es

Supported Platforms
-------------------
- Linux
- Windows (not tested)
- MacOS
- Web (not tested in a while)
- Android (not tested in a while)

Compiling
---------

You might have to install some libraries for building.
For building web install emscriptem from their git repo. Installing from package manager may give compilation errors.

- Native
  - For Unix:  

    1. ```shell
        git clone https://github.com/tabdroid-1/TabbyEngine.git && mkdir TabbyEngine/build && cd TabbyEngine/build
        ```

    2. ```shell
        cmake .. && make
        ```
    
  - For Windows:  
    - mingw 
     
      1. ```shell
         git clone https://github.com/tabdroid-1/MaineCoon.git && mkdir MaineCoon/build && cd MaineCoon/build
          ```
      2. ```shell
         cmake -DCMAKE_SYSTEM_NAME=Windows -DCMAKE_C_COMPILER=i686-w64-mingw32-gcc -DCMAKE_CXX_COMPILER=i686-w64-mingw32-g++ ..
         make -j4
          ```

- For Web 

  1. ```shell
     git clone https://github.com/tabdroid-1/TabbyEngine.git && mkdir TabbyEngine/build && cd TabbyEngine/build
     ```

  2. ```shell
     emcmake cmake .. && make
     ```

  3. ```shell
     python3 -m http.server 8080
     ```
     
- For Android 

  1. ```shell
     # It should build out of the box when built from Android Studio
     ```



Note From the author
 --------------------
- If you are going to use this game engine, pls support as much platform as possible (eh. You do not have to support Chrome OS or FreeBSD) and support more architectures like x86_32, x86_64, aarch64. You do not have to that but if you do it you would make me happi. :3

Special Thanks
 --------------
- Jonny Hotbody
- Peppino Spaghetti
- Soilder from TF2
- 2008 Toyota Corolla
- Bon
