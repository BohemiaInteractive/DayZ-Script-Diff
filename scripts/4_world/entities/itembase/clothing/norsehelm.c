class NorseHelm extends HelmetBase
{
	override array<int> GetEffectWidgetTypes()
	{
		return { EffectWidgetsTypes.HELMET2_OCCLUDER };
	}

	override protected void InitGlobalExclusionValues()
	{
		super.InitGlobalExclusionValues();
		
		AddSingleExclusionValueGlobal(EAttExclusions.EXCLUSION_HEADGEAR_HELMET_0);
		
		AddSingleExclusionValueGlobal(EAttExclusions.EXCLUSION_MASK_0);
		AddSingleExclusionValueGlobal(EAttExclusions.EXCLUSION_MASK_1);
		AddSingleExclusionValueGlobal(EAttExclusions.EXCLUSION_MASK_2);
		AddSingleExclusionValueGlobal(EAttExclusions.EXCLUSION_MASK_3);
		AddSingleExclusionValueGlobal(EAttExclusions.EXCLUSION_HEADSTRAP_0);
	}
};
