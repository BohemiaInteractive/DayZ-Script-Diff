//---------------------------------------------------------
//Native exceptions - legacy methods for direct access to specific postprocesses. Each one is evaluated and handled separately, this just connects them to the system.

//!DOF postprocess, does not directly use materials
class PPEDOF: PPEClassBase
{
	//g_Game.OverrideDOF(bool enable, float focusDistance, float focusLength, float focusLengthNear, float blur, float focusDepthOffset);
	static const int PARAM_ENABLE = 0;
	static const int PARAM_FOCUS_DIST = 1;
	static const int PARAM_FOCUS_LEN = 2;
	static const int PARAM_FOCUS_LEN_NEAR = 3;
	static const int PARAM_BLUR = 4;
	static const int PARAM_FOCUS_DEPTH_OFFSET = 5;
	
	static const int L_0_ADS = 100;
	static const int L_1_ADS = 100;
	static const int L_2_ADS = 100;
	static const int L_3_ADS = 100;
	static const int L_4_ADS = 100;
	static const int L_5_ADS = 100;
	
	override int GetPostProcessEffectID()
	{
		return PPEExceptions.DOF;
	}
	
	override void RegisterMaterialParameters()
	{
		 //no known max. 1000 used as max
		RegisterParameterScalarBool(PARAM_ENABLE,"Enable",false);
		
		RegisterParameterScalarFloat(PARAM_FOCUS_DIST,"FocusDistance",2.0,0.0,1000.0); //2.0f used in code with null camera interpolation 
		RegisterParameterScalarFloat(PARAM_FOCUS_LEN,"FocusLength",-1.0,-1.0,1000.0); //-1.0f used as code default
		RegisterParameterScalarFloat(PARAM_FOCUS_LEN_NEAR,"FocusLengthNear",-1.0,-1.0,1000.0); //-1.0f used as code default
		RegisterParameterScalarFloat(PARAM_BLUR,"Blur",1.0,0.0,1000.0); //1.0f used in code with null camera interpolation 
		RegisterParameterScalarFloat(PARAM_FOCUS_DEPTH_OFFSET,"FocusDepthOffset",0.0,0.0,1000.0); //0.0f used as code default
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
		Param enabled_par = GetParameterCommandData(PARAM_ENABLE).GetCurrentValues();
		bool is_enabled = Param1<bool>.Cast(enabled_par).param1;
		
		if (is_enabled)
		{
			array<float> array_values = {-1.0};
			
			for (int i = 1; i < PARAM_FOCUS_DEPTH_OFFSET + 1; i++)
			{
				Param values = GetParameterCommandData(i).GetCurrentValues();
				float value_var_float = Param1<float>.Cast(values).param1;
				array_values.Insert(value_var_float);
			}
			
			g_Game.OverrideDOF(true, array_values.Get(PARAM_FOCUS_DIST), array_values.Get(PARAM_FOCUS_LEN), array_values.Get(PARAM_FOCUS_LEN_NEAR), array_values.Get(PARAM_BLUR), array_values.Get(PARAM_FOCUS_DEPTH_OFFSET));
		}
		else
		{
			g_Game.OverrideDOF(false,0.0,0.0,0.0,0.0,1.0);
		}
	}
}
