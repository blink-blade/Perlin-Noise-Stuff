#version 460 core
out vec4 FragColor;
in vec3 ourColor;
in vec2 TexCoord;

uniform vec4 timeOffsetColor;
uniform sampler2D texture1;
uniform float mixAmount;

void main() {
    //vec2 fragCoord = gl_FragCoord.xy;
    //FragColor = vec4(fragCoord.x / 800.0f, 0.0f, fragCoord.y / 2000.0f, 1.0f);
    //FragColor = vec4(ourColor.x, ourColor.y, ourColor.z, 1.0);
 FragColor = texture(texture1, TexCoord) * vec4(ourColor, 1.0);
    //FragColor = vec4(FragColor.x - timeOffsetColor.y, FragColor.y + timeOffsetColor.y, FragColor.z + timeOffsetColor.y, 1.0f);
};