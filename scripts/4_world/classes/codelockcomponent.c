//#define CODELOCKCOMPONENT_LOG

// Code lock state enum for specific states
enum CodeLockStage
{
	NONE,
	NORMAL,
	SETTING,
	PROTECTION_STAGE_ONE,
	PROTECTION_STAGE_TWO
}

enum CodeLockUIAction
{
	NONE,
	CONFIRM_INPUT,
	CLEAR_INPUT,
	DIGIT_INPUT
}

class CodeLockComponent : Managed
{
	const int MIN_LOCK_PIN_LENGTH = 4; // min amount of digits the combination needs to contain
	const int MAX_LOCK_PIN_LENGTH = 6; // max amount of digits the combination can contain
	
	private const float DEFAULT_BRUTEFORCE_TIME = 30.0;
	private const float DEFAULT_BRUTEFORCE_RESET_TIME = 3600.0;
	
	protected int m_MinLockPinLength;
	
	protected string m_InputPIN;			// combination that is dialed on lock
	protected string m_LockPIN;		// actual combination of the lock
	
	protected bool m_IsLocked;			// lock state
	protected EntityAI	m_Parent;		// code lock parent entity
	protected int m_DoorIndex = -1;	// code lock door index for parent building if code lock belongs to a door
	
	protected bool m_HasPIN;			// code state
	
	protected float m_BruteForceProtectionTime;
	protected float m_BruteForceResetTime;
	
	protected int m_Stage;
	protected ref Timer m_BruteForceUpdateTimer;
	protected ref Timer m_BruteForceResetTimer;
	protected int m_WrongCodeCount;
	protected bool m_HasBruteForceProtection;
	protected bool m_HandleBruteForceProtection;
	protected CodeLockUIAction m_UIAction;
	
	void CodeLockComponent(notnull EntityAI parent, int minLockPinLength = MIN_LOCK_PIN_LENGTH)
	{
		m_Parent = parent;
		m_MinLockPinLength = minLockPinLength;
				
		m_InputPIN = "";
		m_LockPIN = "";
		
		m_IsLocked = false;	
		m_HasPIN = false;
		
		m_DoorIndex = -1;
		
		m_Stage = CodeLockStage.NONE;
		
		m_BruteForceProtectionTime = CfgGameplayHandler.GetExternalLockProtectionTime();
		m_BruteForceResetTime = CfgGameplayHandler.GetExternalLockProtectionResetTime();
		
		if (m_BruteForceProtectionTime <= 0)
			m_BruteForceProtectionTime = DEFAULT_BRUTEFORCE_TIME;
		
		if (m_BruteForceResetTime <= 0)
			m_BruteForceResetTime = DEFAULT_BRUTEFORCE_RESET_TIME;
	}
	
	void SetStage(CodeLockStage stage)
	{
		m_Stage = stage;
		#ifdef CODELOCKCOMPONENT_LOG
		ErrorEx(string.Format("Lock stage=%1", typename.EnumToString(CodeLockStage, m_Stage)), ErrorExSeverity.INFO);
		#endif
	}
	
	int GetStage()
	{
		return m_Stage;
	}
	
	void SetMinimalPinCodeLength(int length)
	{
		m_MinLockPinLength = length;
	}
	
	bool SetInputCode(string pin, bool force = true)
	{
		if (!force)
		{
			int pinLength = pin.Length();
			if (pinLength < m_MinLockPinLength || pinLength > MAX_LOCK_PIN_LENGTH)
				return false;
		}

		m_InputPIN = pin;
		
		#ifdef CODELOCKCOMPONENT_LOG
		ErrorEx("Set input pin=" + m_InputPIN, ErrorExSeverity.INFO);
		#endif
		
		return true;
	}
	
	protected bool ValidatePIN(string pin)
	{
		if (pin == "")
			return false;
		
		int pinLength = pin.Length();
		return pinLength >= m_MinLockPinLength && pinLength <= MAX_LOCK_PIN_LENGTH;
	}
		
	bool SetLockCode(string pin, bool sync = true)
	{
		if (!ValidatePIN(pin))
		{
			#ifdef CODELOCKCOMPONENT_LOG
			ErrorEx(string.Format("Invalid PIN length: %1 (min: %2, max: %3)", pin.Length(), m_MinLockPinLength, MAX_LOCK_PIN_LENGTH), ErrorExSeverity.WARNING);
			#endif
			return false;
		}
				
		m_LockPIN = pin;
		m_HasPIN = true;
		
		SetStage(CodeLockStage.NORMAL);
		SetLockState(true, false);
		
		#ifdef CODELOCKCOMPONENT_LOG
		ErrorEx("Set lock pin=" + m_LockPIN, ErrorExSeverity.INFO);
		#endif
		
		if (sync)
			SyncAndMarkDirty();
				
		return true;
	}
	
	void ResetPIN(bool sync = true)
	{
 		m_LockPIN = "";
		m_HasPIN = false;
		SetStage(CodeLockStage.SETTING);
		
		if (sync)
			SyncAndMarkDirty();
	}
		
	void SetLockState(bool state, bool sync = true)
	{
		if (m_IsLocked == state)
			return;
		
		m_IsLocked = state;
		
		if (!state)
		{
			StopAllTimers();
			
			m_HasBruteForceProtection = false;
       	 	m_HandleBruteForceProtection = false;
			m_WrongCodeCount = 0;
			
			if (m_HasPIN)
			{
				if (m_Stage >= CodeLockStage.PROTECTION_STAGE_ONE)
				{
					SetStage(CodeLockStage.PROTECTION_STAGE_TWO);
				}
				else
				{
					SetStage(CodeLockStage.NORMAL);
				}
			}
		}
		
		if (m_DoorIndex > -1)
			HandleHierarchyRootLock(state);
		
		if (g_Game.IsServer() || !g_Game.IsMultiplayer())
		{
			ItemBase parentIB = ItemBase.Cast(m_Parent);
			if (parentIB)
			{
				if (state)
				{
					parentIB.StartItemSoundServer(SoundConstants.ITEM_DIGITAL_LOCK_LOCK);
				}
				else
				{
					parentIB.StartItemSoundServer(SoundConstants.ITEM_DIGITAL_LOCK_UNLOCK);
				}
			}
		}
		
		if (sync)
			SyncAndMarkDirty();
				
		#ifdef CODELOCKCOMPONENT_LOG
		ErrorEx("Set lock state=" + m_IsLocked, ErrorExSeverity.INFO);
		#endif
	}
	
	protected void HandleHierarchyRootLock(bool state)
	{
		if (m_DoorIndex == -1)
		{
			#ifdef CODELOCKCOMPONENT_LOG
			ErrorEx(string.Format("Error! Invalid call of HandleHierarchyRootLock without valid door index! Parent=%1 | Door index=%2", m_Parent, m_DoorIndex), ErrorExSeverity.ERROR);
			#endif
			return;
		}

		Building building = Building.Cast(m_Parent.GetHierarchyRoot());
		if (!building || !building.CanUseConstruction())
		{
			#ifdef CODELOCKCOMPONENT_LOG
			ErrorEx(string.Format("Error! Invalid hierarchy root parent for lock! Parent=%1 | Hierarchy root parent=%2", m_Parent, m_Parent.GetHierarchyRoot()), ErrorExSeverity.ERROR);
			#endif
			return;
		}
		
		if (m_DoorIndex > building.GetDoorCount() - 1)
		{
			#ifdef CODELOCKCOMPONENT_LOG
			ErrorEx(string.Format("Warning! Given door index is out of bounds! Door index=%1 | Doors count=%2", m_DoorIndex, building.GetDoorCount()), ErrorExSeverity.ERROR);
			#endif
			return;
		}
		
		if (state)
		{
			building.LockDoor(m_DoorIndex);
			
			#ifdef CODELOCKCOMPONENT_LOG
			ErrorEx(string.Format("Locked door! Door index=%1 | Doors count=%2", m_DoorIndex, building.GetDoorCount()), ErrorExSeverity.INFO);
			#endif
		}
		else
		{
			building.UnlockDoor(m_DoorIndex);
			
			#ifdef CODELOCKCOMPONENT_LOG
			ErrorEx(string.Format("Unlocked door! Door index=%1 | Doors count=%2", m_DoorIndex, building.GetDoorCount()), ErrorExSeverity.INFO);
			#endif
		}
	}
	
	void OnStoreSave(ParamsWriteContext ctx)
	{
		ctx.Write(m_LockPIN);
		ctx.Write(m_IsLocked);
		ctx.Write(m_DoorIndex);
	}
	
	bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!ctx.Read(m_LockPIN))
		{
			m_LockPIN = "";
			m_HasPIN = false;
			return false;
		}
		
		if (!ctx.Read(m_IsLocked))
			return false;

		if (!ctx.Read(m_DoorIndex))
			return false;
		
		#ifdef CODELOCKCOMPONENT_LOG
		ErrorEx(string.Format("Loaded lock pin=%1", m_LockPIN), ErrorExSeverity.INFO);
		ErrorEx(string.Format("Lock pin length=%1", m_LockPIN.Length()), ErrorExSeverity.INFO);
		#endif
		
		if (m_LockPIN != "" && m_LockPIN.Length() >= m_MinLockPinLength)
		{
			m_HasPIN = true;
		}
		
		#ifdef CODELOCKCOMPONENT_LOG
		ErrorEx(string.Format("Has pin=%1", m_HasPIN), ErrorExSeverity.INFO);
		ErrorEx(string.Format("Is locked=%1", m_IsLocked), ErrorExSeverity.INFO);
		#endif
		
		SetLockState(m_IsLocked, false);
		
		return true;
	}
	
	protected void CheckUIActionServer(CodeLockUIAction uiAction)
	{
		if (g_Game.IsServer() || !g_Game.IsMultiplayer())
		{
			#ifdef CODELOCKCOMPONENT_LOG
			ErrorEx("UI Action=" + typename.EnumToString(CodeLockUIAction, uiAction), ErrorExSeverity.INFO);
			#endif
			
			m_UIAction = uiAction;
			SyncAndMarkDirty();
			
			g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(ResetUIActionServer, 100);
		}
	}
	
	protected void ResetUIActionServer()
	{
		m_UIAction = CodeLockUIAction.NONE;
		SyncAndMarkDirty();
	}
	
	void CheckUIActionClient()
	{
		#ifdef CODELOCKCOMPONENT_LOG
		ErrorEx("UI Action=" + typename.EnumToString(CodeLockUIAction, m_UIAction), ErrorExSeverity.INFO);
		#endif
		
		ItemBase parentIB = ItemBase.Cast(m_Parent);
		switch(m_UIAction)
		{
			case CodeLockUIAction.CLEAR_INPUT:
			{
				parentIB.StartItemSoundClient(SoundConstants.ITEM_DIGITAL_LOCK_CLEAR_INPUT);
				break;
			}
			case CodeLockUIAction.CONFIRM_INPUT:
			{
				parentIB.StartItemSoundClient(SoundConstants.ITEM_DIGITAL_LOCK_CONFIRM_INPUT);
				break;
			}
			case CodeLockUIAction.DIGIT_INPUT:
			{
				parentIB.StartItemSoundClient(SoundConstants.ITEM_DIGITAL_LOCK_DIGIT_INPUT);
				break;
			}
		}
		
		m_UIAction = CodeLockUIAction.NONE;
	}
		
	// Called on server on RPC request
	protected void CheckCodeRequest(string pin, PlayerIdentity identity)
	{
		bool success;

		if (g_Game.IsServer() || !g_Game.IsMultiplayer())
		{
			if (!SetInputCode(pin))
				return;
						
			if (m_HasPIN)
			{
				// Check lock state and see if pin is valid
				if (m_IsLocked)
				{
					#ifdef CODELOCKCOMPONENT_LOG
					ErrorEx("Lock pin=" + GetInputPIN(), ErrorExSeverity.INFO);
					ErrorEx("Actual pin=" + GetLockPIN(), ErrorExSeverity.INFO);
					#endif
															
					if (m_InputPIN == m_LockPIN)
					{
						// Lock is locked and given pin is same as lock pin. Unlock lock
						// SetLockState will handle clearing brute force protection
						SetLockState(false, false);
						success = true;
					}
					else
					{
						// Handle brute force protection
						ItemBase parentIB = ItemBase.Cast(m_Parent);
						if (parentIB)
							parentIB.StartItemSoundServer(SoundConstants.ITEM_DIGITAL_LOCK_ERROR);
						HandleBruteForceProtectionServer();	
						success = false;
					}
				}
			}
			else
			{
				// Lock has no pin. Set pin!
				SetLockCode(pin, false);
				success = true;
			}
		}
		
		if (g_Game.IsDedicatedServer())
		{
			m_Parent.SetSynchDirty();
		}

		// Force visual update after setting code
		DigitalCodeLock codeLock;
		if (Class.CastTo(codeLock, m_Parent) && codeLock.GetVisualManager())
		{
			codeLock.GetVisualManager().UpdateVisuals();
		}
		
		Param1<bool> response = new Param1<bool>(success);
		GetGame().RPCSingleParam(m_Parent, ERPCs.RPC_CODE_LOCK_RESPONSE, response, true, identity);
	}
	
	protected void HandleBruteForceProtectionServer()
	{
		if (m_HasBruteForceProtection)
			return;
				
		m_WrongCodeCount++;
		
		StopAllTimers();
		
		if (ShouldTriggerProtectionStage())
		{
			ActivateBruteForceProtection(m_BruteForceProtectionTime, m_BruteForceResetTime);
		}
	}
	
	private void ActivateBruteForceProtection(float protectionTime, float protectionResetTime)
	{
		if (m_Stage < CodeLockStage.PROTECTION_STAGE_ONE)
		{
			SetStage(CodeLockStage.PROTECTION_STAGE_ONE);
		}
		else if (m_Stage == CodeLockStage.PROTECTION_STAGE_ONE)
		{
			SetStage(CodeLockStage.PROTECTION_STAGE_TWO);
		}
		
		m_HasBruteForceProtection = true;
		m_HandleBruteForceProtection = true;
		
		m_BruteForceUpdateTimer = new Timer(CALL_CATEGORY_SYSTEM);
		m_BruteForceUpdateTimer.Run(protectionTime, this, "CheckBruteForceState");
		
		m_BruteForceResetTimer = new Timer(CALL_CATEGORY_SYSTEM);
		m_BruteForceResetTimer.Run(protectionResetTime, this, "CheckBruteForceReset");
		
		m_WrongCodeCount = 0;
		UpdateParentVisuals();
		
		if (g_Game.IsDedicatedServer())
			m_Parent.SetSynchDirty();
	}
	
	void StopAllTimers()
	{
		if (m_BruteForceUpdateTimer)
		{
			m_BruteForceUpdateTimer.Stop();
			m_BruteForceUpdateTimer = NULL;
		}
		
		if (m_BruteForceResetTimer)
		{
			m_BruteForceResetTimer.Stop();
			m_BruteForceResetTimer = NULL;
		}
	}
	
	private bool ShouldTriggerProtectionStage()
	{		
		switch(m_Stage)
		{
			case CodeLockStage.NORMAL:
			{
				return m_WrongCodeCount >= CfgGameplayHandler.GetExternalLockProtectionCountStageOne();
			}
			case CodeLockStage.PROTECTION_STAGE_ONE:
			case CodeLockStage.PROTECTION_STAGE_TWO:
			{
				return m_WrongCodeCount >= CfgGameplayHandler.GetExternalLockProtectionCountStageTwo();
			}
		}
		return false;
	}
		
	protected void CheckBruteForceState()
	{
		if (m_BruteForceUpdateTimer)
		{
			m_BruteForceUpdateTimer.Stop();
			m_BruteForceUpdateTimer = NULL;
		}
		
		m_HandleBruteForceProtection = false;
    	m_HasBruteForceProtection = false;
		
		SyncAndMarkDirty();
	}
	
	protected void CheckBruteForceReset()
	{
		if (m_BruteForceResetTimer)
		{
			m_BruteForceResetTimer.Stop();
			m_BruteForceResetTimer = NULL;
		}
		
		if (m_BruteForceUpdateTimer)
		{
			m_BruteForceUpdateTimer.Stop();
			m_BruteForceUpdateTimer = NULL;
		}
		
		m_HasBruteForceProtection = false;
		m_HandleBruteForceProtection = false;
		m_WrongCodeCount = 0;
		
		if (m_HasPIN)
			SetStage(CodeLockStage.NORMAL);
		
		SyncAndMarkDirty();
	}
	
	void OnParentDetached(bool sync = true)
	{		
		m_DoorIndex = -1;
		OnParentReset(false);
		
		if (sync)
			SyncAndMarkDirty();
	}
	
	void OnParentReset(bool sync = true)
	{
		SetLockState(false, false);
		ResetPIN(false);
		m_WrongCodeCount = 0;
		m_HasBruteForceProtection = false;
		SetStage(CodeLockStage.NONE);
		
		if (m_BruteForceUpdateTimer)
		{
			m_BruteForceUpdateTimer.Stop();
			m_BruteForceUpdateTimer = NULL;
		}
		
		if (m_BruteForceResetTimer)
		{
			m_BruteForceResetTimer.Stop();
			m_BruteForceResetTimer = NULL;
		}
		
		if (sync)
			SyncAndMarkDirty();
	}
	
	void OnRPC(PlayerIdentity sender, int type, ParamsReadContext ctx) 
	{
		switch (type)
		{
			case ERPCs.RPC_CODE_LOCK_REQUEST:
			{
				Param1<string> code = new Param1<string>("");
				if (!ctx.Read(code))
					return;
				
				string dialedCode = code.param1;
				CheckCodeRequest(dialedCode, sender);
				SetInputCode("", true);
				break;
			}
			case ERPCs.RPC_CODE_LOCK_RESPONSE:
			{
				Param1<bool> response = new Param1<bool>(false);
				if (!ctx.Read(response))
					return;
				
				DigitalCodeLockUIBase codeLockUI = DigitalCodeLockUIBase.Cast(GetGame().GetUIManager().FindMenu(MENU_DIGITAL_CODELOCK));
				if (codeLockUI)
				{
					bool codeResponse = response.param1;
					codeLockUI.CodeResponse(codeResponse);
					SetInputCode("", true);
				}		
				break;
			}
			case ERPCs.RPC_CODE_LOCK_UI_ACTION:
			{
				Param1<int> action = new Param1<int>(CodeLockUIAction.NONE);
				if (!ctx.Read(action))
					return;
				
				CodeLockUIAction uiAction = action.param1;
				if (uiAction != CodeLockUIAction.NONE)
					CheckUIActionServer(uiAction);
				break;
			}
		}
	}
	
	protected void UpdateParentVisuals()
	{
		if (!m_Parent)
			return;
		
		DigitalCodeLock codeLock;
		if (Class.CastTo(codeLock, m_Parent) && codeLock.GetVisualManager())
		{
			codeLock.GetVisualManager().MarkDirty();
			codeLock.GetVisualManager().UpdateVisuals();
		}
	}
	
	protected void SyncAndMarkDirty()
	{	
		// Always update parent visuals (will be marked dirty or updated depending on context)
		UpdateParentVisuals();
		
		if (g_Game.IsDedicatedServer())
			m_Parent.SetSynchDirty();
	}
	
	EntityAI GetParent()
	{
		return m_Parent;
	}
	
	void SetDoorIndex(int index)
	{
		m_DoorIndex = index;
	}
	
	int GetDoorIndex()
	{
		return m_DoorIndex;
	}
	
	//! ToDo: Door parent position handling?!
	vector GetPosition()
	{
		return m_Parent.GetPosition();
	}
	
	string GetInputPIN()
	{
		return m_InputPIN;
	}
	
	bool HasPIN() 
	{
		return m_HasPIN;
	}
	
	string GetLockPIN()
	{
		return m_LockPIN;
	}
	
	bool IsLocked()
	{
		return m_IsLocked;
	}
		
	bool HasParent()
	{
		return m_Parent || m_DoorIndex > -1;
	}
	
	bool HasBruteForceProtection()
	{
		return m_HasBruteForceProtection;
	}
	
	bool ShouldHandleBruteForce()
	{
		return m_HandleBruteForceProtection;
	}
	
	int GetWrongCodeCount()
	{
		return m_WrongCodeCount;
	}
	
	int GetMinLockPINLength()
	{
		return m_MinLockPinLength;
	}
	
	CodeLockUIAction GetUIAction()
	{
		return m_UIAction;
	}
}

class CodeLockComponentStatic : CodeLockComponent
{
	const string SELECTION_INTERACT		= "codelockinteract"; //! used on static objects

	override vector GetPosition()
	{
		vector position = m_Parent.GetSelectionBasePositionLS(SELECTION_INTERACT);
		return m_Parent.ModelToWorld(position);
	}
}
