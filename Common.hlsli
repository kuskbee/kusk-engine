#ifndef __COMMON_HLSLI__
#define __COMMON_HLSLI__

// 쉐이더에서 include할 내용들을 .hlsli 파일에 작성
// Properties -> Item Type: Does not participate in build으로 설정

#define MAX_LIGHTS 3 // 쉐이더에서도 #define 사용 가능
#define NUM_DIR_LIGHTS 1
#define NUM_POINT_LIGHTS 1 
#define NUM_SPOT_LIGHTS 1

// 샘플러들을 모든 쉐이더에서 공통으로 사용
SamplerState linearWrapSampler : register(s0);
SamplerState linearClampSampler : register(s1);

// 공용 텍스쳐들 t10 부터 시작
TextureCube envIBLTex : register(t10);
TextureCube specularIBLTex : register(t11);
TextureCube irradianceIBLTex : register(t12);
Texture2D brdfTex : register(t13);

// 조명
struct Light
{
    float3 radiance; // Strength
    float fallOffStart;
    float3 direction;
    float fallOffEnd;
    float3 position;
    float spotPower;
};

// 공용 Constants
cbuffer GlobalConstants : register(b1)
{
    matrix view;
    matrix proj;
    matrix viewProj;
    float3 eyeWorld;
    float strengthIBL;
    
    int textureToDraw = 0; // 0 : Env, 1 : Specular, 2 : Irradiance, 그외 : 검은색
    float envLodBias = 0.0f; // 환경맵 LodBias
    float lodBias = 2.0f; // 다른 물체들 LoadBias
    // 거울 관련 데이터
    bool isMirror;
    float4 mirrorPlane;
    
    Light light[MAX_LIGHTS];
};

struct VertexShaderInput
{
    float3 posModel : POSITION;     // 모델 좌표계의 위치 postion
    float3 normalModel : NORMAL0;    // 모델 좌표계의 normal
    float2 texcoord : TEXCOORD0;
    float3 tangentModel : TANGENT0;
};

struct PixelShaderInput
{
    float4 posProj : SV_Position;   // Screen position
    float3 posModel : POSITION0;     // Model position
    float3 posWorld : POSITION1;     // World position (조명 계산에 사용)
    float3 normalWorld : NORMAL0;
    float2 texcoord : TEXCOORD0;
    float3 tangentWorld : TANGENT0;
};

#endif // __COMMON_HLSLI__