// PPE when player is in contaminated area trigger
class PPERequester_ContaminatedAreaTint extends PPERequester_GameplayBase
{
	protected vector m_StartRGB = vector.Zero;
	protected float m_AccumulatedTime = 0;
	protected bool m_FadeIn = false;
	protected bool m_FadeOut = false;
	
	const float FADE_TIME = 3;
	// the end result is 1 - the value set here
	const float R_TARGET = 0.142; // end value 0.858
	const float G_TARGET = 0.15; // end value 0.850
	const float B_TARGET = 0.44; // end value 0.560
	
	override protected void OnStart( Param par = null )
	{
		super.OnStart( par );
		
		m_AccumulatedTime = 0;
		
		m_FadeIn = true;
		m_FadeOut = false;
		
		SetTargetValueFloat(PostProcessEffectType.FilmGrain,PPEFilmGrain.PARAM_SHARPNESS,false,10.0,PPEFilmGrain.L_1_TOXIC_TINT,PPOperators.HIGHEST);
		SetTargetValueFloat(PostProcessEffectType.FilmGrain,PPEFilmGrain.PARAM_GRAINSIZE,false,1.0,PPEFilmGrain.L_2_TOXIC_TINT,PPOperators.LOWEST);
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
		
		if ( m_FadeOut )
		{
			if (m_AccumulatedTime <= FADE_TIME)
			{
				m_AccumulatedTime += delta;
			
				m_StartRGB[0] = ( 1 - R_TARGET ) + FadeColourMult( 0, R_TARGET, m_AccumulatedTime / FADE_TIME );
				m_StartRGB[1] = ( 1 - G_TARGET ) + FadeColourMult( 0, G_TARGET, m_AccumulatedTime / FADE_TIME );
				m_StartRGB[2] = ( 1 - B_TARGET ) + FadeColourMult( 0, B_TARGET, m_AccumulatedTime / FADE_TIME );
				
				SetTargetValueColor(PostProcessEffectType.Glow,PPEGlow.PARAM_COLORIZATIONCOLOR,{m_StartRGB[0], m_StartRGB[1], m_StartRGB[2], 0.0},PPEGlow.L_23_TOXIC_TINT,PPOperators.MULTIPLICATIVE);
			}
			else
			{
				Stop(); //proper termination after a fadeout
			}
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
}