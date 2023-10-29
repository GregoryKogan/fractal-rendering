#ifndef SKETCH_HPP
#define SKETCH_HPP

#include <SDL2/SDL.h>

#include <tuple>

class Sketch {
   public:
    Sketch(SDL_Renderer *renderer);

    void setup();
    void update(double delta_time);
    void draw() const noexcept;

    void set_window_size(int width, int height) noexcept;

   private:
    SDL_Renderer *renderer_;
    int window_width_ = 1024;
    int window_height_ = 1024;

    std::tuple<int, int, int> bg_color_ = {36, 40, 59};

    double block_x_ = 0;
    double block_y_ = 0;
    double block_w_ = 50;
    double block_h_ = 50;
    double block_vel_x_ = 0;
    double block_vel_y_ = 0;
};

#endif