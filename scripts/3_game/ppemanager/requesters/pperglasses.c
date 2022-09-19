//glasses and helmets
//! base, not to be used directly, would lead to layering collisions!
class PPERequester_GenericBlackGlassesBase extends PPERequester_GameplayBase
{
	override protected void OnStart(Param par = null)
	{
		super.OnStart();
		
		SetTargetValueFloat(PostProcessEffectType.GodRays,PPEGodRays.PARAM_INTENSITY,true,0,PPEGodRays.L_0_GLASSES,PPOperators.LOWEST);
		SetTargetValueColor(PostProcessEffectType.Glow,PPEGlow.PARAM_COLORIZATIONCOLOR,{0.4,0.4,0.4,0.0},PPEGlow.L_23_GLASSES,PPOperators.SUBSTRACT);
	}
}

class PPERequester_GlassesSportBlack extends PPERequester_GenericBlackGlassesBase{}
class PPERequester_GlassesDesignerBlack extends PPERequester_GenericBlackGlassesBase{}
class PPERequester_MotoHelmetBlack extends PPERequester_GenericBlackGlassesBase{}

class PPERequester_GlassesSportBlue extends PPERequester_GameplayBase
{
	override protected void OnStart(Param par = null)
	{
		super.OnStart();
		
		SetTargetValueFloat(PostProcessEffectType.GodRays,PPEGodRays.PARAM_INTENSITY,true,0,PPEGodRays.L_0_GLASSES,PPOperators.LOWEST);
		SetTargetValueColor(PostProcessEffectType.Glow,PPEGlow.PARAM_COLORIZATIONCOLOR,{0.3,0.3,0.0,0.0},PPEGlow.L_23_GLASSES,PPOperators.SUBSTRACT);
	}
}

class PPERequester_GlassesSportGreen extends PPERequester_GameplayBase	
{
	override protected void OnStart(Param par = null)
	{
		super.OnStart();
		
		SetTargetValueFloat(PostProcessEffectType.GodRays,PPEGodRays.PARAM_INTENSITY,true,0,PPEGodRays.L_0_GLASSES,PPOperators.LOWEST);
		SetTargetValueColor(PostProcessEffectType.Glow,PPEGlow.PARAM_COLORIZATIONCOLOR,{0.2,0.0,0.2,0.0},PPEGlow.L_23_GLASSES,PPOperators.SUBSTRACT);
	}
}

class PPERequester_GlassesSportOrange extends PPERequester_GameplayBase
{
	override protected void OnStart(Param par = null)
	{
		super.OnStart();
		
		SetTargetValueFloat(PostProcessEffectType.GodRays,PPEGodRays.PARAM_INTENSITY,true,0,PPEGodRays.L_0_GLASSES,PPOperators.LOWEST);
		SetTargetValueColor(PostProcessEffectType.Glow,PPEGlow.PARAM_COLORIZATIONCOLOR,{0.0,0.3,0.5,0.0},PPEGlow.L_23_GLASSES,PPOperators.SUBSTRACT);
	}
}

class PPERequester_GlassesAviator extends PPERequester_GameplayBase
{
	override protected void OnStart(Param par = null)
	{
		super.OnStart();
		
		SetTargetValueFloat(PostProcessEffectType.GodRays,PPEGodRays.PARAM_INTENSITY,true,0,PPEGodRays.L_0_GLASSES,PPOperators.LOWEST);
		SetTargetValueColor(PostProcessEffectType.Glow,PPEGlow.PARAM_COLORIZATIONCOLOR,{0.3,0.3,0.1,0.0},PPEGlow.L_23_GLASSES,PPOperators.SUBSTRACT);
	}
}

class PPERequester_TacticalGoggles extends PPERequester_GameplayBase
{
	override protected void OnStart(Param par = null)
	{
		super.OnStart();
		
		SetTargetValueFloat(PostProcessEffectType.GodRays,PPEGodRays.PARAM_INTENSITY,true,0,PPEGodRays.L_0_GLASSES,PPOperators.LOWEST);
	}
}

class PPERequester_WeldingMask extends PPERequester_GameplayBase
{
	override protected void OnStart(Param par = null)
	{
		super.OnStart();
		
		SetTargetValueFloat(PostProcessEffectType.GodRays,PPEGodRays.PARAM_INTENSITY,true,0,PPEGodRays.L_0_GLASSES,PPOperators.LOWEST);
		SetTargetValueColor(PostProcessEffectType.Glow,PPEGlow.PARAM_COLORIZATIONCOLOR,{0.7,0.7,0.7,0.0},PPEGlow.L_23_GLASSES,PPOperators.SUBSTRACT);
	}
}