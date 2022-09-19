class Colt1911_Base : Pistol_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new Colt1911Recoil(this);
	}
	
	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{
		GameInventory inventory = GetInventory();
		inventory.CreateInInventory( "PistolSuppressor" );
		inventory.CreateInInventory( "TLRLight" );
		inventory.CreateInInventory( "Battery9V" );
		
		SpawnAttachedMagazine("Mag_1911_7Rnd");
	}
};

class Colt1911 : Colt1911_Base {};
class Engraved1911 : Colt1911_Base {};