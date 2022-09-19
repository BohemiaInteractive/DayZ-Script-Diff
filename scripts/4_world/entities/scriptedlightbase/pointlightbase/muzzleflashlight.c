class MuzzleFlashLight extends PointLightBase
{
	static float m_Lifetime = 0.04;
	
	void MuzzleFlashLight()
	{
		SetLifetime(m_Lifetime);
		FadeIn(m_Lifetime * 0.5);
		SetFadeOutTime(m_Lifetime * 0.5);
		SetVisibleDuringDaylight(true);
		SetRadiusTo( 15 );
		SetBrightnessTo(1);
		SetCastShadow(false);
		SetAmbientColor(0.76, 0.68, 0.31);
		SetDiffuseColor(0.76, 0.68, 0.31);
		SetFlareVisible(false);
	}
}

class MuzzleFlashLight_1 extends PointLightBase
{
	static float m_Lifetime = 0.08;
	
	void MuzzleFlashLight_1()
	{
		SetLifetime( m_Lifetime );
		FadeIn( m_Lifetime * 0.2 );
		SetFadeOutTime( m_Lifetime * 0.2 );
		SetVisibleDuringDaylight( true );
		SetRadiusTo( 17 );
		SetBrightnessTo( 0.5 );
		SetCastShadow( false );
		SetAmbientColor( 0.56, 0.42, 0.3 );
		SetDiffuseColor( 0.56, 0.42, 0.3 );
		SetFlareVisible( false );
	}
}

class MuzzleFlashLight_2 extends PointLightBase
{
	static float m_Lifetime = 0.01;
	
	void MuzzleFlashLight_2()
	{
		SetLifetime( m_Lifetime );
		FadeIn( m_Lifetime * 0.1 );
		SetFadeOutTime( m_Lifetime * 0.1 );
		SetVisibleDuringDaylight( true );
		SetRadiusTo( 12 );
		SetBrightnessTo( 0.2 );
		SetCastShadow( false );
		SetAmbientColor( 0.36, 0.32, 0.13 );
		SetDiffuseColor( 0.36, 0.32, 0.13 );
		SetFlareVisible( false );
	}
}