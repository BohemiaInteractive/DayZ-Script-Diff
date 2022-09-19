//---------------------------------------------------------
//Native exceptions - legacy methods for direct access to specific postprocesses. Each one is evaluated and handled separately, this just connects them to the system.

//!g_Game.NightVissionLightParams, does not directly use materials. Controls light multiplication and filmgrain noise intensity (multiplier)
class PPELightIntensityParamsNative: PPEClassBase
{
	//g_Game.SetEVValue
	static const int PARAM_LIGHT_MULT = 0;
	static const int PARAM_NOISE_MULT = 1;
	
	static const int L_0_NVG = 100;
	static const int L_0_TOXIC_TINT = 200;
	static const int L_1_NVG = 100;
	static const int L_1_TOXIC_TINT = 200;
	
	override int GetPostProcessEffectID()
	{
		return PPEExceptions.NVLIGHTPARAMS;
	}
	
	override void RegisterMaterialParameters()
	{
		RegisterParameterScalarFloat(PARAM_LIGHT_MULT,"lightIntensityMul",1.0,0.0,50.0); //some reasonable values
		RegisterParameterScalarFloat(PARAM_NOISE_MULT,"noiseIntensity",0.0,0.0,50.0); //some reasonable values
	}
	
	override void ApplyValueChanges()
	{
		if (m_UpdatedParameters.Count() > 0)
		{
			SetFinalParameterValue(-1); //unique handling
		}
		
		m_UpdatedParameters.Clear();
	}
	
	//! Overriden to handle the specific exception
	override void SetFinalParameterValue(int parameter_idx)
	{
		array<float> array_values = new array<float>;
		
		for (int i = 0; i < PARAM_NOISE_MULT + 1; i++)
		{
			Param values = GetParameterCommandData(i).GetCurrentValues();
			float value_var_float = Param1<float>.Cast(values).param1;
			array_values.Insert(value_var_float);
		}
		
		g_Game.NightVissionLightParams(array_values.Get(PARAM_LIGHT_MULT),array_values.Get(PARAM_NOISE_MULT));
		
		//DbgPrnt("PPEDebug | SetFinalParameterValue | PPELightIntensityParamsNative | float val: " + value_var_float);
	}
}