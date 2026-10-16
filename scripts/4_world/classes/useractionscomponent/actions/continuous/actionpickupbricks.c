class PickUpBricksActionData : ActionData
{
	ItemBase 	m_LastBricksPile; //locally used on server only
}

class ActionPickUpBricksCB : ActionContinuousBaseCB
{
	static const float TIME_HANDS = 1.5;
	
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousRepeat(TIME_HANDS);
	}
};

class ActionPickUpBricks: ActionContinuousBase
{	
	static const int YIELD = 3;
	
	void ActionPickUpBricks()
	{
		m_CallbackClass = ActionPickUpBricksCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_BRICKSTACK;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT;
		m_Text = "#pick_bricks";
		m_LockTargetOnUse = false;
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionTarget = new CCTNonRuined(UAMaxDistances.DEFAULT);
		m_ConditionItem = new CCINotPresent;
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item)
	{
		Object target_O = target.GetObject();
		
		if (target_O.IsInherited(PileOfBricks))
		{			
			return true;
		}
		
		return false;
	}
	
	override ActionData CreateActionData()
	{
		PickUpBricksActionData action_data = new PickUpBricksActionData();
		return action_data;
	}
	
	override void OnFinishProgressServer( ActionData action_data )
	{
		PickUpBricksActionData pickUpBricksData = PickUpBricksActionData.Cast(action_data);
		
		PileOfBricks item_POWP = PileOfBricks.Cast( pickUpBricksData.m_Target.GetObject() );
		item_POWP.RemoveBricks(YIELD);
		
		if (!pickUpBricksData.m_LastBricksPile)
		{
			SpawnNewCommonBrick(pickUpBricksData,YIELD);
		}
		else if ((pickUpBricksData.m_LastBricksPile.GetQuantity() + YIELD) >= pickUpBricksData.m_LastBricksPile.GetQuantityMax())
		{
			int remnant = pickUpBricksData.m_LastBricksPile.GetQuantity() + YIELD - pickUpBricksData.m_LastBricksPile.GetQuantityMax();
			pickUpBricksData.m_LastBricksPile.SetQuantity(pickUpBricksData.m_LastBricksPile.GetQuantityMax());
			
			if (remnant > 0)
				SpawnNewCommonBrick(pickUpBricksData,remnant);
		}
		else
		{
			pickUpBricksData.m_LastBricksPile.AddQuantity(YIELD);
		}
	}
	
	protected void SpawnNewCommonBrick(PickUpBricksActionData data, float quantity)
	{
		ItemBase bricksNewResult = ItemBase.Cast(data.m_Player.SpawnEntityOnGroundRaycastDispersed("CommonBrick",UAItemsSpreadRadius.VERY_NARROW));
		bricksNewResult.SetQuantity(quantity);
		data.m_LastBricksPile = bricksNewResult;
	}
};