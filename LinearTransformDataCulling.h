#pragma once

#include "LinearTransformDataCulling_Core.h"
#include "DataStructure/EntityGridCell.h"
#include "DataStructure/EntityBlockViewer.h"

#include <Vector4.h>
#include <Matrix4x4.h>

#include <atomic>

namespace doom
{
	namespace graphics
	{
		struct SIMDFrustumPlanes
		{
			math::Vector4 mFrustumPlanes[8];
		};

		/// <summary>
		/// This is implementation of Data Oriented ViewFrustumCulling of Frostbite in 2011
		/// 
		/// This culling use SIMD DotProduct, So Check LightMath_Cpp/Matrix4x4Float_Aligned.inl
		///
		/// Feature :
		/// 1. Linear arrays scale great. All entity's position data is stored linearlly.
		/// 2. Tree based sturucture or other Acceleration strucutures isn;t required 
		/// 3. Frustum vs Sphere intersections check per loop
		/// 3. Threads solve intersections(blocks) parrallily
		/// 
		/// references : 
		/// https://www.gdcvault.com/play/1014491/Culling-the-Battlefield-Data-Oriented
		/// https://www.slideshare.net/DICEStudio/culling-the-battlefield-data-oriented-design-in-practice
		/// 
		/// 
		/// https://macton.smugmug.com/Other/2008-07-15-by-Eye-Fi/n-xmKDH/
		/// 
		/// </summary>
		class LinearTransformDataCulling
		{
		private:

			/// <summary>
			/// will be used at CullBlockEntityJob
			/// </summary>
			std::atomic<unsigned int> mAtomicCurrentBlockIndex;

			unsigned int mCameraCount;
			SIMDFrustumPlanes mSIMDFrustumPlanes[MAX_CAMERA_COUNT];

			bool bmIsInitializedEntityBlockPool{ false };
			EntityBlock* EntityBlockPool;

			EntityGridCell mEntityGridCell{};

			/// <summary>
			/// Block Swap removedblock with last block, and return swapped lastblock to pool
			/// </summary>
			void RemoveBlock();
			/// <summary>
			/// Remove Entity is nothing, Just decrement EntityCountInBlocks
			/// And if EntityCountInBlocks Of Block become zero, Remove the block using RemoveBlock function
			/// </summary>
			void RemoveEnitty();
		public:

			LinearTransformDataCulling();

			/// <summary>
			/// increment EntityCountInBlock of TargetBlock.
			/// If All blocks is full, Get new block from Block Pool
			/// </summary>
			EntityBlockViewer AllocateNewEntity();

			void UpdateFrustumPlane(unsigned int frustumPlaneIndex, const math::Matrix4x4& mvpMatrix);

			/// <summary>
			/// Solve View Frustum Culling from multiple threads
			/// </summary>
			void CullBlockEntityJob();
			void ClearIsVisibleFlag();
			void ResetAtomicCurrentBlockIndex();
		};
	}
}



