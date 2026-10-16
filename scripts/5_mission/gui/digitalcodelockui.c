class DigitalCodeLockUI : DigitalCodeLockUIBase
{
	private const int MOVE_LEFT = 0;
	private const int MOVE_RIGHT = 1;
	private const int MOVE_UP = 2;
	private const int MOVE_DOWN = 3;
	
	private const int RESET_BUTTON_INDEX = 9;
	private const int CONFIRM_BUTTON_INDEX = 11;
	
	private const float BT_REPEAT_DELAY = 0.35;				// Delay from first press to begin tick repeat state
	private const float BT_REPEAT_TIME = 0.09;				// Tick repeat frequency time
	private const float BT_THRESHOLD_VALUE = 0.8;			// Threshold values for analog inputs
	
	private const int MIN_LOCK_PIN_LENGTH = 4;
	private const int MAX_LOCK_PIN_LENGTH = 6;
	
#ifndef PLATFORM_CONSOLE
	private const int KEY_DIGIT_1 = 0;
	private const int KEY_DIGIT_2 = 1;
	private const int KEY_DIGIT_3 = 2;
	private const int KEY_DIGIT_4 = 3;
	private const int KEY_DIGIT_5 = 4;
	private const int KEY_DIGIT_6 = 5;
	private const int KEY_DIGIT_7 = 6;
	private const int KEY_DIGIT_8 = 7;
	private const int KEY_DIGIT_9 = 8;
	private const int KEY_DIGIT_CLEAR = 9;
	private const int KEY_DIGIT_0 = 10;
	private const int KEY_DIGIT_CONFIRM = 11;
	
	protected ref map<KeyCode, int> m_KeyCodeMap;
#endif
	
	private ref array<string> m_InvInputNames = {"UAUILeftInventory","UAUIRightInventory","UAUIUpInventory","UAUIDownInventory"};
	private ref array<UAIDWrapper> m_InvInputWrappers;
	private ref array<float> m_InvInputTimes;
	private int m_InvInputWrappersCount;
	private float m_SensitivityThreshold = 0.0;
	private UAInput m_InvUAInput;
	private int m_InvInputActive = 0;
	
	private ref array<string> m_InputExcludes = {"menu_move"};
	
	private ref array<ButtonWidget> m_NumButtons;
	private int m_SelectedButtonIndex = 0; 					// Default to button "5" (index 4)
	protected bool m_IsActive = false;
	
	private string m_EnteredPIN;
	private EntityAI m_TargetLock;
	private vector m_TargetPosition;
	private bool m_HasPIN = false;
	
	protected Widget m_ConsoleToolbar;
	protected RichTextWidget m_ConsoleToolbarRichText;
	
	void DigitalCodeLockUI()
	{
	#ifndef PLATFORM_CONSOLE
		if (!m_KeyCodeMap)
		{
			m_KeyCodeMap = new map<KeyCode, int>();
			m_KeyCodeMap.Insert(KeyCode.KC_1, KEY_DIGIT_1);
			m_KeyCodeMap.Insert(KeyCode.KC_NUMPAD1, KEY_DIGIT_1);
			m_KeyCodeMap.Insert(KeyCode.KC_2, KEY_DIGIT_2);
			m_KeyCodeMap.Insert(KeyCode.KC_NUMPAD2, KEY_DIGIT_2);
			m_KeyCodeMap.Insert(KeyCode.KC_3, KEY_DIGIT_3);
			m_KeyCodeMap.Insert(KeyCode.KC_NUMPAD3, KEY_DIGIT_3);
			m_KeyCodeMap.Insert(KeyCode.KC_4, KEY_DIGIT_4);
			m_KeyCodeMap.Insert(KeyCode.KC_NUMPAD4, KEY_DIGIT_4);
			m_KeyCodeMap.Insert(KeyCode.KC_5, KEY_DIGIT_5);
			m_KeyCodeMap.Insert(KeyCode.KC_NUMPAD5, KEY_DIGIT_5);
			m_KeyCodeMap.Insert(KeyCode.KC_6, KEY_DIGIT_6);
			m_KeyCodeMap.Insert(KeyCode.KC_NUMPAD6, KEY_DIGIT_6);
			m_KeyCodeMap.Insert(KeyCode.KC_7, KEY_DIGIT_7);
			m_KeyCodeMap.Insert(KeyCode.KC_NUMPAD7, KEY_DIGIT_7);
			m_KeyCodeMap.Insert(KeyCode.KC_8, KEY_DIGIT_8);
			m_KeyCodeMap.Insert(KeyCode.KC_NUMPAD8, KEY_DIGIT_8);
			m_KeyCodeMap.Insert(KeyCode.KC_9, KEY_DIGIT_9);
			m_KeyCodeMap.Insert(KeyCode.KC_NUMPAD9, KEY_DIGIT_9);
			m_KeyCodeMap.Insert(KeyCode.KC_BACK, KEY_DIGIT_CLEAR);
			m_KeyCodeMap.Insert(KeyCode.KC_0, KEY_DIGIT_0);
			m_KeyCodeMap.Insert(KeyCode.KC_NUMPAD0, KEY_DIGIT_0);
			m_KeyCodeMap.Insert(KeyCode.KC_NUMPADENTER, KEY_DIGIT_CONFIRM);
			m_KeyCodeMap.Insert(KeyCode.KC_RETURN, KEY_DIGIT_CONFIRM);
		}
	#endif
	}
	
	void ~DigitalCodeLockUI()
	{
		m_EnteredPIN = "";
		m_TargetLock = null;
	}
	
	void InitInputWrapperData()
	{
		int namesCount = m_InvInputNames.Count();
		m_InvInputWrappers = new array<UAIDWrapper>();
		m_InvInputTimes = new array<float>;
		
		foreach (string inputName : m_InvInputNames)
		{
			m_InvInputWrappers.Insert(GetUApi().GetInputByName(inputName).GetPersistentWrapper());
			m_InvInputTimes.Insert(0);
		}
		
		m_InvInputWrappersCount = m_InvInputWrappers.Count();
	}
	
	override void SetTargetLock(CodeLockComponent lock)
	{
		if (m_TargetLock || !lock)
			return;
		
		m_TargetLock = lock.GetParent();
		m_TargetPosition = lock.GetPosition();
		m_HasPIN = lock.HasPIN();
		
		#ifdef CODELOCKCOMPONENT_LOG
		ErrorEx("Set lock=" + m_TargetLock, ErrorExSeverity.INFO);
		ErrorEx("Lock position=" + m_TargetPosition, ErrorExSeverity.INFO);
		ErrorEx("Has code=" + m_HasPIN, ErrorExSeverity.INFO);
		#endif
	}	
	
	protected string GetLayout()
	{
		return "gui/layouts/day_z_digital_lock.layout";
	}
	
	override Widget Init()
	{
		layoutRoot = GetGame().GetWorkspace().CreateWidgets(GetLayout());
		
		m_ConsoleToolbar = layoutRoot.FindWidget("ConsoleToolbar");
		m_ConsoleToolbarRichText = RichTextWidget.Cast(layoutRoot.FindAnyWidget("ContextToolbarText"));
		
		InitInputWrapperData();
		InitializeButtons();
		
		#ifdef PLATFORM_CONSOLE
		GetGame().GetMission().GetOnInputDeviceChanged().Insert(OnInputDeviceChanged);
		#else
		m_ConsoleToolbar.Show(false);
		#endif
		
		return layoutRoot;
	}
	
	void InitializeButtons()
	{
		// Store references to all number buttons in order (1-9, C, 0, Enter)
		m_NumButtons = new array<ButtonWidget>;
		
		// Add all buttons in the layout to the array
		m_NumButtons.Insert(ButtonWidget.Cast(layoutRoot.FindAnyWidget("KeyNumber1_Button")));
		m_NumButtons.Insert(ButtonWidget.Cast(layoutRoot.FindAnyWidget("KeyNumber2_Button")));
		m_NumButtons.Insert(ButtonWidget.Cast(layoutRoot.FindAnyWidget("KeyNumber3_Button")));
		m_NumButtons.Insert(ButtonWidget.Cast(layoutRoot.FindAnyWidget("KeyNumber4_Button")));
		m_NumButtons.Insert(ButtonWidget.Cast(layoutRoot.FindAnyWidget("KeyNumber5_Button")));
		m_NumButtons.Insert(ButtonWidget.Cast(layoutRoot.FindAnyWidget("KeyNumber6_Button")));
		m_NumButtons.Insert(ButtonWidget.Cast(layoutRoot.FindAnyWidget("KeyNumber7_Button")));
		m_NumButtons.Insert(ButtonWidget.Cast(layoutRoot.FindAnyWidget("KeyNumber8_Button")));
		m_NumButtons.Insert(ButtonWidget.Cast(layoutRoot.FindAnyWidget("KeyNumber9_Button")));
		m_NumButtons.Insert(ButtonWidget.Cast(layoutRoot.FindAnyWidget("KeyClear_Button")));
		m_NumButtons.Insert(ButtonWidget.Cast(layoutRoot.FindAnyWidget("KeyNumber0_Button")));
		m_NumButtons.Insert(ButtonWidget.Cast(layoutRoot.FindAnyWidget("KeyLock_Button")));
	}
			
	protected void ClearInput()
	{
		m_EnteredPIN = "";
		for (int i = 0; i < MAX_LOCK_PIN_LENGTH; ++i)
		{
			TextWidget digitTextW = TextWidget.Cast(layoutRoot.FindAnyWidget("PasswordDigitNumber_Text" + i));
			if (digitTextW)
				digitTextW.SetText("");
		}
	}
	
	protected void Select()
	{
		int i;
		TextWidget digitTextW;
		
		if (m_SelectedButtonIndex >= 0 && m_SelectedButtonIndex < m_NumButtons.Count())
		{
			Param1<int> uiAction = new Param1<int>(0);
			if (m_SelectedButtonIndex == CONFIRM_BUTTON_INDEX)		// Confirm
			{
				uiAction.param1 = CodeLockUIAction.CONFIRM_INPUT;
				g_Game.RPCSingleParam(m_TargetLock, ERPCs.RPC_CODE_LOCK_UI_ACTION, uiAction, true);
				ConfirmCode();
			}
			else if (m_SelectedButtonIndex == RESET_BUTTON_INDEX) 	// Reset
			{
				uiAction.param1 = CodeLockUIAction.CLEAR_INPUT;
				g_Game.RPCSingleParam(m_TargetLock, ERPCs.RPC_CODE_LOCK_UI_ACTION, uiAction, true);
				ClearInput();
			}
			else									// Select digit
			{
				uiAction.param1 = CodeLockUIAction.DIGIT_INPUT;
				g_Game.RPCSingleParam(m_TargetLock, ERPCs.RPC_CODE_LOCK_UI_ACTION, uiAction, true);
				
				string codeText;
				if (m_EnteredPIN == "")
				{
					codeText = "";
				}
				else
				{
					codeText = m_EnteredPIN;
				}
								
				int codeLength = codeText.Length();
				if (codeLength < MAX_LOCK_PIN_LENGTH)
				{
					int digit;
					if (m_SelectedButtonIndex <= 9)
						digit = m_SelectedButtonIndex + 1;
					else if (m_SelectedButtonIndex == 10)
						digit = 0;
					
					codeText = codeText + digit.ToString();
					m_EnteredPIN = codeText;
					
					#ifdef CODELOCKCOMPONENT_LOG
					ErrorEx("Code=" + m_EnteredPIN, ErrorExSeverity.INFO);
					#endif
					
					string digitS = m_EnteredPIN[0];
					
					for (i = 0; i < m_EnteredPIN.Length(); ++i)
					{										
						digitTextW = TextWidget.Cast(layoutRoot.FindAnyWidget("PasswordDigitNumber_Text" + i));
						if (digitTextW)
							digitTextW.SetText(m_EnteredPIN[i]);
					}
				}
			}
		}
	}
	
	protected void ConfirmCode()
	{
		if (m_EnteredPIN == "" || !m_TargetLock)
			return;
		
		int pinLength = m_EnteredPIN.Length();
		int minPINLength = MIN_LOCK_PIN_LENGTH;
		
		DigitalCodeLock codeLock;
		if (Class.CastTo(codeLock, m_TargetLock))
			minPINLength = codeLock.GetMinLockPINLength();
		
		if (pinLength < minPINLength || pinLength > MAX_LOCK_PIN_LENGTH)
			return;
		
		#ifdef CODELOCKCOMPONENT_LOG
		ErrorEx("PIN set to=" + m_EnteredPIN, ErrorExSeverity.INFO);
		#endif
		
		for (int i = 0; i < MAX_LOCK_PIN_LENGTH - 1; ++i)
		{
			TextWidget digitTextW = TextWidget.Cast(layoutRoot.FindAnyWidget("PasswordDigitNumber_Text" + i));
			if (digitTextW)
				digitTextW.SetText("");
		}
		
		Param1<string> pin = new Param1<string>(m_EnteredPIN);
		g_Game.RPCSingleParam(m_TargetLock, ERPCs.RPC_CODE_LOCK_REQUEST, pin, true);
		m_EnteredPIN = "";
	}
	
	override void CodeResponse(bool response) 
	{
		if (m_HasPIN)
		{
			if (response)
			{
				// Lock has code and dialed code was correct. CLOSE UI
				Close();
			}
			else
			{			
				// Lock has code and dialed code was incorrect. CLOSE UI
				Close();
			}
		}
		else
		{
			if (response)
			{
				// Lock has no code and dialed code was accepted. CLOSE UI
				Close();
			}
		}
	}
	
	protected void ControllerMovementButtonTickHandler(float timeslice)
	{
		float tickvalue = 0;
		
		if (m_SensitivityThreshold > 0.0)
		{
			if (m_SensitivityThreshold < BT_REPEAT_TIME) //sensitivity solution...
			{
				m_SensitivityThreshold += timeslice;
				return;
			}
			else
			{
				m_SensitivityThreshold = 0.0;
			}
		}
		
		for (int i = 0; i < m_InvInputWrappersCount; i++)
		{
			m_InvUAInput = m_InvInputWrappers[i].InputP();
			
			if (m_InvUAInput.LocalValue() > BT_THRESHOLD_VALUE)
			{
				tickvalue = m_InvInputTimes[i];
				tickvalue += timeslice;
			}
			else
			{
				tickvalue = 0.0;
			}
			
			if (tickvalue < BT_REPEAT_DELAY && m_InvUAInput.LocalPress())
			{
				m_InvInputActive |= (1 << i);
				m_SensitivityThreshold += timeslice;
			}
			else if (tickvalue > (BT_REPEAT_DELAY + BT_REPEAT_TIME))
			{
				while (tickvalue > (BT_REPEAT_DELAY + BT_REPEAT_TIME))
				{
					tickvalue -= BT_REPEAT_TIME;
				}
				
				m_InvInputActive |= (1 << i);
				m_SensitivityThreshold += timeslice;
			}
			else
			{
				m_InvInputActive &= ~(1 << i);
			}
			
			m_InvInputTimes[i] = tickvalue;
		}
	}
	
	override void Update(float timeslice)
	{
		super.Update(timeslice);
		
		// Close menu if player reaches certain distance to lock
		if (m_TargetLock)
		{
			Man player = g_Game.GetPlayer();
			vector playerPos = player.GetPosition();
			
			if (vector.DistanceSq(playerPos, m_TargetPosition) > 5.0 * 5.0)
			{
				Close();
				return;
			}
		}
		
		Input input = GetGame().GetInput();
		if (input.LocalPress("UAUIBack", false))
		{
			Close();
			return;
		}
				
		#ifdef PLATFORM_CONSOLE		
		CheckControllerInputs(input, timeslice);
		#endif
	}
	
	protected void CheckControllerInputs(Input input, float timeslice)
	{
		if (input.LocalPress("UAUICtrlX", false))
		{
			ConfirmCode();
			return;
		}
		else if (input.LocalPress("UAUICtrlY", false))
		{
			ClearInput();
			return;
		}
		
		// lock ui movement
		ControllerMovementButtonTickHandler(timeslice);
		
		if (m_InvInputActive & (1 << MOVE_RIGHT))
		{
			NavigateRight();
		}
		
		if (m_InvInputActive & (1 << MOVE_LEFT))
		{
			NavigateLeft();
		}
		
		if (m_InvInputActive & (1 << MOVE_UP))
		{
			NavigateUp();
		}
		
		if (m_InvInputActive & (1 << MOVE_DOWN))
		{
			NavigateDown();
		}
		
		m_InvInputActive = 0;
	}
	
	protected void NavigateRight()
	{
		int row = m_SelectedButtonIndex / 3;
		int col = m_SelectedButtonIndex % 3;
		
		// Move right (if not at right edge)
		if (col < 2)
		{
			m_SelectedButtonIndex = row * 3 + (col + 1);
			UpdateFocus();
		}
	}
    
	protected void NavigateLeft()
	{
		int row = m_SelectedButtonIndex / 3;
		int col = m_SelectedButtonIndex % 3;
		
		// Move left (if not at left edge)
		if (col > 0)
		{
			m_SelectedButtonIndex = row * 3 + (col - 1);
			UpdateFocus();
	    }
	}
    
	protected void NavigateUp()
	{
		int row = m_SelectedButtonIndex / 3;
		int col = m_SelectedButtonIndex % 3;
		
		// Move up (if not at top edge)
		if (row > 0)
		{
			m_SelectedButtonIndex = (row - 1) * 3 + col;
			UpdateFocus();
		}
	}
    
	protected void NavigateDown()
	{
		int row = m_SelectedButtonIndex / 3;
		int col = m_SelectedButtonIndex % 3;
		
		// Move down (if not at bottom edge)
		if (row < 3)
		{
			m_SelectedButtonIndex = (row + 1) * 3 + col;
			UpdateFocus();
	    }
	}
	
	protected void UpdateFocus()
	{
		if (m_SelectedButtonIndex >= 0 && m_SelectedButtonIndex < m_NumButtons.Count())
		{
			// Position highlight over selected button
			ButtonWidget selectedBtn = m_NumButtons[m_SelectedButtonIndex];
			if (selectedBtn)
			{
				SetFocus(selectedBtn);
			}
		}
	}
	
	void SetActive(bool active)
	{
		m_IsActive = active;
	
		if (active)
		{
			m_SelectedButtonIndex = -1;
		}
	}
		
	// Console toolbar
	protected void UpdateConsoleToolbar()
	{
		string select = string.Format(" %1", InputUtils.GetRichtextButtonIconFromInputAction("UAUISelect", "Select", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
		string confirm = string.Format(" %1", InputUtils.GetRichtextButtonIconFromInputAction("UAUICtrlX", "Confirm PIN", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
		string clear = string.Format(" %1", InputUtils.GetRichtextButtonIconFromInputAction("UAUICtrlY", "Clear PIN", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
		string close = string.Format(" %1", InputUtils.GetRichtextButtonIconFromInputAction("UAUIBack", "Exit", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
		
		string navigateIcon;
		#ifdef PLATFORM_PS4
		navigateIcon = string.Format("<image set=\"%1\" name=\"%2\" scale=\"%3\" />", "playstation_buttons2", "DPAD_all_directions");
		#else
		navigateIcon = string.Format("<image set=\"%1\" name=\"%2\" scale=\"%3\" />", "xbox_buttons2", "DPAD_all_directions");
		#endif
		string navigate = string.Format(" %1 %2", navigateIcon, "Navigate");
		string text = string.Format("%1 %2 %3 %4 %5", navigate, select, confirm, clear, close);
		m_ConsoleToolbarRichText.SetText(text);
	}
	
	protected void OnInputDeviceChanged(EInputDeviceType pInputDeviceType)
	{
		int i;
		ButtonWidget button;
		MissionGameplay mission = MissionGameplay.Cast(g_Game.GetMission());
		
		switch (pInputDeviceType)
		{
			case EInputDeviceType.CONTROLLER:
			{
				m_ConsoleToolbar.Show(true);
				
				if (mission)
					mission.MoveHudForInventory(true);
				
				g_Game.GetUIManager().ShowUICursor(false);
								
				button = ButtonWidget.Cast(GetFocus());
				if (button)
					button.SetState(false);
				
				SetFocus(m_NumButtons[0]);
				
				UpdateConsoleToolbar();
				break;
			}
			default:
			{
				if (g_Game.GetInput().IsEnabledMouseAndKeyboardEvenOnServer())
				{
					m_ConsoleToolbar.Show(false);
					
					if (mission)
						mission.MoveHudForInventory(false);
					
					g_Game.GetUIManager().ShowUICursor(true);
										
					button = ButtonWidget.Cast(GetFocus());
					if (button)
						button.SetState(false);
					
					SetFocus(m_NumButtons[0]);
				}
				break;
			}
		}
	}
	
	override void OnShow()
	{
		super.OnShow();
		
		MissionGameplay mission = MissionGameplay.Cast(g_Game.GetMission());
		if (!mission)
			return;
		
		mission.AddActiveInputExcludes(m_InputExcludes);
		
		#ifdef PLATFORM_CONSOLE
		Widget w = m_NumButtons[0];
		if (w)
			SetFocus(w);
		
		mission.MoveHudForInventory(true);
		
		OnInputDeviceChanged(g_Game.GetInput().GetCurrentInputDevice());
		#endif
	}
		
	override void OnHide()
	{
		super.OnHide();
		
		MissionGameplay mission = MissionGameplay.Cast(g_Game.GetMission());
		if (!mission)
			return;
		
		mission.RemoveActiveInputExcludes(m_InputExcludes);
		#ifdef PLATFORM_CONSOLE
		mission.MoveHudForInventory(false);
		#endif
		
		ClearInput();
	}
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		super.OnClick(w, x, y, button);
		
		for (int i = 0; i < m_NumButtons.Count(); i++)
		{
			ButtonWidget bw = m_NumButtons[i];
			if (w == bw)
			{
				m_SelectedButtonIndex = i;
				Select();
				return true;
			}
		}
		
		return false;
	}

#ifndef PLATFORM_CONSOLE
	override void KeyPress(int key)
	{
		super.KeyPress(key);	
		
		int buttonIndex = -1;
		if (m_KeyCodeMap.Find(key, buttonIndex) && buttonIndex > -1)
		{
			m_SelectedButtonIndex = buttonIndex;
			Select();
		}
	}
#endif
}
