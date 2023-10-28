#include <SDL2/SDL.h>

#include <iostream>

#include "messaging/messaging.hpp"

class Application {
   public:
    Application();
    ~Application();

    void loop();
    void update(double delta_time);
    void draw();

   private:
    int window_width_ = 640;
    int window_height_ = 480;
    SDL_Window *window_;
    SDL_Renderer *renderer_;
    SDL_Event window_event_;

    Uint64 last_time_ = 0;

    struct ApplicationData {
        double block_x = 0;
        double block_y = 0;
        double block_w = 50;
        double block_h = 50;
        double block_vel_x = 0;
        double block_vel_y = 0;
    } data_;
};