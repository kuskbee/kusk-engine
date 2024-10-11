// ���̴����� include�� ������ .hlsli ���Ͽ� �ۼ�
// Properties -> Item type: Does not participate in build���� ����

/* ���� : C++ SimpleMath -> hlsl */
// Matrix -> matrix �Ǵ� float4x4
// Vector3 -> float3
// float3 a = normalize(b);
// float a = dot(v1, v2);
// Saturate -> saturate() ���
// float l = length(v);
// struct A{ float a = 1.0f; }; <- ����ü���� �ʱ�ȭ �Ұ�
// Vector3(0.0f) -> float3(0.0, 0.0, 0.0) // �Ǽ� �ڿ� f ���ʿ�
// Vector4::Transform(v, M) -> mul(v, M)

#define MAX_LIGHTS 3 // ���̴������� #define ��� ����
#define NUM_DIR_LIGHTS 1
#define NUM_POINT_LIGHTS 1 
#define NUM_SPOT_LIGHTS 1

// ����
struct Material
{
    float3 ambient;
    float shininess;
    float3 diffuse;
    float dummy1;   // 16bytes �����ֱ� ���� �߰�
    float3 specular;
    float dummy2;
};

// ����
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
    
    // ���̵��� �������� ��������� �Ÿ� ���
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
    
    // ���̵��� �������� ��������� �Ÿ� ���
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
    float3 posModel : POSITION;     // �� ��ǥ���� ��ġ postion
    float3 normalModel : NORMAL;    // �� ��ǥ���� normal
    float2 texcoord : TEXCOORD0;
};

struct PixelShaderInput
{
    float4 posProj : SV_Position;   // Screen position
    float3 posWorld : POSITION;     // World position (���� ��꿡 ���)
    float3 normalWorld : NORMAL;
    float2 texcoord : TEXCOORD;
    float3 color : COLOR;           // Normal lines ���̴����� ���
};