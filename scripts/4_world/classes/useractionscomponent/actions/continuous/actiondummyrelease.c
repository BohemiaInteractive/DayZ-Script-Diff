//dummy action for release indication
class ActionDummyContinuousRelease: ActionContinuousBase
{
	void ActionDummyContinuousRelease()
	{
		m_Text = "#STR_FishingAction_Yank" + " [" + "#keybind_release" + "]";
	}
	
	override typename GetInputType()
	{
		return DefaultActionInput;
	}
	
	override bool HasTarget()
	{
		return false;
	}
}