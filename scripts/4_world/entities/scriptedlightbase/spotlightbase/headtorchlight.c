class HeadtorchLight extends SpotLightBase
{
	private static float m_DefaultBrightness = 3;
	private static float m_DefaultRadius = 20;
	
	void HeadtorchLight()
	{
		SetVisibleDuringDaylight( true );
		SetRadiusTo( m_DefaultRadius );
		SetSpotLightAngle( 95 );
		SetCastShadow( true );
		FadeIn( 0.06 );
		SetBrightnessTo( m_DefaultBrightness );
		SetAmbientColor( 0.92, 0.85, 0.58 );
		SetDiffuseColor( 0.92, 0.85, 0.58 );
		SetFadeOutTime( 0.1 );
		//SetDisableShadowsWithinRadius(0.25); // Idea for optimization: Uncomment this to disable shadows from Headtorch while it's on player's head during 1P view.
	}
	
	void SetColorToWhite()
	{
		SetAmbientColor( 0.92, 0.85, 0.86 );
		SetDiffuseColor( 0.92, 0.85, 0.86 );
	}
	
	void SetColorToRed()
	{
		SetAmbientColor( 1.0, 0.2, 0.2 );
		SetDiffuseColor( 1.0, 0.2, 0.2 );
	}
	
	void SetIntensity( float coef, float time )
	{
		FadeBrightnessTo( m_DefaultBrightness * coef, time );
		FadeRadiusTo( m_DefaultRadius * coef, time );
	}
}