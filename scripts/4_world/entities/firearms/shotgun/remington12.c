class R12_Base : ChamberFirst_InnerMagazine_SemiAutomatic_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new R12Recoil(this);
	}
		
	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{
		//super.OnDebugSpawn();
		SpawnAmmo("Ammo_12gaPellets",SAMF_DEFAULT);
		GameInventory inventory = GetInventory();
		inventory.CreateInInventory( "UniversalLight" );
		inventory.CreateInInventory( "Battery9V" );
		inventory.CreateInInventory( "ReflexOptic" );
		inventory.CreateInInventory( "Battery9V" );
		
		EntityAI entity;
		if ( Class.CastTo(entity, this) )
		{
			entity.SpawnEntityOnGroundPos("Ammo_12gaPellets", entity.GetPosition());
		}
	}	
}
class R12 : R12_Base
{
};