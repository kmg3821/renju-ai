![renju](https://github.com/user-attachments/assets/322a94b4-d045-4756-bdf1-88234579305b)


# How to build  
1. ``mkdir build``
2. ``cd ./build``
3. ``cmake .. -DCMAKE_BUILD_TYPE=Release``
4. ``cmake --build . -j --config Release``  

Tested on the lastest version (in 27 Sep 2025) of Windows, Ubuntu, and Raspberry Pi OS.

## Trouble shooting
* If ``missing type specifier - int assumed. Note: C++ does not support default-int`` error occurs, refer to the solution at https://github.com/tensorflow/tensorflow/issues/64376#issuecomment-2069774768
* If ``undefined reference to 'xnn_something'`` error occurs, add ``-DTFLITE_ENABLE_XNNPACK=OFF`` to the cmake configuration.


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
* As of now, error handling is poor. The code contains minimal functionality.
* The AI is not 100% perfect. It could make mistakes, placing strange moves.
* The model defeated some online Gomoku programs :  
   <img width="509" height="254" alt="defeat2" src="https://github.com/user-attachments/assets/8448bf70-e7cc-4313-90fd-a1965fc9c3b0" />


# How was it made
I used JAX ecosystem to create the AI model. The model is based on Alphazero. I made a big model first and distill it to a smaller one. Then, I converted it to tensorflow model. I'm going to upload the full training scripts later.

