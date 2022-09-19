class Morphine: Inventory_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionInjectMorphineTarget);
		AddAction(ActionInjectMorphineSelf);
	}
	
	override void OnApply(PlayerBase player)
	{
		if( player.GetModifiersManager().IsModifierActive(eModifiers.MDF_MORPHINE ) )//effectively resets the timer
		{
			player.GetModifiersManager().DeactivateModifier( eModifiers.MDF_MORPHINE );
		}
		player.GetModifiersManager().ActivateModifier( eModifiers.MDF_MORPHINE );
	}
};
