class SawPlanksActionData : ActionData
{
	ItemBase 	m_LastPlanksPile; //locally used on server only
}

class ActionSawPlanksCB : ActionContinuousBaseCB
{
	static const float TIME_SAW_HANDSAW = 1.5;
	static const float TIME_SAW_HACKSAW = 3.0;
	static const float TIME_AXES = 1.2;
	
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousRepeat(GetDefaultTime());
	}
	
	float GetDefaultTime()
	{
		string item_type = m_ActionData.m_MainItem.GetType();
		
		switch(item_type)
		{
			case "Hacksaw": 
				return TIME_SAW_HACKSAW;
			break;
		
			case "HandSaw": 
				return TIME_SAW_HANDSAW;
			break;
		
			default: // axes
				return TIME_AXES;
			break
		}
		Debug.Log("ActionSawPlanksCB | Item detection error, assigning negative time","recipes");
		return -1;
	}
};

class ActionSawPlanks: ActionContinuousBase
{
	static const int DECREASE_HEALTH_OF_TOOL_DEFAULT = 10; // this constant is not use anymore see ActionConstants.c UADamageApplied
	//static const int DECREASE_HEALTH_OF_TOOL_AXE = 20; // axes
	//static const int DECREASE_FUEL_OF_CHAINSAW = 20; // chainsaw fuel in ml
	
	static const int YIELD = 3;
	
	void ActionSawPlanks()
	{
		m_CallbackClass = ActionSawPlanksCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_DISASSEMBLE;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_MEDIUM;
		m_Text = "#saw_planks";
		m_LockTargetOnUse = false;
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionTarget = new CCTNonRuined(UAMaxDistances.DEFAULT);
		m_ConditionItem = new CCINonRuined;
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		Object target_O = target.GetObject();
		
		if ( item  &&  target_O.IsInherited(PileOfWoodenPlanks))
		{
			string item_type = item.GetType();
			
			switch(item_type)
			{
				case "Chainsaw":
					if ( item.HasEnergyManager()  &&  item.GetCompEM().CanWork() )
					{
						return true;
					}
					else
					{
						return false;
					}
				break;
			}
			
			return true;
		}
		
		return false;
	}
	
	override ActionData CreateActionData()
	{
		SawPlanksActionData action_data = new SawPlanksActionData();
		return action_data;
	}
	
	override void OnFinishProgressServer( ActionData action_data )
	{
		SawPlanksActionData sawPlanksData = SawPlanksActionData.Cast(action_data);
		
		PileOfWoodenPlanks item_POWP = PileOfWoodenPlanks.Cast( sawPlanksData.m_Target.GetObject() );
		item_POWP.RemovePlanks(YIELD);
		
		if (!sawPlanksData.m_LastPlanksPile)
		{
			SpawnNewPlankPile(sawPlanksData,YIELD);
		}
		else if ((sawPlanksData.m_LastPlanksPile.GetQuantity() + YIELD) >= sawPlanksData.m_LastPlanksPile.GetQuantityMax())
		{
			int remnant = sawPlanksData.m_LastPlanksPile.GetQuantity() + YIELD - sawPlanksData.m_LastPlanksPile.GetQuantityMax();
			sawPlanksData.m_LastPlanksPile.SetQuantity(sawPlanksData.m_LastPlanksPile.GetQuantityMax());
			
			if (remnant > 0)
				SpawnNewPlankPile(sawPlanksData,remnant);
		}
		else
		{
			sawPlanksData.m_LastPlanksPile.AddQuantity(YIELD);
		}
		
		sawPlanksData.m_MainItem.DecreaseHealth("", "", UADamageApplied.SAW_PLANKS);
	}
	
	protected void SpawnNewPlankPile(SawPlanksActionData data, float quantity)
	{
		ItemBase planksNewResult = ItemBase.Cast(data.m_Player.SpawnEntityOnGroundRaycastDispersed("WoodenPlank",0.3,UAItemsSpreadRadius.VERY_NARROW));
		planksNewResult.SetQuantity(quantity);
		data.m_LastPlanksPile = planksNewResult;
	}
	
	///////////////////////////////////////////////////////////////
	//DEPREDATED; do NOT store anything on the shared server action directly!
	ItemBase m_Planks;
	ref InventoryLocation m_PlanksLocation = new InventoryLocation;
};