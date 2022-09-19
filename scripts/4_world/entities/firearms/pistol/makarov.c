class MakarovIJ70_Base : Pistol_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new MakarovRecoil(this);
	}
	
	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{
		GameInventory inventory = GetInventory();
		inventory.CreateInInventory( "PistolSuppressor" );
		
		SpawnAttachedMagazine("Mag_IJ70_8Rnd");
	}
};