class Luger_Base : Pistol_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new LugerRecoil(this);
	}
	
	override int GetChargedPistolAnimState()
	{
		return PistolAnimState.DEFAULT;
	}

	override int GetOpenPistolAnimState()
	{
		return PistolAnimState.OPENED_DISCHARGED;
	}
	
	override int GetJammedPistolAnimState()
	{
		return PistolAnimState.CLOSED_CHARGED;
	}
	
};

class Luger : Luger_Base {};