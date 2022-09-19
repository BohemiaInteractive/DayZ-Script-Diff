class CharcoalTablets extends Edible_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceConsumeSingle);
		AddAction(ActionConsumeSingle);
		//AddAction(ActionForceFeed);
		//AddAction(ActionEatBig);
	}
	
	override void OnConsume(float amount, PlayerBase consumer)
	{
		if( !consumer.GetModifiersManager().IsModifierActive(eModifiers.MDF_CHARCOAL ) )
		{
			consumer.GetModifiersManager().ActivateModifier( eModifiers.MDF_CHARCOAL );	
		}
		
	}
}