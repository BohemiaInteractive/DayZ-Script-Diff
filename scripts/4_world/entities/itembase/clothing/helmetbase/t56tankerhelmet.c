class T56TankerHelmet_Colorbase : HelmetBase 
{
	override protected set<int> GetAttachmentExclusionInitSlotValue(int slotId)
	{
		set<int> ret = super.GetAttachmentExclusionInitSlotValue(slotId);
		if (slotId == InventorySlots.HEADGEAR)
		{
			ret.Insert(EAttExclusions.EXCLUSION_MASK_2);
		}
		return ret;
	}
};
class T56TankerHelmet_Tan : T56TankerHelmet_Colorbase {};
class T56TankerHelmet_Olive : T56TankerHelmet_Colorbase {};
