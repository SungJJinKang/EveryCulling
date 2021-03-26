#include "LinearViewFrustumCulling.h"

#include "DataStructure/EntityBlock.h"

doom::graphics::LinearViewFrustumCulling::LinearViewFrustumCulling()
{
	this->EntityBlockPool = new EntityBlock[MAX_ENTITY_BLOCK_COUNT];
	this->bmIsInitializedEntityBlockPool = true;
}
