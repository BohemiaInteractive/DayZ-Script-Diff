class PointLightBase extends ScriptedLightBase
{
	void PointLightBase()
	{
		SetLightType(LightSourceType.PointLight); // This function must be called in constructor of the light!
	}
}