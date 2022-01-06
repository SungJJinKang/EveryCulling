#pragma once

#include "MaskedSWOcclusionCullingStage.h"


namespace culling
{
	class Tile;
	class RasterizeOccludersStage : public MaskedSWOcclusionCullingStage
	{
	private:

		std::array<std::atomic<size_t>, MAX_CAMERA_COUNT> mFinishedTileCount;
		
		

		/// <summary>
		/// https://www.slideshare.net/IntelSoftware/masked-software-occlusion-culling 46p
		///
		///	Shuffle CoverageMask
		///
		///	Original : One Row is configous 32bit ( 4byte )
		///	After shuffled : One 8 x 4 tile is configous 32bit ( 4byte )
		/// </summary>
		/// <param name="coverageMask"></param>
		/// <returns></returns>
		FORCE_INLINE culling::M256I ShuffleCoverageMask(const culling::M256I& coverageMask) const;

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
		void RasterizeBinnedTriangles(const size_t cameraIndex, culling::Tile* const tile);
		
		culling::Tile* GetNextDepthBufferTile(const size_t cameraIndex);
		culling::Tile* GetNextDepthBufferTileBatch(const size_t cameraIndex, const size_t batchCount);

	public:

		RasterizeOccludersStage(MaskedSWOcclusionCulling* mOcclusionCulling);

		void ResetCullingModule() override;
		void CullBlockEntityJob(const size_t cameraIndex) override;
		const char* GetCullingModuleName() const override;

		// TODO : Use linear depth!! ( float linearDepth = (2.0 * near * far) / (far + near - z * (far - near)) )
	};
}


