class Saiga_Base : RifleBoltFree_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new SiagaRecoil(this);
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
	
	override bool CanEnterIronsights()
	{
		ItemOptics optic = GetAttachedOptics();
		if ( optic && PSO1Optic.Cast(optic) || PSO11Optic.Cast(optic) || KashtanOptic.Cast(optic) || KazuarOptic.Cast(optic) )
			return true;
		return super.CanEnterIronsights();
	}
		
	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{
		super.OnDebugSpawn();
		GameInventory inventory = GetInventory();
		inventory.CreateInInventory( "Saiga_Bttstck" );
		inventory.CreateInInventory( "KobraOptic" );
		inventory.CreateInInventory( "Battery9V" );
	}	
}
class Saiga : Saiga_Base
{

};