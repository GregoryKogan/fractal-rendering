#ifndef MESSAGING_HPP
#define MESSAGING_HPP

#include <emscripten.h>

#include <iostream>
#include <string>

#include "rapidjson/document.h"

extern "C" {
EMSCRIPTEN_KEEPALIVE
void handleMessage(const char* message);
}

void sendMessage(const std::string& message);
void startListeningForMessages();

#endif