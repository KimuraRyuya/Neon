#ifndef WI_SHADER_GLOBALS_HF
#define WI_SHADER_GLOBALS_HF
#include "../Library/GPUParticle/ShaderInterop.h"
#include "../Library/GPUParticle/ShaderInTeropRenderer.h"

static const float PI = 3.14159265358979323846;
static const float SQRT2 = 1.41421356237309504880;

inline float rand(inout float seed, in float2 uv)
{
    float result = frac(sin(seed * dot(uv, float2(12.9898f, 78.233f))) * 43758.5453f);
    seed += 1.0f;
    return result;
}

#endif // WI_SHADER_GLOBALS_HF