class M79_Base extends RifleSingleShotManual_Base
{
	void M79_Base ()
	{
	}
	
	override bool ShootsExplosiveAmmo()
	{
		return true;
	}
	
	override RecoilBase SpawnRecoilObject()
	{
		return new M79Recoil(this);
	}
	
	override void OnDebugSpawn()
	{
		SpawnAmmo("Ammo_40mm_Explosive",SAMF_DEFAULT);
		
		EntityAI entity;
		if ( Class.CastTo(entity, this) )
		{
			entity.SpawnEntityOnGroundPos("Ammo_40mm_Explosive", entity.GetPosition());
			entity.SpawnEntityOnGroundPos("Ammo_40mm_Explosive", entity.GetPosition());
			entity.SpawnEntityOnGroundPos("Ammo_40mm_Explosive", entity.GetPosition());
		}
	}

};

class M79 extends M79_Base
{
	
	override void AssembleGun()
	{
		super.AssembleGun();
		
		if ( !FindAttachmentBySlotName("weaponOpticsAug") )
		{
			GetInventory().CreateAttachment("M79DummyOptics");
		}
	}
		
};

class M79DummyOptics extends ItemOptics
{
	override bool CanPutAsAttachment( EntityAI parent )
	{
		return true;
	}
	
	override bool IsSightOnly()
	{
		return true;
	}
};

