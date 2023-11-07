//!contents of this class will be transfered to client upon connecting, with the variables in either initial state as set through the 'InitServer..()' call, or replaced with contents of the json configuration file if such file is both present and reading is enabled in server.cfg
class CfgGameplayJson
{
	int version = -1;
	
	//! Obsolete, 'InitServer' on individual json items is now called centrally
	void InitServer()
	{
	}
	
	//-------------------------------------------------------------------------------------------------
	
	
	//!!! all member variables must correspond with the cfggameplay.json file contents !!!!
	ref ITEM_GeneralData GeneralData			= new ITEM_GeneralData;
	ref ITEM_PlayerData PlayerData 				= new ITEM_PlayerData;
	ref ITEM_WorldData WorldsData 				= new ITEM_WorldData;
	ref ITEM_BaseBuildingData BaseBuildingData 	= new ITEM_BaseBuildingData;
	ref ITEM_UIData UIData 						= new ITEM_UIData;
	ref ITEM_MapData MapData 					= new ITEM_MapData;
	
};

class ITEM_DataBase
{
	void ITEM_DataBase()
	{
		#ifdef SERVER
		CfgGameplayHandler.RegisterItem(this);
		#endif
	}
	
	bool ValidateServer()
	{
		return true;
	}
	
	void InitServer();
}

class ITEM_GeneralData : ITEM_DataBase
{
	override void InitServer()
	{
		disableBaseDamage = GetGame().ServerConfigGetInt( "disableBaseDamage" );
		disableContainerDamage = GetGame().ServerConfigGetInt( "disableContainerDamage" );
		disableRespawnDialog = GetGame().ServerConfigGetInt("disableRespawnDialog");
	}
	
	override bool ValidateServer()
	{
		return true;
	}
	//-------------------------------------------------------------------------------------------------
	//!!! all member variables must correspond with the cfggameplay.json file contents !!!!
	bool disableBaseDamage;
	bool disableContainerDamage;
	bool disableRespawnDialog;
	bool disableRespawnInUnconsciousness;

};

//--------------------------------------------------------------------------------------------------------------------------------------------------

class ITEM_PlayerData : ITEM_DataBase
{
	ref ITEM_StaminaData StaminaData			 	= new ITEM_StaminaData;
	ref ITEM_ShockHandlingData ShockHandlingData 	= new ITEM_ShockHandlingData;
	ref ITEM_MovementData MovementData 				= new ITEM_MovementData;
	ref ITEM_DrowningData DrowningData 				= new ITEM_DrowningData;
	
	override void InitServer()
	{
		disablePersonalLight = GetGame().ServerConfigGetInt( "disablePersonalLight" );
	}
	
	override bool ValidateServer()
	{
		return true;
	}
	
	//-------------------------------------------------------------------------------------------------
	//!!! all member variables must correspond with the cfggameplay.json file contents !!!!
	bool disablePersonalLight;
	bool disable2dMap;
	ref TStringArray spawnGearPresetFiles;

};

//--------------------------------------------------------------------------------------------------------------------------------------------------

class ITEM_ShockHandlingData : ITEM_DataBase
{
	override void InitServer()
	{
	}
	
	override bool ValidateServer()
	{
		return true;
	}
	
	//-------------------------------------------------------------------------------------------------
	//!!! all member variables must correspond with the cfggameplay.json file contents !!!!
	float shockRefillSpeedConscious = PlayerConstants.SHOCK_REFILL_CONSCIOUS_SPEED;
	float shockRefillSpeedUnconscious = PlayerConstants.SHOCK_REFILl_UNCONSCIOUS_SPEED;
	bool allowRefillSpeedModifier = true;
};

//--------------------------------------------------------------------------------------------------------------------------------------------------

class ITEM_StaminaData : ITEM_DataBase
{
	override void InitServer()
	{
	}
	
	override bool ValidateServer()
	{
		return true;
	}
	
	//-------------------------------------------------------------------------------------------------
	//!!! all member variables must correspond with the cfggameplay.json file contents !!!!
	float staminaWeightLimitThreshold = GameConstants.STAMINA_WEIGHT_LIMIT_THRESHOLD;
	float staminaMax = GameConstants.STAMINA_MAX;
	float staminaKgToStaminaPercentPenalty = GameConstants.STAMINA_KG_TO_STAMINAPERCENT_PENALTY;
	float staminaMinCap = GameConstants.STAMINA_MIN_CAP;
	float sprintStaminaModifierErc = 1;//consumption of stamina during standing sprint modification
	float sprintStaminaModifierCro = 1;//consumption of stamina during crouched sprint modification
	float sprintSwimmingStaminaModifier = 1;//consumption of stamina during swimming sprint modification
	float sprintLadderStaminaModifier = 1;//consumption of stamina during climbing sprint modification
	float meleeStaminaModifier = 1;//consumption of stamina during melee attacks and evasion modification
	float obstacleTraversalStaminaModifier = 1;// vaulting and climbing stamina consumption modification
	float holdBreathStaminaModifier = 1;// hold breath  stamina consumption modification
};

//--------------------------------------------------------------------------------------------------------------------------------------------------

class ITEM_MovementData : ITEM_DataBase
{
	override void InitServer()
	{
	}
	
	override bool ValidateServer()
	{
		return true;
	}
	
	//-------------------------------------------------------------------------------------------------
	//!!! all member variables must correspond with the cfggameplay.json file contents !!!!
	float timeToStrafeJog  		= 0.1;
	float rotationSpeedJog   	= 0.15;
	float timeToSprint			= 0.45;
	float timeToStrafeSprint 	= 0.3;
	float rotationSpeedSprint	= 0.15;
	bool allowStaminaAffectInertia = 1;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------

class ITEM_WorldData : ITEM_DataBase
{
	override void InitServer()
	{
		lightingConfig = GetGame().ServerConfigGetInt( "lightingConfig" );
		wetnessWeightModifiers = {GameConstants.WEIGHT_DRY,GameConstants.WEIGHT_DAMP,GameConstants.WEIGHT_WET,GameConstants.WEIGHT_SOAKING_WET,GameConstants.WEIGHT_DRENCHED};
	}
	
	override bool ValidateServer()
	{
		if (!wetnessWeightModifiers || wetnessWeightModifiers.Count() != 5)
		{
			return false;
		}
		
		return true;
	}
	
	//-------------------------------------------------------------------------------------------------
	//!!! all member variables must correspond with the cfggameplay.json file contents !!!!
	int lightingConfig;
	
	ref array<string> objectSpawnersArr;
	ref array<float> environmentMinTemps;
	ref array<float> environmentMaxTemps;
	ref array<float> wetnessWeightModifiers = {GameConstants.WEIGHT_DRY,GameConstants.WEIGHT_DAMP,GameConstants.WEIGHT_WET,GameConstants.WEIGHT_SOAKING_WET,GameConstants.WEIGHT_DRENCHED};
};

//--------------------------------------------------------------------------------------------------------------------------------------------------

class ITEM_BaseBuildingData : ITEM_DataBase
{
	
	override void InitServer()
	{
	}
	
	override bool ValidateServer()
	{
		return true;
	}
	
	//-------------------------------------------------------------------------------------------------
	//!!! all member variables must correspond with the cfggameplay.json file contents !!!!
	ref ITEM_HologramData HologramData = new ITEM_HologramData;
	ref ITEM_ConstructionData ConstructionData = new ITEM_ConstructionData;
};

//--------------------------------------------------------------------------------------------------------------------------------------------------

class ITEM_HologramData : ITEM_DataBase
{
	
	override void InitServer()
	{
		disallowedTypesInUnderground = new TStringSet();
		disallowedTypesInUnderground.Insert("FenceKit");
		disallowedTypesInUnderground.Insert("TerritoryFlagKit");
		disallowedTypesInUnderground.Insert("WatchtowerKit");
	}
	
	override bool ValidateServer()
	{
		return true;
	}
	
	//-------------------------------------------------------------------------------------------------
	//!!! all member variables must correspond with the cfggameplay.json file contents !!!!
	bool disableIsCollidingBBoxCheck;
	bool disableIsCollidingPlayerCheck;
	bool disableIsClippingRoofCheck;
	bool disableIsBaseViableCheck;
	bool disableIsCollidingGPlotCheck;
	bool disableIsCollidingAngleCheck;
	bool disableIsPlacementPermittedCheck;
	bool disableHeightPlacementCheck;
	bool disableIsUnderwaterCheck;
	bool disableIsInTerrainCheck;
	ref TStringSet disallowedTypesInUnderground;
};

//--------------------------------------------------------------------------------------------------------------------------------------------------

class ITEM_ConstructionData : ITEM_DataBase
{
	
	override void InitServer()
	{
	}
	
	override bool ValidateServer()
	{
		return true;
	}
	
	//-------------------------------------------------------------------------------------------------
	//!!! all member variables must correspond with the cfggameplay.json file contents !!!!
	bool disablePerformRoofCheck;
	bool disableIsCollidingCheck;
	bool disableDistanceCheck;
};

//--------------------------------------------------------------------------------------------------------------------------------------------------

//! data for UI, in-game HUD, and similar
class ITEM_UIData : ITEM_DataBase
{
	override void InitServer()
	{
		HitIndicationData.InitServer();
	}
	
	override bool ValidateServer()
	{
		return true;
	}
	
	//-------------------------------------------------------------------------------------------------
	//!!! all member variables must correspond with the cfggameplay.json file contents !!!!
	ref ITEM_HitIndicationData HitIndicationData = new ITEM_HitIndicationData;
	bool use3DMap = false;
};

//--------------------------------------------------------------------------------------------------------------------------------------------------

class ITEM_HitIndicationData : ITEM_DataBase
{
	override void InitServer()
	{
		hitDirectionOverrideEnabled = false;
		hitIndicationPostProcessEnabled = false;
	}
	
	override bool ValidateServer()
	{
		return true;
	}
	
	//-------------------------------------------------------------------------------------------------
	//!!! all member variables must correspond with the cfggameplay.json file contents !!!!
	//!!! all member variables must be defined in the .json file, otherwise they are treated as '0' value (unless that's what you want..)
	bool hitDirectionOverrideEnabled = false;
	int hitDirectionBehaviour = HitDirectionModes.STATIC;
	int hitDirectionStyle = HitIndicatorType.SPLASH;
	string hitDirectionIndicatorColorStr;
	float hitDirectionMaxDuration = HitDirectionConstants.DURATION_BASE;
	float hitDirectionBreakPointRelative = HitDirectionConstants.BREAKPOINT_BASE;
	float hitDirectionScatter = HitDirectionConstants.SCATTER;
	bool hitIndicationPostProcessEnabled = false;
};

//--------------------------------------------------------------------------------------------------------------------------------------------------

class ITEM_MapData : ITEM_DataBase
{
	override void InitServer()
	{
	}
	
	override bool ValidateServer()
	{
		return true;
	}
	
	//-------------------------------------------------------------------------------------------------
	//!!! all member variables must correspond with the cfggameplay.json file contents !!!!
	bool ignoreMapOwnership 		= false;
	bool ignoreNavItemsOwnership  	= false;
	bool displayPlayerPosition		= false;
	bool displayNavInfo				= true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------

class ITEM_DrowningData : ITEM_DataBase
{
	override void InitServer()
	{
	}
	
	override bool ValidateServer()
	{
		return true;
	}
	
	//-------------------------------------------------------------------------------------------------
	//!!! all member variables must correspond with the cfggameplay.json file contents !!!!
	float staminaDepletionSpeed = 10;
	float healthDepletionSpeed = 10;
	float shockDepletionSpeed = 10;
}
