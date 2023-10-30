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
    const float scaled_x_step = 2.0f / (grid_width_ - 1) * render_step_width_;
    const float scaled_y_step = 4.0f / (grid_height_ - 1) * render_step_width_;

    // sparse rendering
    int x = 0;
    float scaled_x = 0.0f;
    while (x < grid_width_) {
        int y = 0;
        float scaled_y = -2.0f;
        while (y < grid_height_) {
            grid_[y][x] = is_in_set_(scaled_x, scaled_y);

            y += render_step_width_;
            scaled_y += scaled_y_step;
        }
        x += render_step_width_;
        scaled_x += scaled_x_step;
    }

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
                grid_[y][x + i] = is_in_set_(scaled_x_step / render_step_width_ * (x + i),
                                             scaled_y_step / render_step_width_ * y - 2.0f);
                grid_[y + render_step_width_][x + i] =
                    is_in_set_(scaled_x_step / render_step_width_ * (x + i),
                               scaled_y_step / render_step_width_ * (y + render_step_width_) - 2.0f);
                grid_[y + i][x] = is_in_set_(scaled_x_step / render_step_width_ * x,
                                             scaled_y_step / render_step_width_ * (y + i) - 2.0f);
                grid_[y + i][x + render_step_width_] =
                    is_in_set_(scaled_x_step / render_step_width_ * (x + render_step_width_),
                               scaled_y_step / render_step_width_ * (y + i) - 2.0f);
            }

            // fill in checkerboard pattern
            for (int i = 1; i < render_step_width_; i++) {
                for (int j = 1; j < render_step_width_; j++) {
                    if ((i + j) % 2 != 0) continue;
                    grid_[y + i][x + j] = is_in_set_(scaled_x_step / render_step_width_ * (x + j),
                                                     scaled_y_step / render_step_width_ * (y + i) - 2.0f);
                }
            }

            // fill in the rest by averaging the 4 neighbors
            for (int i = 1; i < render_step_width_; i++) {
                for (int j = 1; j < render_step_width_; j++) {
                    if ((i + j) % 2 == 0) continue;

                    grid_[y + i][x + j] = (grid_[y + i - 1][x + j] + grid_[y + i + 1][x + j] + grid_[y + i][x + j - 1] +
                                           grid_[y + i][x + j + 1]) /
                                          4.0f;
                }
            }
        }
    }
}

void Sketch::calculate_c_() noexcept {
    const int index = static_cast<int>(animation_progress_ * (c_points_.size() - 1));
    const float progress = std::fmod(animation_progress_ * (c_points_.size() - 1), 1.0f);

    c_real_ = std::lerp(c_points_[index].first, c_points_[index + 1].first, progress);
    c_imag_ = std::lerp(c_points_[index].second, c_points_[index + 1].second, progress);
}

float Sketch::is_in_set_(const float& x, const float& y) const noexcept {
    float z_real = x;
    float z_imag = y;

    int iterations = 0;
    while (++iterations < max_iterations_) {
        const float z_real_temp = z_real * z_real - z_imag * z_imag + c_real_;
        z_imag = 2.0f * z_real * z_imag + c_imag_;
        z_real = z_real_temp;

        if (z_real > 2.0f || z_real < -2.0f || z_imag > 2.0f || z_imag < -2.0f) break;
    }

    if (iterations == 1) return 0.0f;

    const float mod = std::sqrt(z_real * z_real + z_imag * z_imag);
    const float smooth = static_cast<float>(iterations) - std::log2(std::max(1.0f, std::log2(mod)));
    return smooth / static_cast<float>(max_iterations_);
}

std::tuple<int, int, int> Sketch::get_color_(const float& t) const noexcept {
    if (t == 2.0f) return std::tuple<int, int, int>(255, 0, 0);

    const int r = 9 * (1 - t) * t * t * t * 255;
    const int g = 15 * (1 - t) * (1 - t) * t * t * 255;
    const int b = 8.5 * (1 - t) * (1 - t) * (1 - t) * t * 255;

    return std::tuple<int, int, int>(r, g, b);
}
