class ActionUnrestrainSelfCB : ActionContinuousBaseCB
{
	const float DEFAULT_STRUGGLE_TIME = 2;
	
	override void CreateActionComponent()
	{
		float time = DEFAULT_STRUGGLE_TIME;
		
		if ( m_ActionData.m_MainItem.ConfigIsExisting("StruggleLength") )
		{
			time = m_ActionData.m_MainItem.ConfigGetFloat("StruggleLength");
		}
		
		#ifdef DEVELOPER
		if ( m_ActionData.m_Player.IsQuickRestrain() )
		{
			time = DEBUG_QUICK_UNRESTRAIN_TIME;
		}
		#endif
		
		m_ActionData.m_ActionComponent = new CAContinuousTime(time);
	}
	
	override void EndActionComponent()
	{
		// TODO for second type animation SetCommand(DayZPlayerConstants.CMD_ACTIONINT_FINISH);
		if ( m_ActionData.m_State == UA_FINISHED )
		{
			SetCommand(DayZPlayerConstants.CMD_ACTIONINT_END);
		}
		else if (m_ActionData.m_State == UA_CANCEL )
		{
			m_Canceled = true;
 			SetCommand(DayZPlayerConstants.CMD_ACTIONINT_INTERRUPT);
			return;
			//Cancel();
		}
		else
		{
			m_Canceled = true;
			SetCommand(DayZPlayerConstants.CMD_ACTIONINT_INTERRUPT);
			return;
		}
		m_ActionData.m_State = UA_FINISHED;
	}	
};

class ActionUnrestrainSelf: ActionContinuousBase
{
	void ActionUnrestrainSelf()
	{
		m_CallbackClass = ActionUnrestrainSelfCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_RESTRAINEDSTRUGGLE;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_RESTRAINEDSTRUGGLE;
		m_Text = "#struggle";
	}
	
	override void CreateConditionComponents()  
	{		
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTSelf;
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		return player.IsRestrained();
	}
	
	override bool HasProneException()
	{
		return true;
	}

	override bool HasTarget()
	{
		return false;
	}
	
	override void OnEndAnimationLoopServer( ActionData action_data )
	{	

	}
	
	
	override void OnFinishProgressServer( ActionData action_data )
	{
		PlayerBase player = PlayerBase.Cast(action_data.m_Player);
		EntityAI item_in_hands = action_data.m_MainItem;

		if (item_in_hands)
		{
			if (item_in_hands.IsInherited(BarbedWireLocked))
			{
				int randNum = Math.RandomInt(0, 3); // Values 0 to 3 represent fore arm and arm roll bleed sources
				player.m_BleedingManagerServer.AttemptAddBleedingSourceBySelection(PlayerBase.m_BleedingSourcesUp[randNum]);
			}
			player.SetRestrained(false);
			item_in_hands.AddHealth(-GameConstants.CLEAN_UNRESTRAIN_DAMAGE);
			MiscGameplayFunctions.TransformRestrainItem(item_in_hands, null, null, player);
			//string new_item_name = MiscGameplayFunctions.ObtainRestrainItemTargetClassname(item_in_hands);			
			//MiscGameplayFunctions.TurnItemInHandsIntoItemEx(player, new UnrestrainSelfPlayer(item_in_hands, new_item_name));
		}
	}
	
	override bool CanBeUsedInRestrain()
	{
		return true;
	}
};

class UnrestrainSelfPlayer : ReplaceItemWithNewLambdaBase { };