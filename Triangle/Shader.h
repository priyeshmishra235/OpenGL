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

  GLuint GetAmbientIntensityLocation() { return this->uniformAmbientIntensity; }
  GLuint GetAmbientColorLocation() { return this->uniformAmbientColor; }
  GLuint GetDiffuseIntensityLocation() { return uniformDiffuseIntensity; }
  GLuint GetDirectionLocation() { return uniformDirection; }
  ~Shader();

private:
  GLuint shader, uniformModel, uniformProjection, uniformView,
      uniformAmbientIntensity, uniformAmbientColor, uniformDiffuseIntensity,
      uniformDirection;

  std::string readShaderCodeFromFile(const char *shaderPath);
  void addShader(GLuint theProgram, const char *shaderCode, GLenum shaderType);
  void compileShaders(const char *vShaderCode, const char *fShaderCode);
};
