class PaydayMask_ColorBase : Mask_Base
{
	override protected set<int> GetAttachmentExclusionInitSlotValue(int slotId)
	{
		set<int> ret = super.GetAttachmentExclusionInitSlotValue(slotId);
		if (slotId == InventorySlots.MASK)
		{
			ret.Insert(EAttExclusions.LEGACY_HEADGEAR_MASK); //same as 'noHeadgear' used to be
		}
		return ret;
	}
};

class PaydayMask_Dallas : PaydayMask_ColorBase{};
class PaydayMask_Hoxton : PaydayMask_ColorBase{};
class PaydayMask_Wolf : PaydayMask_ColorBase{};
class PaydayMask_Chains : PaydayMask_ColorBase{};