/**@class		Mosin9130
 * @brief		base for Mosin
 * @NOTE		name copies config base class
 **/
class Mosin9130_Base extends BoltActionRifle_InnerMagazine_Base
{
	void Mosin9130_Base ()
	{
	}
	
	override bool CanEnterIronsights()
	{
		ItemOptics optic = GetAttachedOptics();
		if (optic && PUScopeOptic.Cast(optic))
			return true;
		return super.CanEnterIronsights();
	}
};


class Mosin9130 extends Mosin9130_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new MosinRecoil(this);
	}
	
				
	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{
		super.OnDebugSpawn();
		GameInventory inventory = GetInventory();

		inventory.CreateInInventory( "PUScopeOptic" );
		inventory.CreateInInventory( "Mosin_Compensator" );	
		
		EntityAI entity;
		if ( Class.CastTo(entity, this) )
		{
			entity.SpawnEntityOnGroundPos("Ammo_762x54", entity.GetPosition());
		}
	}
};

class SawedoffMosin9130_Base extends Mosin9130_Base
{
	override RecoilBase SpawnRecoilObject()
	{
		return new MosinSawedOffRecoil(this);
	}
};
