#ifndef SKETCH_HPP
#define SKETCH_HPP

#include <SDL2/SDL.h>

#include <cmath>
#include <numeric>

class Sketch {
   public:
    Sketch(SDL_Renderer *renderer);

    void setup();
    void update(const double &delta_time);
    void draw() const noexcept;

    void set_window_size(const int &width, const int &height) noexcept;

   private:
    SDL_Renderer *renderer_;
    int window_width_ = 1024;
    int window_height_ = 1024;

    int resolution_ = 350;
    int cell_width_ = 4;
    int max_iterations_ = 30;
    float c_real_ = 0.0f;
    float c_imag_ = 0.0f;

    // interpolation animation for c_ between multiple points
    std::vector<std::pair<float, float>> c_points_ = {
        std::pair<float, float>(0.0f, 0.0f),        std::pair<float, float>(-0.4f, -0.59f),
        std::pair<float, float>(-0.54f, 0.54f),     std::pair<float, float>(-0.8f, 0.156f),
        std::pair<float, float>(-0.835f, -0.2321f), std::pair<float, float>(0.0f, 0.8f),
        std::pair<float, float>(0.285f, 0.01f),     std::pair<float, float>(0.34f, -0.05f),
        std::pair<float, float>(0.355f, 0.355f),    std::pair<float, float>(0.37f, 0.1f),
        std::pair<float, float>(0.0f, 0.0f)};

    float animation_progress_ = 0.0f;
    float animation_speed_ = 0.00003f;

    void calculate_c_() noexcept;
    float is_in_set_(const float &x, const float &y) const noexcept;
};

#endif