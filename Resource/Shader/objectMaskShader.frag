#version 460 core
//ObjectMaskShader.frag

in vec2 vTexCoord;
uniform sampler2D uObjectTexture;

layout(location = 0) out vec4 FragColor;

void main() {
    float alpha = texture(uObjectTexture, vTexCoord).a;
    float mask = step(0.01, alpha);
    FragColor = vec4(mask, mask, mask, 1.0); // ✔️ vec4로 출력
}