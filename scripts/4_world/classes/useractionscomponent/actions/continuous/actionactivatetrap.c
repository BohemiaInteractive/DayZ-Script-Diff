class ActionActivateTrapCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.DEFAULT_DEPLOY);
	}
	
	override void InitActionComponent()
	{
		super.InitActionComponent();
		
		RegisterAnimationEvent("CraftingAction", UA_IN_CRAFTING);
	}
}

class ActionActivateTrap : ActionContinuousBase
{	
	void ActionActivateTrap()
	{
		m_CallbackClass		= ActionActivateTrapCB;
		m_CommandUID		= DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;
		m_FullBody			= true;
		m_StanceMask 		= DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Sound 			= "craft_universal_0";
		m_SpecialtyWeight 	= UASoftSkillsWeight.PRECISE_LOW;

		m_Text = "#activate";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionTarget = new CCTNonRuined( UAMaxDistances.DEFAULT );
		m_ConditionItem = new CCINotPresent;
	}
	
	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}

	override bool HasProgress()
	{
		return true;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		ItemBase targetItem = ItemBase.Cast(target.GetObject());
		if (targetItem && targetItem.IsBeingPlaced())
			return false;
		
		if (player.PhysicsGetLinkedEntity() != null)
			return false;
		
		Object targetObject = target.GetObject();
		if (targetObject != null)
		{
			if (targetObject.IsInherited(TrapBase))
			{
				TrapBase trap = TrapBase.Cast(targetObject);
				if (trap.IsActivable() && trap.IsPlaceableAtPosition(trap.GetPosition()))
					return true;
			}
			else if (targetObject.IsInherited(TrapSpawnBase))
			{
				TrapSpawnBase spawnTrap = TrapSpawnBase.Cast(targetObject);
			
				if (spawnTrap && !spawnTrap.IsActive())
					return spawnTrap.IsPlaceableAtPosition(spawnTrap.GetPosition());
			}
		} 
		
		return false;
	}
	
	override void OnStartAnimationLoop( ActionData action_data )
	{
		super.OnStartAnimationLoop( action_data );

		if ( !GetGame().IsMultiplayer() || GetGame().IsServer() )
		{
			TrapBase trap = TrapBase.Cast( action_data.m_Target.GetObject() );
			if ( trap )
			{
				Param1<bool> play = new Param1<bool>( true );
				GetGame().RPCSingleParam( trap, SoundTypeTrap.ACTIVATING, play, true );
			}
		}
	}
	
	override void OnEnd( ActionData action_data )
	{
		if ( !GetGame().IsMultiplayer() || GetGame().IsServer() )
		{
			TrapBase trap = TrapBase.Cast( action_data.m_Target.GetObject());
			if ( trap )
			{
				Param1<bool> play = new Param1<bool>( false );
				GetGame().RPCSingleParam( trap, SoundTypeTrap.ACTIVATING, play, true );
			}
		}
	}
	
	override void OnEndAnimationLoop(ActionData action_data)
	{
		super.OnEndAnimationLoop(action_data);

		if (!GetGame().IsMultiplayer() || GetGame().IsServer())
		{
			TrapBase trap = TrapBase.Cast( action_data.m_Target.GetObject());
			Param1<bool> play = new Param1<bool>(false);
			GetGame().RPCSingleParam(trap, SoundTypeTrap.ACTIVATING, play, true);
		}
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		Object targetObject = action_data.m_Target.GetObject();
		if (targetObject != null)
		{
			vector orientation = action_data.m_Player.GetOrientation();
			vector position =  targetObject.GetPosition();
			
			if (targetObject.IsInherited(TrapBase))
			{
				TrapBase trap = TrapBase.Cast(targetObject);
				trap.OnPlacementComplete(action_data.m_Player, position, orientation);
				trap.StartActivate(action_data.m_Player);
			}
			else if (targetObject.IsInherited(TrapSpawnBase))
			{
				TrapSpawnBase spawnTrap = TrapSpawnBase.Cast( targetObject );
				spawnTrap.OnPlacementComplete(action_data.m_Player, position, orientation);
			}
		}
	}
}
