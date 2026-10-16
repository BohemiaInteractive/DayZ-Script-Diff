class ActionConstructionPartInfoContinuous: ActionBuildPart
{
	void ActionConstructionPartInfoContinuous()
	{
	}
	
	override void InitInfoData()
	{
		m_InfoData = new ActionInfoDataConstructionImpossible();
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINone();
		m_ConditionTarget = new CCTCursor();
	}
	
	override bool CanBeStarted()
	{
		return false;
	}
	
	override bool IsLocal()
	{
		return true;
	}
	
	override void OnActionInfoUpdate(PlayerBase player, ActionTarget target, ItemBase item)
	{
		ConstructionActionData cad = player.GetConstructionActionData();
		ConstructionPart part = cad.GetBuildPartInfoAtIndex(m_VariantID);
		if (part)
		{
			string overrideText = part.GetBuildTextOverride();
			if (overrideText == string.Empty)
				m_Text = "#build" + " " + part.GetName();
			else
				m_Text = overrideText;
			
			#ifdef DIAG_DEVELOPER
			if (DiagMenu.GetBool(DiagMenuIDs.BASEBUILDING_FULLDEBUG))
			{
				m_Text = "dbg " + "#build" + " " + part.GetName() + " | " + part.GetPartName();
			}
			#endif
		}
		
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
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
			if (SimplifiedCollisionInfoCheck(player,target,item))
			{
				return GatherInfoData(player,target,item);
			}
		}
		
		return false;
	}
	
	override bool ActionConditionContinue(ActionData action_data)
	{
		return false;
	}
	
	override protected bool GatherInfoData(PlayerBase player, ActionTarget target, ItemBase item)
	{
		ClearInfoData();
		
		EntityAI ent;
		if (Class.CastTo(ent,target.GetObject()))
		{
			ActionInfoDataConstructionImpossible info = ActionInfoDataConstructionImpossible.Cast(m_InfoData);
			ConstructionBase construction = ConstructionBase.Cast(ent.GetConstructionBasic());
			ConstructionActionData cad = player.GetConstructionActionData();
			ConstructionPart part = cad.GetBuildPartInfoAtIndex(m_VariantID);
			construction.CanBuildPartEx(part.GetPartName(), item, info.m_InfoContextMask);
			info.SetConstructionPart(part);
			info.SetTool(item);
			
			#ifdef DIAG_DEVELOPER
			if (DiagMenu.GetBool(DiagMenuIDs.BASEBUILDING_FULLDEBUG) && info.m_InfoContextMask)
				DbgPrint(construction, info, target.GetComponentIndex()); //print here for 'real' mask data..
			#endif
			
			info.m_InfoContextMask |= EConstructionInfoCategories.MATERIALS_PRESENCE; //material info should always be visible
			info.m_InfoContextMask |= EConstructionInfoCategories.TOOL_PRESENCE; //tools info should always be visible
			
			return info.CanDisplayInfo();
		}
		
		return false;
	}
	
	#ifdef DIAG_DEVELOPER
	void DbgPrint(notnull ConstructionBase construction, ActionInfoDataConstructionBase info, int componentIdx)
	{
		int failureMask = info.m_InfoContextMask;
		ConstructionPart part = info.GetConstructionPart();
		string partName = part.GetPartName();
		
		if (failureMask == 0)
		{
			ConstructionDebug.RemoveBuildPartInfoData(partName);
		}
		else
		{
			string conflictingPart;
			if (failureMask & EConstructionInfoCategories.PART_CONFLICTED)
				construction.HasConflictPart(partName, conflictingPart);
	
			string missingSlot;
			float missingQty;
			if (failureMask & EConstructionInfoCategories.MATERIALS_PRESENCE)
				construction.HasMaterialsEx(partName, false, missingSlot, missingQty);
	
			ItemBase tool = info.GetTool();
			string badToolType = "hands";
			if ((failureMask & EConstructionInfoCategories.TOOL_PRESENCE) && tool)
				badToolType = tool.GetType();
	
			ActionInfoDebugData data = new ActionInfoDebugData();
			data.partName = partName;
			data.componentIdx = componentIdx;
			data.failureMask = failureMask;
			data.conflictingPart = conflictingPart;
			data.missingSlot = missingSlot;
			data.missingQty = missingQty;
			data.badToolType = badToolType;
			
			ConstructionDebug.AddBuildPartInfoData(data);
		}
	}
	#endif
	
	//! 'ComplexBuildCollideCheckClient' not suitable for this, due to missing context (looks into wrong varinats array); returns 'true' if collision does not block action
	protected bool SimplifiedCollisionInfoCheck(PlayerBase player, ActionTarget target, ItemBase item)
	{
		EntityAI ent = EntityAI.Cast(target.GetObject());
		ConstructionActionData constructionActionData = player.GetConstructionActionData();
		constructionActionData.SetTarget(ent);
		ConstructionPart part = constructionActionData.GetBuildPartInfoAtIndex(m_VariantID);
		ConstructionBase construction = ConstructionBase.Cast(ent.GetConstructionBasic());
		
		if (!part)
		{
			ErrorEx("no part available!");
			constructionActionData.GetBuildPartInfoAtIndex(m_VariantID);
		}
		else
		{
			BaseBuildingBase baseBuilding = BaseBuildingBase.Cast(ent);
			if (baseBuilding && baseBuilding.MustBeBuiltFromOutside() && baseBuilding.IsPlayerInsideEx(player, part.GetMainPartName()))
				return false;
			
			if (!construction.ConstructionDirectionCheck(player, part, EConstructionDirCheckType.INDOORS))
				return false;
			if (!construction.ConstructionInsiderBoxCheck(player, part))
				return false;
			if (player.GetInputController().CameraIsFreeLook())
				return false;
			
			if (baseBuilding)
			{
				bool boo;
				if (baseBuilding.PerformRoofCheckForBase(part.GetPartName(),player,boo) && boo)
					return false;
			}
			
			if (player.IsPlacingLocal() || player.IsPlacingServer())
				return false;
			
			CollisionCheckData checkData = new CollisionCheckData();
			checkData.m_PartName = part.GetPartName();
			
			return !construction.IsCollidingEx(checkData);
		}
		
		return false;
	}
}

// just a duplicate to allow displaying in interact category of target widget
class ActionConstructionPartInfoInteract: ActionConstructionPartInfoContinuous
{
	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}
}
///////////////////////////

class ActionInfoDataBase
{
	int m_InfoContextMask;
	
	void ActionInfoDataBase()
	{
		m_InfoContextMask = 0;
	}
	
	bool CanDisplayInfo()
	{
		return true;
	}
}

//! not intended for direct use
class ActionInfoDataConstructionBase: ActionInfoDataBase
{
	protected ItemBase m_Tool;
	protected ConstructionPart m_Part;
	
	void SetConstructionPart(ConstructionPart part)
	{
		m_Part = part;
	}
	
	ConstructionPart GetConstructionPart()
	{
		return m_Part;
	}
	
	void SetTool(ItemBase tool)
	{
		m_Tool = tool;
	}
	
	ItemBase GetTool()
	{
		return m_Tool;
	}
}

//! always visible
class ActionInfoDataConstructionPossible: ActionInfoDataConstructionBase
{
}

//! visible only under specific conditions
class ActionInfoDataConstructionImpossible: ActionInfoDataConstructionBase
{
	override bool CanDisplayInfo()
	{
		bool forceHide = false;
		forceHide |= m_InfoContextMask & EConstructionInfoCategories.PART_CONSTRUCTED;
		forceHide |= m_InfoContextMask & EConstructionInfoCategories.PART_CONFLICTED;
		forceHide |= m_InfoContextMask & EConstructionInfoCategories.PART_MISSING_REQUIREMENT;
		forceHide |= m_InfoContextMask & EConstructionInfoCategories.PART_MISSING_SUPPORT;
		bool show = true;
		//show |= m_InfoContextMask & EConstructionInfoCategories.TOOL_PRESENCE;
		//show |= m_InfoContextMask & EConstructionInfoCategories.MATERIALS_PRESENCE;
		
		return super.CanDisplayInfo() && !forceHide && show;
	}
}
