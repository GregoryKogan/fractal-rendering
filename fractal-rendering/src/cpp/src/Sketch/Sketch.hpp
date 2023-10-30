#ifndef SKETCH_HPP
#define SKETCH_HPP

#include <SDL2/SDL.h>
#include <wasm_simd128.h>

#include <cmath>
#include <iostream>
#include <numeric>

class Sketch {
   public:
    Sketch(SDL_Renderer *renderer);

    void setup();
    void update(const double &delta_time);
    void draw() noexcept;

    void set_window_size(const int &width, const int &height) noexcept;

   private:
    SDL_Renderer *renderer_;
    int window_width_ = 0;
    int window_height_ = 0;
    int canvas_width_ = 0;
    int canvas_center_ = 0;
    int canvas_offset_x_ = 0;
    int canvas_offset_y_ = 0;
    int grid_height_ = 0;
    int grid_width_ = 0;

    int resolution_ = 512;
    int render_step_width_ = 6;
    int cell_width_ = 0;
    float scaled_x_step_ = 0.0f;
    float scaled_y_step_ = 0.0f;
    std::vector<std::vector<float>> grid_;

    int max_iterations_ = 50;
    float c_real_ = 0.0f;
    float c_imag_ = 0.0f;

    void render_grid_() noexcept;

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

    inline void calculate_c_() noexcept;
    inline float is_in_set_(const float &x, const float &y) const noexcept;
    inline std::tuple<int, int, int> get_color_(const float &t) const noexcept;
};

inline float fast_log2(float x);

#endif