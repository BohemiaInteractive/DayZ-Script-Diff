class CZ550_Base : BoltActionRifle_ExternalMagazine_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new CZ550Recoil(this);
	}
			
	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{
		super.OnDebugSpawn();
		
		EntityAI entity;
		if ( Class.CastTo(entity, this) )
		{
			GetInventory().CreateAttachment("HuntingOptic");
		}
	}
};