#ifndef _cGameObject_HG_
#define _cGameObject_HG_

#include <glm/glm.hpp>	// 
#include <string>

// To simplify, this is also a "mesh"
class cGameObject
{
public:
	cGameObject() : 
		cached_VBO_ID(-1), 
		scale(1.0f),
		bUseDebugColour(false),
		bIsWireframe(false), 
		shininess(1.0f), 
		bIsADebugObject(false)
	{};
	glm::vec3 position;
	glm::vec3 preRotation;
	glm::vec3 postRotation;
	float scale;	
	//glm::vec4 colour;
	glm::vec4 debugColour;
	bool bUseDebugColour;
	bool bIsWireframe;

	glm::vec4 diffuse;
	glm::vec4 ambient;
	glm::vec3 specular;
	float shininess;

	// Added
	std::string modelName;		// File name
	int cached_VBO_ID;// = -1;	// Huh??
	unsigned int numberOfTriangles;

	bool bIsADebugObject;

	// More physics things
	glm::vec3 velocity;
	glm::vec3 accel;		// acceleration

	void changePostPosition(void)
	{
		for(int i=0;i<1000;i++)
			this->position.x = this->position.x - 0.05f;
	}
};

#endif
