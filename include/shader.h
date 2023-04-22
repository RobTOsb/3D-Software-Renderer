//This is where the pixel shading happens.
//Only has a PBR shader as I thought the other ones were cluttering
//up the header file. Means I could've done this without inheritance.

#ifndef SHADER_H
#define SHADER_H

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "renderer.h"
#include "model.h"
#include "camera.h"
#include "lights.h"
#include <iostream>

#define PI 3.14159265359f

struct IShader
{
	virtual glm::vec4 vertex(glm::vec3 position, glm::vec3& uv, glm::vec3& tangent, glm::vec3& normal, int index) = 0;
	virtual glm::vec3 fragment(glm::vec3 bar) = 0;
};

struct PBRShader : IShader
{
    //Camera position
    glm::vec3 cameraPos;

    //Textures
    Texture* mAlbedo;
    Texture* mNormal;
    Texture* mAO;
    Texture* mMetal;
    Texture* mRough;
    Texture* mHeight;

    //Height scaling factor
    float heightScale = 0.3f;

    //Model data
    glm::vec3 varyingUV[3];
    glm::vec3 varyingNorm[3];
    glm::vec3 Tan[3];
    glm::vec3 BiTan[3];
    glm::vec3 Normal[3];
    glm::vec3 varyingWorld[3];
   
    //Matrices
    glm::mat4 MV, MVP, M, V, N;
    glm::mat3 TBN;

    //Light variables
    std::vector<Lights*> lights;
    glm::vec3 lightPosition;
    glm::vec3 lightDir[3];
    float intensity;
    glm::vec3 lightColor;

    float DistributionGGX(glm::vec3 N, glm::vec3 H, float roughness)
    {
        float a = roughness * roughness;
        float a2 = a * a;
        float NdotH = std::max(dot(N, H), 0.0f);
        float NdotH2 = NdotH * NdotH;

        float nom = a2;
        float denom = (NdotH2 * (a2 - 1.0) + 1.0);
        denom = PI * denom * denom;

        return nom / denom;
    }

    float GeometrySchlickGGX(float NdotV, float roughness)
    {
        float r = (roughness + 1.0f);
        float k = (r * r) / 8.0f;

        float nom = NdotV;
        float denom = NdotV * (1.0f - k) + k;

        return nom / denom;
    }

    float GeometrySmith(glm::vec3 N, glm::vec3 V, glm::vec3 L, float roughness)
    {
        float NdotV = std::max(dot(N, V), 0.0f);
        float NdotL = std::max(dot(N, L), 0.0f);
        float ggx2 = GeometrySchlickGGX(NdotV, roughness);
        float ggx1 = GeometrySchlickGGX(NdotL, roughness);

        return ggx1 * ggx2;
    }

    glm::vec3 fresnelSchlick(float cosTheta, glm::vec3 F0)
    {
        return F0 + (1.0f - F0) * pow(glm::clamp(1.0f - cosTheta, 0.0f, 1.0f), 5.0f);
    }

    //Parallax mapping, doesn't work right now
    //glm::vec2 ParallaxMapping(glm::vec2 uv, glm::vec3 viewDir)
    //{
    //    // number of depth layers
    //    const float minLayers = 8;
    //    const float maxLayers = 32;
    //    // number of depth layers
    //    float numLayers = glm::mix(maxLayers, minLayers, glm::abs(glm::dot(glm::vec3(0, 0, 1), viewDir)));
    //    // calculate the size of each layer
    //    float layerDepth = 1.0 / numLayers;
    //    // depth of current layer
    //    float currentLayerDepth = 0.0;

    //    glm::vec2 p = glm::vec2(viewDir.x, viewDir.y) / viewDir.z * heightScale;

    //    glm::vec2 deltaUV = p / numLayers;

    //    glm::vec2 currentUV = uv;

    //    float currentHeightValue = mHeight->SampleTexture(currentUV.x, currentUV.y).r;

    //    while (currentLayerDepth < currentHeightValue)
    //    {
    //        // shift texture coordinates along direction of P
    //        currentUV -= deltaUV;
    //        // get depthmap value at current texture coordinates
    //        currentHeightValue = mHeight->SampleTexture(currentUV.x, currentUV.y).r;
    //        // get depth of next layer
    //        currentLayerDepth += layerDepth;
    //    }

    //    glm::vec2 prevUV = currentUV + deltaUV;

    //    float afterDepth = currentHeightValue - currentLayerDepth;
    //    float beforeDepth = mHeight->SampleTexture(prevUV.x, prevUV.y).r - currentLayerDepth + layerDepth;

    //    float weight = afterDepth / (afterDepth - beforeDepth);

    //    glm::vec2 finalUV = prevUV * weight + currentUV * (1.0f * weight);
    //    
    //    return finalUV;
    //}

    virtual glm::vec4 vertex(glm::vec3 position, glm::vec3& uv, glm::vec3& tangent, glm::vec3& normal, int index) override
    {
        //Setup TBN variables
        Normal[index] = (N * glm::vec4(normal, 1.0f));
        Tan[index] = (N * glm::vec4(tangent, 1.0f));
        BiTan[index] = glm::cross(Normal[index], Tan[index]);
        //TBN = glm::mat3(Tan, BiTan, Normal);

        glm::vec4 v = glm::vec4(position, 1.0f);

        varyingWorld[index] = glm::vec3(M * v);

        varyingNorm[index] = N * glm::vec4(normal, 1.0f);

        varyingUV[index] = uv;

        return MVP * v;
    }



    virtual glm::vec3 fragment(glm::vec3 bar) override
    {
        //Interpolate UV's
        float u = (varyingUV[0].x * bar.x) + (varyingUV[1].x * bar.y) + (varyingUV[2].x * bar.z);
        float v = (varyingUV[0].y * bar.x) + (varyingUV[1].y * bar.y) + (varyingUV[2].y * bar.z);

        //Interpolate the tangent, bitangent and normals
        glm::vec3 tangent = glm::normalize((Tan[0] * bar.x) + (Tan[1] * bar.y) + (Tan[2] * bar.z));
        glm::vec3 normal = glm::normalize((Normal[0] * bar.x) + (Normal[1] * bar.y) + (Normal[2] * bar.z));
        glm::vec3 bitangent = glm::normalize((BiTan[0] * bar.x) + (BiTan[1] * bar.y) + (BiTan[2] * bar.z));

        TBN = glm::mat3(tangent, bitangent, normal);

        //Interpolate fragment position
        glm::vec3 fragPos = glm::vec3(varyingWorld[0] * bar.x + varyingWorld[1] * bar.y + varyingWorld[2] * bar.z);

        glm::vec3 viewDir = glm::normalize(cameraPos - fragPos);


        //TODO: fix parallax mapping
        //glm::vec2 parallaxUV = ParallaxMapping(glm::vec2(u, v), viewDir);

        // Sample the albedo and normal textures
        glm::vec3 albedo = mAlbedo->SampleTexture(u,v);
        glm::vec3 tNormal = mNormal->SampleTexture(u, v);
        float ao = mAO->SampleTexture(u, v).r;
        float metalness = mMetal->SampleTexture(u, v).r;
        float roughness = mRough->SampleTexture(u, v).r;
        //Correct normal values, not used because I think it looks a bit different
        //tNormal = (tNormal * 2.0f - 1.0f);
        tNormal = glm::normalize(TBN * tNormal);


        //Corrects the F0 between metal and Dielectric
        //derived from Angel Ortiz's software renderer
        glm::vec3 F0 = glm::vec3(0.04f);
        F0 = glm::mix(F0, albedo, metalness);
        float invMetal = (1.0f - metalness);
        glm::vec3 F0Correct = (F0 * invMetal) + (albedo * metalness);
        glm::vec3 Lo = glm::vec3(0.0f);

        float constant = 1.0f;
        float linear = 0.09f;
        float quadratic = 0.032f;

        //Apply PBR shading for each light in the lights vector
        //Could make this SIMD if I felt there was a need to, however
        //I think my current performance is satisfactory for now.
        for (int i = 0; i < lights.size(); i++)
        {

            //Retrieve data from the lights vector
            lightPosition = lights[i]->position;
            intensity = lights[i]->intensity;
            lightColor = lights[i]->color;

            //Light direction
            glm::vec3 L = glm::normalize(lightPosition - fragPos);
            //Calculating half-vector
            glm::vec3 H = glm::normalize(viewDir + L);

            //Calculate dot product between normal and light direction
            float NdotL = std::max(glm::dot(tNormal, L), 0.0f);

            //Calculating the attenuation for the point lights in the scene
            float distance = glm::length(lightPosition - fragPos);
            float attenuation = 1.0f / (constant + linear * distance + quadratic * (distance * distance));
            glm::vec3 radiance = lightColor * attenuation;

            //BDRF Cook-Torrance
            float NDF = DistributionGGX(tNormal, H, roughness);
            float G = GeometrySmith(tNormal, viewDir, L, roughness);
            glm::vec3 F = fresnelSchlick(std::max(glm::dot(H, viewDir), 0.0f), F0Correct);

            glm::vec3 numerator = NDF * G * F;
            float denominator = 4.0f * std::max(glm::dot(tNormal, viewDir), 0.0f) * std::max(glm::dot(tNormal, L), 0.0f) + 0.0001f;
            glm::vec3 specular = (numerator / denominator);

            glm::vec3 kS = F;
            glm::vec3 kD = (glm::vec3(1.0f) - kS);

            kD *= invMetal;

            Lo += (kD * albedo / PI + specular) * NdotL * radiance * intensity;
        }

        //Simple ambient term
        glm::vec3 ambient = glm::vec3(0.1f) * albedo;

        glm::vec3 color = Lo + ambient;

        //Correct HDR colours
        color = color / (color + glm::vec3(1.0f));
         
        return color;
    }
};
#endif
