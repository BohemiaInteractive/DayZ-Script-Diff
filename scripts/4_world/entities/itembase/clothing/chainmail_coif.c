class Chainmail_Coif extends HeadGear_Base
{
	override protected set<int> GetAttachmentExclusionInitSlotValue(int slotId)
	{
		set<int> ret = super.GetAttachmentExclusionInitSlotValue(slotId);
		if (slotId == InventorySlots.HEADGEAR)
		{
			ret.Insert(EAttExclusions.LEGACY_MASK_HEADGEAR); //same as 'noMask' used to be
			
			ret.Insert(EAttExclusions.SHAVING_HEADGEAR_ATT_0);
		}
		return ret;
	}
};
