class ActionPlaceObjectCB : ActiondeployObjectCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.DEFAULT_PLACE);
	}
}

class ActionPlaceObject : ActionDeployObject
{		
	void ActionPlaceObject()
	{
		m_CallbackClass	= ActionPlaceObjectCB;

		m_Text = "#place_object";
	}

	override bool HasProgress()
	{
		return false;
	}
}
