#include "QueryOcclusionCulling.h"

#ifdef ENABLE_QUERY_OCCLUSION

#include "../../EveryCulling.h"

#ifdef CULLING_OPENGL

static const char* const OCCLUSION_VERTEX_SHADER
{
"\
#version 460 core\
layout(location = 0) in vec3 aPos;\
uniform mat4 viewProjectionMatrix;\
void main()\
{\
gl_Position = viewProjectionMatrix * vec4(aPos, 1.0);\
}\
"
};


static const char* const OCCLUSION_FRAGMENT_SHADER
{
"\
#version 460 core\
void main()\
{\
}\
"\
};

#endif


bool culling::QueryOcclusionCulling::CompileOccluderShader()
{
#ifdef CULLING_OPENGL
	std::uint32_t vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
	std::uint32_t fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertex_shader_id, 1, &OCCLUSION_VERTEX_SHADER, NULL);
	glShaderSource(fragment_shader_id, 1, &OCCLUSION_FRAGMENT_SHADER, NULL);
	
	glCompileShader(vertex_shader_id);
	glCompileShader(fragment_shader_id);

	mOcclusionMaterialID = glCreateProgram();

	glAttachShader(mOcclusionMaterialID, vertex_shader_id);
	glAttachShader(mOcclusionMaterialID, fragment_shader_id);

	glLinkProgram(mOcclusionMaterialID);


	INT32 isSuccess = 0;
	glGetProgramiv(mOcclusionMaterialID, GL_LINK_STATUS, &isSuccess);
	assert(isSuccess != 0); // Shader Compiling is success???

	glDeleteShader(vertex_shader_id);
	glDeleteShader(fragment_shader_id);

	return isSuccess;

#elif CULLING_DIRECTX

	return false;

#endif

	assert(0);
}

bool culling::QueryOcclusionCulling::InitElementBufferObject()
{
#ifdef CULLING_OPENGL
	
	glGenBuffers(1, &(mElementBufferObjectID));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mElementBufferObjectID);

	const std::uint32_t AABB_INDEX_DATA[] = 
	{
			0, 1, 2, 2, 3, 0, 
			3, 2, 4, 4, 5, 3, 
			5, 4, 6, 6, 7, 5,
			7, 6, 1, 1, 0, 7,
			7, 0, 3, 3, 5, 7, 
			6, 1, 2, 2, 4, 6
	};

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(AABB_INDEX_DATA), AABB_INDEX_DATA, GL_STATIC_DRAW);

	return true;
#elif CULLING_DIRECTX

	return false;

#endif

	assert(0);
}

void culling::QueryOcclusionCulling::InitQueryOcclusionCulling()
{

	if (CompileOccluderShader() == false)
	{
		return;
	}

	if (InitElementBufferObject() == false)
	{
		return;
	}
	mViewProjectionMatrixUnifromLocation = glGetUniformLocation(mOcclusionMaterialID, "viewProjectionMatrix");


	bmIsQueryOcclusionReady = true;
}


bool culling::QueryOcclusionCulling::StartQuery(const std::uint32_t queryID)
{
	assert(queryID != 0);

#ifdef CULLING_OPENGL
	glBeginQuery(GL_SAMPLES_PASSED, queryID);
	glBeginQuery(GL_ANY_SAMPLES_PASSED, queryID);
	//glBeginQuery(GL_ANY_SAMPLES_PASSED_CONSERVATIVE, mQueryID);
	return true;
#elif CULLING_DIRECTX
	return false;
#endif
}

void culling::QueryOcclusionCulling::StopQuery(const std::uint32_t queryID)
{
#ifdef CULLING_OPENGL
	glEndQuery(queryID);
#elif CULLING_DIRECTX

#endif
}

static void GenAABB8VerticesFromLocalAABB(const culling::AABB& localAABB, culling::Vec3 * aabb8Vercies)
{
	aabb8Vercies[0].x = localAABB.mMin[0];
	aabb8Vercies[0].y = localAABB.mMax[1];
	aabb8Vercies[0].z = localAABB.mMin[2];
	
	aabb8Vercies[1].x = localAABB.mMin[0];
	aabb8Vercies[1].y = localAABB.mMin[1];
	aabb8Vercies[1].z = localAABB.mMin[2];

	aabb8Vercies[2].x = localAABB.mMax[0];
	aabb8Vercies[2].y = localAABB.mMin[1];
	aabb8Vercies[2].z = localAABB.mMin[2];

	aabb8Vercies[3].x = localAABB.mMax[0];
	aabb8Vercies[3].y = localAABB.mMax[1];
	aabb8Vercies[3].z = localAABB.mMin[2];

	aabb8Vercies[4].x = localAABB.mMax[0];
	aabb8Vercies[4].y = localAABB.mMin[1];
	aabb8Vercies[4].z = localAABB.mMax[2];

	aabb8Vercies[5].x = localAABB.mMax[0];
	aabb8Vercies[5].y = localAABB.mMax[1];
	aabb8Vercies[5].z = localAABB.mMax[2];

	aabb8Vercies[6].x = localAABB.mMin[0];
	aabb8Vercies[6].y = localAABB.mMin[1];
	aabb8Vercies[6].z = localAABB.mMax[2];

	aabb8Vercies[7].x = localAABB.mMin[0];
	aabb8Vercies[7].y = localAABB.mMax[1];
	aabb8Vercies[7].z = localAABB.mMax[2];

}




void culling::QueryOcclusionCulling::QueryOccludeeAABB(const culling::QueryObject& queryObject)
{
	

	assert(bmIsQueryOcclusionReady == true);

	if (StartQuery(queryObject.mQueryID) == false)
	{
		return;
	}

#ifdef CULLING_OPENGL

	glUniformMatrix4fv(mViewProjectionMatrixUnifromLocation, 1, GL_FALSE, reinterpret_cast<const float*>(&(queryObject.mLocal2WorldMatrix)));

	//Draw Occludee AABB
	glBindVertexArray(queryObject.mVertexArrayObjectID);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	

#elif CULLING_DIRECTX

#endif

	StopQuery(queryObject.mQueryID);
}

culling::QueryOcclusionCulling::QueryOcclusionCulling(culling::EveryCulling* everyCulling)
	: CullingModule{ everyCulling }
{

}

culling::QueryOcclusionCulling::~QueryOcclusionCulling()
{
	glDeleteProgram(mOcclusionMaterialID);
	glDeleteBuffers(1, &(mElementBufferObjectID));

	for (culling::QueryObject* queryObject : mQueryObjects)
	{
		DestroyQueryObject(queryObject);
	}
}


static void vector_swap_popback(std::vector<culling::QueryObject*>& vec, culling::QueryObject* erasedQueryObject)
{
	for (size_t i = 0; i < vec.size(); i++)
	{
		if (vec[i] == erasedQueryObject)
		{
			vec[i] = vec.back();
			vec.pop_back();
			return;
		}
	}
}

void culling::QueryOcclusionCulling::DestroyQueryObject(culling::QueryObject* queryObject)
{
	glDeleteBuffers(1, &(queryObject->mBufferID));
	glDeleteVertexArrays(1, &(queryObject->mVertexArrayObjectID));
	glDeleteQueries(1, &(queryObject->mQueryID));
	delete queryObject;
}


void culling::QueryOcclusionCulling::ClearEntityData(EntityBlock* currentEntityBlock, size_t entityIndex)
{
	culling::QueryObject* queryObject = currentEntityBlock->mQueryObjects[entityIndex];
	if (queryObject != nullptr)
	{
		vector_swap_popback(mQueryObjects, queryObject);
		currentEntityBlock->mQueryObjects[entityIndex] = nullptr;
		DestroyQueryObject(queryObject);
	}
}




void culling::QueryOcclusionCulling::GenQueryObject(EntityBlock* currentEntityBlock, size_t entityIndex, const culling::AABB& occlusionAABBLocalMinMax)
{
	assert(currentEntityBlock != nullptr);

	if (bmIsQueryOcclusionReady == false)
	{
		InitQueryOcclusionCulling();
	}
	
	assert(currentEntityBlock->mQueryObjects[entityIndex] == nullptr);

	//TODO : Make quey pool
	culling::QueryObject* newQueryObject = new culling::QueryObject;
	std::uint32_t newQuery;
	std::uint32_t newVertexBufferID;
	std::uint32_t newVertexArrayObjectID;

#ifdef CULLING_OPENGL
	glGenQueries(1, &newQuery);

	glGenBuffers(1, &(newVertexBufferID));
	glGenVertexArrays(1, &(newVertexArrayObjectID));

	glBindVertexArray(newVertexArrayObjectID);
	glBindBuffer(GL_ARRAY_BUFFER, newVertexBufferID);

	culling::Vec3 aabbLocalPoints[8];
	GenAABB8VerticesFromLocalAABB(occlusionAABBLocalMinMax, aabbLocalPoints);

	//8 vertexs, 3 components
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8 * 3, aabbLocalPoints, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

	newQueryObject->mQueryID = newQuery;
	newQueryObject->mBufferID = newVertexBufferID;
	newQueryObject->mVertexArrayObjectID = newVertexArrayObjectID;

	currentEntityBlock->mQueryObjects[entityIndex] = newQueryObject;
	mQueryObjects.push_back(newQueryObject);

#elif CULLING_DIRECTX
	assert(0);

#endif
	
}

void culling::QueryOcclusionCulling::GenQueryObject(culling::EntityBlockViewer& entityBlockViewer, const culling::AABB& occlusionAABBLocalMinMax)
{
	GenQueryObject(entityBlockViewer.mTargetEntityBlock, entityBlockViewer.mEntityIndexInBlock, occlusionAABBLocalMinMax);
}

void culling::QueryOcclusionCulling::CullBlockEntityJob(const size_t cameraIndex)
{
}

/// <summary>
/// Use Query Result After a frame
/// == Do ConditionalRendering on previous frame's query ( Maybe I need Two Query )
/// http://tomcc.github.io/2014/08/31/visibility-1.html
/// 
/// </summary>
void culling::QueryOcclusionCulling::QueryOccludeeAABB()
{
	if (mQueryObjects.empty() == true)
	{
		return;
	}

	glDisable(GL_CULL_FACE);
	glDepthMask(GL_FALSE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mElementBufferObjectID);

	for (const culling::QueryObject* queryObject : mQueryObjects)
	{
		QueryOccludeeAABB(*queryObject);
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//turn rendering back on
	glEnable(GL_CULL_FACE);
	glDepthMask(GL_TRUE);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}

//reference : https://github.com/progschj/OpenGL-Examples/blob/master/10queries_conditional_render.cpp
//
// Screen Space AABB�� �޸� Query Occlusion�� ��� ������Ʈ�� Query ����� ����ϰ�, �̱۽����忡���� �۵���
// 
// Query ����� ������Ʈ���� ��� ������ LocalAABB�� QueryID, MeshBuffer, MeshArrayBuffer, ElementBuffer�� ���� �ϳ��� ��������
// �ʿ��� �����Ͱ� ũ�� EntityBlock���� ������ �Ҵ��� �޾ƾ��Ѵ�. 
// 
// �׸��� ������ �� StartConditionalRenderingIfQueryActive�� Query�� ��쿡�� Conditional Rendering Ŀ���� ������
//
//

#endif


