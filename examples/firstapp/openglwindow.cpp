#include "openglwindow.hpp"

#include <fmt/core.h>
#include <imgui.h>

void OpenGLWindow::initializeGL() {
  auto windowSettings{getWindowSettings()};
  fmt::print("Initial window size: {}x{}\n", windowSettings.width,
             windowSettings.height);
}

void OpenGLWindow::paintGL() {
  // Set the clear color
  glClearColor(m_clearColor[0], m_clearColor[1], m_clearColor[2],
               m_clearColor[3]);

  // Clear the color buffer
  glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLWindow::paintUI() {
  // Parent class will show fullscreen button and FPS meter
  // abcg::OpenGLWindow::paintUI();

  // Our own ImGui widgets go below
  {
    // Window begin
    
    auto windowSettings{getWindowSettings()};
    ImGui::SetNextWindowPos(ImVec2(0,0));
    ImGui::SetNextWindowSize(ImVec2(windowSettings.width, windowSettings.height));
    ImGui::Begin("Hello, first app!");
    ImGui::Columns(3, "mycolumns3", false);
    ImGui::Separator();
    for (int n = 0; n < 9; n++) {
      char label[32];
      sprintf(label, "Item %d", n);
    //   if (ImGui::Selectable(label)) {
    //   }
      ImGui::Button(label, ImVec2(100, 0.0f));
      ImGui::NextColumn();
    }
    // //Static text
    // auto windowSettings{getWindowSettings()};
    // ImGui::Text("Current window size: %dx%d (in windowed mode)",
    // windowSettings.width, windowSettings.height);

    // //Slider from 0.0f to 1.0f
    // static float f{};
    // ImGui::SliderFloat("float", &f, 0.0f, 1.0f);

    // //ColorEdit to change the clear color
    // ImGui::ColorEdit3("clear color", m_clearColor.data());

    // //More static text
    // ImGui::Text("Application avg %.3f ms/frame (%.1f) FPS",
    // 1000.0/ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    // Window end
    ImGui::End();
  }
}