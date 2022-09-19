//Transfer locktypes from BasicDefines.hpp here. This is mostly for readability
//NONE must be 0 as it is actively used below
enum eLockTypes
{
	NONE = 0,
	LOCK_SCREW = 1,
	LOCK_BOLT = 2,
	LOCK_NUT = 3,
	LOCK_WIRE = 4
}

class ActionLockAttachmentCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime( UATimeSpent.BASEBUILDING_REPAIR_FAST );
	}
};

class ActionLockAttachment: ActionContinuousBase
{
	private const string LOCK_VERSION = "#widget_lock";
	private const string UNLOCK_VERSION = "#widget_unlock";
	private string m_Name = LOCK_VERSION;
	
	void ActionLockAttachment()
	{
		m_CallbackClass = ActionLockAttachmentCB;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
		
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT;
		m_FullBody = true;
	}
	
	override void OnActionInfoUpdate( PlayerBase player, ActionTarget target, ItemBase item )
	{
		ItemBase target_IB = ItemBase.Cast(target.GetObject());
		if ( target_IB.IsLockedInSlot() )
			m_Text = UNLOCK_VERSION;
		else if ( target_IB.IsAlive() )
			m_Text = LOCK_VERSION;
		else
			m_Text = "";
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTCursorNoRuinCheck(UAMaxDistances.SMALL);
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if ( !target ) 
			return false;
		
		EntityAI parent_EAI = EntityAI.Cast(target.GetParent());
		ItemBase target_IB = ItemBase.Cast(target.GetObject());
		
		array<int> compLock = item.m_CompatibleLocks;
		int targetType = eLockTypes.NONE;
		
		if ( target_IB && parent_EAI )
		{
			//CAR DOOR HANDLING
			//I don't really like this, but sometimes specifics have to be handled in generics
			CarDoor targetDoor = CarDoor.Cast(target_IB);
			if ( targetDoor )
			{
				InventoryLocation loc = new InventoryLocation();
				bool isPresent = targetDoor.GetInventory().GetCurrentInventoryLocation( loc );
				
				if ( !isPresent || loc.GetSlot() == -1 )
					return false;
				
				string slotName = InventorySlots.GetSlotName( loc.GetSlot() );
				if ( slotName && CarScript.Cast( parent_EAI ).GetCarDoorsState( slotName ) != CarDoorState.DOORS_OPEN )
					return false;
			}
			//END OF CAR DOOR SPECIFICS
			
			
			if ( target_IB.IsLockedInSlot() )
				m_Name = UNLOCK_VERSION;
			else if ( target_IB.IsAlive() )
				m_Name = LOCK_VERSION;
			else if ( !target_IB.IsLockedInSlot() ) 
				return false;
			
			targetType = target_IB.GetLockType();
		
			for (int i = 0; i < compLock.Count(); i++)
			{
				if ( targetType == compLock[i] && targetType != eLockTypes.NONE)
				{
					return true;
				}
			}
		}
		return false;
	}
	
	override void OnStartAnimationLoop( ActionData action_data )
	{
		super.OnStartAnimationLoop( action_data );

		if ( !GetGame().IsMultiplayer() || GetGame().IsServer() )
		{
			ItemBase target_IB = ItemBase.Cast( action_data.m_Target.GetObject() );
			Param2<bool, string> play = new Param2<bool, string>( true, action_data.m_MainItem.GetLockSoundSet() );
			GetGame().RPCSingleParam( target_IB, ERPCs.RPC_SOUND_LOCK_ATTACH, play, true );
		}
	}
	
	override void OnEnd( ActionData action_data )
	{
		if ( !GetGame().IsMultiplayer() || GetGame().IsServer() )
		{
			ItemBase target_IB = ItemBase.Cast( action_data.m_Target.GetObject() );
			Param2<bool, string> play = new Param2<bool, string>( false, action_data.m_MainItem.GetLockSoundSet() );
			GetGame().RPCSingleParam( target_IB, ERPCs.RPC_SOUND_LOCK_ATTACH, play, true );
		}
	}
	
	override void OnEndAnimationLoop( ActionData action_data )
	{
		super.OnEndAnimationLoop( action_data );

		if ( !GetGame().IsMultiplayer() || GetGame().IsServer() )
		{
			ItemBase target_IB = ItemBase.Cast( action_data.m_Target.GetObject() );
			Param2<bool, string> play = new Param2<bool, string>( false, action_data.m_MainItem.GetLockSoundSet() );
			GetGame().RPCSingleParam( target_IB, ERPCs.RPC_SOUND_LOCK_ATTACH, play, true );
		}
	}
	
	override void OnFinishProgressServer( ActionData action_data )
	{	
		ItemBase target_IB = ItemBase.Cast(action_data.m_Target.GetObject());
		
		MiscGameplayFunctions.DealAbsoluteDmg(action_data.m_MainItem, 5);
		
		if (target_IB.IsLockedInSlot())
		{
			target_IB.UnlockFromParent();
			return;
		}
		
		target_IB.LockToParent();
	}
}