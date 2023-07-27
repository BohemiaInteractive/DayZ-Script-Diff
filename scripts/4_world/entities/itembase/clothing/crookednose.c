class CrookedNose extends Mask_Base
{
	override protected void InitGlobalExclusionValues()
	{
		super.InitGlobalExclusionValues();
		
		AddSingleExclusionValueGlobal(EAttExclusions.EXCLUSION_MASK_0);
		AddSingleExclusionValueGlobal(EAttExclusions.EXCLUSION_MASK_1);
		AddSingleExclusionValueGlobal(EAttExclusions.EXCLUSION_MASK_2);
		
		AddSingleExclusionValueGlobal(EAttExclusions.EXCLUSION_HEADGEAR_HELMET_0);
	}
	
	override protected set<int> GetAttachmentExclusionInitSlotValue(int slotId)
	{
		set<int> ret = super.GetAttachmentExclusionInitSlotValue(slotId);
		
		//allows shaving
		ret.RemoveItem(EAttExclusions.SHAVING_MASK_ATT_0);
		return ret;
	}
}
