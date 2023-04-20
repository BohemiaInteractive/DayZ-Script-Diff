class UniversalTemperatureSourceLambdaBase
{
	void UniversalTemperatureSourceLambdaBase();
	void ~UniversalTemperatureSourceLambdaBase();
	void Execute(UniversalTemperatureSourceSettings pSettings, UniversalTemperatureSourceResult resultValues);
	void DryItemsInVicinity(UniversalTemperatureSourceSettings pSettings);
	
	//! --------------------------------------
	//! DEPRECATED
	const float HEAT_THROUGH_AIR_COEF = 0.003;
}
