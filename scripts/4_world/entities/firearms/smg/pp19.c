/**@class	PP19_Base
 * @brief	basic PP19 submachine gun
 **/
class PP19_Base : RifleBoltFree_Base
{
	void PP19_Base()
	{
	}
	
	override RecoilBase SpawnRecoilObject()
	{
		return new PP19Recoil(this);
	}
	
	//some command is different for this weapon
	override int GetWeaponSpecificCommand(int weaponAction ,int subCommand)
	{
		if ( weaponAction == WeaponActions.CHAMBERING)
		{
			switch (subCommand)
			{
				case WeaponActionChamberingTypes.CHAMBERING_ONEBULLET_UNIQUE_CLOSED:
					return WeaponActionChamberingTypes.CHAMBERING_ONEBULLET_OPENED;
				
				default:
					return subCommand;
			}
		
		}
		return subCommand;
	}
	
	override bool CanEnterIronsights()
	{
		ItemOptics optic = GetAttachedOptics();
		if (optic && PSO1Optic.Cast(optic) || PSO11Optic.Cast(optic) || KazuarOptic.Cast(optic))
			return true;
		return super.CanEnterIronsights();
	}
	
	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{
		GameInventory inventory = GetInventory();
		inventory.CreateInInventory( "PistolSuppressor" );
		inventory.CreateInInventory( "PP19_Bttstck" );
		inventory.CreateInInventory( "KobraOptic" );
		inventory.CreateInInventory( "Battery9V" );
		
		SpawnAttachedMagazine("Mag_PP19_64Rnd");
	}
};