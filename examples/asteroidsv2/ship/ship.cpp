#include "ship.hpp"

#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/rotate_vector.hpp>

void Ship::initializeGL(GLuint program) {
  terminateGL();

  m_program = program;
  m_colorLoc = glGetUniformLocation(m_program, "color");
  m_rotationLoc = glGetUniformLocation(m_program, "rotation");
  m_scaleLoc = glGetUniformLocation(m_program, "scale");
  m_translationLoc = glGetUniformLocation(m_program, "translation");

  m_rotation = 0.0f;
  m_translation = glm::vec2(0);
  m_velocity_horizontal = glm::vec2(0.2f, 0);
  m_velocity = glm::vec2(0);

  std::array<glm::vec2, 24> positions{
      // Ship body
      glm::vec2{-02.5f, +12.5f},
      glm::vec2{-09.5f, -07.5f},
      glm::vec2{-03.5f, -12.5f},
      glm::vec2{+03.5f, -12.5f},
      glm::vec2{+09.5f, -07.5f},
      glm::vec2{0.0f, -15.5f},
      glm::vec2{+15.5f, +02.5f},
      glm::vec2{+02.5f, +12.5f},

      // Cannon left
      glm::vec2{-12.5f, +10.5f},
      glm::vec2{-12.5f, +04.0f},
      glm::vec2{-09.5f, +04.0f},
      glm::vec2{-09.5f, +10.5f},

      // Cannon right
      glm::vec2{+09.5f, +10.5f},
      glm::vec2{+09.5f, +04.0f},
      glm::vec2{+12.5f, +04.0f},
      glm::vec2{+12.5f, +10.5f},

      // Thruster trail (left)
      glm::vec2{-10.0f, -07.5f},
      glm::vec2{-07.5f, -18.0f},
      glm::vec2{-05.0f, -07.5f},

      // Thruster trail (right)
      glm::vec2{+05.0f, -07.5f},
      glm::vec2{+07.5f, -18.0f},
      glm::vec2{+10.0f, -07.5f},
  };

  // Normalize
  for (auto &position : positions) {
    position /= glm::vec2{15.5f, 15.5f};
  }

  std::array indices{0, 1, 2, 0, 2, 3, 7, 0, 3, 7, 3, 4, 2, 3, 5,
                     // Thruster trails
                     16, 17, 18, 19, 20, 21};

  // Generate VBO
  glGenBuffers(1, &m_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions.data(),
               GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate EBO
  glGenBuffers(1, &m_ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(),
               GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  GLint positionAttribute{glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  glGenVertexArrays(1, &m_vao);

  // Bind vertex attributes to current VAO
  glBindVertexArray(m_vao);

  glEnableVertexAttribArray(positionAttribute);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

  // End of binding to current VAO
  glBindVertexArray(0);
}

void Ship::paintGL(const GameData &gameData) {
  if (gameData.m_state != State::Playing) return;

  glUseProgram(m_program);

  glBindVertexArray(m_vao);

  glUniform1f(m_scaleLoc, m_scale);
  glUniform1f(m_rotationLoc, m_rotation);
  glUniform2fv(m_translationLoc, 1, &m_translation.x);

  // Restart thruster blink timer every 100 ms
  if (m_trailBlinkTimer.elapsed() > 100.0 / 1000.0) m_trailBlinkTimer.restart();

  if (gameData.m_input[static_cast<size_t>(Input::Up)]) {
    // Show thruster trail during 50 ms
    if (m_trailBlinkTimer.elapsed() < 50.0 / 1000.0) {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      // 50% transparent
      glUniform4f(m_colorLoc, 1, 1, 1, 0.5f);

      glDrawElements(GL_TRIANGLES, 14 * 3, GL_UNSIGNED_INT, nullptr);

      glDisable(GL_BLEND);
    }
  }

  glUniform4fv(m_colorLoc, 1, &m_color.r);
  glDrawElements(GL_TRIANGLES, 12 * 3, GL_UNSIGNED_INT, nullptr);

  glBindVertexArray(0);

  glUseProgram(0);
}

void Ship::terminateGL() {
  glDeleteBuffers(1, &m_vbo);
  glDeleteBuffers(1, &m_ebo);
  glDeleteVertexArrays(1, &m_vao);
}

void Ship::update(const GameData &gameData, float deltaTime) {
  // Translation
  if (gameData.m_input[static_cast<size_t>(Input::Left)])
    m_translation -= m_velocity_horizontal * deltaTime;

  if (gameData.m_input[static_cast<size_t>(Input::Right)])
    m_translation += m_velocity_horizontal * deltaTime;

  // Apply thrust
  if (gameData.m_input[static_cast<size_t>(Input::Up)] &&
      gameData.m_state == State::Playing) {
    // Thrust in the forward vector
    glm::vec2 forward = glm::rotate(glm::vec2{0.0f, 1.0f}, m_rotation);
    m_velocity += forward * deltaTime;
    m_velocity_horizontal += glm::vec2{0.1f, 0} * deltaTime;
  }

  if (gameData.m_input[static_cast<size_t>(Input::Down)] &&
      gameData.m_state == State::Playing) {
    // Thrust in the forward vector
    glm::vec2 forward = glm::rotate(glm::vec2{0.0f, 1.0f}, m_rotation);

    if ((m_velocity - forward * deltaTime).y > 0) {
      m_velocity -= forward * deltaTime;
      m_velocity_horizontal -= glm::vec2{0.1f, 0} * deltaTime;
    }
  }
}