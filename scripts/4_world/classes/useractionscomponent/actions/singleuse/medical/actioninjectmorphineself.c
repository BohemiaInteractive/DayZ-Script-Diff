class ActionInjectMorphineSelf: ActionInjectSelf
{
	override void ApplyModifiers( ActionData action_data )
	{
		action_data.m_MainItem.OnApply(action_data.m_Player);
		//player.m_ModifiersManager.ActivateModifier(eModifiers.MDF_CHEMICALPOISON);
	}
};