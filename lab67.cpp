#include "lab67.h"

// HUD globals
HUD hud;

// Parameters from xml
float sphereDiameter = 0.4f;
float sphereDiameterTolerance = 0.4f;
float innerCylinderD = 3.0f;
float outerCylinderD = 6.0f;
float cylinderH = 6.0f;
float cylindersDelta = 3.0f;
float particlesH = 6.0f;
float scaleFactor = 10.0f;
int maxParticles = 512;
int discreteK = 8;

// Physics SDK globals
NxPhysicsSDK* gPhysicsSDK = NULL;
NxScene* gScene = NULL;
NxVec3            gDefaultGravity(0, -9.8, 0);
UserAllocator* gAllocator;

// Display globals
int gMainHandle;
int mx = 0;
int my = 0;

// Camera globals
float	gCameraAspectRatio = 1;
NxVec3	gCameraPos(0, 5, -15);
NxVec3	gCameraForward(0, 0, 1);
NxVec3	gCameraRight(-1, 0, 0);
const NxReal gCameraSpeed = 10;


// Keyboard globals
#define MAX_KEYS 256
bool gKeys[MAX_KEYS];

// Simulation globals
NxReal gDeltaTime = 1.0 / 60.0;
bool bHardwareScene = true;
bool bPause = false;
bool bShadows = true;

// Actor globals
NxActor* groundPlane = NULL;

// Fluid
NxFluidEmitter* fluidEmitter = 0;

// Water modes
enum
{
	Particles,
	GenMesh,
	StandartMesh
};
int WaterMode = StandartMesh;

void ClearFilter()
{
	NxU32 nbActors = gScene->getNbActors();
	NxActor** actors = gScene->getActors();
	while (nbActors--)
	{
		NxActor* actor = *actors++;
		NxVec3 pos = actor->getGlobalPosition();
		if (NxVec3(pos.x, 0, pos.z).magnitude() > (outerCylinderD - sphereDiameter) / 2 ||
			pos.y > cylinderH)
		{
			gScene->releaseActor(*actor);
		}
		else
		{
			if (actor->isSleeping())
				actor->wakeUp();
			else
				actor->putToSleep();
		}
	}
}

void PrintControls()
{
	printf("\n Flight Controls:\n ----------------\n w = forward, s = back\n a = strafe left, d = strafe right\n q = up, z = down\n");
	printf("\n Miscellaneous:\n --------------\n p   = Pause\n x   = Toggle Shadows\n F10 = Reset Scene \n Esc = exit \n");
	printf("\n Water Filter:\n --------------\n space = start/stop granules and clear extra granules");
	printf("\n Sphere modes:\n --------------\n 1 - particles, 2 - generated mesh, 3 - standart mesh");
}

void ProcessCameraKeys()
{
	NxReal deltaTime;

	if (bPause) deltaTime = 0.0005; else deltaTime = gDeltaTime;

	// Process camera keys
	for (int i = 0; i < MAX_KEYS; i++)
	{
		if (!gKeys[i])
		{
			continue;
		}

		switch (i)
		{
			// Camera controls
		case 'w': { gCameraPos += gCameraForward * gCameraSpeed * deltaTime; break; }
		case 's': { gCameraPos -= gCameraForward * gCameraSpeed * deltaTime; break; }
		case 'a': { gCameraPos -= gCameraRight * gCameraSpeed * deltaTime; break; }
		case 'd': { gCameraPos += gCameraRight * gCameraSpeed * deltaTime; break; }
		case 'z': { gCameraPos -= NxVec3(0, 1, 0) * gCameraSpeed * deltaTime; break; }
		case 'q': { gCameraPos += NxVec3(0, 1, 0) * gCameraSpeed * deltaTime; break; }
		}
	}
}

void SetupCamera()
{
	gCameraAspectRatio = (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT);

	// Setup camera
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, gCameraAspectRatio, 1.0f, 10000.0f);
	gluLookAt(
		gCameraPos.x, gCameraPos.y, gCameraPos.z, 
		gCameraPos.x + gCameraForward.x, gCameraPos.y + gCameraForward.y, gCameraPos.z + gCameraForward.z, 
		0.0f, 1.0f, 0.0f
	);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void RenderActors(bool shadows)
{
	// Render all the actors in the scene
	NxU32 nbActors = gScene->getNbActors();
	NxActor** actors = gScene->getActors();
	while (nbActors--)
	{
		NxActor* actor = *actors++;
		DrawActor(actor, NULL, false);
		// Handle shadows
		if (shadows)
		{
			DrawActorShadow(actor, false);
		}
	}
}

void RenderCallback()
{
	// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ProcessCameraKeys();
	SetupCamera();

	if (gScene && !bPause)
	{
		GetPhysicsResults();
		StartPhysics();
	}

	// Display scene
	RenderActors(bShadows);

	if(WaterMode == Particles)
		RenderFluid();

	// Render the HUD
	hud.Render();

	glFlush();
	glutSwapBuffers();
}

void ReshapeCallback(int width, int height)
{
	glViewport(0, 0, width, height);
	gCameraAspectRatio = float(width) / float(height);
}

void IdleCallback()
{
	glutPostRedisplay();
}

void KeyboardCallback(unsigned char key, int x, int y)
{
	gKeys[key] = true;
}

void KeyboardUpCallback(unsigned char key, int x, int y)
{
	gKeys[key] = false;

	switch (key)
	{
	case 'p': {
		bPause = !bPause;
		getElapsedTime();
		break;
	}
	case 'x': { bShadows = !bShadows; break; }
	case 27: { exit(0); break; }
	case 32: { ClearFilter(); break; }

	case '1': { WaterMode = Particles; ResetNx(); break; }
	case '2': { WaterMode = GenMesh; ResetNx(); break; }
	case '3': { WaterMode = StandartMesh; ResetNx(); break; }
	default: { break; }
	}
}

void SpecialCallback(int key, int x, int y)
{
	switch (key)
	{
		// Reset PhysX
	case GLUT_KEY_F10: ResetNx(); return;
	}
}

void MouseCallback(int button, int state, int x, int y)
{
	mx = x;
	my = y;
}

void MotionCallback(int x, int y)
{
	int dx = mx - x;
	int dy = my - y;

	gCameraForward.normalize();
	gCameraRight.cross(gCameraForward, NxVec3(0, 1, 0));

	NxQuat qx(NxPiF32 * dx * 20 / 180.0f, NxVec3(0, 1, 0));
	qx.rotate(gCameraForward);
	NxQuat qy(NxPiF32 * dy * 20 / 180.0f, gCameraRight);
	qy.rotate(gCameraForward);

	mx = x;
	my = y;
}

void ExitCallback()
{
	ReleaseNx();
}

void InitGlut(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(800, 600);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	gMainHandle = glutCreateWindow("Laboratory work 6-7");
	glutSetWindow(gMainHandle);
	glutDisplayFunc(RenderCallback);
	glutReshapeFunc(ReshapeCallback);
	glutIdleFunc(IdleCallback);
	glutKeyboardFunc(KeyboardCallback);
	glutKeyboardUpFunc(KeyboardUpCallback);
	glutSpecialFunc(SpecialCallback);
	glutMouseFunc(MouseCallback);
	glutMotionFunc(MotionCallback);
	MotionCallback(0, 0);
	atexit(ExitCallback);

	// Setup default render states
	glClearColor(0.0f, 0.0f, 0.0f, 1.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_CULL_FACE);

	// Setup lighting
	glEnable(GL_LIGHTING);
	float AmbientColor[] = { 0.0f, 0.1f, 0.2f, 0.0f };         glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientColor);
	float DiffuseColor[] = { 0.2f, 0.2f, 0.2f, 0.0f };         glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseColor);
	float SpecularColor[] = { 0.5f, 0.5f, 0.5f, 0.0f };         glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularColor);
	float Position[] = { 100.0f, 100.0f, -400.0f, 1.0f };  glLightfv(GL_LIGHT0, GL_POSITION, Position);
	glEnable(GL_LIGHT0);
}


void CreateFilter()
{
	float oCylR = outerCylinderD / 2;
	float iCylR = innerCylinderD / 2;


	NxActor* oCylinder = CreateOuterCylinder(oCylR, cylinderH, discreteK);
	NxActor* iCylinder = CreateInnerCylinder(innerCylinderD / 2, cylinderH - cylindersDelta, discreteK);
	iCylinder->setGlobalPosition(NxVec3(0, cylindersDelta, 0));


	if (WaterMode == Particles)
	{
		fluidEmitter = CreateFluidEmitter(NxVec3(0.0, cylinderH * 1.5f, 0.0f), oCylR, oCylR);
	}
	else
	{

		float gridStep = sphereDiameter + sphereDiameterTolerance;
		int nElem = (int)2 * (outerCylinderD / gridStep) + (int)(cylinderH / gridStep);
		float waterH = particlesH;

		for (float x = -oCylR; x <= oCylR; x += gridStep)
		{
			for (float z = -oCylR; z <= oCylR; z += gridStep)
			{
				for (float y = 0; y <= waterH; y += gridStep)
				{
					NxActor* wSphere;
					float dr = rand() % 1024 * sphereDiameterTolerance / 2048;

					if (WaterMode == GenMesh)
						wSphere = CreateWaterSphere(sphereDiameter / 2 + dr, discreteK);
					else
						wSphere = CreateSphere(sphereDiameter / 2 + dr);


					wSphere->setGlobalPosition(NxVec3(x, y + cylinderH * 1.5f, z));
				}
			}
		}
	}

}

void InitNx()
{
	srand(getCurrentTime());

	LoadParameters();


	// Initialize Camera Parameters
	gCameraAspectRatio = 1.0f;
	gCameraPos = NxVec3(0, 5, -15);
	gCameraForward = NxVec3(0, 0, 1);
	gCameraRight = NxVec3(-1, 0, 0);

	// Create a memory allocator
	if (!gAllocator)
	{
		gAllocator = new UserAllocator;
	}

	// Create the physics SDK
	gPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, gAllocator);
	if (!gPhysicsSDK)  return;

	// Set the physics parameters
	gPhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.01);

	// Set the debug visualization parameters

	// Create the scene
	NxSceneDesc sceneDesc;
	sceneDesc.simType = NX_SIMULATION_SW;
	sceneDesc.gravity = gDefaultGravity;
	gScene = gPhysicsSDK->createScene(sceneDesc);
	if (!gScene)
	{
		sceneDesc.simType = NX_SIMULATION_SW;
		gScene = gPhysicsSDK->createScene(sceneDesc);
		if (!gScene) return;
	}


	// Create the default material
	NxMaterial* defaultMaterial = gScene->getMaterialFromIndex(0);
	defaultMaterial->setRestitution(0.5);
	defaultMaterial->setStaticFriction(0.5);
	defaultMaterial->setDynamicFriction(0.5);


	// Create the objects in the scene
	groundPlane = CreateGroundPlane();


	bPause = true;
	CreateFilter();

	//Initialize HUD
	InitializeHUD();

	// Get the current time
	getElapsedTime();

	// Start the first frame of the simulation
	if (gScene)  StartPhysics();
}



void ReleaseNx()
{
	GetPhysicsResults();  // Make sure to fetchResults() before shutting down  

	ReleaseFluid();

	if (gScene)
	{
		gPhysicsSDK->releaseScene(*gScene);
	}
	if (gPhysicsSDK)  gPhysicsSDK->release();
}

void ResetNx()
{
	ReleaseNx();
	InitNx();
}

void StartPhysics()
{
	// Update the time step
	gDeltaTime = getElapsedTime();

	// Start collision and dynamics for delta time since the last frame
	gScene->simulate(gDeltaTime);
	gScene->flushStream();
}

void GetPhysicsResults()
{ 
	// Get results from gScene->simulate(gDeltaTime)
	while (!gScene->fetchResults(NX_RIGID_BODY_FINISHED, false));
}

void InitializeHUD()
{
	bHardwareScene = (gScene->getSimType() == NX_SIMULATION_HW);

	hud.Clear();
	hud.AddDisplayString((char*)("Laboratory work 6-7 \nMade by student from RK6-51B \nYokubauskas Dmitry"), 0.01f, 0.1f);
}

int main(int argc, char** argv)
{
	PrintControls();
	InitGlut(argc, argv);
	InitNx();
	glutMainLoop();
	ReleaseNx();
	return 0;
}
