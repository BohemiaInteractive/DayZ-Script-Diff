class ActionDestroyPartCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousRepeat( UATimeSpent.DEFAULT_DESTROY/ActionDestroyPart.Cast(m_ActionData.m_Action).CYCLES );
	}
};

class ActionDestroyPart: ActionContinuousBase
{
	static int CYCLES = 4;
	
	void ActionDestroyPart()
	{
		m_CallbackClass = ActionDestroyPartCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_DISASSEMBLE;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT;		
		
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_HIGH;
		m_Text = "#destroy";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNone;
	}
	
	override void OnActionInfoUpdate( PlayerBase player, ActionTarget target, ItemBase item )
	{
		ConstructionActionData construction_action_data = player.GetConstructionActionData();	
		m_Text = "#destroy " + construction_action_data.GetTargetPart();
	}
	
	override bool CanBeUsedLeaning()
	{
		return false;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		//Action not allowed if player has broken legs
		if (player.GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS)
			return false;
		
		return DestroyCondition( player, target, item, true );
	}
	
	override bool ActionConditionContinue( ActionData action_data )
	{	
		return DestroyCondition( action_data.m_Player, action_data.m_Target, action_data.m_MainItem , false );
	}
	
	override void OnFinishProgressServer( ActionData action_data )
	{	
		BaseBuildingBase base_building = BaseBuildingBase.Cast( action_data.m_Target.GetObject() );
		Construction construction = base_building.GetConstruction();
		ConstructionActionData construction_action_data = action_data.m_Player.GetConstructionActionData();
		ConstructionPart construction_part = construction_action_data.GetTargetPart();
		
		if ( construction.CanDestroyPart( construction_part.GetPartName() ) )
		{
			//build
			string part_name = construction_part.GetPartName();
			string zone_name;
			DamageSystem.GetDamageZoneFromComponentName(base_building,part_name,zone_name);
			
			if (zone_name != "")
			{
				base_building.AddHealth(zone_name,"Health",-(base_building.GetMaxHealth(zone_name,"")/CYCLES));
				if ( base_building.GetHealth(zone_name,"Health") < 1 )
					construction.DestroyPartServer( action_data.m_Player, construction_part.GetPartName(), AT_DESTROY_PART );
			}
			else
			{
				construction.DestroyPartServer( action_data.m_Player, construction_part.GetPartName(), AT_DESTROY_PART );
			}
			
			//add damage to tool
			action_data.m_MainItem.DecreaseHealth( UADamageApplied.DESTROY, false );
		}

		action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
	}
	
	protected bool DestroyCondition( PlayerBase player, ActionTarget target, ItemBase item, bool camera_check )
	{	
		Object target_object = target.GetObject();
		if ( target_object && target_object.CanUseConstruction() )
		{
			string part_name = target_object.GetActionComponentName( target.GetComponentIndex() );
			
			BaseBuildingBase base_building = BaseBuildingBase.Cast( target_object );
			Construction construction = base_building.GetConstruction();		
			ConstructionPart construction_part = construction.GetConstructionPartToDestroy( part_name );
			
			if ( construction_part )
			{
				//camera and position checks
				if ( !player.GetInputController().CameraIsFreeLook() && IsInReach(player, target, UAMaxDistances.DEFAULT) && !player.GetInputController().CameraIsFreeLook() )
				{
					//Camera check (client-only)
					if ( camera_check )
					{
						if ( GetGame() && ( !GetGame().IsDedicatedServer() ) )
						{
							if ( !base_building.IsFacingCamera( part_name ) )
							{
								return false;
							}
						}
					}
					
					ConstructionActionData construction_action_data = player.GetConstructionActionData();
					construction_action_data.SetTargetPart( construction_part );
					
					return true;				
				}
			}
		}
		
		return false;
	}
	
	override string GetAdminLogMessage(ActionData action_data)
	{
		return " destroyed " + action_data.m_Target.GetObject().GetDisplayName() + " with " + action_data.m_MainItem.GetDisplayName();
	}
}