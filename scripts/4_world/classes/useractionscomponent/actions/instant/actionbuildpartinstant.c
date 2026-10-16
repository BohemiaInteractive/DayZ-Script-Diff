
#ifdef DIAG_DEVELOPER
class ActionBuildPartInstantDebug : ActionInstantBase
{	
	void ActionBuildPartInstantDebug()
	{
		m_Text = "build part debug";
	}
	
	override bool HasTarget()
	{
		return true;
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINone();
		m_ConditionTarget = new CCTCursor();
	}
	
	override void OnActionInfoUpdate(PlayerBase player, ActionTarget target, ItemBase item)
	{
		ConstructionActionData cad = player.GetConstructionActionData();
		ConstructionPart part = cad.GetBuildPartInstantAtIndex(m_VariantID);
		if (part)
		{
			m_Text = "dbgBuild "+ part.GetName() + " - INSTANT";
			
			#ifdef DIAG_DEVELOPER
			if (DiagMenu.GetBool(DiagMenuIDs.BASEBUILDING_FULLDEBUG))
			{
				m_Text = "dbgBuild | part classname: " + part.GetPartName() + " - INSTANT";
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
		ConstructionActionData cad = player.GetConstructionActionData();
		if (cad.GetFastConstructionMode() != EConstructionDebugModes.INSTANT)
			return false;
		
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
	
	override void OnStartServer(ActionData action_data)
	{
		EntityAI ent = EntityAI.Cast(action_data.m_Target.GetObject());
		ConstructionBase construction = ConstructionBase.Cast(ent.GetConstructionBasic());
		
		string partName = BuildPartActionData.Cast(action_data).m_PartType;
		ConstructionPart part = construction.GetConstructionPart(partName);
		CollisionCheckData checkData = new CollisionCheckData();
		
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
		}
	}
	
	override ActionData CreateActionData()
	{
		BuildPartActionData action_data = new BuildPartActionData();
		return action_data;
	}
	
	override bool SetupAction(PlayerBase player, ActionTarget target, ItemBase item, out ActionData action_data, Param extra_data = NULL)
	{	
		if (super.SetupAction(player, target, item, action_data, extra_data))
		{
			if (!g_Game.IsDedicatedServer())
				ComposeBuildActionDataClient(player,target,item,action_data,extra_data);
			
			return true;
		}
		
		return false;
	}
	
	protected void ComposeBuildActionDataClient(PlayerBase player, ActionTarget target, ItemBase item, out ActionData action_data, Param extra_data = NULL)
	{
		ConstructionActionData cad = action_data.m_Player.GetConstructionActionData();
		ConstructionPart part = cad.GetBuildPartInstantAtIndex(m_VariantID);
		BuildPartActionData.Cast(action_data).m_PartType = part.GetPartName();
	}
	
	override void WriteToContext(ParamsWriteContext ctx, ActionData action_data)
	{
		super.WriteToContext(ctx, action_data);
		
		ctx.Write(BuildPartActionData.Cast(action_data).m_PartType);
	}
	
	override bool ReadFromContext(ParamsReadContext ctx, out ActionReciveData action_recive_data)
	{
		action_recive_data = new BuildPartActionReciveData();
		super.ReadFromContext(ctx, action_recive_data);
		
		string partType;
		if (ctx.Read(partType))
		{
			BuildPartActionReciveData.Cast(action_recive_data).m_PartType = partType;
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
	
	protected void ClearInfoData()
	{
		m_InfoData.m_InfoContextMask = 0;
	}
	
	protected bool GatherInfoData(PlayerBase player, ActionTarget target, ItemBase item)
	{
		ClearInfoData();
		
		ActionInfoDataConstructionPossible info = ActionInfoDataConstructionPossible.Cast(m_InfoData);
		ConstructionActionData cad = player.GetConstructionActionData();
		ConstructionPart part = cad.GetBuildPartInstantAtIndex(m_VariantID);
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
		return cad.GetBuildPartInstantAtIndex(m_VariantID);
	}
};
#endif