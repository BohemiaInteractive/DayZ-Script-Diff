class R12_Base : ChamberFirst_InnerMagazine_SemiAutomatic_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new R12Recoil(this);
	}
		
	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{
		super.OnDebugSpawn();
		GameInventory inventory = GetInventory();
		inventory.CreateInInventory( "UniversalLight" );
		inventory.CreateInInventory( "ReflexOptic" );
		inventory.CreateInInventory( "Battery9V" );
	}	
}
class R12 : R12_Base
{
};