class ActionForceFeedCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousQuantityEdible(UAQuantityConsumed.EAT_NORMAL,UATimeSpent.DEFAULT);
	}
};

class ActionForceFeed: ActionForceConsume
{
	void ActionForceFeed()
	{
		m_CallbackClass = ActionForceFeedCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_FORCEFEED;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		
		m_Text = "#feed";
		//m_Animation = "feed";
	}
	
	override void CreateConditionComponents() 
	{
		m_ConditionTarget = new CCTMan(UAMaxDistances.DEFAULT);
		m_ConditionItem = new CCINonRuined;
	}
	

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if (!super.ActionCondition( player, target, item))
			return false;
		
		PlayerBase target_player = PlayerBase.Cast(target.GetObject());
		
		if (target_player)
			return target_player.CanEatAndDrink();
		else
			return false;
	}
	
	
	override void OnEndServer( ActionData action_data )
	{	
		super.OnEndServer(action_data);
		if ( action_data.m_Player.HasBloodyHands() && !action_data.m_Player.GetInventory().FindAttachment( InventorySlots.GLOVES ) )
		{
			Object targetPlayer = action_data.m_Target.GetObject();
			PlayerBase target = PlayerBase.Cast( targetPlayer );
			if ( target )
			{
				target.SetBloodyHandsPenalty();
			}
		}
	}
	
		
		
	
};


//-----------------SMALL BITES VARIANT-------------------

class ActionForceFeedSmallCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousQuantityEdible(UAQuantityConsumed.EAT_NORMAL,UATimeSpent.DEFAULT);
	}
};

class ActionForceFeedSmall: ActionForceConsume
{
	void ActionForceFeed()
	{
		m_CallbackClass = ActionForceFeedSmallCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_FORCEFEED;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		
		
		//m_Animation = "feed";
	}
	
	override void CreateConditionComponents() 
	{
		m_ConditionTarget = new CCTMan(UAMaxDistances.DEFAULT);
		m_ConditionItem = new CCINonRuined;
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if (!super.ActionCondition( player, target, item))
			return false;
		
		PlayerBase target_player = PlayerBase.Cast(target.GetObject());
		
		if (target_player)
			return target_player.CanEatAndDrink();
		else
			return false;
	}
	
		
	override string GetText()
	{
		return "#feed";
	}
};