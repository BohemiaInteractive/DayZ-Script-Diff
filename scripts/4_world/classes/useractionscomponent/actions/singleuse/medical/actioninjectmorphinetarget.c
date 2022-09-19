class ActionInjectMorphineTarget: ActionInjectTarget
{
	void ActionInjectMorphineTarget()
	{
	}
		
	override void ApplyModifiers( ActionData action_data )
	{
		action_data.m_MainItem.OnApply(PlayerBase.Cast(action_data.m_Target.GetObject()));
		//PlayerBase ntarget = PlayerBase.Cast( target.GetObject() );
		//target.m_ModifiersManager.ActivateModifier(eModifiers.MDF_CHEMICALPOISON);
	}
};