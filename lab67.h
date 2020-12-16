

#ifndef lab67
#define lab67

#include "GL/glut.h"

#include <stdio.h>

#include "HUD.H"

#include "Timing.h"
#include "NxPhysics.h"
#include "DrawObjects.h"

#include "Stream.h"
#include "NxCooking.h"

#include "tinyxml.h"
#include "tinystr.h"
#include "UserAllocator.h"


NxActor* CreateGroundPlane();
NxActor* CreateOuterCylinder(const float r, const float h, const int k);
NxActor* CreateInnerCylinder(const float r, const float h, const int k);
NxActor* CreateWaterSphere(const float r, const int k);
NxActor* CreateSphere(const float r);
void CreateFilter();

NxFluid* CreateFluid();
void ReleaseFluid();
void RenderFluid();
NxFluidEmitter* CreateFluidEmitter(const NxVec3 pos, const NxReal dimX, const NxReal dimY);


void LoadParameters();
void GetParameter(TiXmlDocument& doc, float& param, const char* pName);

void PrintControls();
void ProcessCameraKeys();
void SetupCamera();
void RenderActors(bool shadows);

void RenderCallback();
void ReshapeCallback(int width, int height);
void IdleCallback();
void KeyboardCallback(unsigned char key, int x, int y);
void KeyboardUpCallback(unsigned char key, int x, int y);
void SpecialCallback(int key, int x, int y);
void MouseCallback(int button, int state, int x, int y);
void MotionCallback(int x, int y);
void ExitCallback();

void ClearFilter();
void InitGlut(int argc, char** argv);

void InitializeHUD();

void InitNx();
void ReleaseNx();
void ResetNx();

void StartPhysics();
void GetPhysicsResults();

int main(int argc, char** argv);


#endif  // LESSON102_H
