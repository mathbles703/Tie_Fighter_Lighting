#ifndef _cLightDesc_HG_
#define _cLightDesc_HG_

// theLights[x].position
#include <glm/glm.hpp>		// Basic types

struct cLightDesc
{
	cLightDesc();
	glm::vec4 position;
	glm::vec4 ambient;	
	glm::vec4 diffuse;
	glm::vec4 specular;
	float attenConst;	// = 0.1f;
	float attenLinear;	// = 0.1f;
	float attenQuad;	// = 0.1f;
	int lightType;   // 0=point, 1=spot, 2=directional

	// Nasty OpenGL stuff 
	int UniLoc_position;		// GLint
	int UniLoc_ambient;
	int UniLoc_diffuse;
	int UniLoc_specular;
	int UniLoc_attenConst;
	int UniLoc_attenLinear;
	int UniLoc_attenQuad;
	int UniLoc_type;

	// 0.25 passed in, returns some distance
	static const float DEFAULTMAXDISTANCE;// = 100.0f;
	float calcDistanceAtBrightness( float brightness, float maxDistance = DEFAULTMAXDISTANCE );
	enum enumCalcMethod
	{
		USE_LINEAR,
		USE_NEWTON_RAPHSON
	};
	enumCalcMethod BrightnessSearchMethod;
	float calcAttenuation(float dist);
	float distSearchEpsilon;
	static const float DEFAULTDISTANCESEARCHEPSILON;// = 0.01f;

private:
	float m_calcDistanceAtBrightnessLinear( float brightness, float maxDistance );
	float m_calcDistanceAtBrightnessNR( float brightness, float maxDistance );
};


#endif
