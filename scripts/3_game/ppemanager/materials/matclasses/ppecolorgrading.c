//!ColorGrading - PostProcessEffectType.ColorGrading
//TODO
class PPEColorGrading: PPEClassBase
{
	//static const int PARAM_COLORTABLE = 0;
	
	override int GetPostProcessEffectID()
	{
		return PostProcessEffectType.ColorGrading;
	}
	
	override string GetDefaultMaterialPath()
	{
		return "Graphics/Materials/postprocess/colorgrading";
	}
	
	override void RegisterMaterialParameters()
	{
		//RegisterParameterTexture(PARAM_COLORTABLE,"ColorTable","Graphics/Textures/postprocess/color_test.edds");
	}
}