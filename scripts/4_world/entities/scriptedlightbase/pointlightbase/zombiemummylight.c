class ZombieMummyLight extends PointLightBase
{
	void ZombieMummyLight()
	{
		SetVisibleDuringDaylight(false);
		SetRadiusTo(2.1);
		SetBrightnessTo(3.1);
		SetCastShadow(false);
		SetFadeOutTime(0.2);
		SetDiffuseColor(0.6, 0.5, 0.1);
		SetAmbientColor(0.6, 0.5, 0.1);
		SetFlareVisible(false);
		SetFlickerAmplitude(0.35);
		SetFlickerSpeed(0.25);
		SetDancingShadowsMovementSpeed(0.05);
		SetDancingShadowsAmplitude(0.03);
	}
};