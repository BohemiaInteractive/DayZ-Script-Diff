class LongHorn_Base : SingleShotPistol_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new LongHornRecoil(this);
	}
	
	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{
		super.OnDebugSpawn();
		GetInventory().CreateInInventory( "PistolOptic" );
	}	
};

class LongHorn : LongHorn_Base {};