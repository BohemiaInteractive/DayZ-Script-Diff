class FlareLight extends PointLightBase
{
	static float 	m_FlareRadius = 100;
	static float 	m_FlareBrightness = 10.0;
	static string 	m_MemoryPoint = "light";
	
	void FlareLight()
	{
		SetVisibleDuringDaylight( true );
		SetRadiusTo( m_FlareRadius );
		SetBrightnessTo( m_FlareBrightness );
		FadeIn( 1 );
		SetFadeOutTime( 0.2 );
		SetDiffuseColor( 0.7, 0.7, 0.3 );
		SetAmbientColor( 0.7, 0.7, 0.3 );
		SetFlareVisible( false );
		SetFlickerAmplitude( 0.9 );
		SetFlickerSpeed( 0.3 );
		SetDancingShadowsMovementSpeed( 0.5 );
		SetDancingShadowsAmplitude( 0.15 );
		//SetLifetime( 50 );
		EnableLinear( true );
		EnableHeatHaze( true );
		SetHeatHazeRadius( 0.1 );
		SetHeatHazePower( 0.02 );
		#ifdef PLATFORM_WINDOWS
			SetCastShadow( true );
		#else
			SetCastShadow( false );
		#endif
		
	}
}

class FlareLightRed extends FlareLight
{
	
	void FlareLightRed()
	{
		SetDiffuseColor( 1.0, 0.3, 0.3 );
		SetAmbientColor( 1.0, 0.3, 0.3 );
	}
}

class FlareLightGreen extends FlareLight
{	
	void FlareLightGreen()
	{
		SetDiffuseColor( 0.3, 1.0, 0.3 );
		SetAmbientColor( 0.3, 1.0, 0.3 );
	}
}

class FlareLightBlue extends FlareLight
{
	void FlareLightBlue()
	{
		SetDiffuseColor( 0.3, 0.3, 1.0 );
		SetAmbientColor( 0.3, 0.3, 1.0 );
	}
}

class FlareLightContamination extends FlareLightRed
{
	void FlareLightContamination()
	{
		SetLifetime( 20 );
	}
}