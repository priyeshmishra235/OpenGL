#version 330 core
out vec4 FragColor;
in vec3 lc;
void main() {
    FragColor = vec4(lc, 1.0);
}
