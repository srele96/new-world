#include "BS_thread_pool.hpp"
#include "SDL.h"
#include "SDL_video.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_sdl2.h"
#include "curl/curl.h"
#include "glad/glad.h"
#include "imgui.h"
// TODO: Figure out the correct include #include <boost/json.hpp>
// Without the <> include, i get the warning, unused directly
// I am not sure if this is the correct way to handle the import correctly
#include <boost/json.hpp>
#include <chrono>
#include <curl/easy.h>
#include <future>
#include <iostream>
#include <stdexcept>
#include <string>

static size_t write_cb(char *ptr, size_t size, size_t nmemb, void *userdata) {
  // What now?
  // Supposedly, the std::cout is not safe inside the write callback?
  std::cout << "Write callback" << std::endl;
  auto *out = static_cast<std::string *>(userdata);
  const size_t total{size * nmemb};
  // Wrap in try/catch, may throw
  out->append(ptr, total);
  return total;
}

int main() {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
    std::cout << "SDL_Init failed: " << SDL_GetError();
    return 1;
  }

  curl_global_init(CURL_GLOBAL_DEFAULT);

  boost::json::value jv{
      boost::json::parse(R"({"foo":"something","bar":"something else"})")};
  boost::json::object obj{jv.as_object()};
  std::cout << "Parsed fields:\n";
  if (auto *p = obj.if_contains("foo")) {
    std::cout << "foo = " << p->as_string() << "\n";
  }
  if (auto *p = obj.if_contains("bar")) {
    std::cout << "bar = " << p->as_string() << "\n";
  }

  BS::thread_pool pool;
  auto result{pool.submit_task([]() { std::cout << "do stuff\n"; })};

  auto checkStatus{[&result]() {
    std::future_status status = result.wait_for(std::chrono::seconds(0));
    switch (status) {
    case std::future_status::deferred: {
      std::cout << "deferred\n";
      break;
    }
    case std::future_status::ready: {
      std::cout << "ready\n";
      break;
    }
    case std::future_status::timeout: {
      std::cout << "timeout\n";
      break;
    }
    default: {
      std::cout << "Something went wrong deciding which status we received.\n";
    }
    }
  }};

  checkStatus();

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

  SDL_Window *window =
      SDL_CreateWindow("Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

  SDL_GLContext gl_context = SDL_GL_CreateContext(window);
  gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);
  SDL_GL_MakeCurrent(window, gl_context);
  SDL_GL_SetSwapInterval(1); // vsync

  // --- ImGui ---
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;

  ImGui::StyleColorsDark();

  ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
  ImGui_ImplOpenGL3_Init("#version 330");

  bool running = true;

  std::future<std::string> fetchData;

  // --- Main loop ---
  while (running) {
    // TODO: Add some sort of polling here, so we can hold the result data once retrieved...

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      ImGui_ImplSDL2_ProcessEvent(&event);
      if (event.type == SDL_QUIT)
        running = false;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // --- UI ---
    ImGui::Begin("Hello");

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                          ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                          ImVec4(0.8f, 0.0f, 0.0f, 1.0f));

    ImVec2 displaySize = ImGui::GetIO().DisplaySize;

    ImGui::PopStyleColor(3);

    if (ImGui::Button("Fetch")) {
      fetchData = pool.submit_task([]() {
        CURL *curl{curl_easy_init()};
        if (!curl) {
          // Not sure how to handle this case and what the consequence of
          // throwing will be
          throw std::runtime_error("curl_easy_init failed.");
        }

        std::string response;

        // Check out this naruto API, how can i use it? Some cool ideas!
        curl_easy_setopt(curl, CURLOPT_URL,
                         "https://dattebayo-api.onrender.com/characters");
        // Write callback?
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5L);
        // Useful for debugging
        // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        // Do i even need user agent?
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "my-app/1.0");

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
          std::cout << "Bad response code: " << curl_easy_strerror(res) << "\n";
          curl_easy_cleanup(curl);
          throw std::runtime_error(curl_easy_strerror(res));
        }

        std::cout << "All OK, task done\n";
        curl_easy_cleanup(curl);
        return response;
      });
    }

    if (ImGui::Button("Check result")) {
      if (fetchData.wait_for(std::chrono::seconds(0)) ==
          std::future_status::ready) {
        std::cout << "The data is ready:\n";
        const std::string res{fetchData.get()};
        namespace json = boost::json;
        json::value jv{json::parse(res)};
        if (!jv.is_object()) {
          std::cerr << "Invalid json object\n";
        } else {
          std::cout << "Printing json data:\n";

          boost::json::object const& obj {jv.as_object()};

          for(auto const& [key, value] : obj) {
            std::cout << key << ", ";
          }
        }
      } else {
        std::cout << "Data is not ready yet\n";
      }
    }

    ImGui::Text("ImGui is working.");
    ImGui::Button("Button");
    ImGui::End();

    // --- Render ---
    ImGui::Render();
    // --- Do i need this? ---
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);
  }

  // --- Cleanup ---
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  SDL_GL_DeleteContext(gl_context);
  SDL_DestroyWindow(window);
  SDL_Quit();

  curl_global_cleanup();

  return 0;
}
