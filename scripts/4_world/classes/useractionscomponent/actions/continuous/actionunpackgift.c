class ActionUnpackGiftCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.UNPACK);
	}
};

class ActionUnpackGift: ActionContinuousBase
{	
	void ActionUnpackGift()
	{
		m_CallbackClass = ActionUnpackGiftCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_OPENITEM;
		//m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_LOW;
		m_Text = "#STR_Unwrap";
	}

	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTSelf;
	}

	override bool HasTarget()	
	{
		return false;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		return true;
	}

	override void OnFinishProgressServer( ActionData action_data )
	{
		ItemBase item = ItemBase.Cast(action_data.m_MainItem);
		
		CargoBase cargo = item.GetInventory().GetCargo();
		
		InventoryLocation il_dst = new InventoryLocation;
		InventoryLocation il_src = new InventoryLocation;
		
		float dmg = item.GetHealth01("","");
		
		if( dmg > 0.25 )
		{
			Object paper = GetGame().CreateObjectEx("GiftWrapPaper",action_data.m_Player.GetPosition(),ECE_PLACE_ON_SURFACE,RF_DEFAULT);
			paper.SetHealth01("","", dmg - 0.25 );
		}
		
		for(int i = cargo.GetItemCount() - 1; i >= 0 ; i--)
		{
			EntityAI cargo_item = cargo.GetItem(i);

			GameInventory.SetGroundPosByOwner(action_data.m_Player,cargo_item,il_dst);
			cargo_item.GetInventory().GetCurrentInventoryLocation(il_src);

			if(GetGame().IsDedicatedServer())
				action_data.m_Player.ServerTakeToDst( il_src, il_dst );
			else
				action_data.m_Player.LocalTakeToDst( il_src, il_dst );
		}

		item.DeleteSafe();
	}
};