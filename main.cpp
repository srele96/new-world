#include <SDL_video.h>
#include <iostream>
#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "SDL2/SDL.h"

int main() {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
    std::cout << "SDL_Init failed: " << SDL_GetError();
    return 1;
  }
  IMGUI_CHECKVERSION();

  SDL_Window* window = SDL_CreateWindow(
    "Test",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    1280,
    720,
    SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
  );

  // TODO: Test this implementation

  return 0;
}
