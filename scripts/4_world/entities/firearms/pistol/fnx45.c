class FNX45_Base : Pistol_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new Fnx45Recoil(this);
	}
		
	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{
		GameInventory inventory = GetInventory();
		inventory.CreateInInventory( "PistolSuppressor" );
		inventory.CreateInInventory( "FNP45_MRDSOptic" );
		inventory.CreateInInventory( "TLRLight" );
		inventory.CreateInInventory( "Battery9V" );
		inventory.CreateInInventory( "Battery9V" );
		
		SpawnAttachedMagazine("Mag_FNX45_15Rnd");
	}
	
};