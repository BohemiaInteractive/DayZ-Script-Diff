class ActionRestrainTargetCB : ActionContinuousBaseCB
{
	const float DEFAULT_RESTRAIN_TIME = 2;
	
	override void CreateActionComponent()
	{
		float time = DEFAULT_RESTRAIN_TIME;
		
		if ( m_ActionData.m_MainItem.ConfigIsExisting("RestrainTime") )
		{
			time = m_ActionData.m_MainItem.ConfigGetFloat("RestrainTime");
		}
		
		if ( m_ActionData.m_Player.IsQuickRestrain() )
		{
			time = DEBUG_QUICK_UNRESTRAIN_TIME;
		}
		
		m_ActionData.m_ActionComponent = new CAContinuousTime(time);
	}
};

class ActionRestrainTarget: ActionContinuousBase
{	
	void ActionRestrainTarget()
	{
		m_CallbackClass = ActionRestrainTargetCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_RESTRAINTARGET;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
		m_Text = "#restrain";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionTarget = new CCTMan(UAMaxDistances.DEFAULT);
		m_ConditionItem = new CCINonRuined;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		PlayerBase other_player = PlayerBase.Cast(target.GetObject());
		if (!other_player)
			return false;
		if ( player.GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_SERVER )
		{
			return other_player.CanBeRestrained();
		}
		else
		{
			return !other_player.IsRestrained();
		}

	}
	
	override int GetStanceMask(PlayerBase player)
	{
		if ( player.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_PRONE))
			return DayZPlayerConstants.STANCEMASK_CROUCH;
		else
			return DayZPlayerConstants.STANCEMASK_ERECT;
	}

	override bool ActionConditionContinue(ActionData action_data)
	{
		PlayerBase target_player = PlayerBase.Cast(action_data.m_Target.GetObject());
		
		if ( g_Game.IsServer() && g_Game.IsMultiplayer() )
		{
			if ( target_player.IsSurrendered() || !target_player.CanBeRestrained() )
			{
				return false;
			}
		}
		if ( g_Game.IsServer() )
		{
			ActionRestrainTargetCB callback = ActionRestrainTargetCB.Cast(action_data.m_Callback);
			
			if ( callback.GetActionComponentProgress() > 0.75 && !target_player.IsRestrainPrelocked() )
			{
				target_player.SetRestrainPrelocked(true);
			}
			
			return !g_Game.GetMission().IsPlayerDisconnecting(target_player);
			
		}
		return true;
	}
	
	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);
		
		PlayerBase target_player = PlayerBase.Cast(action_data.m_Target.GetObject());
		if ( target_player.IsSurrendered() )
		{
			SurrenderDataRestrain sdr = new SurrenderDataRestrain;
			target_player.EndSurrenderRequest(sdr);
		}
		else if ( target_player.IsEmotePlaying() )
		{
			target_player.m_EmoteManager.ServerRequestEmoteCancel();
		}
		
		target_player.SetRestrainStarted(true);
	}
	
	override void OnEndServer(ActionData action_data)
	{
		super.OnEndServer(action_data);
		
		PlayerBase target_player = PlayerBase.Cast(action_data.m_Target.GetObject());
		if (target_player)
		{
			target_player.SetRestrainStarted(false);
			target_player.SetRestrainPrelocked(false);
		}
	}

	override void OnFinishProgressServer( ActionData action_data )
	{
		PlayerBase target_player = PlayerBase.Cast(action_data.m_Target.GetObject());
		PlayerBase source_player = PlayerBase.Cast(action_data.m_Player);
		
		if (CanReceiveAction(action_data.m_Target) && !target_player.IsRestrained())
		{
			EntityAI item_in_hands_target = target_player.GetEntityInHands();
			EntityAI item_in_hands_source = source_player.GetEntityInHands();
			
			if ( !item_in_hands_source )
			{
				Error("Restraining target failed, nothing in hands");
				return;
			}
	
			string new_item_name = MiscGameplayFunctions.ObtainRestrainItemTargetClassname(item_in_hands_source);
			if (item_in_hands_target)
			{
				ChainedDropAndRestrainLambda lambda2 = new ChainedDropAndRestrainLambda(item_in_hands_target, item_in_hands_target.GetType(), target_player, false, source_player);
				MiscGameplayFunctions.TurnItemInHandsIntoItemEx(target_player, lambda2);
			}
			else
			{
				RestrainTargetPlayerLambda lambda = new RestrainTargetPlayerLambda(item_in_hands_source, new_item_name, target_player);
				source_player.LocalReplaceItemInHandsWithNewElsewhere(lambda);
			}
		}
	}
	
	override void OnFinishProgressClient( ActionData action_data )
	{
		super.OnFinishProgressClient( action_data );
		
		g_Game.GetAnalyticsClient().OnActionRestrain();
	}
};

class ChainedDropAndRestrainLambda : DestroyItemInCorpsesHandsAndCreateNewOnGndLambda
{
	PlayerBase m_SourcePlayer;
	ExplosivesBase m_PairedExplosive;

	void ChainedDropAndRestrainLambda (EntityAI old_item, string new_item_type, PlayerBase player, bool destroy = false, PlayerBase src_player = null)
	{
		m_SourcePlayer = src_player;
	}
	
	override void OnSuccess(EntityAI new_item)
	{
		super.OnSuccess(new_item);
		
		if (m_PairedExplosive)
		{
			m_PairedExplosive.UnpairRemote();
			ItemBase ib = ItemBase.Cast(new_item);
			ib.PairWithDevice(m_PairedExplosive);
			m_PairedExplosive.Arm();
		}
		
		// as soon as previous op is finish, start another one
		EntityAI item_in_hands_source = m_SourcePlayer.GetEntityInHands();
		if (item_in_hands_source)
		{
			string new_item_name = MiscGameplayFunctions.ObtainRestrainItemTargetClassname(item_in_hands_source);
			RestrainTargetPlayerLambda lambda = new RestrainTargetPlayerLambda(item_in_hands_source, new_item_name, m_Player);
			if (m_SourcePlayer)
				m_SourcePlayer.LocalReplaceItemInHandsWithNewElsewhere(lambda);
			else
				Error("ChainedDropAndRestrainLambda: missing source player!");
		}
		else
		{
			Error("ChainedDropAndRestrainLambda: missing source item in hands!");
		}
	}
	
	override void CopyOldPropertiesToNew (notnull EntityAI old_item, EntityAI new_item)
	{
		super.CopyOldPropertiesToNew(old_item, new_item);
		RemoteDetonatorTrigger trigger = RemoteDetonatorTrigger.Cast(old_item);
		if (trigger)
			m_PairedExplosive = ExplosivesBase.Cast(trigger.GetControlledDevice());
	}
}


class RestrainTargetPlayerLambda : TurnItemIntoItemLambda
{
	PlayerBase m_TargetPlayer;

	void RestrainTargetPlayerLambda (EntityAI old_item, string new_item_type, PlayerBase player)
	{
		m_TargetPlayer = player;
		
		InventoryLocation targetHnd = new InventoryLocation;
		targetHnd.SetHands(m_TargetPlayer, null);
		OverrideNewLocation(targetHnd);
	}
	
	override void OnSuccess(EntityAI new_item)
	{
		super.OnSuccess(new_item);

		m_TargetPlayer.SetRestrained(true);
		m_TargetPlayer.OnItemInHandsChanged();
	}
};

class SurrenderDataRestrain extends SurrenderData
{
	override void End()
	{
		
	}
}