class Aug_Base : RifleBoltFree_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new AUGRecoil(this);
	}
	
	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{
		EntityAI entity;
		if ( Class.CastTo(entity, this) )
		{
			entity.SpawnEntityOnGroundPos("Mag_Aug_30Rnd", entity.GetPosition());
		}
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
};

class Aug extends Aug_Base
{
	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{
		GameInventory inventory = GetInventory();

		inventory.CreateInInventory( "UniversalLight" );
		inventory.CreateInInventory( "Battery9V" );	
		inventory.CreateInInventory( "ACOGOptic" );
		inventory.CreateInInventory( "M4_Suppressor" );
		
		SpawnAttachedMagazine("Mag_Aug_30Rnd");
	}	
}

class AugShort extends Aug_Base
{
	override bool CanEnterIronsights()
	{
		return true;
	}
	
	override void AssembleGun()
	{
		super.AssembleGun();
		
		if ( !FindAttachmentBySlotName("weaponOpticsAug") )
		{
			GetInventory().CreateAttachment("AugOptic");
		}
	}
	
	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{
		SpawnAttachedMagazine("Mag_Aug_30Rnd");
	}	
}


class AugOptic: ItemOptics
{
	override bool CanPutAsAttachment( EntityAI parent )
	{
		return true;
	}
};