class B95_base : DoubleBarrel_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new B95Recoil(this);
	}
	
		
	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{
		super.OnDebugSpawn();
		GameInventory inventory = GetInventory();

		inventory.CreateInInventory( "HuntingOptic" );
		
		
		EntityAI entity;
		if ( Class.CastTo(entity, this) )
		{
			entity.SpawnEntityOnGroundPos("Ammo_308Win", entity.GetPosition());
		}
	}
	
};
class B95 : B95_base {};
class SawedoffB95 extends B95_base {};
