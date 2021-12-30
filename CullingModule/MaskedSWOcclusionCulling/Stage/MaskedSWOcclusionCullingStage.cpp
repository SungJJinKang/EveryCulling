#include "MaskedSWOcclusionCullingStage.h"

#include "../../MaskedSWOcclusionCulling/MaskedSWOcclusionCulling.h"

culling::MaskedSWOcclusionCullingStage::MaskedSWOcclusionCullingStage
(
	MaskedSWOcclusionCulling* const maskedSWOcclusionCulling
)
: mMaskedOcclusionCulling{ maskedSWOcclusionCulling }, CullingModule(maskedSWOcclusionCulling->mEveryCulling)
{

}

const char* culling::MaskedSWOcclusionCullingStage::GetCullingModuleName() const
{
	return "MaskedSWOcclusionCullingStage";
}
