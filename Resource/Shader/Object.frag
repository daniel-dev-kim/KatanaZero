#version 460 core

in vec2 vTexCoord;
in vec2 vWorldPos;
in vec2 vTexCenterPos;
in vec2 vMaskUV;

out vec4 FragColor;

uniform vec2 uUVOffset;
uniform vec2 uUVSize;
uniform sampler2D uTexture1;
uniform sampler2D uTexture2;
uniform float uBlendFactor;
uniform bool uIsAfterimage;
uniform vec4 uColor;
uniform vec2 uLightPos;
uniform vec3 uLightColor;
uniform float uLightPower;
uniform bool uUseLighting;
uniform bool uUseTexture;
uniform vec2 uFillRatio;
uniform float uTime;
uniform bool uUseGlowEffect;
uniform bool uIsSmokeEffect;
uniform sampler2D uObjectMask;
uniform vec2 uScreenSize;
uniform bool uUseBounding;
uniform vec2 uWorldRenderLowerBound;
uniform vec2 uWorldRenderUpperBound;
uniform bool uUseMask;
uniform sampler2D uMask;
uniform bool uEnableObjectWave;
uniform float uObjectWaveStrength;
uniform float uObjectWaveWidth;
uniform float uObjectWaveOffset;
uniform int uEnableObjectGlitch;
uniform float uObjectGlitchSeed;
uniform bool uEnableTVNoise;
uniform float uTVNoiseWidth;
vec2 random2(vec2 p) {
    return fract(sin(vec2(dot(p, vec2(127.1, 311.7)),
                          dot(p, vec2(269.5, 183.3)))) * 43758.5453);
}

vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
    float foundBlockIndex = -1.0; // ✴️ 여기에 전역처럼 미리 선언


    if (uUseBounding && (vWorldPos.x < uWorldRenderLowerBound.x ||
                         vWorldPos.y > uWorldRenderLowerBound.y ||
                         vWorldPos.x > uWorldRenderUpperBound.x ||
                         vWorldPos.y < uWorldRenderUpperBound.y))
        discard;

    vec2 correctedUV = vTexCoord * uUVSize + uUVOffset;

    // 글리치 (UV 왜곡 적용 위치 수정)
    if (uEnableObjectGlitch > 0) {
        vec2 blockUV = correctedUV;
        float accumulated = 0.0;

        if (uEnableObjectGlitch == 1) {
            for (int i = 0; i < 100; i++) {
                float h = 0.02 + random2(vec2(uObjectGlitchSeed, float(i))).x * 0.06;
                if (accumulated + h > 1.0) break;
                if (correctedUV.y >= accumulated && correctedUV.y < accumulated + h) {
                    foundBlockIndex = float(i);
                    break;
                }
                accumulated += h;
            }
            if (foundBlockIndex >= 0.0) {
                vec2 randVal = random2(vec2(foundBlockIndex, uObjectGlitchSeed));
                float xOffset = (randVal.x - 0.5);
                correctedUV.x += xOffset;
            }
        } else if (uEnableObjectGlitch == 2) {
            for (int i = 0; i < 100; i++) {
                float w = 0.02 + random2(vec2(uObjectGlitchSeed + 100.0, float(i))).x * 0.06;
                if (accumulated + w > 1.0) break;
                if (correctedUV.x >= accumulated && correctedUV.x < accumulated + w) {
                    foundBlockIndex = float(i);
                    break;
                }
                accumulated += w;
            }
            if (foundBlockIndex >= 0.0) {
                vec2 randVal = random2(vec2(foundBlockIndex, uObjectGlitchSeed + 100.0));
                float yOffset = (randVal.x - 0.5);
                correctedUV.y += yOffset;
            }
        }
    }

    if (correctedUV.x > uFillRatio.x || correctedUV.y > uFillRatio.y)
        discard;

    if (uUseMask) {
        float maskAlpha = texture(uMask, vMaskUV).a;
        if (maskAlpha > 0.5)
            discard;
    }

    vec4 texColor1 = texture(uTexture1, correctedUV);

    if (uEnableObjectWave) {
        vec2 waveUV = correctedUV;
        waveUV.y = fract(correctedUV.y + uObjectWaveOffset * 0.3);
        waveUV.x += sin((waveUV.y + uObjectWaveOffset) * uObjectWaveWidth * 3.141592) * 0.02 * uObjectWaveStrength;
        texColor1 = texture(uTexture1, waveUV);
    }

    vec4 texColor2 = texture(uTexture2, correctedUV);

    if (texColor1.a < 0.01 || texColor2.a < 0.01)
        discard;

    vec4 blended;
    if (uUseTexture) {
        float a1 = texColor1.a;
        float a2 = texColor2.a;
        float blendAlpha = a1 * (1.0 - uBlendFactor) + a2 * uBlendFactor;
        vec3 blendColor = (
            texColor1.rgb * a1 * (1.0 - uBlendFactor) +
            texColor2.rgb * a2 * uBlendFactor * 2.0
        ) / max(blendAlpha, 0.001);
        blended = vec4(blendColor, blendAlpha);
    } else {
        blended = vec4(1.0);
    }

    vec3 finalColor = blended.rgb * uColor.rgb;

    if (uUseLighting && !uIsAfterimage) {
        float dist = length(vWorldPos.xy - uLightPos.xy);
        float light = clamp(1.0 - (dist / uLightPower), 0.0, 1.0);

        vec2 offset = vec2(1.0 / 44.0, 1.0 / 32.0);
        vec2 offsets[4] = vec2[4](
            vec2(-offset.x, 0), vec2(offset.x, 0),
            vec2(0, -offset.y), vec2(0, offset.y));

        float centerAlpha = texture(uTexture1, correctedUV).a;
        float alphaDiff = 0.0;
        for (int i = 0; i < 4; ++i) {
            float neighborAlpha = texture(uTexture1, correctedUV + offsets[i]).a;
            alphaDiff += abs(centerAlpha - neighborAlpha);
        }
        alphaDiff /= 4.0;
        float edgeFactor = smoothstep(0.0, 0.1, alphaDiff);
        vec2 fromCenter = normalize(vTexCenterPos - vWorldPos);
        vec2 lightDir = normalize(vWorldPos - uLightPos);
        float dirFactor = max(dot(fromCenter, lightDir), 0.0);
        float edgeGlow = pow(edgeFactor * dirFactor, 2.0) * light;
        finalColor = mix(finalColor, uLightColor, edgeGlow);
    }

    if (uIsAfterimage) {
        if (uUseLighting) {
            float afterimageDist = length(vWorldPos.xy - uLightPos.xy);
            float afterimageLight = clamp(1.0 - (afterimageDist / uLightPower), 0.0, 1.0);
            finalColor = uColor.rgb * (1.0 - afterimageLight) + uLightColor * afterimageLight;
        } else {
            finalColor = uColor.rgb;
        }
    }

    float finalAlpha = blended.a * uColor.a;

    if (uUseGlowEffect) {
        float glowCenter = clamp(uTime, 0.0, 1.0);
        float glowWidth = 0.6;
        float dist = abs(correctedUV.x - glowCenter);
        float factor = clamp(1.0 - dist / glowWidth, 0.0, 1.0);
        float glowVisibility = pow(factor, 1.5);
        finalColor *= mix(0.5, 1.5, glowVisibility);
        finalAlpha *= glowVisibility;
    }

    if (uIsSmokeEffect) {
        vec2 screenUV = gl_FragCoord.xy / uScreenSize;
        float objectMask = texture(uObjectMask, screenUV).r;
        finalColor *= uLightColor;
        if (objectMask != 0.0) finalColor *= 0.5;
    }

    // 색상 필터 (글리치 블록 후처리)
    if (uEnableObjectGlitch > 0 && foundBlockIndex >= 0.0) {
        vec2 randVal = random2(vec2(foundBlockIndex, uObjectGlitchSeed + (uEnableObjectGlitch == 2 ? 100.0 : 0.0)));
        if (randVal.y < 0.25) {
            vec3 colorFilter = hsv2rgb(vec3(randVal.x, 0.7, 1.0));
            finalColor *= colorFilter;
        }
    }

    if (uEnableTVNoise) {
    // 가로 줄 수 설정
    const int lineCount = 40;
    for (int i = 0; i < lineCount; ++i) {
        // 줄의 세로 위치
        float lineY = random2(vec2(float(i), floor(uTime * 60.0))).x;

        // 현재 픽셀이 줄 근처인지 확인
        float dist = abs(correctedUV.y - lineY);
        float lineWidth = uTVNoiseWidth;

        if (dist < lineWidth) {
            // 길이 및 밝기 결정
            float len = 0.01 + random2(vec2(float(i), 17.17)).x * 0.03;
            float seed = float(i) + floor(uTime * 60.0); // 프레임마다 바뀜
            float startX = random2(vec2(seed, 13.31)).x * (1.0 - len);

            if (correctedUV.x > startX && correctedUV.x < startX + len) {
                // 색상: 밝은 회색 ~ 흰색
                float brightness = 0.1 + random2(vec2(float(i), 3.14)).x * 0.6;
                vec3 noiseColor = vec3(brightness);

                // 줄이 많으면 살짝 섞이게 가중치 줌
                finalColor = noiseColor;
                finalAlpha = 1.0; // ✅ 알파도 강제로 불투명하게
            }
        }
    }
    }

    FragColor = vec4(finalColor, finalAlpha);
}