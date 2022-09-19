//!Colors - PostProcessEffectType.Colors
//TODO - 'ColorsEffect' type may be used differently in c++, no emat linked to it? Investigate.
class PPEColors: PPEClassBase
{
	static const int PARAM_BRIGHTNESS = 0;
	static const int PARAM_CONTRAST = 1;
	static const int PARAM_OFFSET = 2;
	static const int PARAM_OVERLAYFACTOR = 3;
	static const int PARAM_OVERLAYCOLOR = 4;
	static const int PARAM_SATURATION = 5;
	static const int PARAM_COLORIZATIONCOLOR = 6;
	static const int PARAM_DESATURATIONWEIGHTS = 7;
	
	override int GetPostProcessEffectID()
	{
		return PostProcessEffectType.Colors;
	}
	
	override string GetDefaultMaterialPath()
	{
		return "Graphics/Materials/postprocess/colors";
	}
	
	override void RegisterMaterialParameters()
	{
		RegisterParameterScalarFloat(PARAM_BRIGHTNESS,"Brightness",1,0,2);
		RegisterParameterScalarFloat(PARAM_CONTRAST,"Contrast",1,0,2);
		RegisterParameterScalarFloat(PARAM_OFFSET,"Offset",0,-1,1);
		RegisterParameterScalarFloat(PARAM_OVERLAYFACTOR,"OverlayFactor",0,0,1);
		RegisterParameterColor(PARAM_OVERLAYCOLOR,"OverlayColor",1,1,1,0);
		RegisterParameterScalarFloatEx(PARAM_SATURATION,"Saturation",1,0,1,PPEMatClassParameterFloatSaturation);
		RegisterParameterColorEx(PARAM_COLORIZATIONCOLOR,"ColorizationColor",1.0,1.0,1.0,0.0,PPEMatClassParameterColorColorization);
		RegisterParameterVector(PARAM_DESATURATIONWEIGHTS,"DesaturationWeights",{0.299,0.587,0.114,0});
	}
}