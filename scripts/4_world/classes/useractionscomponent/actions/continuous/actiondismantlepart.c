class ActionDismantlePartCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		float time = SetCallbackDuration(m_ActionData.m_MainItem);
		m_ActionData.m_ActionComponent = new CAContinuousTime( time );
	}
	
	float SetCallbackDuration( ItemBase item )
	{
		/*switch( item.Type() )
		{
			case Pickaxe:
			case Shovel:
			case FieldShovel:
				return UATimeSpent.BASEBUILDING_DECONSTRUCT_MEDIUM;
			case FirefighterAxe:
				return UATimeSpent.BASEBUILDING_DECONSTRUCT_FAST;
			default:
				return UATimeSpent.BASEBUILDING_DECONSTRUCT_SLOW;
		}*/
		return UATimeSpent.BASEBUILDING_DECONSTRUCT_SLOW;
	}
};

class ActionDismantlePart: ActionContinuousBase
{
	void ActionDismantlePart()
	{
		m_CallbackClass = ActionDismantlePartCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_DISASSEMBLE;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT;	
		
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_HIGH;
		m_Text = "#dismantle";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNone;//CCTNonRuined( UAMaxDistances.BASEBUILDING );
	}
	
	override void OnActionInfoUpdate( PlayerBase player, ActionTarget target, ItemBase item )
	{
		ConstructionActionData construction_action_data = player.GetConstructionActionData();
		m_Text = "#dismantle " + construction_action_data.GetTargetPart().GetName();
	}
		
	override string GetText()
	{
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		if ( player )
		{
			ConstructionActionData construction_action_data = player.GetConstructionActionData();
			ConstructionPart constrution_part = construction_action_data.GetTargetPart();
			
			if ( constrution_part )
			{
				return "#dismantle" + " " + constrution_part.GetName();
			}
		}
		
		return "";
	}
	
	override bool CanBeUsedLeaning()
	{
		return false;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		if ( player.IsPlacingLocal() )
			return false;
		
		//Action not allowed if player has broken legs
		if (player.GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS)
			return false;
		
		return DismantleCondition( player, target, item, true ) && player.m_MovementState.m_iStanceIdx != DayZPlayerConstants.STANCEIDX_PRONE;
	}
	
	override bool ActionConditionContinue( ActionData action_data )
	{	
		return DismantleCondition( action_data.m_Player, action_data.m_Target, action_data.m_MainItem , false ) && action_data.m_Player.m_MovementState.m_iStanceIdx != DayZPlayerConstants.STANCEIDX_PRONE;
	}	
	
	override void OnFinishProgressServer( ActionData action_data )
	{	
		BaseBuildingBase base_building = BaseBuildingBase.Cast( action_data.m_Target.GetObject() );
		Construction construction = base_building.GetConstruction();
		ConstructionActionData construction_action_data = action_data.m_Player.GetConstructionActionData();
		ConstructionPart construction_part = construction_action_data.GetTargetPart();
		
		if ( construction.CanDismantlePart( construction_part.GetPartName(), action_data.m_MainItem ) )
		{
			//build
			construction.DismantlePartServer( action_data.m_Player, construction_part.GetPartName(), AT_DISMANTLE_PART );
			
			//add damage to tool
			action_data.m_MainItem.DecreaseHealth( UADamageApplied.DISMANTLE, false );
		}

		action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
	}
		
	//setup
	override bool SetupAction( PlayerBase player, ActionTarget target, ItemBase item, out ActionData action_data, Param extra_data = NULL )
	{	
		if ( super.SetupAction( player, target, item, action_data, extra_data ) )
		{
			SetBuildingAnimation( item );
			
			return true;
		}
		
		return false;
	}
	
	protected void SetBuildingAnimation( ItemBase item )
	{
		switch ( item.Type() )
		{
			case Pickaxe:
			case Shovel:
			case FarmingHoe:
			case FieldShovel:
				m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_DIG;
				break;
			case Pliers:
				m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
				break;				
			default:
				m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_DISASSEMBLE;
				break;
		}
	}	
	
	protected bool DismantleCondition( PlayerBase player, ActionTarget target, ItemBase item, bool camera_check )
	{	
		if ( player && !player.IsPlacingLocal() && !player.IsPlacingServer() )
		{
			Object target_object = target.GetObject();
			EntityAI target_entity;
			if ( target_object && target_object.CanUseConstruction() )
			{
				//invalid if is gate and is locked
				if ( Class.CastTo(target_entity,target_object) && (target_entity.FindAttachmentBySlotName("Att_CombinationLock") || target_entity.FindAttachmentBySlotName("Material_FPole_Flag")) )
				{
					return false;
				}
				
				string part_name = target_object.GetActionComponentName( target.GetComponentIndex() );
				
				BaseBuildingBase base_building = BaseBuildingBase.Cast( target_object );
				Construction construction = base_building.GetConstruction();		
				ConstructionPart construction_part = construction.GetConstructionPartToDismantle( part_name, item );
				
				if ( construction_part )
				{
					/*Print("DismantleCondition");
					Print(part_name);
					Print("construction_part.GetPartName: " + construction_part.GetPartName());
					Print("construction_part.GetMainPartName: " + construction_part.GetMainPartName());
					Print("-----");*/
					
					//invalid on gate if the gate is opened
					if ( construction_part.IsGate() && base_building.IsOpened() )
						return false;
					
					//camera and position checks
					bool checked = false;
					
					if ( construction_part.IsBase() )
						checked = true;
					
					if ( !checked && base_building.IsPlayerInside( player, part_name ) && !player.GetInputController().CameraIsFreeLook() )
					{
						//Camera check (client-only)
						if ( camera_check )
						{
							if ( GetGame() && ( !GetGame().IsDedicatedServer() ) )
							{
								if ( base_building.IsFacingCamera( part_name ) )
								{
									return false;
								}
							}
						}

						checked = true;
					}

					if ( checked )
					{
						ConstructionActionData construction_action_data = player.GetConstructionActionData();
						construction_action_data.SetTargetPart( construction_part );
						
						return true;
					}
				}
			}
		}
		
		return false;
	}
	
	override string GetAdminLogMessage(ActionData action_data)
	{
		ConstructionActionData construction_action_data = action_data.m_Player.GetConstructionActionData();
		ConstructionPart construction_part = construction_action_data.GetTargetPart();
		
		string message = string.Format("Dismantled %1 from %2 with %3", construction_part.GetName(), action_data.m_Target.GetObject().GetDisplayName(), action_data.m_MainItem.GetDisplayName() );
		return message;
	}
}