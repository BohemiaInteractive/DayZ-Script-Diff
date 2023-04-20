class NorseHelm extends HelmetBase
{
	override array<int> GetEffectWidgetTypes()
	{
		return { EffectWidgetsTypes.HELMET2_OCCLUDER };
	}

	override protected void InitGlobalExclusionValues()
	{
		super.InitGlobalExclusionValues();
		
		AddSingleExclusionValueGlobal(EAttachmentExclusionFlags.OCCUPANCY_ZONE_HEADGEAR_HELMET_0);
		
		AddSingleExclusionValueGlobal(EAttachmentExclusionFlags.OCCUPANCY_ZONE_MASK_0);
		AddSingleExclusionValueGlobal(EAttachmentExclusionFlags.OCCUPANCY_ZONE_MASK_1);
		AddSingleExclusionValueGlobal(EAttachmentExclusionFlags.OCCUPANCY_ZONE_MASK_2);
		AddSingleExclusionValueGlobal(EAttachmentExclusionFlags.OCCUPANCY_ZONE_MASK_3);
		AddSingleExclusionValueGlobal(EAttachmentExclusionFlags.OCCUPANCY_ZONE_HEADSTRAP_0);
	}
};
