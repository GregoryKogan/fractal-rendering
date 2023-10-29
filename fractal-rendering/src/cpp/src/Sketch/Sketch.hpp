#ifndef SKETCH_HPP
#define SKETCH_HPP

#include <SDL2/SDL.h>

#include <cmath>
#include <complex>
#include <numeric>

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

    int resolution_ = 256;
    int max_iterations_ = 30;
    std::complex<float> c_ = std::complex<float>(0.0f, 0.0f);

    // interpolation animation for c_ between multiple points
    std::vector<std::complex<float>> c_points_ = {
        std::complex<float>(0.0f, 0.0f),    std::complex<float>(-0.4f, -0.59f),     std::complex<float>(-0.54f, 0.54f),
        std::complex<float>(-0.8f, 0.156f), std::complex<float>(-0.835f, -0.2321f), std::complex<float>(0.0f, 0.8f),
        std::complex<float>(0.285f, 0.01f), std::complex<float>(0.34f, -0.05f),     std::complex<float>(0.355f, 0.355f),
        std::complex<float>(0.37f, 0.1f),   std::complex<float>(0.0f, 0.0f)};
    float animation_progress_ = 0.0f;
    float animation_speed_ = 0.00003f;

    void calculate_c_() noexcept;

    float is_in_set_(float x, float y) const noexcept;
};

#endif