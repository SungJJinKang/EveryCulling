#pragma once

namespace doom
{
	namespace graphics
	{
		struct EntityBlock;
		struct EntityBlockViewer
		{
			EntityBlock* mTargetEntityBlock;
			unsigned int mEntityIndexInBlock;
		};
	}
}