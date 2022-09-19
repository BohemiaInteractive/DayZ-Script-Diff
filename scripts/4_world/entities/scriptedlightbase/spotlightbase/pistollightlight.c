class PistollightLight extends SpotLightBase
{
	private static float m_DefaultBrightness = 5.25;
	private static float m_DefaultRadius = 18;
	private static float m_DefaultAngle = 100;
	
	
	void PistollightLight()
	{
		SetVisibleDuringDaylight( true );
		SetRadiusTo( m_DefaultRadius );
		SetSpotLightAngle( m_DefaultAngle );
		SetCastShadow( true );
		SetBrightnessTo( m_DefaultBrightness );
		SetFadeOutTime( 0.05 );
		SetAmbientColor( 0.92, 0.88, 0.82 );
		SetDiffuseColor( 0.92, 0.88, 0.82 );
		SetDisableShadowsWithinRadius( 1.2 );
	}
	
	void SetIntensity( float coef, float time )
	{
		FadeBrightnessTo( m_DefaultBrightness * coef, time );
		FadeRadiusTo( m_DefaultRadius * coef, time );
	}
	
	override void UpdateLightMode( string slotName )
	{
		switch ( slotName )
		{
			case "weaponFlashlight":
				SetSpotLightAngle( m_DefaultAngle/2 );
				SetRadiusTo( m_DefaultRadius * 1.8 );
			break;
			
			default:
				SetSpotLightAngle( m_DefaultAngle );
				SetRadiusTo( m_DefaultRadius );
			break;
		}
	}
}