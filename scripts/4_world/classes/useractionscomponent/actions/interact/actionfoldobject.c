class ActionFoldObject: ActionInteractBase
{
	void ActionFoldObject()
	{
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_MEDIUM;
		m_Text = "#fold";
	}

	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}
	
	override bool HasProgress()
	{
		return false;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		Object targetObject = target.GetObject();
		if ( targetObject.IsInherited(HescoBox) )
		{
			HescoBox hesco = HescoBox.Cast( targetObject );
			
			if ( hesco.GetState() == HescoBox.UNFOLDED )
			{
				return true;
			}
		}
		return false;
	}

	override void OnExecuteServer( ActionData action_data )
	{
		Object targetObject = action_data.m_Target.GetObject();
		HescoBox hesco = HescoBox.Cast( targetObject );
		if ( hesco.GetState() == HescoBox.UNFOLDED )
		{
			hesco.Fold();
		}
		
		action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
	}
};