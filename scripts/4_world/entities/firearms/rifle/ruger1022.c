class Ruger1022_Base : RifleBoltFree_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new Ruger1022Recoil(this); //TODO
	}
	
	/*override int GetWeaponSpecificCommand(int weaponAction ,int subCommand)
	{
		if ( weaponAction == WeaponActions.RELOAD)
		{
			switch (subCommand)
			{
				case WeaponActionReloadTypes.RELOADSRIFLE_MAGAZINE_BULLET:
					return WeaponActionReloadTypes.RELOADRIFLE_MAGAZINE_BULLET;
				
				case WeaponActionReloadTypes.RELOADSRIFLE_NOMAGAZINE_BULLET:
					return WeaponActionReloadTypes.RELOADRIFLE_NOMAGAZINE_BULLET;
				
				case WeaponActionReloadTypes.RELOADSRIFLE_MAGAZINE_NOBULLET:
					return WeaponActionReloadTypes.RELOADRIFLE_MAGAZINE_NOBULLET;
				
				case WeaponActionReloadTypes.RELOADSRIFLE_NOMAGAZINE_NOBULLET:
					return WeaponActionReloadTypes.RELOADRIFLE_NOMAGAZINE_NOBULLET;
				
				default:
					return subCommand;
			}
		
		}
		return subCommand;
	}*/
	
	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{		
		GameInventory inventory = GetInventory();

		inventory.CreateInInventory( "HuntingOptic" );
		
		SpawnAttachedMagazine("Mag_Ruger1022_30Rnd");
	}	
};