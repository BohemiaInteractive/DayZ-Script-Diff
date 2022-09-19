class CfgGameplayHandler
{
	private static string m_Path = "$mission:cfgGameplay.json";
	
	static ref CfgGameplayJson m_Data = new CfgGameplayJson;
	//---------------------------------------------------------------------------------------
	static bool LoadData()
	{
		if ( !FileExist( m_Path ) )
		{
			m_Path = "";
			GetGame().GetWorldName( m_Path );
			m_Path = string.Format("DZ/worlds/%1/ce/cfgGameplay.json", m_Path );
		}
		
		if (!GetGame().ServerConfigGetInt( "enableCfgGameplayFile" ) || !FileExist( m_Path ))
		{
			m_Data.InitServer();
			OnLoaded();
			return false;
		}
		
		JsonFileLoader<CfgGameplayJson>.JsonLoadFile( m_Path, m_Data );//we are allowed to read the file, so we replace the default data with data from json
		OnLoaded();
		return true;
	}
	
	//---------------------------------------------------------------------------------------
	static void OnLoaded()
	{
		GetGame().GetMission().OnGameplayDataHandlerLoad();
		DayZGame.Cast(GetGame()).OnGameplayDataHandlerLoad();
		ObjectSpawnerHandler.OnGameplayDataHandlerLoad();
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

	//--- WorldsData	--------------------------------------------------------------------
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
	static float GetMovementTimeToStrafeJog()
	{
		return m_Data.PlayerData.MovementData.timeToStrafeJog;
	}
	//----------------------------------------------------------------------------------
	static float GetMovementTimeToSprint()
	{
		return m_Data.PlayerData.MovementData.timeToSprint;
	}
	//----------------------------------------------------------------------------------
	static float GetMovementTimeToStrafeSprint()
	{
		return m_Data.PlayerData.MovementData.timeToStrafeSprint;
	}
	//----------------------------------------------------------------------------------
	static float GetMovementRotationSpeedJog()
	{
		return m_Data.PlayerData.MovementData.rotationSpeedJog;
	}
	//----------------------------------------------------------------------------------
	static float GetMovementRotationSpeedSprint()
	{
		return m_Data.PlayerData.MovementData.rotationSpeedSprint;
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
}