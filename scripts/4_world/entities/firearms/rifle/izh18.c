/**@class		Izh18_Base
 * @brief		base for Izh18
 * @NOTE		name copies config base class
 **/
class Izh18_Base extends RifleSingleShot_Base
{
	void Izh18_Base ()
	{
	}
	
	override RecoilBase SpawnRecoilObject()
	{
		return new Izh18Recoil(this);
	}
					
	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{
		super.OnDebugSpawn();
		
		EntityAI entity;
		if ( Class.CastTo(entity, this) )
		{
			entity.SpawnEntityOnGroundPos("Ammo_762x39", entity.GetPosition());
		}
	}
};
