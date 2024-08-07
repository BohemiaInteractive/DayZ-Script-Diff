class ActionEmptyBottleBaseCB : ActionContinuousBaseCB
{	
	bool m_RPCStopAlreadySent;//since stopping contains a sound tail, we need to stop it only once, this bool ensures that
	override void CreateActionComponent()
	{
		float EmptiedQuantity;
		Bottle_Base bottle = Bottle_Base.Cast(m_ActionData.m_MainItem);
		if (bottle)
			EmptiedQuantity = bottle.GetLiquidEmptyRate() * bottle.GetLiquidThroughputCoef();
		m_ActionData.m_ActionComponent = new CAContinuousEmpty(EmptiedQuantity);
	}
	
	override void EndActionComponent()
	{
		ActionContinuousBase action = ActionContinuousBase.Cast(m_ActionData.m_Action);
		if (action.UseAlternativeInterrupt(m_ActionData))
			SetCommand(DayZPlayerConstants.CMD_ACTIONINT_FINISH);
		else
			SetCommand(DayZPlayerConstants.CMD_ACTIONINT_END);
	}
};

class ActionEmptyBottleBase: ActionContinuousBase
{
	void ActionEmptyBottleBase()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_EMPTY_VESSEL;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
		
		m_CallbackClass = ActionEmptyBottleBaseCB;
		m_FullBody = false;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "#empty";
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNone;
	}

	override bool HasTarget()
	{
		return false;
	}
	
	override bool HasAlternativeInterrupt()
	{
		return true;
	}
	
	override bool UseAlternativeInterrupt(ActionData action_data)
	{
		return HasAlternativeInterrupt() && (action_data.m_ActionComponent.GetProgress() >= 1);
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if ( GetGame().IsServer() && GetGame().IsMultiplayer() )
			return true;
		
		return item.IsLiquidPresent() && item.GetQuantity() > item.GetQuantityMin() && !item.GetIsFrozen();
	}
	
	override bool ActionConditionContinue( ActionData action_data )
	{
		return true;
	}
	
	override void OnStartAnimationLoop( ActionData action_data )
	{
		SendRPC(action_data,true);
	}
	
	override void OnEndAnimationLoop( ActionData action_data )
	{
		SendRPC(action_data,false);
	}
	
	override void OnEndServer( ActionData action_data )
	{
		SendRPC(action_data,false);
		
		super.OnEndServer(action_data);
	}

	protected void SendRPC(ActionData actionData, bool enable)
	{
		if ( !GetGame().IsMultiplayer() || GetGame().IsServer() )
		{
			ActionEmptyBottleBaseCB comp = ActionEmptyBottleBaseCB.Cast(actionData.m_Callback);
			if (comp.m_RPCStopAlreadySent)
				return;
			
			Bottle_Base target_vessel = Bottle_Base.Cast( actionData.m_MainItem );
			Param1<bool> play = new Param1<bool>( enable );
			GetGame().RPCSingleParam( target_vessel, SoundTypeBottle.EMPTYING, play, true );
			if (!enable)
				comp.m_RPCStopAlreadySent = true;
		}
	}
};