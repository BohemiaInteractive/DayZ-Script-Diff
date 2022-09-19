class Winchester70_Base : BoltActionRifle_InnerMagazine_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new Winchester70Recoil(this);
	}
	
			
	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{
		super.OnDebugSpawn();
		EntityAI entity;
		if ( Class.CastTo(entity, this) )
		{
			entity.GetInventory().CreateInInventory( "HuntingOptic" );	
			entity.SpawnEntityOnGroundPos("Ammo_308Win", entity.GetPosition());
		}
	}
};