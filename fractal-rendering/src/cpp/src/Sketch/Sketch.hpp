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
    std::vector<std::vector<float>> grid_;

    int resolution_ = 1024;
    int render_step_width_ = 6;
    int cell_width_ = 0;
    float scaled_x_step_ = 0.0f;
    float scaled_y_step_ = 0.0f;

    int max_iterations_ = 50;
    float c_real_ = 0.0f;
    float c_imag_ = 0.0f;

    // interpolation animation for c_ between multiple points
    using Point = std::pair<float, float>;
    std::vector<Point> c_points_ = {
        {0.0f, 0.0f},    {-0.4f, -0.59f},    {-0.702f, -0.384f},  {-0.835f, -0.2321f}, {-2.0f, 0.0f},
        {-0.8f, 0.156f}, {-0.835f, 0.2321f}, {-0.7269f, 0.1889f}, {-0.54f, 0.54f},     {-0.4f, 0.6f},
        {0.0f, 0.8f},    {0.078f, 0.656f},   {0.355f, 0.355f},    {0.45f, 0.1428f},    {0.37f, 0.1f},
        {0.285f, 0.01},  {0.285f, 0.0f},     {0.34f, -0.05},      {0.0f, 0.0f},
    };

    float animation_progress_ = 0.0f;
    float animation_speed_ = 0.00001f;

    void render_grid_() noexcept;
    void render_sparse_grid() noexcept;
    void render_sparse_grid_column(int x, float scaled_x) noexcept;
    bool are_corners_black(int x, int y) noexcept;
    void fill_edges(int x, int y, float scaled_x_step_div_render_step_width,
                    float scaled_y_step_div_render_step_width) noexcept;
    void fill_edge(int x, int y, int i, float scaled_x_step_div_render_step_width,
                   float scaled_y_step_div_render_step_width) noexcept;
    void fill_checkerboard_pattern(int x, int y, float scaled_x_step_div_render_step_width,
                                   float scaled_y_step_div_render_step_width) noexcept;
    void fill_rest_by_averaging_neighbors(int x, int y) noexcept;
    float average_of_neighbors(int x, int y, int i, int j) noexcept;
    void calculate_c_() noexcept;
    float is_in_set_(const float &x, const float &y) const noexcept;
    std::tuple<int, int, int> get_color_(const float &t) const noexcept;
    void draw_rect_(const int &x, const int &y);
};

inline float fast_log2(float x);

#endif