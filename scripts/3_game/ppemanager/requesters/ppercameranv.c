class PPERequester_CameraNV extends PPERequester_GameplayBase
{
	static const int NV_NO_BATTERY= 0; //darkness
	static const int NV_DEFAULT_OPTICS = 1;
	static const int NV_DEFAULT_GLASSES = 2;
	static const int NV_PUMPKIN = 3;
	static const int NV_TRANSITIVE = 4; //resets EV during camera transitions
	static const int NV_DAYTIME_OPTICS = 5;
	
	protected int m_CurrentMode;
	protected float m_UGExposureCoef = 1.0;
	
	protected void SetNVMode(int mode)
	{
		SetTargetValueFloat(PPEExceptions.EYEACCOM,PPEEyeAccomodationNative.PARAM_INTENSITY,false,1.0,PPEEyeAccomodationNative.L_0_NVG_GENERIC,PPOperators.SET);
		
		switch (mode)
		{
			case NV_NO_BATTERY: //battery off
				SetTargetValueColor(PostProcessEffectType.Glow,PPEGlow.PARAM_COLORIZATIONCOLOR,{0.0,0.0,0.0,0.0},PPEGlow.L_23_NVG,PPOperators.MULTIPLICATIVE);
				SetTargetValueFloatDefault(PPEExceptions.EXPOSURE,PPEExposureNative.PARAM_INTENSITY);
				SetTargetValueFloatDefault(PostProcessEffectType.FilmGrain,PPEFilmGrain.PARAM_SHARPNESS);
				SetTargetValueFloatDefault(PostProcessEffectType.FilmGrain,PPEFilmGrain.PARAM_GRAINSIZE);
				SetTargetValueFloat(PPEExceptions.NVLIGHTPARAMS,PPELightIntensityParamsNative.PARAM_LIGHT_MULT,false,1.0,PPELightIntensityParamsNative.L_0_NVG,PPOperators.HIGHEST);
				SetTargetValueFloat(PPEExceptions.NVLIGHTPARAMS,PPELightIntensityParamsNative.PARAM_NOISE_MULT,false,0.0,PPELightIntensityParamsNative.L_1_NVG,PPOperators.HIGHEST);
			break;
			
			case NV_DEFAULT_OPTICS: //NV optic on
				SetTargetValueColor(PostProcessEffectType.Glow,PPEGlow.PARAM_COLORIZATIONCOLOR,{0.0,1.0,0.0,0.0},PPEGlow.L_23_NVG,PPOperators.MULTIPLICATIVE);
				SetTargetValueFloat(PPEExceptions.EXPOSURE,PPEExposureNative.PARAM_INTENSITY,false,7.0 * m_UGExposureCoef,PPEExposureNative.L_0_NVG_OPTIC,PPOperators.ADD);
				SetTargetValueFloat(PostProcessEffectType.FilmGrain,PPEFilmGrain.PARAM_SHARPNESS,false,9.0,PPEFilmGrain.L_1_NVG,PPOperators.SET);
				SetTargetValueFloat(PostProcessEffectType.FilmGrain,PPEFilmGrain.PARAM_GRAINSIZE,false,1.0,PPEFilmGrain.L_2_NVG,PPOperators.SET);
				SetTargetValueFloat(PPEExceptions.NVLIGHTPARAMS,PPELightIntensityParamsNative.PARAM_LIGHT_MULT,false,3.0,PPELightIntensityParamsNative.L_0_NVG,PPOperators.HIGHEST);
				SetTargetValueFloat(PPEExceptions.NVLIGHTPARAMS,PPELightIntensityParamsNative.PARAM_NOISE_MULT,false,2.0,PPELightIntensityParamsNative.L_1_NVG,PPOperators.HIGHEST);
			break;
			
			case NV_DAYTIME_OPTICS: //NV optic in daytime mode
				SetTargetValueColor(PostProcessEffectType.Glow,PPEGlow.PARAM_COLORIZATIONCOLOR,{0.0,1.0,0.0,0.0},PPEGlow.L_23_NVG,PPOperators.MULTIPLICATIVE);
				SetTargetValueFloat(PPEExceptions.EXPOSURE,PPEExposureNative.PARAM_INTENSITY,false,2.0 * m_UGExposureCoef,PPEExposureNative.L_0_NVG_OPTIC,PPOperators.ADD);
				SetTargetValueFloat(PostProcessEffectType.FilmGrain,PPEFilmGrain.PARAM_SHARPNESS,false,9.0,PPEFilmGrain.L_1_NVG,PPOperators.SET);
				SetTargetValueFloat(PostProcessEffectType.FilmGrain,PPEFilmGrain.PARAM_GRAINSIZE,false,1.0,PPEFilmGrain.L_2_NVG,PPOperators.SET);
				SetTargetValueFloat(PPEExceptions.NVLIGHTPARAMS,PPELightIntensityParamsNative.PARAM_LIGHT_MULT,false,3.0,PPELightIntensityParamsNative.L_0_NVG,PPOperators.HIGHEST);
				SetTargetValueFloat(PPEExceptions.NVLIGHTPARAMS,PPELightIntensityParamsNative.PARAM_NOISE_MULT,false,1.0,PPELightIntensityParamsNative.L_1_NVG,PPOperators.HIGHEST);
			break;
			
			case NV_DEFAULT_GLASSES: //goggles on
				SetTargetValueColor(PostProcessEffectType.Glow,PPEGlow.PARAM_COLORIZATIONCOLOR,{0.0,1.0,0.0,0.0},PPEGlow.L_23_NVG,PPOperators.MULTIPLICATIVE);
				SetTargetValueFloat(PPEExceptions.EXPOSURE,PPEExposureNative.PARAM_INTENSITY,false,7.0 * m_UGExposureCoef,PPEExposureNative.L_0_NVG_GOGGLES,PPOperators.ADD);
				SetTargetValueFloat(PostProcessEffectType.FilmGrain,PPEFilmGrain.PARAM_SHARPNESS,false,10.0,PPEFilmGrain.L_1_NVG,PPOperators.SET);
				SetTargetValueFloat(PostProcessEffectType.FilmGrain,PPEFilmGrain.PARAM_GRAINSIZE,false,1.0,PPEFilmGrain.L_2_NVG,PPOperators.SET);
				SetTargetValueFloat(PPEExceptions.NVLIGHTPARAMS,PPELightIntensityParamsNative.PARAM_LIGHT_MULT,false,2.0,PPELightIntensityParamsNative.L_0_NVG,PPOperators.HIGHEST);
				SetTargetValueFloat(PPEExceptions.NVLIGHTPARAMS,PPELightIntensityParamsNative.PARAM_NOISE_MULT,false,1.0,PPELightIntensityParamsNative.L_1_NVG,PPOperators.HIGHEST);
			break;
			
			case NV_PUMPKIN: //pumpkin-o-vision
				SetTargetValueColor(PostProcessEffectType.Glow,PPEGlow.PARAM_COLORIZATIONCOLOR,{1.0,0.2,0.0,0.0},PPEGlow.L_23_NVG,PPOperators.MULTIPLICATIVE);
				SetTargetValueFloat(PPEExceptions.EXPOSURE,PPEExposureNative.PARAM_INTENSITY,false,5.5 * m_UGExposureCoef,PPEExposureNative.L_0_NVG_GOGGLES,PPOperators.ADD);
				SetTargetValueFloatDefault(PostProcessEffectType.FilmGrain,PPEFilmGrain.PARAM_SHARPNESS);
				SetTargetValueFloatDefault(PostProcessEffectType.FilmGrain,PPEFilmGrain.PARAM_GRAINSIZE);
				SetTargetValueFloat(PPEExceptions.NVLIGHTPARAMS,PPELightIntensityParamsNative.PARAM_LIGHT_MULT,false,1.0,PPELightIntensityParamsNative.L_0_NVG,PPOperators.HIGHEST);
				SetTargetValueFloat(PPEExceptions.NVLIGHTPARAMS,PPELightIntensityParamsNative.PARAM_NOISE_MULT,false,0.0,PPELightIntensityParamsNative.L_1_NVG,PPOperators.HIGHEST);
			break;
			
			case NV_TRANSITIVE: //camera transition
				//SetTargetValueFloat(PPEExceptions.EXPOSURE,PPEExposureNative.PARAM_INTENSITY,false,0,PPEExposureNative.L_0_NVG_OFF,PPOperators.SET);
				SetTargetValueFloatDefault(PPEExceptions.EXPOSURE,PPEExposureNative.PARAM_INTENSITY);
			break;
		}
		
		m_CurrentMode = mode;
	}
	
	override protected void OnStart(Param par = null)
	{
		super.OnStart();
		
		SetNVMode(Param1<int>.Cast(par).param1);
	}
	
	//other values reset automatically on stop
	override protected void OnStop(Param par = null)
	{
		super.OnStop(par);
	}
	
	void SetUndergroundExposureCoef(float coef)
	{
		m_UGExposureCoef = coef;
		
		if (m_IsRunning)
			SetNVMode(m_CurrentMode); //TODO: improve (oof...)
	}
}