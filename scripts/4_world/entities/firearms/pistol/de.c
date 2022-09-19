class Deagle_Base : Pistol_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new DEagleRecoil(this);
	}
	
	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{
		GameInventory inventory = GetInventory();
		inventory.CreateInInventory( "PistolSuppressor" );
		inventory.CreateInInventory( "PistolOptic" );
		
		SpawnAttachedMagazine("Mag_Deagle_9rnd");
	}
	
};

class Deagle : Deagle_Base {};
class Deagle_Gold : Deagle_Base {};

