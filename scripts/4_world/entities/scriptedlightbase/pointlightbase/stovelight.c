class StoveLight extends PointLightBase
{
	void StoveLight()
	{
		SetVisibleDuringDaylight( false );
		SetRadiusTo( 1 );
		SetBrightnessTo( 3.0 );
		SetCastShadow( true );
		SetFadeOutTime( 0.2 );
		SetDiffuseColor( 0.2, 0.45, 0.7 );
		SetAmbientColor( 0.2, 0.45, 0.7 );
		SetFlareVisible( false );
		SetFlickerAmplitude( 0.35 );
		SetFlickerSpeed( 0.55 );
		SetDancingShadowsMovementSpeed( 0.05 );
		SetDancingShadowsAmplitude( 0.03 );
		EnableHeatHaze( true);
		SetHeatHazeRadius( 0.08 );
		SetHeatHazePower( 0.015 );
	}
}