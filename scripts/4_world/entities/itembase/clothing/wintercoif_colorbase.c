class WinterCoif_ColorBase extends HeadGear_Base
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
	
	override protected set<int> GetAttachmentExclusionInitSlotValue(int slotId)
	{
		set<int> ret = super.GetAttachmentExclusionInitSlotValue(slotId);
		if (slotId == InventorySlots.HEADGEAR)
		{
			ret.Insert(EAttExclusions.EXCLUSION_MASK_3);
		}
		return ret;
	}
};
class WinterCoif_Black extends WinterCoif_ColorBase {};
class WinterCoif_Blue extends WinterCoif_ColorBase {};
class WinterCoif_Green extends WinterCoif_ColorBase {};
class WinterCoif_Skull extends WinterCoif_ColorBase {};
