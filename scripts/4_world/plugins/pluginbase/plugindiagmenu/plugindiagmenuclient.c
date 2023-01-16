// For modding, see PluginDiagMenuModding.c
// !!! MODDING DISCLAIMER: These are debug functionality files, if you are thinking about modding the vanilla ones, do so at your own risk
//                         These files will not be maintained with the thought of "what if a modder modded this" (Excluding the modding functionality of course)
//                         Which is why the modding functionality was developed with the thought of the modded ones having their own isolated safe space

class PluginDiagMenuClient : PluginDiagMenu
{
#ifdef DIAG_DEVELOPER
	static int m_SystemsMask;
	
	// Cheats
	bool m_ModifiersDisabled;
	int m_IsInvincible;	
	bool m_StaminaDisabled;
	
	// Misc
	float m_Playtime;
	bool m_LogPlayerStats;	
	Shape m_VehicleFreeAreaBox;	
	ref EnvDebugData m_EnvDebugData;
	
	override void OnInit()
	{
		super.OnInit();
		
		BindCallbacks();
	}
	
	protected void BindCallbacks()
	{	
		//---------------------------------------------------------------
		// LEVEL 2 - Script > Crafting
		//---------------------------------------------------------------
		DiagMenu.BindCallback(DiagMenuIDs.CRAFTING_GENERATE, CBCraftingGenerate);
		DiagMenu.BindCallback(DiagMenuIDs.CRAFTING_INSTANT, CBCraftingInstant);
		DiagMenu.BindCallback(DiagMenuIDs.CRAFTING_DUMP, CBCraftingDump);
		
		//---------------------------------------------------------------
		// LEVEL 2 - Script > Vehicles
		//---------------------------------------------------------------	
		DiagMenu.BindCallback(DiagMenuIDs.VEHICLE_DEBUG_OUTPUT, CBVehicleDebugOutput);
		DiagMenu.BindCallback(DiagMenuIDs.VEHICLE_DUMP_CRASH_DATA, CBDumpCrashData);
		
		//---------------------------------------------------------------
		// LEVEL 2 - Script > Cheats
		//---------------------------------------------------------------
		DiagMenu.BindCallback(DiagMenuIDs.CHEATS_MODIFIERS, CBCheatsModifiers);
		DiagMenu.BindCallback(DiagMenuIDs.CHEATS_KILL_PLAYER, CBCheatsKillPlayer);
		DiagMenu.BindCallback(DiagMenuIDs.CHEATS_INVINCIBILITY, CBCheatsInvincibility);
		DiagMenu.BindCallback(DiagMenuIDs.CHEATS_DISABLE_STAMINA, CBCheatsStaminaDisable);
		DiagMenu.BindCallback(DiagMenuIDs.CHEATS_RESET_PLAYER, CBCheatsResetPlayer);
		DiagMenu.BindCallback(DiagMenuIDs.CHEATS_RESET_PLAYER_MAX, CBCheatsResetPlayerMax);
		DiagMenu.BindCallback(DiagMenuIDs.CHEATS_INVENTORY_ACCESS, CBCheatsInventoryAccess);
		DiagMenu.BindCallback(DiagMenuIDs.CHEATS_FIX_ITEMS, CBCheatsFixItems);
		
		//---------------------------------------------------------------
		// LEVEL 2 - Script > Player Agents
		//---------------------------------------------------------------
		DiagMenu.BindCallback(DiagMenuIDs.PLAYER_AGENTS_INJECTS_SHOW, CBPlayerAgentsInjectActions);

		//---------------------------------------------------------------
		// LEVEL 2 - Script > Soft Skills
		//---------------------------------------------------------------
		DiagMenu.BindCallback(DiagMenuIDs.SOFT_SKILLS_SHOW_DEBUG, CBSoftSkillsShowDebug);
		DiagMenu.BindCallback(DiagMenuIDs.SOFT_SKILLS_TOGGLE_STATE, CBSoftSkillsToggleState);
		DiagMenu.BindCallback(DiagMenuIDs.SOFT_SKILLS_TOGGLE_MODEL, CBSoftSkillsToggleModel);
		DiagMenu.BindCallback(DiagMenuIDs.SOFT_SKILLS_SPECIALTY_VALUE, CBSoftSkillsSpecialtyValue);
		
		//---------------------------------------------------------------
		// LEVEL 2 - Script > Lifespan
		//---------------------------------------------------------------
		DiagMenu.BindCallback(DiagMenuIDs.LIFESPAN_BLOODY_HANDS, CBLifespanBloodyHands);
		DiagMenu.BindCallback(DiagMenuIDs.LIFESPAN_PLAYTIME_UPDATE, CBLifespanPlaytimeUpdate);
		
		//---------------------------------------------------------------
		// LEVEL 2 - Script > Misc
		//---------------------------------------------------------------
		DiagMenu.BindCallback(DiagMenuIDs.MISC_DISABLE_PERSONAL_LIGHT, CBMiscPersonalLight);
		DiagMenu.BindCallback(DiagMenuIDs.MISC_ITEM_DEBUG_ACTIONS, CBMiscItemDebugActions); // Is enabled by default now
		DiagMenu.BindCallback(DiagMenuIDs.MISC_LOG_PLAYER_STATS, CBMiscLogPlayerStats);
		
		//---------------------------------------------------------------
		// LEVEL 2 - Script > Misc
		//---------------------------------------------------------------
		DiagMenu.BindCallback(DiagMenuIDs.MISC_PERMANENT_CROSSHAIR, CBMiscPermanentCrossHair);		
		DiagMenu.BindCallback(DiagMenuIDs.MISC_TOGGLE_HUD, CBMiscToggleHud);
		DiagMenu.BindCallback(DiagMenuIDs.MISC_ENVIRONMENT_DEBUG, CBMiscEnvironmentDebug);
		DiagMenu.BindCallback(DiagMenuIDs.MISC_DISPLAY_PLAYER_INFO, CBMiscDisplayPlayerInfo);
		DiagMenu.BindCallback(DiagMenuIDs.MISC_UNIVERSAL_TEMPERATURE_SOURCES, CBMiscUniversalTemperatureSources);			
		DiagMenu.BindCallback(DiagMenuIDs.MISC_BULLET_IMPACT, CBMiscBulletImpact);			
		DiagMenu.BindCallback(DiagMenuIDs.MISC_GO_UNCONSCIOUS, CBMiscGoUnconscious);
		DiagMenu.BindCallback(DiagMenuIDs.MISC_GO_UNCONSCIOUS_DELAYED, CBMiscGoUnconsciousDelayed);
		DiagMenu.BindCallback(DiagMenuIDs.MISC_QUICK_RESTRAIN, CBMiscQuickRestrain);

		//---------------------------------------------------------------
		// LEVEL 3 - Script > Misc > Hair Hiding
		//---------------------------------------------------------------
		DiagMenu.BindCallback(DiagMenuIDs.MISC_HAIR_LEVEL_HIDE, CBMiscHairHide);
		DiagMenu.BindCallback(DiagMenuIDs.MISC_HAIR_HIDE_ALL, CBMiscHairHideAll);
				
		//---------------------------------------------------------------
		// LEVEL 2 - Script > Misc
		//---------------------------------------------------------------
		DiagMenu.BindCallback(DiagMenuIDs.MISC_CAM_SHAKE, CBMiscCamShake);
		DiagMenu.BindCallback(DiagMenuIDs.MISC_QUICK_FISHING, CBMiscQuickFishing);
		DiagMenu.BindCallback(DiagMenuIDs.MISC_SHOCK_IMPACT, CBMiscShockImpact);
		DiagMenu.BindCallback(DiagMenuIDs.MISC_SHOW_PLUG_ARROWS, CBMiscPlugArrows);			
		DiagMenu.BindCallback(DiagMenuIDs.MISC_TARGETABLE_BY_AI, CBMiscTargetableByAI);

		//---------------------------------------------------------------
		// LEVEL 3 - Script > Misc > Hit Indication
		//---------------------------------------------------------------
		DiagMenu.BindCallback(DiagMenuIDs.MISC_HIT_INDICATION_DEBUG, CBMiscHitIndication);
		DiagMenu.BindCallback(DiagMenuIDs.MISC_HIT_INDICATION_SPAWN_HIT, CBMiscHitIndicationSpawnHit);					
		DiagMenu.BindCallback(DiagMenuIDs.MISC_HIT_INDICATION_MODE, CBMiscHitIndication);
		DiagMenu.BindCallback(DiagMenuIDs.MISC_HIT_INDICATION_TYPE, CBMiscHitIndication);
		DiagMenu.BindCallback(DiagMenuIDs.MISC_HIT_INDICATION_DURATION, CBMiscHitIndication);
		DiagMenu.BindCallback(DiagMenuIDs.MISC_HIT_INDICATION_BREAKPOINT, CBMiscHitIndication);
		DiagMenu.BindCallback(DiagMenuIDs.MISC_HIT_INDICATION_DISTANCE, CBMiscHitIndication);
		DiagMenu.BindCallback(DiagMenuIDs.MISC_HIT_INDICATION_ROTATION, CBMiscHitIndication);
		DiagMenu.BindCallback(DiagMenuIDs.MISC_HIT_INDICATION_SCATTER, CBMiscHitIndication);
		DiagMenu.BindCallback(DiagMenuIDs.MISC_HIT_INDICATION_DISABLE_PPE, CBMiscHitIndication);
		
		//---------------------------------------------------------------
		// LEVEL 2 - Script > Misc
		//---------------------------------------------------------------	
		DiagMenu.BindCallback(DiagMenuIDs.MISC_FREEZE_ENTITY, CBMiscFreezeEntity);

		//---------------------------------------------------------------
		// LEVEL 2 - Script > Simulate script
		//---------------------------------------------------------------		
		DiagMenu.BindCallback(DiagMenuIDs.SIMULATE_INFINITE_LOOP, CBSimulateInfiniteLoop);
		DiagMenu.BindCallback(DiagMenuIDs.SIMULATE_NULL_POINTER, CBSimulateNullPointer);
		DiagMenu.BindCallback(DiagMenuIDs.SIMULATE_DIVISION_BY_ZERO, CBSimulateDivisionByZero);
		DiagMenu.BindCallback(DiagMenuIDs.SIMULATE_ERROR_FUNCTION, CBSimulateErrorFunction);
		
		//---------------------------------------------------------------
		// LEVEL 2 - Script > Weapon
		//---------------------------------------------------------------
		DiagMenu.BindCallback(DiagMenuIDs.WEAPON_PARTICLES, CBWeaponParticles);		
		DiagMenu.BindCallback(DiagMenuIDs.WEAPON_AIM_NOISE, CBWeaponAimNoise);
		DiagMenu.BindCallback(DiagMenuIDs.WEAPON_RECOIL, CBWeaponRecoil);
		DiagMenu.BindCallback(DiagMenuIDs.WEAPON_UNLIMITED_AMMO, CBWeaponUnlimitedAmmo);
		DiagMenu.BindCallback(DiagMenuIDs.WEAPON_BURST_VERSION, CBWeaponBurstVersion);

		//---------------------------------------------------------------
		// LEVEL 2 - Script > Bleeding
		//---------------------------------------------------------------
		DiagMenu.BindCallback(DiagMenuIDs.BLEEDING_SOURCES, CBBleedingSources);
		DiagMenu.BindCallback(DiagMenuIDs.BLEEDING_DISABLE_BLOOD_LOSS, CBBleedingDisableBloodLoss);
		DiagMenu.BindCallback(DiagMenuIDs.BLEEDING_ACTIVATE_SOURCE, CBBleedingActivateSource);
		DiagMenu.BindCallback(DiagMenuIDs.BLEEDING_ACTIVATE_ALL_SOURCES, CBBleedingActivateAllSources);
		DiagMenu.BindCallback(DiagMenuIDs.BLEEDING_ACTIVATE_SOURCE_LEVEL, CBBleedingActivateSourceLevel);
		DiagMenu.BindCallback(DiagMenuIDs.BLEEDING_RELOAD, CBBleedingReload);
		
		//---------------------------------------------------------------
		// LEVEL 3 - Script > Bleeding > Bleeding Indication
		//---------------------------------------------------------------
		DiagMenu.BindCallback(DiagMenuIDs.BLEEDINGINDICATORS, CBBBleedingIndicators);
		DiagMenu.BindCallback(DiagMenuIDs.BLEEDINGINDICATORS_USEVALUEOVERRIDES, CBBBleedingIndicators);
		DiagMenu.BindCallback(DiagMenuIDs.BLEEDINGINDICATORS_DROP_RANDOMROTATION, CBBBleedingIndicators);
		DiagMenu.BindCallback(DiagMenuIDs.BLEEDINGINDICATORS_SEQUENCE_DURATION, CBBBleedingIndicators);
		DiagMenu.BindCallback(DiagMenuIDs.BLEEDINGINDICATORS_DROP_BASEDURATION, CBBBleedingIndicators);
		DiagMenu.BindCallback(DiagMenuIDs.BLEEDINGINDICATORS_DROP_MINDELAY, CBBBleedingIndicators);
		DiagMenu.BindCallback(DiagMenuIDs.BLEEDINGINDICATORS_DROP_MAXDELAY, CBBBleedingIndicators);
		DiagMenu.BindCallback(DiagMenuIDs.BLEEDINGINDICATORS_DROP_PROGRESSTHRESHOLD, CBBBleedingIndicators);
		DiagMenu.BindCallback(DiagMenuIDs.BLEEDINGINDICATORS_DROP_STARTSIZE, CBBBleedingIndicators);
		DiagMenu.BindCallback(DiagMenuIDs.BLEEDINGINDICATORS_DROP_ENDSIZE, CBBBleedingIndicators);
		DiagMenu.BindCallback(DiagMenuIDs.BLEEDINGINDICATORS_DROP_RNDSCALEMIN, CBBBleedingIndicators);
		DiagMenu.BindCallback(DiagMenuIDs.BLEEDINGINDICATORS_DROP_RNDSCALEMAX, CBBBleedingIndicators);
		DiagMenu.BindCallback(DiagMenuIDs.BLEEDINGINDICATORS_DROP_SCATTER, CBBBleedingIndicators);
		DiagMenu.BindCallback(DiagMenuIDs.BLEEDINGINDICATORS_DROP_SLIDEDISTANCE, CBBBleedingIndicators);

		//---------------------------------------------------------------
		// LEVEL 4 - Script > Bleeding > Bleeding Indication > Color Debug
		//---------------------------------------------------------------
		DiagMenu.BindCallback(DiagMenuIDs.BLEEDINGINDICATORS_DROP_COLOR_ENABLEDESATURATION, CBBBleedingIndicators);		
		DiagMenu.BindCallback(DiagMenuIDs.BLEEDINGINDICATORS_DROP_COLOR_DESATURATIONEND, CBBBleedingIndicators);
		DiagMenu.BindCallback(DiagMenuIDs.BLEEDINGINDICATORS_DROP_COLOR_R, CBBBleedingIndicators);
		DiagMenu.BindCallback(DiagMenuIDs.BLEEDINGINDICATORS_DROP_COLOR_G, CBBBleedingIndicators);
		DiagMenu.BindCallback(DiagMenuIDs.BLEEDINGINDICATORS_DROP_COLOR_B, CBBBleedingIndicators);
		DiagMenu.BindCallback(DiagMenuIDs.BLEEDINGINDICATORS_DROP_COLOR_A_START, CBBBleedingIndicators);
		DiagMenu.BindCallback(DiagMenuIDs.BLEEDINGINDICATORS_DROP_COLOR_A_END, CBBBleedingIndicators);
		
		//---------------------------------------------------------------
		// LEVEL 2 - Script > Logs
		//---------------------------------------------------------------
		DiagMenu.BindCallback(DiagMenuIDs.LOGS_ACTIONS, CBLogsActions);
		DiagMenu.BindCallback(DiagMenuIDs.LOGS_WEAPONS, CBLogsWeapons);
		DiagMenu.BindCallback(DiagMenuIDs.LOGS_SYMPTOMS, CBLogsSymptoms);

		//---------------------------------------------------------------
		// LEVEL 3 - Script > Logs > InventoryLogs
		//---------------------------------------------------------------
		DiagMenu.BindCallback(DiagMenuIDs.LOGS_INVENTORY_MOVE, CBLogsInventoryMove);
		DiagMenu.BindCallback(DiagMenuIDs.LOGS_INVENTORY_RESERVATION, CBLogsInventoryReservation);
		DiagMenu.BindCallback(DiagMenuIDs.LOGS_INVENTORY_HFSM, CBLogsInventoryHFSM);

		//---------------------------------------------------------------
		// LEVEL 2 - Script > Triggers
		//---------------------------------------------------------------
		DiagMenu.BindCallback(DiagMenuIDs.TRIGGER_DEBUG, CBTriggerDebug);

		//---------------------------------------------------------------
		// LEVEL 2 - Script > Base Building
		//---------------------------------------------------------------
		DiagMenu.BindCallback(DiagMenuIDs.BASEBUILDING_WOOD, CBBaseBuildingWood);
		DiagMenu.BindCallback(DiagMenuIDs.BASEBUILDING_GATE, CBBaseBuildingGate);
			
		//---------------------------------------------------------------
		// LEVEL 2 - Script > Finishers
		//---------------------------------------------------------------
		DiagMenu.BindCallback(DiagMenuIDs.FINISHERS_FINISHER_FORCED, CBFinishersFinisherForced);
			
		//---------------------------------------------------------------
		// LEVEL 2 - Script > Cinematic Camera Tools
		//---------------------------------------------------------------
		DiagMenu.BindCallback(DiagMenuIDs.CAMERATOOLS_ENABLE_REMOTE_CAMERA, CBCameraToolsEnableRemoteCamera);
	}
	
	//---------------------------------------------
	override void OnRPC(PlayerBase player, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(player, rpc_type, ctx);
		
		switch (rpc_type)
		{
			case ERPCs.DIAG_MISC_ENVIRONMENT_DEBUG_DATA:
			{
				if (!m_EnvDebugData)
					m_EnvDebugData = new EnvDebugData();
				
				ctx.Read(m_EnvDebugData);
				break;
			}
			
			case ERPCs.DIAG_VEHICLES_DUMP_CRASH_DATA_CONTENTS:
			{
				Param1<array<ref CrashDebugData>> par = new Param1<array<ref CrashDebugData>>(null);
				
				if (ctx.Read(par))
					CrashDebugData.DumpDataArray(par.param1);
				break;
			}	
		}
	}
	
	//---------------------------------------------
	override void OnUpdate(float delta_time)
	{		
		super.OnUpdate(delta_time);
		
		//---------------------------------------------
		// every system can handle their own actions locally(within the system itself),
		// this section is for systems which don't have client side representation,
		// or don't have their own tick, or for whatever other reason you prefer to put the logic here
		// PUT ONLY METHOD CALLS HERE, NO CODE !!
		// PUT ONLY METHOD CALLS HERE, NO CODE !!
		//---------------------------------------------
		
		UpdateMiscVehicleGetOutBox();
		UpdateEnvironmentDebug();
		CheckTimeAccel();		
	}
	
	//---------------------------------------------
	void CheckTimeAccel()
	{
		if (!FeatureTimeAccel.m_CurrentTimeAccel)
		{
			InitTimeAccel();
		}
		
		if (GetGame() && GetGame().GetPlayer()) 
		{
			TimeAccelParam param = GetTimeAccelMenuState();

			if (!FeatureTimeAccel.AreTimeAccelParamsSame(param, FeatureTimeAccel.m_CurrentTimeAccel))
			{
				int timeAccelBig = param.param2;
				float timeAccelSmall = param.param2 - timeAccelBig;
				FeatureTimeAccel.CopyTimeAccelClipboard(param.param1, timeAccelBig, timeAccelSmall, param.param3 );
				FeatureTimeAccel.SendTimeAccel(GetGame().GetPlayer(), param);
				FeatureTimeAccel.m_CurrentTimeAccel = param;
			}		
		}
	
	}
	
	//---------------------------------------------
	static void CBCraftingGenerate(bool enabled, int id)
	{
		PluginRecipesManager pluginRecipesManager = PluginRecipesManager.Cast(GetPlugin(PluginRecipesManager));
		DiagButtonAction(enabled, id, ScriptCaller.Create(pluginRecipesManager.CallbackGenerateCache));
	}
	
	//---------------------------------------------
	static void CBCraftingInstant(bool enabled)
	{
		PluginRecipesManager pluginRecipesManager = PluginRecipesManager.Cast(GetPlugin(PluginRecipesManager));
		DiagToggleRPC(enabled, pluginRecipesManager.IsEnableDebugCrafting(), ERPCs.DIAG_CRAFTING_INSTANT);
	}
	
	//---------------------------------------------
	static void CBCraftingDump(bool enabled, int id)
	{	
		PluginRecipesManager pluginRecipesManager = PluginRecipesManager.Cast(GetPlugin(PluginRecipesManager));
		DiagButtonAction(enabled, id, ScriptCaller.Create(pluginRecipesManager.GenerateHumanReadableRecipeList));
	}
	
	//---------------------------------------------
	static void CBCheatsModifiers(bool enabled)
	{
		PluginDiagMenuClient pluginDiag = PluginDiagMenuClient.Cast(GetPlugin(PluginDiagMenuClient));
		DiagToggleRPCServer(enabled, pluginDiag.m_ModifiersDisabled, ERPCs.DIAG_CHEATS_MODIFIERS);
	}
	
	//---------------------------------------------
	static void CBCheatsKillPlayer(bool enabled, int id)
	{
		DiagButtonRPC(enabled, id, ERPCs.DIAG_CHEATS_KILL_PLAYER, true);
	}
	
	//---------------------------------------------
	static void CBCheatsInvincibility(int value)
	{
		PluginDiagMenuClient pluginDiag = PluginDiagMenuClient.Cast(GetPlugin(PluginDiagMenuClient));
		DiagToggleRPCServer(value, pluginDiag.m_IsInvincible, ERPCs.DIAG_CHEATS_INVINCIBILITY);	
	}

	//---------------------------------------------
	static void CBCheatsStaminaDisable(bool enabled)
	{
		PluginDiagMenuClient pluginDiag = PluginDiagMenuClient.Cast(GetPlugin(PluginDiagMenuClient));
		DiagToggleRPCServer(enabled, pluginDiag.m_StaminaDisabled, ERPCs.DIAG_CHEATS_DISABLE_STAMINA);
	}

	//---------------------------------------------
	static void CBCheatsResetPlayer(bool enabled, int id)
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player)
			player.ResetPlayer(false);
		DiagButtonRPC(enabled, id, ERPCs.DIAG_CHEATS_RESET_PLAYER, true);
	}

	//---------------------------------------------
	static void CBCheatsResetPlayerMax(bool enabled, int id)
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player)
			player.ResetPlayer(true);
		DiagButtonRPC(enabled, id, ERPCs.DIAG_CHEATS_RESET_PLAYER_MAX, true);
	}
	
	//---------------------------------------------
	static void CBCheatsInventoryAccess(bool enabled)
	{
		DiagToggleRPCServer(enabled, PlayerBase.DEBUG_INVENTORY_ACCESS, ERPCs.DIAG_CHEATS_INVENTORY_ACCESS);
	}	
		
	//---------------------------------------------
	static void CBCheatsFixItems(bool enabled, int id)
	{
		DiagButtonRPC(enabled, id, ERPCs.DIAG_CHEATS_ITEMS_FIX, true);
	}
	
	//---------------------------------------------
	static void CBPlayerAgentsInjectActions(bool enabled)
	{
		DiagDebugAction(DebugActionType.PLAYER_AGENTS, enabled);
	}
	
	//---------------------------------------------
	static void CBSoftSkillsShowDebug(bool enabled)
	{
		SoftSkillsManager ssMngr = GetPlayer().GetSoftSkillsManager();		
		DiagToggleRPCAction(enabled, ScriptCaller.Create(ssMngr.CreateDebugWindow), ssMngr.IsDebug(), ERPCs.DIAG_SOFT_SKILLS_SHOW_DEBUG);
	}
	
	//---------------------------------------------
	static void CBSoftSkillsToggleState(bool enabled)
	{	
		SoftSkillsManager ssMngr = GetPlayer().GetSoftSkillsManager();		
		DiagToggleRPC(enabled, ssMngr.GetSoftSkillsState(), ERPCs.DIAG_SOFT_SKILLS_TOGGLE_STATE);
	}
	
	//---------------------------------------------
	static void CBSoftSkillsToggleModel(bool enabled)
	{
		SoftSkillsManager ssMngr = GetPlayer().GetSoftSkillsManager();
		DiagToggleRPC(enabled, ssMngr.IsLinear(), ERPCs.DIAG_SOFT_SKILLS_TOGGLE_MODEL);
	}

	//---------------------------------------------
	static void CBSoftSkillsSpecialtyValue(float value)
	{
		SoftSkillsManager ssMngr = GetPlayer().GetSoftSkillsManager();
		DiagToggleRPC(value, ssMngr.GetSpecialtyLevel(), ERPCs.DIAG_SOFT_SKILLS_SPECIALTY_VALUE);	
	}

	//---------------------------------------------
	static void CBLifespanBloodyHands(bool enabled)
	{		
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player)
			DiagToggleRPCServer(enabled, player.HasBloodyHands(), ERPCs.DIAG_LIFESPAN_BLOODY_HANDS);
	}

	//---------------------------------------------
	static void CBLifespanPlaytimeUpdate(float value)
	{
		PluginDiagMenuClient pluginDiag = PluginDiagMenuClient.Cast(GetPlugin(PluginDiagMenuClient));
		DiagToggleRPCServer(value * 60, pluginDiag.m_Playtime, ERPCs.DIAG_LIFESPAN_PLAYTIME_UPDATE);
	}
	
	//---------------------------------------------
	static void CBMiscPersonalLight(bool enabled)
	{
		PlayerBaseClient.m_PersonalLightDisabledByDebug = enabled;
		PlayerBaseClient.UpdatePersonalLight();
	}
	
	//---------------------------------------------
	static void CBMiscItemDebugActions(bool enabled)
	{
		DiagDebugAction(DebugActionType.GENERIC_ACTIONS, enabled);
	}
	
	//---------------------------------------------
	static void CBMiscLogPlayerStats(bool enabled)
	{
		PluginDiagMenuClient pluginDiag = PluginDiagMenuClient.Cast(GetPlugin(PluginDiagMenuClient));
		DiagToggleRPC(enabled, pluginDiag.m_LogPlayerStats, ERPCs.DIAG_MISC_LOG_PLAYER_STATS);
 	}
	
	//---------------------------------------------	
	static void CBMiscPermanentCrossHair(bool enabled)
	{
		PluginPermanentCrossHair crosshair = PluginPermanentCrossHair.Cast(GetPlugin(PluginPermanentCrossHair));
		crosshair.SwitchPermanentCrossHair(enabled);
	}
	
	//---------------------------------------------		
	void UpdateMiscVehicleGetOutBox()
	{
		if ( DiagMenu.GetBool(DiagMenuIDs.MISC_VEHICLE_GETOUT_BOX) )
		{
			if (m_VehicleFreeAreaBox)
			{
				m_VehicleFreeAreaBox.Destroy();
				m_VehicleFreeAreaBox = null;
			}
			
			HumanCommandVehicle hcv = GetGame().GetPlayer().GetCommand_Vehicle();
			if (!hcv)
				return;
			
			m_VehicleFreeAreaBox = hcv.GetTransport().DebugFreeAreaAtDoor( hcv.GetVehicleSeat() );
		}
		else if (m_VehicleFreeAreaBox)
		{
			m_VehicleFreeAreaBox.Destroy();
			m_VehicleFreeAreaBox = null;
		}
	}
	
	//---------------------------------------------	
	static void CBMiscToggleHud(bool enabled)
	{
		Mission mission = GetGame().GetMission();
		mission.GetHud().Show(!enabled);
		if (enabled)
		{
			mission.HideCrosshairVisibility();
		}
		else
		{
			mission.RefreshCrosshairVisibility();
		}
	}
	
	//---------------------------------------------	
	static void CBMiscEnvironmentDebug(bool enabled)
	{
		if (GetGame().IsMultiplayer())
		{
			SendDiagRPC(enabled, ERPCs.DIAG_MISC_ENVIRONMENT_DEBUG);
		}
	}
	
	void UpdateEnvironmentDebug()
	{
		if ( DiagMenu.GetBool(DiagMenuIDs.MISC_ENVIRONMENT_DEBUG) && m_EnvDebugData )
		{
			Environment.DisplayEnvDebugPlayerInfo(true, m_EnvDebugData);
		}
		else if (m_EnvDebugData)
		{
			m_EnvDebugData = null;
		}
	}
	
	//---------------------------------------------	
	static void CBMiscDisplayPlayerInfo(int value)
	{
		PluginRemotePlayerDebugClient prpdc = PluginRemotePlayerDebugClient.Cast(GetPlugin(PluginRemotePlayerDebugClient));
		prpdc.RequestPlayerInfo(PlayerBase.Cast(GetGame().GetPlayer()), value);
	}
	
	//---------------------------------------------	
	static void CBMiscUniversalTemperatureSources(bool enabled)
	{
		PluginUniversalTemperatureSourceClient client = PluginUniversalTemperatureSourceClient.Cast(GetPlugin(PluginUniversalTemperatureSourceClient));
		client.RequestUniversalTemperatureSources(PlayerBase.Cast(GetGame().GetPlayer()), enabled);
	}

	//---------------------------------------------	
	static void CBMiscBulletImpact(bool enabled, int id)
	{
		PlayerBase player = GetPlayer();	
		CachedObjectsParams.PARAM2_FLOAT_FLOAT.param1 = 32;
		CachedObjectsParams.PARAM2_FLOAT_FLOAT.param2 = 0.3;
		DiagButtonActionParams(enabled, id, ScriptCaller.Create(player.SpawnDamageDealtEffect2), CachedObjectsParams.PARAM2_FLOAT_FLOAT);
	}
	
	//---------------------------------------------
	static void CBMiscGoUnconscious(bool enabled, int id)
	{
		DiagButtonRPC(enabled, id, ERPCs.DIAG_MISC_GO_UNCONSCIOUS, true);
	}
	
	//---------------------------------------------
	static void CBMiscGoUnconsciousDelayed(bool enabled, int id)
	{
		DiagButtonRPC(enabled, id, ERPCs.DIAG_MISC_GO_UNCONSCIOUS_DELAYED, true);
	}
	
	//---------------------------------------------	
	static void CBMiscQuickRestrain(bool enabled)
	{	
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		DiagToggleRPC(enabled, player.IsQuickRestrain(), ERPCs.DIAG_MISC_QUICK_RESTRAIN);
	}
	
	//---------------------------------------------	
	static void CBMiscHairHide(bool enabled, int id)
	{
		int value = DiagMenu.GetRangeValue(DiagMenuIDs.MISC_HAIR_LEVEL);
		DiagButtonRPCIntInt(enabled, id, value, enabled, ERPCs.DIAG_MISC_HAIR_LEVEL_HIDE, true);
	}
	
	//---------------------------------------------	
	static void CBMiscHairHideAll(bool enabled)
	{
		CachedObjectsParams.PARAM2_INT_INT.param1 = -1;
		CachedObjectsParams.PARAM2_INT_INT.param2 = enabled;
		
		SendDiagRPC(CachedObjectsParams.PARAM2_INT_INT, ERPCs.DIAG_MISC_HAIR_LEVEL_HIDE, true);
	}
	
	//---------------------------------------------		
	static void CBMiscCamShake(bool enabled, int id)
	{
		DayZPlayerCamera cam = GetGame().GetPlayer().GetCurrentCamera();
		DiagButtonAction(enabled, id, ScriptCaller.Create(cam.SpawnDiagCameraShake));
	}
	
	//---------------------------------------------	
	static void CBMiscQuickFishing(bool enabled)
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		DiagToggleRPC(enabled, player.IsQuickFishing(), ERPCs.DIAG_MISC_QUICK_FISHING);
	}
	
	//---------------------------------------------
	static void CBMiscShockImpact(bool enabled, int id)
	{
		DiagButtonAction(enabled, id, ScriptCaller.Create(ShockImpact));
	}
	
	static void ShockImpact()
	{
		static float intensity;
		intensity += 0.25;
		intensity = Math.WrapFloat(intensity, 0.25, 1);
		
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		player.SpawnShockEffect(intensity);
	}
	
	//---------------------------------------------
	static void CBMiscPlugArrows(bool enabled)
	{
		GetGame().EnableEMPlugs(enabled);
	}
	
	//---------------------------------------------
	static void CBMiscTargetableByAI(bool enabled, int id)
	{
		PlayerBase player = GetPlayer();
		if (player)
			DiagToggleRPC(enabled, player.m_CanBeTargetedDebug, ERPCs.DIAG_MISC_TARGETABLE_BY_AI);
	}
	
	//---------------------------------------------
	static void CBMiscHitIndication()
	{
		PlayerBase player = GetPlayer();
		
		if (DiagMenu.GetBool(DiagMenuIDs.MISC_HIT_INDICATION_DEBUG))
		{
			HitDirectionEffectBase.m_Mode = DiagMenu.GetValue(DiagMenuIDs.MISC_HIT_INDICATION_MODE);
			HitDirectionEffectBase.m_ID = DiagMenu.GetValue(DiagMenuIDs.MISC_HIT_INDICATION_TYPE);
			HitDirectionEffectBase.m_DurationMax = DiagMenu.GetRangeValue(DiagMenuIDs.MISC_HIT_INDICATION_DURATION);
			HitDirectionEffectBase.m_BreakPointBase = DiagMenu.GetRangeValue(DiagMenuIDs.MISC_HIT_INDICATION_BREAKPOINT);
			HitDirectionEffectBase.m_DistanceAdjust = DiagMenu.GetRangeValue(DiagMenuIDs.MISC_HIT_INDICATION_DISTANCE);
			HitDirectionEffectBase.m_RotationOverride = DiagMenu.GetRangeValue(DiagMenuIDs.MISC_HIT_INDICATION_ROTATION);
			HitDirectionEffectBase.m_Scatter = DiagMenu.GetRangeValue(DiagMenuIDs.MISC_HIT_INDICATION_SCATTER);		
			
			if (player)
			{
				player.SetHitPPEEnabled(!DiagMenu.GetBool(DiagMenuIDs.MISC_HIT_INDICATION_DISABLE_PPE));
			}
		}
		else
		{
			HitDirectionEffectBase.CheckValues();
			if (player)
			{
				player.SetHitPPEEnabled(CfgGameplayHandler.GetHitIndicationPPEEnabled());
			}
		}
	}
	
	//---------------------------------------------
	static void CBMiscHitIndicationSpawnHit(bool enabled, int id)
	{		
		DiagButtonAction(enabled, id, ScriptCaller.Create(SpawnHitDirEffect));
	}
	
	static void SpawnHitDirEffect()
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		GetGame().GetMission().GetHud().SpawnHitDirEffect(player, DiagMenu.GetRangeValue(DiagMenuIDs.MISC_HIT_INDICATION_SPAWN_HIT_DIRECTION), 1.0);
	}
	
	//---------------------------------------------
	static void CBMiscFreezeEntity(bool enabled, int id)
	{
		DiagButtonAction(enabled, id, ScriptCaller.Create(FreezeEntity));
	}
	
	static void FreezeEntity()
	{
		EntityAI entity;
		if (PluginSceneManager.GetInstance() && PluginSceneManager.GetInstance().IsOpened())
		{
			// Scene Editor
			SceneObject scene_obj = PluginSceneManager.GetInstance().GetSelectedSceneObject();

			if (scene_obj)
			{
				entity = scene_obj.GetObject();
			}
		}
		else
		{
			// FreeDebugCamera
			FreeDebugCamera camera = FreeDebugCamera.GetInstance();
			entity = EntityAI.Cast(camera.GetCrosshairObject());
		}
		
		if (entity)
			entity.DisableSimulation(!entity.GetIsSimulationDisabled());
	}
	
	//---------------------------------------------
	static void CBVehicleDebugOutput(int value)
	{
		
		int bitValue = 0;
		if (value == 1)//"Basic"
		{
			bitValue = EVehicleDebugOutputType.DAMAGE_APPLIED;
		}
		if (value == 2)//"Extended"
		{
			bitValue = EVehicleDebugOutputType.DAMAGE_APPLIED | EVehicleDebugOutputType.DAMAGE_CONSIDERED;
		}
		else if (value == 3)//"Contact"
		{
			bitValue = EVehicleDebugOutputType.CONTACT;
		}
		else if (value == 4)//"Basic" + "Contact"
		{
			bitValue = EVehicleDebugOutputType.DAMAGE_APPLIED | EVehicleDebugOutputType.CONTACT;
		}
		SendDiagRPC(bitValue, ERPCs.DIAG_VEHICLE_DEBUG_OUTPUT, true);
	}
	
	//---------------------------------------------
	static void CBDumpCrashData(bool value)
	{
		if (value)
		{
			SendDiagRPC(true, ERPCs.DIAG_VEHICLES_DUMP_CRASH_DATA_REQUEST, true);
			DiagMenu.SetValue(DiagMenuIDs.VEHICLE_DUMP_CRASH_DATA, 0);
		}
		
	}
	
	//---------------------------------------------
	static void SimulateMode(bool enabled, int id, int rpc)
	{
		int value = DiagMenu.GetRangeValue(DiagMenuIDs.SIMULATE_MODE);
		
		switch (value)
		{
			case 0:
			{
				DiagButtonRPC(enabled, id, rpc, true);
				break;
			}
			case 1:
			{
				DiagButtonRPCSelf(enabled, id, rpc);
				break;
			}
			case 2:
			{
				DiagButtonRPC(enabled, id, rpc, true);
				DiagButtonRPCSelf(enabled, id, rpc);
				break;
			}
		}
	}
	
	//---------------------------------------------
	static void CBSimulateInfiniteLoop(bool enabled, int id)
	{
		SimulateMode(enabled, id, ERPCs.DIAG_SIMULATE_INFINITE_LOOP);
	}
	
	//---------------------------------------------
	static void CBSimulateNullPointer(bool enabled, int id)
	{
		SimulateMode(enabled, id, ERPCs.DIAG_SIMULATE_NULL_POINTER);
	}
	
	//---------------------------------------------
	static void CBSimulateDivisionByZero(bool enabled, int id)
	{
		SimulateMode(enabled, id, ERPCs.DIAG_SIMULATE_DIVISION_BY_ZERO);
	}
	
	//---------------------------------------------
	static void CBSimulateErrorFunction(bool enabled, int id)
	{
		SimulateMode(enabled, id, ERPCs.DIAG_SIMULATE_ERROR_FUNCTION);
	}
	
	//---------------------------------------------
	static void CBWeaponParticles(bool enabled)
	{
		DiagToggleRPCServer(enabled, PrtTest.m_GunParticlesState, ERPCs.DIAG_WEAPON_PARTICLES);
	}

	//---------------------------------------------	
	static void CBWeaponDebug(bool enabled)
	{
		PlayerBase player = GetPlayer();
		DiagToggleAction(enabled, ScriptCaller.Create(player.ShowWeaponDebug), player.IsWeaponDebugEnabled());
	}
	
	//---------------------------------------------	
	static void CBWeaponAimNoise(bool enabled)
	{
		PlayerBase player = GetPlayer();
		if (player)
		{
			DayZPlayerImplementAiming aimModel = player.GetAimingModel();
			DiagToggleAction(enabled, ScriptCaller.Create(aimModel.SetAimNoiseAllowed), aimModel.IsAimNoiseAllowed());
		}
	}
	
	//---------------------------------------------	
	static void CBWeaponRecoil(bool enabled)
	{
		PlayerBase player = GetPlayer();
		if (player)
		{
			DayZPlayerImplementAiming aimModel = player.GetAimingModel();
			DiagToggleRPC(enabled, aimModel.IsProceduralRecoilEnabled(), ERPCs.DIAG_WEAPON_RECOIL);
		}
	}
	
	//---------------------------------------------
	static void CBWeaponUnlimitedAmmo(bool enabled)
	{
		DiagDebugAction(DebugActionType.UNLIMITED_AMMO, enabled);
	}
	
	//---------------------------------------------
	static void CBWeaponBurstVersion(int value)
	{
		PlayerBase player = GetPlayer();
		WeaponManager wpnMngr = player.GetWeaponManager();
		DiagToggleRPC(value, wpnMngr.GetBurstOption(), ERPCs.DIAG_WEAPON_BURST_VERSION);
	}
	
	//---------------------------------------------
	static void CBBleedingSources(bool enabled)
	{
		PlayerBase player = GetPlayer();
		player.GetBleedingManagerRemote().SetDiag(enabled);
	}
	
	//---------------------------------------------
	static void CBBleedingDisableBloodLoss(bool enabled)
	{		
		SendDiagRPC(enabled, ERPCs.DIAG_BLEEDING_DISABLE_BLOOD_LOSS, true);
	}
	
	//---------------------------------------------
	static void CBBleedingActivateSource(int value)
	{
		SendDiagRPC(value, ERPCs.DIAG_BLEEDING_ACTIVATE_SOURCE, true);	
	}
	
	//---------------------------------------------
	static void CBBleedingActivateAllSources(bool enabled, int id)
	{
		DiagButtonRPC(enabled, id, ERPCs.DIAG_BLEEDING_ACTIVATE_ALL_SOURCES, true);
	}
	
	//---------------------------------------------
	static void CBBleedingActivateSourceLevel(int value)
	{
		PlayerBase player = GetPlayer();
		BleedingSourcesManagerBase mgr = player.GetBleedingManagerRemote();
			
		int idx = 100; //invalid index, just deletes all
		if (value != 0)
		{
			string bone = ""; //lower-case!
			switch (value)
			{
				case 1: //PlayerConstants.BLEEDING_SOURCE_FLOW_MODIFIER_LOW
					bone = "lefttoebase";
					break;
					
				case 2: //PlayerConstants.BLEEDING_SOURCE_FLOW_MODIFIER_MEDIUM
					bone = "leftarm";
					break;
					
				case 3: //PlayerConstants.BLEEDING_SOURCE_FLOW_MODIFIER_HIGH
					bone = "neck";
					break;
			}
			
			int count = mgr.m_BleedingSourceZone.Count();
				
			for (int i = 0; i < count; ++i)
			{
				if (mgr.m_BleedingSourceZone.GetKey(i) == bone)
				{
					idx = i;
					break;
				}
			}
		}
		
		CBBleedingActivateSource(idx);
	}
	
	//---------------------------------------------
	static void CBBleedingReload(bool enabled, int id)
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		BleedingSourcesManagerRemote bsMngrRem = player.GetBleedingManagerRemote();
		DiagButtonAction(enabled, id, ScriptCaller.Create(bsMngrRem.Reload));
	}
	
	//---------------------------------------------
	static void CBBBleedingIndicators()
	{
		bool diagEnable = DiagMenu.GetBool(DiagMenuIDs.BLEEDINGINDICATORS);
		if (diagEnable != DbgBleedingIndicationStaticInfo.m_DbgEnableBleedingIndication)
		{
			DbgBleedingIndicationStaticInfo.m_DbgEnableBleedingIndication = diagEnable;
			PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
			if (player && player.GetBleedingManagerRemote())
			{
				player.GetBleedingManagerRemote().Reload();
			}
		}
		
		DbgBleedingIndicationStaticInfo.m_DbgUseOverrideValues = DiagMenu.GetBool(DiagMenuIDs.BLEEDINGINDICATORS_USEVALUEOVERRIDES);
		
		if (DbgBleedingIndicationStaticInfo.m_DbgUseOverrideValues)
		{
			DbgBleedingIndicationStaticInfo.m_DbgSequenceDuration = DiagMenu.GetRangeValue(DiagMenuIDs.BLEEDINGINDICATORS_SEQUENCE_DURATION);
			DbgBleedingIndicationStaticInfo.m_DbgDropRotationRandom = DiagMenu.GetBool(DiagMenuIDs.BLEEDINGINDICATORS_DROP_RANDOMROTATION);
			DbgBleedingIndicationStaticInfo.m_DbgDropDurationBase = DiagMenu.GetRangeValue(DiagMenuIDs.BLEEDINGINDICATORS_DROP_BASEDURATION);
			DbgBleedingIndicationStaticInfo.m_DbgDropMinDelay = DiagMenu.GetRangeValue(DiagMenuIDs.BLEEDINGINDICATORS_DROP_MINDELAY);
			DbgBleedingIndicationStaticInfo.m_DbgDropMaxDelay = DiagMenu.GetRangeValue(DiagMenuIDs.BLEEDINGINDICATORS_DROP_MAXDELAY);
			DbgBleedingIndicationStaticInfo.m_DbgDropProgressTreshold = DiagMenu.GetRangeValue(DiagMenuIDs.BLEEDINGINDICATORS_DROP_PROGRESSTHRESHOLD);
			DbgBleedingIndicationStaticInfo.m_DbgDropStartSize = DiagMenu.GetRangeValue(DiagMenuIDs.BLEEDINGINDICATORS_DROP_STARTSIZE);
			DbgBleedingIndicationStaticInfo.m_DbgDropEndSize = DiagMenu.GetRangeValue(DiagMenuIDs.BLEEDINGINDICATORS_DROP_ENDSIZE);
			DbgBleedingIndicationStaticInfo.m_DbgDropSizeVariationMin = DiagMenu.GetRangeValue(DiagMenuIDs.BLEEDINGINDICATORS_DROP_RNDSCALEMIN);
			DbgBleedingIndicationStaticInfo.m_DbgDropSizeVariationMax = DiagMenu.GetRangeValue(DiagMenuIDs.BLEEDINGINDICATORS_DROP_RNDSCALEMAX);
			DbgBleedingIndicationStaticInfo.m_DbgDropScatter = (int)DiagMenu.GetRangeValue(DiagMenuIDs.BLEEDINGINDICATORS_DROP_SCATTER);
			DbgBleedingIndicationStaticInfo.m_DbgDropSlideDistance = DiagMenu.GetRangeValue(DiagMenuIDs.BLEEDINGINDICATORS_DROP_SLIDEDISTANCE);
			
			DbgBleedingIndicationStaticInfo.m_DbgDropDesaturate = DiagMenu.GetBool(DiagMenuIDs.BLEEDINGINDICATORS_DROP_COLOR_ENABLEDESATURATION);
			DbgBleedingIndicationStaticInfo.m_DbgDesaturationEnd = DiagMenu.GetRangeValue(DiagMenuIDs.BLEEDINGINDICATORS_DROP_COLOR_DESATURATIONEND);
			DbgBleedingIndicationStaticInfo.m_DbgDropColorRed = (int)DiagMenu.GetRangeValue(DiagMenuIDs.BLEEDINGINDICATORS_DROP_COLOR_R);
			DbgBleedingIndicationStaticInfo.m_DbgDropColorGreen = (int)DiagMenu.GetRangeValue(DiagMenuIDs.BLEEDINGINDICATORS_DROP_COLOR_G);
			DbgBleedingIndicationStaticInfo.m_DbgDropColorBlue = (int)DiagMenu.GetRangeValue(DiagMenuIDs.BLEEDINGINDICATORS_DROP_COLOR_B);
			DbgBleedingIndicationStaticInfo.m_DbgDropColorAlphaStart = (int)DiagMenu.GetRangeValue(DiagMenuIDs.BLEEDINGINDICATORS_DROP_COLOR_A_START);
			DbgBleedingIndicationStaticInfo.m_DbgDropColorAlphaEnd = (int)DiagMenu.GetRangeValue(DiagMenuIDs.BLEEDINGINDICATORS_DROP_COLOR_A_END);
		}
	}
	
	//---------------------------------------------
	static void CBLogsActions(bool enabled)
	{
		DiagToggleRPC(enabled, LogManager.IsActionLogEnable(), ERPCs.DIAG_LOGS_ACTIONS);
	}
	
	//---------------------------------------------
	static void CBLogsWeapons(bool enabled)
	{
		DiagToggleRPC(enabled, LogManager.IsWeaponLogEnable(), ERPCs.DIAG_LOGS_WEAPONS);
	}
	
	//---------------------------------------------
	static void CBLogsSymptoms(bool enabled)
	{
		DiagToggleRPC(enabled, LogManager.IsSymptomLogEnable(), ERPCs.DIAG_LOGS_SYMPTOMS);
	}
	
	//---------------------------------------------
	static void CBLogsInventoryMove(bool enabled)
	{
		DiagToggleRPC(enabled, LogManager.IsInventoryMoveLogEnable(), ERPCs.DIAG_LOGS_INVENTORY_MOVE);
	}
	
	//---------------------------------------------
	static void CBLogsInventoryReservation(bool enabled)
	{
		DiagToggleRPC(enabled, LogManager.IsInventoryReservationLogEnable(), ERPCs.DIAG_LOGS_INVENTORY_RESERVATION);
	}
	
	//---------------------------------------------
	static void CBLogsInventoryHFSM(bool enabled)
	{
		DiagToggleRPC(enabled, LogManager.IsInventoryHFSMLogEnable(), ERPCs.DIAG_LOGS_INVENTORY_HFSM);
	}
	
	//---------------------------------------------
	static void CBTriggerDebug(bool enabled)
	{
		EnableDebugSystemClient(ESubscriberSystems.TRIGGERS, enabled);
	}
	
	//---------------------------------------------
	static void CBBaseBuildingWood(bool enabled)
	{
		SendDiagRPC(enabled, ERPCs.DIAG_BASEBUILDING_WOOD, true);
	}
	
	//---------------------------------------------
	static void CBBaseBuildingGate(bool enabled)
	{
		SendDiagRPC(enabled, ERPCs.DIAG_BASEBUILDING_GATE, true);
	}
	
	//---------------------------------------------
	static void CBFinishersFinisherForced(int value)
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		DiagToggleRPC(value - 1, player.GetMeleeCombat().DebugGetForcedFinisherType(), ERPCs.DIAG_FINISHERS_FORCE_FINISHER);
	}
	
	//---------------------------------------------		
	static void CBCameraToolsEnableRemoteCamera(bool enabled)
	{
		PlayerBase player = GetPlayer();
		
		if (!player)
			return;
		
		if (!enabled && player.m_CameraToolsMenuClient)
		{
			player.m_CameraToolsMenuClient.DelayedDestroy();
		}
		
		auto param = new Param2<bool, EntityAI>(enabled, GetGame().GetPlayer());
		GetGame().RPCSingleParam( null, ERPCs.DIAG_CAMERATOOLS_CAM_SUBSCRIBE, param, true );
	}
	
	//---------------------------------------------
	// Misc		
	//---------------------------------------------
	static PlayerBase GetPlayer()
	{
		if (!GetGame())
			return null;
		
		return PlayerBase.Cast(GetGame().GetPlayer());
	}
	
	//---------------------------------------------
	// DebugAction		
	//---------------------------------------------
	static void DiagDebugAction(DebugActionType type, bool enable)
	{
		int mask;
		if (enable)
		{
			mask = (ItemBase.GetDebugActionsMask() | type);
		}
		else
		{
			mask = (ItemBase.GetDebugActionsMask() & (~type));
		}
		
		DiagToggleRPCAction(mask, ScriptCaller.Create(ItemBase.SetDebugActionsMask), ItemBase.HasDebugActionsMask(type), ERPCs.DIAG_ITEM_DEBUG_ACTIONS);
	}
	
	//---------------------------------------------
	// SubscriberClient
	//---------------------------------------------
	static void EnableDebugSystemClient(ESubscriberSystems system, bool enable)
	{
		int mask;
		if (enable)
		{
			mask = (m_SystemsMask | system);//turn on bit
		}
		else
		{
			mask = (m_SystemsMask & (~system));//turn off bit
		}
		
		if (mask != m_SystemsMask)
		{
			CachedObjectsParams.PARAM1_INT.param1 = mask;
			GetGame().RPCSingleParam( GetGame().GetPlayer(), ERPCs.DEV_DIAGMENU_SUBSCRIBE, CachedObjectsParams.PARAM1_INT, true, GetGame().GetPlayer().GetIdentity() );
			m_SystemsMask = mask;
		}
	}
	
	
	//---------------------------------------------
	// Helper diag functions			
	//---------------------------------------------
	static void DiagButtonActionParams(bool enabled, int id, ScriptCaller action, Param params)
	{
		if ( enabled )
		{
			action.Invoke(params);				
			DiagMenu.SetValue(id, false);
		}
	}
	
	//---------------------------------------------
	static void DiagButtonAction(bool enabled, int id, ScriptCaller action)
	{
		DiagButtonActionParams(enabled, id, action, null);
	}
	
	//---------------------------------------------
	static void DiagButtonRPC(bool enabled, int id, int rpc, bool serverOnly = false)
	{
		ScriptCaller action = ScriptCaller.Create(SendDiagRPCBasic);
		SendDiagRPCBasicParam params = new SendDiagRPCBasicParam(rpc, serverOnly);
		
		DiagButtonActionParams(enabled, id, action, params);
	}
	
	//---------------------------------------------
	static void DiagButtonRPCSelf(bool enabled, int id, int rpc)
	{
		ScriptCaller action = ScriptCaller.Create(SendDiagRPCSelfBasic);
		SendDiagRPCSelfBasicParam params = new SendDiagRPCSelfBasicParam(rpc);
		
		DiagButtonActionParams(enabled, id, action, params);
	}
	
	//---------------------------------------------
	static void DiagButtonRPCIntInt(bool enabled, int id, int value, int value2, int rpc, bool serverOnly = false)
	{
		ScriptCaller action = ScriptCaller.Create(SendDiagRPCParam);
		
		CachedObjectsParams.PARAM2_INT_INT.param1 = value;
		CachedObjectsParams.PARAM2_INT_INT.param2 = value2;
		SendDiagRPCParamParam params = new SendDiagRPCParamParam(rpc, serverOnly, CachedObjectsParams.PARAM2_INT_INT);
		
		DiagButtonActionParams(enabled, id, action, params);
	}
	
	//---------------------------------------------
	static void DiagToggleRPC(bool value, int current, int rpc)
	{
		if (value != current)
		{
			SendDiagRPC(value, rpc, false);
		}
	}
	
	//---------------------------------------------
	static void DiagToggleRPC(int value, int current, int rpc)
	{
		if (value != current)
		{
			SendDiagRPC(value, rpc, false);
		}
	}
	
	//---------------------------------------------
	static void DiagToggleRPC(float value, int current, int rpc)
	{
		if (value != current)
		{
			SendDiagRPC(value, rpc, false);
		}
	}
	
	//---------------------------------------------
	static void DiagToggleRPCServer(bool value, out bool current, int rpc)
	{
		if (value != current)
		{
			current = value;
			SendDiagRPC(value, rpc, true);		
		}
	}
	
	//---------------------------------------------
	static void DiagToggleRPCServer(int value, out int current, int rpc)
	{
		if (value != current)
		{
			current = value;
			SendDiagRPC(value, rpc, true);		
		}
	}
	
	//---------------------------------------------
	static void DiagToggleRPCServer(float value, out float current, int rpc)
	{
		if (value != current)
		{
			current = value;
			SendDiagRPC(value, rpc, true);		
		}
	}
	
	//---------------------------------------------
	static void DiagToggleRPCAction(int value, ScriptCaller action, int current, int rpc)
	{
		if (value != current)
		{
			SendDiagRPC(value, rpc);
			action.Invoke(value);
		}
	}
	
	//---------------------------------------------
	static void DiagToggleAction(int value, ScriptCaller action, int current)
	{
		if (value != current)
		{
			action.Invoke(value);
		}
	}
		
	//---------------------------------------------
	static void SendDiagRPCSelf(Param value, int rpc)
	{
		PlayerBase player = GetPlayer();
		if (player) 
		{
			GetGame().RPCSelfSingleParam(player, rpc, value);
		}
 	}
	
	//---------------------------------------------
	static void SendDiagRPCSelfBasic(SendDiagRPCSelfBasicParam param)
	{
		SendDiagRPCSelf(null, param.param1);
 	}
	
	//---------------------------------------------
	static void SendDiagRPCSelfParam(SendDiagRPCSelfParamParam param)
	{
		SendDiagRPCSelf(param.param2, param.param1);
 	}
		
	//---------------------------------------------
	static void SendDiagRPC(Param value, int rpc, bool serverOnly = false)
	{
		PlayerBase player = GetPlayer();
		if (player) 
		{
			PlayerIdentity playerIdentity = player.GetIdentity();
			if (GetGame().IsMultiplayer() && playerIdentity)
			{
				GetGame().RPCSingleParam(player, rpc, value, true, playerIdentity);
			}
			
			if (!GetGame().IsMultiplayer() || !serverOnly)
			{
				SendDiagRPCSelf(value, rpc);
			}
		}
 	}
	
	//---------------------------------------------
	static void SendDiagRPCBasic(SendDiagRPCBasicParam param)
	{
		SendDiagRPC(null, param.param1, param.param2);
 	}
	
	//---------------------------------------------
	static void SendDiagRPCParam(SendDiagRPCParamParam param)
	{
		SendDiagRPC(param.param3, param.param1, param.param2);
 	}
	
	//---------------------------------------------
	static void SendDiagRPC(int rpc, bool serverOnly = false)
	{
		SendDiagRPC(null, rpc, serverOnly);
 	}
	
	//---------------------------------------------
	static void SendDiagRPC(bool value, int rpc, bool serverOnly = false)
	{
		CachedObjectsParams.PARAM1_BOOL.param1 = value;
		SendDiagRPC(CachedObjectsParams.PARAM1_BOOL, rpc, serverOnly);
 	}
	
	//---------------------------------------------
	static void SendDiagRPC(int value, int rpc, bool serverOnly = false)
	{
		CachedObjectsParams.PARAM1_INT.param1 = value;
		SendDiagRPC(CachedObjectsParams.PARAM1_INT, rpc, serverOnly);
 	}
	
	//---------------------------------------------
	static void SendDiagRPC(float value, int rpc, bool serverOnly = false)
	{
		CachedObjectsParams.PARAM1_FLOAT.param1 = value;
		SendDiagRPC(CachedObjectsParams.PARAM1_FLOAT, rpc, serverOnly);
 	}
#endif
}