
/** @file */


// constants for device (binding device) type determine:
//
//	EUAINPUT_DEVICE_KEYBOARD
//	EUAINPUT_DEVICE_MOUSE
//	EUAINPUT_DEVICE_KEYBOARDMOUSE
//	EUAINPUT_DEVICE_CONTROLLER
//	EUAINPUT_DEVICE_IR

// -------------------------------------------------------------------------
class UAIDWrapper
{
	private void UAIDWrapper() {}
	private void ~UAIDWrapper() {}
	
	proto native UAInput InputP();			// get input pointer
};

// -------------------------------------------------------------------------
class UAInput
{
	proto native int ID();			// return input index
	proto native int NameHash();	// return input hash

	proto native int BindingCount();	// return binding count
	proto native int Binding( int iIndex );	// return binding at index

	proto native void ClearBinding();	// remove all bindings

	proto native void BindCombo( string sButtonName );		// bind combo to this input by name (single/ or append to existing button)
	proto native void BindComboByHash( int iHash );			// bind combo to this input by hash (single/ or append to existing button)

	proto native void AddAlternative();						// add new alternative keybind
	proto native void ClearAlternative( int iIndex );		// clear binding alternative by index
	proto native void SelectAlternative( int iIndex );		// select binding alternative by index
	proto native int AlternativeCount();					// get currently assigned alternative count
	proto native int AlternativeIndex();					// get currently selected alternative index
	proto native void ClearDeviceBind( int iDeviceFlags );	// clear binding for specific device(s)

	proto native int BindKeyCount(); // binded key count (for selected alternative)
	proto native int GetBindKey( int iIndex ); // binded "control" at index (for selected alternative)
	proto native int GetBindDevice( int iIndex ); // binded "control" device type at index (for selected alternative)
	proto native bool CheckBindDevice( int iIndex, int iDeviceFlags ); // compare binded device "control" type :: EUAINPUT_DEVICE_KEYBOARD for example

	proto native float LocalValue();

	proto native bool LocalPress();
	proto native bool LocalRelease();
	proto native bool LocalHold();
	proto native bool LocalHoldBegin();
	proto native bool LocalDoubleClick();
	proto native bool LocalClick();

	proto native bool IsCombo();	// return true if there is currently combo bind - use Binding() result !!!

	// normal state is there are no limits, input generates all the events
	// but if there is an limiter, 
	proto native bool IsLimited();	// return true if there is an event limit

	proto native bool IsLimitConditionActive();	// return true if event limits matches current control item activity

	proto native bool IsPressLimit();		// if limited to PRESS
	proto native bool IsReleaseLimit();		// if limited to RELEASE
	proto native bool IsHoldLimit();		// if limited to HOLD
	proto native bool IsHoldBeginLimit();	// if limited to HOLDBEGIN
	proto native bool IsClickLimit();		// if limited to SINGLE CLICK
	proto native bool IsDoubleClickLimit();	// if limited to DOUBLE CLICK

	proto native bool HasSorting( int iIndex );		// has sorting group index?

	proto native void Supress();			// supress press event for next frame (while not pressed ATM - otherwise until release)

	proto native bool IsLocked();			// determine if locked (not active ATM)
	proto native void Lock();				// lock (until unlock called or exclusion is selected)
	proto native void Unlock();				// unlock exclusively

	proto native int ConflictCount();		// get number of conflicts with other inputs

	proto native void ForceEnable( bool bEnable );	// force enable on/ off
	proto native void ForceDisable( bool bEnable );	// force disable on/ off

	proto native void Backlit_Override( int eType, int iColor ); // enable/ disable backlit of associated controls (EUABACKLIT_*)
	proto native bool Backlit_Enabled(); // check whether associated controls are backlit

	proto native UAIDWrapper GetPersistentWrapper(); // create persistent object for input access

	private void UAInput();
	private void ~UAInput();
};

// -------------------------------------------------------------------------
class UAInterface
{
	private void UAInterface() {}
	private void ~UAInterface() {}
	
	// getting action state
	/**
	\brief Get action state
	@param action id of action, defined in \ref 4_World/Classes/UserActionsComponent/_constants.c
	@param check_focus if true and game is unfocused, returns 0; otherwise returns actual value
	@return actual action state as float, for regular two state buttons returns 0 or 1, for analog buttons/axes returns value in interval <0, 1> with consideration of defined deadzones
	@see SyncedValue()
	*/
	proto native float	SyncedValue_ID(int action, bool check_focus = true);
	proto native float	SyncedValue(string action, bool check_focus = true);
		
	/**  
	\brief Returns true just in frame, when action was invoked (button was pressed)
	@param action id of action, defined in \ref 4_World/Classes/UserActionsComponent/_constants.c
	@param check_focus if true and game is unfocused, returns 0; otherwise returns actual value
	@return true if action was invoked in that frame, false otherwise
	@see SyncedPress()
	*/
	proto native bool	SyncedPress_ID(int action, bool check_focus = true);
	proto native bool	SyncedPress(string action, bool check_focus = true);
	
	/**  
	\brief Returns true just in frame, when release action happened (button was released)
	@param action id of action, defined in \ref 4_World/Classes/UserActionsComponent/_constants.c
	@param check_focus if true and game is unfocused, returns 0; otherwise returns actual value
	@return true if action was released in that frame, false otherwise
	@see SyncedRelease()
	*/
	proto native bool	SyncedRelease_ID(int action, bool check_focus = true);
	proto native bool	SyncedRelease(string action, bool check_focus = true);

	/**  
	\brief Returns true just in frame, when hold action invoked (button is hold)
	@param action id of action, defined in \ref 4_World/Classes/UserActionsComponent/_constants.c
	@param check_focus if true and game is unfocused, returns 0; otherwise returns actual value
	@return true if action was released in that frame, false otherwise
	@see SyncedHold()
	*/
	proto native bool	SyncedHold_ID(int action, bool check_focus = true);
	proto native bool	SyncedHold(string action, bool check_focus = true);

	/**  
	\brief Returns true just in frame, when double click action invoked (button double clicked)
	@param action id of action, defined in \ref 4_World/Classes/UserActionsComponent/_constants.c
	@param check_focus if true and game is unfocused, returns 0; otherwise returns actual value
	@return true if action was released in that frame, false otherwise
	@see SyncedDbl()
	*/
	proto native bool	SyncedDbl_ID(int action, bool check_focus = true);
	proto native bool	SyncedDbl(string action, bool check_focus = true);
	
	/**  
	\brief Returns true just in frame, when single click action invoked (button pressed and released before hold timer)
	@param action id of action, defined in \ref 4_World/Classes/UserActionsComponent/_constants.c
	@param check_focus if true and game is unfocused, returns 0; otherwise returns actual value
	@return true if action was released in that frame, false otherwise
	@see SyncedClick()
	*/
	proto native bool	SyncedClick_ID(int action, bool check_focus = true);
	proto native bool	SyncedClick(string action, bool check_focus = true);
	
};

// -------------------------------------------------------------------------
class UAInputAPI
{
	private void UAInputAPI() {}
	private void ~UAInputAPI() {}
	
	proto native void ListCurrentProfile();
	proto native void ListCurrentPreset();
	proto native void ListAvailableButtons();
	proto native void ListActiveGroup();

	proto native void GetActiveInputs( out TIntArray items ); //! returns list of all bindable (i.e. visible) inputs from the active group ('core' by default)

	proto native UAInput GetInputByID( int iID );
	proto native UAInput GetInputByName( string sInputName );

	proto native owned string GetButtonName( int iHash );	// get localized name for any button hash
	proto native owned string GetButtonIcon( int iHash );	// get associated icon path for any button hash

	proto native int ModificatorCount();	// modificator count
	proto native owned string GetModificatorName( int index );	// modificator name
	
	// for options only mapping, do not call normally as it is not performance wise!
	proto native int DeterminePressedButton();	// recently pressed key, mouse button or pad button (returns zero if nothing pressed)
	proto native int DeterminedCount(); // all buttons (keys, etc.) pressed during determination test (call only when DeterminePressedButton() returned something !)
	proto native int GetDetermined( int iIndex ); // total count of pressed buttons (call only when DeterminePressedButton() returned something !)

	proto native UAInput RegisterInput( string sInputName, string sLoc, string sGroupName );
	proto native void DeRegisterInput( string sInputName );

	proto native void RegisterGroup( string sGroupName, string sLoc );
	proto native void DeRegisterGroup( string sGroupName );

	proto native void UpdateControls(); // call this on each change of exclusion
	proto native void ActivateGroup( string sGroupName );
	proto native void ActivateExclude( string sExcludeName );
	proto native void ActivateContext( string sContextName );
	proto native void ActivateModificator( string sModName );
	proto native void DeactivateModificator( string sModName );

	proto native void DeactivateContext();

	proto native bool PresetCreateNew(); // create new preset from the selected one - (false == cannot create new == too many presets!)
	proto native bool PresetDelete( int index ); // delete specific preset - (false == not deletable!)
	proto native int PresetCurrent(); // determine index of current preset - (-1 == not selected)
	proto native void PresetSelect( int index ); // select specific preset
	proto native int PresetCount(); // count of presets
	proto native owned string PresetName( int index ); // name of selected preset

	proto native int SortingCount(); // sorting group count
	proto native owned string SortingName( int index );	// sorting group name (different from localization!)
	proto native owned string SortingLocalization( int index );	// sorting group localized name

	proto native void Export(); // export XML (user) configuration
	proto native void Revert(); // revert XML (user) configuration - all of it and use default PBO content!
	proto native void SaveInputPresetMiscData(); // saves preset index on consoles, where the regular XMLs are unavailable
	proto native void SupressNextFrame( bool bForce); // supress inputs for nextframe (until key release - call this when leaving main menu and alike - to avoid button collision after character control returned)
	
	proto native void Backlit_None(); // turn off backlit
	proto native void Backlit_Background( int eType, int iColor1, int iColor2 ); // start backlit
	proto native void Backlit_Animation( string strName, float fLifeTime, int iColor1, int iColor2 ); // start backlit animation and after time limit -> continue with another one
	proto native bool Backlit_Enabled(); // check whether backlit active
	proto native void Backlit_KeyByHash( int uHash, int eLayer, int eType, int iColor ); // key directly (during keybinding process) (EUABACKLIT_*)
	proto native void Backlit_KeyByName( string strName, int eLayer, int eType, int iColor ); // key directly (by name) (EUABACKLIT_*)
	proto native void Backlit_Remove( int eLayer ); // turn off layer of backlit
	proto native void Backlit_ForceUpdate(); // this forces update immediately - USE ONLY IN LOADING SEQUENCE !!!
	proto native void Backlit_EmptyQueue(); // empty queue of animations - use if you want to play something immediately

};

proto native UAInputAPI GetUApi();
