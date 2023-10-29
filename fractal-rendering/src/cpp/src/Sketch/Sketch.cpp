#include "Sketch.hpp"

Sketch::Sketch(SDL_Renderer* renderer) : renderer_(renderer) { setup(); }

void Sketch::setup() {
    // random block position and velocity
    block_x_ = rand() % window_width_;
    block_y_ = rand() % window_height_;
    block_vel_x_ = (rand() % 1000) / 2000.0;
    block_vel_y_ = (rand() % 1000) / 2000.0;
}

void Sketch::update(double delta_time) {
    block_x_ += block_vel_x_ * delta_time;
    block_y_ += block_vel_y_ * delta_time;

    if (block_x_ < 0) {
        block_x_ = 0;
        block_vel_x_ *= -1;
    } else if (block_x_ + block_w_ > window_width_) {
        block_x_ = window_width_ - block_w_;
        block_vel_x_ *= -1;
    }

    if (block_y_ < 0) {
        block_y_ = 0;
        block_vel_y_ *= -1;
    } else if (block_y_ + block_h_ > window_height_) {
        block_y_ = window_height_ - block_h_;
        block_vel_y_ *= -1;
    }
}

void Sketch::draw() const noexcept {
    SDL_SetRenderDrawColor(renderer_, std::get<0>(bg_color_), std::get<1>(bg_color_), std::get<2>(bg_color_), 255);
    SDL_RenderClear(renderer_);

    SDL_Rect rect;
    rect.x = block_x_;
    rect.y = block_y_;
    rect.w = block_w_;
    rect.h = block_h_;

    SDL_SetRenderDrawColor(renderer_, 169, 177, 214, 255);
    SDL_RenderFillRect(renderer_, &rect);
    SDL_SetRenderDrawColor(renderer_, std::get<0>(bg_color_), std::get<1>(bg_color_), std::get<2>(bg_color_), 255);

    SDL_RenderPresent(renderer_);
}

void Sketch::set_window_size(int width, int height) noexcept {
    window_width_ = width;
    window_height_ = height;
}
