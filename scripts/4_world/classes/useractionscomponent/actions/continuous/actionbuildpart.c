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
		m_ActionData.m_ActionComponent = new CAContinuousTime(time);
	}
	
	float SetCallbackDuration(ItemBase item)
	{
		#ifdef DIAG_DEVELOPER
		if (m_ActionData.m_Player.GetConstructionActionData().GetFastConstructionMode() == EConstructionDebugModes.FAST)
			return 1;
		#endif
		
		if (item) //TODO: configure and evaluate item suitability?
			return UATimeSpent.BASEBUILDING_CONSTRUCT_MEDIUM;
		else
			return UATimeSpent.BASEBUILDING_CONSTRUCT_SLOW;
	}
};

class ActionBuildPart: ActionContinuousBase
{
	void ActionBuildPart()
	{
		m_CallbackClass = ActionBuildPartCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_ASSEMBLE;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_HIGH;
		m_LockTargetOnUse 	= false;
		m_Text = "#build";
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTCursor;
	}
	
	override void OnActionInfoUpdate(PlayerBase player, ActionTarget target, ItemBase item)
	{
		ConstructionActionData cad = player.GetConstructionActionData();
		ConstructionPart part = cad.GetBuildPartAtIndex(m_VariantID);
		if (part)
		{
			string overrideText = part.GetBuildTextOverride();
			if (overrideText == string.Empty)
				m_Text = "#build "+ part.GetName();
			else
				m_Text = overrideText;
			
			#ifdef DIAG_DEVELOPER
			if (DiagMenu.GetBool(DiagMenuIDs.BASEBUILDING_FULLDEBUG))
			{
				m_Text = "dbgBuild | part classname: " + part.GetPartName();
			}
			#endif
		}
	}
	
	override void InitInfoData()
	{
		m_InfoData = new ActionInfoDataConstructionPossible();
	}
	
	override bool CanBeUsedLeaning()
	{
		return false;
	}
	
	override bool CanBeUsedInFreelook()
	{
		return false;
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		#ifdef DIAG_DEVELOPER
		if (player.GetConstructionActionData().GetFastConstructionMode() == EConstructionDebugModes.INSTANT)
			return false;
		#endif
		
		EntityAI ent;
		if (!Class.CastTo(ent,target.GetObject()))
			return false;
		
		if (player.IsPlacingLocal() || player.IsPlacingServer())
			return false;
		
		//Action not allowed if player has broken legs
		if (player.GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS)
			return false;
		
		if (!ent.CanUseConstruction() || !ent.CanUseConstructionBuild())
			return false;
		
		if ((!GetGame().IsDedicatedServer()))
		{
			if (!PositioningCheck(player, target))
				return false;
			
			if (MiscGameplayFunctions.ComplexBuildCollideCheckClient(player, target, item, m_VariantID))
				return GatherInfoData(player,target,item);
			
			return false;
		}
		return true;
	}
	
	override bool ActionConditionContinue(ActionData action_data)
	{
		EntityAI ent = EntityAI.Cast(action_data.m_Target.GetObject());
		ConstructionBase construction = ConstructionBase.Cast(ent.GetConstructionBasic());
		string part_name = BuildPartActionData.Cast(action_data).m_PartType;
		CollisionCheckData check_data = new CollisionCheckData;
		
		check_data.m_PartName = part_name;
		check_data.m_AdditionalExcludes.Insert(action_data.m_Player);
		
		bool canBuild = construction.CanBuildPart(part_name, action_data.m_MainItem, true);
		#ifdef DIAG_DEVELOPER
		if (action_data.m_Player.GetConstructionActionData().GetFastConstructionMode())
			canBuild = true;
		#endif
		if (GetGame().IsServer())
		{
			bool collides = construction.IsCollidingEx(check_data);
			
			return !collides && canBuild;
		}
		else
		{
			return canBuild;
		}
	}
	
	override void OnFinishProgressServer(ActionData action_data)
	{
		EntityAI ent = EntityAI.Cast(action_data.m_Target.GetObject());
		ConstructionBase construction = ConstructionBase.Cast(ent.GetConstructionBasic());
		
		string partName = BuildPartActionData.Cast(action_data).m_PartType;
		ConstructionPart part = construction.GetConstructionPart(partName);
		CollisionCheckData checkData = new CollisionCheckData;
		
		checkData.m_PartName = partName;
		checkData.m_AdditionalExcludes.Insert(action_data.m_Player);
		
		bool canBuild = construction.CanBuildPart(partName, action_data.m_MainItem, UseMainItem()); //TODO: do we need to check it here when ActionVariantManager already does it?
		#ifdef DIAG_DEVELOPER
		if (action_data.m_Player.GetConstructionActionData().GetFastConstructionMode())
			canBuild = true;
		#endif
		if (!construction.IsCollidingEx(checkData) && canBuild)
		{
			//build
			construction.BuildPartServerEx(action_data.m_Player, partName, AT_BUILD_PART);
			
			#ifdef DIAG_DEVELOPER
			//skip tool/hand damage on debug action
			if (action_data.m_Player.GetConstructionActionData().GetFastConstructionMode())
				return;
			#endif
			
			//add damage to tool or hands
			if (action_data.m_MainItem)
			{
				if (part.DamageToolOnBuild())
					action_data.m_MainItem.DecreaseHealth(UADamageApplied.BUILD, false);
			}
			else
			{
				if (part.DamageHandsOnBuild())
				{
					EntityAI gloves = action_data.m_Player.GetInventory().FindAttachment(InventorySlots.GLOVES);
					if (gloves && gloves.GetHealthLevel() < 4)
					{
						gloves.AddHealth("","", PlayerConstants.GLOVES_DAMAGE_HAND_CONSTRUCTION);
					}
					else if (Math.RandomFloat01() < PlayerConstants.CHANCE_TO_BLEED_HAND_CONSTRUCTION)
					{
						if (Math.RandomFloat01() < 0.5)
							action_data.m_Player.GetBleedingManagerServer().AttemptAddBleedingSourceBySelection("LeftForeArmRoll");
						else
							action_data.m_Player.GetBleedingManagerServer().AttemptAddBleedingSourceBySelection("RightForeArmRoll");
						action_data.m_Player.RequestSoundEventEx(EPlayerSoundEventID.INJURED_LIGHT); //play sound regardless?
					}
				}
			}
		}
	}
	
	override string GetSoundCategory(ActionData action_data)
    {
        return "Base_building";
    }
	
	override ActionData CreateActionData()
	{
		BuildPartActionData action_data = new BuildPartActionData;
		return action_data;
	}
	
	//setup
	override bool SetupAction(PlayerBase player, ActionTarget target, ItemBase item, out ActionData action_data, Param extra_data = NULL)
	{	
		if (super.SetupAction(player, target, item, action_data, extra_data))
		{
			if (!g_Game.IsDedicatedServer())
			{
				ComposeBuildActionDataClient(player,target,item,action_data,extra_data);
			}
			
			SetBuildingAnimationEx(player, target, item, action_data, extra_data);
			return true;
		}
		
		return false;
	}
	
	protected void ComposeBuildActionDataClient(PlayerBase player, ActionTarget target, ItemBase item, out ActionData action_data, Param extra_data = NULL)
	{
		ConstructionActionData cad = action_data.m_Player.GetConstructionActionData();
		ConstructionPart part = cad.GetBuildPartAtIndex(m_VariantID);
		BuildPartActionData.Cast(action_data).m_PartType = part.GetPartName();
	}
	
	protected void SetBuildingAnimationEx(PlayerBase player, ActionTarget target, ItemBase item, ActionData action_data, Param extra_data = NULL)
	{
		if (!item)
			return;
		
		switch (item.Type())
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
			case BrickTrowel:
				m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_BRICKTROWEL;
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
	
	override bool ReadFromContext(ParamsReadContext ctx, out ActionReciveData action_recive_data)
	{
		action_recive_data = new BuildPartActionReciveData;
		super.ReadFromContext(ctx, action_recive_data);
		
		string part_type;
		if (ctx.Read(part_type))
		{
			BuildPartActionReciveData.Cast(action_recive_data).m_PartType = part_type;
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
		
		BuildPartActionData.Cast(action_data).m_PartType = BuildPartActionReciveData.Cast(action_recive_data).m_PartType;
	}
	
	override string GetAdminLogMessage(ActionData action_data)
	{
		string partName = BuildPartActionData.Cast(action_data).m_PartType;
		
		string itemName = "hands";
		if (action_data.m_MainItem)
			itemName = action_data.m_MainItem.GetDisplayName();
		
		string message = string.Format("Built %1 on %2 with %3", partName, action_data.m_Target.GetObject().GetDisplayName(), itemName);
		return message;
	}
	
	protected void ClearInfoData()
	{
		m_InfoData.m_InfoContextMask = 0;
	}
	
	protected bool GatherInfoData(PlayerBase player, ActionTarget target, ItemBase item)
	{
		ClearInfoData();
		
		ActionInfoDataConstructionPossible info = ActionInfoDataConstructionPossible.Cast(m_InfoData);
		ConstructionActionData cad = player.GetConstructionActionData();
		ConstructionPart part = cad.GetBuildPartAtIndex(m_VariantID);
		info.SetConstructionPart(part);
		info.SetTool(item);
		
		info.m_InfoContextMask |= EConstructionInfoCategories.MATERIALS_PRESENCE; //material info should always be visible
		info.m_InfoContextMask |= EConstructionInfoCategories.TOOL_PRESENCE; //tools info should always be visible
		
		return info.CanDisplayInfo();
	}
	
	//! Can build from the inside only, if applicable
	protected bool PositioningCheck(PlayerBase player, ActionTarget target)
	{
		EntityAI ent = EntityAI.Cast(target.GetObject());
		BaseBuildingBase bbb = BaseBuildingBase.Cast(ent);
		ConstructionActionData cad = player.GetConstructionActionData();
		ConstructionPart part = GetTargetPart(cad);
		
		if (part)
		{
			ConstructionBase construction = ConstructionBase.Cast(ent.GetConstructionBasic());
			if (!ent.IsPlayerInsideEx(player, part.GetPartName()))
			{
				if (bbb && !bbb.MustBeBuiltFromOutside())
					return false;
				else if (!bbb)
					return false;
			}
			if (!construction.ConstructionDirectionCheck(player, part, EConstructionDirCheckType.INDOORS))
				return false;
			if (!construction.ConstructionInsiderBoxCheck(player, part))
				return false;
			
			return true;
		}
		return false;
	}
	
	//! Does not work on server, since variants are not updated there (invoker does not invoke)
	protected ConstructionPart GetTargetPart(ConstructionActionData cad)
	{
		return cad.GetBuildPartAtIndex(m_VariantID);
	}
	
	//////////////
	//deprecated//
	//////////////
	[Obsolete("Use 'SetBuildingAnimationEx' instead")]
	protected void SetBuildingAnimation(ItemBase item)
	{
		SetBuildingAnimationEx(null,null,item,null);
	}
}

class ActionBuildPartHandsGeneric: ActionBuildPart
{
	void ActionBuildPartHandsGeneric()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_DEPLOY_2HD;
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTCursor;
	}
	
	override bool UseMainItem()
	{
		return false;
	}
	
	override void OnActionInfoUpdate(PlayerBase player, ActionTarget target, ItemBase item)
	{
		ConstructionActionData cad = player.GetConstructionActionData();
		ConstructionPart part = cad.GetBuildPartHandsGenericAtIndex(m_VariantID);
		if (part)
		{
			string overrideText = part.GetBuildTextOverride();
			if (overrideText == string.Empty)
				m_Text = "#build "+ part.GetName();
			else
				m_Text = overrideText;
			
			#ifdef DIAG_DEVELOPER
			if (DiagMenu.GetBool(DiagMenuIDs.BASEBUILDING_FULLDEBUG))
			{
				m_Text = "dbgBuild | part classname: " + part.GetPartName() + " with hands";
			}
			#endif
		}
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		#ifdef DIAG_DEVELOPER
		if (player.GetConstructionActionData().GetFastConstructionMode() == EConstructionDebugModes.INSTANT)
			return false;
		#endif
		
		if (item)
			return false;
		
		EntityAI ent;
		if (!Class.CastTo(ent,target.GetObject()))
			return false;
		
		if (player.IsPlacingLocal() || player.IsPlacingServer())
			return false;
		
		//Action not allowed if player has broken legs
		if (player.GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS)
			return false;
		
		bool handConstructionCondition = ent.CanUseHandConstruction();
		#ifdef DIAG_DEVELOPER
		if (player.GetConstructionActionData().GetFastConstructionMode())
			handConstructionCondition = true;
		#endif
		
		if (!ent.CanUseConstruction() || !ent.CanUseConstructionBuild() || !handConstructionCondition)
			return false;
		
		if ((!GetGame().IsDedicatedServer()))
		{
			if (!PositioningCheck(player, target))
				return false;
			
			if (MiscGameplayFunctions.ComplexBuildCollideCheckClient(player, target, item, m_VariantID))
				return GatherInfoData(player,target,item);
			
			return false;
		}
		return true;
	}
	
	override bool ActionConditionContinue(ActionData action_data)
	{
		EntityAI ent = EntityAI.Cast(action_data.m_Target.GetObject());
		ConstructionBase construction = ConstructionBase.Cast(ent.GetConstructionBasic());
		string part_name = BuildPartActionData.Cast(action_data).m_PartType;
		CollisionCheckData check_data = new CollisionCheckData;
		
		check_data.m_PartName = part_name;
		check_data.m_AdditionalExcludes.Insert(action_data.m_Player);
		
		bool canBuild = construction.CanBuildPart(part_name, null, false);
		#ifdef DIAG_DEVELOPER
		if (action_data.m_Player.GetConstructionActionData().GetFastConstructionMode())
			canBuild = true;
		#endif
		if (GetGame().IsServer())
		{
			bool collides = construction.IsCollidingEx(check_data);
			
			return !collides && canBuild;
		}
		else
		{
			return canBuild;
		}
	}
	
	override protected bool GatherInfoData(PlayerBase player, ActionTarget target, ItemBase item)
	{
		ClearInfoData();
		
		ActionInfoDataConstructionPossible info = ActionInfoDataConstructionPossible.Cast(m_InfoData);
		ConstructionActionData cad = player.GetConstructionActionData();
		ConstructionPart part = cad.GetBuildPartHandsGenericAtIndex(m_VariantID);
		info.SetConstructionPart(part);
		info.SetTool(item);
		
		info.m_InfoContextMask |= EConstructionInfoCategories.MATERIALS_PRESENCE; //material info should always be visible
		/*
		int actionMask = part.GetBuildActionTypeMask();
		actionMask &= ~EConstructionTools.TOOL_HANDS;
		if (actionMask != 0)
			info.m_InfoContextMask |= EConstructionInfoCategories.TOOL_PRESENCE; //tools info ONLY when also buildable by one!
		*/
		info.m_InfoContextMask |= EConstructionInfoCategories.TOOL_PRESENCE; //tools info should always be visible
		
		return info.CanDisplayInfo();
	}
	
	override protected void ComposeBuildActionDataClient(PlayerBase player, ActionTarget target, ItemBase item, out ActionData action_data, Param extra_data = NULL)
	{
		ConstructionActionData cad = action_data.m_Player.GetConstructionActionData();
		BuildPartActionData.Cast(action_data).m_PartType = cad.GetBuildPartHandsGenericAtIndex(m_VariantID).GetPartName();
	}
	
	//! Does not work on server, since variants are not updated there (invoker does not invoke)
	override protected ConstructionPart GetTargetPart(ConstructionActionData cad)
	{
		return cad.GetBuildPartHandsGenericAtIndex(m_VariantID);
	}
	
	//! here we base anims on targeted part
	override protected void SetBuildingAnimationEx(PlayerBase player, ActionTarget target, ItemBase item, ActionData action_data, Param extra_data = NULL)
	{
		if (item)
			return;
		
		BuildPartActionData buildData;
		if (Class.CastTo(buildData, action_data))
		{
			EntityAI ent = EntityAI.Cast(action_data.m_Target.GetObject());
			ConstructionBase construction = ConstructionBase.Cast(ent.GetConstructionBasic());
			ConstructionPart part = construction.GetConstructionPart(buildData.m_PartType);
			int animOverrideID = part.GetHandBuildFBAnimOverride();
			
			if (animOverrideID >= 0)
				m_CommandUID = animOverrideID;
			else
				m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_DEPLOY_2HD;
		}
	}
}

// Deprecated
class ActionActionBuildPartNoTool: ActionBuildPartHandsGeneric {}