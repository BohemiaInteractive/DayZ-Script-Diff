class Headdress_Colorbase extends HeadGear_Base
{
	override protected void InitGlobalExclusionValues()
	{
		super.InitGlobalExclusionValues();
		
		AddSingleExclusionValueGlobal(EAttExclusions.EXCLUSION_HEADSTRAP_0);
		AddSingleExclusionValueGlobal(EAttExclusions.EXCLUSION_MASK_2);
	}
};
class Headdress_Wolf extends Headdress_Colorbase {};
