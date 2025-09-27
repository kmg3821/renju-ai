#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/model.h"
#include "mongoose.h"

#include "search.h"

#include <iostream>
#include <memory>
#include <math.h>
#include <iomanip>
#include <iostream>
#include <string>

board_t board;
std::unique_ptr<tflite::Interpreter> interpreter;

static void ev_handler(struct mg_connection *c, int ev, void *ev_data) {
  if (ev == MG_EV_HTTP_MSG) {
    mg_http_message *hm = (struct mg_http_message *) ev_data;
    if (mg_match(hm->uri, mg_str("/"), NULL)) {
      mg_http_serve_opts opts = {};
      mg_http_serve_file(c, hm, "renju.html", &opts);
    } 
    else if (mg_match(hm->uri, mg_str("/play"), NULL)) {
      std::cout << hm->body.buf << std::endl;

      for(int i = 0; i < BDLEN; ++i) {
        for(int j = 0; j < BDLEN; ++j) {
          board[i+1][j+1] = hm->body.buf[BDLEN*i + j] - '0';
        }
      }
      int16_t turn = (hm->body.buf[BDLEN*BDLEN + 1] - '0') * 100 +
        (hm->body.buf[BDLEN*BDLEN + 2] - '0') * 10 +
        (hm->body.buf[BDLEN*BDLEN + 3] - '0') * 1;
      int16_t action = (hm->body.buf[BDLEN*BDLEN + 5] - '0') * 100 +
        (hm->body.buf[BDLEN*BDLEN + 6] - '0') * 10 +
        (hm->body.buf[BDLEN*BDLEN + 7] - '0') * 1;

      pos_t pos = {(int8_t)(action/BDLEN + 1), (int8_t)(action%BDLEN + 1)};
      int8_t player = (turn & 1) == 0 ? BLACK : WHITE;
      int8_t result;
      if(player == BLACK) {
        result = is_white_terminated(board, pos);
      }
      else {
        result = is_black_terminated(board, pos);
      }
      SearchOutput out = {result, -1, 0.0f};
      if(result == CONT) {
        out = search_move(board, interpreter, turn);
      }
      mg_http_reply(c, 200, "Content-Type: text/plain\r\n", "%d,%d,%f", out.result, out.action, out.value);
    }
  }
}

int main(void) {
  auto model = tflite::FlatBufferModel::BuildFromFile("renju.tflite");
  if (!model) {
    std::cerr << "Failed to load model\n";
    return 1;
  }

  tflite::ops::builtin::BuiltinOpResolver resolver;
  tflite::InterpreterBuilder builder(*model, resolver);
  builder(&interpreter);

  if (!interpreter) {
    std::cerr << "Failed to build interpreter\n";
    return 1;
  }

  if (interpreter->AllocateTensors() != kTfLiteOk) {
    std::cerr << "Failed to allocate tensors\n";
    return 1;
  }

  init_board(board);

  struct mg_mgr mgr;
  mg_mgr_init(&mgr);
  mg_http_listen(&mgr, "http://localhost:8080", ev_handler, NULL);
  for (;;) {
    mg_mgr_poll(&mgr, 1000);
  }
  return 0;
}
