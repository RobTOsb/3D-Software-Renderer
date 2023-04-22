#include "model.h"
#include <iostream>
#define TINYOBJLOADER_IMPLEMENTATION

#include "tiny_obj_loader.h"

Model::Model(){}

Model::~Model(){}

//Tinyobjloader to load the mesh data

bool Model::loadModel(const std::string& filename)
{
    std::string input = filename;

    tinyobj::ObjReaderConfig reader_config;
    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(input, reader_config))
    {
        if (!reader.Error().empty())
        {
            std::cerr << "TinyObjReader: " << reader.Error();
        }
        return false;
    }

    if (!reader.Warning().empty())
    {
        std::cout << "TinyObjReader: " << reader.Warning();
    }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();

    // Store unique vertices, UVs, and normals
    for (size_t i = 0; i < attrib.vertices.size(); i += 3)
    {
        mVertices.push_back(glm::vec3(attrib.vertices[i], attrib.vertices[i + 1], attrib.vertices[i + 2]));
    }

    for (size_t i = 0; i < attrib.texcoords.size(); i += 2)
    {
        mUVs.push_back(glm::vec3(attrib.texcoords[i], attrib.texcoords[i + 1], 0.0f));
    }

    for (size_t i = 0; i < attrib.normals.size(); i += 3)
    {
        mNormals.push_back(glm::vec3(attrib.normals[i], attrib.normals[i + 1], attrib.normals[i + 2]));
    }

    for (size_t s = 0; s < shapes.size(); s++)
    {
        size_t index_offset = 0;

        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
        {
            int fv = shapes[s].mesh.num_face_vertices[f];

            if (fv != 3)
            {
                continue;
            }

            glm::ivec3 faceVertexIndices;
            glm::ivec3 faceTextureIndices;
            glm::ivec3 faceNormalIndices;

            for (size_t v = 0; v < 3; v++)
            {
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

                faceVertexIndices[v] = idx.vertex_index;
                faceTextureIndices[v] = idx.texcoord_index;
                faceNormalIndices[v] = idx.normal_index;
            }

            mVertexIndices.push_back(faceVertexIndices);
            mTextureIndices.push_back(faceTextureIndices);
            mNormalIndices.push_back(faceNormalIndices);

            index_offset += fv;
            mNumFaces++;
        }
    }

    //Calculate face normals for back face culling
    for (int i = 0; i < mNumFaces; ++i) 
    {
        glm::ivec3 faceVertexIndices = mVertexIndices[i];

        int index0 = faceVertexIndices.x;
        int index1 = faceVertexIndices.y;
        int index2 = faceVertexIndices.z;

        glm::vec3 v0 = mVertices[index0];
        glm::vec3 v1 = mVertices[index1];
        glm::vec3 v2 = mVertices[index2];

        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;

        glm::vec3 faceNormal = glm::normalize(glm::cross(edge1, edge2));
        mFaceNormals.push_back(faceNormal);
    }

    CalculateTangent();

    std::cout << "V: " << mVertices.size() << " F: " << mNumFaces << " UV: " << mUVs.size() << " FN: " << mFaceNormals.size() << "\n";

    return true;
}

//Calculates the tangent space of the model
//Derived from Angelo Ortiz's software renderer
//I tried several other ways, but his way works the best
void Model::CalculateTangent()
{
    std::vector<std::vector<glm::vec3>> tempTangents(mVertices.size());
    std::vector<std::vector<glm::vec3>> tempBiTangents(mVertices.size());
    glm::vec3 tangent, biTangent;

    for (int i = 0; i < mNumFaces; ++i) {
        glm::ivec3 vIndices = mVertexIndices[i];
        glm::ivec3 tIndices = mTextureIndices[i];

        glm::vec3 v0 = mVertices[vIndices[0]];
        glm::vec3 v1 = mVertices[vIndices[1]];
        glm::vec3 v2 = mVertices[vIndices[2]];

        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;

        glm::vec3 deltaUV1 = mUVs[tIndices[1]] - mUVs[tIndices[0]];
        glm::vec3 deltaUV2 = mUVs[tIndices[2]] - mUVs[tIndices[0]];

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent = glm::normalize(tangent);

        biTangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        biTangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        biTangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        biTangent = glm::normalize(biTangent);

        tempTangents[vIndices[0]].push_back(tangent);
        tempTangents[vIndices[1]].push_back(tangent);
        tempTangents[vIndices[2]].push_back(tangent);

        tempBiTangents[vIndices[0]].push_back(biTangent);
        tempBiTangents[vIndices[1]].push_back(biTangent);
        tempBiTangents[vIndices[2]].push_back(biTangent);

    }

    for (int j = 0; j < mVertices.size(); ++j) {
        int count1 = 0, count2 = 0;

        glm::vec3 temp1{ 0 };
        while (!tempTangents[j].empty()) {
            ++count1;
            temp1 += tempTangents[j].back();
            if (temp1.x == 0 && temp1.y == 0 && temp1.z == 0) {
                temp1 = tempTangents[j].back();
            }
            tempTangents[j].pop_back();
        }

        temp1 = temp1 * (1.0f / count1);
        mTangents.push_back(temp1);

        glm::vec3 temp2{ 0 };
        while (!tempBiTangents[j].empty()) {
            ++count2;
            temp2 += tempBiTangents[j].back();
            if (temp2.x == 0 && temp2.y == 0 && temp2.z == 0) {
                temp2 = tempBiTangents[j].back();
            }
            tempBiTangents[j].pop_back();
        }
        temp2 = temp2 * (1.0f / count2);
        mBiTangents.push_back(temp2);
    }

    //Renormalize and correct handedness of tangent/bitangent vectors
    for (int k = 0; k < mNumFaces; ++k) {
        glm::ivec3 vIndices = mVertexIndices[k];
        glm::ivec3 nIndices = mNormalIndices[k];

        //Per vertex
        glm::vec3 tangentV0 = mTangents[vIndices[0]];
        glm::vec3 tangentV1 = mTangents[vIndices[1]];
        glm::vec3 tangentV2 = mTangents[vIndices[2]];

        glm::vec3 normalV0 = mNormals[nIndices[0]];
        glm::vec3 normalV1 = mNormals[nIndices[1]];
        glm::vec3 normalV2 = mNormals[nIndices[2]];

        glm::vec3 biTangentV0 = mBiTangents[vIndices[0]];
        glm::vec3 biTangentV1 = mBiTangents[vIndices[1]];
        glm::vec3 biTangentV2 = mBiTangents[vIndices[2]];

        //Renormalizing
        tangentV0 = glm::normalize(tangentV0 - (normalV0 * glm::dot(tangentV0, normalV0)));
        tangentV1 = glm::normalize(tangentV1 - (normalV1 * glm::dot(tangentV1, normalV1)));
        tangentV2 = glm::normalize(tangentV2 - (normalV2 * glm::dot(tangentV2, normalV2)));

        mTangents[vIndices[0]] = tangentV0;
        mTangents[vIndices[1]] = tangentV1;
        mTangents[vIndices[2]] = tangentV2;

    }
}

// GETTERS AND SETTERS

//Set model matrices
void Model::SetTranslate(glm::vec3 translate)
{
    mTranslate = glm::translate(glm::mat4(1.0f), translate);
}

void Model::SetRotation(glm::vec3 rotation)
{
    rotate = rotation;

    mRotation = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    mRotation = glm::rotate(mRotation, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    mRotation = glm::rotate(mRotation, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
}

void Model::SetScale(glm::vec3 scale)
{
    mScale = glm::scale(glm::mat4(1.0f), scale);
}

void Model::SetModelMatrix()
{
    mModelMatrix = mTranslate * mRotation * mScale;
}

glm::mat4 Model::GetModelMatrix()
{
    return mModelMatrix;
}


