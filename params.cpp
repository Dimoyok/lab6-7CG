#include "lab67.h"

extern float sphereDiameter;
extern float sphereDiameterTolerance;
extern float innerCylinderD;
extern float outerCylinderD;
extern float cylinderH;
extern float cylindersDelta;
extern float particlesH;
extern float scaleFactor;
extern int maxParticles;
extern int discreteK;

void GetParameter(TiXmlDocument& doc, float& param, const char* pName)
{
	TiXmlNode* node;
	if ((node = doc.FirstChild(pName)) == NULL)
	{
		printf("File don't have \"%s\" parameter!", pName);
		return;
	}

	if ((node = node->FirstChild()) == NULL)
	{
		printf("Parameter \"%s\" don't have value!", pName);
		return;
	}

	param = atof(node->Value());
}


void GetParameter(TiXmlDocument& doc, int& param, const char* pName)
{
	TiXmlNode* node;
	if ((node = doc.FirstChild(pName)) == NULL)
	{
		printf("File don't have \"%s\" parameter!", pName);
		return;
	}

	if ((node = node->FirstChild()) == NULL)
	{
		printf("Parameter \"%s\" don't have value!", pName);
		return;
	}

	param = atoi(node->Value());
}

void LoadParameters()
{
	TiXmlDocument doc("lab67.xml");
	if (!doc.LoadFile())
	{
		puts("\nerror loading xml!\n");
	}
	else
	{
		GetParameter(doc, sphereDiameter, "sphereDiameter");
		GetParameter(doc, sphereDiameterTolerance, "sphereDiameterTolerance");
		GetParameter(doc, innerCylinderD, "innerCylinderD");
		GetParameter(doc, outerCylinderD, "outerCylinderD");
		GetParameter(doc, cylinderH, "cylinderH");
		GetParameter(doc, cylindersDelta, "cylindersDelta");

		GetParameter(doc, particlesH, "particlesH");
		GetParameter(doc, scaleFactor, "scaleFactor");
		GetParameter(doc, maxParticles, "maxParticles");
		GetParameter(doc, discreteK, "discreteK");


	}
}