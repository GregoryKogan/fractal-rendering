#include "Application.hpp"

Application::Application() {
    SDL_GetWindowSize(window_, &window_width_, &window_height_);
    SDL_CreateWindowAndRenderer(window_width_, window_height_, SDL_WINDOW_RESIZABLE, &window_, &renderer_);

    if (!window_ || !renderer_) {
        std::cout << "Failed to create window and renderer\n";
        std::cout << "SDL2 Error: " << SDL_GetError() << "\n";
        return;
    }

    SDL_SetRenderDrawColor(renderer_, 36, 40, 59, 255);
    last_time_ = SDL_GetTicks64();

    // random block position and velocity
    data_.block_x = rand() % window_width_;
    data_.block_y = rand() % window_height_;
    data_.block_vel_x = (rand() % 1000) / 2000.0;
    data_.block_vel_y = (rand() % 1000) / 2000.0;
}

Application::~Application() { SDL_DestroyWindow(window_); }

void Application::loop() {
    bool keep_window_open = true;
    while (keep_window_open) {
        while (SDL_PollEvent(&window_event_) > 0) {
            switch (window_event_.type) {
                case SDL_QUIT:
                    keep_window_open = false;
                    break;
                case SDL_WINDOWEVENT:
                    switch (window_event_.window.event) {
                        case SDL_WINDOWEVENT_RESIZED:
                            window_width_ = window_event_.window.data1;
                            window_height_ = window_event_.window.data2;
                            break;
                    }
                    break;
            }
        }

        Uint64 current_time = SDL_GetTicks64();
        Uint64 delta_time = current_time - last_time_;
        last_time_ = current_time;
        update((double)delta_time);
        draw();
    }
}

void Application::update(double delta_time) {
    data_.block_x += data_.block_vel_x * delta_time;
    data_.block_y += data_.block_vel_y * delta_time;

    if (data_.block_x < 0) {
        data_.block_x = 0;
        data_.block_vel_x *= -1;
    } else if (data_.block_x + data_.block_w > window_width_) {
        data_.block_x = window_width_ - data_.block_w;
        data_.block_vel_x *= -1;
    }

    if (data_.block_y < 0) {
        data_.block_y = 0;
        data_.block_vel_y *= -1;
    } else if (data_.block_y + data_.block_h > window_height_) {
        data_.block_y = window_height_ - data_.block_h;
        data_.block_vel_y *= -1;
    }
}

void Application::draw() {
    SDL_RenderClear(renderer_);

    SDL_Rect rect;
    rect.x = data_.block_x;
    rect.y = data_.block_y;
    rect.w = data_.block_w;
    rect.h = data_.block_h;

    SDL_SetRenderDrawColor(renderer_, 169, 177, 214, 255);
    SDL_RenderFillRect(renderer_, &rect);
    SDL_SetRenderDrawColor(renderer_, 36, 40, 59, 255);

    SDL_RenderPresent(renderer_);
}
