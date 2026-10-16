class ActionInteractCodeLockBase : ActionInteractBase
{
	protected const string ATTACHMENT_CODE_LOCK	= "Att_CodeLock";
	
	protected bool m_IsFacingBackside;
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		#ifndef NO_GUI	
		if (g_Game.GetUIManager().GetMenu())
			return false;
		#endif
		
		return true;
	}
}
