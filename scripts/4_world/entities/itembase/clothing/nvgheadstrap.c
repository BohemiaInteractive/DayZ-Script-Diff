class NVGHeadstrap extends Glasses_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionToggleNVG);
	}
	
	override protected void InitGlobalExclusionValues()
	{
		super.InitGlobalExclusionValues();
		ClearSingleExclusionValueGlobal(EAttachmentExclusionFlags.OCCUPANCY_ZONE_GLASSES_REGULAR_0);
		AddSingleExclusionValueGlobal(EAttachmentExclusionFlags.OCCUPANCY_ZONE_HEADSTRAP_0);
	}
};