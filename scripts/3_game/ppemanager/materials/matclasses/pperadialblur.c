//!RadialBlur - PostProcessEffectType.RadialBlur
class PPERadialBlur: PPEClassBase
{
	static const int PARAM_POWERX = 0;
	static const int PARAM_POWERY = 1;
	static const int PARAM_OFFSETX = 2;
	static const int PARAM_OFFSETY = 3;
	static const int PARAM_PIXELSCALE = 4;
	
	
	static const int L_0_PAIN_BLUR = 100;
	
	
	override int GetPostProcessEffectID()
	{
		return PostProcessEffectType.RadialBlur;
	}
	
	override string GetDefaultMaterialPath()
	{
		return "Graphics/Materials/postprocess/radialblur";
	}
	
	override void RegisterMaterialParameters()
	{
		RegisterParameterScalarFloat(PARAM_POWERX,"PowerX",0.0,0.0,0.1);
		RegisterParameterScalarFloat(PARAM_POWERY,"PowerY",0.0,0.0,0.1);
		RegisterParameterScalarFloat(PARAM_OFFSETX,"OffsetX",0.05,0.0,0.5);
		RegisterParameterScalarFloat(PARAM_OFFSETY,"OffsetY",0.05,0.0,0.5);
		RegisterParameterScalarFloat(PARAM_PIXELSCALE,"PixelScale",0.5,0.125,1.0);
	}
}