#ifdef PLATFORM_CONSOLE
class MainMenuWorldData : WorldData
{
	override bool WeatherOnBeforeChange(EWeatherPhenomenon type, float actual, float change, float time)
	{
		return false;
	}
}
#endif
