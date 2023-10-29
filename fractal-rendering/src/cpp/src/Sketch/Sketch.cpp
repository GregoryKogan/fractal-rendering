#include "Sketch.hpp"

Sketch::Sketch(SDL_Renderer* renderer) : renderer_(renderer) { setup(); }

void Sketch::setup() { cell_width_ = std::max(std::min(window_width_, window_height_) / resolution_, 1); }

void Sketch::update(const double& delta_time) {
    animation_progress_ += animation_speed_ * delta_time;
    if (animation_progress_ >= 1.0f) animation_progress_ = 0.0f;
    calculate_c_();
}

void Sketch::draw() const noexcept {
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
    SDL_RenderClear(renderer_);

#pragma omp parallel for
    for (int x = window_width_ / cell_width_ / 2; x <= window_width_ / cell_width_; ++x) {
        for (int y = 0; y <= window_height_ / cell_width_; ++y) {
            const float scaled_x = std::lerp(-2.0f, 2.0f, float(x) / (window_width_ / cell_width_));
            const float scaled_y = std::lerp(-2.0f, 2.0f, float(y) / (window_height_ / cell_width_));
            const float iters = is_in_set_(scaled_x, scaled_y);

            const float t = iters / static_cast<float>(max_iterations_);
            const int r = 9 * (1 - t) * t * t * t * 255;
            const int g = 15 * (1 - t) * (1 - t) * t * t * 255;
            const int b = 8.5 * (1 - t) * (1 - t) * (1 - t) * t * 255;
            SDL_SetRenderDrawColor(renderer_, r, g, b, 255);
            SDL_Rect rect{x * cell_width_, y * cell_width_, cell_width_, cell_width_};
            SDL_Rect rect2{(window_width_ / cell_width_ - x) * cell_width_,
                           (window_height_ / cell_width_ - y) * cell_width_, cell_width_, cell_width_};
            SDL_RenderFillRect(renderer_, &rect);
            SDL_RenderFillRect(renderer_, &rect2);
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

    const float c_real = std::lerp(c_points_[index].real(), c_points_[index + 1].real(), progress);
    const float c_imag = std::lerp(c_points_[index].imag(), c_points_[index + 1].imag(), progress);
    c_ = std::complex<float>(c_real, c_imag);
}

float Sketch::is_in_set_(const float& x, const float& y) const noexcept {
    std::complex<float> z(x, y);

    int iterations = 0;
    while (++iterations < max_iterations_) {
        z = std::pow(z, 2.0f) + c_;
        if (std::abs(z.real()) > 2 || std::abs(z.imag()) > 2) break;
    }

    if (iterations == 1) return 0.0f;

    const float mod = std::sqrt(std::norm(z));
    const float smooth = static_cast<float>(iterations) - std::log2(std::max(1.0f, std::log2(mod)));
    return smooth;
}