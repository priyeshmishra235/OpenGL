#include "Mesh.h"

// Constructor: Initializes all GPU buffer handles to 0.
// This avoids accidental usage of uninitialized OpenGL objects.
Mesh::Mesh() {
  VAO = 0;
  VBO = 0;
  IBO = 0;
  indexCount = 0;
}

void Mesh::CreateMesh(GLfloat *vertices, unsigned int *indices,
                      unsigned int numOfVertices, unsigned int numOfIndices) {
  // Store how many indices this mesh has. This will be used in glDrawElements.
  indexCount = numOfIndices;

  // Generate a Vertex Array Object. This stores the configuration of vertex
  // attributes, and which buffers are bound to which targets.
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO); // Binding the VAO makes it the current one.

  // Generate and bind an Index Buffer Object (Element Array Buffer).
  // This holds indices into the vertex buffer, allowing you to reuse vertices
  // efficiently.
  glGenBuffers(1, &IBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
  // Upload the index data to GPU memory.
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, numOfIndices * sizeof(indices[0]),
               indices, GL_STATIC_DRAW);

  // Generate and bind a Vertex Buffer Object.
  // This stores raw vertex data (like positions, normals, texture coordinates,
  // etc.)
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  // Upload the vertex data to GPU memory.
  glBufferData(GL_ARRAY_BUFFER, numOfVertices * sizeof(vertices[0]), vertices,
               GL_STATIC_DRAW);

  // Configure the layout of the vertex buffer data.
  // This tells OpenGL how to interpret the raw vertex buffer:
  // - location = 0 (matches the layout location in the vertex shader)
  // - 3 components per vertex (x, y, z)
  // - GL_FLOAT: each component is a float
  // - GL_FALSE: no normalization
  // - 0 stride: tightly packed
  // - 0 offset: starts at the beginning of the buffer
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0); // Enable this attribute for rendering.

  // Cleanup bindings to avoid accidental modification.
  // Note: GL_ELEMENT_ARRAY_BUFFER is stored in the VAO, so it must remain bound
  // while VAO is bound.
  glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind VBO
  glBindVertexArray(0);             // Unbind VAO
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
               0); // Unbind IBO (safe now that VAO is unbound)
}

void Mesh::RenderMesh() {
  // Bind the VAO which contains all the state (VBO, IBO, attribute layout).
  glBindVertexArray(VAO);
  // Bind the index buffer in case it's required (some drivers store it outside
  // VAO).
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

  // Issue the draw call.
  // This tells the GPU to draw `indexCount` indices from the IBO as triangles.
  glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

  // Unbind VAO and IBO to prevent side-effects.
  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// Destructor: Frees up GPU resources.
// Without this, your application may leak memory every time a mesh is created.
Mesh::~Mesh() {
  // Delete the buffers only if they were created.
  // commented it out because it was not in course but good practice
  if (IBO != 0)
    glDeleteBuffers(1, &IBO);
  if (VBO != 0)
    glDeleteBuffers(1, &VBO);
  if (VAO != 0)
    glDeleteVertexArrays(1, &VAO);

  // Reset to zero to avoid dangling references.
  IBO = 0;
  VBO = 0;
  VAO = 0;
  indexCount = 0;
}
