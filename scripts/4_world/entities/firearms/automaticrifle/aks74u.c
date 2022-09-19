class AKS74U : AK74_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new Aks74uRecoil(this);
	}
	
	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{
		EntityAI entity;
		if ( Class.CastTo(entity, this) )
		{
			entity.GetInventory().CreateInInventory( "AKS74U_Bttstck" );
			entity.GetInventory().CreateInInventory( "AK_Suppressor" );		
			entity.SpawnEntityOnGroundPos("Mag_AK74_30Rnd", entity.GetPosition());
		}
	}
};