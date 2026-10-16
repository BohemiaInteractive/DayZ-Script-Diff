class InteractCodeLockActionData : ActionData
{
	bool   m_IsFacingBackside  = false;
	vector m_CodeLockPosition  = vector.Zero;
}

class InteractCodeLockActionReciveData : ActionReciveData
{
	bool   m_IsFacingBackside  = false;
	vector m_CodeLockPosition  = vector.Zero;
}

class ActionInteractCodeLockItem : ActionInteractCodeLockBase
{
	override void CreateConditionComponents()
	{
		m_ConditionItem 	= new CCINone();
		m_ConditionTarget 	= new CCTCursorInherited({DigitalCodeLock}, UAMaxDistances.DEFAULT);

		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_ATTACHITEM;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
	}
	
	override ActionData CreateActionData()
	{
		InteractCodeLockActionData actionData = new InteractCodeLockActionData();
		return actionData;
	}
	
	override bool SetupAction(PlayerBase player, ActionTarget target, ItemBase item, out ActionData action_data, Param extra_data = NULL)
	{
		if (super.SetupAction( player, target, item, action_data, extra_data))
		{
			if (!g_Game.IsDedicatedServer())
			{
				DigitalCodeLock codeLock = DigitalCodeLock.Cast(target.GetObject());
				if (!codeLock || (codeLock && !codeLock.HasEnergy()))
					return false;
				
				if (!codeLock.GetHierarchyRoot())
					return false;

				InteractCodeLockActionData interactActionData = InteractCodeLockActionData.Cast(action_data);
				interactActionData.m_CodeLockPosition = codeLock.GetWorldPosition();
				interactActionData.m_IsFacingBackside = codeLock.IsPlayerOnBackside(player);
			}
		}

		return true;
	}

	override void WriteToContext(ParamsWriteContext ctx, ActionData action_data)
	{
		super.WriteToContext(ctx, action_data);
		
		InteractCodeLockActionData interactActionData = InteractCodeLockActionData.Cast(action_data);
		if (!interactActionData)
			return;

		ctx.Write(interactActionData.m_IsFacingBackside);
		ctx.Write(interactActionData.m_CodeLockPosition);
	}
	
	override bool ReadFromContext(ParamsReadContext ctx, out ActionReciveData action_recive_data)
	{
		if (!action_recive_data)
			action_recive_data = new InteractCodeLockActionReciveData;

		super.ReadFromContext(ctx, action_recive_data);
		InteractCodeLockActionReciveData interactReciveData = InteractCodeLockActionReciveData.Cast(action_recive_data);
		
		bool isFacingBackside = false;
		if (!ctx.Read(isFacingBackside))
			return false;
		
		interactReciveData.m_IsFacingBackside = isFacingBackside;

		vector lockPos = vector.Zero;
		if (!ctx.Read(lockPos))
			return false;
		interactReciveData.m_CodeLockPosition = lockPos;

		return true;
	}
	
	override void HandleReciveData(ActionReciveData action_recive_data, ActionData action_data)
	{
		super.HandleReciveData(action_recive_data, action_data);
		
		InteractCodeLockActionReciveData interactReciveData = InteractCodeLockActionReciveData.Cast(action_recive_data);
		InteractCodeLockActionData interactActionData = InteractCodeLockActionData.Cast(action_data);
		
		interactActionData.m_IsFacingBackside = interactReciveData.m_IsFacingBackside;
		interactActionData.m_CodeLockPosition = interactReciveData.m_CodeLockPosition;
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!super.ActionCondition(player, target, item))
			return false;
		
		bool isFacingBackside = false;
		DigitalCodeLock codeLock = DigitalCodeLock.Cast(target.GetObject());
		if (!codeLock || (codeLock && !codeLock.HasEnergy()))
			return false;
		
		if (!g_Game.IsDedicatedServer())
		{
			if (!codeLock.GetHierarchyRoot())
				return false;
			
			isFacingBackside = codeLock.IsPlayerOnBackside(player);
			
			if (codeLock.HasPIN())
			{
				if (!codeLock.IsLocked() && codeLock.CanBeLocked())
				{
					m_Text = "#STR_LOCK_CODELOCK";
					return true;
				}
				
				if (codeLock.IsLocked() && codeLock.CanBeUnlocked())
				{
					if (!isFacingBackside && !codeLock.HasBruteForceProtection())
					{
						m_Text = "#STR_ENTER_CODE";
						return true;
					}
					else if (isFacingBackside)
					{
						m_Text = "#STR_UNLOCK_CODELOCK";
						return true;
					}
				}
			}
			else
			{
				if (!isFacingBackside && codeLock.CanSetCode())
				{
					m_Text = "#STR_SET_CODE";
					return true;
				}
			}
		}
		else
		{
			return true;
		}
		
		return false;
	}
		
	override void OnExecuteServer(ActionData action_data)
	{
		DigitalCodeLock codeLock = DigitalCodeLock.Cast(action_data.m_Target.GetObject());
		if (!codeLock)
			return;
		
		InteractCodeLockActionData interactActionData = InteractCodeLockActionData.Cast(action_data);
		if (!interactActionData || interactActionData.m_CodeLockPosition == vector.Zero)
			return;
		
		//! Re-validate the lock side on the server, the client-provided value is not trusted
		bool isFacingBackside = codeLock.IsPlayerOnBackside(action_data.m_Player);
		
		if (interactActionData.m_IsFacingBackside && !isFacingBackside)
		{
			ErrorEx(string.Format("[ActionInteractCodeLockItem] Client claimed backside but server disagrees, physical release rejected! Player=%1 | Lock=%2", action_data.m_Player, codeLock), ErrorExSeverity.INFO);
			return;
		}
		
		if ((!codeLock.HasPIN() || codeLock.HasPIN() && codeLock.IsLocked()) && !isFacingBackside)
		{
			codeLock.StartItemSoundServer(SoundConstants.ITEM_DIGITAL_LOCK_ACCESS);
		}
		
		DigitalCodeLockUIBase codeLockUI;
		if (codeLock.HasPIN())
		{
			if (!codeLock.IsLocked() && codeLock.CanBeLocked())
			{
				codeLock.SetLockState(true);
				return;
			}
			
			if (codeLock.IsLocked() && codeLock.CanBeUnlocked() && isFacingBackside)
			{
				codeLock.SetLockState(false);
			}
		}
		
		interactActionData.m_IsFacingBackside = false;
	}
	
	override void OnExecuteClient(ActionData action_data)
	{
		HandleCodelockClient(action_data);
	}
	
	//! Only handles single player/offline handling
	override void OnExecute(ActionData action_data)
	{
		if (!GetGame().IsServer() && GetGame().IsMultiplayer())
			return;
		
		HandleCodelockClient(action_data);
	}
	
	protected void HandleCodelockClient(ActionData actionData)
	{
		DigitalCodeLock codeLock = DigitalCodeLock.Cast(actionData.m_Target.GetObject());
		if (!codeLock)
			return;
		
		if (!codeLock.GetHierarchyRoot())
			return;
		
		InteractCodeLockActionData interactActionData = InteractCodeLockActionData.Cast(actionData);
		bool isFacingBackside = interactActionData.m_IsFacingBackside;
		
		DigitalCodeLockUIBase codeLockUI;
		if ((!codeLock.HasPIN() || codeLock.HasPIN() && codeLock.IsLocked()) && !isFacingBackside)
		{
			#ifndef NO_GUI
			codeLockUI = DigitalCodeLockUIBase.Cast(GetGame().GetUIManager().EnterScriptedMenu(MENU_DIGITAL_CODELOCK, null));
			if (codeLockUI)
				codeLockUI.SetTargetLock(codeLock.GetCodeLockComponent());
			#endif
			return;
		}
	}
		
	override bool CanBeSetFromInventory()
	{
		return false;
	}
}
