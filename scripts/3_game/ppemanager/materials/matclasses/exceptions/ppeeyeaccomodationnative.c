//---------------------------------------------------------
//Native exceptions - legacy methods for direct access to specific postprocesses. Each one is evaluated and handled separately, this just connects them to the system.

//!Eye Accomodation postprocess, does not directly use materials
class PPEEyeAccomodationNative: PPEClassBase
{
	//g_Game.GetWorld().SetEyeAccom
	static const int PARAM_INTENSITY = 0;
	
	static const int L_0_UNDERGROUND = 100;
	static const int L_0_NVG_GENERIC = 300;
	/*static const int L_0_NVG_OPTIC = 400;
	static const int L_0_NVG_GOGGLES = 401;
	static const int L_0_NVG_OFF = 402;*/
	static const int L_0_BURLAP = 500;
	
	override int GetPostProcessEffectID()
	{
		return PPEExceptions.EYEACCOM;
	}
	
	override void RegisterMaterialParameters()
	{
		RegisterParameterScalarFloat(PARAM_INTENSITY,"Intensity",1.0,0.0,1000.0); //no known max. 1000 used as max
	}
	
	//! Overriden to handle the specific exception
	override void SetFinalParameterValue(int parameter_idx)
	{
		Param values = GetParameterCommandData(parameter_idx).GetCurrentValues();
		float value_var_float = Param1<float>.Cast(values).param1;
		
		g_Game.GetWorld().SetEyeAccom(value_var_float);
		//DbgPrnt("PPEDebug | SetFinalParameterValue | PPEEyeAccomodationNative | float val: " + value_var_float);
	}
}
