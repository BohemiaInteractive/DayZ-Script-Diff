//!HBAO - PostProcessEffectType.HBAO
//TODO - may be just a dummy, since SSAO already uses the HBAO materials?
class PPEHBAO: PPEClassBase
{
	static const int PARAM_RADIUSMETERS = 0;
	static const int PARAM_INTENSITY = 1;
	static const int PARAM_BLURSHARPNESS = 2;
	static const int PARAM_NDOTVBIAS = 3;
	static const int PARAM_SMALLSCALEAO = 4;
	static const int PARAM_LARGESCALEAO = 5;
	static const int PARAM_NUMDIRECTIONS = 6;
	static const int PARAM_NUMSAMPLES = 7;
	static const int PARAM_DEINTERLEAVE = 8;
	
	override int GetPostProcessEffectID()
	{
		return PostProcessEffectType.HBAO;
	}
	
	override string GetDefaultMaterialPath()
	{
		return "Graphics/Materials/postprocess/hbao";
	}
	
	override void RegisterMaterialParameters()
	{
		RegisterParameterScalarFloat(PARAM_RADIUSMETERS,"RadiusMeters",2.0,0.1,10.0);
		RegisterParameterScalarFloat(PARAM_INTENSITY,"Intensity",1.0,0.1,100.0);
		RegisterParameterScalarFloat(PARAM_BLURSHARPNESS,"BlurSharpness",32.0,1.0,300.0);
		RegisterParameterScalarFloat(PARAM_NDOTVBIAS,"NdotVBias",0.2,0.0,0.5);
		RegisterParameterScalarFloat(PARAM_SMALLSCALEAO,"SmallScaleAO",1.0,0.1,2.0);
		RegisterParameterScalarFloat(PARAM_LARGESCALEAO,"LargeScaleAO",1.0,0.1,2.0);
		RegisterParameterScalarInt(PARAM_NUMDIRECTIONS,"NumDirections",8,1,8);
		RegisterParameterScalarInt(PARAM_NUMSAMPLES,"NumSamples",4,1,8);
		RegisterParameterScalarBool(PARAM_DEINTERLEAVE,"Deinterleave",true);
	}
};