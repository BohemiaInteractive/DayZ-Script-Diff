class PPERequester_BurlapSackEffects extends PPERequester_GameplayBase
{
	override protected void OnStart(Param par = null)
	{
		super.OnStart();
		
		SetTargetValueFloat(PPEExceptions.EXPOSURE,PPEExposureNative.PARAM_INTENSITY,false,-100.0,PPEExposureNative.L_0_BURLAP,PPOperators.SET);
		SetTargetValueFloat(PPEExceptions.EYEACCOM,PPEEyeAccomodationNative.PARAM_INTENSITY,false,0.0,PPEEyeAccomodationNative.L_0_BURLAP,PPOperators.SET);
		SetTargetValueFloat(PostProcessEffectType.Glow,PPEGlow.PARAM_VIGNETTE,false,2.0,PPEGlow.L_25_BURLAP,PPOperators.SET);
		SetTargetValueColor(PostProcessEffectType.Glow,PPEGlow.PARAM_VIGNETTECOLOR,{0.0,0.0,0.0,0.0},PPEGlow.L_26_BURLAP,PPOperators.LOWEST);
		
		GetGame().GetMission().GetEffectWidgets().AddSuspendRequest(EffectWidgetSuspends.BURLAPSACK);
	}
	
	override protected void OnStop(Param par = null)
	{
		super.OnStop();
		
		GetGame().GetMission().GetEffectWidgets().RemoveSuspendRequest(EffectWidgetSuspends.BURLAPSACK);
	}
}