//!GaussFilter - PostProcessEffectType.GaussFilter
class PPEGaussFilter: PPEClassBase
{
	static const int PARAM_INTENSITY = 0;
	
	static const int L_0_ADS = 100;
	static const int L_0_SHOCK = 200;
	static const int L_0_FEVER = 300;
	static const int L_0_FLASHBANG = 400;
	static const int L_0_INV = 500;
	static const int L_0_MENU = 600;
	static const int L_0_CONTROLS = 700;
	static const int L_0_TUTORIALS = 800;
	static const int L_0_SERVER_BROWSER = 900;
	static const int L_0_DISCONNECT = 1000;
	
	override int GetPostProcessEffectID()
	{
		return PostProcessEffectType.GaussFilter;
	}
	
	override string GetDefaultMaterialPath()
	{
		return "Graphics/Materials/postprocess/gauss";
	}
	
	override void RegisterMaterialParameters()
	{
		RegisterParameterScalarFloat(PARAM_INTENSITY,"Intensity",0.0,0.0,1.0);
	}
}