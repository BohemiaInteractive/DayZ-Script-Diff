class ActionDigOutStashCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.DIG_STASH);
	}
};

class ActionDigOutStash: ActionContinuousBase
{	
	void ActionDigOutStash()
	{
		m_CallbackClass = ActionDigOutStashCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_DIGUPCACHE;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_LOW;
		
		m_Text = "#dig_stash";
	}
	
	override void CreateConditionComponents()  
	{	
		
		m_ConditionTarget = new CCTObject(UAMaxDistances.DEFAULT);
		m_ConditionItem = new CCINonRuined;
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		ItemBase target_IB;
		
		if ( Class.CastTo(target_IB, target.GetObject()) )
		{			
			if ( target_IB.CanBeDigged() )
			{
				if ( target_IB.IsInherited(UndergroundStash) )
				{
					return true;
				}
			}
		}
		
		return false;
	}

	override void OnExecuteClient( ActionData action_data )
	{
		super.OnExecuteClient( action_data );
		
		SpawnParticleShovelRaise( action_data );
	}
	
	override void OnExecuteServer( ActionData action_data )
	{
		super.OnExecuteServer( action_data );
		
		if ( !GetGame().IsMultiplayer() ) // Only in singleplayer
		{
			SpawnParticleShovelRaise( action_data );
		}
	}
	
	void SpawnParticleShovelRaise( ActionData action_data )
	{
		PlayerBase player = action_data.m_Player;
		ParticleManager.GetInstance().PlayOnObject( ParticleList.DIGGING_STASH, player );
	}
	
	
	override void OnFinishProgressServer( ActionData action_data )
	{
		Object targetObject = action_data.m_Target.GetObject();
		EntityAI targetEntity = EntityAI.Cast(targetObject);
		UndergroundStash target_stash;
		if (!targetEntity)
		{
			Error("ActionDigStash - Cannot get inventory of targetObject=" + targetObject);
			return;
		}
		
		InventoryLocation target_IL = new InventoryLocation;
		if (!targetEntity.GetInventory().GetCurrentInventoryLocation(target_IL))
		{
			Error("ActionDigStash: Cannot get inventory location of targetObject=" + targetObject);
			return;
		}
		
		// Dig out of stash
		if ( Class.CastTo(target_stash, targetEntity) )
		{
			ItemBase chest = target_stash.GetStashedItem();
			
			if (chest)
			{
				DigOutStashLambda lambda(target_stash, "", action_data.m_Player);
				action_data.m_Player.ServerReplaceItemWithNew(lambda);
			}
			else
				g_Game.ObjectDelete( target_stash );
		}

		//Apply tool damage
		MiscGameplayFunctions.DealAbsoluteDmg(action_data.m_MainItem, 10);
		
		action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
	}
	
	override string GetAdminLogMessage( ActionData action_data )
	{
		string message = string.Format("Player %1 Dug out %2 at position %3", action_data.m_Player, action_data.m_Target.GetObject(), action_data.m_Target.GetObject().GetPosition() );
		return message;
	}
};

class DigOutStashLambda : DropEquipAndDestroyRootLambda
{
	void DigOutStashLambda(EntityAI old_item, string new_item_type, PlayerBase player)
	{ }

	override void CopyOldPropertiesToNew(notnull EntityAI old_item, EntityAI new_item)
	{
		super.CopyOldPropertiesToNew(old_item, new_item);
	}
};