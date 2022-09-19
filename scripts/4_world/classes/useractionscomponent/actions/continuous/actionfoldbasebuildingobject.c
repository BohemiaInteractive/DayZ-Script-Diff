class ActionFoldBaseBuildingObjectCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime( UATimeSpent.DEFAULT_DEPLOY );
	}
}

class ActionFoldBaseBuildingObject: ActionContinuousBase
{
	void ActionFoldBaseBuildingObject()
	{
		m_CallbackClass = ActionFoldBaseBuildingObjectCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_HIGH;
		m_Text = "#fold";
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionTarget = new CCTNonRuined( UAMaxDistances.DEFAULT );
		m_ConditionItem = new CCINotPresent;
	}
	
	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		Object targetObject = target.GetObject();
		
		if ( targetObject.CanUseConstruction() )
		{
			BaseBuildingBase base_building = BaseBuildingBase.Cast( targetObject );
			
			if ( base_building.CanFoldBaseBuildingObject() )
			{
				return true;
			}
		}
		
		return false;
	}
	
	override void OnFinishProgressServer( ActionData action_data )
	{
		ItemBase item;
		BaseBuildingBase base_building = BaseBuildingBase.Cast( action_data.m_Target.GetObject() );
		
		base_building.CreateConstructionKitInHands(action_data.m_Player);
		base_building.DestroyConstruction();
	}
	
	override string GetAdminLogMessage(ActionData action_data)
	{
		return " folded " + action_data.m_Target.GetObject().GetDisplayName();
	}
}