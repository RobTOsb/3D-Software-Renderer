#ifndef LIGHTS_H
#define LIGHTS_H
#include <string>
#include "SDL.h"
#include "glm/glm.hpp"
#include <iostream>

struct Lights
{
	Lights(glm::vec3 position, float intensity, glm::vec3 color) : position(position), intensity(intensity), color(color) {};

	~Lights() = default;

    void Update(float delta)
    {
        totalTime += delta; // Update totalTime with the passed delta
        float ang = totalTime * time;

        float radius = 4.0f; // Set the desired radius of the circle around the model
        glm::vec3 modelCenter = glm::vec3(0.0f, 0.0f, 0.0f); // Set the center of the model

        // Update position.x and position.z using the calculated angle and radius
        position.x = modelCenter.x + radius * sin(ang);
        //position.y = modelCenter.y + radius * sin(ang);
        position.z = modelCenter.z + radius * cos(ang);
    }



	glm::vec3 position;
	float intensity;
	glm::vec3 color;
	float time = 0.0005f;
	Uint32 totalTime = 0;
};

#endif

