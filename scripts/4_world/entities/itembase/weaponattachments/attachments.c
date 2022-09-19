class ButtstockBase extends Inventory_Base
{
	override void OnWasAttached(EntityAI parent, int slot_id)
	{
		super.OnWasAttached(parent, slot_id);
		
		if( parent.IsWeapon() )
		{
			parent.SetButtstockAttached(true,slot_id);
		}		
	}
	
	override void OnWasDetached(EntityAI parent, int slot_id)
	{
		super.OnWasDetached(parent, slot_id);
		
		if( parent.IsWeapon() )
		{
			parent.SetButtstockAttached(false);
		}
	}
};

class AK74_WoodBttstck : ButtstockBase {};
class AKS74U_Bttstck : ButtstockBase {};
class AK_FoldingBttstck : ButtstockBase {};
class AK_PlasticBttstck : ButtstockBase {};
class AK_WoodBttstck : ButtstockBase {};
class Fal_FoldingBttstck : ButtstockBase {};
class Fal_OeBttstck : ButtstockBase {};
class GhillieAtt_ColorBase : ButtstockBase {};
class Groza_LowerReceiver : ButtstockBase {};
class M249_Bipod : ButtstockBase {};
class M4_CQBBttstck : ButtstockBase {};
class M4_MPBttstck : ButtstockBase {};
class M4_OEBttstck : ButtstockBase {};
class MP5k_StockBttstck : ButtstockBase {};
class Saiga_Bttstck : ButtstockBase {};
class PP19_Bttstck : ButtstockBase {};
