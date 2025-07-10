#version 330
// GLSL version 3.30 — matches OpenGL 3.3 Core Profile
// Ensures support for modern shader features like layout qualifiers and uniforms

layout(location = 0) in vec3 pos;
// 'pos' is a vertex attribute: a 3-component float vector (x, y, z)
// It comes from the vertex buffer (VBO) bound in your Mesh class
//
// Because of `layout(location = 0)`,
// this must match the index used in `glVertexAttribPointer(...)` in Mesh.cpp:
//     glVertexAttribPointer(0, 3, GL_FLOAT, ...)
//     glEnableVertexAttribArray(0);

out vec4 vCol;
// This will be interpolated across the triangle and passed to the fragment shader
// You use it to assign a color to each pixel based on the vertex position

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;
// 'model' transforms the object from model space to world space (translation, rotation, scaling)
// 'projection' transforms the 3D position into clip space (perspective or orthographic projection)
//
// These are set in C++ using:
//     glGetUniformLocation(shader, "model")
//     glUniformMatrix4fv(..., modelMatrix)
//     glUniformMatrix4fv(..., projectionMatrix)

void main()
{
    // Transform the vertex position by model → projection
    // This moves the vertex from object space to clip space (standard OpenGL pipeline)
    gl_Position = projection * view * model * vec4(pos, 1.0);
    // Generate a color from position:
    // - This clamps position values between 0.0 and 1.0 to avoid out-of-bounds RGB
    // - Effectively maps x, y, z to R, G, B respectively
    // - Adds 1.0 as alpha for full opacity
    vCol = vec4(clamp(pos, 0.0f, 1.0f), 1.0f);
    // This color will then be interpolated across the triangle
    // and passed to the fragment shader as 'vCol'
}
