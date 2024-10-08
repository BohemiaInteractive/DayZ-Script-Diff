class ActionUncoverHeadSelfCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.COVER_HEAD);
	}
};


class ActionUncoverHeadBase: ActionContinuousBase
{
	void UncoverHead(PlayerBase target, PlayerBase source)
	{
		EntityAI attachment;
		if (Class.CastTo(attachment, target.GetInventory().FindAttachment(InventorySlots.HEADGEAR)) && attachment.GetType() == "BurlapSackCover")
		{
			ItemBase new_item = null;
			if (!source.GetHumanInventory().GetEntityInHands())
			{
				new_item = ItemBase.Cast(HumanInventory.Cast(source.GetInventory()).CreateInHands("BurlapSack"));
			}
			else
			{
				if (!Class.CastTo(new_item,source.GetInventory().CreateInInventory("BurlapSack")))//full inventory
				{
					vector m4[4];
					source.GetTransformWS(m4);
					InventoryLocation target_gnd = new InventoryLocation;
					target_gnd.SetGround(null, m4);
					new_item = ItemBase.Cast(GameInventory.LocationCreateEntity(target_gnd, "BurlapSack",ECE_IN_INVENTORY,RF_DEFAULT));
				}
			}
			
			if (new_item)
			{
				MiscGameplayFunctions.TransferItemProperties(attachment,new_item,true,false,true);
				attachment.Delete();
			}
		}
	}

}

class ActionUncoverHeadSelf: ActionUncoverHeadBase
{
	void ActionUncoverHeadSelf()
	{
		m_CallbackClass = ActionUncoverHeadSelfCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_COVERHEAD_SELF;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_PRONE;
	
		m_Text = "#uncover_head";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTNone;
	}

	override bool HasTarget()
	{
		return false;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		if ( IsWearingBurlap(player) ) 
			return true;

		return false;
	}

	override void OnFinishProgressServer( ActionData action_data )
	{	
		UncoverHead(action_data.m_Player, action_data.m_Player);
	}


	bool IsWearingBurlap( PlayerBase player )
	{
		EntityAI attachment;
		Class.CastTo(attachment, player.GetInventory().FindAttachment(InventorySlots.HEADGEAR));
		if ( attachment && attachment.IsInherited(BurlapSackCover) )
		{
			return true;
		}
		return false;		
	}
};