#pragma once
class Object;
class ObjectComponent {
public:
    Object* owner = nullptr;
public:
	virtual void Initialize() {};
	virtual void Update(double dt) {};
    virtual ~ObjectComponent() {}
};

class TextureData;
struct RenderInfo
{
    glm::vec4 color = glm::vec4(1.0f);           
    glm::vec3 lightColor;                        
    float lightPower;                            

    glm::vec2 texelPos = { 0.0, 0.0 };           
    glm::vec2 frameSize = { 0.0, 0.0 };          
    glm::vec2 renderRatio = glm::vec2(1.0);  
    glm::vec2 worldRenderLowerBound = glm::vec2(0., 720.);
    glm::vec2 worldRenderUpperBound = glm::vec2(1280., 0.);
    glm::vec2 lightPos;                          

    TextureData* texture2 = nullptr;             
    TextureData* mask = nullptr;
    glm::mat4 maskMat;
    float timer;                       
    float blendFactor;                           
    float uObjectWaveStrength;
    float uObjectWaveWidth;    
    float uObjectWaveOffset;
    float uObjectGlitchSeed;
    float uTVNoiseWidth;
    int uEnableObjectGlitch = 0;

    bool isFixed = false;                        
    bool useGlowEffect = false;                  
    bool useTexture = true;                      
    bool isSmoke = false;                        
    bool useLight = false;  
    bool useBounding = false;
    bool useMask = false;
    bool uEnableObjectWave = false;
    bool uEnableTVNoise = false;
};

