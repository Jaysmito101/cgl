/*
MIT License
Copyright (c) 2023 Jaysmito Mukherjee (jaysmito101@gmail.com)
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#define CGL_LOGGING_ENABLED
#define CGL_EXCLUDE_NETWORKING
#define CGL_EXCLUDE_SKY_RENDERER
#define CGL_EXCLUDE_PHONG_RENDERER
#define CGL_EXCLUDE_TILEMAP_RENDERER
#define CGL_IMPLEMENTATION
#include "cgl.h"


// set this to 0 to disable the ui and remove nuklear dependency
#define GUI_DEBUG 1

#if GUI_DEBUG
#pragma warning(push, 0)
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT
#include "nuklear.h"
#include "nuklear_glfw_gl4.h"
#pragma warning(pop)
#endif


#define SIZEX          1366
#define SIZEY          768

static const char* DIFFUSE_VERTEX_SHADER_SOURCE = 
  "#version 430 core\n"
  "\n"
  "layout (location = 0) in vec4 position;\n"
  "layout (location = 1) in vec4 normal;\n"
  "layout (location = 2) in vec4 texcoord;\n"
  "\n"
  "out vec2 v_texcoord;\n"
  "out vec3 v_normal;\n"
  "out vec3 v_position;\n"
  "\n"
  "uniform mat4 u_model;\n"
  "uniform mat4 u_view;\n"
  "uniform mat4 u_projection;\n"
  "uniform mat4 u_viewProjection;\n"
  "\n"
  "void main() {\n"
  "  v_texcoord = texcoord.xy;\n"
  "  v_normal = (u_model * normal).xyz;\n"
  "  v_position = (u_model * position).xyz;\n"
  "  gl_Position =  u_viewProjection * u_model * position;\n"
  "}\n";

static const char* DIFFUSE_FRAGMENT_SHADER_SOURCE = 
  "#version 430 core\n"
  "\n"
  "in vec2 v_texcoord;\n"
  "in vec3 v_normal;\n"
  "in vec3 v_position;\n"
  "\n"
  "out vec4 frag_color;\n"
  "\n"
  "uniform vec3 u_lightPos;\n"
  "uniform vec3 u_lightColor;\n"
  "uniform vec3 u_objectColor;\n"
  "\n"
  "vec3 aces(vec3 x) {\n"
  "  const float a = 2.51;\n"
  "  const float b = 0.03;\n"
  "  const float c = 2.43;\n"
  "  const float d = 0.59;\n"
  "  const float e = 0.14;\n"
  "  return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);\n"
  "}\n"
  "\n"
  "float exponentialToLinear(float depth)\n"
  "{\n"
  "    float z = depth * 2.0 - 1.0;\n"
  "    return (2.0 * 0.1f * 100.0f) / (100.0f + 0.1f - z * (100.0f - 0.1f));\n"
  "}\n"
  "\n"
  "void main() {\n"
  " vec3 N = normalize(v_normal);\n"
  " vec3 L = normalize(u_lightPos - v_position);\n"
  " float NdotL = max(dot(N, L), 0.0);\n"
  " vec3 diffuse = u_lightColor * NdotL;\n"
  " vec3 ambient = vec3(0.1f, 0.1f, 0.1f);\n"
  " vec3 col = (ambient + diffuse) * u_objectColor;\n"
  //" col = aces(col);\n"
  //" col = pow(col, vec3(1.0f / 2.2f));\n"
  " frag_color = vec4(col, 1.0);\n"
  "}\n";

static const char* PRESENT_VERTEX_SHADER_SOURCE = 
  "#version 430 core\n"
  "\n"
  "layout (location = 0) in vec4 position;\n"
  "layout (location = 1) in vec4 normal;\n"
  "layout (location = 2) in vec4 texcoord;\n"
  "\n"
  "out vec2 v_texcoord;\n"
  "\n"
  "void main() {\n"
  "  v_texcoord = vec2(1.0 - texcoord.y, 1.0 - texcoord.x);\n"
  "  gl_Position = position;\n"
  "}\n";

static const char* PRESENT_FRAGMENT_SHADER_SOURCE =
  "#version 430 core\n"
  "\n"
  "in vec2 v_texcoord;\n"
  "\n"
  "out vec4 fragColor;\n"
  "\n"
  "uniform sampler2D u_texture;\n"
  "uniform sampler3D u_noiseTexture3d128;\n"
  "uniform sampler3D u_noiseTexture3d32;\n"
  "uniform sampler2D u_noiseTexture2d128;\n"
  "uniform float u_time;\n"
  "uniform float u_aspectRatio;\n"
  "uniform int u_vizMode;\n"
  "uniform int u_channel;\n"
  "uniform float u_slice;\n"
  "uniform float u_scale = 1.0f;\n"
  "uniform vec3 u_offset = vec3(0.0f, 0.0f, 0.0f);\n"
  "\n"
  "void main() {\n"
  " // if vizmode != 0 adjust the texcoord according to aspect ratio\n"
  " vec2 texcoord = v_texcoord;\n"
  " if (u_vizMode != 0) {\n"
  "   texcoord.x *= u_aspectRatio;\n"
  " }\n"
  "\n"
  " if (texcoord.x < 0.0f || texcoord.x > 1.0f || texcoord.y < 0.0f || texcoord.y > 1.0f) {\n"
  "   fragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);\n"
  "   return;\n"
  " }\n"
  "\n"
  " if (u_vizMode == 0) {\n"
  "   fragColor = texture(u_texture, texcoord);\n"
  " } else if (u_vizMode == 1) {\n"
  "   vec4 col = texture(u_noiseTexture3d128, u_scale * ( u_offset + vec3(texcoord, u_slice)));\n"
  "   fragColor = vec4(vec3(col[u_channel]), 1.0f);\n"
  " } else if (u_vizMode == 2) {\n"
  "   vec4 col = texture(u_noiseTexture3d32, u_scale * ( u_offset + vec3(texcoord, u_slice)));\n"
  "   fragColor = vec4(vec3(col[u_channel]), 1.0f);\n"
  " } else if (u_vizMode == 3) {\n"
  "   vec4 col = texture(u_noiseTexture2d128, u_scale * ( u_offset.xy + vec2(texcoord)));\n"
  "   fragColor = vec4(vec3(col[u_channel]), 1.0f);\n"
  " } else {\n"
  "   fragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n"
  " }\n"
  "}\n";

static const char* CLOUDS_VERTEX_SHADER_SOURCE = 
  "#version 430 core\n"
  "\n"
  "layout (location = 0) in vec4 position;\n"
  "layout (location = 1) in vec4 normal;\n"
  "layout (location = 2) in vec4 texcoord;\n"
  "\n"
  "out vec2 v_texcoord;\n"
  "out vec3 v_normal;\n"
  "out vec3 v_position;\n"
  "out vec3 v_viewVector;\n"
  "\n"
  "uniform mat4 u_projection;\n"
  "uniform mat4 u_view;\n"
  "\n"
  "void main() {\n"
  "  vec2 uv = vec2(1.0 - texcoord.y, 1.0 - texcoord.x);\n"
  "  v_texcoord = uv;\n"
  "  vec3 viewVector = (inverse(u_projection) * vec4(uv * 2.0 - 1.0, 1.0, 1.0)).xyz;\n"
  "  v_viewVector = (inverse(u_view) * vec4(viewVector, 0.0)).xyz;\n"
  "  gl_Position = vec4(position.xy, 0.0, 1.0);\n"
  "}\n";

static const char* CLOUDS_FRAGMENT_SHADER_SOURCE = 
  "#version 430 core\n"
  "\n"
  "out vec4 fragColor;\n"
  "\n"
  "in vec2 v_texcoord;\n"
  "in vec3 v_viewVector;\n"
  "\n"
  "uniform sampler2D u_texture;\n"
  "uniform sampler2D u_depth;\n"
  "uniform sampler3D u_shapeNoise;\n"
  "uniform sampler3D u_detailNoise;\n"
  "uniform sampler2D u_curlNoise;\n"
  "\n"
  "uniform int u_numSteps;\n"
  "uniform int u_numLightSteps;\n"
  "\n"
  "uniform float u_lightAbsorptionThroughCloud;\n"
  "uniform float u_lightAbsorptionTowardsSun;\n"
  "uniform float u_densityFactor;\n"
  "uniform float u_scale;\n"
  "uniform float u_threshold;\n"
  "uniform float u_time;\n"
  "uniform float u_far;\n"
  "uniform float u_near;\n"
  "uniform float u_heightFactor;\n"
  "uniform float u_detailScale;\n"
  "uniform float u_animationSpeed;\n"
  "\n"
  "uniform vec3 u_offset;\n"
  "uniform vec3 u_detailOffset;\n"
  "uniform vec3 u_cameraPos;\n"
  "uniform vec3 u_lightPos;\n"
  "uniform vec3 u_lightColor;\n"
  "uniform vec3 u_cloudBoundsMin;\n"
  "uniform vec3 u_cloudBoundsMax;\n"
  "\n"
  "uniform vec4 u_shapeNoiseWeights;\n"
  "uniform vec4 u_detailNoiseWeights;\n"
  "uniform vec4 u_phaseParams;\n"
  "\n"
  "#define saturate(x) clamp(x, 0.0, 1.0)\n"
  "\n"
  "float remap01(float v, float low, float high) \n"
  "{\n"
  "    return (v-low)/(high-low);\n"
  "}\n"
  "\n"
  "vec2 rayBoxDst(vec3 boundsMin, vec3 boundsMax, vec3 rayOrigin, vec3 rayDir) \n"
  "{\n"
  "    vec3 t0 = (boundsMin - rayOrigin) / rayDir;\n"
  "    vec3 t1 = (boundsMax - rayOrigin) / rayDir;\n"
  "    vec3 tmin = min(t0, t1);\n"
  "    vec3 tmax = max(t0, t1);\n"
  "    \n"
  "    float dstA = max(max(tmin.x, tmin.y), tmin.z);\n"
  "    float dstB = min(tmax.x, min(tmax.y, tmax.z));\n"
  "\n"
  "    // CASE 1: ray intersects box from outside (0 <= dstA <= dstB)\n"
  "    // dstA is dst to nearest intersection, dstB dst to far intersection\n"
  "\n"
  "    // CASE 2: ray intersects box from inside (dstA < 0 < dstB)\n"
  "    // dstA is the dst to intersection behind the ray, dstB is dst to forward intersection\n"
  "\n"
  "    // CASE 3: ray misses box (dstA > dstB)\n"
  "\n"
  "    float dstToBox = max(0.0, dstA);\n"
  "    float dstInsideBox = max(0.0, dstB - dstToBox);\n"
  "    return vec2(dstToBox, dstInsideBox);\n"
  "}\n"
  "\n"
  "float exponentialToLinear(float depth)\n"
  "{\n"
  "    float z = depth * 2.0 - 1.0;\n"
  "    return (2.0 * u_near * u_far) / (u_far + u_near - z * (u_far - u_near));\n"
  "}\n"
  "\n"
  "\n"
  "float sampleDensity(vec3 pos)\n"
  "{\n"
  "    vec3 shapeAnimOffset = vec3(u_time, 0.0, 0.0) * u_animationSpeed;\n"
  "    vec3 detailAnimOffset = vec3(u_time, 0.0, 0.0) * u_animationSpeed * 1.3;\n"
  "\n"
  "    vec3 uvw = (pos + u_offset + shapeAnimOffset) * u_scale;\n"
  "\n"
  "    float heightPercent = (pos.y - u_cloudBoundsMin.y) / (u_cloudBoundsMax.y - u_cloudBoundsMin.y);\n"
  "    const float gMin = 0.4, gMax = 0.6;\n"
  "    float heightGrad = saturate(remap01(heightPercent, 0.0, gMin)) * saturate(remap01(heightPercent, 1.0, gMax));\n"
  "    heightGrad = pow(heightGrad, u_heightFactor);\n"
  "\n"
  "    vec4 normalizedNoiseWeights = u_shapeNoiseWeights / dot(u_shapeNoiseWeights, vec4(1.0));\n"
  "    float density = dot(texture(u_shapeNoise, uvw * 0.03), normalizedNoiseWeights) * heightGrad;\n"
  "    density = density - u_threshold;\n"
  "    density = max(0.0, density);\n"
  "\n"
  "    if (density > 0.0f)\n"
  "    {\n"
  "        vec3 detailuvw = (pos + u_detailOffset + detailAnimOffset) * u_detailScale;\n"
  "        vec4 normalizedDetailWeights = u_detailNoiseWeights / dot(u_detailNoiseWeights, vec4(1.0));\n"
  "        float detail = dot(texture(u_detailNoise, detailuvw * 0.03), normalizedDetailWeights);\n"
  "\n"
  "        float oneMinusDetail = 1.0 - detail;\n"
  "        density = density * oneMinusDetail * oneMinusDetail * oneMinusDetail;\n"
  "    }\n"
  "\n"
  "    return density * u_densityFactor;\n"
  "}\n"
  "\n"
  "\n"
  "// Henyey-Greenstein phase function\n"
  "float hg(float a, float g)\n"
  "{\n"
  "    float g2 = g * g;\n"
  "    float denom = pow(1.0 + g2 - 2.0 * g * a, 1.5) * 4.0 * 3.14159;\n"
  "    return (1.0 - g2) / denom;\n"
  "}\n"
  "\n"
  "\n"
  "float phase(in vec3 rayDir, in vec3 pos)\n"
  "{\n"
  "    vec3 lightDir = normalize(u_lightPos - pos);\n"
  "    float cosAngle = dot(rayDir, lightDir);\n"
  "\n"
  "    float blend = 0.5f;\n"
  "\n"
  "    float hgBlend = hg(cosAngle, u_phaseParams.x) * (1.0 - blend) + hg(cosAngle, -u_phaseParams.y) * blend;\n"
  "    return u_phaseParams.z + hgBlend * u_phaseParams.w;\n"
  "}\n"
  "\n"
  "float lightmarch(vec3 pos)\n"
  "{\n"
  "    vec3 dirToLight = normalize(u_lightPos - pos);\n"
  "\n"
  "    vec2 dst = rayBoxDst(u_cloudBoundsMin, u_cloudBoundsMax, pos, dirToLight);\n"
  "    float dstToBox = dst.x;\n"
  "    float dstInsideBox = dst.y;\n"
  "\n"
  "    float dstStep = dstInsideBox / float(u_numLightSteps);\n"
  "    float totalDensity = 0.0;\n"
  "\n"
  "    for (int i = 0; i < u_numLightSteps; i++)\n"
  "    {\n"
  "        vec3 rayPos = pos + dirToLight * (dstToBox + dstStep * float(i));\n"
  "        totalDensity += sampleDensity(rayPos) * dstStep; \n"
  "    }\n"
  "\n"
  "    float transmittance = exp(-totalDensity * u_lightAbsorptionTowardsSun);\n"
  "    return transmittance;\n"
  "}\n"
  "\n"
  "\n"
  "void main()\n"
  "{\n"
  "    vec3 color = texture(u_texture, v_texcoord).rgb;\n"
  "    float depth = exponentialToLinear(texture(u_depth, v_texcoord).r);\n"
  "\n"
  "    vec3 rayOrigin = u_cameraPos;\n"
  "    vec3 rayDir = normalize(v_viewVector);\n"
  "\n"
  "    vec2 dst = rayBoxDst(u_cloudBoundsMin, u_cloudBoundsMax, rayOrigin, rayDir);\n"
  "    float dstToBox = dst.x;\n"
  "    float dstInsideBox = dst.y;\n"
  "\n"
  "    float dstTraveled = 0.0;\n"
  "    float dstStep = dstInsideBox / float(u_numSteps);\n"
  "    float dstLimit = min(depth - dstToBox, dstInsideBox);\n"
  "\n"
  "    vec3 rayPos = vec3(0.0);\n"
  "    float transmittance = 1.0f;\n"
  "    vec3 lightEnergy = vec3(0.0);\n"
  "    float phaseValue = 0.0;\n"
  "\n"
  "    while (dstTraveled < dstLimit)\n"
  "    {\n"
  "        rayPos = rayOrigin + rayDir * (dstToBox + dstTraveled);\n"
  "        phaseValue = phase(rayDir, rayPos);\n"
  "\n"
  "        float density = sampleDensity(rayPos);\n"
  "\n"
  "        if (density > 0.0) \n"
  "        {\n"
  "            float lightTransmittance = lightmarch(rayPos);\n"
  "\n"
  "            lightEnergy += density * dstStep * transmittance * lightTransmittance * phaseValue;\n"
  "            transmittance *= exp(-density * dstStep * u_lightAbsorptionThroughCloud);\n"
  "\n"
  "            if (transmittance < 0.01) \n"
  "            {\n"
  "                break;\n"
  "            }\n"
  "        }\n"
  "\n"
  "        dstTraveled += dstStep;\n"
  "    }\n"
  "\n"
  "    color = color * transmittance + lightEnergy * u_lightColor;\n"
  "\n"
  "    fragColor = vec4(color, 1.0);\n"
  "}\n";


static const char* CLOUDS_COMPUTE_SHADER_SOURCE = 
  "#version 430 core\n"
  "\n"
  "layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;\n"
  "\n"
  "layout(rgba32f, binding = 0) uniform image3D u_3d128;\n"
  "layout(rgba32f, binding = 1) uniform image3D u_3d32;\n"
  "layout(rgba32f, binding = 2) uniform image2D u_2d128;\n"
  "\n"
  "uniform int u_mode;\n"
  "uniform int u_width;\n"
  "uniform int u_height;\n"
  "uniform int u_depth;\n"
  "uniform float u_startingScale;\n"
  "\n"
  "#define MOD3 vec3(.1031,.11369,.13787)\n"
  "\n"
  "vec3 hash33(vec3 p3)\n"
  "{\n"
  "	p3 = fract(p3 * MOD3);\n"
  "    p3 += dot(p3, p3.yxz+19.19);\n"
  "    return -1.0 + 2.0 * fract(vec3((p3.x + p3.y)*p3.z, (p3.x+p3.z)*p3.y, (p3.y+p3.z)*p3.x));\n"
  "}\n"
  "\n"
  "ivec3 wrapNeghbour(ivec3 v, int numCells, inout vec3 boxOffset)\n"
  "{\n"
  "	for (int i = 0 ; i < 3 ; i++)\n"
  "	{\n"
  "		if (v[i] < 0)\n"
  "		{\n"
  "			v[i] += numCells;\n"
  "			boxOffset[i] = -1.0;\n"
  "		}\n"
  "		else if (v[i] >= numCells)\n"
  "		{\n"
  "			v[i] -= numCells;\n"
  "			boxOffset[i] = 1.0;\n"
  "		}\n"
  "		else \n"
  "		{\n"
  "			boxOffset[i] = 0.0;\n"
  "		}\n"
  "	}\n"
  "\n"
  "	return ivec3(mod(v.x, numCells), mod(v.y, numCells), mod(v.z, numCells));\n"
  "}\n"
  "\n"
  "// From: https://www.shadertoy.com/view/3d3fWN\n"
  "float worley(vec3 p, float scale){\n"
  "\n"
  "    vec3 id = floor(p*scale);\n"
  "    vec3 fd = fract(p*scale);\n"
  "\n"
  "    float n = 0.;\n"
  "\n"
  "    float minimalDist = 1.;\n"
  "\n"
  "    for(float x = -1.; x <=1.; x++){\n"
  "        for(float y = -1.; y <=1.; y++){\n"
  "            for(float z = -1.; z <=1.; z++){\n"
  "\n"
  "                vec3 coord = vec3(x,y,z);\n"
  "                vec3 rId = hash33(mod(id+coord,scale))*0.5+0.5;\n"
  "\n"
  "                vec3 r = coord + rId - fd; \n"
  "\n"
  "                float d = dot(r,r);\n"
  "\n"
  "                if(d < minimalDist){\n"
  "                    minimalDist = d;\n"
  "                }\n"
  "\n"
  "            }//z\n"
  "        }//y\n"
  "    }//x\n"
  "    \n"
  "    return sqrt(minimalDist);\n"
  "}\n"
  "\n"
  "\n"
  "float perlin_noise(vec3 p)\n"
  "{\n"
  "    vec3 pi = floor(p);\n"
  "    vec3 pf = p - pi;\n"
  "    \n"
  "    vec3 w = pf * pf * (3.0 - 2.0 * pf);\n"
  "    \n"
  "    return 	mix(\n"
  "        		mix(\n"
  "                	mix(dot(pf - vec3(0, 0, 0), hash33(pi + vec3(0, 0, 0))), \n"
  "                        dot(pf - vec3(1, 0, 0), hash33(pi + vec3(1, 0, 0))),\n"
  "                       	w.x),\n"
  "                	mix(dot(pf - vec3(0, 0, 1), hash33(pi + vec3(0, 0, 1))), \n"
  "                        dot(pf - vec3(1, 0, 1), hash33(pi + vec3(1, 0, 1))),\n"
  "                       	w.x),\n"
  "                	w.z),\n"
  "        		mix(\n"
  "                    mix(dot(pf - vec3(0, 1, 0), hash33(pi + vec3(0, 1, 0))), \n"
  "                        dot(pf - vec3(1, 1, 0), hash33(pi + vec3(1, 1, 0))),\n"
  "                       	w.x),\n"
  "                   	mix(dot(pf - vec3(0, 1, 1), hash33(pi + vec3(0, 1, 1))), \n"
  "                        dot(pf - vec3(1, 1, 1), hash33(pi + vec3(1, 1, 1))),\n"
  "                       	w.x),\n"
  "                	w.z),\n"
  "    			w.y);\n"
  "}\n"
  "\n"
  "vec4 worley3CH(vec3 uv)\n"
  "{\n"
  "    float worleyNoise0 = 1.0 - worley(uv * float(u_startingScale), 4);\n"
  "    float worleyNoise1 = 1.0 - worley(uv * float(u_startingScale), 8);\n"
  "    float worleyNoise2 = 1.0 - worley(uv * float(u_startingScale), 16);\n"
  "    float worleyNoise3 = 1.0 - worley(uv * float(u_startingScale), 32);\n"
  "\n"
  "    float worleyFBM0 = worleyNoise0*0.625f + worleyNoise1*0.25f + worleyNoise2*0.125f;\n"
  "	float worleyFBM1 = worleyNoise1*0.625f + worleyNoise2*0.25f + worleyNoise3*0.125f;\n"
  "	float worleyFBM2 = worleyNoise2*0.75f + worleyNoise3*0.25f; \n"
  "\n"
  "\n"
  "    vec4 color = vec4(\n"
  "        worleyFBM0,\n"
  "        worleyFBM1,\n"
  "        worleyFBM2,\n"
  "        worleyNoise0\n"
  "    );\n"
  "\n"
  "    return color;\n"
  "}\n"
  "\n"
  "\n"
  "//----------------------------------------------------------------------------------------\n"
  "vec3 HashALU(in vec3 p, in float numCells)\n"
  "{\n"
  "	// This is tiling part, adjusts with the scale\n"
  "	p = mod(p, numCells);\n"
  "	\n"
  "    p = vec3( dot(p,vec3(127.1,311.7, 74.7)),\n"
  "			  dot(p,vec3(269.5,183.3,246.1)),\n"
  "			  dot(p,vec3(113.5,271.9,124.6)));\n"
  "\n"
  "	return -1.0 + fract(sin(p)*43758.5453123) * 2.0;\n"
  "}\n"
  "\n"
  "#define Hash HashALU\n"
  "\n"
  "//----------------------------------------------------------------------------------------\n"
  "float TileableNoise(in vec3 p, in float numCells )\n"
  "{\n"
  "	vec3 f, i;\n"
  "	\n"
  "	p *= numCells;\n"
  "\n"
  "	\n"
  "	f = fract(p);		// Separate integer from fractional\n"
  "    i = floor(p);\n"
  "	\n"
  "    vec3 u = f*f*(3.0-2.0*f); // Cosine interpolation approximation\n"
  "\n"
  "    return mix( mix( mix( dot( Hash( i + vec3(0.0,0.0,0.0), numCells ), f - vec3(0.0,0.0,0.0) ), \n"
  "                          dot( Hash( i + vec3(1.0,0.0,0.0), numCells ), f - vec3(1.0,0.0,0.0) ), u.x),\n"
  "                     mix( dot( Hash( i + vec3(0.0,1.0,0.0), numCells ), f - vec3(0.0,1.0,0.0) ), \n"
  "                          dot( Hash( i + vec3(1.0,1.0,0.0), numCells ), f - vec3(1.0,1.0,0.0) ), u.x), u.y),\n"
  "                mix( mix( dot( Hash( i + vec3(0.0,0.0,1.0), numCells ), f - vec3(0.0,0.0,1.0) ), \n"
  "                          dot( Hash( i + vec3(1.0,0.0,1.0), numCells ), f - vec3(1.0,0.0,1.0) ), u.x),\n"
  "                     mix( dot( Hash( i + vec3(0.0,1.0,1.0), numCells ), f - vec3(0.0,1.0,1.0) ), \n"
  "                          dot( Hash( i + vec3(1.0,1.0,1.0), numCells ), f - vec3(1.0,1.0,1.0) ), u.x), u.y), u.z );\n"
  "}\n"
  "\n"
  "float TileableNoiseFBM(in vec3 p, float numCells, int octaves)\n"
  "{\n"
  "	float f = 0.0;\n"
  "    \n"
  "	// Change starting scale to any integer value...\n"
  "    p = mod(p, vec3(numCells));\n"
  "	float amp = 0.5;\n"
  "    float sum = 0.0;\n"
  "	\n"
  "	for (int i = 0; i < octaves; i++)\n"
  "	{\n"
  "		f += TileableNoise(p, numCells) * amp;\n"
  "        sum += amp;\n"
  "		amp *= 0.5;\n"
  "\n"
  "		// numCells must be multiplied by an integer value...\n"
  "		numCells *= 2.0;\n"
  "	}\n"
  "\n"
  "	return f / sum;\n"
  "}\n"
  "\n"
  "vec3 snoiseVec3( vec3 x )\n"
  "{\n"
  "  float numCells = 6.0;\n"
  "  int octaves = 3;\n"
  "   \n"
  "  float s  = TileableNoiseFBM(vec3( x ), numCells, octaves);\n"
  "  float s1 = TileableNoiseFBM(vec3( x.y - 19.1 , x.z + 33.4 , x.x + 47.2 ), numCells, octaves);\n"
  "  float s2 = TileableNoiseFBM(vec3( x.z + 74.2 , x.x - 124.5 , x.y + 99.4 ), numCells, octaves);\n"
  "  vec3 c = vec3( s , s1 , s2 );\n"
  "  return c;\n"
  "\n"
  "}\n"
  "\n"
  "vec3 curlNoise(vec3 p)\n"
  "{\n"
  "  const float e = .1;\n"
  "  vec3 dx = vec3( e   , 0.0 , 0.0 );\n"
  "  vec3 dy = vec3( 0.0 , e   , 0.0 );\n"
  "  vec3 dz = vec3( 0.0 , 0.0 , e   );\n"
  "\n"
  "  vec3 p_x0 = snoiseVec3( p - dx );\n"
  "  vec3 p_x1 = snoiseVec3( p + dx );\n"
  "  vec3 p_y0 = snoiseVec3( p - dy );\n"
  "  vec3 p_y1 = snoiseVec3( p + dy );\n"
  "  vec3 p_z0 = snoiseVec3( p - dz );\n"
  "  vec3 p_z1 = snoiseVec3( p + dz );\n"
  "\n"
  "  float x = p_y1.z - p_y0.z - p_z1.y + p_z0.y;\n"
  "  float y = p_z1.x - p_z0.x - p_x1.z + p_x0.z;\n"
  "  float z = p_x1.y - p_x0.y - p_y1.x + p_y0.x;\n"
  "\n"
  "  const float divisor = 1.0 / ( 2.0 * e );\n"
  "  return normalize( vec3( x , y , z ) * divisor );\n"
  "}\n"
  "\n"
  "\n"
  "void gen3d128() {\n"
  "    ivec3 coord = ivec3(gl_GlobalInvocationID.xyz);\n"
  "    vec3 uv = vec3(coord) / vec3(u_width, u_height, u_depth);\n"
  "\n"
  "    vec4 color = worley3CH(uv);\n"
  "    color.gba = color.rgb;\n"
  "    color.r = perlin_noise(uv * u_startingScale) * 0.5 + 0.5 + color.g;\n"
  "    \n"
  "    imageStore(u_3d128, coord, color);\n"
  "}\n"
  "\n"
  "void gen3d32() {\n"
  "    ivec3 coord = ivec3(gl_GlobalInvocationID.xyz);\n"
  "    vec3 uv = vec3(coord) / vec3(u_width, u_height, u_depth);\n"
  "\n"
  "    imageStore(u_3d32, coord, worley3CH(uv));\n"
  "}\n"
  "\n"
  "void gen2d128() {\n"
  "    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);\n"
  "    vec2 uv = vec2(coord) / vec2(u_width, u_height);\n"
  "\n"
  "    vec4 color =  vec4(curlNoise(vec3(uv, 0.0)), 1.0);\n"
  "\n"
  "    imageStore(u_2d128, coord, color);\n"
  "}\n"
  "\n"
  "\n"
  "void main() {\n"
  "    if (u_mode == 0) {\n"
  "        gen3d128();\n"
  "    } else if (u_mode == 1) {\n"
  "        gen3d32();\n"
  "    } else if (u_mode == 2) {\n"
  "        gen2d128();\n"
  "    }\n"
  "}\n";

static struct 
{
  struct 
  {
    CGL_window* window;
    CGL_framebuffer* framebuffer;
    CGL_shader* presentShader;
    CGL_float time;
    CGL_float deltaTime;
    CGL_int frame;

    CGL_int vizMode;
    CGL_int vizChannel;
    CGL_float vizSlice;
    CGL_float vizScale;
    CGL_vec3 vizOffset;
  } core;


  struct 
  {
    CGL_mesh_gpu* cube;
    CGL_mesh_gpu* sphere;
    CGL_shader* diffuseShader;
    CGL_framebuffer* framebuffer;
    CGL_mat4 modelMat;

    struct 
    {
      CGL_vec3 position;
      CGL_vec3 color;
    } light;

    struct 
    {
      CGL_float fov;
      CGL_float aspect;
      CGL_float nearV;
      CGL_float farV;
      CGL_vec3 position;
      CGL_vec3 target;
      CGL_mat4 viewMat;
      CGL_mat4 projectionMat;
      CGL_mat4 viewProjectionMat;
      float moveSpeed;
    } camera;
  } scene;


  struct 
  {
    CGL_framebuffer* framebuffer;
    CGL_shader* shader;

    CGL_shader* noiseShader;

    CGL_texture* noiseTexture3d128;
    CGL_texture* noiseTexture3d32;
    CGL_texture* noiseTexture2d128;

    CGL_float noiseStartingScale[3];
    CGL_float noiseScaleFactor[3];

    CGL_vec3 boundsMin;
    CGL_vec3 boundsMax;
    CGL_vec4 phaseParams;

    CGL_float lightAbsorptionThroughSun;
    CGL_float lightAbsorptionThroughCloud;

    CGL_float densityFactor;
    CGL_float scale;
    CGL_float threshold;
    CGL_vec3 offset;
    CGL_float heightFactor;
    CGL_vec4 shapeNoiseWeights;

    CGL_float detailScale;
    CGL_vec3 detailOffset;
    CGL_vec4 detailNoiseWeights;

    CGL_float animationSpeed;

    CGL_int numSteps;    
    CGL_int numLightSteps;    

  } cloud;

  struct 
  {
    CGL_vec2 mousePos;
    CGL_vec2 mouseDelta;

    struct 
    {
      CGL_bool left;
      CGL_bool right;
      CGL_bool middle;
    } mouseButton;
  } input;

#if GUI_DEBUG

  struct 
  {
    struct nk_context* ctx;
    struct nk_colorf bg;
    struct nk_glfw glfw;
  } nuklearData;

#endif

} g_Context;

void setup_camera()
{
  CGL_int width, height;
  CGL_window_get_size(g_Context.core.window, &width, &height);
  g_Context.scene.camera.aspect = (float)width / (float)height;
  g_Context.scene.camera.fov = CGL_deg_to_rad(60.0f);
  g_Context.scene.camera.nearV = 0.1f;
  g_Context.scene.camera.farV = 100.0f;

  g_Context.scene.camera.moveSpeed = 8.0f;
}

void calculate_camera()
{
  g_Context.scene.camera.viewMat = CGL_mat4_look_at(g_Context.scene.camera.position, g_Context.scene.camera.target, CGL_vec3_init(0.0001f, 1.001f, 0.0001f));
  g_Context.scene.camera.projectionMat = CGL_mat4_perspective(g_Context.scene.camera.fov, g_Context.scene.camera.aspect, g_Context.scene.camera.nearV, g_Context.scene.camera.farV);
  g_Context.scene.camera.viewProjectionMat = CGL_mat4_mul(g_Context.scene.camera.projectionMat, g_Context.scene.camera.viewMat);
}

void generate_noise_data()
{
  CGL_info("Generating noise data...");

  CGL_float startTime = CGL_utils_get_time();

  glBindImageTexture(0, CGL_texture_get_internal_handle(g_Context.cloud.noiseTexture3d128), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
  glBindImageTexture(1, CGL_texture_get_internal_handle(g_Context.cloud.noiseTexture3d32), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
  glBindImageTexture(2, CGL_texture_get_internal_handle(g_Context.cloud.noiseTexture2d128), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);


  // the 128x128x128 4-channel perlin worley noise
  CGL_shader_bind(g_Context.cloud.noiseShader);
  CGL_shader_set_uniform_int(g_Context.cloud.noiseShader, CGL_shader_get_uniform_location(g_Context.cloud.noiseShader, "u_mode"), 0);
  CGL_shader_set_uniform_int(g_Context.cloud.noiseShader, CGL_shader_get_uniform_location(g_Context.cloud.noiseShader, "u_width"), 128);
  CGL_shader_set_uniform_int(g_Context.cloud.noiseShader, CGL_shader_get_uniform_location(g_Context.cloud.noiseShader, "u_height"), 128);
  CGL_shader_set_uniform_int(g_Context.cloud.noiseShader, CGL_shader_get_uniform_location(g_Context.cloud.noiseShader, "u_depth"), 128);
  CGL_shader_set_uniform_float(g_Context.cloud.noiseShader, CGL_shader_get_uniform_location(g_Context.cloud.noiseShader, "u_startingScale"), g_Context.cloud.noiseStartingScale[0]);
  CGL_shader_set_uniform_float(g_Context.cloud.noiseShader, CGL_shader_get_uniform_location(g_Context.cloud.noiseShader, "u_scaleFactor"), g_Context.cloud.noiseScaleFactor[0]);
  CGL_shader_compute_dispatch(g_Context.cloud.noiseShader, 128 / 16, 128 / 16, 128);



  // the 32x32x32 4-channel worley noise (here its 64)
  CGL_shader_bind(g_Context.cloud.noiseShader);
  CGL_shader_set_uniform_int(g_Context.cloud.noiseShader, CGL_shader_get_uniform_location(g_Context.cloud.noiseShader, "u_mode"), 1);
  CGL_shader_set_uniform_int(g_Context.cloud.noiseShader, CGL_shader_get_uniform_location(g_Context.cloud.noiseShader, "u_width"), 64);
  CGL_shader_set_uniform_int(g_Context.cloud.noiseShader, CGL_shader_get_uniform_location(g_Context.cloud.noiseShader, "u_height"), 64);
  CGL_shader_set_uniform_int(g_Context.cloud.noiseShader, CGL_shader_get_uniform_location(g_Context.cloud.noiseShader, "u_depth"), 64);
  CGL_shader_set_uniform_float(g_Context.cloud.noiseShader, CGL_shader_get_uniform_location(g_Context.cloud.noiseShader, "u_startingScale"), g_Context.cloud.noiseStartingScale[1]);
  CGL_shader_set_uniform_float(g_Context.cloud.noiseShader, CGL_shader_get_uniform_location(g_Context.cloud.noiseShader, "u_scaleFactor"), g_Context.cloud.noiseScaleFactor[1]);
  CGL_shader_compute_dispatch(g_Context.cloud.noiseShader, 64 / 16, 64 / 16, 64);

  // the 128x128 2-channel curl noise
  CGL_shader_bind(g_Context.cloud.noiseShader);
  CGL_shader_set_uniform_int(g_Context.cloud.noiseShader, CGL_shader_get_uniform_location(g_Context.cloud.noiseShader, "u_mode"), 2);
  CGL_shader_set_uniform_int(g_Context.cloud.noiseShader, CGL_shader_get_uniform_location(g_Context.cloud.noiseShader, "u_width"), 128);
  CGL_shader_set_uniform_int(g_Context.cloud.noiseShader, CGL_shader_get_uniform_location(g_Context.cloud.noiseShader, "u_height"), 128);
  CGL_shader_set_uniform_float(g_Context.cloud.noiseShader, CGL_shader_get_uniform_location(g_Context.cloud.noiseShader, "u_startingScale"), g_Context.cloud.noiseStartingScale[2]);
  CGL_shader_set_uniform_float(g_Context.cloud.noiseShader, CGL_shader_get_uniform_location(g_Context.cloud.noiseShader, "u_scaleFactor"), g_Context.cloud.noiseScaleFactor[2]);
  CGL_shader_compute_dispatch(g_Context.cloud.noiseShader, 128 / 16, 128 / 16, 1);


  
  CGL_float endTime = CGL_utils_get_time();
  CGL_info("Noise data generated in %f seconds", endTime - startTime);
}

void setup_scene() 
{
  // Setup Meshes
  CGL_mesh_cpu* cube = CGL_mesh_cpu_cube(false);
  CGL_mesh_cpu_recalculate_normals(cube);

  CGL_mesh_cpu* sphere = CGL_mesh_cpu_sphere(32, 32);
  CGL_mesh_cpu_recalculate_normals(sphere);

  g_Context.scene.cube = CGL_mesh_gpu_create();
  CGL_mesh_gpu_upload(g_Context.scene.cube, cube, true);
  CGL_mesh_cpu_destroy(cube);

  g_Context.scene.sphere = CGL_mesh_gpu_create();
  CGL_mesh_gpu_upload(g_Context.scene.sphere, sphere, true);
  CGL_mesh_cpu_destroy(sphere);

  // Setup Framebuffer
  g_Context.scene.framebuffer = CGL_framebuffer_create(SIZEX, SIZEY);
  g_Context.cloud.framebuffer = CGL_framebuffer_create(SIZEX, SIZEY);

  // Setup Shaders
  g_Context.scene.diffuseShader = CGL_shader_create(DIFFUSE_VERTEX_SHADER_SOURCE, DIFFUSE_FRAGMENT_SHADER_SOURCE, NULL);
  g_Context.core.presentShader = CGL_shader_create(PRESENT_VERTEX_SHADER_SOURCE, PRESENT_FRAGMENT_SHADER_SOURCE, NULL);



  // Setup Light
  g_Context.scene.light.position = CGL_vec3_init(10.0f, 10.0f, 10.0f);
  g_Context.scene.light.color = CGL_vec3_init(1.0f, 1.0f, 1.0f);  

  // Setup Camera
  g_Context.scene.camera.position = CGL_vec3_init(8.0f, 8.0f, 8.0f);
  g_Context.scene.camera.target = CGL_vec3_init(0.0f, 1.0f, 0.0f);
  setup_camera();
  calculate_camera();

  // Setup Clouds
  // g_Context.cloud.noiseShader = CGL_shader_compute_create_from_files("compute.glsl", NULL);
  g_Context.cloud.noiseShader = CGL_shader_compute_create(CLOUDS_COMPUTE_SHADER_SOURCE, NULL);

  // CGL_byte* cloudFragShaderSource = CGL_utils_read_file("frag.glsl", NULL);
  // g_Context.cloud.shader = CGL_shader_create(CLOUDS_VERTEX_SHADER_SOURCE, cloudFragShaderSource, NULL);
  // CGL_free(cloudFragShaderSource);
  g_Context.cloud.shader = CGL_shader_create(CLOUDS_VERTEX_SHADER_SOURCE, CLOUDS_FRAGMENT_SHADER_SOURCE, NULL);

  g_Context.cloud.noiseTexture3d128 = CGL_texture_create_3d(128, 128, 128, GL_RGBA, GL_RGBA32F, GL_FLOAT);
  g_Context.cloud.noiseTexture3d32 = CGL_texture_create_3d(64, 64, 64, GL_RGBA, GL_RGBA32F, GL_FLOAT);
  g_Context.cloud.noiseTexture2d128 = CGL_texture_create_blank(128, 128, GL_RGBA, GL_RGBA32F, GL_FLOAT);  
  g_Context.core.vizChannel = 0;
  g_Context.core.vizMode = 0;

  generate_noise_data();
}

void update_camera()
{
  if (g_Context.input.mouseButton.middle)
  {
    CGL_vec3 cameraFront = CGL_vec3_normalize_(CGL_vec3_sub(g_Context.scene.camera.target, g_Context.scene.camera.position));
    CGL_vec3 cameraLeft = CGL_vec3_normalize_(CGL_vec3_cross_(cameraFront, CGL_vec3_init(0.0001f, 1.0001f, 0.0001f)));
    CGL_vec3 cameraUp = CGL_vec3_normalize_(CGL_vec3_cross_(cameraLeft, cameraFront));

    g_Context.scene.camera.position = CGL_vec3_sub_(g_Context.scene.camera.position, CGL_vec3_scale_(cameraLeft, g_Context.input.mouseDelta.x * g_Context.scene.camera.moveSpeed ));

    if (CGL_window_is_key_pressed(g_Context.core.window, CGL_KEY_LEFT_SHIFT))
      g_Context.scene.camera.position = CGL_vec3_add_(g_Context.scene.camera.position, CGL_vec3_scale_(cameraFront, g_Context.input.mouseDelta.y * g_Context.scene.camera.moveSpeed ));
    else 
      g_Context.scene.camera.position = CGL_vec3_sub_(g_Context.scene.camera.position, CGL_vec3_scale_(cameraUp, g_Context.input.mouseDelta.y * g_Context.scene.camera.moveSpeed ));

    calculate_camera();
  }

}

#if 0
// This part is just for auto shader reloading during development
void reload_cloud_shader()
{
  static CGL_uint lastHashFrag = 0, lastHashCompute = 0;

  CGL_sizei fileSize = 0;
  CGL_byte* cloudShaderSource = CGL_utils_read_file("frag.glsl", &fileSize);
  if (fileSize > 0)
  {
    CGL_uint hash = CGL_utils_super_fast_hash(cloudShaderSource, fileSize);
    if (hash != lastHashFrag)
    {
      CGL_shader* shader = CGL_shader_create(CLOUDS_VERTEX_SHADER_SOURCE, cloudShaderSource, NULL);
      if (shader) 
      {
        CGL_shader_destroy(g_Context.cloud.shader); 
        g_Context.cloud.shader = shader;
        CGL_info("Clouds fragment shader reloaded");
      }
      else 
      {
        CGL_warn("Cloud fragment shader reload failed");
      }
      lastHashFrag = hash;
    }
    CGL_free(cloudShaderSource);
  }

  cloudShaderSource = CGL_utils_read_file("compute.glsl", &fileSize);
  if (fileSize > 0)
  {
    CGL_uint hash = CGL_utils_super_fast_hash(cloudShaderSource, fileSize);
    if (hash != lastHashCompute)
    {
      CGL_shader* shader = CGL_shader_compute_create(cloudShaderSource, NULL);
      if (shader) 
      {
        CGL_shader_destroy(g_Context.cloud.noiseShader);
        g_Context.cloud.noiseShader = shader;
        CGL_info("Clouds compute shader reloaded");
        generate_noise_data();
      }
      else 
      {
        CGL_warn("Cloud compute shader reload failed");
      }
      lastHashCompute = hash;
    }
    CGL_free(cloudShaderSource);
  }

}
#endif

void update_scene()
{
  g_Context.scene.light.position.x = 10.0f * cosf(g_Context.core.time);
  g_Context.scene.light.position.z = 10.0f * sinf(g_Context.core.time);

  #if 0
  if(g_Context.core.frame % 60 == 0)
    reload_cloud_shader();
  #endif

  update_camera();
  
  
  {
    for (CGL_int i = CGL_KEY_0; i <= CGL_KEY_4; i++)
    {
      if (CGL_window_is_key_pressed(g_Context.core.window, i))
      {
        CGL_int ind = i - CGL_KEY_0 - 1;
        if (CGL_window_is_key_pressed(g_Context.core.window, CGL_KEY_M)) 
          g_Context.core.vizMode = ind;
        else if (CGL_window_is_key_pressed(g_Context.core.window, CGL_KEY_C))
          g_Context.core.vizChannel = ind;
        break;
      }
    }
  }

}

void render_object()
{
  CGL_shader_bind(g_Context.scene.diffuseShader);
  CGL_shader_set_uniform_vec3(g_Context.scene.diffuseShader, CGL_shader_get_uniform_location(g_Context.scene.diffuseShader, "u_lightPos"), &g_Context.scene.light.position);
  CGL_shader_set_uniform_vec3(g_Context.scene.diffuseShader, CGL_shader_get_uniform_location(g_Context.scene.diffuseShader, "u_lightColor"), &g_Context.scene.light.color);

  // Render Cube
  CGL_shader_set_uniform_mat4(g_Context.scene.diffuseShader, CGL_shader_get_uniform_location(g_Context.scene.diffuseShader, "u_view"), &g_Context.scene.camera.viewMat);
  CGL_shader_set_uniform_mat4(g_Context.scene.diffuseShader, CGL_shader_get_uniform_location(g_Context.scene.diffuseShader, "u_projection"), &g_Context.scene.camera.projectionMat);
  CGL_shader_set_uniform_mat4(g_Context.scene.diffuseShader, CGL_shader_get_uniform_location(g_Context.scene.diffuseShader, "u_viewProjection"), &g_Context.scene.camera.viewProjectionMat);

  g_Context.scene.modelMat = CGL_mat4_translate(0.0f, -0.1f, 0.0f);
  g_Context.scene.modelMat = CGL_mat4_mul(g_Context.scene.modelMat, CGL_mat4_scale(8.0f, 0.2f, 8.0f));
  CGL_shader_set_uniform_mat4(g_Context.scene.diffuseShader, CGL_shader_get_uniform_location(g_Context.scene.diffuseShader, "u_model"), &g_Context.scene.modelMat);
  CGL_shader_set_uniform_vec3v(g_Context.scene.diffuseShader, CGL_shader_get_uniform_location(g_Context.scene.diffuseShader, "u_objectColor"), 0.55f, 0.55f, 0.3f);
  CGL_mesh_gpu_render(g_Context.scene.cube);

  g_Context.scene.modelMat = CGL_mat4_translate(0.5f, 0.6f, 0.0f);
  g_Context.scene.modelMat = CGL_mat4_mul(g_Context.scene.modelMat, CGL_mat4_scale(0.6f, 0.6f, 0.6f));
  g_Context.scene.modelMat = CGL_mat4_mul(g_Context.scene.modelMat, CGL_mat4_rotate_y(CGL_PI * 0.25f));
  CGL_shader_set_uniform_mat4(g_Context.scene.diffuseShader, CGL_shader_get_uniform_location(g_Context.scene.diffuseShader, "u_model"), &g_Context.scene.modelMat);
  CGL_shader_set_uniform_vec3v(g_Context.scene.diffuseShader, CGL_shader_get_uniform_location(g_Context.scene.diffuseShader, "u_objectColor"), 0.6f, 0.2f, 0.1f);
  CGL_mesh_gpu_render(g_Context.scene.cube);

  g_Context.scene.modelMat = CGL_mat4_translate(-3.5f, 0.6f, 2.8f);
  g_Context.scene.modelMat = CGL_mat4_mul(g_Context.scene.modelMat, CGL_mat4_scale(0.5f, 2.8f, 0.5f));
  g_Context.scene.modelMat = CGL_mat4_mul(g_Context.scene.modelMat, CGL_mat4_rotate_y(CGL_PI * 0.25f));
  CGL_shader_set_uniform_mat4(g_Context.scene.diffuseShader, CGL_shader_get_uniform_location(g_Context.scene.diffuseShader, "u_model"), &g_Context.scene.modelMat);
  CGL_shader_set_uniform_vec3v(g_Context.scene.diffuseShader, CGL_shader_get_uniform_location(g_Context.scene.diffuseShader, "u_objectColor"), 0.1f, 0.6f, 0.2f);
  CGL_mesh_gpu_render(g_Context.scene.cube);

  g_Context.scene.modelMat = CGL_mat4_translate(2.0f, 1.0f, 3.0f);
  g_Context.scene.modelMat = CGL_mat4_mul(g_Context.scene.modelMat, CGL_mat4_scale(0.8f, 0.8f, 0.8f));
  CGL_shader_set_uniform_mat4(g_Context.scene.diffuseShader, CGL_shader_get_uniform_location(g_Context.scene.diffuseShader, "u_model"), &g_Context.scene.modelMat);
  CGL_shader_set_uniform_vec3v(g_Context.scene.diffuseShader, CGL_shader_get_uniform_location(g_Context.scene.diffuseShader, "u_objectColor"), 0.2f, 0.2f, 0.6f);
  CGL_mesh_gpu_render(g_Context.scene.cube);
}

void render_cloud()
{
  CGL_shader_bind(g_Context.cloud.shader);
  
  CGL_texture_bind(CGL_framebuffer_get_color_texture(g_Context.scene.framebuffer), 1);
  CGL_shader_set_uniform_int(g_Context.cloud.shader, CGL_shader_get_uniform_location(g_Context.cloud.shader, "u_texture"), 1);
  
  CGL_texture_bind(CGL_framebuffer_get_depth_texture(g_Context.scene.framebuffer), 2);
  CGL_shader_set_uniform_int(g_Context.cloud.shader, CGL_shader_get_uniform_location(g_Context.cloud.shader, "u_depth"), 2);

  CGL_texture_bind(g_Context.cloud.noiseTexture3d128, 3);
  CGL_shader_set_uniform_int(g_Context.cloud.shader, CGL_shader_get_uniform_location(g_Context.cloud.shader, "u_shapeNoise"), 3);

  CGL_texture_bind(g_Context.cloud.noiseTexture3d32, 4);
  CGL_shader_set_uniform_int(g_Context.cloud.shader, CGL_shader_get_uniform_location(g_Context.cloud.shader, "u_detailNoise"), 4);

  CGL_texture_bind(g_Context.cloud.noiseTexture2d128, 5);
  CGL_shader_set_uniform_int(g_Context.cloud.shader, CGL_shader_get_uniform_location(g_Context.cloud.shader, "u_curlNoise"), 5);

  CGL_shader_set_uniform_float(g_Context.cloud.shader, CGL_shader_get_uniform_location(g_Context.cloud.shader, "u_far"), g_Context.scene.camera.farV);
  CGL_shader_set_uniform_float(g_Context.cloud.shader, CGL_shader_get_uniform_location(g_Context.cloud.shader, "u_near"), g_Context.scene.camera.nearV);
  CGL_shader_set_uniform_vec3(g_Context.cloud.shader, CGL_shader_get_uniform_location(g_Context.cloud.shader, "u_cameraPos"), &g_Context.scene.camera.position);
  CGL_shader_set_uniform_vec3(g_Context.cloud.shader, CGL_shader_get_uniform_location(g_Context.cloud.shader, "u_lightPos"), &g_Context.scene.light.position);
  CGL_shader_set_uniform_vec3(g_Context.cloud.shader, CGL_shader_get_uniform_location(g_Context.cloud.shader, "u_lightColor"), &g_Context.scene.light.color);

  CGL_shader_set_uniform_mat4(g_Context.cloud.shader, CGL_shader_get_uniform_location(g_Context.cloud.shader, "u_projection"), &g_Context.scene.camera.projectionMat);
  CGL_shader_set_uniform_mat4(g_Context.cloud.shader, CGL_shader_get_uniform_location(g_Context.cloud.shader, "u_view"), &g_Context.scene.camera.viewMat);


  CGL_shader_set_uniform_vec3(g_Context.cloud.shader, CGL_shader_get_uniform_location(g_Context.cloud.shader, "u_cloudBoundsMin"), &g_Context.cloud.boundsMin);
  CGL_shader_set_uniform_vec3(g_Context.cloud.shader, CGL_shader_get_uniform_location(g_Context.cloud.shader, "u_cloudBoundsMax"), &g_Context.cloud.boundsMax);
  CGL_shader_set_uniform_vec4(g_Context.cloud.shader, CGL_shader_get_uniform_location(g_Context.cloud.shader, "u_phaseParams"), &g_Context.cloud.phaseParams);
  CGL_shader_set_uniform_float(g_Context.cloud.shader, CGL_shader_get_uniform_location(g_Context.cloud.shader, "u_lightAbsorptionThroughSun"), g_Context.cloud.lightAbsorptionThroughSun);
  CGL_shader_set_uniform_float(g_Context.cloud.shader, CGL_shader_get_uniform_location(g_Context.cloud.shader, "u_lightAbsorptionThroughCloud"), g_Context.cloud.lightAbsorptionThroughCloud);  
  CGL_shader_set_uniform_float(g_Context.cloud.shader, CGL_shader_get_uniform_location(g_Context.cloud.shader, "u_densityFactor"), g_Context.cloud.densityFactor);
  CGL_shader_set_uniform_float(g_Context.cloud.shader, CGL_shader_get_uniform_location(g_Context.cloud.shader, "u_scale"), g_Context.cloud.scale);
  CGL_shader_set_uniform_float(g_Context.cloud.shader, CGL_shader_get_uniform_location(g_Context.cloud.shader, "u_time"), g_Context.core.time);
  CGL_shader_set_uniform_vec3(g_Context.cloud.shader, CGL_shader_get_uniform_location(g_Context.cloud.shader, "u_offset"), &g_Context.cloud.offset);
  CGL_shader_set_uniform_float(g_Context.cloud.shader, CGL_shader_get_uniform_location(g_Context.cloud.shader, "u_threshold"), g_Context.cloud.threshold);
  CGL_shader_set_uniform_float(g_Context.cloud.shader, CGL_shader_get_uniform_location(g_Context.cloud.shader, "u_heightFactor"), g_Context.cloud.heightFactor);
  CGL_shader_set_uniform_vec4(g_Context.cloud.shader, CGL_shader_get_uniform_location(g_Context.cloud.shader, "u_shapeNoiseWeights"), &g_Context.cloud.shapeNoiseWeights);

  CGL_shader_set_uniform_float(g_Context.cloud.shader, CGL_shader_get_uniform_location(g_Context.cloud.shader, "u_detailScale"), g_Context.cloud.detailScale);
  CGL_shader_set_uniform_vec3(g_Context.cloud.shader, CGL_shader_get_uniform_location(g_Context.cloud.shader, "u_detailOffset"), &g_Context.cloud.detailOffset);
  CGL_shader_set_uniform_vec4(g_Context.cloud.shader, CGL_shader_get_uniform_location(g_Context.cloud.shader, "u_detailNoiseWeights"), &g_Context.cloud.detailNoiseWeights);

  CGL_shader_set_uniform_float(g_Context.cloud.shader, CGL_shader_get_uniform_location(g_Context.cloud.shader, "u_animationSpeed"), g_Context.cloud.animationSpeed);

  CGL_shader_set_uniform_int(g_Context.cloud.shader, CGL_shader_get_uniform_location(g_Context.cloud.shader, "u_numSteps"), g_Context.cloud.numSteps);
  CGL_shader_set_uniform_int(g_Context.cloud.shader, CGL_shader_get_uniform_location(g_Context.cloud.shader, "u_numLightSteps"), g_Context.cloud.numLightSteps);


  CGL_gl_render_screen_quad();
}

void render_present()
{
  CGL_shader_bind(g_Context.core.presentShader);
    
  CGL_texture_bind(CGL_framebuffer_get_color_texture(g_Context.cloud.framebuffer), 1);
  CGL_shader_set_uniform_int(g_Context.core.presentShader, CGL_shader_get_uniform_location(g_Context.core.presentShader, "u_texture"), 1);

  CGL_texture_bind(g_Context.cloud.noiseTexture3d128, 2);
  CGL_shader_set_uniform_int(g_Context.core.presentShader, CGL_shader_get_uniform_location(g_Context.core.presentShader, "u_noiseTexture3d128"), 2);

  CGL_texture_bind(g_Context.cloud.noiseTexture3d32, 3);
  CGL_shader_set_uniform_int(g_Context.core.presentShader, CGL_shader_get_uniform_location(g_Context.core.presentShader, "u_noiseTexture3d32"), 3);

  CGL_texture_bind(g_Context.cloud.noiseTexture2d128, 4);
  CGL_shader_set_uniform_int(g_Context.core.presentShader, CGL_shader_get_uniform_location(g_Context.core.presentShader, "u_noiseTexture2d128"), 4);

  CGL_shader_set_uniform_int(g_Context.core.presentShader, CGL_shader_get_uniform_location(g_Context.core.presentShader, "u_vizMode"), g_Context.core.vizMode);
  CGL_shader_set_uniform_int(g_Context.core.presentShader, CGL_shader_get_uniform_location(g_Context.core.presentShader, "u_channel"), g_Context.core.vizChannel);
  CGL_shader_set_uniform_float(g_Context.core.presentShader, CGL_shader_get_uniform_location(g_Context.core.presentShader, "u_time"), g_Context.core.time);
  CGL_shader_set_uniform_float(g_Context.core.presentShader, CGL_shader_get_uniform_location(g_Context.core.presentShader, "u_aspectRatio"), g_Context.scene.camera.aspect);
  CGL_shader_set_uniform_float(g_Context.core.presentShader, CGL_shader_get_uniform_location(g_Context.core.presentShader, "u_slice"), g_Context.core.vizSlice);
  CGL_shader_set_uniform_float(g_Context.core.presentShader, CGL_shader_get_uniform_location(g_Context.core.presentShader, "u_scale"), g_Context.core.vizScale);
  CGL_shader_set_uniform_vec3(g_Context.core.presentShader, CGL_shader_get_uniform_location(g_Context.core.presentShader, "u_offset"), &g_Context.core.vizOffset);
  CGL_gl_render_screen_quad();
}

void render_scene()
{
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glDepthFunc(GL_LESS);

  CGL_framebuffer_bind(g_Context.scene.framebuffer);
  CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f);  
  render_object();
  
  CGL_framebuffer_bind( g_Context.cloud.framebuffer );
  CGL_gl_clear(0.0f, 0.0f, 0.0f, 1.0f);
  render_cloud();

  CGL_framebuffer_bind( g_Context.core.framebuffer );
  CGL_gl_clear(0.0f, 0.0f, 0.0f, 1.0f);
  render_present();
}

#if GUI_DEBUG

void render_nuklear()
{
  struct nk_context *ctx = g_Context.nuklearData.ctx;
  if (nk_begin(ctx, "Settings", nk_rect(50, 50, 230, 250),
    NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
    NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))

  static int settingsTab = 0;
  static const char* settingsTabNames[] = {"Vizualization", "Cloud Noise", "Cloud Settings"};
  static int settingsTabCount = sizeof(settingsTabNames) / sizeof(settingsTabNames[0]);

  nk_layout_row_dynamic(ctx, 25, 1);
  settingsTab = nk_combo(ctx, settingsTabNames, settingsTabCount, settingsTab, 30, nk_vec2(200,200));

  nk_layout_row_dynamic(ctx, 5, 1);
  nk_label(ctx, "", NK_TEXT_LEFT);
  // Vizualization
  if (settingsTab == 0)  
  {
    
    // a combo box for viz mode with label
    {
      static const char *items[] = {"Color", "Shape Noise", "Detail Noise", "Curl Noise"};
      static int selected = 0;
      //nk_layout_row(ctx, NK_DYNAMIC, 25, 2, (float[]){0.35f, 0.65f});
      nk_layout_row_dynamic(ctx, 25, 2);
      nk_label(ctx, "Viz Mode", NK_TEXT_LEFT);
      selected = nk_combo(ctx, items, sizeof(items) / sizeof(items[0]), selected, 25, nk_vec2(200,200));
      g_Context.core.vizMode = selected;
    }

    if (g_Context.core.vizMode != 0)
    {
      // a combo box for viz channel with label
      {
        static const char *items[] = {"Red", "Green", "Blue", "Alpha"};
        static int selected = 0;
        //nk_layout_row(ctx, NK_DYNAMIC, 25, 2, (float[]){0.35f, 0.65f});
        nk_layout_row_dynamic(ctx, 25, 2);
        nk_label(ctx, "Viz Channel", NK_TEXT_LEFT);
        selected = nk_combo(ctx, items, sizeof(items) / sizeof(items[0]), selected, 25, nk_vec2(200,200));
        g_Context.core.vizChannel = selected;
      }

      // Viz Slice
      {
        nk_layout_row_dynamic(ctx, 25, 2);
        nk_label(ctx, "Viz Slice", NK_TEXT_LEFT);
        nk_slider_float(ctx, 0.0f, &g_Context.core.vizSlice, 1.0f, 0.01f);
      }

      // Viz Scale
      {
        nk_layout_row_dynamic(ctx, 25, 2);
        nk_label(ctx, "Viz Scale", NK_TEXT_LEFT);
        nk_slider_float(ctx, 1.0f, &g_Context.core.vizScale, 20.0f, 0.01f);
      }

      // Viz Offset
      {
        nk_layout_row_dynamic(ctx, 25, 4);
        nk_label(ctx, "Viz Offset", NK_TEXT_LEFT);
        nk_property_float(ctx, "X:", -10.0f, &g_Context.core.vizOffset.x, 10.0f, 0.01f, 0.01f);
        nk_property_float(ctx, "Y:", -10.0f, &g_Context.core.vizOffset.y, 10.0f, 0.01f, 0.01f);
        nk_property_float(ctx, "Z:", -10.0f, &g_Context.core.vizOffset.z, 10.0f, 0.01f, 0.01f);
      }

    }

  }
  // Cloud Noise
  else if (settingsTab == 1)
  {

    static int noiseTexture = 0;
    // a combo box for noise texture
    {
      static const char *items[] = {"Shape Noise", "Detail Noise", "Curl Noise"};
      nk_layout_row_dynamic(ctx, 25, 2);
      nk_label(ctx, "Texture", NK_TEXT_LEFT);
      noiseTexture = nk_combo(ctx, items, sizeof(items) / sizeof(items[0]), noiseTexture, 25, nk_vec2(200,200));
    }

    bool hasNoiseChanged = false;

    // the starting scale
    {
      nk_layout_row_dynamic(ctx, 25, 2);
      nk_label(ctx, "Starting Scale", NK_TEXT_LEFT);
      CGL_int tmp = (CGL_int)g_Context.cloud.noiseStartingScale[noiseTexture];
      hasNoiseChanged = nk_slider_int(ctx,  0, &tmp, 20, 1) || hasNoiseChanged;
      g_Context.cloud.noiseStartingScale[noiseTexture] = (float)tmp;

    }


    if(hasNoiseChanged) 
    {
      generate_noise_data();
    }

  }
  // Cloud Settings
  else if (settingsTab == 2)
  {
    // clouds bounds min
    {
      nk_layout_row_dynamic(ctx, 25, 4);
      nk_label(ctx, "Bounds Min", NK_TEXT_LEFT);
      nk_property_float(ctx, "X Max:", -100.0f, &g_Context.cloud.boundsMin.x, 100.0f, 0.1f, 0.01f);
      nk_property_float(ctx, "Y Max:", -100.0f, &g_Context.cloud.boundsMin.y, 100.0f, 0.1f, 0.01f);
      nk_property_float(ctx, "Z Max:", -100.0f, &g_Context.cloud.boundsMin.z, 100.0f, 0.1f, 0.01f);
    }

    // clouds bounds max
    {
      nk_layout_row_dynamic(ctx, 25, 4);
      nk_label(ctx, "Bounds Max", NK_TEXT_LEFT);
      nk_property_float(ctx, "X Min:", -100.0f, &g_Context.cloud.boundsMax.x, 100.0f, 0.01f, 0.01f);
      nk_property_float(ctx, "Y Min:", -100.0f, &g_Context.cloud.boundsMax.y, 100.0f, 0.01f, 0.01f);
      nk_property_float(ctx, "Z Min:", -100.0f, &g_Context.cloud.boundsMax.z, 100.0f, 0.01f, 0.01f);
    }

    // light absorption through sun
    {
      nk_layout_row_dynamic(ctx, 25, 2);
      nk_label(ctx, "Light Absorption Through Sun", NK_TEXT_LEFT);
      nk_slider_float(ctx, 0.0f, &g_Context.cloud.lightAbsorptionThroughSun, 1.0f, 0.01f);
    }

    // light absorption through cloud
    {
      nk_layout_row_dynamic(ctx, 25, 2);
      nk_label(ctx, "Light Absorption Through Cloud", NK_TEXT_LEFT);
      nk_slider_float(ctx, 0.0f, &g_Context.cloud.lightAbsorptionThroughCloud, 1.0f, 0.01f);
    }

    // phase params
    {
      nk_layout_row_dynamic(ctx, 25, 1);
      nk_label(ctx, "Phase Params", NK_TEXT_LEFT);
      nk_layout_row_dynamic(ctx, 25, 4);
      nk_slider_float(ctx, 0.0f, &g_Context.cloud.phaseParams.x, 1.0f, 0.01f);
      nk_slider_float(ctx, 0.0f, &g_Context.cloud.phaseParams.y, 1.0f, 0.01f);
      nk_slider_float(ctx, 0.0f, &g_Context.cloud.phaseParams.z, 1.0f, 0.01f);
      nk_slider_float(ctx, 0.0f, &g_Context.cloud.phaseParams.w, 1.0f, 0.01f);
    }

    // density factor
    {
      nk_layout_row_dynamic(ctx, 25, 2);
      nk_label(ctx, "Density Factor", NK_TEXT_LEFT);
      nk_slider_float(ctx, 0.0f, &g_Context.cloud.densityFactor, 500.0f, 0.01f);
    }

    // threshold
    {
      nk_layout_row_dynamic(ctx, 25, 2);
      nk_label(ctx, "Threshold", NK_TEXT_LEFT);
      nk_slider_float(ctx, 0.0f, &g_Context.cloud.threshold, 2.0f, 0.0001f);
    }

    // height factor
    {
      nk_layout_row_dynamic(ctx, 25, 2);
      nk_label(ctx, "Height Factor", NK_TEXT_LEFT);
      nk_slider_float(ctx, 0.0f, &g_Context.cloud.heightFactor, 10.0f, 0.01f);
    }

    // shape noise weights
    {
      nk_layout_row_dynamic(ctx, 25, 1);
      nk_label(ctx, "Shape Noise Weights", NK_TEXT_LEFT);
      nk_layout_row_dynamic(ctx, 25, 4);
      nk_slider_float(ctx, 0.0f, &g_Context.cloud.shapeNoiseWeights.x, 1.0f, 0.01f);
      nk_slider_float(ctx, 0.0f, &g_Context.cloud.shapeNoiseWeights.y, 1.0f, 0.01f);
      nk_slider_float(ctx, 0.0f, &g_Context.cloud.shapeNoiseWeights.z, 1.0f, 0.01f);
      nk_slider_float(ctx, 0.0f, &g_Context.cloud.shapeNoiseWeights.w, 1.0f, 0.01f);
    }

    // scale
    {
      nk_layout_row_dynamic(ctx, 25, 2);
      nk_label(ctx, "Scale", NK_TEXT_LEFT);
      nk_slider_float(ctx, 0.0f, &g_Context.cloud.scale, 5.0f, 0.01f);
    }

    // offset
    {
      nk_layout_row_dynamic(ctx, 25, 4);
      nk_label(ctx, "Offset", NK_TEXT_LEFT);
      nk_property_float(ctx, "Xo:", -100.0f, &g_Context.cloud.offset.x, 100.0f, 0.01f, 0.01f);
      nk_property_float(ctx, "Yo:", -100.0f, &g_Context.cloud.offset.y, 100.0f, 0.01f, 0.01f);
      nk_property_float(ctx, "Zo:", -100.0f, &g_Context.cloud.offset.z, 100.0f, 0.01f, 0.01f);
    }

    // detail scale
    {
      nk_layout_row_dynamic(ctx, 25, 2);
      nk_label(ctx, "Detail Scale", NK_TEXT_LEFT);
      nk_slider_float(ctx, 0.0f, &g_Context.cloud.detailScale, 5.0f, 0.01f);
    }

    // detail offset
    {
      nk_layout_row_dynamic(ctx, 25, 4);
      nk_label(ctx, "Detail Offset", NK_TEXT_LEFT);
      nk_property_float(ctx, "Xd:", -100.0f, &g_Context.cloud.detailOffset.x, 100.0f, 0.01f, 0.01f);
      nk_property_float(ctx, "Yd:", -100.0f, &g_Context.cloud.detailOffset.y, 100.0f, 0.01f, 0.01f);
      nk_property_float(ctx, "Zd:", -100.0f, &g_Context.cloud.detailOffset.z, 100.0f, 0.01f, 0.01f);
    }

    // detail noise weights
    {
      nk_layout_row_dynamic(ctx, 25, 1);
      nk_label(ctx, "Detail Noise Weights", NK_TEXT_LEFT);
      nk_layout_row_dynamic(ctx, 25, 4);
      nk_slider_float(ctx, 0.0f, &g_Context.cloud.detailNoiseWeights.x, 1.0f, 0.01f);
      nk_slider_float(ctx, 0.0f, &g_Context.cloud.detailNoiseWeights.y, 1.0f, 0.01f);
      nk_slider_float(ctx, 0.0f, &g_Context.cloud.detailNoiseWeights.z, 1.0f, 0.01f);
      nk_slider_float(ctx, 0.0f, &g_Context.cloud.detailNoiseWeights.w, 1.0f, 0.01f);
    }

    // animation speed
    {
      nk_layout_row_dynamic(ctx, 25, 2);
      nk_label(ctx, "Animation Speed", NK_TEXT_LEFT);
      nk_slider_float(ctx, 0.0f, &g_Context.cloud.animationSpeed, 1.0f, 0.01f);
    }

    // num steps
    {
      nk_layout_row_dynamic(ctx, 25, 2);
      nk_label(ctx, "Num Steps", NK_TEXT_LEFT);
      nk_slider_int(ctx, 0, &g_Context.cloud.numSteps, 200, 1);
    }

    // num light steps
    {
      nk_layout_row_dynamic(ctx, 25, 2);
      nk_label(ctx, "Num Light Steps", NK_TEXT_LEFT);
      nk_slider_int(ctx, 0, &g_Context.cloud.numLightSteps, 50, 1);
    }
    
  }
  nk_end(ctx);
}

#endif

void free_scene()
{
  CGL_mesh_gpu_destroy(g_Context.scene.cube);
  CGL_mesh_gpu_destroy(g_Context.scene.sphere);
  CGL_shader_destroy(g_Context.scene.diffuseShader);
  CGL_shader_destroy(g_Context.core.presentShader);
  CGL_shader_destroy(g_Context.cloud.noiseShader);
  CGL_texture_destroy(g_Context.cloud.noiseTexture3d128);
  CGL_texture_destroy(g_Context.cloud.noiseTexture3d32);
  CGL_texture_destroy(g_Context.cloud.noiseTexture2d128);
  CGL_shader_destroy(g_Context.cloud.shader);
  CGL_framebuffer_destroy(g_Context.scene.framebuffer);
  CGL_framebuffer_destroy(g_Context.cloud.framebuffer);
}

void initialize_stuff()
{
  g_Context.core.framebuffer = CGL_framebuffer_create_from_default(g_Context.core.window);
  g_Context.input.mouseDelta = CGL_vec2_init(0.0f, 0.0f);
  g_Context.input.mousePos = CGL_vec2_init(0.0f, 0.0f);
  g_Context.core.frame = 0;
  g_Context.core.vizMode = 0;
  g_Context.core.vizChannel = 0;
  g_Context.core.vizSlice = 0.0f;
  g_Context.core.vizScale = 1.0f;
  g_Context.core.vizOffset = CGL_vec3_init(0.0f, 0.0f, 0.0f);
  g_Context.cloud.lightAbsorptionThroughSun = 1.0f;
  g_Context.cloud.lightAbsorptionThroughCloud = 1.0f;
  g_Context.cloud.boundsMin = CGL_vec3_init(-6.0f, -1.0f, -6.0f);
  g_Context.cloud.boundsMax = CGL_vec3_init(6.0f, 5.0f, 6.0f);
  g_Context.cloud.phaseParams = CGL_vec4_init(1.0f, 1.0f, 1.0f, 1.0f);
  g_Context.cloud.densityFactor = 64.0f;
  g_Context.cloud.scale = 1.0f;
  g_Context.cloud.offset = CGL_vec3_init(0.0f, 0.0f, 0.0f);
  g_Context.cloud.threshold = 0.65f;
  g_Context.cloud.heightFactor = 2.0f;
  g_Context.cloud.shapeNoiseWeights = CGL_vec4_init(1.0f, 1.0f, 1.0f, 1.0f);
  g_Context.cloud.noiseScaleFactor[0] = g_Context.cloud.noiseScaleFactor[1] = g_Context.cloud.noiseScaleFactor[2] = 2.0f;
  g_Context.cloud.noiseStartingScale[0] = g_Context.cloud.noiseStartingScale[1] = g_Context.cloud.noiseStartingScale[2] = 2.0f;
  g_Context.cloud.detailScale = 1.0f;
  g_Context.cloud.detailOffset = CGL_vec3_init(0.0f, 0.0f, 0.0f);
  g_Context.cloud.detailNoiseWeights = CGL_vec4_init(1.0f, 1.0f, 1.0f, 1.0f);
  g_Context.cloud.animationSpeed = 0.1f;
  g_Context.cloud.numSteps = 100;
  g_Context.cloud.numLightSteps = 20;
}

int main() 
{
  // Initialize stuff
  CGL_init();
  g_Context.core.window  = CGL_window_create(SIZEX, SIZEY, "Clouds - Jaysmito Mukherjee");
  CGL_window_make_context_current( g_Context.core.window );
  CGL_gl_init();
  CGL_widgets_init();

#if GUI_DEBUG

  g_Context.nuklearData.ctx = nk_glfw3_init(&g_Context.nuklearData.glfw, CGL_window_get_glfw_handle(g_Context.core.window), NK_GLFW3_INSTALL_CALLBACKS);
  {
    struct nk_font_atlas *atlas;
    nk_glfw3_font_stash_begin(&g_Context.nuklearData.glfw, &atlas);
    nk_glfw3_font_stash_end(&g_Context.nuklearData.glfw);
  }

#endif

  // Allocate resoruces
  initialize_stuff();
  setup_scene();

  CGL_float prev_time = CGL_utils_get_time();


  while(!CGL_window_should_close(g_Context.core.window)) 
  {
    g_Context.core.time = CGL_utils_get_time();
    g_Context.core.deltaTime = g_Context.core.time - prev_time;
    prev_time = g_Context.core.time;

    g_Context.core.frame += 1;

    CGL_int width, height;
    CGL_window_get_size(g_Context.core.window, &width, &height);

    CGL_double xpos, ypos;
    CGL_window_get_mouse_position(g_Context.core.window, &xpos, &ypos);
    CGL_vec2 mousePos = CGL_vec2_init((CGL_float)xpos, SIZEY - (CGL_float)ypos);
    mousePos = CGL_vec2_div_(mousePos, CGL_vec2_init((CGL_float)width, (CGL_float)height));
    mousePos = CGL_vec2_add_(CGL_vec2_mul_(mousePos, CGL_vec2_init(2.0f, 2.0f)), CGL_vec2_init(-1.0f, -1.0f));

    g_Context.input.mouseDelta = CGL_vec2_sub(mousePos, g_Context.input.mousePos);
    g_Context.input.mousePos = mousePos;

    g_Context.input.mouseButton.left = CGL_window_get_mouse_button(g_Context.core.window, CGL_MOUSE_BUTTON_LEFT) == CGL_PRESS;
    g_Context.input.mouseButton.right = CGL_window_get_mouse_button(g_Context.core.window, CGL_MOUSE_BUTTON_RIGHT) == CGL_PRESS;
    g_Context.input.mouseButton.middle = CGL_window_get_mouse_button(g_Context.core.window, CGL_MOUSE_BUTTON_MIDDLE) == CGL_PRESS;


    
    CGL_window_set_size(g_Context.core.window, SIZEX, SIZEY);
    CGL_window_poll_events(g_Context.core.window);

   
    update_scene();
    render_scene();    

#if GUI_DEBUG
    
    CGL_framebuffer_bind( g_Context.core.framebuffer );
    nk_glfw3_new_frame(&g_Context.nuklearData.glfw);
    render_nuklear();
    nk_glfw3_render(&g_Context.nuklearData.glfw, NK_ANTI_ALIASING_ON, 1024 * 512, 1024 * 128);

#endif

    if (CGL_window_is_key_pressed(g_Context.core.window, CGL_KEY_G))
      generate_noise_data();

    if (CGL_window_is_key_pressed(g_Context.core.window, CGL_KEY_ESCAPE)) 
      break;

    CGL_window_swap_buffers(g_Context.core.window);
  }

  free_scene();

#if GUI_DEBUG
    nk_glfw3_shutdown(&g_Context.nuklearData.glfw);
#endif

  // Free resources and shutdown
  CGL_framebuffer_destroy( g_Context.core.framebuffer );
  CGL_widgets_shutdown();
  CGL_gl_shutdown();
  CGL_window_destroy( g_Context.core.window );
  CGL_shutdown();
}
