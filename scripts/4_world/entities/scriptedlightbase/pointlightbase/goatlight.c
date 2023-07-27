class GoatLight extends PointLightBase
{
	void GoatLight()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(1.6);
		SetBrightnessTo(3.1);
		SetCastShadow(false);
		SetFadeOutTime(15);
		SetDiffuseColor(0.85, 0.1, 0.1);
		SetAmbientColor(0.85, 0.1, 0.1);
		SetFlareVisible(false);
		SetFlickerAmplitude(0.35);
		SetFlickerSpeed(0.25);
		SetDancingShadowsMovementSpeed(0.05);
		SetDancingShadowsAmplitude(0.03);
	}
};