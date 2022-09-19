class BuildPartActionReciveData : ActionReciveData
{
	string m_PartType;
}

class BuildPartActionData : ActionData
{
	string m_PartType;
};

class ActionBuildPartCB : ActionContinuousBaseCB
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
				return UATimeSpent.BASEBUILDING_CONSTRUCT_MEDIUM;
			default:
				return UATimeSpent.BASEBUILDING_CONSTRUCT_FAST;
		}*/
		return UATimeSpent.BASEBUILDING_CONSTRUCT_MEDIUM;
	}
};

class ActionBuildPart: ActionContinuousBase
{
	void ActionBuildPart()
	{
		m_CallbackClass = ActionBuildPartCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_ASSEMBLE;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT;
		
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_HIGH;
		m_Text = "#build";
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNone;
	}
	
	override void OnActionInfoUpdate( PlayerBase player, ActionTarget target, ItemBase item )
	{
		ConstructionActionData construction_action_data = player.GetConstructionActionData();
		ConstructionPart constrution_part = construction_action_data.GetBuildPartAtIndex(m_VariantID);
			
		if ( constrution_part )
		{
			m_Text = "#build " + constrution_part.GetName();
		}
	}
	
	override bool CanBeUsedLeaning()
	{
		return false;
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if ( player.IsPlacingLocal() || player.IsPlacingServer() )
			return false;
		
		//Action not allowed if player has broken legs
		if (player.GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS)
			return false;
		
		//hack - gate
		//WTF
		if (target.GetObject() && (!target.GetObject().CanUseConstructionBuild() || target.GetObject().CanUseHandConstruction()))
			return false;
		
		if ( (!GetGame().IsDedicatedServer()) )
		{
			if ( MiscGameplayFunctions.ComplexBuildCollideCheckClient(player, target, item, m_VariantID ) )
			{
				return true;
			}
			return false;
		}
		return true;
	}
	
	override bool ActionConditionContinue( ActionData action_data )
	{
		BaseBuildingBase base_building = BaseBuildingBase.Cast( action_data.m_Target.GetObject() );
		Construction construction = base_building.GetConstruction();
		string part_name = BuildPartActionData.Cast(action_data).m_PartType;
		CollisionCheckData check_data = new CollisionCheckData;
		
		check_data.m_PartName = part_name;
		check_data.m_AdditionalExcludes.Insert(action_data.m_Player);
		
		return !construction.IsCollidingEx( check_data ) && construction.CanBuildPart( part_name, action_data.m_MainItem, true );
	}
	
	override void OnFinishProgressServer( ActionData action_data )
	{
		BaseBuildingBase base_building = BaseBuildingBase.Cast( action_data.m_Target.GetObject() );
		Construction construction = base_building.GetConstruction();
		
		string part_name = BuildPartActionData.Cast(action_data).m_PartType;
		CollisionCheckData check_data = new CollisionCheckData;
		
		check_data.m_PartName = part_name;
		check_data.m_AdditionalExcludes.Insert(action_data.m_Player);
		
		if ( !construction.IsCollidingEx( check_data ) && construction.CanBuildPart( part_name, action_data.m_MainItem, true ) ) //redundant at this point?
		{
			//build
			construction.BuildPartServer( action_data.m_Player, part_name, AT_BUILD_PART );
			//add damage to tool
			action_data.m_MainItem.DecreaseHealth( UADamageApplied.BUILD, false );
			action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
		}
	}
	
	override ActionData CreateActionData()
	{
		BuildPartActionData action_data = new BuildPartActionData;
		return action_data;
	}
	
	//setup
	override bool SetupAction( PlayerBase player, ActionTarget target, ItemBase item, out ActionData action_data, Param extra_data = NULL )
	{	
		if ( super.SetupAction( player, target, item, action_data, extra_data ) )
		{
			SetBuildingAnimation( item );
			
			if ( !GetGame().IsDedicatedServer() )
			{
				ConstructionActionData construction_action_data = action_data.m_Player.GetConstructionActionData();
				BuildPartActionData.Cast(action_data).m_PartType = construction_action_data.GetBuildPartAtIndex(m_VariantID).GetPartName();
			}
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
			case SledgeHammer:
				m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_MINEROCK;
				break;
			default:
				m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_ASSEMBLE;
				break;
		}
	}
	
	override void WriteToContext(ParamsWriteContext ctx, ActionData action_data)
	{
		super.WriteToContext(ctx, action_data);
		
		ctx.Write(BuildPartActionData.Cast(action_data).m_PartType);
	}
	
	override bool ReadFromContext(ParamsReadContext ctx, out ActionReciveData action_recive_data )
	{
		action_recive_data = new BuildPartActionReciveData;
		super.ReadFromContext(ctx, action_recive_data);
		
		string part_type;
		if ( ctx.Read(part_type) )
		{
			BuildPartActionReciveData.Cast( action_recive_data ).m_PartType = part_type;
			return true;
		}
		else
		{
			return false;
		}
	}
	
	override void HandleReciveData(ActionReciveData action_recive_data, ActionData action_data)
	{
		super.HandleReciveData(action_recive_data, action_data);
		
		BuildPartActionData.Cast(action_data).m_PartType = BuildPartActionReciveData.Cast( action_recive_data ).m_PartType;
	}
	
	override string GetAdminLogMessage(ActionData action_data)
	{
		ConstructionActionData construction_action_data = action_data.m_Player.GetConstructionActionData();
		string partName = BuildPartActionData.Cast(action_data).m_PartType;
		
		string message = string.Format("Built %1 on %2 with %3", partName, action_data.m_Target.GetObject().GetDisplayName(), action_data.m_MainItem.GetDisplayName() );
		return message;
	}
}

class ActionActionBuildPartNoTool: ActionBuildPart
{
	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}
	
	override bool UseMainItem()
	{
		return false;
	}
	
	override bool HasProgress()
	{
		return true;
	}
	
	override bool HasAlternativeInterrupt()
	{
		return false;
	}
}