class PainkillerTablets extends Edible_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceConsumeSingle);
		AddAction(ActionEatTabletFromWrapper);
	}
	
	override void OnConsume(float amount, PlayerBase consumer)
	{
		if (consumer.GetModifiersManager().IsModifierActive(eModifiers.MDF_PAINKILLERS)) // effectively resets the timer
			consumer.GetModifiersManager().DeactivateModifier(eModifiers.MDF_PAINKILLERS);

		consumer.GetModifiersManager().ActivateModifier(eModifiers.MDF_PAINKILLERS);
	}
}
