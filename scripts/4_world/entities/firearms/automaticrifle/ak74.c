class AK74_Base : AKM_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new Ak74Recoil(this);
	}
	
		
	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{		
		GameInventory inventory = GetInventory();

		inventory.CreateInInventory( "AK74_WoodBttstck" );
		inventory.CreateInInventory( "AK74_Hndgrd" );
		inventory.CreateInInventory( "PSO1Optic" );
		inventory.CreateInInventory( "AK_Bayonet" );
		inventory.CreateInInventory( "Battery9V" );	
			
		SpawnAttachedMagazine("Mag_AK74_30Rnd");
	}
};

class AK74 : AK74_Base
{
	override bool CanEnterIronsights()
	{
		ItemOptics optic = GetAttachedOptics();
		if (optic && PSO1Optic.Cast(optic) || PSO11Optic.Cast(optic) || KazuarOptic.Cast(optic))
			return true;
		return super.CanEnterIronsights();
	}
};