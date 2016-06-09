#include "cLightDesc.h"

cLightDesc::cLightDesc()
{
	this->attenConst = 0.0f;
	this->attenLinear = 0.1f;
	this->attenQuad = 0.1f;		// Why not?

	this->lightType = 0;	

	this->BrightnessSearchMethod = cLightDesc::USE_LINEAR;	
	this->distSearchEpsilon = cLightDesc::DEFAULTDISTANCESEARCHEPSILON;

	return;
}

//static
const float cLightDesc::DEFAULTMAXDISTANCE = 100.0f;

//static
const float cLightDesc::DEFAULTDISTANCESEARCHEPSILON = 0.0001f;


// 0.25 passed in, returns some distance
float cLightDesc::calcDistanceAtBrightness( float brightness, float maxDistance )	// 0.5f;
{
	switch ( this->BrightnessSearchMethod )
	{
	case cLightDesc::USE_LINEAR:
		return this->m_calcDistanceAtBrightnessLinear(brightness, maxDistance);
		break;
	case cLightDesc::USE_NEWTON_RAPHSON:
		return this->m_calcDistanceAtBrightnessNR(brightness, maxDistance );
		break;
	}
	// This really "shouldn't" happen... 
	return 0.0f;
}


float cLightDesc::m_calcDistanceAtBrightnessLinear( float brightness, float maxDistance )
{
	//const float epsilon = 0.01f;
	int iterations = 0;
	// 6 million times a second... 
	float step = this->distSearchEpsilon / 10.0f;
	for ( float x = 0.0f; x < maxDistance; x += step, iterations++ )			// 5000x
	{
		float curAtten = calcAttenuation(x);

		// is it within a certain range of values
		if ( curAtten >= (brightness-this->distSearchEpsilon) && 		// 0.5-0.01, 4.99f
			 curAtten <= (brightness+this->distSearchEpsilon) )         // 0.5+0.01, 5.01f
		{
			return x;
		}
	}
	// If we get here, return something (zero?)
	return 0.0f;
}

float cLightDesc::m_calcDistanceAtBrightnessNR( float brightness, float maxDistance )
{
	//const float epsilon = 0.01f;

	// Set the maximum and minimum distances we've calculated
	// At the start, we pick the entire range
	float distCalcMaxSoFar = maxDistance;
	float distCalcMinSoFar = 0.0f;


	// This will ensure we only do a maximum of 100 iterations 
	// (which is WAY to many) keeping us from an infinite loop
	int watchDogCountdown = 100;
	while ( watchDogCountdown > 0 )
	{
		watchDogCountdown--;

		// Make a reasonable guess for distance
		// This is one of the key steps. 
		// Note we are taking the half the distance between the mid and max values, 
		//  then adding it to the min distance. Effectively, we are guessing 
		//  half way between the last closest guesses, zeroing in on the actual value.
		float testDistance = ((distCalcMaxSoFar - distCalcMinSoFar) / 2.0f) + distCalcMinSoFar;

		float curAtten = calcAttenuation(testDistance);

		// Too small (i.e. too dark or to FAR away)? 
		if ( curAtten < (brightness-this->distSearchEpsilon) )
		{	// We are too far away from the light. 
			// Set the last "max" value to the current test distance
			distCalcMaxSoFar = testDistance;
		}
		// Too large (i.e. to bright or to CLOSE)?
		else if ( curAtten > (brightness+this->distSearchEpsilon) )
		{	// We are too close to the light.
			// Set the last "min" value to the current test distance
			distCalcMinSoFar = testDistance;
		}
		else
		{	// we nailed it. Not to bright and not too dark
			return testDistance;
		}
	};//	while ( watchDogCountdown > 0 )

	// We would only get here if the watchdog failed
	// (likely we have a REALLY bright light and the distance isn't big enough)
	// If we get here, return something (zero?)
	return 0.0f;
}


float cLightDesc::calcAttenuation(float dist)
{
	// Taken right from the shader
	//float atten = 1.0f /( theLights[lightID].attenConst + 
	//                      theLights[lightID].attenLinear * dist +
	//					  theLights[lightID].attenQuad * dist * dist );
	float atten = 1.0f /( this->attenConst + 
	                      this->attenLinear * dist +
						  this->attenQuad * dist * dist );

	return atten;
}