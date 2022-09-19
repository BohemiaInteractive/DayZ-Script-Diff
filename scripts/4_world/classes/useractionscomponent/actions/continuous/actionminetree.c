class MineActionData : ActionData
{
	EHarvestType m_HarvestType = EHarvestType.NORMAL;
}

class ActionMineTreeCB : ActionContinuousBaseCB
{
	private const float TIME_BETWEEN_MATERIAL_DROPS_DEFAULT = 4;
	
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousMineWood(TIME_BETWEEN_MATERIAL_DROPS_DEFAULT);
	} 
};

class ActionMineBase : ActionContinuousBase
{
	EHarvestType m_HarvestType = EHarvestType.NORMAL;
	
	override ActionData CreateActionData()
	{
		MineActionData data = new MineActionData;
		data.m_HarvestType = m_HarvestType;
		return data;
	}
	
	override void OnActionInfoUpdate( PlayerBase player, ActionTarget target, ItemBase item )
	{
		m_Text =  "#harvest" + " " + GetYieldName(player, target, item);
	}
	
	string GetYieldName( PlayerBase player, ActionTarget target, ItemBase item )
	{
		//given the circumstances, the implementation bellow is the path of least resistance
		Object targetObject = target.GetObject();
		WoodBase wood = WoodBase.Cast(targetObject);
		RockBase rock = RockBase.Cast(targetObject);
		string yieldName;
		if (wood || rock)
		{
			map<string,int> output_map = new map<string,int>;
			if (wood)
			{
				wood.GetMaterialAndQuantityMapEx(item, output_map, m_HarvestType);
			}
			else
			{
				rock.GetMaterialAndQuantityMap(item, output_map);
			}
			if (output_map.Count() > 0)
			{
				yieldName = MiscGameplayFunctions.GetItemDisplayName(output_map.GetKey(0));
			}
		}
		return yieldName;
	}
	
}
	
class ActionMineTree : ActionMineBase
{
	void ActionMineTree()
	{
		m_CallbackClass = ActionMineTreeCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_HACKTREE;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_HIGH;
	}
	
	override void CreateConditionComponents()
	{	
		m_ConditionTarget = new CCTCursor(UAMaxDistances.SMALL);
		m_ConditionItem = new CCINonRuined;
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		
		//Action not allowed if player has broken legs
		if (player.GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS)
			return false;
		
		Object targetObject = target.GetObject();
		//GetMaterialAndQuantityMap
		
		return targetObject.IsTree() && targetObject.IsCuttable();
	}
	
	override void OnFinishProgressServer( ActionData action_data )
	{
		action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
		// "hiding" of map item
		//action_data.m_Target.GetObject().SetOrigin("0 0 0");
	}
};