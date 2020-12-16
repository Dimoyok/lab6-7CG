#include "lab67.h"


extern NxScene* gScene;
extern NxPhysicsSDK* gPhysicsSDK;

NxActor* CreateGroundPlane()
{
	// Create a plane with default descriptor
	NxPlaneShapeDesc planeDesc;
	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&planeDesc);
	return gScene->createActor(actorDesc);
}


NxActor* CreateInnerCylinder(const float r, const float h, const int k)
{
	static NxTriangleMeshDesc* iCylinderDesc = NULL;

	int numVerts = k * 2;
	int numTris = k * 2 + (k - 2) * 2;
	NxVec3* verts = new NxVec3[numVerts];
	for (int i = 0; i < k; i++)
	{
		verts[i * 2] = NxVec3(r * cos(i * NxPi * 2 / k), 0.0f, r * sin(i * NxPi * 2 / k));
		verts[i * 2 + 1] = NxVec3(r * cos(i * NxPi * 2 / k), h, r * sin(i * NxPi * 2 / k));
	}

	NxU32* indices = new NxU32[numTris * 3];
	for (int i = 0; i < k; i++)
	{
		indices[i * 6] = i * 2;
		indices[i * 6 + 1] = i * 2 + 1;
		indices[i * 6 + 2] = (i + 1) % k * 2;

		indices[i * 6 + 3] = (i + 1) % k * 2;
		indices[i * 6 + 4] = i * 2 + 1;
		indices[i * 6 + 5] = (i + 1) % k * 2 + 1;
	}

	for (int i = 1; i < k - 1; i++)
	{
		indices[k * 6 + (i - 1) * 6] = 0;
		indices[k * 6 + (i - 1) * 6 + 1] = i * 2;
		indices[k * 6 + (i - 1) * 6 + 2] = (i + 1) * 2;

		indices[k * 6 + (i - 1) * 6 + 3] = 1;
		indices[k * 6 + (i - 1) * 6 + 4] = (i + 1) * 2 + 1;
		indices[k * 6 + (i - 1) * 6 + 5] = i * 2 + 1;
	}

	// Create descriptor for triangle mesh
	if (!iCylinderDesc)
	{
		iCylinderDesc = new NxTriangleMeshDesc();
	}
	iCylinderDesc->numVertices = numVerts;
	iCylinderDesc->pointStrideBytes = sizeof(NxVec3);
	iCylinderDesc->points = verts;
	iCylinderDesc->numTriangles = numTris;
	iCylinderDesc->triangles = indices;
	iCylinderDesc->triangleStrideBytes = 3 * sizeof(NxU32);


	// The actor has one shape, a triangle mesh
	NxInitCooking();
	MemoryWriteBuffer buf;

	bool status = NxCookTriangleMesh(*iCylinderDesc, buf);
	NxTriangleMesh* pMesh;
	if (status)
	{
		pMesh = gPhysicsSDK->createTriangleMesh(MemoryReadBuffer(buf.data));
	}
	else
	{
		pMesh = NULL;
	}
	NxCloseCooking();

	NxTriangleMeshShapeDesc tmsd;
	tmsd.meshData = pMesh;
	tmsd.userData = iCylinderDesc;

	NxActorDesc actorDesc;
	NxBodyDesc  bodyDesc;

	actorDesc.shapes.pushBack(&tmsd);
	actorDesc.body = NULL;
	NxActor* actor = NULL;

	if (pMesh)
	{
		// Save mesh in userData for drawing
		pMesh->saveToDesc(*iCylinderDesc);
		//

		actor = gScene->createActor(actorDesc);
	}

	delete[] verts;
	delete[] indices;

	return actor;
}

NxActor* CreateOuterCylinder(const float r, const float h, const int k)
{
	static NxTriangleMeshDesc* oCylinderDesc = NULL;

	int numVerts = k * 2;
	int numTris = k * 2;
	NxVec3* verts = new NxVec3[numVerts];
	for (int i = 0; i < k; i++)
	{
		verts[i * 2] = NxVec3(r * cos(i * NxPi * 2 / k), 0.0f, r * sin(i * NxPi * 2 / k));
		verts[i * 2 + 1] = NxVec3(r * cos(i * NxPi * 2 / k), h, r * sin(i * NxPi * 2 / k));
	}

	NxU32* indices = new NxU32[numTris * 3];
	for (int i = 0; i < k; i++)
	{
		indices[i * 6] = i * 2;
		indices[i * 6 + 1] = (i + 1) % k * 2;
		indices[i * 6 + 2] = i * 2 + 1;

		indices[i * 6 + 3] = (i + 1) % k * 2;
		indices[i * 6 + 4] = (i + 1) % k * 2 + 1;
		indices[i * 6 + 5] = i * 2 + 1;
	}

	// Create descriptor for triangle mesh
	if (!oCylinderDesc)
	{
		oCylinderDesc = new NxTriangleMeshDesc();
	}
	oCylinderDesc->numVertices = numVerts;
	oCylinderDesc->pointStrideBytes = sizeof(NxVec3);
	oCylinderDesc->points = verts;
	oCylinderDesc->numTriangles = numTris;
	oCylinderDesc->triangles = indices;
	oCylinderDesc->triangleStrideBytes = 3 * sizeof(NxU32);


	// The actor has one shape, a triangle mesh
	NxInitCooking();
	MemoryWriteBuffer buf;

	bool status = NxCookTriangleMesh(*oCylinderDesc, buf);
	NxTriangleMesh* pMesh;
	if (status)
	{
		pMesh = gPhysicsSDK->createTriangleMesh(MemoryReadBuffer(buf.data));
	}
	else
	{
		pMesh = NULL;
	}
	NxCloseCooking();

	NxTriangleMeshShapeDesc tmsd;
	tmsd.meshData = pMesh;
	tmsd.userData = oCylinderDesc;

	NxActorDesc actorDesc;
	NxBodyDesc  bodyDesc;

	actorDesc.shapes.pushBack(&tmsd);
	actorDesc.body = NULL;
	NxActor* actor = NULL;

	if (pMesh)
	{
		// Save mesh in userData for drawing
		pMesh->saveToDesc(*oCylinderDesc);
		//

		actor = gScene->createActor(actorDesc);
	}

	delete[] verts;
	delete[] indices;

	return actor;
}

NxActor* CreateSphere(float r)
{

	// Add a single-shape actor to the scene
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	// The actor has one shape, a sphere, 1m on radius
	NxSphereShapeDesc sphereDesc;
	sphereDesc.radius = r;

	actorDesc.shapes.pushBack(&sphereDesc);
	actorDesc.body = &bodyDesc;
	actorDesc.density = 1.0f;
	return gScene->createActor(actorDesc);
}


NxActor* CreateWaterSphere(const float r, const int k)
{
	static NxConvexMeshDesc* waterDesk = NULL;

	int dk = k / 4 > 11 ? 11 : k / 4;

	int numVerts = dk * (dk - 1) * 2 + 2;
	NxVec3* verts = new NxVec3[numVerts];
	verts[0] = NxVec3(0, 0, -r);
	for (int i = 0; i < dk - 1; i++)
	{
		for (int j = 0; j < 2 * dk; j++)
		{
			float theta = (i + 1) * NxPi / dk;
			float phi = j * NxPi / dk;

			verts[i * 2 * dk + j + 1] = NxVec3(
				r * sin(theta) * cos(phi),
				r * sin(theta) * sin(phi),
				r * cos(theta)
			);
		}
	}
	verts[numVerts - 1] = NxVec3(0, 0, r);

	// Create descriptor for convex mesh
	if (!waterDesk)
	{
		waterDesk = new NxConvexMeshDesc();
	}
	waterDesk->numVertices = numVerts;
	waterDesk->pointStrideBytes = sizeof(NxVec3);
	waterDesk->points = verts;
	waterDesk->flags = NX_CF_COMPUTE_CONVEX;

	NxConvexShapeDesc convexShapeDesc;
	convexShapeDesc.userData = waterDesk;


	// Two ways on cooking mesh: 1. Saved in memory, 2. Saved in file	
	NxInitCooking();
	// Cooking from memory
	MemoryWriteBuffer buf;

	bool status = NxCookConvexMesh(*waterDesk, buf);

	NxConvexMesh* pMesh = gPhysicsSDK->createConvexMesh(MemoryReadBuffer(buf.data));
	convexShapeDesc.meshData = pMesh;
	NxCloseCooking();

	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;
	NxActor* actor = NULL;


	if (pMesh)
	{
		// Save mesh in userData for drawing.
		pMesh->saveToDesc(*waterDesk);
		//
		NxActorDesc actorDesc;
		actorDesc.shapes.pushBack(&convexShapeDesc);
		actorDesc.body = &bodyDesc;
		actorDesc.density = 1.0f;

		actor = gScene->createActor(actorDesc);
	}

	delete[] verts;

	return actor;
}