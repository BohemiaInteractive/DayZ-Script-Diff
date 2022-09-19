class FlashlightLight extends SpotLightBase
{
	private static float m_DefaultBrightness = 4;
	private static float m_DefaultRadius = 25;
	
	void FlashlightLight()
	{
		SetVisibleDuringDaylight( true );
		SetRadiusTo( m_DefaultRadius );
		SetSpotLightAngle( 110 );
		SetCastShadow( true );
		EnableSpecular( true );
		SetBrightnessTo( m_DefaultBrightness );
		SetFadeOutTime( 0.15 );
		SetAmbientColor( 0.9, 0.85, 0.75 );
		SetDiffuseColor( 0.9, 0.85, 0.75 );
	}
	
	void SetIntensity( float coef, float time )
	{
		FadeBrightnessTo(m_DefaultBrightness * coef, time);
	}
}