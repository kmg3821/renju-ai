# How to build  
1. ``mkdir build``
2. ``cd ./build``
3. ``cmake .. -DCMAKE_BUILD_TYPE=Release``
4. ``cmake --build . -j --config Release``  

Tested on the lastest version (in 27 Sep 2025) of Windows, Ubuntu, and Raspberry Pi OS.

# How to play  
Locate the following three files at the same directory:
* renju executable (e.g. .exe)
* renju.tflite
* renju.html

Then,

1. Run the renju executable
2. Open ``localhost:8080`` in the browser
3. Play


# Note  
* The AI is not perfect. Sometimes, it makes mistakes, placing strange moves. I will enhance the AI model sooner or later.
* As of now, error handling is poor. The code contains minimal functionality.


# Trouble shooting
* If ``error C4430: missing type specifier - int assumed. Note: C++ does not support default-int`` occurs during build, refer to the solution at https://github.com/tensorflow/tensorflow/issues/64376#issuecomment-2069774768
* If ``undefined reference to 'xnn_something'`` error occurs during build, add ``-DTFLITE_ENABLE_XNNPACK=OFF`` to the cmake configuration.
