class DustGoggles_ColorBase : Glasses_Base {};
class DustGoggles_Transparent : DustGoggles_ColorBase{};
class DustGoggles_Black : DustGoggles_ColorBase
{
	override int GetGlassesEffectID()
	{
		return PPERequesterBank.REQ_GLASSESSPORTBLACK;
	}
};