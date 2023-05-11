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
		ClearSingleExclusionValueGlobal(EAttExclusions.EXCLUSION_GLASSES_REGULAR_0);
		AddSingleExclusionValueGlobal(EAttExclusions.EXCLUSION_HEADSTRAP_0);
	}
};