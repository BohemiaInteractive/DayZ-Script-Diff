class Izh43Shotgun_Base : DoubleBarrel_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new Izh43Recoil(this);
	}
				
	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{
		super.OnDebugSpawn();
		
		EntityAI entity;
		if ( Class.CastTo(entity, this) )
		{
			entity.SpawnEntityOnGroundPos("Ammo_12gaPellets", entity.GetPosition());
			entity.SpawnEntityOnGroundPos("Ammo_12gaSlug", entity.GetPosition());
			entity.SpawnEntityOnGroundPos("Ammo_12gaRubberSlug", entity.GetPosition());
		}
	}
};

class Izh43Shotgun: Izh43Shotgun_Base {};
class SawedoffIzh43Shotgun: Izh43Shotgun_Base {};

