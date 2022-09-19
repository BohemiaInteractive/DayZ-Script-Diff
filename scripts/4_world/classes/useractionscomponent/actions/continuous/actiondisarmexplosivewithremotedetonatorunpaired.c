class ActionDisarmExplosiveWithRemoteDetonatorUnpairedCB : ActionDisarmExplosiveCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.DISARM_EXPLOSIVE_REMOTE_UNPAIRED);
	}	
}

class ActionDisarmExplosiveWithRemoteDetonatorUnpaired : ActionDisarmExplosiveWithRemoteDetonator
{
	void ActionDisarmExplosiveWithRemoteDetonatorUnpaired()
	{
		m_CallbackClass	= ActionDisarmExplosiveWithRemoteDetonatorUnpairedCB;
		m_CommandUID 	= DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
		m_StanceMask	= DayZPlayerConstants.STANCEMASK_CROUCH;
		m_FullBody 		= true;
		m_Text 			= "#disarm";
	}
	
	override void CreateConditionComponents()
	{
		m_ConditionItem 	= new CCINonRuined();
		m_ConditionTarget 	= new CCTCursor();
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!target)
		{
			return false;
		}

		//! explosives only
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
		
		RemoteDetonatorReceiver attachedReceiver = RemoteDetonatorReceiver.Cast(explosive.GetAttachmentByType(RemoteDetonatorReceiver));
		if (!explosive.IsInherited(ClaymoreMine) && attachedReceiver && attachedReceiver.IsRuined())
		{
			return false;
		}
		
		//! RD trigger
		RemoteDetonatorTrigger rdt = RemoteDetonatorTrigger.Cast(item);
		if (rdt && target.GetObject() != rdt.GetControlledDevice())
		{
			return true;
		}
		
		//! RD kit
		RemoteDetonator rd = RemoteDetonator.Cast(item);
		if (rd && rd.IsKit())
		{
			return true;
		}
		
		return false;
	}
}
