#version 330 core
out vec4 FragColor;
in vec3 ourColor;
in vec2 TexCoord;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float mixFactor;
void main()
{
    vec4 t1 = texture(texture1, TexCoord);
    vec4 t2 = texture(texture2, vec2(TexCoord.x, TexCoord.y));
    FragColor = mix(t1, t2, mixFactor);
}
