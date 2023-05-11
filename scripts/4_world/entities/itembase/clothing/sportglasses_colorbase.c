class SportGlasses_ColorBase extends Glasses_Base 
{
	override protected void InitGlobalExclusionValues()
	{
		super.InitGlobalExclusionValues();
		ClearSingleExclusionValueGlobal(EAttExclusions.EXCLUSION_GLASSES_REGULAR_0);
		AddSingleExclusionValueGlobal(EAttExclusions.EXCLUSION_GLASSES_TIGHT_0);
		
		AddSingleExclusionValueGlobal(EAttExclusions.EXCLUSION_MASK_2); //TODO: check consistent mask conflict
	}
};

class SportGlasses_Black extends SportGlasses_ColorBase
{
	override int GetGlassesEffectID()
	{
		return PPERequesterBank.REQ_GLASSESSPORTBLACK;
	}
};

class SportGlasses_Blue extends SportGlasses_ColorBase
{
	override int GetGlassesEffectID()
	{
		return PPERequesterBank.REQ_GLASSESSPORTBLUE;
	}
};

class SportGlasses_Green extends SportGlasses_ColorBase
{
	override int GetGlassesEffectID()
	{
		return PPERequesterBank.REQ_GLASSESSPORTGREEN;
	}
};

class SportGlasses_Orange extends SportGlasses_ColorBase
{
	override int GetGlassesEffectID()
	{
		return PPERequesterBank.REQ_GLASSESSPORTORANGE;
	}
};