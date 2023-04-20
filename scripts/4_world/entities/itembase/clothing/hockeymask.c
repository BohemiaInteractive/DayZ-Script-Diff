class HockeyMask extends Mask_Base
{
	override protected void InitGlobalExclusionValues()
	{
		super.InitGlobalExclusionValues();
		
		AddSingleExclusionValueGlobal(EAttachmentExclusionFlags.OCCUPANCY_ZONE_MASK_0);
		
		AddSingleExclusionValueGlobal(EAttachmentExclusionFlags.OCCUPANCY_ZONE_GLASSES_TIGHT_0);
		AddSingleExclusionValueGlobal(EAttachmentExclusionFlags.OCCUPANCY_ZONE_GLASSES_REGULAR_0);
	}
}