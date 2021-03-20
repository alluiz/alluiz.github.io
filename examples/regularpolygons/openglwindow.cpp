
#include "openglwindow.hpp"
#include <imgui.h>
#include <cppitertools/itertools.hpp>
#include "abcg.hpp"

void OpenGLWindow::initializeGL() {
  
  const auto *vertexShader{R"gl(

      #version 410

      layout(location = 0) in vec2 inPosition;
      layout(location = 1) in vec4 inColor;

      uniform vec2 translation;
      uniform float scale;

      out vec4 fragColor;

      void main() {
        vec2 newPosition = inPosition * scale + translation;
        gl_Position = vec4(newPosition, 0, 1);
        fragColor = inColor;
      }

  )gl"};

  const auto *fragmentShader{R"gl(

    #version 410

    in vec4 fragColor;

    out vec4 outColor;

    void main() { outColor = fragColor; }

  )gl"};

  m_program = createProgramFromString(vertexShader, fragmentShader);

  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT);

  auto seed{std::chrono::steady_clock::now().time_since_epoch().count()};

  m_randomEngine.seed(seed);
}

void OpenGLWindow::paintGL() {
  
  if (m_elapsedTimer.elapsed() < m_delay / 1000.0) return;
    
  m_elapsedTimer.restart();

  std::uniform_int_distribution<int> intDist(3, 20);
  auto sides{intDist(m_randomEngine)};

  setupModel(sides);

  glViewport(0, 0, m_viewportWidth, m_viewportHeight);
  glUseProgram(m_program);
  
  std::uniform_real_distribution<float> rd1(-1.0f, 1.0f);
  glm::vec2 translation{rd1(m_randomEngine), rd1(m_randomEngine)};
  GLint translationLocation{glGetUniformLocation(m_program, "translation")};
  glUniform2fv(translationLocation, 1, &translation.x);

  std::uniform_real_distribution<float> rd2(0.01f, 0.25f);
  auto scale{rd2(m_randomEngine)};
  GLint scaleLocation{glGetUniformLocation(m_program, "scale")};
  glUniform1f(scaleLocation, scale);

  glBindVertexArray(m_vao);
  glDrawArrays(GL_TRIANGLE_FAN, 0, sides + 2);
  
  glBindVertexArray(0);
  glUseProgram(0);
    
}

void OpenGLWindow::paintUI() {
  
  abcg::OpenGLWindow::paintUI();

  auto widgetSize{ImVec2(200, 72)};
  ImGui::SetNextWindowPos(ImVec2(
    m_viewportWidth - widgetSize.x - 5, 
    m_viewportHeight - widgetSize.y - 5));
  
  ImGui::SetNextWindowSize(widgetSize);

  auto windowFlags{ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar};
  ImGui::Begin(" ", nullptr, windowFlags);

  ImGui::PushItemWidth(140);
  ImGui::SliderInt("Delay", &m_delay, 0, 200, "%d ms");
  ImGui::PopItemWidth();

  if (ImGui::Button("Clear window", ImVec2(-1, 30))) {
    glClear(GL_COLOR_BUFFER_BIT);
  }

  ImGui::End();

}

void OpenGLWindow::resizeGL(int width, int height) {

  m_viewportWidth = width;
  m_viewportHeight = height;

  glClear(GL_COLOR_BUFFER_BIT);

}

void OpenGLWindow::terminateGL() {
  glDeleteProgram(m_program);
  glDeleteBuffers(1, &m_vboPositions);
  glDeleteBuffers(1, &m_vboColors);
  glDeleteVertexArrays(1, &m_vao);
}

void OpenGLWindow::setupModel(int sides) {

  glDeleteBuffers(1, &m_vboPositions);
  glDeleteBuffers(1, &m_vboColors);
  glDeleteVertexArrays(1, &m_vao);

  std::uniform_real_distribution<float> rd(0.0f, 1.0f);
  glm::vec3 color1{rd(m_randomEngine), rd(m_randomEngine), rd(m_randomEngine)};
  glm::vec3 color2{rd(m_randomEngine), rd(m_randomEngine), rd(m_randomEngine)};

  sides = std::max(3, sides);

  std::vector<glm::vec2> positions(0);
  std::vector<glm::vec3> colors(0);

  positions.emplace_back(0, 0);
  colors.push_back(color1);

  auto step{M_PI * 2 / sides};
  for (auto angle : iter::range(0.0, M_PI * 2, step)) {
    positions.emplace_back(std::cos(angle), std::sin(angle));
    colors.push_back(color2);
  }

  positions.push_back(positions.at(1));
  colors.push_back(color2);

  glGenBuffers(1, &m_vboPositions);
  glBindBuffer(GL_ARRAY_BUFFER, m_vboPositions);
  glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2), positions.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glGenBuffers(1, &m_vboColors);
  glBindBuffer(GL_ARRAY_BUFFER, m_vboColors);
  glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  GLint positionAttribute{glGetAttribLocation(m_program, "inPosition")};
  GLint colorAttribute{glGetAttribLocation(m_program, "inColor")};

  glGenVertexArrays(1, &m_vao);
  glBindVertexArray(m_vao);
  
  glEnableVertexAttribArray(positionAttribute);
  glBindBuffer(GL_ARRAY_BUFFER, m_vboPositions);
  glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glEnableVertexAttribArray(colorAttribute);
  glBindBuffer(GL_ARRAY_BUFFER, m_vboColors);
  glVertexAttribPointer(colorAttribute, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(0);
}