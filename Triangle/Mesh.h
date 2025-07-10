#include <GL/glew.h> // Required for GLuint, gl* function types

#pragma once

// Mesh represents a renderable 3D object made of vertices and indices.
//
// Internally, this class manages:
// - VAO: Vertex Array Object, binds the layout of vertex attributes
// - VBO: Vertex Buffer Object, stores raw vertex data (positions, normals,
// etc.)
// - IBO/EBO: Index Buffer Object (aka Element Buffer Object), stores indices
// for indexed drawing
//
// This abstraction helps decouple your render logic from raw OpenGL calls.
class Mesh {
public:
  // Constructor: initializes buffer handles to 0 (invalid state)
  Mesh();

  // Creates and uploads geometry data to the GPU
  //
  // Parameters:
  // - vertices: raw float array of vertex data (e.g., positions, normals,
  // texcoords)
  // - indices: index array for indexed drawing (avoids vertex duplication)
  // - numOfVertices: number of floats in the vertex array
  // - numOfIndices: number of integers in the index array
  void CreateMesh(GLfloat *vertices, unsigned int *indices,
                  unsigned int numOfVertices, unsigned int numOfIndices);

  // Binds the VAO and issues a draw call using glDrawElements
  void RenderMesh();

  // Destructor: should free VAO/VBO/IBO using glDelete*
  ~Mesh();

private:
  // OpenGL object handles
  GLuint VAO; // Vertex Array Object: holds attribute bindings and buffer state
  GLuint VBO; // Vertex Buffer Object: holds raw vertex data
  GLuint IBO; // Index Buffer Object: holds indices for glDrawElements
  GLuint indexCount; // Number of indices to draw (used in glDrawElements)
};
