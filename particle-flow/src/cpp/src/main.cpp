#include <ctime>

#include "Application.hpp"
#include "messaging/messaging.hpp"

int main() {
    startListeningForMessages();

    srand(time(NULL));

    Application app;

    app.loop();

    return 0;
}