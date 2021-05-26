#include "QueryOcclusionCulling.h"

#ifdef ENABLE_QUERY_OCCLUSION

#include "../../EveryCulling.h"
#include "../../DataType/EntityGridCell.h"

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
layout(location = 0) out vec4 oColor; \
void main()\
{\
	oColor = vec4(1.0); \
}\
"\
};

#endif


bool culling::QueryOcclusionCulling::CompileOccluderShader()
{
#ifdef CULLING_OPENGL
	unsigned int vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertex_shader_id, 1, &OCCLUSION_VERTEX_SHADER, NULL);
	glShaderSource(fragment_shader_id, 1, &OCCLUSION_FRAGMENT_SHADER, NULL);
	
	glCompileShader(vertex_shader_id);
	glCompileShader(fragment_shader_id);

	this->mOcclusionMaterialID = glCreateProgram();

	glAttachShader(this->mOcclusionMaterialID, vertex_shader_id);
	glAttachShader(this->mOcclusionMaterialID, fragment_shader_id);

	glLinkProgram(this->mOcclusionMaterialID);


	int isSuccess = 0;
	glGetProgramiv(this->mOcclusionMaterialID, GL_LINK_STATUS, &isSuccess);
	assert(isSuccess); // Shader Compiling is success???

	glDeleteShader(vertex_shader_id);
	glDeleteShader(fragment_shader_id);

	return isSuccess;

#elif CULLING_DIRECTX

	return false;

#endif

	assert(0);
}

bool culling::QueryOcclusionCulling::InitVertexBuffer()
{
#ifdef CULLING_OPENGL
	glGenBuffers(1, &(this->mVertexBufferID));
	glGenVertexArrays(1, &(this->mVertexArrayObjectID));

	return true;
#elif CULLING_DIRECTX

	return false;

#endif

	assert(0);
}

void culling::QueryOcclusionCulling::InitQueryOcclusionCulling()
{

	if (this->CompileOccluderShader() == false)
	{
		return;
	}

	if (this->InitVertexBuffer() == false)
	{
		return;
	}



	this->bmIsQueryOcclusionReady = true;

}

unsigned int culling::QueryOcclusionCulling::GenQuery()
{
	//TODO : Make quey pool
	unsigned int newQuery;
	glGenQueries(1, &newQuery);
	return newQuery;
}

bool culling::QueryOcclusionCulling::StartQuery(const unsigned int queryID)
{
	assert(this->mQueryID != 0);

#ifdef CULLING_OPENGL
	glBeginQuery(GL_SAMPLES_PASSED, queryID);
	glBeginQuery(GL_ANY_SAMPLES_PASSED, queryID);
	//glBeginQuery(GL_ANY_SAMPLES_PASSED_CONSERVATIVE, this->mQueryID);
	return true;
#elif CULLING_DIRECTX
	return false;
#endif
}

void culling::QueryOcclusionCulling::StopQuery(const unsigned int queryID)
{
#ifdef CULLING_OPENGL
	glEndQuery(queryID);
#elif CULLING_DIRECTX

#endif
}

culling::AABBVertices culling::QueryOcclusionCulling::GenAABBVerticesFromWorldSpace(const culling::Vector3& minLocalSpace, const culling::Vector3& maxLocalSpace)
{
	return culling::AABBPoints();
}

culling::AABBVertices culling::QueryOcclusionCulling::GenAABBVerticesFromLocalSpace(const culling::Vector3& minLocalSpace, const culling::Vector3& maxLocalSpace, const culling::Matrix4X4& localToWorldMatrix)
{
	return culling::AABBPoints();
}

culling::QueryOcclusionCulling::QueryOcclusionCulling(culling::EveryCulling* everyCulling)
	: CullingModule{ everyCulling }
{

}

culling::QueryOcclusionCulling::~QueryOcclusionCulling()
{
	glDeleteProgram(this->mOcclusionMaterialID);

	glDeleteVertexArrays(1, &(this->mVertexArrayObjectID));
	glDeleteBuffers(1, &(this->mVertexBufferID));
}

void culling::QueryOcclusionCulling::ClearEntityData(EntityBlock* currentEntityBlock, unsigned int entityIndex)
{
	//throw std::logic_error("The method or operation is not implemented.");
}

bool culling::QueryOcclusionCulling::StartConditionalRender(unsigned int queryID)
{
#ifdef CULLING_OPENGL

	// If query didn't contribute to sample count(AABB is hidden by other object),
	// After glBeginConditionalRender Every Rendering Commands will be ignored
	glBeginConditionalRender(queryID, GL_QUERY_BY_REGION_WAIT);
	// Test GL_QUERY_WAIT vs GL_QUERY_NO_WAIT
	// 
	// GL_QUERY_WAIT : If query isn't completed yet, Wait it
	// GL_QUERY_NO_WAIT : Don't wait until query complete. if query isn't completed just draw it normally
	return true;
#elif CULLING_DIRECTX

	return false;
#endif
}

void culling::QueryOcclusionCulling::StopConditionalRender()
{
#ifdef CULLING_OPENGL
	glEndConditionalRender();
#elif CULLING_DIRECTX

#endif
}



void culling::QueryOcclusionCulling::QueryOccludeeAABB(const unsigned int queryID, const culling::Vector3* occlusionAABBWorldVertices)
{
	if (this->bmIsQueryOcclusionReady == false)
	{
		this->InitQueryOcclusionCulling();
	}

	assert(this->bmIsQueryOcclusionReady == true);

	if (this->StartQuery(queryID) == false)
	{
		return;
	}

#ifdef CULLING_OPENGL
	//Draw Occluder
	glBindVertexArray(this->mVertexArrayObjectID);
	glBindBuffer(GL_ARRAY_BUFFER, this->mVertexBufferID);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8, occlusionAABBWorldVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

	glDrawArrays(GL_TRIANGLES, 0, static_cast<unsigned int>(verticeCount / 3));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
#elif CULLING_DIRECTX

#endif

	this->StopQuery(queryID);
}


void culling::QueryOcclusionCulling::QueryOccludeeAABB()
{
	auto& gridCell = this->mCullingSystem->mEntityGridCell;
	for (size_t i = 0; i < gridCell.mEntityBlocks.size(); i++)
	{
		culling::EntityBlock* entityBlock = gridCell.mEntityBlocks[i];
		for (size_t entityIndex = 0; entityIndex < gridCell.AllocatedEntityCountInBlocks[i]; entityIndex++)
		{
			if (entityBlock->mUseQuery[entityIndex] == true)
			{

			}
		}
	}
}

//reference : https://github.com/progschj/OpenGL-Examples/blob/master/10queries_conditional_render.cpp
//
// Screen Space AABB와 달리 Query Occlusion은 몇몇 오브젝트만 Query 기능을 사용하고, 싱글스레드에서만 작동함
// 
// Query 사용할 오브젝트들은 모두 각각의 LocalAABB와 QueryID, MeshBuffer, MeshArrayBuffer, ElementBuffer를 각자 하나씩 가져야함
// 필요한 데이터가 크기 EntityBlock과는 별개로 할당을 받아야한다. 
// 
// 그리고 렌더링 전 StartConditionalRenderingIfQueryActive로 Query일 경우에만 Conditional Rendering 커지게 만들자
//
//

void culling::QueryOcclusionCulling::StartConditionalRenderingIfQueryActive(const EntityBlockViewer& entityBlockViewer)
{
	entityBlockViewer.mTargetEntityBlock->mUseQuery[entityBlockViewer->mEntityIndexInBlock]
}

#endif


