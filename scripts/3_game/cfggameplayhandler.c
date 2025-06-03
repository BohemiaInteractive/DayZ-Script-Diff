class CfgGameplayHandler
{
	private static string m_Path = "$mission:cfggameplay.json";
	private static ref array<ref ITEM_DataBase> m_Items = new array<ref ITEM_DataBase>();
	
	static ref CfgGameplayJson m_Data = new CfgGameplayJson();
	
	//---------------------------------------------------------------------------------------
	static void RegisterItem(ITEM_DataBase item)
	{
		m_Items.Insert(item);
	}
	//---------------------------------------------------------------------------------------
	private static void ValidateItems()
	{
		foreach (ITEM_DataBase item : m_Items)
		{
			if (!item.ValidateServer())
			{
				string itemName = item.Type().ToString();
				itemName.Replace("ITEM_", "");
				PrintToRPT("Validation failed during loading of 'cfggameplay.json' for " + itemName);
				item.InitServer();
			}
		}
	}
	
	//---------------------------------------------------------------------------------------
	private static void InitData()
	{
		foreach (ITEM_DataBase item:m_Items)
		{
			item.InitServer();
		}
	}
	#ifdef DEVELOPER
	//---------------------------------------------------------------------------------------
	static void Output()
	{
		Print(m_Items.Count());
	}
	#endif
	//---------------------------------------------------------------------------------------
	static bool LoadData()
	{
		if (!FileExist(m_Path))
		{
			m_Path = "";
			GetGame().GetWorldName(m_Path);
			m_Path = string.Format("dz/worlds/%1/ce/cfggameplay.json", m_Path);
		}

		bool cfgGameplayFileEnabled = GetGame().ServerConfigGetInt( "enableCfgGameplayFile" );

		#ifdef DIAG_DEVELOPER
		if (!GetGame().IsDedicatedServer())
		{
			cfgGameplayFileEnabled = true;
		}
		#endif
		
		if (!cfgGameplayFileEnabled || !FileExist(m_Path))
		{
			m_Data.InitServer();//legacy call
			InitData();
			OnLoaded();
			return false;
		}
		
		string errorMessage;
		if (!JsonFileLoader<CfgGameplayJson>.LoadFile(m_Path, m_Data, errorMessage)) //! we are allowed to read the file, so we replace the default data with data from json
			ErrorEx(errorMessage);

		ValidateItems();
		OnLoaded();
		
		return true;
	}
	
	//---------------------------------------------------------------------------------------
	static void OnLoaded()
	{
		GetGame().GetMission().OnGameplayDataHandlerLoad();
		DayZGame.Cast(GetGame()).OnGameplayDataHandlerLoad();
	}
	
	//---------------------------------------------------------------------------------------
	static void SyncDataSend(notnull Man player)
	{
		GetGame().RPCSingleParam(player, ERPCs.RPC_CFG_GAMEPLAY_SYNC, new Param1<CfgGameplayJson>(m_Data), true, player.GetIdentity());
	}
	
	//---------------------------------------------------------------------------------------
	static void SyncDataSendEx(notnull PlayerIdentity identity)
	{
		//Debug.Log("SyncDataSendEx");
		GetGame().RPCSingleParam(null, ERPCs.RPC_CFG_GAMEPLAY_SYNC, new Param1<CfgGameplayJson>(m_Data), true, identity);
	}
	
	//---------------------------------------------------------------------------------------
	
	static void OnRPC(Man player, ParamsReadContext ctx)
	{
		Param1<CfgGameplayJson> data = new Param1<CfgGameplayJson>(null);
		
		if ( ctx.Read(data) )
		{
			m_Data = data.param1;
		}
		else
		{
			ErrorEx("CfgGameplayHandler - client failed to read incoming data");
		}
	}

	
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//!! =====================================================================================
	//!!! the naming convention for static functions in this file is Get+MemberVariableName !!
	//!! =====================================================================================
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!	
	
	//========================
	// !!! ONLY GETTERS BELLOW
	//========================
		
	
	//----------------------------------------------------------------------------------
	static bool GetDisablePersonalLight()
	{
		return m_Data.PlayerData.disablePersonalLight;
	}
	//----------------------------------------------------------------------------------

	//-------------------------------   WorldsData   -----------------------------------
	static int GetLightingConfig()
	{
		return m_Data.WorldsData.lightingConfig;
	}

	static TStringArray GetObjectSpawnersArr()
	{
		return m_Data.WorldsData.objectSpawnersArr;
	}

	static array<float> GetEnvironmentMinTemps()
	{
		return m_Data.WorldsData.environmentMinTemps;
	}

	static array<float> GetEnvironmentMaxTemps()
	{
		return m_Data.WorldsData.environmentMaxTemps;
	}
	static array<float> GetWetnessWeightModifiers()
	{
		return m_Data.WorldsData.wetnessWeightModifiers;
	}
	//----------------------------------------------------------------------------------

	
	static bool GetDisableBaseDamage()
	{
		return m_Data.GeneralData.disableBaseDamage;
	}
	//----------------------------------------------------------------------------------
	
	static bool GetDisableContainerDamage()
	{
		return m_Data.GeneralData.disableContainerDamage;
	}
	//----------------------------------------------------------------------------------
	static bool GetDisableRespawnDialog()
	{
		return m_Data.GeneralData.disableRespawnDialog;
	}
	//----------------------------------------------------------------------------------
	static bool GetDisableRespawnInUnconsciousness()
	{
		return m_Data.GeneralData.disableRespawnInUnconsciousness;
	}
	//----------------------------------------------------------------------------------
	static float GetSprintStaminaModifierErc()
	{
		return m_Data.PlayerData.StaminaData.sprintStaminaModifierErc;
	}
	//----------------------------------------------------------------------------------
	static float GetSprintStaminaModifierCro()
	{
		return m_Data.PlayerData.StaminaData.sprintStaminaModifierCro;
	}
	//----------------------------------------------------------------------------------
	static float GetSprintSwimmingStaminaModifier()
	{
		return m_Data.PlayerData.StaminaData.sprintSwimmingStaminaModifier;
	}//----------------------------------------------------------------------------------
	static float GetSprintLadderStaminaModifier()
	{
		return m_Data.PlayerData.StaminaData.sprintLadderStaminaModifier;
	}
	//----------------------------------------------------------------------------------
	static float GetStaminaWeightLimitThreshold()
	{
		return m_Data.PlayerData.StaminaData.staminaWeightLimitThreshold;
	}
	//----------------------------------------------------------------------------------
	static float GetStaminaMax()
	{
		return m_Data.PlayerData.StaminaData.staminaMax;
	}
	//----------------------------------------------------------------------------------
	static float GetStaminaKgToStaminaPercentPenalty()
	{
		return m_Data.PlayerData.StaminaData.staminaKgToStaminaPercentPenalty;
	}
	//----------------------------------------------------------------------------------
	static float GetStaminaMinCap()
	{
		return m_Data.PlayerData.StaminaData.staminaMinCap;
	}
	//----------------------------------------------------------------------------------
	static float GetMeleeStaminaModifier()
	{
		return m_Data.PlayerData.StaminaData.meleeStaminaModifier;
	}
	//----------------------------------------------------------------------------------
	static float GetObstacleTraversalStaminaModifier()
	{
		return m_Data.PlayerData.StaminaData.obstacleTraversalStaminaModifier;
	}
	//----------------------------------------------------------------------------------
	static float GetHoldBreathStaminaModifier()
	{
		return m_Data.PlayerData.StaminaData.holdBreathStaminaModifier;
	}
	//----------------------------------------------------------------------------------	
	static float GetMovementTimeToStrafeJog()
	{
		return Math.Max(0.01, m_Data.PlayerData.MovementData.timeToStrafeJog);
	}
	//----------------------------------------------------------------------------------
	static float GetMovementTimeToSprint()
	{
		return Math.Max(0.01, m_Data.PlayerData.MovementData.timeToSprint);
	}
	//----------------------------------------------------------------------------------
	static float GetMovementTimeToStrafeSprint()
	{
		return Math.Max(0.01, m_Data.PlayerData.MovementData.timeToStrafeSprint);
	}
	//----------------------------------------------------------------------------------
	static float GetMovementRotationSpeedJog()
	{
		return Math.Max(0.01, m_Data.PlayerData.MovementData.rotationSpeedJog);
	}
	//----------------------------------------------------------------------------------
	static float GetMovementRotationSpeedSprint()
	{
		return Math.Max(0.01, m_Data.PlayerData.MovementData.rotationSpeedSprint);
	}
	//----------------------------------------------------------------------------------
	static EWeaponObstructionMode GetWeaponObstructionModeStatic()
	{
		return m_Data.PlayerData.WeaponObstructionData.staticMode;
	}
	//----------------------------------------------------------------------------------
	static EWeaponObstructionMode GetWeaponObstructionModeDynamic()
	{
		return m_Data.PlayerData.WeaponObstructionData.dynamicMode;
	}
	//----------------------------------------------------------------------------------
	static bool GetDisable2dMap()
	{
		return m_Data.PlayerData.disable2dMap;
	}
	//----------------------------------------------------------------------------------
	static bool GetDisableIsCollidingBBoxCheck()
	{
		return m_Data.BaseBuildingData.HologramData.disableIsCollidingBBoxCheck;
	}
	//----------------------------------------------------------------------------------
	static bool GetDisableIsCollidingPlayerCheck()
	{
		return m_Data.BaseBuildingData.HologramData.disableIsCollidingPlayerCheck;
	}
	//----------------------------------------------------------------------------------
	static bool GetDisableIsClippingRoofCheck()
	{
		return m_Data.BaseBuildingData.HologramData.disableIsClippingRoofCheck;
	}
	//----------------------------------------------------------------------------------
	static bool GetDisableIsBaseViableCheck()
	{
		return m_Data.BaseBuildingData.HologramData.disableIsBaseViableCheck;
	}
	//----------------------------------------------------------------------------------
	static bool GetDisableIsInTerrainCheck()
	{
		return m_Data.BaseBuildingData.HologramData.disableIsInTerrainCheck;
	}
	//----------------------------------------------------------------------------------
	static bool GetDisableIsUnderwaterCheck()
	{
		return m_Data.BaseBuildingData.HologramData.disableIsUnderwaterCheck;
	}
	//----------------------------------------------------------------------------------
	static bool GetDisableHeightPlacementCheck()
	{
		return m_Data.BaseBuildingData.HologramData.disableHeightPlacementCheck;
	}
	//----------------------------------------------------------------------------------
	static bool GetDisableIsPlacementPermittedCheck()
	{
		return m_Data.BaseBuildingData.HologramData.disableIsPlacementPermittedCheck;
	}
	//----------------------------------------------------------------------------------
	static bool GetDisableIsCollidingAngleCheck()
	{
		return m_Data.BaseBuildingData.HologramData.disableIsCollidingAngleCheck;
	}
	//----------------------------------------------------------------------------------
	static bool GetDisableIsCollidingGPlotCheck()
	{
		return m_Data.BaseBuildingData.HologramData.disableIsCollidingGPlotCheck;
	}
	//----------------------------------------------------------------------------------	
	static bool GetDisableIsCollidingCheck()
	{
		return m_Data.BaseBuildingData.ConstructionData.disableIsCollidingCheck;
	}
	//----------------------------------------------------------------------------------
	static bool GetDisablePerformRoofCheck()
	{
		return m_Data.BaseBuildingData.ConstructionData.disablePerformRoofCheck;
	}
	//----------------------------------------------------------------------------------
	static bool GetDisableDistanceCheck()
	{
		return m_Data.BaseBuildingData.ConstructionData.disableDistanceCheck;
	}
	//----------------------------------------------------------------------------------
	static bool GetDisableColdAreaPlacementCheck()
	{
		return m_Data.BaseBuildingData.HologramData.disableColdAreaPlacementCheck;
	}
	//----------------------------------------------------------------------------------
	static TStringSet GetDisallowedTypesInUnderground()
	{
		return m_Data.BaseBuildingData.HologramData.disallowedTypesInUnderground;
	}
	//----------------------------------------------------------------------------------
	static bool GetHitIndicationOverrideEnabled() //TODO - ?
	{
		return m_Data.UIData.HitIndicationData.hitDirectionOverrideEnabled;
	}
	//----------------------------------------------------------------------------------
	static int GetHitIndicationMode()
	{
		return m_Data.UIData.HitIndicationData.hitDirectionBehaviour;
	}
	//----------------------------------------------------------------------------------
	static int GetHitIndicationTypeID()
	{
		return m_Data.UIData.HitIndicationData.hitDirectionStyle;
	}
	//----------------------------------------------------------------------------------
	static int GetHitIndicationIndicatorColor()
	{
		return m_Data.UIData.HitIndicationData.hitDirectionIndicatorColorStr.HexToInt();
	}
	//----------------------------------------------------------------------------------
	static float GetHitIndicationMaxDuration()
	{
		return m_Data.UIData.HitIndicationData.hitDirectionMaxDuration;
	}
	//----------------------------------------------------------------------------------
	static float GetHitIndicationBreakPoint()
	{
		return m_Data.UIData.HitIndicationData.hitDirectionBreakPointRelative;
	}
	//----------------------------------------------------------------------------------
	static float GetHitIndicationScatter()
	{
		return m_Data.UIData.HitIndicationData.hitDirectionScatter;
	}
	//----------------------------------------------------------------------------------
	static bool GetHitIndicationPPEEnabled()
	{
		return m_Data.UIData.HitIndicationData.hitIndicationPostProcessEnabled;
	}
	//----------------------------------------------------------------------------------
	static bool GetUse3DMap()
	{
		return m_Data.UIData.use3DMap;
	}
	//----------------------------------------------------------------------------------	
	static float GetShockRefillSpeedConscious()
	{
		return m_Data.PlayerData.ShockHandlingData.shockRefillSpeedConscious;
	}
	//----------------------------------------------------------------------------------
	static float GetShockRefillSpeedUnconscious()
	{
		return m_Data.PlayerData.ShockHandlingData.shockRefillSpeedUnconscious;
	}
	//----------------------------------------------------------------------------------
	static bool GetAllowRefillSpeedModifier()
	{
		return m_Data.PlayerData.ShockHandlingData.allowRefillSpeedModifier;
	}

	//--- MapData ----------------------------------------------------------------------
	static bool GetMapIgnoreMapOwnership()
	{
		return m_Data.MapData.ignoreMapOwnership;
	}
	//----------------------------------------------------------------------------------
	static bool GetMapIgnoreNavItemsOwnership()
	{
		return m_Data.MapData.ignoreNavItemsOwnership;
	}
	//----------------------------------------------------------------------------------
	static bool GetMapDisplayPlayerPosition()
	{
		return m_Data.MapData.displayPlayerPosition;
	}
	//----------------------------------------------------------------------------------
	static bool GetMapDisplayNavigationInfo()
	{
		return m_Data.MapData.displayNavInfo;
	}
	//----------------------------------------------------------------------------------
	static float GetStaminaDepletionSpeed()
	{
		return m_Data.PlayerData.DrowningData.staminaDepletionSpeed;
	}
	//----------------------------------------------------------------------------------
	static float GetHealthDepletionSpeed()
	{
		return m_Data.PlayerData.DrowningData.healthDepletionSpeed;
	}
	//----------------------------------------------------------------------------------
	static float GetShockDepletionSpeed()
	{
		return m_Data.PlayerData.DrowningData.shockDepletionSpeed;
	}
	//----------------------------------------------------------------------------------
	static bool GetAllowStaminaAffectInertia()
	{
		return m_Data.PlayerData.MovementData.allowStaminaAffectInertia;
	}
	//----------------------------------------------------------------------------------
	static TStringArray GetPlayerSpawnGearPresetFiles()
	{
		return m_Data.PlayerData.spawnGearPresetFiles;
	}
	//----------------------------------------------------------------------------------
	static TStringArray GetPlayerRestrictedAreaFiles()
	{
		return m_Data.WorldsData.playerRestrictedAreaFiles;
	}
	//----------------------------------------------------------------------------------
	static float GetBoatDecayMultiplier()
	{
		return m_Data.VehicleData.boatDecayMultiplier;
	}
}