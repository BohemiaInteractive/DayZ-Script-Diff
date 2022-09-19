class AK101_Base : AKM_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new Ak101Recoil(this);
	}
			
	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{
		GameInventory inventory = GetInventory();

		inventory.CreateInInventory( "AK_Bayonet" );
		inventory.CreateInInventory( "AK_PlasticBttstck" );	
		inventory.CreateInInventory( "AK_RailHndgrd" );
		inventory.CreateInInventory( "PSO1Optic" );		
		inventory.CreateInInventory( "UniversalLight" );	
		inventory.CreateInInventory( "Battery9V" );
		inventory.CreateInInventory( "Battery9V" );	
		
		SpawnAttachedMagazine("Mag_AK101_30Rnd");
	}
};

class AK101 : AK101_Base
{
	override bool CanEnterIronsights()
	{
		ItemOptics optic = GetAttachedOptics();
		if (optic && PSO1Optic.Cast(optic) || PSO11Optic.Cast(optic) || KazuarOptic.Cast(optic))
			return true;
		return super.CanEnterIronsights();
	}
};