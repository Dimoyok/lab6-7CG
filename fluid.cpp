#include "lab67.h"


// fluid

// Fluid particle globals
NxU32            gNumParticles = 0;
NxVec3* gParticles = 0;

bool bHardwareFluid = false;



NxFluid* fluid = NULL;

NxVec3* gVerts = 0;
NxU32            gNumVerts = 0;
NxU32            gNumTriangles = 0;
NxVec3* gNormals = 0;
NxU32* gIndices = 0;


extern float sphereDiameter;
extern float scaleFactor;
extern int maxParticles;
extern NxScene* gScene;
extern NxFluidEmitter* fluidEmitter;


NxFluid* CreateFluid()
{
	// Create fluid
	NxFluidDesc fluidDesc;
	fluidDesc.setToDefault();
	fluidDesc.kernelRadiusMultiplier = 2.0f;
	fluidDesc.restParticlesPerMeter = 1 / (sphereDiameter / scaleFactor);
	fluidDesc.motionLimitMultiplier = 3.0f;
	fluidDesc.packetSizeMultiplier = 8;
	fluidDesc.collisionDistanceMultiplier = 0.1;
	fluidDesc.stiffness = 50.0f;
	fluidDesc.viscosity = 40.0f;
	fluidDesc.restDensity = 1000.0f;
	fluidDesc.damping = 0.0f;
	// There are some API changes since 280 version, Fluid collision coefficients have been renamed, 
	// E.g. NxFluidDesc::dynamicCollisionAdhesion is named NxFluidDesc::dynamicFrictionForDynamicShapes. 
#if NX_SDK_VERSION_NUMBER < 280
	fluidDesc.staticCollisionRestitution = 0.162f;
	fluidDesc.staticCollisionAdhesion = 0.146f;
	fluidDesc.dynamicCollisionRestitution = 0.5f;
	fluidDesc.dynamicCollisionAdhesion = 0.5f;
#else 
	fluidDesc.restitutionForStaticShapes = 0.162f;
	fluidDesc.dynamicFrictionForStaticShapes = 0.146f;
	fluidDesc.restitutionForDynamicShapes = 0.5f;
	fluidDesc.dynamicFrictionForDynamicShapes = 0.5f;
#endif
	fluidDesc.simulationMethod = NX_F_SPH;
	fluidDesc.maxParticles = maxParticles;

	gParticles = new NxVec3[fluidDesc.maxParticles];
	fluidDesc.particlesWriteData.bufferPos = &gParticles[0].x;
	fluidDesc.particlesWriteData.bufferPosByteStride = sizeof(NxVec3);
	fluidDesc.particlesWriteData.bufferVel = 0;
	fluidDesc.particlesWriteData.bufferVelByteStride = 0;
	//fluidDesc.particlesWriteData.maxParticles = fluidDesc.maxParticles;
	fluidDesc.particlesWriteData.numParticlesPtr = &gNumParticles;

	if (!bHardwareFluid)
		fluidDesc.flags &= ~NX_FF_HARDWARE;

	NxFluid* mFluid = gScene->createFluid(fluidDesc);
	if (!mFluid)
	{
		fluidDesc.flags &= ~NX_FF_HARDWARE;
		bHardwareFluid = false;
		mFluid = gScene->createFluid(fluidDesc);
	}
	return mFluid;
}

NxFluidEmitter* CreateFluidEmitter(const NxVec3 pos, const NxReal dimX, const NxReal dimY)
{
	fluid = CreateFluid();
	assert(fluid);

	NxQuat q;
	q.fromAngleAxis(90, NxVec3(1, 0, 0));
	NxMat34 mat;
	mat.M.fromQuat(q);
	mat.t = pos;

	// Create emitter
	NxFluidEmitterDesc emitterDesc;
	emitterDesc.setToDefault();
	emitterDesc.frameShape = NULL;
	emitterDesc.dimensionX = dimX;
	emitterDesc.dimensionY = dimY;
	emitterDesc.relPose = mat;
	emitterDesc.rate = 100;
	emitterDesc.randomAngle = 0.0f;
	emitterDesc.randomPos = NxVec3(0.0f, 0.0f, 0.0f);
	emitterDesc.fluidVelocityMagnitude = 2.5f;
	emitterDesc.repulsionCoefficient = 0.02f;
	emitterDesc.maxParticles = 0;
	emitterDesc.particleLifetime = 0.0f;
	emitterDesc.type = NX_FE_CONSTANT_PRESSURE;
	emitterDesc.shape = NX_FE_ELLIPSE;
	return fluid->createEmitter(emitterDesc);
}

void ReleaseFluid()
{
	if (fluid)
	{
		// Destroy the emitter.
		if (fluidEmitter)
		{
			fluid->releaseEmitter(*fluidEmitter);
			fluidEmitter = 0;
		}

		// Destroy the Fluid.
		if (gScene)
		{
			gScene->releaseFluid(*fluid);
		}
		fluid = 0;
	}

	// Destroy our buffers
	if (gParticles)
	{
		delete[] gParticles;
		gParticles = 0;
	}
	if (gVerts)
	{
		delete[] gVerts;
		gVerts = 0;
	}
	if (gNormals)
	{
		delete[] gNormals;
		gNormals = 0;
	}
	if (gIndices)
	{
		delete[] gIndices;
		gIndices = 0;
	}

	gNumTriangles = 0;
}

void RenderFluid()
{
	glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
	for (NxU32 p = 0; p < gNumParticles; p++)
	{
		NxVec3& particle = gParticles[p];
		glPushMatrix();
		glTranslatef(particle.x, particle.y, particle.z);
		glutSolidSphere(sphereDiameter / 1.8 / scaleFactor, 8, 5);
		glPopMatrix();
	}
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

}