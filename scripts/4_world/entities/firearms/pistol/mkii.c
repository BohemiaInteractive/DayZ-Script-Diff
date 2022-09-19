class MKII_Base : Pistol_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new MkiiRecoil(this);
	}
	
	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{
		super.OnDebugSpawn();
	}
};
