// Test case for light sources not working during daytime as expected

class BrightnessTestLight extends PointLightBase
{
	float m_Timer = 0;
	
	void BrightnessTestLight()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo( 30 );
		SetCastShadow(true);
		SetDiffuseColor(1, 1, 1);
		SetAmbientColor(1, 1, 1);

		SetBrightnessTo(10);
	}
	
	override void OnFrameLightSource(IEntity other, float timeSlice)
	{
		m_Timer += timeSlice;
		
		float pulse = Math.AbsFloat( Math.Cos( m_Timer) );
		
		if (pulse > 0)
		{
			Print(pulse);
			SetBrightnessTo(pulse);
		}
	}
}