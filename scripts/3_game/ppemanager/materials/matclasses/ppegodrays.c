//!GodRays - PostProcessEffectType.GodRays
class PPEGodRays: PPEClassBase
{
	static const int PARAM_INTENSITY = 0;
	static const int PARAM_SUNVISIBLE = 1;
	static const int PARAM_OVERBURNINTENSITY = 2;
	static const int PARAM_OVERBURNSTART = 3;
	static const int PARAM_OVERBURNEND = 4;
	//static const int PARAM_SUNMASKMAT = 5; //Warning, not to be used until DECLARE_RESOURCE_NAME solution is implemented!
	
	static const int L_0_GLASSES = 100;
	
	override int GetPostProcessEffectID()
	{
		return PostProcessEffectType.GodRays;
	}
	
	override string GetDefaultMaterialPath()
	{
		return "Graphics/Materials/postprocess/godrayssun";
	}
	
	override void RegisterMaterialParameters()
	{
		RegisterParameterScalarFloat(PARAM_INTENSITY,"Intensity",0.8,0,1);
		RegisterParameterScalarFloat(PARAM_SUNVISIBLE,"SunVisible",1.0,0,1);
		RegisterParameterScalarFloat(PARAM_OVERBURNINTENSITY,"OverBurnIntensity",0.25,0,1);
		RegisterParameterScalarFloat(PARAM_OVERBURNSTART,"OverBurnStart",0.025,0,1);
		RegisterParameterScalarFloat(PARAM_OVERBURNEND,"OverBurnEnd",0.175,0,1);
		
		//TODO
		//SunMaskMat - DECLARE_RESOURCE_NAME
	}
}