// 쉐이더에서 include할 내용은 .hlsli 파일에 작성
// Properties -> Item type: Does not participate in build으로 설정

/* 참고 : C++ SimpleMath -> hlsl */
// Matrix -> matrix 또는 float4x4
// Vector3 -> float3
// float3 a = normalize(b);
// float a = dot(v1, v2);
// Saturate -> saturate() 사용
// float l = length(v);
// struct A{ float a = 1.0f; }; <- 구조체에서 초기화 불가
// Vector3(0.0f) -> float3(0.0, 0.0, 0.0) // 실수 뒤에 f 불필요
// Vector4::Transform(v, M) -> mul(v, M)

#define MAX_LIGHTS 3 // 쉐이더에서도 #define 사용 가능
#define NUM_DIR_LIGHTS 1
#define NUM_POINT_LIGHTS 1 
#define NUM_SPOT_LIGHTS 1

// 재질
struct Material
{
    float3 ambient;
    float shininess;
    float3 diffuse;
    float dummy1;   // 16bytes 맞춰주기 위해 추가
    float3 specular;
    float dummy2;
};

// 조명
struct Light
{
    float3 strength;
    float fallOffStart;
    float3 direction;
    float fallOffEnd;
    float3 position;
    float spotPower;
};

float3 BlinnPhong(float3 lightStrength, float3 lightVec, float3 normal,
                  float3 toEye, Material mat)
{
    float3 halfway = normalize(toEye + lightVec);
    float hdotn = dot(halfway, normal);
    float3 specular = mat.specular * pow(max(hdotn, 0.0), mat.shininess);
    return mat.ambient + (mat.diffuse + specular) * lightStrength;
}

float3 ComputeDirectionalLight(Light L, Material mat, float3 normal,
                               float3 toEye)
{
    float3 lightVec = -L.direction;
    float ndotl = max(dot(normal, lightVec), 0.0);
    float3 lightStrength = L.strength * ndotl;
    return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}

float CalcAttenuation(float d, float falloffStart, float falloffEnd)
{
    //Linear falloff
    return saturate((falloffEnd - d) / (falloffEnd - falloffStart));
}

float3 ComputePointLight(Light L, Material mat, float3 pos, float3 normal,
                         float3 toEye)
{
    float3 lightVec = L.position - pos;
    
    // 쉐이딩할 지점부터 조명까지의 거리 계산
    float d = length(lightVec);
    
    if(d > L.fallOffEnd)
    {
        return float3(0.0, 0.0, 0.0);
    }
    else
    {
        lightVec /= d; // normalize
        
        float ndotl = max(dot(normal, lightVec), 0.0);
        float3 lightStrength = L.strength * ndotl;
    
        float att = CalcAttenuation(d, L.fallOffStart, L.fallOffEnd);
        lightStrength *= att;
        
        return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
    }
}

float3 ComputeSpotLight(Light L, Material mat, float3 pos, float3 normal,
                        float3 toEye)
{
    float3 lightVec = L.position - pos;
    
    // 쉐이딩할 지점부터 조명까지의 거리 계산
    float d = length(lightVec);
    
    if (d > L.fallOffEnd)
    {
        return float3(0.0, 0.0, 0.0);
    }
    else
    {
        lightVec /= d; // normalize
        
        float ndotl = max(dot(normal, lightVec), 0.0);
        float3 lightStrength = L.strength * ndotl;
        
        float att = CalcAttenuation(d, L.fallOffStart, L.fallOffEnd);
        lightStrength *= att;
        
        float spotFactor = pow(max(-dot(lightVec, L.direction), 0.0), L.spotPower);
        lightStrength *= spotFactor;
        
        return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
    }
}

struct VertexShaderInput
{
    float3 posModel : POSITION;     // 모델 좌표계의 위치 postion
    float3 normalModel : NORMAL;    // 모델 좌표계의 normal
    float2 texcoord : TEXCOORD0;
};

struct PixelShaderInput
{
    float4 posProj : SV_Position;   // Screen position
    float3 posWorld : POSITION;     // World position (조명 계산에 사용)
    float3 normalWorld : NORMAL;
    float2 texcoord : TEXCOORD;
    float3 color : COLOR;           // Normal lines 쉐이더에서 사용
};