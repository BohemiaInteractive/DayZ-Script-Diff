class SkiGoggles_ColorBase : Glasses_Base {};
class SkiGoggles_WhiteClear : SkiGoggles_ColorBase{};
class SkiGoggles_WhiteDark : SkiGoggles_ColorBase
{
	override int GetGlassesEffectID()
	{
		return PPERequesterBank.REQ_GLASSESSPORTBLACK;
	}
};

class SkiGoggles_BlackYellow : SkiGoggles_ColorBase
{
	override int GetGlassesEffectID()
	{
		return PPERequesterBank.REQ_GLASSESSPORTGREEN;
	}
};