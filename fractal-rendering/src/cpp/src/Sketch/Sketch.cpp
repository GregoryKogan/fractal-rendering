#include "Sketch.hpp"

Sketch::Sketch(SDL_Renderer* renderer) : renderer_(renderer) { setup(); }

void Sketch::setup() { cell_width_ = std::max(std::min(window_width_, window_height_) / resolution_, 1); }

void Sketch::update(const double& delta_time) {
    animation_progress_ += animation_speed_ * delta_time;
    if (animation_progress_ >= 1.0f) animation_progress_ = 0.0f;
    calculate_c_();
}

#include <iostream>
void Sketch::draw() const noexcept {
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
    SDL_RenderClear(renderer_);

    const float scaled_x_step = 4.0f / (window_width_ / cell_width_);
    const float scaled_y_step = 4.0f / (window_height_ / cell_width_);

    int x = window_width_ / cell_width_ / 2;
    float scaled_x = 0.0f;
    while (x <= window_width_ / cell_width_) {
        int y = 0;
        float scaled_y = -2.0f;
        while (y <= window_height_ / cell_width_) {
            const float iters = is_in_set_(scaled_x, scaled_y);

            const float t = iters / static_cast<float>(max_iterations_);
            const int r = 9 * (1 - t) * t * t * t * 255;
            const int g = 15 * (1 - t) * (1 - t) * t * t * 255;
            const int b = 8.5 * (1 - t) * (1 - t) * (1 - t) * t * 255;
            SDL_SetRenderDrawColor(renderer_, r, g, b, 255);
            SDL_Rect rect{x * cell_width_, y * cell_width_, cell_width_, cell_width_};
            SDL_Rect rect_mirror{(window_width_ / cell_width_ - x) * cell_width_,
                                 (window_height_ / cell_width_ - y) * cell_width_, cell_width_, cell_width_};

            y++;
            scaled_y += scaled_y_step;

            SDL_RenderFillRect(renderer_, &rect);
            SDL_RenderFillRect(renderer_, &rect_mirror);
        }
        x++;
        scaled_x += scaled_x_step;
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
    return smooth;
}