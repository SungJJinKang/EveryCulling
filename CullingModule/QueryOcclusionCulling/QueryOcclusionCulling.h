#pragma once

#include <vector>

#include "../CullingModule.h"

#include "../../DataType/QueryObject.h"
#include "../../DataType/EntityBlockViewer.h"
#include "../../DataType/Math/AABB.h"

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
	/// 4. But we use Previous Frame's Query Data, because ReadBack from GPU to CPU is slow.
	/// 
	/// references : https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_occlusion_query.txt , https://www.khronos.org/registry/OpenGL/extensions/NV/NV_conditional_render.txt      
	///
	/// </summary>
	class QueryOcclusionCulling : public CullingModule
	{

		friend class Graphics_Server;

	private:

		std::uint32_t mOcclusionMaterialID{ 0 };
		std::uint32_t mElementBufferObjectID{ 0 };
		std::uint32_t mViewProjectionMatrixUnifromLocation;

		bool bmIsQueryOcclusionReady{ false };

		std::vector<QueryObject*> mQueryObjects;

		bool CompileOccluderShader();
		bool InitElementBufferObject();

		void InitQueryOcclusionCulling();

		//Mesh mBakedOccluderMesh{};
		/// <summary>
		/// Really simple Material
		/// Just Draw 
		/// </summary>
		//std::unique_ptr<Material> mOccluderMaterial{};

	
		/// <summary>
		/// draw Simple bounding volume(Occluder) after this function call
		/// TODO : Disable Pixel Shading, Texturing
		/// </summary>
		bool StartQuery(const std::uint32_t queryID);
		void StopQuery(const std::uint32_t queryID);

		/// <summary>
		/// Start Query and Draw AABB of Occludee
		/// You should pass World Space AABB Vertices
		/// 
		/// Quering AABB Data require a few times. 
		/// So Don't Do ConditionalRender instantly after Call Query Data!!!!!!!!!!!!!!!!!!!!!!!!!!! 
		/// Do other works between QueryOccludeeAABB and StartConditionalRender
		/// </summary>
		/// <param name="queryObject"></param>
		void QueryOccludeeAABB(const culling::QueryObject& queryObject);

	public:

		QueryOcclusionCulling(culling::EveryCulling* everyCulling);
		virtual ~QueryOcclusionCulling();

		/// <summary>
		/// 
		/// </summary>
		/// <param name="currentEntityBlock"></param>
		/// <param name="entityCountInBlock"></param>
		/// <param name="cameraIndex"></param>
		void DestroyQueryObject(culling::QueryObject* queryObject);
		void ClearEntityData(EntityBlock* currentEntityBlock, size_t entityIndex) override;

		void QueryOccludeeAABB();
	
		void GenQueryObject(EntityBlock* currentEntityBlock, size_t entityIndex, const culling::AABB & occlusionAABBLocalMinMax);
		void GenQueryObject(culling::EntityBlockViewer& entityBlockViewer, const culling::AABB & occlusionAABBLocalMinMax);

		virtual void CullBlockEntityJob
		(
			const size_t cameraIndex
		);

		/*
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
		FORCE_INLINE static void StartConditionalRender(std::uint32_t queryID)
		{
#ifdef CULLING_OPENGL
			assert(queryID > 0);
			// If query didn't contribute to sample count(AABB is hidden by other object),
			// After glBeginConditionalRender Every Rendering Commands will be ignored
			glBeginConditionalRender(queryID, GL_QUERY_BY_REGION_WAIT);
			// Test GL_QUERY_WAIT vs GL_QUERY_NO_WAIT
			// 
			// GL_QUERY_WAIT : If query isn't completed yet, Wait it
			// GL_QUERY_NO_WAIT : Don't wait until query complete. if query isn't completed just draw it normally
#elif CULLING_DIRECTX

#endif
		}
		FORCE_INLINE static void StopConditionalRender()
		{
#ifdef CULLING_OPENGL
			glEndConditionalRender();
#elif CULLING_DIRECTX

#endif
		}
		*/
	};

}



#endif