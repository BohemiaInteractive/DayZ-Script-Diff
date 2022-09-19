//!DynamicBlur - PostProcessEffectType.DynamicBlur
class PPEDynamicBlur: PPEClassBase
{
	static const int PARAM_BLURRINESS = 0;
	
	override int GetPostProcessEffectID()
	{
		return PostProcessEffectType.DynamicBlur;
	}
	
	override string GetDefaultMaterialPath()
	{
		return "Graphics/Materials/postprocess/dynamicblur";
	}
	
	override void RegisterMaterialParameters()
	{
		RegisterParameterScalarFloat(PARAM_BLURRINESS,"Blurriness",20,0,30);
	}
}