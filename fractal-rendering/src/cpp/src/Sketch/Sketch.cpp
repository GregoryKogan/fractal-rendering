#include "Sketch.hpp"

Sketch::Sketch(SDL_Renderer* renderer) : renderer_(renderer) { setup(); }

void Sketch::setup() {
    cell_width_ = std::max(std::min(window_width_, window_height_) / resolution_, 1);
    grid_.resize(window_height_ / cell_width_);
    for (auto& row : grid_) row.resize(window_width_ / cell_width_ / 2);
}

void Sketch::update(const double& delta_time) {
    animation_progress_ += animation_speed_ * delta_time;
    if (animation_progress_ >= 1.0f) animation_progress_ = 0.0f;
    calculate_c_();

    const float scaled_x_step = 2.0f / (grid_[0].size() - 1);
    const float scaled_y_step = 4.0f / (grid_.size() - 1);
    const int grid_width = grid_[0].size();
    const int grid_height = grid_.size();

    int x = 0;
    float scaled_x = 0.0f;
    while (x < grid_width) {
        int y = 0;
        float scaled_y = -2.0f;
        while (y < grid_height) {
            if ((x + y) % 2 == 0) {
                y++;
                scaled_y += scaled_y_step;
                continue;
            }
            grid_[y][x] = is_in_set_(scaled_x, scaled_y);

            y++;
            scaled_y += scaled_y_step;
        }
        x++;
        scaled_x += scaled_x_step;
    }
}

#include <iostream>
void Sketch::draw() const noexcept {
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
    SDL_RenderClear(renderer_);

    const int grid_width = grid_[0].size();
    const int grid_height = grid_.size();

    for (int x = 0; x < grid_width; x++) {
        for (int y = 0; y < grid_height; y++) {
            float t;
            if ((x + y) % 2 == 0 && x > 0 && y > 0 && x < grid_width - 1 && y < grid_height - 1 &&
                grid_[y - 1][x] == grid_[y + 1][x] && grid_[y][x - 1] == grid_[y][x + 1] &&
                grid_[y - 1][x] == grid_[y][x - 1] && (grid_[y - 1][x] == 0.0f || grid_[y - 1][x] == 1.0f))
                continue;
            else
                t = is_in_set_((float)x / (grid_width - 1) * 2.0f, (float)y / (grid_height - 1) * 4.0f - 2.0f);

            if (t == 0.0f || t == 1.0f) continue;

            const int r = 9 * (1 - t) * t * t * t * 255;
            const int g = 15 * (1 - t) * (1 - t) * t * t * 255;
            const int b = 8.5 * (1 - t) * (1 - t) * (1 - t) * t * 255;

            SDL_SetRenderDrawColor(renderer_, r, g, b, 255);
            SDL_Rect rect{window_width_ / 2 + x * cell_width_, y * cell_width_, cell_width_, cell_width_};
            SDL_Rect rect_mirror{window_width_ / 2 - x * cell_width_ - cell_width_,
                                 window_height_ - y * cell_width_ - cell_width_, cell_width_, cell_width_};
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