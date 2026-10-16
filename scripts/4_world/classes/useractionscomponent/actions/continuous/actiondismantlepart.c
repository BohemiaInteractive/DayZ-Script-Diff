class ActionDismantlePartCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		float time = SetCallbackDuration(m_ActionData.m_MainItem);
		m_ActionData.m_ActionComponent = new CAContinuousTime(time);
	}
	
	float SetCallbackDuration( ItemBase item )
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

class ActionDismantlePart: ActionContinuousBase
{
	void ActionDismantlePart()
	{
		m_CallbackClass = ActionDismantlePartCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_DISASSEMBLE;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT;	
		
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_HIGH;
		m_LockTargetOnUse 	= false;
		m_Text = "#dismantle";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTCursor;
	}
	
	override void OnActionInfoUpdate( PlayerBase player, ActionTarget target, ItemBase item )
	{
		ConstructionActionData cad = player.GetConstructionActionData();
		ConstructionPart part = cad.GetTargetPart();
		if (part)
		{
			string overrideText = part.GetDismantleTextOverride();
			if (overrideText == string.Empty)
				m_Text = "#dismantle "+ part.GetName();
			else
				m_Text = overrideText;
			
			#ifdef DIAG_DEVELOPER
			if (DiagMenu.GetBool(DiagMenuIDs.BASEBUILDING_FULLDEBUG))
			{
				m_Text = "dbgDismantle | part classname: " + part.GetPartName();
			}
			#endif
		}
	}
	
	override bool CanBeUsedLeaning()
	{
		return false;
	}
	
	override bool CanBeUsedInFreelook()
	{
		return false;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		#ifdef DIAG_DEVELOPER
		if (player.GetConstructionActionData().GetFastConstructionMode() == EConstructionDebugModes.INSTANT)
			return false;
		#endif
		
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
		EntityAI ent = EntityAI.Cast(action_data.m_Target.GetObject());
		ConstructionBase construction = ConstructionBase.Cast(ent.GetConstructionBasic());
		
		ConstructionActionData construction_action_data = action_data.m_Player.GetConstructionActionData();
		ConstructionPart construction_part = construction_action_data.GetTargetPart();
		
		if ( construction.CanDismantlePart( construction_part.GetPartName(), action_data.m_MainItem ) )
		{
			//build
			construction.DismantlePartServerEx( action_data.m_Player, construction_part.GetPartName(), AT_DISMANTLE_PART );
			
			//add damage to tool
			if (action_data.m_MainItem)
				action_data.m_MainItem.DecreaseHealth(UADamageApplied.DISMANTLE, false);
		}
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
	
	protected bool DismantleCondition(PlayerBase player, ActionTarget target, ItemBase item, bool camera_check)
	{	
		if (player && !player.IsPlacingLocal() && !player.IsPlacingServer())
		{
			EntityAI targetEntity;
			if (Class.CastTo(targetEntity,target.GetObject()) && targetEntity.CanUseConstruction())
			{
				if (targetEntity.FindAttachmentBySlotName("Material_FPole_Flag"))
					return false;
				
				BaseBuildingBase baseBuilding = BaseBuildingBase.Cast(targetEntity);
				ConstructionBase construction = ConstructionBase.Cast(targetEntity.GetConstructionBasic());
				string partName;
				ConstructionPart constructionPart;
				
				array<string> compNames = new array<string>();
				targetEntity.GetActionComponentNameList(target.GetComponentIndex(), compNames);
				int count = compNames.Count();
				for (int i = 0; i < count; ++i)
				{
					partName = compNames[i];
					constructionPart = construction.GetConstructionPartToDismantle(partName, item);
					if (constructionPart)
						break;
				}
				
				if (constructionPart)
				{
					//invalid on gate if the gate is opened
					if (baseBuilding && constructionPart.IsGate() && baseBuilding.IsOpened())
						return false;
					
					if (constructionPart.HasEssentialAttachmentSlots())
					{
						array<string> slots = constructionPart.GetEssentialAttachmentSlots();
						foreach(string slotName: slots)
						{
							EntityAI attachment = targetEntity.FindAttachmentBySlotName(slotName);
							if (attachment)
							{
								#ifdef DIAG_DEVELOPER
								if (DiagMenu.GetBool(DiagMenuIDs.BASEBUILDING_FULLDEBUG))
								{
									DbgPrintFailure(constructionPart, "essential attachment " + attachment.GetType() + " attached in slot: " + slotName);
								}
								#endif
								return false;
							}
						}
					}
					
					//camera and position checks
					bool checked = false;
					
					if (constructionPart.IsBase())
						checked = true;
					
					if (!checked && targetEntity.IsPlayerInsideEx(player, partName))
					{
						//Camera check (client-only)
						if (camera_check)
						{
							if (g_Game && (!g_Game.IsDedicatedServer()))
							{
								if (targetEntity.IsFacingCamera(partName))
								{
									#ifdef DIAG_DEVELOPER
									if (DiagMenu.GetBool(DiagMenuIDs.BASEBUILDING_FULLDEBUG))
									{
										DbgPrintFailure(constructionPart, "!IsFacingCamera");
									}
									#endif
									return false;
								}
							}
						}
						
						checked = true;
					}
					
					//directional checks first
					if (!construction.ConstructionDirectionCheck(player, constructionPart, EConstructionDirCheckType.INDOORS))
					{
						#ifdef DIAG_DEVELOPER
						if (DiagMenu.GetBool(DiagMenuIDs.BASEBUILDING_FULLDEBUG))
						{
							DbgPrintFailure(constructionPart, "ConstructionDirectionCheck failing, type: EConstructionDirCheckType.INDOORS");
						}
						#endif
						return false;
					}
					
					//Also checks for the reversible parts
					if (constructionPart.IsReversible() && (constructionPart.IsReversedLocal() != construction.ConstructionDirectionCheck(player, constructionPart, EConstructionDirCheckType.REFERENCE_DIRECTION)))
					{
						#ifdef DIAG_DEVELOPER
						if (DiagMenu.GetBool(DiagMenuIDs.BASEBUILDING_FULLDEBUG))
						{
							DbgPrintFailure(constructionPart, "ConstructionDirectionCheck failing, type: EConstructionDirCheckType.REFERENCE_DIRECTION");
						}
						#endif
						return false;
					}
					
					if (!construction.ConstructionInsiderBoxCheck(player, constructionPart))
					{
						#ifdef DIAG_DEVELOPER
						if (DiagMenu.GetBool(DiagMenuIDs.BASEBUILDING_FULLDEBUG))
						{
							DbgPrintFailure(constructionPart, "ConstructionInsiderBoxCheck failing");
							//Debug.Draw
						}
						#endif
						return false;
					}
					
					if (checked)
					{
						ConstructionActionData constructionActionData = player.GetConstructionActionData();
						constructionActionData.SetTargetPart(constructionPart);
						
						return true;
					}
				}
			}
		}
		
		return false;
	}
	
	override string GetAdminLogMessage(ActionData action_data)
	{
		ConstructionActionData cad = action_data.m_Player.GetConstructionActionData();
		ConstructionPart constructionPart = cad.GetTargetPart();
		
		string itemName = "hands";
		if (action_data.m_MainItem)
			itemName = action_data.m_MainItem.GetDisplayName();
		
		string message = string.Format("Dismantled %1 from %2 with %3", constructionPart.GetName(), action_data.m_Target.GetObject().GetDisplayName(), itemName);
		return message;
	}
	
	#ifdef DIAG_DEVELOPER
	protected void DbgPrintFailure(ConstructionPart part, string reason)
	{
		ActionDismantleDebugData data = new ActionDismantleDebugData();
		data.partName = part.GetPartName();
		data.reason = reason;
		ConstructionDebug.SetActionDismantleData(data);
	}
	#endif
}