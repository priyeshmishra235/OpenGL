#version 330
// Specifies the version of GLSL (OpenGL Shading Language) being used.
// Version 330 corresponds to OpenGL 3.3 Core Profile.

in vec4 vCol;
// 'vCol' is an input varying variable received from the vertex shader.
// It represents the interpolated color at this fragment (pixel).
// Must match the name/type of an `out` variable from the vertex shader.

// For example, if your vertex shader outputs:
//    out vec4 vCol;
// Then this fragment shader will receive the interpolated value here.

out vec4 colour;
// This is the output color of this fragment.
// The final pixel color that gets written to the framebuffer (screen) will be whatever value is assigned to 'colour'.
// This replaces the old fixed-function `gl_FragColor` from legacy GLSL.

void main()
{
    colour = vCol;
    // Assign the interpolated input color directly to the output.
    // No lighting, shading, or texture processing — pure passthrough color.
    // Each fragment will receive a color computed by interpolating
    // the vertex colors based on screen space rasterization rules.
}

