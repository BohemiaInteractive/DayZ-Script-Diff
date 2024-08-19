/**@class		Izh18_Base
 * @brief		base for Izh18Shotgun
 * @NOTE		name copies config base class
 **/
class Izh18Shotgun_Base extends RifleSingleShot_Base
{
	void Izh18Shotgun_Base ()
	{
	}
	
	override RecoilBase SpawnRecoilObject()
	{
		return new Izh18ShotgunRecoil(this);
	}
					
	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{
		super.OnDebugSpawn();

	    SpawnEntityOnGroundPos("Ammo_12gaPellets", GetPosition());
	    SpawnEntityOnGroundPos("Ammo_12gaSlug", GetPosition());
	    SpawnEntityOnGroundPos("Ammo_12gaRubberSlug", GetPosition());
	}
};
