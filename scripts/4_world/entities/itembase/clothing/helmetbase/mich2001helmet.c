class Mich2001Helmet extends HelmetBase
{
	override bool CanPutAsAttachment( EntityAI parent )
	{
		if(!super.CanPutAsAttachment(parent)) {return false;}
		
		Clothing eyewear = Clothing.Cast(parent.FindAttachmentBySlotName("Eyewear"));
		
		if ( eyewear && eyewear.ConfigGetBool("isStrap") )
		{
			return false;
		}
		return true;
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionTurnOnHelmetFlashlight); //use default light actions instead?
		AddAction(ActionTurnOffHelmetFlashlight);
		AddAction(ActionToggleNVG);
	}
	
	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{
		EntityAI entity;
		if ( Class.CastTo(entity, this) )
		{
			entity.GetInventory().CreateInInventory( "NVGoggles" );
			entity.GetInventory().CreateInInventory( "UniversalLight" );
			entity.GetInventory().CreateInInventory( "Battery9V" );
			entity.GetInventory().CreateInInventory( "Battery9V" );
		}
	}
};