class SequentialActionData : ActionData
{
	int m_Stage;
	void MultiStageBaseActionData()
	{
		m_Stage = 0;
	}
}

class ActionSequentialBase : ActionBase
{
	void ActionMultiStageBase() 
	{
	}
	
	override ActionData CreateActionData()
	{
		ActionData action_data = new SequentialActionData;
		return action_data;
	}
	
	override int GetStanceMask(PlayerBase player)
	{
		return DayZPlayerConstants.STANCEMASK_ALL;
	}
	
	override int GetActionCategory()
	{
		return AC_SINGLE_USE;
	}

	override typename GetInputType()
	{
		return DefaultActionInput;
	}
	
	override bool CanBePerformedFromInventory()
	{
		return true;
	}
	
	override bool CanBeUsedOnBack()
	{
		return true;
	}
	
	override bool CanBeUsedRaised()
	{
		return true;
	}
	
	override void Start( ActionData action_data )
	{
		super.Start( action_data );
		
		SequentialActionData ad = SequentialActionData.Cast( action_data );
		OnStageStart( ad );
	}
	
	void SetStage(int stage, SequentialActionData action_data)
	{
		OnStageEnd(action_data);
		action_data.m_Stage = stage;
		OnStageStart(action_data);
	}
	
	void OnStageStart(SequentialActionData action_data)
	{
	}
		
	void OnStageEnd(SequentialActionData action_data)
	{
	}
	
	override float GetProgress( ActionData action_data )
	{
		return -1;
	}
};
