class ActionDisarmExplosiveWithRemoteDetonatorCB : ActionDisarmExplosiveCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.DISARM_EXPLOSIVE_REMOTE_PAIRED);
	}	
}

class ActionDisarmExplosiveWithRemoteDetonator : ActionDisarmExplosive
{
	void ActionDisarmExplosiveWithRemoteDetonator()
	{
		m_CallbackClass	= ActionDisarmExplosiveWithRemoteDetonatorCB;
		m_CommandUID 	= DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
		m_StanceMask	= DayZPlayerConstants.STANCEMASK_CROUCH;
		m_FullBody 		= true;
		m_Text 			= "#disarm";
	}
	
	override void CreateConditionComponents()
	{
		m_ConditionItem 	= new CCINonRuined();
		m_ConditionTarget 	= new CCTNonRuined();
	}
	
	override bool CanBeSetFromInventory()
	{
		return true;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!target)
		{
			return false;
		}
		
		ExplosivesBase explosive = ExplosivesBase.Cast(target.GetObject());
		if (!explosive)
		{
			return false;
		}

		if (explosive.IsRuined() || !explosive.GetArmed() || !explosive.CanBeDisarmed())
		{
			return false;
		}
		
		if (explosive.GetAttachmentByType(KitchenTimer) || explosive.GetAttachmentByType(AlarmClock_ColorBase))
		{
			return false;
		}

		RemoteDetonatorTrigger rdt = RemoteDetonatorTrigger.Cast(item);
		if (rdt && rdt.IsConnected())
		{

			if (explosive != rdt.GetControlledDevice())
			{
				return false;
			}

			ExplosivesBase controlledDevice = ExplosivesBase.Cast(rdt.GetControlledDevice());
			if (controlledDevice && !controlledDevice.IsRuined() && controlledDevice.GetArmed())
			{
				return true;
			}
		}
		
		return false;
	}
	
	override void OnFinishProgressServer(ActionData action_data)
	{
		ExplosivesBase target;
		Class.CastTo(target, action_data.m_Target.GetObject());

		ItemBase detonator;
		Class.CastTo(detonator, action_data.m_MainItem);
		
		target.Disarm();
		target.SetTakeable(true);
		
		//! claymore has integrated detonator
		if (target.IsInherited(ClaymoreMine))
		{
			detonator.Delete();
			return;
		}
		
		ReplaceItemWithNewLambdaBase lambda = new ReplaceItemWithNewLambdaBase(detonator, "RemoteDetonator");
		MiscGameplayFunctions.TurnItemIntoItemEx(action_data.m_Player, lambda);
		//! refresh IK, item changed
		action_data.m_Player.GetItemAccessor().OnItemInHandsChanged();;
	}
}
