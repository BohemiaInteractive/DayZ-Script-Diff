class CAContinuousBase: CABase
{			
	override bool IsContinuousAction() //informs command callback whether action is looped or oneshot
	{
		return true;
	}
	
	void OnCompletePogress(ActionData action_data)
	{
		ActionContinuousBase action = ActionContinuousBase.Cast(action_data.m_Action);
		if(action)
			action.OnFinishProgress(action_data);
	}
};