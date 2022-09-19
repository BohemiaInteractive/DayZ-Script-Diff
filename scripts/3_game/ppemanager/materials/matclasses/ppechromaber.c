//!ChromAber - PostProcessEffectType.ChromAber
class PPEChromAber: PPEClassBase
{
	static const int PARAM_POWERX = 0;
	static const int PARAM_POWERY = 1;
	
	static const int L_0_INTRO = 100;
	static const int L_1_INTRO = 100;
	
	override int GetPostProcessEffectID()
	{
		return PostProcessEffectType.ChromAber;
	}
	
	override string GetDefaultMaterialPath()
	{
		return "Graphics/Materials/postprocess/chromaber";
	}
	
	override void RegisterMaterialParameters()
	{
		RegisterParameterScalarFloat(PARAM_POWERX,"PowerX",0.0,0.0,0.05);
		RegisterParameterScalarFloat(PARAM_POWERY,"PowerY",0.0,0.0,0.05);
	}
}