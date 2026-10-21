class PPERequester_CameraNV extends PPERequester_GameplayBase
{
	static const int NV_NO_BATTERY	  = 0; //darkness
	static const int NV_DEFAULT_OPTICS  = 1;
	static const int NV_DEFAULT_GLASSES = 2;
	static const int NV_PUMPKIN		 = 3;
	static const int NV_TRANSITIVE	  = 4; //resets EV during camera transitions
	static const int NV_DAYTIME_OPTICS  = 5;

	// ---- New OPTICS condition variants -----------------------------------------
	static const int NV_UNDERGROUND_OPTICS			= 6;
	static const int NV_SANDSTORM_OPTICS			  = 7;
	static const int NV_UNDERGROUND_SANDSTORM_OPTICS  = 8;

	// ---- New GLASSES condition variants ----------------------------------------
	static const int NV_DAYTIME_GLASSES			   = 9;
	static const int NV_UNDERGROUND_GLASSES		   = 10;
	static const int NV_SANDSTORM_GLASSES			 = 11;
	static const int NV_UNDERGROUND_SANDSTORM_GLASSES = 12;

	// ---- New PUMPKIN condition variants ----------------------------------------
	static const int NV_DAYTIME_PUMPKIN			   = 13;
	static const int NV_UNDERGROUND_PUMPKIN		   = 14;
	static const int NV_SANDSTORM_PUMPKIN			 = 15;
	static const int NV_UNDERGROUND_SANDSTORM_PUMPKIN = 16;

	// ---- Daytime OPTICS condition variants -------------------------------------
	static const int NV_UNDERGROUND_DAYTIME_OPTICS        = 17;
	static const int NV_UNDERGROUND_SANDSTORM_DAYTIME_OPTICS = 18;
	// ----------------------------------------------------------------------------

	// ---- Exposure safety tuning ------------------------------------------------
	// Hard cap for the additive exposure so no mode can blow out the frame
	// (the "solid green screen" underground-during-daytime-sandstorm bug).
	protected const float NV_EXPOSURE_MAX = 7.0;
	// ----------------------------------------------------------------------------

	protected int   m_CurrentMode;
	protected float m_UGExposureCoef = 1.0;

	// Scales the requested additive exposure by the underground coefficient and clamps
	// the result so it can never saturate the frame, regardless of m_UGExposureCoef.
	protected float ComputeSafeExposure(float baseExposure)
	{
		float exposure = baseExposure * m_UGExposureCoef;
		return Math.Clamp(exposure, 0.0, NV_EXPOSURE_MAX);
	}

	// Shared filmgrain + light setup for the OPTICS family, parameterised by exposure/noise.
	protected void ApplyOpticsProfile(float exposure, float noiseMult)
	{
		SetTargetValueColor(PostProcessEffectType.Glow,PPEGlow.PARAM_COLORIZATIONCOLOR,{0.0,1.0,0.0,0.0},PPEGlow.L_23_NVG,PPOperators.MULTIPLICATIVE);
		SetTargetValueFloat(PPEExceptions.EXPOSURE,PPEExposureNative.PARAM_INTENSITY,false,ComputeSafeExposure(exposure),PPEExposureNative.L_0_NVG_OPTIC,PPOperators.ADD);
		SetTargetValueFloat(PostProcessEffectType.FilmGrain,PPEFilmGrain.PARAM_SHARPNESS,false,9.0,PPEFilmGrain.L_1_NVG,PPOperators.SET);
		SetTargetValueFloat(PostProcessEffectType.FilmGrain,PPEFilmGrain.PARAM_GRAINSIZE,false,1.0,PPEFilmGrain.L_2_NVG,PPOperators.SET);
		SetTargetValueFloat(PPEExceptions.NVLIGHTPARAMS,PPELightIntensityParamsNative.PARAM_LIGHT_MULT,false,3.0,PPELightIntensityParamsNative.L_0_NVG,PPOperators.HIGHEST);
		SetTargetValueFloat(PPEExceptions.NVLIGHTPARAMS,PPELightIntensityParamsNative.PARAM_NOISE_MULT,false,noiseMult,PPELightIntensityParamsNative.L_1_NVG,PPOperators.HIGHEST);
	}

	// Shared filmgrain + light setup for the GLASSES family.
	protected void ApplyGlassesProfile(float exposure, float noiseMult)
	{
		SetTargetValueColor(PostProcessEffectType.Glow,PPEGlow.PARAM_COLORIZATIONCOLOR,{0.0,1.0,0.0,0.0},PPEGlow.L_23_NVG,PPOperators.MULTIPLICATIVE);
		SetTargetValueFloat(PPEExceptions.EXPOSURE,PPEExposureNative.PARAM_INTENSITY,false,ComputeSafeExposure(exposure),PPEExposureNative.L_0_NVG_GOGGLES,PPOperators.ADD);
		SetTargetValueFloat(PostProcessEffectType.FilmGrain,PPEFilmGrain.PARAM_SHARPNESS,false,10.0,PPEFilmGrain.L_1_NVG,PPOperators.SET);
		SetTargetValueFloat(PostProcessEffectType.FilmGrain,PPEFilmGrain.PARAM_GRAINSIZE,false,1.0,PPEFilmGrain.L_2_NVG,PPOperators.SET);
		SetTargetValueFloat(PPEExceptions.NVLIGHTPARAMS,PPELightIntensityParamsNative.PARAM_LIGHT_MULT,false,2.0,PPELightIntensityParamsNative.L_0_NVG,PPOperators.HIGHEST);
		SetTargetValueFloat(PPEExceptions.NVLIGHTPARAMS,PPELightIntensityParamsNative.PARAM_NOISE_MULT,false,noiseMult,PPELightIntensityParamsNative.L_1_NVG,PPOperators.HIGHEST);
	}

	// Shared setup for the PUMPKIN family (orange colorization, no filmgrain).
	protected void ApplyPumpkinProfile(float exposure, float noiseMult)
	{
		SetTargetValueColor(PostProcessEffectType.Glow,PPEGlow.PARAM_COLORIZATIONCOLOR,{1.0,0.2,0.0,0.0},PPEGlow.L_23_NVG,PPOperators.MULTIPLICATIVE);
		SetTargetValueFloat(PPEExceptions.EXPOSURE,PPEExposureNative.PARAM_INTENSITY,false,ComputeSafeExposure(exposure),PPEExposureNative.L_0_NVG_GOGGLES,PPOperators.ADD);
		SetTargetValueFloatDefault(PostProcessEffectType.FilmGrain,PPEFilmGrain.PARAM_SHARPNESS);
		SetTargetValueFloatDefault(PostProcessEffectType.FilmGrain,PPEFilmGrain.PARAM_GRAINSIZE);
		SetTargetValueFloat(PPEExceptions.NVLIGHTPARAMS,PPELightIntensityParamsNative.PARAM_LIGHT_MULT,false,1.0,PPELightIntensityParamsNative.L_0_NVG,PPOperators.HIGHEST);
		SetTargetValueFloat(PPEExceptions.NVLIGHTPARAMS,PPELightIntensityParamsNative.PARAM_NOISE_MULT,false,noiseMult,PPELightIntensityParamsNative.L_1_NVG,PPOperators.HIGHEST);
	}

	float GetUndergroundExposureCoef()
	{
		return m_UGExposureCoef;
	}

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
			
			// ============================ OPTICS ============================
			case NV_DEFAULT_OPTICS:                ApplyOpticsProfile(7.0, 2.0); break; //night, surface
			case NV_DAYTIME_OPTICS:                ApplyOpticsProfile(2.0, 1.0); break; //daytime, surface
			case NV_UNDERGROUND_OPTICS:            ApplyOpticsProfile(7.0, 2.0); break; //enclosed/dark, nighttime
			case NV_UNDERGROUND_DAYTIME_OPTICS:    ApplyOpticsProfile(2.0, 1.0); break; //enclosed/dark, daytime
			case NV_SANDSTORM_OPTICS:              ApplyOpticsProfile(3.5, 2.0); break; //surface, sandstorm + nighttime
			case NV_UNDERGROUND_SANDSTORM_OPTICS:  ApplyOpticsProfile(1.0, 2.0); break; //tunnel + sandstorm, nighttime
			case NV_UNDERGROUND_SANDSTORM_DAYTIME_OPTICS:
				ApplyOpticsProfile(1.0, 1.0); break; //tunnel + sandstorm, daytime

			// ============================ GLASSES ===========================
			case NV_DEFAULT_GLASSES:			   ApplyGlassesProfile(7.0, 1.0); break; //night, surface
			case NV_DAYTIME_GLASSES:			   ApplyGlassesProfile(2.0, 1.0); break; //daytime, surface
			case NV_UNDERGROUND_GLASSES:		   ApplyGlassesProfile(7.0, 1.0); break; //enclosed/dark, daytime/nighttime
			case NV_SANDSTORM_GLASSES:			 ApplyGlassesProfile(3.5, 1.0); break; //surface, sandstorm + nighttime
			case NV_UNDERGROUND_SANDSTORM_GLASSES: ApplyGlassesProfile(7.0, 1.0); break; //tunnel + daytime/nighttime + sandstorm

			// ============================ PUMPKIN ===========================
			case NV_PUMPKIN:					   ApplyPumpkinProfile(5.5, 0.0); break; //night, surface
			case NV_DAYTIME_PUMPKIN:			   ApplyPumpkinProfile(2.0, 0.0); break; //daytime, surface
			case NV_UNDERGROUND_PUMPKIN:		   ApplyPumpkinProfile(5.5, 0.0); break; //enclosed/dark, daytime/nighttime
			case NV_SANDSTORM_PUMPKIN:			 ApplyPumpkinProfile(4.5, 0.0); break; //surface, sandstorm + nighttime
			case NV_UNDERGROUND_SANDSTORM_PUMPKIN: ApplyPumpkinProfile(5.5, 0.0); break; //tunnel + daytime/nighttime + sandstorm

			case NV_TRANSITIVE: //camera transition
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