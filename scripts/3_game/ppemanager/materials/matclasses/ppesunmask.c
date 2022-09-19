//!SunMask - PostProcessEffectType.SunMask
//A dummy class in script; 'SunMaskEffect' is used as a part of Rain and GodRays
class PPESunMask: PPEClassBase
{
	/*
	static const int PARAM_INTENSITY = 0;
	static const int PARAM_SUNSIZE = 1;
	static const int PARAM_VSTREAKINT = 2;
	static const int PARAM_DSTREAKINT = 3;
	static const int PARAM_SUNMASK = 4;
	*/
	override int GetPostProcessEffectID()
	{
		return PostProcessEffectType.SunMask;
	}
	
	override string GetDefaultMaterialPath()
	{
		return "";
	}
	
	/*override void RegisterMaterialParameters()
	{
		RegisterParameterScalarFloat(PARAM_INTENSITY,"Intensity",0.8,0.0,1.0);
		RegisterParameterScalarFloat(PARAM_SUNSIZE,"SunSize",0.2,0.0,1.0);
		RegisterParameterScalarFloat(PARAM_VSTREAKINT,"VerticalStreakIntensity",0.75,0.0,5.0);
		RegisterParameterScalarFloat(PARAM_DSTREAKINT,"DiagonalStreakIntensity",0.6,0.0,5.0);
		//RegisterParameterResource(PARAM_SUNMASK,"SunMask","");
	}*/
}