class ActionOpenFence: ActionInteractBase
{
	ref NoiseParams m_NoisePar;
	const string SELECTION_INTERACT = "gate_interact";
	
	void ActionOpenFence()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_OPENDOORFW;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_Text = "#open";
	}

	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTCursor;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		Object targetObject = target.GetObject();
		if ( targetObject && targetObject.CanUseConstruction() )
		{
			Fence fence = Fence.Cast( targetObject );
			
			if ( fence && fence.CanOpenFence() )
			{
				array<string> selections = new array<string>;
				targetObject.GetActionComponentNameList(target.GetComponentIndex(), selections);
				
				for ( int i = 0; i < selections.Count(); i++ )
				{
					if ( selections[i] == SELECTION_INTERACT )
						return true;
				}
			}
		}
		
		return false;
	}
	
	override void OnStartServer( ActionData action_data )
	{
		Fence fence = Fence.Cast( action_data.m_Target.GetObject() );
		fence.OpenFence();
	}
	
	override void OnEndServer( ActionData action_data )
	{
		m_NoisePar = new NoiseParams();
		m_NoisePar.LoadFromPath("CfgVehicles SurvivorBase NoiseActionDefault");
		NoiseSystem noise = GetGame().GetNoiseSystem();
		if ( noise )
		{
			if ( action_data.m_Player )
				noise.AddNoisePos(action_data.m_Player, action_data.m_Target.GetObject().GetPosition(), m_NoisePar);
		}
	}
}