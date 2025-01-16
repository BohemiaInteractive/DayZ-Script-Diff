class BarbedWireActionReceiveData : ActionReciveData
{
	string m_SlotName;
}

class BarbedWireActionData : ActionData
{
	string m_SlotName;
}

class ActionMountBarbedWireCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.DEFAULT_CONSTRUCT);
	}
};

class ActionMountBarbedWire: ActionContinuousBase
{
	float m_DamageAmount; //DEPRECATED
	string m_SlotName; //DEPRECATED
	
	void ActionMountBarbedWire()
	{
		m_CallbackClass = ActionMountBarbedWireCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT;
		
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_HIGH;
		m_Text = "#mount";
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
			return (barbed_wire && !barbed_wire.IsMounted() && !barbed_wire.IsRuined());
		}
		
		return false;
	}
	
	override ActionData CreateActionData()
	{
		BarbedWireActionData action_data = new BarbedWireActionData();
		return action_data;
	}
	
	override bool SetupAction(PlayerBase player, ActionTarget target, ItemBase item, out ActionData action_data, Param extra_data = null)
	{
		if (super.SetupAction(player, target, item, action_data, extra_data))
		{
			#ifndef SERVER
			BarbedWireActionData actionDataBW = BarbedWireActionData.Cast(action_data);
			actionDataBW.m_SlotName = GetZoneSelection(target);
			#endif
			
			return true;
		}
		
		return false;
	}
	
	override void WriteToContext(ParamsWriteContext ctx, ActionData action_data)
	{
		super.WriteToContext(ctx, action_data);
		BarbedWireActionData actionDataBW
		
		if (Class.CastTo(actionDataBW,action_data))
		{
			ctx.Write(actionDataBW.m_SlotName);
		}
	}
	
	override bool ReadFromContext(ParamsReadContext ctx, out ActionReciveData action_recive_data )
	{
		if (!action_recive_data)
		{
			action_recive_data = new BarbedWireActionReceiveData;
		}
		super.ReadFromContext(ctx, action_recive_data);
		BarbedWireActionReceiveData receiveDataBW = BarbedWireActionReceiveData.Cast(action_recive_data);
		
		string slotName;
		if (!ctx.Read(slotName))
			return false;
		receiveDataBW.m_SlotName = slotName;
		
		return true;
	}
	
	override void HandleReciveData(ActionReciveData action_recive_data, ActionData action_data)
	{
		super.HandleReciveData(action_recive_data, action_data);
		
		BarbedWireActionReceiveData receiveDataBW = BarbedWireActionReceiveData.Cast(action_recive_data);
		BarbedWireActionData.Cast(action_data).m_SlotName = receiveDataBW.m_SlotName;
	}
	
	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);
		
		BarbedWire wire = GetBarbedWire(action_data.m_Target);
		if (wire && wire.GetLoopDeploySoundset() != string.Empty)
			wire.StartItemSoundServer(SoundConstants.ITEM_DEPLOY_LOOP);
	}
	
	override void OnEndServer(ActionData action_data)
	{
		BarbedWire wire = GetBarbedWire(action_data.m_Target);
		if (wire && wire.GetLoopDeploySoundset() != string.Empty)
			wire.StopItemSoundServer(SoundConstants.ITEM_DEPLOY_LOOP);
		
		super.OnEndServer(action_data);
	}
	
	override void OnFinishProgressServer(ActionData action_data)
	{	
		BarbedWireActionData actionDataBW = BarbedWireActionData.Cast(action_data);
		BaseBuildingBase base_building = BaseBuildingBase.Cast(actionDataBW.m_Target.GetObject());
		
		//mount and refresh parent
		BarbedWire wire = BarbedWire.Cast(base_building.FindAttachmentBySlotName(actionDataBW.m_SlotName));
		wire.SetMountedState(true);
		
		if (wire.GetDeploySoundset() != string.Empty)
			wire.StartItemSoundServer(SoundConstants.ITEM_DEPLOY);
		
		//solution for DamageSystem's case sensitivity
		string zone = "invalid";
		array<string> zones = new array<string>;
		zones.Copy(base_building.GetEntityDamageZoneMap().GetKeyArray());
		
		string tmp = "";
		string test = "";
		for (int i = 0; i < zones.Count(); i++)
		{
			tmp = zones.Get(i);
			test = tmp;
			test.ToLower();
			
			if (test == actionDataBW.m_SlotName)
			{
				zone = tmp;
				break;
			}
		}
		
		base_building.SetHealth01(zone,"Health",wire.GetHealth01("","Health")); //attachment slot names and damagezone names must match
	}
	
	BarbedWire GetBarbedWire(ActionTarget target)
	{
		BaseBuildingBase base_building = BaseBuildingBase.Cast(target.GetObject());
		BarbedWire wire;
		if (Class.CastTo(wire,base_building.FindAttachmentBySlotName(GetZoneSelection(target))))
		{
			return wire;
		}
		
		return null;
	}
	
	string GetZoneSelection(ActionTarget target)
	{
		BaseBuildingBase base_building = BaseBuildingBase.Cast(target.GetObject());
		return base_building.GetActionComponentName(target.GetComponentIndex());
	}
	
	override string GetAdminLogMessage(ActionData action_data)
	{
		string message = string.Format("Player %1 Mounted BarbedWire on %2", action_data.m_Player, action_data.m_Target.GetObject().ClassName());
		return message;
	}
}