class ActionFeedTetracyclineAntibiotics: ActionForceConsume
{
	override void ApplyModifiers( ActionData action_data )
	{
		PlayerBase ntarget = PlayerBase.Cast( action_data.m_Target.GetObject() );
		//ntarget.m_ModifiersManager.ActivateModifier(eModifiers.MDF_CHEMICALPOISON);
	}
};