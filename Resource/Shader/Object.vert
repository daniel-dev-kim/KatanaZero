#version 460 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 vTexCoord;
out vec2 vWorldPos; // 🔹 추가된 월드 좌표
out vec2 vTexCenterPos; // 추가
out vec2 vMaskUV; // 🔹 마스크 UV 추가

uniform mat4 uMaskModelInverse;     // 마스크의 모델 행렬 (정적인 눈 위치)
uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

uniform vec2 uUVOffset;
uniform vec2 uUVSize;

void main()
{
    vTexCoord = aTexCoord * uUVSize + uUVOffset;

    vec4 worldPos = uModel * vec4(aPos, 0.0, 1.0);
    vWorldPos = worldPos.xy; // 🔹 월드 좌표 전달
    
    vec4 centerWorld = uModel * vec4(0.0, 0.0, 0.0, 1.0); // 중심은 항상 모델 공간의 (0, 0)
    vTexCenterPos = centerWorld.xy;
    
    // 🔹 마스크 공간에서의 좌표 계산
    vec4 localInMask = uMaskModelInverse * worldPos;
    // 또는, 마스크 텍스처가 [0, 1] UV 기준이라면 그냥 local 좌표 사용
    // → 보통 위 방법을 사용함
    vec2 uvInMask = localInMask.xy;

    vMaskUV = uvInMask.xy * 0.5 + 0.5;

    gl_Position = uProjection * uView * worldPos;
}