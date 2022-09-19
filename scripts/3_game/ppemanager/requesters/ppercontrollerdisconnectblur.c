class PPERequester_ControllerDisconnectBlur extends PPERequester_MenuBase
{
	override protected void OnStart(Param par = null)
	{
		super.OnStart();
		
		SetTargetValueFloat(PostProcessEffectType.GaussFilter,PPEGaussFilter.PARAM_INTENSITY,false,1.0,PPEGaussFilter.L_0_DISCONNECT,PPOperators.SET); //PPOperators.OVERRIDE?
	}
}