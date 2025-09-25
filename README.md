# How to build  
Ninja build system is recommended for fast build.
1. ``mkdir build``
2. ``cd ./build``
3. ``cmake .. -DCMAKE_BUILD_TYPE=Release``
4. ``cmake --build . -j --config Release``


# How to run  
Locate the following three files at the same directory:
* renju binary (e.g. .exe)
* renju.tflite
* renju.html

Then,

1. Run the renju binary
2. Open ``localhost:8080`` in the browser
3. Play


# Note  
The AI is not perfect. Sometimes, it makes mistakes, placing strange moves.  
I will enhance the AI model sooner or later.
