#include "Sketch.hpp"

Sketch::Sketch(SDL_Renderer* renderer) : renderer_(renderer) { setup(); }

void Sketch::setup() {
    cell_width_ = std::max(1, std::min(window_width_, window_height_) / resolution_);
    grid_height_ = resolution_;
    grid_width_ = resolution_ / 2;
    scaled_x_step_ = 2.0f / (grid_width_ - 1) * render_step_width_;
    scaled_y_step_ = 4.0f / (grid_height_ - 1) * render_step_width_;
    grid_.resize(grid_height_, std::vector<float>(grid_width_, 0.0f));
    canvas_width_ = cell_width_ * grid_height_;
    canvas_center_ = canvas_width_ / 2;
    canvas_offset_x_ = (window_width_ - canvas_width_) / 2;
    canvas_offset_y_ = (window_height_ - canvas_width_) / 2;
}

void Sketch::update(const double& delta_time) {
    animation_progress_ += animation_speed_ * delta_time;
    if (animation_progress_ >= 1.0f) animation_progress_ = 0.0f;
    calculate_c_();
    render_grid_();
}

void Sketch::draw() noexcept {
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
    SDL_RenderClear(renderer_);

    for (int x = 0; x < grid_width_; x++) {
        for (int y = 0; y < grid_height_; y++) {
            const float t = grid_[y][x];
            grid_[y][x] = 0.0f;

            if (t == 0.0f || t == 1.0f) continue;

            const auto [r, g, b] = get_color_(t);
            SDL_SetRenderDrawColor(renderer_, r, g, b, 255);
            draw_rect_(x, y);
        }
    }

    SDL_RenderPresent(renderer_);
}

void Sketch::set_window_size(const int& width, const int& height) noexcept {
    window_width_ = width;
    window_height_ = height;
    setup();
}

void Sketch::render_grid_() noexcept {
    render_sparse_grid();

    const float scaled_x_step_div_render_step_width = scaled_x_step_ / render_step_width_;
    const float scaled_y_step_div_render_step_width = scaled_y_step_ / render_step_width_;

    for (int x = 0; x < grid_width_ - render_step_width_; x += render_step_width_) {
        for (int y = 0; y < grid_height_ - render_step_width_; y += render_step_width_) {
            if (are_corners_black(x, y)) continue;

            fill_edges(x, y, scaled_x_step_div_render_step_width, scaled_y_step_div_render_step_width);
            fill_checkerboard_pattern(x, y, scaled_x_step_div_render_step_width, scaled_y_step_div_render_step_width);
            fill_rest_by_averaging_neighbors(x, y);
        }
    }
}

void Sketch::render_sparse_grid() noexcept {
    int x = 0;
    float scaled_x = 0.0f;
    while (x < grid_width_) {
        render_sparse_grid_column(x, scaled_x);
        x += render_step_width_;
        scaled_x += scaled_x_step_;
    }
}

void Sketch::render_sparse_grid_column(int x, float scaled_x) noexcept {
    int y = 0;
    float scaled_y = -2.0f;
    while (y < grid_height_) {
        grid_[y][x] = is_in_set_(scaled_x, scaled_y);
        y += render_step_width_;
        scaled_y += scaled_y_step_;
    }
}

bool Sketch::are_corners_black(int x, int y) noexcept {
    return (grid_[y][x] == 0.0f && grid_[y][x + render_step_width_] == 0.0f &&
            grid_[y + render_step_width_][x] == 0.0f &&
            grid_[y + render_step_width_][x + render_step_width_] == 0.0f) ||
           (grid_[y][x] == 1.0f && grid_[y][x + render_step_width_] == 1.0f &&
            grid_[y + render_step_width_][x] == 1.0f && grid_[y + render_step_width_][x + render_step_width_] == 1.0f);
}

void Sketch::fill_edges(int x, int y, float scaled_x_step_div_render_step_width,
                        float scaled_y_step_div_render_step_width) noexcept {
    for (int i = 1; i < render_step_width_; i++) {
        fill_edge(x, y, i, scaled_x_step_div_render_step_width, scaled_y_step_div_render_step_width);
    }
}

void Sketch::fill_edge(int x, int y, int i, float scaled_x_step_div_render_step_width,
                       float scaled_y_step_div_render_step_width) noexcept {
    grid_[y][x + i] =
        is_in_set_(scaled_x_step_div_render_step_width * (x + i), scaled_y_step_div_render_step_width * y - 2.0f);
    grid_[y + render_step_width_][x + i] =
        is_in_set_(scaled_x_step_div_render_step_width * (x + i),
                   scaled_y_step_div_render_step_width * (y + render_step_width_) - 2.0f);
    grid_[y + i][x] =
        is_in_set_(scaled_x_step_div_render_step_width * x, scaled_y_step_div_render_step_width * (y + i) - 2.0f);
    grid_[y + i][x + render_step_width_] = is_in_set_(scaled_x_step_div_render_step_width * (x + render_step_width_),
                                                      scaled_y_step_div_render_step_width * (y + i) - 2.0f);
}

void Sketch::fill_checkerboard_pattern(int x, int y, float scaled_x_step_div_render_step_width,
                                       float scaled_y_step_div_render_step_width) noexcept {
    for (int i = 1; i < render_step_width_; i += 2) {
        for (int j = 1; j < render_step_width_; j += 2) {
            grid_[y + i][x + j] = is_in_set_(scaled_x_step_div_render_step_width * (x + j),
                                             scaled_y_step_div_render_step_width * (y + i) - 2.0f);
        }
    }
    for (int i = 2; i < render_step_width_; i += 2) {
        for (int j = 2; j < render_step_width_; j += 2) {
            grid_[y + i][x + j] = is_in_set_(scaled_x_step_div_render_step_width * (x + j),
                                             scaled_y_step_div_render_step_width * (y + i) - 2.0f);
        }
    }
}

void Sketch::fill_rest_by_averaging_neighbors(int x, int y) noexcept {
    for (int i = 1; i < render_step_width_; i += 2) {
        for (int j = 2; j < render_step_width_; j += 2) {
            grid_[y + i][x + j] = average_of_neighbors(x, y, i, j);
        }
    }
    for (int i = 2; i < render_step_width_; i += 2) {
        for (int j = 1; j < render_step_width_; j += 2) {
            grid_[y + i][x + j] = average_of_neighbors(x, y, i, j);
        }
    }
}

float Sketch::average_of_neighbors(int x, int y, int i, int j) noexcept {
    return (grid_[y + i - 1][x + j] + grid_[y + i + 1][x + j] + grid_[y + i][x + j - 1] + grid_[y + i][x + j + 1]) /
           4.0f;
}

inline void Sketch::calculate_c_() noexcept {
    const size_t index = static_cast<size_t>(animation_progress_ * (c_points_.size() - 1));
    const float progress = std::fmod(animation_progress_ * (c_points_.size() - 1), 1.0f);

    const auto& current_point = c_points_[index];
    const auto& next_point = c_points_[index + 1];

    c_real_ = current_point.first + (next_point.first - current_point.first) * progress;
    c_imag_ = current_point.second + (next_point.second - current_point.second) * progress;
}

// This function checks if a given point (x, y) is in the Julia set
inline float Sketch::is_in_set_(const float& x, const float& y) const noexcept {
    // Initialize constants for the real and imaginary parts of the complex constant c
    const v128_t c_real = wasm_f32x4_splat(c_real_);
    const v128_t c_imag = wasm_f32x4_splat(c_imag_);

    // Initialize constants for the number 2 and -2
    const v128_t two = wasm_f32x4_splat(2.0f);
    const v128_t neg_two = wasm_f32x4_splat(-2.0f);

    // Initialize the maximum number of iterations
    const v128_t max_iterations = wasm_f32x4_splat(static_cast<float>(max_iterations_));

    // Initialize the real and imaginary parts of z
    v128_t z_real = wasm_f32x4_splat(x);
    v128_t z_imag = wasm_f32x4_splat(y);

    // Initialize the iteration counter
    int iterations = 0;

    // Iterate until the maximum number of iterations is reached
    while (++iterations < max_iterations_) {
        // Calculate the squares of the real and imaginary parts of z
        const v128_t z_real_squared = wasm_f32x4_mul(z_real, z_real);
        const v128_t z_imag_squared = wasm_f32x4_mul(z_imag, z_imag);

        // Update the real part of z
        v128_t z_real_temp = wasm_f32x4_sub(z_real_squared, z_imag_squared);
        z_real_temp = wasm_f32x4_add(z_real_temp, c_real);

        // Update the imaginary part of z
        z_imag = wasm_f32x4_mul(wasm_f32x4_mul(z_real, z_imag), two);
        z_imag = wasm_f32x4_add(z_imag, c_imag);

        // Update the real part of z
        z_real = z_real_temp;

        // Check if z has escaped the circle of radius 2
        const v128_t out_of_bounds =
            wasm_v128_or(wasm_v128_or(wasm_f32x4_gt(z_real, two), wasm_f32x4_lt(z_real, neg_two)),
                         wasm_v128_or(wasm_f32x4_gt(z_imag, two), wasm_f32x4_lt(z_imag, neg_two)));

        // If z has escaped, break the loop
        if (wasm_v128_any_true(out_of_bounds)) break;
    }

    // If only one iteration was done, return 0
    if (iterations == 1) return 0.0f;

    // Calculate the modulus of z
    const v128_t mod_squared = wasm_f32x4_add(wasm_f32x4_mul(z_real, z_real), wasm_f32x4_mul(z_imag, z_imag));
    const v128_t mod = wasm_f32x4_sqrt(mod_squared);

    // Calculate the average modulus
    const float scalar_mod = (wasm_f32x4_extract_lane(mod, 0) + wasm_f32x4_extract_lane(mod, 1) +
                              wasm_f32x4_extract_lane(mod, 2) + wasm_f32x4_extract_lane(mod, 3)) /
                             4.0f;

    // Calculate the logarithm of the average modulus
    const float log_scalar_mod = fast_log2(std::max(1.0f, fast_log2(scalar_mod)));

    // Calculate the smooth color
    const float smooth = static_cast<float>(iterations) - log_scalar_mod;
    const float t = smooth / static_cast<float>(max_iterations_);

    // If t is close to 1.0f or 0.0f, then make it exactly 1.0f or 0.0f
    return (t > 0.999f) ? 1.0f : ((t < 0.001f) ? 0.0f : t);
}

inline std::tuple<int, int, int> Sketch::get_color_(const float& t) const noexcept {
    const float inverse_t = 1 - t;
    const float t_squared = t * t;
    const float t_cubed = t_squared * t;

    const int r = static_cast<int>(9 * inverse_t * t_cubed * 255);
    const int g = static_cast<int>(15 * inverse_t * inverse_t * t_squared * 255);
    const int b = static_cast<int>(8.5 * inverse_t * inverse_t * inverse_t * t * 255);

    return std::make_tuple(r, g, b);
}

void Sketch::draw_rect_(const int& x, const int& y) {
    SDL_Rect rect{canvas_offset_x_ + canvas_center_ + x * cell_width_, canvas_offset_y_ + y * cell_width_, cell_width_,
                  cell_width_};
    SDL_Rect rect_mirror{canvas_offset_x_ + canvas_center_ - x * cell_width_ - cell_width_,
                         canvas_offset_y_ + canvas_width_ - y * cell_width_ - cell_width_, cell_width_, cell_width_};
    SDL_RenderFillRect(renderer_, &rect);
    SDL_RenderFillRect(renderer_, &rect_mirror);
}

inline float fast_log2(float x) {
    union {
        float f;
        uint32_t i;
    } vx = {x};
    union {
        uint32_t i;
        float f;
    } mx = {(vx.i & 0x007FFFFF) | 0x3f000000};

    float y = vx.i * 1.1920928955078125e-7f;
    return y - 124.22551499f - 1.498030302f * mx.f - 1.72587999f / (0.3520887068f + mx.f);
}