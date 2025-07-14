
#version 460 core
out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D skyboxTexture;

void main() {
    FragColor = vec4(texture(skyboxTexture, TexCoords).xyz, 1.0); // set all 4 vector values to 1.0
}