//-----------------------------------------------------------------------------------------------
// DayZInfectedTypeCreate - main dayz type config
//-----------------------------------------------------------------------------------------------

void DayZInfectedTypeCreate(DayZInfectedType pType)
{
	//! register attacks
	pType.RegisterAttacks();

	//! melee hit components (cached in type)
	pType.RegisterHitComponentsForAI();
}
