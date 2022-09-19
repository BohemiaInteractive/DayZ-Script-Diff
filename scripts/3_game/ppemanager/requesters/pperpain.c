class PPERequester_PainBlur extends PPERequester_GameplayBase
{
	void SetRadialBlur(float power_x, float power_y, float offset_x, float offset_y, float pixel_scale = 0.5 )
	{
		SetTargetValueFloat(PostProcessEffectType.RadialBlur,PPERadialBlur.PARAM_POWERX,false,power_x, PPERadialBlur.L_0_PAIN_BLUR ,PPOperators.SET);
		SetTargetValueFloat(PostProcessEffectType.RadialBlur,PPERadialBlur.PARAM_POWERY,false,power_y, PPERadialBlur.L_0_PAIN_BLUR,PPOperators.SET);
		SetTargetValueFloat(PostProcessEffectType.RadialBlur,PPERadialBlur.PARAM_OFFSETX,false,offset_x, PPERadialBlur.L_0_PAIN_BLUR,PPOperators.SET);
		SetTargetValueFloat(PostProcessEffectType.RadialBlur,PPERadialBlur.PARAM_OFFSETY,false,offset_y, PPERadialBlur.L_0_PAIN_BLUR,PPOperators.SET);
		SetTargetValueFloat(PostProcessEffectType.RadialBlur,PPERadialBlur.PARAM_PIXELSCALE,false,pixel_scale,PPERadialBlur.L_0_PAIN_BLUR ,PPOperators.SET);
	}
}
