/**@class	CZ61_Base
 * @brief	basic CZ61 Skorpion submachine gun
 **/
class CZ61_Base : RifleBoltLock_Base
{
	void CZ61()
	{
	}
	
	
	override RecoilBase SpawnRecoilObject()
	{
		return new Cz61Recoil(this);
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
	
	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{
		GameInventory inventory = GetInventory();

		inventory.CreateInInventory( "PistolSuppressor" );
		
		SpawnAttachedMagazine("Mag_CZ61_20Rnd");
	}
};