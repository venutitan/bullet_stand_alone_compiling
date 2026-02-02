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

const int TOTAL_PLANKS = 4;
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
    const char* stlPath = "G:\\bullet3\\examples\\pybullet\\gym\\pybullet_data\\bicycle\\files\\bike_rack.stl"; // Or your full path
    char relativeFileName[1024];
    
    int myMeshId = -1;
    if (b3ResourcePath::findResourcePath(stlPath, relativeFileName, 1024, 0))
    {
        GLInstanceGraphicsShape* gfxShape = LoadMeshFromSTL(relativeFileName, &fileIO);
        if (gfxShape == nullptr) {
            printf("ERROR: LoadMeshFromSTL returned NULL! (File might be binary STL or corrupted)\n");
        } 
        else
        {
            printf("SUCCESS: Loaded %d vertices\n", gfxShape->m_numvertices);
            if (gfxShape && gfxShape->m_numvertices > 0)
            {
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
    btBoxShape* colShape = createBoxShape(btVector3(0.4, 0.2, 1.0));
    btAlignedObjectArray<btRigidBody*> boxes;
    btVector3 scaling(10, 10, 10); // Match your first script's scale

    for (int i = 0; i < TOTAL_PLANKS; ++i) {
        btTransform trans;
        trans.setIdentity();
        trans.setOrigin(btVector3(i * 0.8 - 1.2, 5, 0));

        btRigidBody* body = createRigidBody((i == 0 || i == TOTAL_PLANKS-1) ? 0 : 1.0f, trans, colShape);
        boxes.push_back(body);

        if (myMeshId >= 0) {
            m_guiHelper->getRenderInterface()->registerGraphicsInstance(
                myMeshId, trans.getOrigin(), trans.getRotation(), 
                btVector4(0, 0, 1, 1), scaling);
        }

        // ... inside the loop ...
        else {
            printf("DEBUG: Skipping graphics - myMeshId is invalid!\n");
        }
    }

    printf("DEBUG: Adding Constraints\n");
    for (int i = 0; i < TOTAL_PLANKS - 1; ++i) {
        btPoint2PointConstraint* leftSpring = new btPoint2PointConstraint(*boxes[i], *boxes[i+1], btVector3(-0.5, 0, -0.5), btVector3(0.5, 0, -0.5));
        m_dynamicsWorld->addConstraint(leftSpring);
        btPoint2PointConstraint* rightSpring = new btPoint2PointConstraint(*boxes[i], *boxes[i+1], btVector3(-0.5, 0, 0.5), btVector3(0.5, 0, 0.5));
        m_dynamicsWorld->addConstraint(rightSpring);
    }
    
    //     if (gfxShape) {
    //     delete gfxShape;
    // }
    m_guiHelper->syncPhysicsToGraphics(m_dynamicsWorld);
    printf("DEBUG: initPhysics Finished Successfully!\n");
    
}

void BridgeExample::renderScene()
{
    // Draws the ground and boxes
    CommonRigidBodyBase::renderScene();

    // Makes your OBJ mesh follow the physics boxes
    if (m_guiHelper && m_guiHelper->getRenderInterface())
    {
        m_guiHelper->syncPhysicsToGraphics(m_dynamicsWorld);
    }
}
CommonExampleInterface* StandaloneExampleCreateFunc(CommonExampleOptions& options)
{
    return new BridgeExample(options.m_guiHelper);
}