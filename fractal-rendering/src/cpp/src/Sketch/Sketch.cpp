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
    for (auto& row : grid_) row.resize(grid_width_);
}

void Sketch::update(const double& delta_time) {
    animation_progress_ += animation_speed_ * delta_time;
    if (animation_progress_ >= 1.0f) animation_progress_ = 0.0f;
    calculate_c_();

    const float scaled_x_step = 2.0f / (grid_width_ - 1);
    const float scaled_y_step = 4.0f / (grid_height_ - 1);

    int x = 0;
    float scaled_x = 0.0f;
    while (x < grid_width_) {
        int y = 0;
        float scaled_y = -2.0f;
        while (y < grid_height_) {
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

void Sketch::draw() const noexcept {
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
    SDL_RenderClear(renderer_);

    for (int x = 0; x < grid_width_; x++) {
        for (int y = 0; y < grid_height_; y++) {
            float t;
            if ((x + y) % 2 == 0 && x > 0 && y > 0 && x < grid_width_ - 1 && y < grid_height_ - 1 &&
                grid_[y - 1][x] == grid_[y + 1][x] && grid_[y][x - 1] == grid_[y][x + 1] &&
                grid_[y - 1][x] == grid_[y][x - 1] && (grid_[y - 1][x] == 0.0f || grid_[y - 1][x] == 1.0f))
                continue;
            else
                t = is_in_set_((float)x / (grid_width_ - 1) * 2.0f, (float)y / (grid_height_ - 1) * 4.0f - 2.0f);

            if (t == 0.0f || t == 1.0f) continue;

            const int r = 9 * (1 - t) * t * t * t * 255;
            const int g = 15 * (1 - t) * (1 - t) * t * t * 255;
            const int b = 8.5 * (1 - t) * (1 - t) * (1 - t) * t * 255;

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