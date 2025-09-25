# How to build  
Ninja build system is recommended for fast build.
1. ``mkdir build``
2. ``cd ./build``
3. ``cmake .. -DCMAKE_BUILD_TYPE=Release``
4. ``cmake --build . -j --config Release``


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
The AI is not perfect. Sometimes, it makes mistakes, placing strange moves. I will enhance the AI model sooner or later.


# Trouble shooting
* If ``error C4430: missing type specifier - int assumed. Note: C++ does not support default-int`` occurs during build, refer to the solution at https://github.com/tensorflow/tensorflow/issues/64376#issuecomment-2069774768
