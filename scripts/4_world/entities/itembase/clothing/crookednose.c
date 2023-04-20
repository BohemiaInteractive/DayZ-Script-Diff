class CrookedNose extends Mask_Base
{
	override protected void InitGlobalExclusionValues()
	{
		super.InitGlobalExclusionValues();
		
		AddSingleExclusionValueGlobal(EAttachmentExclusionFlags.OCCUPANCY_ZONE_MASK_0);
		AddSingleExclusionValueGlobal(EAttachmentExclusionFlags.OCCUPANCY_ZONE_MASK_1);
		AddSingleExclusionValueGlobal(EAttachmentExclusionFlags.OCCUPANCY_ZONE_MASK_2);
		
		AddSingleExclusionValueGlobal(EAttachmentExclusionFlags.OCCUPANCY_ZONE_HEADGEAR_HELMET_0);
	}
}
