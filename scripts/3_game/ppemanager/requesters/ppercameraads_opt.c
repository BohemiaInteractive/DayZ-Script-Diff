class PPERequester_CameraADS extends PPERequester_GameplayBase
{
	void SetValuesOptics(out array<float> mask_array, out array<float> lens_array, float gauss = 0.0)
	{
		//mask
		GetGame().ResetPPMask();
		if (mask_array.Count() != 4)
		{
			mask_array = {0.0,0.0,0.0,0.0};
		}
		GetGame().AddPPMask(mask_array[0], mask_array[1], mask_array[2], mask_array[3]);
		
		//lens
		if (lens_array.Count() != 4)
		{
			lens_array = {0.0,0.0,0.0,0.0};
		}
		SetTargetValueFloat(PostProcessEffectType.Glow,PPEGlow.PARAM_LENSDISTORT,false,lens_array[0],PPEGlow.L_27_ADS,PPOperators.SET);
		SetTargetValueFloat(PostProcessEffectType.Glow,PPEGlow.PARAM_MAXCHROMABBERATION,false,lens_array[1],PPEGlow.L_28_ADS,PPOperators.SET);
		SetTargetValueFloat(PostProcessEffectType.Glow,PPEGlow.PARAM_LENSCENTERX,false,lens_array[2],PPEGlow.L_29_ADS,PPOperators.SET);
		SetTargetValueFloat(PostProcessEffectType.Glow,PPEGlow.PARAM_LENSCENTERY,false,lens_array[3],PPEGlow.L_30_ADS,PPOperators.SET);
		
		//DOF - no DOF in optics
		SetTargetValueBoolDefault(PPEExceptions.DOF,PPEDOF.PARAM_ENABLE);
		
		//blur
		SetTargetValueFloat(PostProcessEffectType.GaussFilter,PPEGaussFilter.PARAM_INTENSITY,false,gauss,PPEGaussFilter.L_0_ADS,PPOperators.ADD_RELATIVE);
	}
	
	void SetValuesIronsights(out array<float> DOF_array)
	{
		//mask - no mask
		GetGame().ResetPPMask();
		
		//lens - no lens
		SetTargetValueFloatDefault(PostProcessEffectType.Glow,PPEGlow.PARAM_LENSDISTORT);
		SetTargetValueFloatDefault(PostProcessEffectType.Glow,PPEGlow.PARAM_MAXCHROMABBERATION);
		SetTargetValueFloatDefault(PostProcessEffectType.Glow,PPEGlow.PARAM_LENSCENTERX);
		SetTargetValueFloatDefault(PostProcessEffectType.Glow,PPEGlow.PARAM_LENSCENTERY);
		
		//DOF
		SetTargetValueBool(PPEExceptions.DOF,PPEDOF.PARAM_ENABLE,DOF_array[0],PPEDOF.L_0_ADS,PPOperators.SET);
		SetTargetValueFloat(PPEExceptions.DOF,PPEDOF.PARAM_FOCUS_DIST,false,DOF_array[1],PPEDOF.L_1_ADS,PPOperators.SET);
		SetTargetValueFloat(PPEExceptions.DOF,PPEDOF.PARAM_FOCUS_LEN,false,DOF_array[2],PPEDOF.L_2_ADS,PPOperators.SET);
		SetTargetValueFloat(PPEExceptions.DOF,PPEDOF.PARAM_FOCUS_LEN_NEAR,false,DOF_array[3],PPEDOF.L_3_ADS,PPOperators.SET);
		SetTargetValueFloat(PPEExceptions.DOF,PPEDOF.PARAM_BLUR,false,DOF_array[4],PPEDOF.L_4_ADS,PPOperators.SET);
		SetTargetValueFloat(PPEExceptions.DOF,PPEDOF.PARAM_FOCUS_DEPTH_OFFSET,false,DOF_array[5],PPEDOF.L_5_ADS,PPOperators.SET);
		
		//blur - no gauss blur (DOF bokeh only)
		SetTargetValueFloatDefault(PostProcessEffectType.GaussFilter,PPEGaussFilter.PARAM_INTENSITY);
	}
	
	override protected void OnStop(Param par = null)
	{
		super.OnStop(par);
		
		if ( !GetGame() )
			return;
		
		GetGame().ResetPPMask();
		//material parameter values reset by default on stop
	}
}