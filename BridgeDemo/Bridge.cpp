/*
Bullet Continuous Collision Detection and Physics Library
Copyright (c) 2015 Google Inc. http://bulletphysics.org

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, 
including commercial applications, and to alter it and redistribute it freely, 
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#include "Bridge.h"
#include "btBulletDynamicsCommon.h"
#include "LinearMath/btVector3.h"
#include "LinearMath/btAlignedObjectArray.h"
#include "CommonRigidBodyBase.h"
//#include "Importers/ImportMeshUtility/b3ImportMeshUtility.h"
#include "Utils/b3BulletDefaultFileIO.h"
#include "OpenGLWindow/GLInstanceGraphicsShape.h"
#include  "Importers/ImportSTLDemo/ImportSTLSetup.h"
#include "Utils/b3ResourcePath.h"
#include "OpenGLWindow/GLInstanceGraphicsShape.h"
#include  "Importers/ImportSTLDemo/LoadMeshFromSTL.h"

const int TOTAL_PLANKS = 10;
struct BridgeExample : public CommonRigidBodyBase
{
	BridgeExample(struct GUIHelperInterface* helper)
		: CommonRigidBodyBase(helper)
	{
	}
	virtual ~BridgeExample() {}
	virtual void initPhysics();
	virtual void renderScene();
	void resetCamera()
	{
		float dist = 41;
		float pitch = -35;
		float yaw = 52;
		float targetPos[3] = {0, 0.46, 0};
		m_guiHelper->resetCamera(dist, yaw, pitch, targetPos[0], targetPos[1], targetPos[2]);
	}
};

void BridgeExample::initPhysics()
{
    m_guiHelper->setUpAxis(1);
    createEmptyDynamicsWorld();

    // 1. Load the STL instead of OBJ
    b3BulletDefaultFileIO fileIO;
    const char* stlPath = "G:\\bullet3\\examples\\pybullet\\gym\\pybullet_data\\l_finger_tip.stl"; // Or your full path
    char relativeFileName[1024];
    int myMeshId = -1;
    btConvexHullShape* convexShape = nullptr;
    if (b3ResourcePath::findResourcePath(stlPath, relativeFileName, 1024, 0))
    {
        GLInstanceGraphicsShape* gfxShape = LoadMeshFromSTL(relativeFileName, &fileIO);
        if (gfxShape == nullptr) {
            printf("ERROR: LoadMeshFromSTL returned NULL! (File might be binary STL or corrupted)\n");
        } 
        else
        {
            printf("SUCCESS: Loaded %d vertices\n", gfxShape->m_numvertices);
            convexShape = new btConvexHullShape();
            if (gfxShape && gfxShape->m_numvertices > 0)
            {
                // 1. Add all STL vertices to the Convex Hull
                for (int v = 0; v < gfxShape->m_numvertices; v++) {
                    btVector3 vtx(
                        gfxShape->m_vertices->at(v).xyzw[0],
                        gfxShape->m_vertices->at(v).xyzw[1],
                        gfxShape->m_vertices->at(v).xyzw[2]
                    );
                    convexShape->addPoint(vtx);
                }
                // 2. Register for graphics and get the mesh ID
                myMeshId = m_guiHelper->getRenderInterface()->registerShape(
                    &gfxShape->m_vertices->at(0).xyzw[0],
                    gfxShape->m_numvertices,
                    &gfxShape->m_indices->at(0),
                    gfxShape->m_numIndices);
                delete gfxShape; // Clean up memory after registering
            }
            printf("Found STL at: %s\n", relativeFileName);
            // ... inside the STL loading block ...
            printf("DEBUG: STL myMeshId = %d\n", myMeshId); // Check if this is 0 or higher
        }
    }
    else
    {
         printf("ERROR: Could not find STL file: %s\n", stlPath);
    }

    // 2. Standard Ground Creation
    btBoxShape* groundShape = createBoxShape(btVector3(50, 50, 50)); 

    createRigidBody(0, btTransform(btQuaternion(0,0,0,1), btVector3(0,-50,0)), groundShape);
    // 3. Create Planks and attach STL Graphics
        //btBoxShape* colShape = createBoxShape(btVector3(0.4, 0.2, 1.0)); //commented to use convex hull shape
        
        btAlignedObjectArray<btRigidBody*> boxes;
        btVector3 scaling(10, 10, 10); 
        convexShape->setLocalScaling(scaling);

        for (int i = 0; i < TOTAL_PLANKS; ++i) {
            btTransform trans;
            trans.setIdentity();
            trans.setOrigin(btVector3(i * 12.0, 5, 0));

            // Create Physics Body
            btRigidBody* body = createRigidBody((i == 0 || i == TOTAL_PLANKS-1) ? 0 : 1.0f, trans, convexShape);
            boxes.push_back(body);

            if (myMeshId >= 0) {
                // 1. Register the graphics and get the ID
                int graphicsId = m_guiHelper->getRenderInterface()->registerGraphicsInstance(
                    myMeshId, trans.getOrigin(), trans.getRotation(), 
                    btVector4(0, 0, 1, 1), scaling);

                // 2. LINK: This enables physics interaction for the custom mesh
                body->setUserIndex(graphicsId); 
            }
            else {
                printf("DEBUG: Skipping graphics for plank %d - myMeshId is invalid!\n", i);
            }
        }

    printf("DEBUG: Adding Constraints\n");
    for (int i = 0; i < TOTAL_PLANKS - 1; ++i) {
        btPoint2PointConstraint* leftSpring = new btPoint2PointConstraint(*boxes[i], *boxes[i+1], btVector3(6.0, 0, -0.5), btVector3(-6.0, 0, -0.5));
        m_dynamicsWorld->addConstraint(leftSpring);
        btPoint2PointConstraint* rightSpring = new btPoint2PointConstraint(*boxes[i], *boxes[i+1], btVector3(6.0, 0, 0.5), btVector3(-6.0, 0, 0.5));
        m_dynamicsWorld->addConstraint(rightSpring);
    }
    
    m_guiHelper->syncPhysicsToGraphics(m_dynamicsWorld);
    printf("DEBUG: initPhysics Finished Successfully!\n");
    
}
// void BridgeExample::renderScene()
// {
//     // 1. Sync custom meshes (STL) to physics
//     if (m_guiHelper)
//     {
//         m_guiHelper->syncPhysicsToGraphics(m_dynamicsWorld);
//     }

//     // 2. Draw everything (calls sync again internally and draws debug shapes)
//     CommonRigidBodyBase::renderScene();
// }

void BridgeExample::renderScene()
{
    if (m_dynamicsWorld && m_dynamicsWorld->getDebugDrawer())
    {
        // This line draws the ACTUAL physics shapes
        m_dynamicsWorld->debugDrawWorld();
    }

    m_guiHelper->syncPhysicsToGraphics(m_dynamicsWorld);
    CommonRigidBodyBase::renderScene();
}


CommonExampleInterface* StandaloneExampleCreateFunc(CommonExampleOptions& options)
{
    return new BridgeExample(options.m_guiHelper);
}