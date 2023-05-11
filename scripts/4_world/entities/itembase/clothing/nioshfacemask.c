class NioshFaceMask: Mask_Base
{
	override protected void InitGlobalExclusionValues()
	{
		super.InitGlobalExclusionValues();
		
		AddSingleExclusionValueGlobal(EAttExclusions.EXCLUSION_MASK_0);
		AddSingleExclusionValueGlobal(EAttExclusions.EXCLUSION_MASK_1);
		
		AddSingleExclusionValueGlobal(EAttExclusions.EXCLUSION_HEADGEAR_HELMET_0);
	}
}
