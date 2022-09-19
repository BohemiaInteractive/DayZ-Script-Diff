class ActionForceBitePainkillerTablets: ActionForceConsumeSingle
{
	override void ApplyModifiers( ActionData action_data )
	{
		PlayerBase ntarget = PlayerBase.Cast( action_data.m_Target.GetObject() );
		//target.m_ModifiersManager.ActivateModifier(eModifiers.MDF_CHEMICALPOISON);
	}
};