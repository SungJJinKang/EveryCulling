#pragma once

#include "MaskedSWOcclusionCullingStage.h"

namespace culling
{
	class RasterizeTrianglesStage : public MaskedSWOcclusionCullingStage
	{
	
	private:

		

		/// <summary>
		/// 
		/// </summary>
		void UpdateHierarchicalDepthBuffer()
		{
			/*
			// Discard working layer heuristic
			dist1t = tile.zMax1 - tri.zMax
			dist01 = tile.zMax0 - tile.zMax1
			if (dist1t > dist01)
				tile.zMax1 = 0
				tile.mask = 0
			// Merge current triangle into working layer
			tile.zMax1 = max(tile.zMax1, tri.zMax)
			tile.mask |= tri.coverageMask
			// Overwrite ref. layer if working layer full
			if (tile.mask == ~0)
				tile.zMax0 = tile.zMax1
				tile.zMax1 = 0
				tile.mask = 0
			*/
		}

		/// <summary>
		/// Real Time Redering 3rd ( 873p )
		/// </summary>
		void ComputeTrianglesDepthValueInTile();

		/// <summary>
		/// Compute Depth in Bin of Tile(Sub Tile)
		/// 
		/// CoverageMask, z0DepthMax, z1DepthMax, Triangle Max Depth
		/// 
		/// reference : 
		/// https://stackoverflow.com/questions/24441631/how-exactly-does-opengl-do-perspectively-correct-linear-interpolation
		/// https://www.rose-hulman.edu/class/cs/csse351/m10/triangle_fill.pdf
		/// https://www.comp.nus.edu.sg/~lowkl/publications/lowk_persp_interp_techrep.pdf
		/// </summary>
		void RasterizeBinnedTriangles()
		{
			// Compute Min Depth In Tile(SubTile)
			// ComputeTrianglesDepthValueInTile

			// UpdateHierarchicalDepthBuffer
		}


		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Occludee Depth Test
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		culling::M256F ComputeMinimumDepths
		(
			const culling::M256F* worldAABBVertX,
			const culling::M256F* worldAABBVertY,
			const culling::M256F* worldAABBVertZ
		)
		{
		}

	public:

		RasterizeTrianglesStage(MaskedSWOcclusionCulling* mOcclusionCulling);

		void CullBlockEntityJob(const size_t cameraIndex) override;
	};
}


