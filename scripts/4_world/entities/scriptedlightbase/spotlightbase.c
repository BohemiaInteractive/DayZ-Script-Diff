class SpotLightBase extends ScriptedLightBase
{
	void SpotLightBase()
	{
		SetLightType(LightSourceType.SpotLight); // This function must be called in constructor of the light!
	}
}