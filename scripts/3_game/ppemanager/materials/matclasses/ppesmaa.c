//!SMAA - PostProcessEffectType.SMAA
class PPESMAA: PPEClassBase
{
	static const int PARAM_PRESET = 0;
	
	override int GetPostProcessEffectID()
	{
		return PostProcessEffectType.SMAA;
	}
	
	override string GetDefaultMaterialPath()
	{
		return "Graphics/Materials/postprocess/smaa";
	}
	
	override void RegisterMaterialParameters()
	{
		RegisterParameterScalarInt(PARAM_PRESET,"Preset",0,0,3);
	}
}