#include "messaging.hpp"

void handleMessage(const char* message) {
    rapidjson::Document document;
    document.Parse(message);

    if (document.HasParseError() || !document.IsObject() || !document.HasMember("to") || !document["to"].IsString() ||
        document["to"].GetString() != std::string("cpp") || !document.HasMember("message"))
        return;

    std::string messageString = document["message"].GetString();

    std::cout << "Message from JavaScript: " << messageString << std::endl;
}

void sendMessage(const std::string& message) {
    EM_ASM({ window.postMessage(JSON.stringify(Module.UTF8ToString($0)), '*'); }, message.c_str());
}

void startListeningForMessages() {
    emscripten_run_script(
        "window.addEventListener('message', function(event) { Module.ccall('handleMessage', 'void', "
        "['string'], [event.data]); });");
}
