//Class for loading the mesh data and the functions for retrieving that
//data. Also has all the textures associated with the model.


#ifndef MODEL_H
#define MODEL_H

#include "texture.h"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include <vector>
#include <string>


struct AABB
{
	glm::vec3 min;
	glm::vec3 max;
};


//I previously held all the data in a vector of triangle structs which contained 3 of each of the vertices
//However, I found this to have enough of an impact on performance and it complicated things too much
//so I separated everything out into their own vectors and it increased performance by roughly 1.5x.
class Model
{
public:
	Model();
	~Model();

	bool loadModel(const std::string& filename);

	std::vector<glm::vec3>* getVertices() { return &mVertices; }
	std::vector<glm::vec3>* getNormals() { return &mNormals; }
	std::vector<glm::vec3>* getUV() { return &mUVs; }
	std::vector<glm::vec3>* getTangent() { return &mTangents; }
	std::vector<glm::vec3>* getFaceNormals() { return &mFaceNormals; }

	std::vector<glm::ivec3>* getVertexIndices() { return &mVertexIndices; }
	std::vector<glm::ivec3>* getTextureIndices() { return &mTextureIndices; }
	std::vector<glm::ivec3>* getNormalIndices() { return &mNormalIndices; }


	void CalculateTangent();

	void SetTranslate(glm::vec3 translate);
	void SetRotation(glm::vec3 rotation);
	void SetScale(glm::vec3 scale);
	void SetModelMatrix();

	glm::mat4 GetModelMatrix();

	AABB mBoundingBox;

	Texture mAlbedo;
	Texture mNormal;
	Texture mMetallic;
	Texture mRoughness;
	Texture mAmbientO;
	Texture mHeight;

	int mNumFaces = 0;

	glm::vec3 rotate;

private:
	
	Uint32 totalTime;

	std::vector<glm::vec3> mVertices;
	std::vector<glm::vec3> mNormals;
	std::vector<glm::vec3> mUVs;
	std::vector<glm::vec3> mTangents;
	std::vector<glm::vec3> mBiTangents;

	
	std::vector<glm::vec3> mFaceNormals;
	std::vector<glm::ivec3> mVertexIndices;
	std::vector<glm::ivec3> mTextureIndices;
	std::vector<glm::ivec3> mNormalIndices;


	glm::mat4 mModelMatrix = glm::mat4(1.0f);
	glm::mat4 mTranslate;
	glm::mat4 mRotation;
	glm::mat4 mScale;
};
#endif