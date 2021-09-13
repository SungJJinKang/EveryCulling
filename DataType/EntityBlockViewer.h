#pragma once



namespace culling
{
	struct EntityBlock;
	class QueryOcclusionCulling;

	/// <summary>
	/// Used for storing specific EntityBlock pointer
	/// </summary>
	class EntityBlockViewer
	{
		friend class EveryCulling;
		friend class QueryOcclusionCulling;

	private:

		bool bmIsActive;
		EntityBlock* mTargetEntityBlock;
		/// <summary>
		/// Entity Index in Entity Block
		/// </summary>
		unsigned int mEntityIndexInBlock;

	public:

		EntityBlockViewer() = default;
		EntityBlockViewer(EntityBlock* entityBlock, unsigned int entityIndexInBlock)
			: mTargetEntityBlock{ entityBlock }, mEntityIndexInBlock{ entityIndexInBlock }, bmIsActive{ true }
		{}


		/// <summary>
		/// Get if entity is visible from Camera of parameter cameraIndex
		/// </summary>
		/// <param name="cameraIndex">0 <= cameraIndex < EveryCulling::mCameraCount</param>
		/// <returns></returns>
		bool GetIsCulled(const unsigned int cameraIndex) const;


		void SetEntityPosition(const float* worldPosition);

		void SetSphereBoundRadius(float sphereRadius);
	};
}
