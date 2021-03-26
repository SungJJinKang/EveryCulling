#pragma once

#include "LinearViewFrustumCulling_Core.h"
#include "DataStructure/EntityGridCell.h"

namespace doom
{
	namespace graphics
	{

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
		class LinearViewFrustumCulling
		{
		private:

			bool bmIsInitializedEntityBlockPool{ false };
			EntityBlock* EntityBlockPool;

			EntityGridCell mEntityGridCell{};
		public:

			LinearViewFrustumCulling();

			void AllocateNewEntity();
		};
	}
}



