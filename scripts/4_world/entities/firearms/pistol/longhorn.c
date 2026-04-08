class LongHorn_Base : SingleShotPistol_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new LongHornRecoil(this);
	}
	
	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{
		SpawnAmmo("Ammo_308Win",SAMF_DEFAULT);
		
		GetInventory().CreateInInventory( "PistolOptic" );
		
		EntityAI entity;
		if ( Class.CastTo(entity, this) )
		{
			entity.SpawnEntityOnGroundPos("Ammo_308Win", entity.GetPosition());
		}
	}	
};

class LongHorn : LongHorn_Base {};