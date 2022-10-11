class ActionDigOutStashCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.DIG_STASH);
	}
}

class ActionDigOutStash: ActionContinuousBase
{	
	void ActionDigOutStash()
	{
		m_CallbackClass 	= ActionDigOutStashCB;
		m_CommandUID 		= DayZPlayerConstants.CMD_ACTIONFB_DIGUPCACHE;
		m_FullBody 			= true;
		m_StanceMask 		= DayZPlayerConstants.STANCEMASK_ERECT;
		m_SpecialtyWeight 	= UASoftSkillsWeight.ROUGH_LOW;
		m_Text 				= "#dig_stash";
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionTarget	= new CCTObject(UAMaxDistances.DEFAULT);
		m_ConditionItem 	= new CCINonRuined();
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		ItemBase target_IB;
		if (Class.CastTo(target_IB, target.GetObject()))
		{			
			if (target_IB.CanBeDigged())
			{
				return target_IB.IsInherited(UndergroundStash);
			}
		}
		
		return false;
	}

	override void OnExecuteClient(ActionData action_data)
	{
		super.OnExecuteClient(action_data);
		
		SpawnParticleShovelRaise(action_data);
	}
	
	override void OnExecuteServer(ActionData action_data)
	{
		super.OnExecuteServer(action_data);
		
		if (!GetGame().IsMultiplayer())
		{
			SpawnParticleShovelRaise(action_data);
		}
	}
	
	void SpawnParticleShovelRaise(ActionData action_data)
	{
		ParticleManager.GetInstance().PlayOnObject(ParticleList.DIGGING_STASH, action_data.m_Player);
	}

	override void OnFinishProgressServer( ActionData action_data )
	{
		EntityAI targetEntity = EntityAI.Cast(action_data.m_Target.GetObject());
		if (!targetEntity)
		{
			ErrorEx("Cannot get entity=" + targetEntity);
			return;
		}
		
		InventoryLocation target_IL = new InventoryLocation();
		if (!targetEntity.GetInventory().GetCurrentInventoryLocation(target_IL))
		{
			ErrorEx("Cannot get inventory location of entity=" + targetEntity);
			return;
		}
		
		// Dig out of stash
		UndergroundStash stash;
		if (Class.CastTo(stash, targetEntity))
		{
			ItemBase stashedItem = stash.GetStashedItem();
			if (stashedItem)
			{
				DigOutStashLambda lambda(stash, "", action_data.m_Player);
				action_data.m_Player.ServerReplaceItemWithNew(lambda);
			}
			else
				g_Game.ObjectDelete(stash);
		}

		//Apply tool damage
		MiscGameplayFunctions.DealAbsoluteDmg(action_data.m_MainItem, 10);
		action_data.m_Player.GetSoftSkillsManager().AddSpecialty(m_SpecialtyWeight);
	}
	
	override string GetAdminLogMessage(ActionData action_data)
	{
		return string.Format("Player %1 Dug out %2 at position %3", action_data.m_Player, action_data.m_Target.GetObject(), action_data.m_Target.GetObject().GetPosition());
	}
}

class DigOutStashLambda : DropEquipAndDestroyRootLambda
{
	void DigOutStashLambda(EntityAI old_item, string new_item_type, PlayerBase player) {}

	override void CopyOldPropertiesToNew(notnull EntityAI old_item, EntityAI new_item)
	{
		super.CopyOldPropertiesToNew(old_item, new_item);
	}
}
