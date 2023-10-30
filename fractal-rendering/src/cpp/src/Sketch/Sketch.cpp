#include "Sketch.hpp"

Sketch::Sketch(SDL_Renderer* renderer) : renderer_(renderer) { setup(); }

void Sketch::setup() {
    canvas_width_ = std::min(window_width_, window_height_);
    canvas_center_ = canvas_width_ / 2;
    canvas_offset_x_ = (window_width_ - canvas_width_) / 2;
    canvas_offset_y_ = (window_height_ - canvas_width_) / 2;
    cell_width_ = std::max(1, canvas_width_ / resolution_);
    grid_height_ = canvas_width_ / cell_width_;
    grid_width_ = grid_height_ / 2;
    scaled_x_step_ = 2.0f / (grid_width_ - 1) * render_step_width_;
    scaled_y_step_ = 4.0f / (grid_height_ - 1) * render_step_width_;
    grid_.resize(grid_height_);
    for (auto& row : grid_) row.resize(grid_width_, 0.0f);
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
            SDL_Rect rect{canvas_offset_x_ + canvas_center_ + x * cell_width_, canvas_offset_y_ + y * cell_width_,
                          cell_width_, cell_width_};
            SDL_Rect rect_mirror{canvas_offset_x_ + canvas_center_ - x * cell_width_ - cell_width_,
                                 canvas_offset_y_ + canvas_width_ - y * cell_width_ - cell_width_, cell_width_,
                                 cell_width_};
            SDL_RenderFillRect(renderer_, &rect);
            SDL_RenderFillRect(renderer_, &rect_mirror);
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
    // sparse rendering
    int x = 0;
    float scaled_x = 0.0f;
    while (x < grid_width_) {
        int y = 0;
        float scaled_y = -2.0f;
        while (y < grid_height_) {
            grid_[y][x] = is_in_set_(scaled_x, scaled_y);

            y += render_step_width_;
            scaled_y += scaled_y_step_;
        }
        x += render_step_width_;
        scaled_x += scaled_x_step_;
    }

    // Precompute scaled_x_step_ / render_step_width_ and scaled_y_step_ / render_step_width_
    const float scaled_x_step_div_render_step_width = scaled_x_step_ / render_step_width_;
    const float scaled_y_step_div_render_step_width = scaled_y_step_ / render_step_width_;

    for (x = 0; x < grid_width_ - render_step_width_; x += render_step_width_) {
        for (int y = 0; y < grid_height_ - render_step_width_; y += render_step_width_) {
            // skip if all corners are black
            if ((grid_[y][x] == 0.0f && grid_[y][x + render_step_width_] == 0.0f &&
                 grid_[y + render_step_width_][x] == 0.0f &&
                 grid_[y + render_step_width_][x + render_step_width_] == 0.0f) ||
                (grid_[y][x] == 1.0f && grid_[y][x + render_step_width_] == 1.0f &&
                 grid_[y + render_step_width_][x] == 1.0f &&
                 grid_[y + render_step_width_][x + render_step_width_] == 1.0f))
                continue;

            // fill in the 4 edges
            for (int i = 1; i < render_step_width_; i++) {
                grid_[y][x + i] = is_in_set_(scaled_x_step_div_render_step_width * (x + i),
                                             scaled_y_step_div_render_step_width * y - 2.0f);
                grid_[y + render_step_width_][x + i] =
                    is_in_set_(scaled_x_step_div_render_step_width * (x + i),
                               scaled_y_step_div_render_step_width * (y + render_step_width_) - 2.0f);
                grid_[y + i][x] = is_in_set_(scaled_x_step_div_render_step_width * x,
                                             scaled_y_step_div_render_step_width * (y + i) - 2.0f);
                grid_[y + i][x + render_step_width_] =
                    is_in_set_(scaled_x_step_div_render_step_width * (x + render_step_width_),
                               scaled_y_step_div_render_step_width * (y + i) - 2.0f);
            }

            // fill in checkerboard pattern (split into 2 loops to avoid branching)
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

            // fill in the rest by averaging the 4 neighbors (split into 2 loops to avoid branching)
            for (int i = 1; i < render_step_width_; i += 2) {
                for (int j = 2; j < render_step_width_; j += 2) {
                    grid_[y + i][x + j] = (grid_[y + i - 1][x + j] + grid_[y + i + 1][x + j] + grid_[y + i][x + j - 1] +
                                           grid_[y + i][x + j + 1]) /
                                          4.0f;
                }
            }
            for (int i = 2; i < render_step_width_; i += 2) {
                for (int j = 1; j < render_step_width_; j += 2) {
                    grid_[y + i][x + j] = (grid_[y + i - 1][x + j] + grid_[y + i + 1][x + j] + grid_[y + i][x + j - 1] +
                                           grid_[y + i][x + j + 1]) /
                                          4.0f;
                }
            }
        }
    }
}

inline void Sketch::calculate_c_() noexcept {
    const int index = static_cast<int>(animation_progress_ * (c_points_.size() - 1));
    const float progress = std::fmod(animation_progress_ * (c_points_.size() - 1), 1.0f);

    c_real_ = c_points_[index].first + (c_points_[index + 1].first - c_points_[index].first) * progress;
    c_imag_ = c_points_[index].second + (c_points_[index + 1].second - c_points_[index].second) * progress;
}

inline float Sketch::is_in_set_(const float& x, const float& y) const noexcept {
    const v128_t c_real = wasm_f32x4_splat(c_real_);
    const v128_t c_imag = wasm_f32x4_splat(c_imag_);
    const v128_t two = wasm_f32x4_splat(2.0f);
    const v128_t neg_two = wasm_f32x4_splat(-2.0f);
    const v128_t one = wasm_f32x4_splat(1.0f);
    const v128_t zero = wasm_f32x4_splat(0.0f);
    const v128_t max_iterations = wasm_f32x4_splat(static_cast<float>(max_iterations_));

    v128_t z_real = wasm_f32x4_splat(x);
    v128_t z_imag = wasm_f32x4_splat(y);

    int iterations = 0;
    while (++iterations < max_iterations_) {
        const v128_t z_real_squared = wasm_f32x4_mul(z_real, z_real);
        const v128_t z_imag_squared = wasm_f32x4_mul(z_imag, z_imag);
        v128_t z_real_temp = wasm_f32x4_sub(z_real_squared, z_imag_squared);
        z_real_temp = wasm_f32x4_add(z_real_temp, c_real);
        z_imag = wasm_f32x4_mul(wasm_f32x4_mul(z_real, z_imag), two);
        z_imag = wasm_f32x4_add(z_imag, c_imag);
        z_real = z_real_temp;

        const v128_t z_real_gt_two = wasm_f32x4_gt(z_real, two);
        const v128_t z_real_lt_neg_two = wasm_f32x4_lt(z_real, neg_two);
        const v128_t z_imag_gt_two = wasm_f32x4_gt(z_imag, two);
        const v128_t z_imag_lt_neg_two = wasm_f32x4_lt(z_imag, neg_two);
        bool any_out_of_bounds = wasm_v128_any_true(wasm_v128_or(wasm_v128_or(z_real_gt_two, z_real_lt_neg_two),
                                                                 wasm_v128_or(z_imag_gt_two, z_imag_lt_neg_two)));
        if (any_out_of_bounds) break;
    }

    if (iterations == 1) return 0.0f;

    const v128_t z_real_squared = wasm_f32x4_mul(z_real, z_real);
    const v128_t z_imag_squared = wasm_f32x4_mul(z_imag, z_imag);
    const v128_t mod_squared = wasm_f32x4_add(z_real_squared, z_imag_squared);
    const v128_t mod = wasm_f32x4_sqrt(mod_squared);

    const float scalar_mod = (wasm_f32x4_extract_lane(mod, 0) + wasm_f32x4_extract_lane(mod, 1) +
                              wasm_f32x4_extract_lane(mod, 2) + wasm_f32x4_extract_lane(mod, 3)) /
                             4.0f;

    const float log_scalar_mod = fast_log2(std::max(1.0f, fast_log2(scalar_mod)));
    const float smooth = static_cast<float>(iterations) - log_scalar_mod;
    const float t = smooth / static_cast<float>(max_iterations_);

    // if t is close to 1.0f or 0.0f, then make it exactly 1.0f or 0.0f
    if (t > 0.999f) return 1.0f;
    if (t < 0.001f) return 0.0f;

    return t;
}

inline std::tuple<int, int, int> Sketch::get_color_(const float& t) const noexcept {
    const int r = 9 * (1 - t) * t * t * t * 255;
    const int g = 15 * (1 - t) * (1 - t) * t * t * 255;
    const int b = 8.5 * (1 - t) * (1 - t) * (1 - t) * t * 255;

    return std::tuple<int, int, int>(r, g, b);
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
    float y = vx.i;
    y *= 1.1920928955078125e-7f;

    return y - 124.22551499f - 1.498030302f * mx.f - 1.72587999f / (0.3520887068f + mx.f);
}