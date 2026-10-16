class ActionResetCodeOnCodeLock : ActionInteractBase
{
	protected const string SELECTION_FRONT			= "front";
	
	override void CreateConditionComponents() 
	{
		m_ConditionItem 	= new CCINone();
		m_ConditionTarget 	= new CCTNonRuined(UAMaxDistances.DEFAULT);
		
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_ATTACHITEM;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "#STR_RESET_CODE";
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!super.ActionCondition(player, target, item))
			return false;
		
		DigitalCodeLock codeLock = DigitalCodeLock.Cast(target.GetObject());
		if ((!codeLock || (codeLock && !codeLock.GetCompEM().IsWorking())))
			return false;
				
		bool isFacingBackside = codeLock.IsPlayerOnBackside(player);
		
		if (!codeLock.HasPIN() || codeLock.IsLocked() || isFacingBackside)
			return false;
								
		return true;
	}
	
	override void OnExecuteServer(ActionData action_data)
	{
		DigitalCodeLock codeLock = DigitalCodeLock.Cast(action_data.m_Target.GetObject());
		if (!codeLock)
			return;
		
		codeLock.ResetPIN();
	}
	
	override void OnExecuteClient(ActionData action_data)
	{
		DigitalCodeLock codeLock = DigitalCodeLock.Cast(action_data.m_Target.GetObject());
		if (!codeLock)
			return;
		
		#ifndef NO_GUI
		DigitalCodeLockUIBase codeLockUI = DigitalCodeLockUIBase.Cast(GetGame().GetUIManager().EnterScriptedMenu(MENU_DIGITAL_CODELOCK, null));
		codeLockUI.SetTargetLock(codeLock.GetCodeLockComponent());
		#endif
	}
	
	//! Only handles single player/offline handling
	override void OnExecute(ActionData action_data)
	{
		if (!GetGame().IsServer() && GetGame().IsMultiplayer())
			return;
		
		DigitalCodeLock codeLock = DigitalCodeLock.Cast(action_data.m_Target.GetObject());
		if (!codeLock)
			return;
		
		#ifndef NO_GUI
		DigitalCodeLockUIBase codeLockUI = DigitalCodeLockUIBase.Cast(GetGame().GetUIManager().EnterScriptedMenu(MENU_DIGITAL_CODELOCK, null));
		codeLockUI.SetTargetLock(codeLock.GetCodeLockComponent());
		#endif
	}
		
	override bool CanBeSetFromInventory()
	{
		return false;
	}
}
