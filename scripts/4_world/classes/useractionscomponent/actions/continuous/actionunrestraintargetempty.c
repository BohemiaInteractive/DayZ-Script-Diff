class ActionUnrestrainTargetHandsCB : ActionContinuousBaseCB
{
	const float DEFAULT_STRUGGLE_TIME = 2;
	const float UNRESTRAIN_STRUGGLE_MULTIPLIER = 0.75;//multiplier for the struggle time normally used for the player unrestraining themselves, this allows unrestraining to be bound to the 'StruggleLength' config param
	
	override void CreateActionComponent()
	{
		float time = DEFAULT_STRUGGLE_TIME;
		PlayerBase targetPlayer = PlayerBase.Cast(m_ActionData.m_Target.GetObject());
		EntityAI item_in_hands = targetPlayer.GetItemInHands();
		
		if ( item_in_hands.ConfigIsExisting("StruggleLength") )
		{
			time = item_in_hands.ConfigGetFloat("StruggleLength") * UNRESTRAIN_STRUGGLE_MULTIPLIER;
		}
		#ifdef DEVELOPER
		if ( m_ActionData.m_Player.IsQuickRestrain() )
		{
			time = DEBUG_QUICK_UNRESTRAIN_TIME;
		}
		#endif
		
		m_ActionData.m_ActionComponent = new CAContinuousTime(time);
	}
};

class ActionUnrestrainTargetHands: ActionContinuousBase
{	
	void ActionUnrestrainTargetHands()
	{
		m_CallbackClass = ActionUnrestrainTargetHandsCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_RESTRAINTARGET;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "#unrestrain";
	}
	
	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}
	
	
	
	override void OnStartServer(ActionData action_data)
	{
		action_data.m_Player.TryHideItemInHands(true);
	}

	override void OnEndServer(ActionData action_data)
	{
		action_data.m_Player.TryHideItemInHands(false);
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTObject(UAMaxDistances.DEFAULT);
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		PlayerBase targetPlayer = PlayerBase.Cast(target.GetObject());
		return targetPlayer && targetPlayer.IsRestrained();
	}

	override void OnFinishProgressServer( ActionData action_data )
	{
		PlayerBase targetPlayer = PlayerBase.Cast(action_data.m_Target.GetObject());
		
		
		EntityAI item_in_hands = targetPlayer.GetItemInHands();
		targetPlayer.SetRestrained(false);
		if (item_in_hands)
		{
			MiscGameplayFunctions.TransformRestrainItem(item_in_hands, null, action_data.m_Player, targetPlayer);
		}
		else
		{
			ErrorEx("Failed to obtain item in target player's hands during empty handed unrestraining");
		}
	}
	
	override void OnStartClient(ActionData action_data)
	{
		action_data.m_Player.TryHideItemInHands(true);
	}
	
	override void OnEndClient( ActionData action_data )
	{
		if ( action_data.m_Player ) action_data.m_Player.TryHideItemInHands(false);
	}
	
};