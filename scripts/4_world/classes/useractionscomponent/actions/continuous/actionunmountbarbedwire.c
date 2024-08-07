class ActionUnmountBarbedWireCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.DEFAULT_CONSTRUCT);
	}
};

class ActionUnmountBarbedWire: ActionMountBarbedWire
{
	void ActionUnmountBarbedWire()
	{
		m_CallbackClass = ActionUnmountBarbedWireCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT;
		
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_HIGH;
		m_Text = "#unmount_barbed_wire";
	}
	
	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNonRuined(UAMaxDistances.BASEBUILDING);
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{	
		BaseBuildingBase base_building = BaseBuildingBase.Cast(target.GetObject());
		if (base_building && base_building.CanUseConstruction() && base_building.CanUseConstructionBuild())
		{
			if (!base_building.IsPlayerInside(player,""))
				return false;
			
			BarbedWire barbed_wire = GetBarbedWire(target);
			return (barbed_wire && barbed_wire.IsMounted());
		}
		
		return false;
	}
	
	override void OnFinishProgressServer(ActionData action_data)
	{
		BarbedWireActionData actionDataBW = BarbedWireActionData.Cast(action_data);
		BaseBuildingBase base_building = BaseBuildingBase.Cast(actionDataBW.m_Target.GetObject());
		
		//unmount and refresh parent
		BarbedWire wire = BarbedWire.Cast(base_building.FindAttachmentBySlotName(actionDataBW.m_SlotName));
		wire.SetMountedState(false);
	}

	override string GetZoneSelection(ActionTarget target)
	{
		BaseBuildingBase base_building = BaseBuildingBase.Cast(target.GetObject());
		string selection = base_building.GetActionComponentName(target.GetComponentIndex());
		
		if (selection.Length() > 0)
		{
			int delimiter_index = selection.IndexOfFrom(0, "_mounted");
			if (delimiter_index > -1)
			{
				selection = selection.Substring(0, delimiter_index);
			}
			/*else if (!selection.Contains("_barbedwire_") && selection.Contains("_wall_"))
			{
				selection.Insert(selection.Length(),"_barbedwire_1");
			}*/
		}
		//Print(selection);
		return selection;
	}
	
	override string GetAdminLogMessage(ActionData action_data)
	{
		string message = string.Format("Player %1 Unmounted BarbedWire from %2", action_data.m_Player, action_data.m_Target.GetObject().ClassName());
		return message;
	}
}