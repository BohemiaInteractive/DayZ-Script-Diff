class PPERequester_IntroChromAbb extends PPERequester_MenuBase
{
	override protected void OnStart(Param par = null)
	{
		super.OnStart();
		
		SetTargetValueFloat(PostProcessEffectType.ChromAber,PPEChromAber.PARAM_POWERX,false,0.002,PPEChromAber.L_0_INTRO,PPOperators.SET);
		SetTargetValueFloat(PostProcessEffectType.ChromAber,PPEChromAber.PARAM_POWERY,false,0.002,PPEChromAber.L_1_INTRO,PPOperators.SET);
	}
}