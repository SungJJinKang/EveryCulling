#pragma once

#include "../CullingModule.h"

#include "../../DataType/Math/Vector.h"
#include "../../DataType/Math/Matrix.h"

#include "../../DataType/EntityBlockViewer.h"

#ifdef ENABLE_QUERY_OCCLUSION



namespace culling
{
	/// <summary>
	///
	/// This module is executed on single thread
	/// 
	/// 1. Draw Occluder.        
	/// 2. Draw AABB of Complicated Occludeee mesh. (AABB is much cheaper than complicated mesh)
	///	3. If Any fragment of AABB is drawed on buffer(aabb passed depth, stencil test..!), Draw Complicated Mesh!!!
	/// 
	/// references : https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_occlusion_query.txt , https://www.khronos.org/registry/OpenGL/extensions/NV/NV_conditional_render.txt      
	///
	/// </summary>
	class QueryOcclusionCulling : public CullingModule
	{

		friend class Graphics_Server;

	private:

		unsigned int mOcclusionMaterialID{ 0 };

		unsigned int mVertexBufferID{ 0 };
		unsigned int mVertexArrayObjectID{ 0 };

		bool bmIsQueryOcclusionReady{ false };

		bool CompileOccluderShader();
		bool InitVertexBuffer();

		void InitQueryOcclusionCulling();

		//Mesh mBakedOccluderMesh{};
		/// <summary>
		/// Really simple Material
		/// Just Draw 
		/// </summary>
		//std::unique_ptr<Material> mOccluderMaterial{};

		unsigned int GenQuery();
		/// <summary>
		/// draw Simple bounding volume(Occluder) after this function call
		/// TODO : Disable Pixel Shading, Texturing
		/// </summary>
		bool StartQuery(const unsigned int queryID);
		void StopQuery(const unsigned int queryID);

		/// <summary>
		/// draw complex mesh after this function call
		/// 
		/// We don't need retrieve query to memory from gpu, Just use it in GPU
		/// 
		/// Quering Occluder Data require some times. So Don't Do ConditionalRender instantly after Call Query Data 
		/// Have some wait time to query data
		/// 
		/// 
		///  For best performance, applications should attempt to insert some amount
		///  of non - dependent rendering between an occlusion query and the
		///	 conditionally - rendered primitives that depend on the query result.
		/// </summary>
		bool StartConditionalRender(unsigned int queryID);
		void StopConditionalRender();

		//TODO : Check ScreenSpace AABB size of rendered mesh, If Size if smaller than setting, It's not valuable to use as Occluder

		/// <summary>
		/// Start Query and Draw AABB of Occludee
		/// You should pass World Space AABB Vertices
		/// 
		/// Quering AABB Data require a few times. 
		/// So Don't Do ConditionalRender instantly after Call Query Data!!!!!!!!!!!!!!!!!!!!!!!!!!! 
		/// Do other works between QueryOccludeeAABB and StartConditionalRender
		/// 
		/// </summary>
		/// <param name="queryID"></param>
		/// <param name="occlusionAABBWorldVertices"></param>
		void QueryOccludeeAABB(const unsigned int queryID, const culling::Vector3* occlusionAABBWorldVertices);

		static culling::AABBVertices GenAABBVerticesFromWorldSpace(const culling::Vector3& minLocalSpace, const culling::Vector3& maxLocalSpace);
		static culling::AABBVertices GenAABBVerticesFromLocalSpace(const culling::Vector3& minLocalSpace, const culling::Vector3& maxLocalSpace, const culling::Matrix4X4& localToWorldMatrix);

	public:

		QueryOcclusionCulling(culling::EveryCulling* everyCulling);
		virtual ~QueryOcclusionCulling();

		/// <summary>
		/// 
		/// </summary>
		/// <param name="currentEntityBlock"></param>
		/// <param name="entityCountInBlock"></param>
		/// <param name="cameraIndex"></param>
		virtual void CullBlockEntityJob(EntityBlock* currentEntityBlock, size_t entityCountInBlock, size_t cameraIndex) final;
		void ClearEntityData(EntityBlock* currentEntityBlock, unsigned int entityIndex) override;

		/// <summary>
		/// This function should be executed on single thread
		/// </summary>
		void QueryOccludeeAABB();
		void StartConditionalRenderingIfQueryActive(const EntityBlockViewer& entityBlockViewer);
	};

}



#endif