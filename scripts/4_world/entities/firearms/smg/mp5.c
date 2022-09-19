
/**@class	MP5K_Base
 * @brief	basic mp5 submachine gun
 **/
class MP5K_Base : RifleBoltFree_Base
{
	void MP5K_Base ()
	{
	}
	
	override RecoilBase SpawnRecoilObject()
	{
		return new Mp5kRecoil(this);
	}
	
	
	override int GetWeaponSpecificCommand(int weaponAction ,int subCommand)
	{
		if ( weaponAction == WeaponActions.RELOAD)
		{
			switch (subCommand)
			{
				case WeaponActionReloadTypes.RELOADRIFLE_MAGAZINE_BULLET:
					return WeaponActionReloadTypes.RELOADSRIFLE_MAGAZINE_BULLET;
				
				case WeaponActionReloadTypes.RELOADRIFLE_NOMAGAZINE_BULLET:
					return WeaponActionReloadTypes.RELOADSRIFLE_NOMAGAZINE_BULLET;
				
				case WeaponActionReloadTypes.RELOADRIFLE_MAGAZINE_NOBULLET:
					return WeaponActionReloadTypes.RELOADSRIFLE_MAGAZINE_NOBULLET;
				
				case WeaponActionReloadTypes.RELOADRIFLE_NOMAGAZINE_NOBULLET:
					return WeaponActionReloadTypes.RELOADSRIFLE_NOMAGAZINE_NOBULLET;
				
				default:
					return subCommand;
			}
		
		}
		return subCommand;
	}
	
	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{
		GameInventory inventory = GetInventory();
		inventory.CreateInInventory( "MP5k_StockBttstck" );
		inventory.CreateInInventory( "MP5_RailHndgrd" );
		inventory.CreateInInventory( "MP5_Compensator" );
		inventory.CreateInInventory( "ReflexOptic" );
		inventory.CreateInInventory( "UniversalLight" );
		inventory.CreateInInventory( "Battery9V" );
		inventory.CreateInInventory( "Battery9V" );
		
		SpawnAttachedMagazine("Mag_MP5_30Rnd");
	}
};


