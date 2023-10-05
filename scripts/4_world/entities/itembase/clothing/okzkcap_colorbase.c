class OKZKCap_ColorBase extends HeadGear_Base
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
			ret.Insert(EAttExclusions.EXCLUSION_MASK_0);
			ret.Insert(EAttExclusions.EXCLUSION_MASK_1);
			ret.Insert(EAttExclusions.EXCLUSION_MASK_2);
			ret.Insert(EAttExclusions.EXCLUSION_MASK_3);
			
			ret.Insert(EAttExclusions.SHAVING_HEADGEAR_ATT_0);
		}
		return ret;
	}
};
class OKZKCap_Beige extends OKZKCap_ColorBase {};
class OKZKCap_Green extends OKZKCap_ColorBase {};
