//!UnderWater - PostProcessEffectType.UnderWater
class PPEUnderWater: PPEClassBase
{
	static const int PARAM_RSINTENSITY = 0;
	static const int PARAM_RSPLANEDIST = 1;
	static const int PARAM_RSDEPTHDECREASE = 2;
	static const int PARAM_RSUVSCALE = 3;
	static const int PARAM_WATERPLANELUM = 4;
	
	override int GetPostProcessEffectID()
	{
		return PostProcessEffectType.UnderWater;
	}
	
	override string GetDefaultMaterialPath()
	{
		return "Graphics/Materials/postprocess/uw";
	}
	
	override void RegisterMaterialParameters()
	{
		RegisterParameterScalarFloat(PARAM_RSINTENSITY,"RayShaftsIntensity",0.0,-0.2,0.2);
		RegisterParameterScalarFloat(PARAM_RSPLANEDIST,"RayShaftsPlaneDistance",1.0,0.1,4.0);
		RegisterParameterScalarFloat(PARAM_RSDEPTHDECREASE,"RayShaftsDepthDecrease",2.5,1.0,100.0);
		RegisterParameterScalarFloat(PARAM_RSUVSCALE,"RayShaftsUVScale",2.0,0.1,10.0);
		RegisterParameterScalarFloat(PARAM_WATERPLANELUM,"WaterPlaneLuminance",0.05,0.001,0.5);
	}
};