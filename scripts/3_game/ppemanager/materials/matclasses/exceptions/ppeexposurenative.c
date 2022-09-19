//---------------------------------------------------------
//Native exceptions - legacy methods for direct access to specific postprocesses. Each one is evaluated and handled separately, this just connects them to the system.

//!EV postprocess, does not directly use materials
class PPEExposureNative: PPEClassBase
{
	//g_Game.SetEVValue
	static const int PARAM_INTENSITY = 0;
	
	static const int L_0_NVG_OPTIC = 100;
	static const int L_0_NVG_GOGGLES = 200;
	static const int L_0_NVG_OFF = 300;
	static const int L_0_FLASHBANG = 400;
	static const int L_0_BURLAP = 500;
	static const int L_0_DEATH = 1000;
	
	override int GetPostProcessEffectID()
	{
		return PPEExceptions.EXPOSURE;
	}
	
	override void RegisterMaterialParameters()
	{
		RegisterParameterScalarFloat(PARAM_INTENSITY,"Intensity",0.0,-100.0,100.0); //no real range, it seems. -100..100 used
	}
	
	//! Overriden to handle the specific exception
	override void SetFinalParameterValue(int parameter_idx)
	{
		Param values = GetParameterCommandData(parameter_idx).GetCurrentValues();
		float value_var_float = Param1<float>.Cast(values).param1;
		
		g_Game.SetEVValue(value_var_float);
		//DbgPrnt("PPEDebug | SetFinalParameterValue | PPEExposureNative | float val: " + value_var_float);
	}
}