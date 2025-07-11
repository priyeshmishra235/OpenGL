#include <GL/glew.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#pragma once

class Shader {
public:
  Shader();

  void CreateFromFiles(const char *vShader, const char *fShader);

  void UseShader() { glUseProgram(this->shader); }

  GLuint GetModelLocation() { return this->uniformModel; }
  GLuint GetViewLocation() { return this->uniformView; }
  GLuint GetProjectionLocation() { return this->uniformProjection; }

  ~Shader();

private:
  GLuint shader;
  GLuint uniformModel;
  GLuint uniformProjection;
  GLuint uniformView;

  std::string readShaderCodeFromFile(const char *shaderPath);
  void addShader(GLuint theProgram, const char *shaderCode, GLenum shaderType);
  void compileShaders(const char *vShaderCode, const char *fShaderCode);
};
