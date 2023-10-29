#include "Sketch.hpp"

Sketch::Sketch(SDL_Renderer* renderer) : renderer_(renderer) { setup(); }

void Sketch::setup() {}

void Sketch::update(double delta_time) {
    animation_progress_ += animation_speed_ * delta_time;
    if (animation_progress_ >= 1.0f) animation_progress_ = 0.0f;
    calculate_c_();
}

void Sketch::draw() const noexcept {
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
    SDL_RenderClear(renderer_);

    int cell_width = std::max(std::min(window_width_, window_height_) / resolution_, 1);

    for (int x = 0; x <= window_width_ / cell_width; ++x) {
        for (int y = 0; y <= window_height_ / cell_width; ++y) {
            float scaled_x = std::lerp(-2.0, 2.0, float(x) / (window_width_ / cell_width));
            float scaled_y = std::lerp(-2.0, 2.0, float(y) / (window_height_ / cell_width));
            float iters = is_in_set_(scaled_x, scaled_y);

            int brightness = 255 * iters / max_iterations_;
            SDL_SetRenderDrawColor(renderer_, brightness, brightness, brightness, 255);
            SDL_Rect rect{x * cell_width, y * cell_width, cell_width, cell_width};
            SDL_RenderFillRect(renderer_, &rect);
        }
    }

    SDL_RenderPresent(renderer_);
}

void Sketch::set_window_size(int width, int height) noexcept {
    window_width_ = width;
    window_height_ = height;
}

void Sketch::calculate_c_() noexcept {
    int index = int(animation_progress_ * (c_points_.size() - 1));
    float progress = std::fmod(animation_progress_ * (c_points_.size() - 1), 1.0f);

    float c_real = std::lerp(c_points_[index].real(), c_points_[index + 1].real(), progress);
    float c_imag = std::lerp(c_points_[index].imag(), c_points_[index + 1].imag(), progress);
    c_ = std::complex<float>(c_real, c_imag);
}

float Sketch::is_in_set_(float x, float y) const noexcept {
    std::complex<float> z(x, y);

    int iterations = 0;
    while (++iterations < max_iterations_) {
        z = static_cast<std::complex<float>>(std::pow(z, 2)) + c_;
        if (std::norm(z) > 4) break;
    }

    if (iterations < 2) return 0.0f;

    float mod = std::sqrt(std::norm(z));
    float smooth = float(iterations) - std::log2(std::max(1.0f, std::log2(mod)));
    return smooth;
}
