// PPE when player is in contaminated area trigger
class PPERequester_SpookyAreaTint extends PPERequester_GameplayBase
{
	/*
	protected vector m_StartRGB = vector.Zero;
	protected float m_AccumulatedTime = 0;
	protected bool m_FadeIn = false;
	protected bool m_FadeOut = false;
	
	const float FADE_TIME = 6;
	// the end result is 1 - the value set here
	const float R_TARGET = 0.0;
	const float G_TARGET = 0.3;
	const float B_TARGET = 0.4;
	
	override protected void OnStart( Param par = null )
	{
		super.OnStart( par );
		
		m_AccumulatedTime = 0;
		
		m_FadeIn = true;
		m_FadeOut = false;
		
		//().GetWorld().LoadNewLightingCfg("$mission:lighting_halloween.txt");
	}
	
	override protected void OnUpdate( float delta )
	{
		super.OnUpdate( delta );
		
		if ( m_FadeIn && m_AccumulatedTime <= FADE_TIME )
		{
			m_AccumulatedTime += delta;
		
			m_StartRGB[0] = 1 - FadeColourMult( 0, 1, m_AccumulatedTime / FADE_TIME ) * R_TARGET;
			m_StartRGB[1] = 1 - FadeColourMult( 0, 1, m_AccumulatedTime / FADE_TIME ) * G_TARGET;
			m_StartRGB[2] = 1 - FadeColourMult( 0, 1, m_AccumulatedTime / FADE_TIME ) * B_TARGET;
			
			SetTargetValueColor(PostProcessEffectType.Glow,PPEGlow.PARAM_COLORIZATIONCOLOR,{m_StartRGB[0], m_StartRGB[1], m_StartRGB[2], 0.0},PPEGlow.L_23_TOXIC_TINT,PPOperators.MULTIPLICATIVE);
		}
		
		if ( m_FadeOut && m_AccumulatedTime <= FADE_TIME )
		{
			m_AccumulatedTime += delta;
		
			m_StartRGB[0] = ( 1 - R_TARGET ) + FadeColourMult( 0, R_TARGET, m_AccumulatedTime / FADE_TIME );
			m_StartRGB[1] = ( 1 - G_TARGET ) + FadeColourMult( 0, G_TARGET, m_AccumulatedTime / FADE_TIME );
			m_StartRGB[2] = ( 1 - B_TARGET ) + FadeColourMult( 0, B_TARGET, m_AccumulatedTime / FADE_TIME );
			
			SetTargetValueColor(PostProcessEffectType.Glow,PPEGlow.PARAM_COLORIZATIONCOLOR,{m_StartRGB[0], m_StartRGB[1], m_StartRGB[2], 0.0},PPEGlow.L_23_TOXIC_TINT,PPOperators.MULTIPLICATIVE);
		}
	}
	
	override void OnStop(Param par = null)
	{
		m_FadeIn = false;
		m_FadeOut = false;
		Param1<bool> p;
		
		if (par && Class.CastTo(p,par))
			m_FadeOut = p.param1;
		
		m_AccumulatedTime = 0;
		
		super.OnStop(par);
	}
	
	// Lerped multiplier for RGBA values
	protected float FadeColourMult( float x, float y, float deltaT )
	{
		float output;
		output = Math.Lerp( x, y, deltaT );
		Easing.EaseInOutSine( output );
		return output;
	}
	*/
}