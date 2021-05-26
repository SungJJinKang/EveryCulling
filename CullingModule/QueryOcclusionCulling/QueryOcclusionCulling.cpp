#include "QueryOcclusionCulling.h"

#ifdef ENABLE_QUERY_OCCLUSION



#ifdef CULLING_OPENGL

const char* const OCCLUSION_VERTEX_SHADER
{
"\
#version 460 core\
layout(location = 0) in vec3 aPos;\
void main()\
{\
gl_Position = vec4(aPos, 1.0);\
}\
"
};


const char* const OCCLUSION_FRAGMENT_SHADER
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

#ifdef CULLING_OPENGL
	glGenQueries(1, &(this->mQueryID));
#elif CULLING_DIRECTX
	
#endif

	this->bmIsQueryOcclusionReady = true;

}

bool culling::QueryOcclusionCulling::StartQuery()
{
	assert(this->mQueryID != 0);

#ifdef CULLING_OPENGL
	glBeginQuery(GL_SAMPLES_PASSED, this->mQueryID);
	glBeginQuery(GL_ANY_SAMPLES_PASSED, this->mQueryID);
	//glBeginQuery(GL_ANY_SAMPLES_PASSED_CONSERVATIVE, this->mQueryID);
	return true;
#elif CULLING_DIRECTX
	return false;
#endif
}

bool culling::QueryOcclusionCulling::StopQuery()
{
#ifdef CULLING_OPENGL
	glEndQuery(this->mQueryID);
	return true;
#elif CULLING_DIRECTX
	return false;

#endif
}

culling::QueryOcclusionCulling::~QueryOcclusionCulling()
{
	glDeleteQueries(1, &(this->mQueryID));

	glDeleteProgram(this->mOcclusionMaterialID);

	glDeleteVertexArrays(1, &(this->mVertexArrayObjectID));
	glDeleteBuffers(1, &(this->mVertexBufferID));
}


void culling::QueryOcclusionCulling::StartConditionalRender()
{
#ifdef CULLING_OPENGL
	glBeginConditionalRender(this->mQueryID, GL_QUERY_NO_WAIT);
	// Test GL_QUERY_WAIT vs GL_QUERY_NO_WAIT
	// 
	// GL_QUERY_WAIT : If query isn't completed yet, Wait it
	// GL_QUERY_NO_WAIT : Don't wait until query complete. if query isn't completed just draw it normally
#elif CULLING_DIRECTX

#endif
}

void culling::QueryOcclusionCulling::StopConditionalRender()
{
#ifdef CULLING_OPENGL
	glEndConditionalRender();
#elif CULLING_DIRECTX

#endif
}

void culling::QueryOcclusionCulling::DrawOccluderAABB(const culling::Vector3* occlusionAABBWorldVertices, size_t verticeCount)
{
	if (this->bmIsQueryOcclusionReady == false)
	{
		this->InitQueryOcclusionCulling();
	}

	assert(this->bmIsQueryOcclusionReady == true);

	if (this->StartQuery())
	{
		return;
	}

#ifdef CULLING_OPENGL
	//Draw Occluder
	glBindVertexArray(this->mVertexArrayObjectID);
	glBindBuffer(GL_ARRAY_BUFFER, this->mVertexBufferID);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * verticeCount, occlusionAABBWorldVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

	glDrawArrays(GL_TRIANGLES, 0, static_cast<unsigned int>(verticeCount / 3));

	glBindVertexArray(0);
#elif CULLING_DIRECTX

#endif

	this->StopQuery();
}

#endif



