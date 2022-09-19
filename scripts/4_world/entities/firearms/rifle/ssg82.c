class SSG82_Base : BoltActionRifle_ExternalMagazine_Base
{
	override bool CanEnterIronsights()
	{
		return false;
	}
	
	override void AssembleGun()
	{
		super.AssembleGun();
		
		if ( !FindAttachmentBySlotName("weaponOpticsAug") )
		{
			GetInventory().CreateAttachment("SSG82Optic");
		}
	}
	
	override RecoilBase SpawnRecoilObject()
	{
		return new SSG82Recoil(this);
	}
			
	//Debug menu Spawn Ground Special
	/*override void OnDebugSpawn()
	{
		super.OnDebugSpawn();
	}*/
};


class SSG82Optic: ItemOptics
{
	override bool CanPutAsAttachment( EntityAI parent )
	{
		return true;
	}
};