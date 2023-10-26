#include <ctime>

#include "Application.hpp"

int main(int argc, char *argv[]) {
    srand(time(NULL));

    Application app;

    app.loop();
    app.draw();

    return 0;
}