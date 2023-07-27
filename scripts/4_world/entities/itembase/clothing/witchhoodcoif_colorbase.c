class WitchHoodCoif_ColorBase: HeadGear_Base
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
		AddAction(ActionDeCraftWitchHoodCoif);
	}
	
	override protected set<int> GetAttachmentExclusionInitSlotValue(int slotId)
	{
		set<int> ret = super.GetAttachmentExclusionInitSlotValue(slotId);
		if (slotId == InventorySlots.HEADGEAR)
		{
			ret.Insert(EAttExclusions.LEGACY_MASK_HEADGEAR); //same as 'noMask' used to be
		}
		return ret;
	}
};
class WitchHoodCoif_Black: WitchHoodCoif_ColorBase {};
class WitchHoodCoif_Brown: WitchHoodCoif_ColorBase {};
class WitchHoodCoif_Red: WitchHoodCoif_ColorBase {};