#version 460 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D screenTexture;

// 사인파 왜곡
uniform float waveStrength; // 0.0 ~ 1.0
uniform float waveWidth;    // 예: 40.0
uniform float waveOffset;

// 지그재그 왜곡
uniform float zigzagStrength; // 0.0 ~ 1.0
uniform float zigzagWidth;    // 예: 40.0
uniform float zigzagOffset;       // 예: 0.2 = 초당 이동 속도 (0~1의 UV 단위)

// 흑백 처리
uniform bool grayscale;

// 모자이크
uniform float mosaicStrength; // 0.0 (끄기) ~ 1.0 (강하게)
uniform vec2 resolution;      // 화면 해상도 (예: 1280x720)

//색 순환
uniform bool uColorCycleFull;

//색 순환 및 4분할
uniform bool uColorCycleQuadrant;

//4가지 색, 4가지 반전
uniform bool uFlipRenderWithColor;

//Y축 반전
uniform bool uFlipYOnly;

//잔상
uniform bool uPersistentTrails;
uniform float uTime;

uniform bool uGlitchEnabled;
uniform float uGlitchSeed;

vec2 random2(vec2 p) {
    return fract(sin(vec2(dot(p, vec2(127.1, 311.7)), 
                          dot(p, vec2(269.5, 183.3)))) * 43758.5453);
}
vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main()
{
    vec2 uv = TexCoords;

    // 1. 세로 사인파 왜곡
    if (waveStrength != 0.0) {
        uv.x += sin((uv.y + waveOffset) * waveWidth) * 0.02 * waveStrength;
    }
    float noiseLine = 0.0;
    // 2. 세로 지그재그 왜곡 (삼각파)
    if (zigzagStrength != 0.0) {
        uv.y = fract(uv.y - zigzagOffset); // zigzagOffset = 시간 기반 스크롤 속도

        // 삼각파 계산 (지그재그)
        float wave = abs(fract(uv.y * zigzagWidth) * 2.0 - 1.0);
        uv.x += (wave - 0.5) * 0.05 * zigzagStrength;
    }

    // 3. 모자이크
    if (mosaicStrength > 0.0) {
        float mosaicSize = mix(1.0, 40.0, mosaicStrength); // 1: 해상도 그대로, 40: 큼
        vec2 mosaicStep = mosaicSize / resolution;
        uv = floor(uv / mosaicStep) * mosaicStep + mosaicStep * 0.5;
    }

    vec4 color = texture(screenTexture, uv);

    if (noiseLine > 0.0)
    {
        color.rgb += vec3(0.15); // 밝은 줄 느낌
    }


    // 4. 흑백 처리
    if (grayscale) {
        float gray = dot(color.rgb, vec3(0.299, 0.587, 0.114));
        color = vec4(vec3(gray), color.a);
    }
    float blendFactor = 0.3; // 0.1 ~ 0.3 정도 추천


    // 5. 전체 색 순환 블렌딩
    if (uColorCycleFull) {
        if(color.rgb != vec3(0.0))
        {
            float hue = mod(uTime * 0.1, 1.0); // 0~1 범위에서 순환
            vec3 cycleColor = hsv2rgb(vec3(hue, 1.0, 1.0));
            color.rgb *= cycleColor;
            // 색상 정규화 (0~1.0 범위 유지)
            float boost = 1.5;
            color.rgb = clamp(color.rgb * boost, 0.0, 1.0);
        }
    }

    // 6. 화면 분할 색 순환
    if (uColorCycleQuadrant) {
    vec2 quad = floor(uv * 2.0);         // 0,0 | 1,0 | 0,1 | 1,1
    vec2 localUV = fract(uv * 2.0);      // 각 분할 내부의 uv

    vec4 sampled = texture(screenTexture, localUV); // ✅ 실제로 “분할”해서 그림
    if (sampled.rgb == vec3(0.0)) {
        discard; // ✴️ 완전 검정은 아예 제거
    }
    // 사분할별 색상 결정
    float hue =
    (quad == vec2(0.0, 0.0)) ? 0.08 :   // 주황
    (quad == vec2(1.0, 0.0)) ? 0.75 :   // 보라
    (quad == vec2(0.0, 1.0)) ? 0.6  :   // 파랑
                               0.33;   // 초록

    hue = mod(hue + uTime * 0.1, 1.0);
    vec3 regionColor = hsv2rgb(vec3(hue, 1.0, 0.6));

    color.rgb = sampled.rgb * regionColor;
    // 색상 정규화 (0~1.0 범위 유지)
    float boost = 1.5;
    color.rgb = clamp(color.rgb * boost, 0.0, 1.0);
    }

    // 7. x/y 반전 4분할 렌더 (각기 다른 색상)
    if (uFlipRenderWithColor) {
    vec2 uvX  = vec2(1.0 - uv.x, uv.y);
    vec2 uvY  = vec2(uv.x, 1.0 - uv.y);
    vec2 uvXY = vec2(1.0 - uv.x, 1.0 - uv.y);

    vec3 colOriginal = texture(screenTexture, uv).rgb;
    vec3 colX  = texture(screenTexture, uvX).rgb;
    vec3 colY  = texture(screenTexture, uvY).rgb;
    vec3 colXY = texture(screenTexture, uvXY).rgb;

    color.rgb = vec3(0.0);

    if (length(colOriginal) > 0.001)
        color.rgb += colOriginal * vec3(0.2, 1.0, 1.0); // 하늘색

    if (length(colX) > 0.001)
        color.rgb += colX * vec3(0.2, 1.0, 0.2); // 초록

    if (length(colY) > 0.001)
        color.rgb += colY * vec3(1.0, 0.2, 0.2); // 빨강

    if (length(colXY) > 0.001)
        color.rgb += colXY * vec3(1.0, 1.0, 0.2); // 노랑

        // 색상 정규화 (0~1.0 범위 유지)
        float boost = 1.5;
        color.rgb = clamp(color.rgb * boost, 0.0, 1.0);
    }
    // 8. 대칭
    if (uFlipYOnly) {
        vec2 uvY = vec2(1.0 - uv.x, 1.0 - uv.y);
        color = texture(screenTexture, uvY);
    }

    // 9. 잔상 효과
    if (uPersistentTrails) {
        if(color.rgb != vec3(0.0))
        {
            float hue = mod(uTime * 0.1, 1.0);
            vec3 trailColor = hsv2rgb(vec3(hue, 1.0, 1.0));
            color.rgb = mix(color.rgb, trailColor, 0.9) ; // 덧칠하듯
        }
    }

if (uGlitchEnabled) {
    float y = 0.0;
    float accumulatedHeight = 0.0;
    float maxY = 1.0;
    float blockY = uv.y;
    float foundRowIndex = -1.0;
    float rowTop = 0.0;
    float rowBottom = 0.0;

    // 무작위 줄 나누기 (아래에서 위로 블록을 구성)
    for (int i = 0; i < 100; i++) {
        // 랜덤한 높이 (0.02 ~ 0.08)
        float h = 0.02 + random2(vec2(uGlitchSeed, float(i))).x * 0.06;
        if (accumulatedHeight + h > 1.0) break;

        // 해당 줄에 포함된 경우
        if (blockY >= accumulatedHeight && blockY < accumulatedHeight + h) {
            foundRowIndex = float(i);
            rowTop = accumulatedHeight;
            rowBottom = accumulatedHeight + h;
            break;
        }

        accumulatedHeight += h;
    }

    // 줄을 찾았으면 글리치 처리
    if (foundRowIndex >= 0.0) {
        vec2 randVal = random2(vec2(foundRowIndex, uGlitchSeed));

        // 1. 좌우 왜곡
        float xOffset = (randVal.x - 0.5) * 0.1;
        uv.x += xOffset;

        // 2. 기본 색상
        vec3 glitchColor = texture(screenTexture, uv).rgb;

        // 3. 색 필터 곱 (각 줄마다 다름)
        if (randVal.y < 0.25) {
            vec3 colorFilter = hsv2rgb(vec3(randVal.x, 0.6, 1.0));
            glitchColor *= colorFilter;
        }

        color = vec4(glitchColor, 1.0);
    }
}

    FragColor = color;
}