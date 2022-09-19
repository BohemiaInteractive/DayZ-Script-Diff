//!FXAA - PostProcessEffectType.FXAA
class PPEFXAA: PPEClassBase
{
	static const int PARAM_PRESET = 0;
	
	override int GetPostProcessEffectID()
	{
		return PostProcessEffectType.FXAA;
	}
	
	override string GetDefaultMaterialPath()
	{
		return "Graphics/Materials/postprocess/fxaa";
	}
	
	override void RegisterMaterialParameters()
	{
		RegisterParameterScalarInt(PARAM_PRESET,"Preset",0,0,5);
	}
}