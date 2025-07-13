#include "Mesh.h"
Mesh::Mesh() {
  VAO = 0;
  VBO = 0;
  IBO = 0;
  indexCount = 0;
}

void Mesh::CreateMesh(GLfloat *vertices, unsigned int *indices,
                      unsigned int numVerts, unsigned int numIdx) {
  indexCount = numIdx;

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glGenBuffers(1, &IBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIdx * sizeof(indices[0]), indices,
               GL_STATIC_DRAW);

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, numVerts * sizeof(vertices[0]), vertices,
               GL_STATIC_DRAW);

  // position  ─ layout(location = 0)
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *)0);
  glEnableVertexAttribArray(0);

  // texCoord  ─ layout(location = 1)
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8,
                        (void *)(sizeof(float) * 3));
  glEnableVertexAttribArray(1);

  // normal    ─ layout(location = 2)
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8,
                        (void *)(sizeof(float) * 5));
  glEnableVertexAttribArray(2);

  glBindVertexArray(0);
}

void Mesh::RenderMesh() {
  glBindVertexArray(VAO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
  glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
Mesh::~Mesh() {
  if (IBO != 0)
    glDeleteBuffers(1, &IBO);
  if (VBO != 0)
    glDeleteBuffers(1, &VBO);
  if (VAO != 0)
    glDeleteVertexArrays(1, &VAO);
  IBO = 0;
  VBO = 0;
  VAO = 0;
  indexCount = 0;
}
