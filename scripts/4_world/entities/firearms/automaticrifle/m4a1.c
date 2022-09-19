class M4A1_Base : RifleBoltLock_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new M4a1Recoil(this);
	}		
		
	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{
		super.OnDebugSpawn();
		
		GameInventory inventory = GetInventory();
		inventory.CreateInInventory( "M9A1_Bayonet" );
		inventory.CreateInInventory( "M4_OEBttstck" );	
		inventory.CreateInInventory( "M4_RISHndgrd" );
		inventory.CreateInInventory( "UniversalLight" );		
		inventory.CreateInInventory( "M4_T3NRDSOptic" );	
		inventory.CreateInInventory( "Battery9V" );
		inventory.CreateInInventory( "Battery9V" );	
	}
};