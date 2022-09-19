class ItemOptics extends InventoryItemSuper
{
	bool 				m_data_set;
	bool 				m_allowsDOF; //true if optics DOES NOT have magnification (FOV >= DZPLAYER_CAMERA_FOV_IRONSIGHTS)
	bool 				m_reddot_displayed
	bool 				m_isNVOptic = false;
	int 				m_CurrentOpticMode; //generic optic mode, currently used for NV optics only (could be expanded)
	int 				m_CurrentOpticModeLocal; //local mirror for sync purposes;
	int 				m_reddot_index;
	float 				m_blur_float;
	float 				m_nearplane_override; //override value for DayZPlayerCameraOptics only!
	string 				m_optic_sight_texture;
	string 				m_optic_sight_material;
	string 				m_2D_preload_type;
	ref array<float> 	m_mask_array;
	ref array<float> 	m_lens_array;
	ref array<float> 	m_OpticsDOFProperties;
		
	void ItemOptics()
	{
		m_mask_array = new array<float>;
		m_lens_array = new array<float>;
		m_OpticsDOFProperties = new array<float>;
		
		InitReddotData();
		InitOpticsPPInfo();
		InitCameraOverrideProperties();
		InitOpticsDOFProperties(m_OpticsDOFProperties);
		Init2DPreloadType();
		InitOpticMode();
		
		m_CurrentOpticModeLocal = -1;
		RegisterNetSyncVariableInt( "m_CurrentOpticMode", 0, 63 );
	}
	
	/**@fn		EnterOptics
	 * @brief	switches to optics mode if possible
	 * @return true if success, false otherwise
	 **/
	proto native bool EnterOptics ();
	
	/**@fn		IsInOptics
	 * @brief	is weapon in optics mode or not
	 * @return true if in optics mode, false otherwise
	 **/
	proto native bool IsInOptics ();
	
	/**@fn		ExitOptics
	 * @brief	switches out of optics mode (if possible)
	 * @return true if success, false otherwise
	 **/
	proto native bool ExitOptics ();

	/**@fn		HasWeaponIronsightsOverride
	 * @brief	is weapon in optics mode or not
	 * @return true if optics has defined override optics info for weapon
	 **/
	proto native bool HasWeaponIronsightsOverride ();
	
	/**@fn		UseWeaponIronsightsOverride
	 * @brief	switches into ironsights override settings
	 * @return  true if switching was successful
	 **/
	proto native bool UseWeaponIronsightsOverride(bool state);

	/**@fn		IsUsingWeaponIronsightsOverride
	 * @brief	is optics using ironsights override settings or not
	 * @return  true if optics is using ironsights override settings
	 **/
	proto native bool IsUsingWeaponIronsightsOverride();

		/**@fn		GetStepFOVCount
	 * @brief	returns number of configured steps
	 **/
	proto native int GetStepFOVCount ();
	
	/**@fn		GetStepFOVIndex
	 * @brief	returns index of currently used value in 'discretefov' config array
	 * @return	index of currently used value in 'discretefov' config array
	 **/
	proto native int GetStepFOVIndex ();
	
	/**@fn		SetStepFOVIndex
	 * @brief sets zoom to fov value defined at given in 'discretefov' config array
	 * @param[in] index of configured step, range [0..cfg_max]
	 * @return	true if zoom set
	 **/
	proto native bool SetStepFOVIndex (int step);
	
	/**@fn		StepFOVUp
	 * @brief sets zoom to next defined (respective to current) value in zoom fov config array
	 * @return	true if zoom set
	 **/
	proto native bool StepFOVUp ();
	
	/**@fn		StepFOVDown
	 * @brief	sets zoom to previous (respective to current) defined value in zoom fov config array
	 * @return	true if zoom set
	 **/
	proto native bool StepFOVDown ();

	/**@fn		GetCurrentStepFOV
	 * @brief	returns fov value at current index, or 'OpticsInfo.opticsZoomInit' config value (non-zooming optics)
	 **/
	proto native float GetCurrentStepFOV ();

	/**@fn		GetStepZeroing
	 * @brief	returns position of currently used value in zeroing config array
	 * @return	 position of currently used value in zeroing config array
	 **/
	proto native int GetStepZeroing ();
	
	/**@fn		SetZeroing
	 * @brief sets zeroing to value defined at given position in zeroing config array
	 * @param[in] index of zeroing to set [0..cfg_max]
	 * @return	true if zeroing set
	 **/
	proto native bool SetStepZeroing (int step);
	
	/**
	 * @fn		StepZeroingUp
	 * @brief sets zeroing to next defined (respective to current) value in zeroing config array
	 * @return true if zeroing set
	 **/
	proto native bool StepZeroingUp ();
	
	/**
	 * @fn		StepZeroingDown
	 * @brief	sets zeroing to previous (respective to current) defined value in zeroing config array
	 * @return true if zeroing set
	 **/
	proto native bool StepZeroingDown ();
	
	/**
	 * @fn		GetCameraPoint
	 * @brief	gets camera position & direction in model space of optics entity
	 **/
	proto native void GetCameraPoint (out vector pos, out vector dir);
	
	/**
	 * @fn		GetZoomInit
	 * @brief	gets FOV value, when entering optics
	 **/
	proto native float GetZoomInit();

	/**
	 * @fn		GetZoomMin
	 * @brief	gets FOV minimum
	 **/
	proto native float GetZoomMin();

	/**
	 * @fn		GetZoomMax
	 * @brief	gets FOV maximum
	 **/
	proto native float GetZoomMax();

	/**
	 * @fn		GetZeroingDistanceZoomMin
	 * @brief	Gets Zeroing distance at opticsZoomMin
	 **/
	proto native float GetZeroingDistanceZoomMin();
	
	/**
	 * @fn		GetZeroingDistanceZoomMax
	 * @brief	Gets Zeroing distance at opticsZoomMax
	 **/
	proto native float GetZeroingDistanceZoomMax();


	
	/*override void EEItemAttached(EntityAI item, string slot_name)
	{
		super.EEItemAttached(item, slot_name);
		
		// could maybe just ask for energy component on item?
		if (slot_name == "BatteryD")
		{
			item.GetCompEM().SwitchOn();
		}
	}*/
	
	/*override void EEItemDetached(EntityAI item, string slot_name)
	{
		super.EEItemDetached(item, slot_name);
		
		if (slot_name == "BatteryD")
		{
			item.GetCompEM().SwitchOff();
		}
	}*/
	
	override void OnWorkStart()
	{
		if (!GetGame().IsDedicatedServer())
		{
			ShowReddot(true);
		}
	}
	
	override void OnWorkStop()
	{
		if (!GetGame().IsDedicatedServer())
		{
			ShowReddot(false);
		}
	}
	
	bool IsWorking()
	{
		if (GetCompEM() && GetCompEM().CanWork())
			return true;
		return false;
	}
	
	void UpdateOpticsReddotVisibility()
	{
		if (IsWorking() && !m_reddot_displayed)
		{
			ShowReddot(true);
		}
		else if (!IsWorking() && m_reddot_displayed)
		{
			ShowReddot(false);
		}
	}
	
	override void OnWasAttached( EntityAI parent, int slot_id )
	{
		super.OnWasAttached(parent, slot_id);
		
		SetTakeable(false);
	}

	override void OnWasDetached( EntityAI parent, int slot_id )
	{
		super.OnWasDetached(parent, slot_id);
		
		PlayerBase player;
		if (PlayerBase.CastTo(player, GetHierarchyRootPlayer()))
		{
			player.SetReturnToOptics(false);
		}
		
		SetTakeable(true);
	}
	
	override void OnInventoryExit(Man player)
	{
		super.OnInventoryExit(player);
		
		PlayerBase playerPB;
		if (PlayerBase.CastTo(playerPB, player))
		{
			playerPB.SetReturnToOptics(false);
		}
		
		SetTakeable(true);
	}
	
	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);
		
		ctx.Write(m_CurrentOpticMode);
	}
	
	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if ( !super.OnStoreLoad(ctx,version) )
		{
			return false;
		}
		m_IsStoreLoad = true;
		
		if ( version >= 126 )
		{
			if ( !ctx.Read(m_CurrentOpticMode) )
			{
				m_IsStoreLoad = false;
				return false;
			}
		}
		
		OnOpticModeChange();
		SetSynchDirty();
		
		m_IsStoreLoad = false;
		return true;
	}
	
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		
		if (m_CurrentOpticModeLocal != m_CurrentOpticMode)
		{
			OnOpticModeChange();
			m_CurrentOpticModeLocal = m_CurrentOpticMode;
		}
	} 
	
	void InitReddotData()
	{
		string path = "cfgVehicles " + GetType() + " OpticsInfo";
		string temp;
		
		if (GetGame().ConfigIsExisting(path))
		{
			m_reddot_index = GetHiddenSelectionIndex("reddot");
			if (GetGame().ConfigIsExisting(path + " opticSightTexture"))
			{
				GetGame().ConfigGetText(path + " opticSightTexture", temp);
				m_optic_sight_texture = temp;
				temp = "";
			}
			if (GetGame().ConfigIsExisting(path + " opticSightMaterial"))
			{
				GetGame().ConfigGetText(path + " opticSightMaterial", temp);
				m_optic_sight_material = temp;
				temp = "";
			}
		}
		m_data_set = true;
	}
	
	void ShowReddot(bool state)
	{
		if (GetGame().IsDedicatedServer())
		{
			ErrorEx("should not be called on the server!",ErrorExSeverity.INFO);
			return;
		}
		
		if (!m_data_set)
		{
			InitReddotData();
		}
		
		// does not have reddot
		if (m_reddot_index == -1)
		{
			return;
		}
		
		if (state)
		{
			if (m_optic_sight_texture != "")
				SetObjectTexture(m_reddot_index, m_optic_sight_texture);
			if (m_optic_sight_material != "")
				SetObjectMaterial(m_reddot_index, m_optic_sight_material);
		}
		else
		{
			SetObjectTexture(m_reddot_index, "");
			SetObjectMaterial(m_reddot_index, "");
		}
		m_reddot_displayed = state;
	}
	
	void InitOpticsPPInfo()
	{
		m_allowsDOF = InitDOFAvailability();
		InitOpticsPP(m_mask_array, m_lens_array, m_blur_float);
		
		m_isNVOptic = ConfigGetBool("NVOptic");
	}
	
	//! optics with more than 1x zoom do not allow DOF changes
	bool InitDOFAvailability()
	{
		float fov_max;
		string path = "cfgVehicles " + GetType() + " OpticsInfo";
		
		/*
		Weapon_Base weapon = Weapon_Base.Cast(GetHierarchyParent());
		if (!weapon)
			return false; // no DOF for handheld optics
		*/
		fov_max = GetGame().ConfigGetFloat(path + " opticsZoomMax");
		if (fov_max >= GameConstants.DZPLAYER_CAMERA_FOV_IRONSIGHTS)
		{
			return true;
		}
		return false;
	}
	
	//! initializes values for optics' post-processes
	void InitOpticsPP(out array<float> mask_array, out array<float> lens_array, out float blur_float)
	{
		string path = "cfgVehicles " + GetType() + " OpticsInfo";
		GetGame().ConfigGetFloatArray(path + " PPMaskProperties", mask_array);
		GetGame().ConfigGetFloatArray(path + " PPLensProperties", lens_array);
		blur_float = GetGame().ConfigGetFloat(path + " PPBlurProperties");
	}
	
	void InitCameraOverrideProperties()
	{
		string path = "cfgVehicles " + GetType() + " OpticsInfo";
		if ( GetGame().ConfigIsExisting(path + " nearPlaneDistanceOverride") )
		{
			m_nearplane_override = Math.Max(GetGame().ConfigGetFloat(path + " nearPlaneDistanceOverride"),DayZPlayerCameraBase.CONST_NEARPLANE_OPTICS_MIN);
		}
		else
		{
			m_nearplane_override = DayZPlayerCameraOptics.CONST_NEARPLANE_OPTICS;
		}
	}
	
	//! Initializes DOF properties for optic's alternate ironsights (ACOG etc.)
	bool InitOpticsDOFProperties (out array<float> temp_array)
	{
		if (GetGame().ConfigIsExisting("cfgVehicles " + GetType() + " OpticsInfo PPDOFProperties"))
		{
			GetGame().ConfigGetFloatArray("cfgVehicles " + GetType() + " OpticsInfo PPDOFProperties", temp_array);
			return true;
		}
		else if (GetGame().ConfigIsExisting("cfgVehicles " + GetType() + " OpticsInfoWeaponOverride PPDOFProperties"))
		{
			GetGame().ConfigGetFloatArray("cfgVehicles " + GetType() + " OpticsInfoWeaponOverride PPDOFProperties", temp_array);
			return true;
		}
		return false;
	}
	
	//! returns 'true' for non-magnifying optics
	bool AllowsDOF()
	{
		return m_allowsDOF;
	}
	
	bool IsNVOptic()
	{
		return m_isNVOptic;
	}
	
	int GetCurrentNVType()
	{
		/*
		//TODO - implement this into NV optics and modify (KazuarOptic example below)
		if (IsWorking())
		{
			switch (m_CurrentOpticMode)
			{
				case GameConstants.OPTICS_STATE_DAY:
					return NVTypes.NV_OPTICS_KAZUAR_DAY;
				
				case GameConstants.OPTICS_STATE_NIGHTVISION:
					return NVTypes.NV_OPTICS_KAZUAR_NIGHT;
			}
		}
		else
		{
			return NVTypes.NV_OPTICS_OFF;
		}
		*/
		return NVTypes.NONE;
	}
	
	void SetCurrentOpticMode(int mode)
	{
		m_CurrentOpticMode = mode;
		OnOpticModeChange();
	}
	
	int GetCurrentOpticMode()
	{
		return m_CurrentOpticMode;
	}
	
	//! optic-specific behaviour to be defined here (override)
	void OnOpticModeChange(){}
	void OnOpticEnter()
	{
		if ( GetGame() && !GetGame().IsDedicatedServer() )
		{
			HideSelection("hide");
		}
	}
	void OnOpticExit()
	{
		if ( GetGame() && !GetGame().IsDedicatedServer() )
		{
			ShowSelection("hide");
		}
	}
	
	ref array<float> GetOpticsDOF()
	{
		return m_OpticsDOFProperties;
	}
	ref array<float> GetOpticsPPMask()
	{
		return m_mask_array;
	}
	ref array<float> GetOpticsPPLens()
	{
		return m_lens_array;
	}
	float GetOpticsPPBlur()
	{
		return m_blur_float;
	}
	
	float GetNearPlaneValue()
	{
		return m_nearplane_override;
	}
	
	void Init2DPreloadType()
	{
		string path = "cfgVehicles " + GetType() + " OpticsInfo preloadOpticType";
		string type_2d;
		
		if ( GetGame().ConfigIsExisting(path) )
		{
			GetGame().ConfigGetText(path, type_2d);
			m_2D_preload_type = type_2d;
		}
	}
	
	void InitOpticMode()
	{
		SetCurrentOpticMode(GameConstants.OPTICS_STATE_DAY);
	}
	
	void UpdateSelectionVisibility() {}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionViewOptics);
	}
};	

typedef ItemOptics OpticBase;


