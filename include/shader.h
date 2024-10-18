#ifndef SHADER_H
#define SHADER_H

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "texture.h"


#define PI 3.14159265359f

struct IShader {
    virtual glm::vec4 vertex(glm::vec3 position, glm::vec3& uv, glm::vec3& tangent, glm::vec3& normal, int index) = 0;
    virtual glm::vec3 fragment(glm::vec3 bar) = 0;
};

struct DirectionalLight {
    glm::vec3 direction;
    glm::vec3 color;
    float intensity;
};

struct PBRShader : IShader {
    glm::vec3 cameraPos;

    Texture* mAlbedo;
    Texture* mNormal;
    Texture* mAO;
    Texture* mMetal;
    Texture* mRough;

    glm::vec3 varyingUV[3];
    glm::vec3 varyingNorm[3];
    glm::vec3 Tan[3];
    glm::vec3 BiTan[3];
    glm::vec3 Normal[3];
    glm::vec3 varyingWorld[3];

    glm::mat4 MV, MVP, M, V, N;
    glm::mat3 TBN;

    DirectionalLight light = { glm::vec3(0.0f, 0.0f, -0.5f), glm::vec3(1.0f), 1.0f };

    float DistributionGGX(glm::vec3 N, glm::vec3 H, float roughness) {
        float a = roughness * roughness;
        float a2 = a * a;
        float NdotH = std::max(dot(N, H), 0.0f);
        float NdotH2 = NdotH * NdotH;

        float nom = a2;
        float denom = (NdotH2 * (a2 - 1.0) + 1.0);
        denom = PI * denom * denom;

        return nom / denom;
    }

    float GeometrySchlickGGX(float NdotV, float roughness) {
        float r = (roughness + 1.0f);
        float k = (r * r) / 8.0f;

        float nom = NdotV;
        float denom = NdotV * (1.0f - k) + k;

        return nom / denom;
    }

    float GeometrySmith(glm::vec3 N, glm::vec3 V, glm::vec3 L, float roughness) {
        float NdotV = std::max(dot(N, V), 0.0f);
        float NdotL = std::max(dot(N, L), 0.0f);
        float ggx2 = GeometrySchlickGGX(NdotV, roughness);
        float ggx1 = GeometrySchlickGGX(NdotL, roughness);

        return ggx1 * ggx2;
    }

    glm::vec3 fresnelSchlick(float cosTheta, glm::vec3 F0) {
        return F0 + (1.0f - F0) * pow(1.0f - cosTheta, 5.0f);
    }

    virtual glm::vec4 vertex(glm::vec3 position, glm::vec3& uv, glm::vec3& tangent, glm::vec3& normal, int index) override {
        Normal[index] = glm::vec3(M * glm::vec4(normal, 0.0f));
        Tan[index] = glm::vec3(M * glm::vec4(tangent, 0.0f));
        BiTan[index] = glm::cross(Normal[index], Tan[index]);

        glm::vec4 v = glm::vec4(position, 1.0f);

        varyingWorld[index] = glm::vec3(M * v);
        varyingNorm[index] = glm::vec3(N * glm::vec4(normal, 1.0f));
        varyingUV[index] = uv;

        return MVP * v;
    }

    virtual glm::vec3 fragment(glm::vec3 bar) override {
        float u = (varyingUV[0].x * bar.x) + (varyingUV[1].x * bar.y) + (varyingUV[2].x * bar.z);
        float v = (varyingUV[0].y * bar.x) + (varyingUV[1].y * bar.y) + (varyingUV[2].y * bar.z);


        glm::vec3 tangent = glm::normalize(bar.x * Tan[0] + bar.y * Tan[1] + bar.z * Tan[2]);
        glm::vec3 normal = glm::normalize(bar.x * Normal[0] + bar.y * Normal[1] + bar.z * Normal[2]);
        glm::vec3 bitangent = glm::normalize(bar.x * BiTan[0] + bar.y * BiTan[1] + bar.z * BiTan[2]);

        TBN = glm::mat3(tangent, bitangent, normal);

        glm::vec3 fragPos = bar.x * varyingWorld[0] + bar.y * varyingWorld[1] + bar.z * varyingWorld[2];

        glm::vec3 viewDir = glm::normalize(cameraPos - fragPos);

        glm::vec3 albedo = mAlbedo->SampleTexture(u, v);
        glm::vec3 tNormal = mNormal->SampleTexture(u, v);
        float ao = mAO->SampleTexture(u, v).r;
        float metalness = mMetal->SampleTexture(u, v).r;
        float roughness = mRough->SampleTexture(u, v).r;

        tNormal = glm::normalize(tNormal * 2.0f - 1.0f);
        tNormal = glm::normalize(TBN * tNormal);

        glm::vec3 F0 = glm::vec3(0.04f);
        // F0 = glm::mix(F0, albedo, metalness);

        glm::vec3 Lo = glm::vec3(0.0f);

        glm::vec3 L = -light.direction;
        glm::vec3 H = glm::normalize(viewDir + L);
        float NdotL = std::max(glm::dot(tNormal, L), 0.0f);

        glm::vec3 radiance = light.color * light.intensity;

        float NDF = DistributionGGX(tNormal, H, roughness);
        float G = GeometrySmith(tNormal, viewDir, L, roughness);
        glm::vec3 F = fresnelSchlick(std::max(glm::dot(H, viewDir), 0.0f), F0);

        glm::vec3 numerator = NDF * G * F;
        float denominator = 4.0f * std::max(glm::dot(tNormal, viewDir), 0.0f) * NdotL + 0.0001f;
        glm::vec3 specular = numerator / denominator;

        glm::vec3 kS = F;
        glm::vec3 kD = (glm::vec3(1.0f) - kS) * (1.0f - metalness);

        Lo += (kD * albedo / PI + specular) * radiance * NdotL;

        glm::vec3 ambient = glm::vec3(0.03f) * albedo * ao;
        glm::vec3 color = ambient + Lo;

        // color = color / (color + glm::vec3(1.0f));
        // color = glm::pow(color, glm::vec3(1.0f / 2.2f));

        return color;
    }
};

#endif