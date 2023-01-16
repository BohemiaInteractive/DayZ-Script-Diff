
enum EInputDeviceType
{
	UNKNOWN,
	MOUSE_AND_KEYBOARD,
	CONTROLLER
};

//-----------------------------------------------------------------------------
class Input
{
	// input locking 
	
	/**
	\brief Change game focus number
	@param add number to add to focus number
	@param input_device if equals -1, works globally on all devices, see INPUT_DEVICE_* values in constants.h 
	@see HasGameFocus()
	*/
	proto native void		ChangeGameFocus(int add, int input_device = -1);
	
	/**
	\brief Reset game focus number (set to 0)
	@param input_device if equals -1, works globally on all devices, see INPUT_DEVICE_* values in constants.h 
	@see HasGameFocus()
	*/
	proto native void		ResetGameFocus(int input_device = -1);
	
	/**
	\brief Check if game should have focus
	@param input_device if equals -1, checks all devices, see INPUT_DEVICE_* values in constants.h 
	@return true if focus number == 0, false otherwise
	*/
	proto native bool		HasGameFocus(int input_device = -1);

	// actions
	proto native int		GetActionGroupsCount();
	proto native int		GetActionGroupSize(int group_index);
	proto int				GetActionGroupName(int group_index, out string name);
	proto int				GetActionDesc(int action_index, out string desc);

	// getting action state
	/**
	\brief Get action state
	@param action id of action, defined in \ref 4_World/Classes/UserActionsComponent/_constants.c
	@param check_focus if true and game is unfocused, returns 0; otherwise returns actual value
	@return actual action state as float, for regular two state buttons returns 0 or 1, for analog buttons/axes returns value in interval <0, 1> with consideration of defined deadzones
	@see LocalValue()
	*/
	proto native float	LocalValue_ID(int action, bool check_focus = true);
	proto native float	LocalValue(string action, bool check_focus = true);
		
	/**  
	\brief Returns true just in frame, when action was invoked (button was pressed)
	\note if the input is limited (click, hold, doubleclick), 'Press' event is limited as well, and reacts to the limiter only! Otherwise it registeres the first event, usually 'press' (change of value from 0)
	@param action id of action, defined in \ref 4_World/Classes/UserActionsComponent/_constants.c
	@param check_focus if true and game is unfocused, returns 0; otherwise returns actual value
	@return true if action was invoked in that frame, false otherwise
	@see LocalPress()
	*/
	proto native bool	LocalPress_ID(int action, bool check_focus = true);
	proto native bool	LocalPress(string action, bool check_focus = true);
	
	/**  
	\brief Returns true just in frame, when release action happened (button was released)
	@param action id of action, defined in \ref 4_World/Classes/UserActionsComponent/_constants.c
	@param check_focus if true and game is unfocused, returns 0; otherwise returns actual value
	@return true if action was released in that frame, false otherwise
	@see LocalRelease()
	*/
	proto native bool	LocalRelease_ID(int action, bool check_focus = true);
	proto native bool	LocalRelease(string action, bool check_focus = true);

	/**  
	\brief Returns true just in frame, when hold action invoked (button is hold)
	@param action id of action, defined in \ref 4_World/Classes/UserActionsComponent/_constants.c
	@param check_focus if true and game is unfocused, returns 0; otherwise returns actual value
	@return true if action was released in that frame, false otherwise
	@see LocalHold()
	*/
	proto native bool	LocalHold_ID(int action, bool check_focus = true);
	proto native bool	LocalHold(string action, bool check_focus = true);

	/**  
	\brief Returns true just in frame, when double click action invoked (button double clicked)
	@param action id of action, defined in \ref 4_World/Classes/UserActionsComponent/_constants.c
	@param check_focus if true and game is unfocused, returns 0; otherwise returns actual value
	@return true if action was released in that frame, false otherwise
	@see LocalDbl()
	*/
	proto native bool	LocalDbl_ID(int action, bool check_focus = true);
	proto native bool	LocalDbl(string action, bool check_focus = true);

	/**  
	\brief Disable key until end of frame
	@param key id of key, defined in \ref KeyCode
	\code
	GetGame().GetInput().DisableKey(KeyCode.KC_RETURN);
	\endcode
	*/
	proto native void	DisableKey(int key);
	
	//! Enable mouse and keyboard (on consoles)
	proto native void	EnableMouseAndKeyboard(bool enable);
	//! @return state of support mouse and keyboard (on consoles)
	proto native bool	IsEnabledMouseAndKeyboard();
	/*!
	@return state of support mouse and keyboard. If client playing on server 
	where mouse and keyboard is disabled, then return false. (on consoles)
	*/
	proto native bool	IsEnabledMouseAndKeyboardEvenOnServer();
	
	/*!
	@return Console: Last state queried from the platform operating system for the active gamepad. 
			PC: Always true.
	*/
	proto native bool	IsMouseConnected();
	proto native bool	IsKeyboardConnected();

	//! gets currently selected profile
	proto native int	GetCurrentProfile();
	// gets currently selected profile keys for action
	proto native void	GetCurrentProfileActionKeys(int action_index, out TIntArray keys);
	//! gets profile by index
	proto int			GetProfileName(int profile_index, out string name);
	//! gets profile by name
	proto native int	GetProfilesCount();
	//! setting active profile
	proto native int	SetProfile(int index);


	// devices - joystick only!
	proto native int		GetDevicesCount();
	proto int				GetDeviceName(int device_index, out string name);
	proto native int		IsDeviceXInput(int device_index);
	proto native int		IsDeviceEnabled(int device_index);
	proto native void		SetDeviceEnabled(int device_index, bool enabled);
	
	//! return true if was deflected button.
	proto bool				GetGamepadThumbDirection(GamepadButton thumbButton, out float angle, out float value);
	
	//! clears active gamepad
	proto native void		ResetActiveGamepad();
	proto native void		SelectActiveGamepad(int gamepad);
	proto native void		GetGamepadList(out array<int> gamepads);
	proto void				GetGamepadUser(int gamepad, out BiosUser user);
	/**  
	\brief the on OnGamepadIdentification callback will return the first gamepad where the button was pressed
	@param button the button that needs to be pressed for the identification
	*/
	proto native void		IdentifyGamepad(GamepadButton button);
	//! returns true if there is an active gamepad selected.
	proto native bool		IsActiveGamepadSelected();

	//! returns true if 'Gamepad' or 'Mouse and Keyboard' is connected
	bool IsAnyInputDeviceActive()
	{
		return IsActiveGamepadSelected() || IsMouseConnected() || IsKeyboardConnected();
	}
	
	/**  
	\brief returns true if 'Gamepad' or if 'Mouse/Keyboard' control is allowed locally and on server, and the respective input devicse are connected. Gamepad takes priority.
	@param unavailableDeviceList lists all devices that SHOULD be available, but aren't. Optional.
	*/
	bool AreAllAllowedInputDevicesActive(out array<int> unavailableDeviceList = null)
	{
		bool passed = true;
		bool gamepad = IsActiveGamepadSelected();
		bool mouse = IsMouseConnected();
		bool keyboard = IsKeyboardConnected();
		bool MnKEnabled;
		
		if (g_Game.GetGameState() != DayZGameState.IN_GAME)
		{
			MnKEnabled = IsEnabledMouseAndKeyboard();
		}
		else if (g_Game.GetGameState() != DayZGameState.MAIN_MENU)
		{
			MnKEnabled = IsEnabledMouseAndKeyboardEvenOnServer();
		}
		else
		{
			return true;
		}
		
		if (!MnKEnabled)
		{
			if (!gamepad)
			{
				passed = false;
				FillUnavailableDeviceArray(EUAINPUT_DEVICE_CONTROLLER,unavailableDeviceList);
			}
		}
		else
		{
			if (!gamepad)
			{
				if (!mouse)
				{
					passed = false;
					FillUnavailableDeviceArray(EUAINPUT_DEVICE_MOUSE,unavailableDeviceList);
				}
				if (!keyboard)
				{
					passed = false;
					FillUnavailableDeviceArray(EUAINPUT_DEVICE_KEYBOARD,unavailableDeviceList);
				}
				
				if (!passed)
				{
					FillUnavailableDeviceArray(EUAINPUT_DEVICE_CONTROLLER,unavailableDeviceList);
				}
			}
		}
		return passed;
	}
	
	void FillUnavailableDeviceArray(int device, inout array<int> filler)
	{
		if (filler)
		{
			filler.Insert(device);
		}
	}
	
	//! currently lists only available Gamepad, Mouse, and Keyboard. Extendable as needed.
	void UpdateConnectedInputDeviceList()
	{
		g_Game.GetConnectedInputDeviceList().Clear();
		
		if (IsActiveGamepadSelected())
			g_Game.GetConnectedInputDeviceList().Insert(EUAINPUT_DEVICE_CONTROLLER);
		if (IsMouseConnected())
			g_Game.GetConnectedInputDeviceList().Insert(EUAINPUT_DEVICE_MOUSE);
		if (IsKeyboardConnected())
			g_Game.GetConnectedInputDeviceList().Insert(EUAINPUT_DEVICE_KEYBOARD);
	}
	
	/**
	@return Input device, with the last input event ('mouse and keyboard', 'controller' or
	'unknown' if none input event was fired from the beginning).
	*/
	proto native EInputDeviceType	GetCurrentInputDevice();
	
	/**
	\note For PlayStation, Enter button in Asia territory is typically Circle button (B button),
	but in Europe and America it is Cross button (A button).
	\return Button, which represent Enter/Accept button.
	*/
	proto native GamepadButton		GetEnterButton();
	
	//! callback that is fired when a new gamepad is connected
	void OnGamepadConnected(int gamepad)
	{
		#ifdef PLATFORM_PS4
		BiosUser user;
		GetGamepadUser( gamepad, user );
		if (user && user == GetGame().GetUserManager().GetSelectedUser())
		{
			SelectActiveGamepad(gamepad);
			if (GetGame().GetMission())
				GetGame().GetMission().GetOnInputDeviceConnected().Invoke(EUAINPUT_DEVICE_CONTROLLER); //only for PS, xbox handles it on identification
		}
		#endif
		
		#ifdef PLATFORM_XBOX
		if (gamepad == g_Game.GetPreviousGamepad())
		{
			SelectActiveGamepad(g_Game.GetPreviousGamepad());
			if (GetGame().GetMission())
				GetGame().GetMission().GetOnInputDeviceConnected().Invoke(EUAINPUT_DEVICE_CONTROLLER); //only for PS, xbox handles it on identification
		}
		#endif
	}
	
	//! callback that is fired when gamepad is disconnected
	void OnGamepadDisconnected(int gamepad)
	{
		if (IsInactiveGamepadOrUserSelected(gamepad))
		{
			UpdateConnectedInputDeviceList();
			
			if (!g_Game.IsLoading())
			{
				DayZLoadState state = g_Game.GetLoadState();
				if (state != DayZLoadState.MAIN_MENU_START && state != DayZLoadState.MAIN_MENU_USER_SELECT)
				{
					if (GetGame().GetMission())
						GetGame().GetMission().GetOnInputDeviceDisconnected().Invoke(EUAINPUT_DEVICE_CONTROLLER);
				}
			}
		}
	}

	//! callback that is fired when identification was requested
	void OnGamepadIdentification(int gamepad)
	{
		if (gamepad > -1)
		{
			DayZLoadState state = g_Game.GetLoadState();
			
			UpdateConnectedInputDeviceList();
			SelectActiveGamepad(gamepad);
			g_Game.SelectUser(gamepad);
			g_Game.SetPreviousGamepad(gamepad);
			if (state == DayZLoadState.MAIN_MENU_START || state == DayZLoadState.MAIN_MENU_USER_SELECT)
			{
				if (GetGame().GetMission())
					GetGame().GetMission().Reset();
			}
			
			if (GetGame() && GetGame().GetMission() && GetGame().GetMission().GetOnInputDeviceConnected())
				GetGame().GetMission().GetOnInputDeviceConnected().Invoke(EUAINPUT_DEVICE_CONTROLLER);
		}
	}
	
	int GetUserGamepad( BiosUser user )
	{
		array<int> gamepads = new array<int>;
		GetGamepadList( gamepads );
		for( int i = 0; i < gamepads.Count(); i++ )
		{
			BiosUser user2;
			GetGamepadUser( gamepads[i], user2 );
			if( user == user2 )
				return gamepads[i];
		}
		return -1;
	}
	
	bool IsInactiveGamepadOrUserSelected( int gamepad = -1 )
	{
		#ifdef PLATFORM_XBOX
		return !IsActiveGamepadSelected();
		#endif
		#ifdef PLATFORM_PS4
		BiosUser user;
		GetGamepadUser( gamepad, user );
		return (user == GetGame().GetUserManager().GetSelectedUser());
		#endif
		return false;
	}
	
	//! callback that is fired when mouse is connected (PS: and assigned to the user)
	//! does not fire on PC - mouse/keyboard assumed to always be connected
	void OnMouseConnected()
	{
		UpdateConnectedInputDeviceList();
		if (!g_Game.IsLoading() && GetGame().GetMission())
		{
			DayZLoadState state = g_Game.GetLoadState();
			if (state != DayZLoadState.MAIN_MENU_START && state != DayZLoadState.MAIN_MENU_USER_SELECT)
			{
				GetGame().GetMission().GetOnInputDeviceConnected().Invoke(EUAINPUT_DEVICE_MOUSE);
			}
		}
	}

	//! callback that is fired when mouse is disconnected
	//! does not fire on PC - mouse/keyboard assumed to always be connected
	void OnMouseDisconnected()
	{
		UpdateConnectedInputDeviceList();
		if (!g_Game.IsLoading() && GetGame().GetMission())
		{
			DayZLoadState state = g_Game.GetLoadState();
			if (state != DayZLoadState.MAIN_MENU_START && state != DayZLoadState.MAIN_MENU_USER_SELECT)
			{
				GetGame().GetMission().GetOnInputDeviceDisconnected().Invoke(EUAINPUT_DEVICE_MOUSE);
			}
		}
	}

	//! callback that is fired when keyboard is connected (PS: and assigned to the user)
	//! does not fire on PC - mouse/keyboard assumed to always be connected
	void OnKeyboardConnected()
	{
		UpdateConnectedInputDeviceList();
		if (!g_Game.IsLoading() && GetGame().GetMission())
		{
			DayZLoadState state = g_Game.GetLoadState();
			if (state != DayZLoadState.MAIN_MENU_START && state != DayZLoadState.MAIN_MENU_USER_SELECT)
			{
				GetGame().GetMission().GetOnInputDeviceConnected().Invoke(EUAINPUT_DEVICE_KEYBOARD);
			}
		}
	}

	//! callback that is fired when keyboard is disconnected
	//! does not fire on PC - mouse/keyboard assumed to always be connected
	void OnKeyboardDisconnected()
	{
		UpdateConnectedInputDeviceList();
		if (!g_Game.IsLoading() && GetGame().GetMission())
		{
			DayZLoadState state = g_Game.GetLoadState();
			if (state != DayZLoadState.MAIN_MENU_START && state != DayZLoadState.MAIN_MENU_USER_SELECT)
			{
				GetGame().GetMission().GetOnInputDeviceDisconnected().Invoke(EUAINPUT_DEVICE_KEYBOARD);
			}
		}
	}
	
	//! called from code on different input device use
	void OnLastInputDeviceChanged(EInputDeviceType inputDevice)
	{
		if (GetGame().GetMission())
		{
			GetGame().GetMission().GetOnInputDeviceChanged().Invoke(inputDevice);
		}
	}
};

