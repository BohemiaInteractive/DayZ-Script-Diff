class ActionPackGiftCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.UNPACK);
	}
};

class ActionPackGift: ActionContinuousBase
{	
	void ActionPackGift()
	{
		m_CallbackClass = ActionPackGiftCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_OPENITEM;
		//m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_LOW;
		m_Text = "#STR_Wrap";
	}

	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNonRuined;
	}

	override bool HasTarget()	
	{
		return true;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		ItemBase item_to_pack = ItemBase.Cast(target.GetObject());
		if (!item_to_pack)
			return false;
		
		if ( !item_to_pack.IsTakeable() ) return false;
		if ( item_to_pack.IsBeingPlaced() ) return false;
		if ( BaseBuildingBase.Cast(item_to_pack) ) return false;

		if ( !item_to_pack.CanPutInCargo(null) ) return false;	
		
		
		EntityAI tgt_parent = EntityAI.Cast( target.GetParent() );
		
		if ( tgt_parent )
		{	
			if ( item_to_pack.GetInventory().IsAttachment() )
			{
				if ( !item_to_pack.CanDetachAttachment(tgt_parent) || !tgt_parent.CanReleaseAttachment(item_to_pack) )
					return false;
			}
			else
			{
				if ( !item_to_pack.CanRemoveFromCargo(tgt_parent) || !tgt_parent.CanReleaseCargo(item_to_pack) )
					return false;
			}
		}

		if ( item_to_pack.GetInventory().GetCargo() && item_to_pack.GetInventory().GetCargo().GetItemCount() > 0)
			return false;
		
		if ( !item_to_pack.IsWeapon() && item_to_pack.GetInventory().AttachmentCount() > 0)
			return false;
		
			
		int x,y;
		GetGame().GetInventoryItemSize( item_to_pack, x, y );
			
		if( x > 5 || y > 5 )
			return false;

		return true;
	}
	
	override void OnFinishProgressClient( ActionData action_data )
	{
		PlayerBase player = action_data.m_Player;
		ItemBase item_to_pack = ItemBase.Cast(action_data.m_Target.GetObject());
		
		player.RemoveQuickBarEntityShortcut(item_to_pack);
		
	}

	override void OnFinishProgressServer( ActionData action_data )
	{
		ItemBase item_to_pack = ItemBase.Cast(action_data.m_Target.GetObject());
		ItemBase item = action_data.m_MainItem;
		PlayerBase player = action_data.m_Player;
		string typeName;
		
		if (item_to_pack)
		{
			int x,y;
			GetGame().GetInventoryItemSize( item_to_pack, x, y );
			int rnd = Math.RandomIntInclusive(1, 4);
			
			ReplaceWithNewReciveCargoLambda lambda;
			
			if( x <= 2 && y <= 2 )
			{
				typeName = "GiftBox_Small_" + rnd;
			}
			else if( x <= 3 && y <= 3 )
			{
				typeName = "GiftBox_Medium_" + rnd;
			}
			else if( x <= 5 && y <= 5 )
			{
				typeName = "GiftBox_Large_" + rnd;
			}
			else return;
			
			
			if(!GetGame().IsDedicatedServer())
				player.RemoveQuickBarEntityShortcut(item_to_pack);
			
			lambda = new ReplaceWithNewReciveCargoLambda( item, typeName, player,item_to_pack);
			player.ServerReplaceItemWithNew(lambda);
			
		}
		

	}
};

class ReplaceWithNewReciveCargoLambda : TurnItemIntoItemLambda
{
	protected EntityAI m_ItemToCargo;
	void ReplaceWithNewReciveCargoLambda(EntityAI old_item, string new_item_type, PlayerBase player, EntityAI item_to_cargo) 
	{ 
		m_ItemToCargo = item_to_cargo;
	}
	
	override void OnSuccess(EntityAI new_item)
	{
		super.OnSuccess(new_item);
		
		InventoryLocation src = new InventoryLocation;
		m_ItemToCargo.GetInventory().GetCurrentInventoryLocation(src);
		InventoryLocation dst = new InventoryLocation;
		
		dst.SetCargo(new_item, m_ItemToCargo, 0,0,0,false);
					
		if ( dst.IsValid() )
		{
			if(GetGame().IsDedicatedServer())
				m_Player.ServerTakeToDst(src,dst);
			else
				m_Player.LocalTakeToDst(src,dst);
				
		}
	}
};