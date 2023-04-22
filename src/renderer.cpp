#include "renderer.h"
#include "drawing.h"
#include "SDL_image.h"
#include <glm/gtc/matrix_inverse.hpp>
#include "omp.h"

//Dummy constructor and destructor
Renderer::Renderer(){}
Renderer::~Renderer(){}

bool Renderer::StartUp(const int w, const int h)
{
	bool success = true;

	int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
	if (!(IMG_Init(imgFlags) & imgFlags)) 
	{
		// Handle the error
		SDL_Log("Failed to initialize SDL_image: %s", IMG_GetError());
		success = false;
		return success;
	}

	mPixelBuffer = new Uint32[w * h];

	mZbuffer = new float[w * h];

	return success;
}

void Renderer::render()
{
	bufferClear();

	PBRShader shader;

	for (auto& model : mCurrentScene.getScene())
	{
		AABB transformedBB;

		//Vertices
		std::vector<glm::vec3>* vertices = model->getVertices();
		std::vector<glm::ivec3>* vertexIndices = model->getVertexIndices();

		//Texture coordinates
		std::vector<glm::vec3>* UVs = model->getUV();
		std::vector<glm::ivec3>* textureIndices = model->getTextureIndices();

		//Normals
		std::vector<glm::vec3>* normals = model->getNormals();

		std::vector<glm::ivec3>* normalIndices = model->getNormalIndices();

		//Face normals
		std::vector<glm::vec3>* faceNormals = model->getFaceNormals();

		//Tangents
		std::vector<glm::vec3>* tangents = model->getTangent();

		shader.cameraPos = getSceneCamera()->position;
		
		//Shader textures
		shader.mAlbedo = &model->mAlbedo;
		shader.mAO = &model->mAmbientO;
		shader.mMetal = &model->mMetallic;
		shader.mRough = &model->mRoughness;
		shader.mNormal = &model->mNormal;
		shader.mHeight = &model->mHeight;

		//Shader matrices
		shader.MV = getSceneCamera()->getView() * model->GetModelMatrix();
		shader.MVP = getSceneCamera()->getPerspective() * shader.MV;
		shader.M = model->GetModelMatrix();
		shader.N = glm::transpose(glm::inverse(shader.M));

		glm::mat4 worldToObject = glm::inverse(shader.M);

		//Shader Lights
		shader.lights = mCurrentScene.getLights();

		std::vector<Lights*> lights = mCurrentScene.getLights();
		
		#pragma omp parallel for firstprivate(shader) schedule(dynamic)
		for (int i = 0; i < model->mNumFaces; ++i)
		{
			glm::vec3 triangleVertex[3], triangleNormal[3], triangleTexture[3], triangleTangent[3];

			glm::ivec3 faceV = (*vertexIndices)[i];
			glm::ivec3 faceN = (*normalIndices)[i];
			glm::ivec3 faceT = (*textureIndices)[i];


			//Pack data into triangles
			for (int j = 0; j < 3; j++)
			{
				triangleVertex[j] = (*vertices)[faceV[j]];
				triangleNormal[j] = (*normals)[faceN[j]];
				triangleTexture[j] = (*UVs)[faceT[j]];
				triangleTangent[j] = (*tangents)[faceV[j]];
			}

			if (BackfaceCulling((*faceNormals)[i], triangleVertex[0], worldToObject))
			{
				continue;
			}

			glm::vec4 clipTri[3];

			for (int j = 0; j < 3; j++)
			{
				clipTri[j] = shader.vertex(triangleVertex[j], triangleTexture[j], triangleTangent[j], triangleNormal[j], j);
			}
			
			Drawing::DrawTriangle(mPixelBuffer, clipTri, shader, mZbuffer);
		}
	}
}

void Renderer::bufferClear()
{
	for (int i = 0; i < Display::SCREENWIDTH * Display::SCREENHEIGHT; i++)
	{
		mPixelBuffer[i] = 0;
	}

	for (int i = 0; i < Display::SCREENWIDTH * Display::SCREENHEIGHT; i++)
	{
		mZbuffer[i] = 1.0f;
	}
}

void Renderer::loadScene(std::string& filePath)
{
	mCurrentScene.loadScene(filePath);
}

void Renderer::clearScene()
{
	mCurrentScene.clearScene();
}

bool Renderer::BackfaceCulling(glm::vec3& faceNorm, glm::vec3& vert, glm::mat4& worldToObject)
{
	glm::vec3 viewDir = glm::vec3(worldToObject * glm::vec4(getSceneCamera()->position, 1.0f)) - vert;
	viewDir = glm::normalize(viewDir);

	float intensity = glm::dot(faceNorm, viewDir);

	return intensity <= 0.0f;

}

// GETTERS AND SETTERS

Uint32* Renderer::getPixelBuffer()
{
	return mPixelBuffer;
}

//Deletes the buffers
bool Renderer::ShutDown() 
{ 
	delete[] mPixelBuffer;

	delete[] mZbuffer;

	return true; 
}