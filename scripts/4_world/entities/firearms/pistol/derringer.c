class Derringer_Base : DoubleBarrel_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new DerringerRecoil(this);
	}
	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{
		SpawnAmmo("Ammo_357",SAMF_DEFAULT);
		
		EntityAI entity;
		if ( Class.CastTo(entity, this) )
		{
			entity.SpawnEntityOnGroundPos("Ammo_357", entity.GetPosition());
		}
	}
	
	override int GetAnimationForChamberingWithEjection()
	{
		return WeaponActionChamberingTypes.CHAMBERING_DOUBLE_FIREOUT_EJECT;
	}
};

class Derringer : Derringer_Base {};