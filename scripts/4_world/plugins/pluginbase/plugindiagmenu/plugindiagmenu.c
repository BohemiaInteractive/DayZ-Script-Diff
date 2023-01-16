// For modding, see PluginDiagMenuModding.c
// !!! MODDING DISCLAIMER: These are debug functionality files, if you are thinking about modding the vanilla ones, do so at your own risk
//                         These files will not be maintained with the thought of "what if a modder modded this" (Excluding the modding functionality of course)
//                         Which is why the modding functionality was developed with the thought of the modded ones having their own isolated safe space

enum DebugActionType
{
	GENERIC_ACTIONS = 1,
	PLAYER_AGENTS = 2,
	UNLIMITED_AMMO = 4,
};

#ifdef DIAG_DEVELOPER
enum ESubscriberSystems
{
	TRIGGERS = 0x00000001,
	//SYSTEM2 = 0x00000002,
	//SYSTEM3 = 0x00000004,
	//SYSTEM4 = 0x00000008,
}

typedef Param1<int> SendDiagRPCSelfBasicParam;
typedef Param2<int,Param> SendDiagRPCSelfParamParam;

typedef Param2<int,bool> SendDiagRPCBasicParam;
typedef Param3<int,bool,Param> SendDiagRPCParamParam;
#endif

class PluginDiagMenu : PluginBase
{
#ifdef DIAG_DEVELOPER
	
	int m_ModdedDiagID = DiagMenuIDs.MODDED_MENU;
	
	// LEVEL 3 - Script > Misc > Hair Hiding
	ref map<int,bool> m_HairHidingStateMap;
	ref TStringArray m_HairSelectionArray;
	int m_TotalHairLevelsAdjusted;
	
	override void OnInit()
	{				
		//----------------------
		m_HairHidingStateMap = new map<int,bool>;
		m_HairSelectionArray = new TStringArray;
		
		g_Game.ConfigGetTextArray("cfgVehicles Head_Default simpleHiddenSelections", m_HairSelectionArray);
		m_TotalHairLevelsAdjusted = m_HairSelectionArray.Count() - 1;
		for (int i = 0; i < m_HairSelectionArray.Count(); ++i)
		{
			m_HairHidingStateMap.Insert(i, 1); //all considered "shown" on init
		}
		
		//----------------------
		
		DiagMenu.InitScriptDiags();
		
		RegisterDiags();
	}
	
	override void OnDestroy()
	{
		DiagMenu.ClearScriptDiags();
	}
	
	protected void RegisterDiags()
	{
		RegisterDeveloperDiags();
		RegisterModdedDiagsIDs();
		RegisterModdedDiags();
		
		if (m_ModdedDiagID == DiagMenuIDs.MODDED_MENU)
		{
			DiagMenu.Unregister(DiagMenuIDs.MODDED_MENU);
		}
	}
	
	private void RegisterDeveloperDiags()
	{
		//---------------------------------------------------------------
		// LEVEL 0
		//---------------------------------------------------------------
 		DiagMenu.RegisterMenu(DiagMenuIDs.SCRIPTS_MENU, "Script", -1);
		{
			//---------------------------------------------------------------
			// LEVEL 1 - Script
			//---------------------------------------------------------------
			DiagMenu.RegisterMenu(DiagMenuIDs.VEHICLES, "Vehicles", DiagMenuIDs.SCRIPTS_MENU);
			{
				//---------------------------------------------------------------
				// LEVEL 2 - Script > Vehicles
				//---------------------------------------------------------------
				DiagMenu.RegisterItem(DiagMenuIDs.VEHICLE_DEBUG_OUTPUT, "lalt+6", "Crash Log", DiagMenuIDs.VEHICLES, "None, Basic, Extended, Contact, Basic+Contact");
				DiagMenu.RegisterBool(DiagMenuIDs.VEHICLE_DUMP_CRASH_DATA, "lalt+7", "Dump Crash Data", DiagMenuIDs.VEHICLES);
			}
			//---------------------------------------------------------------
			// LEVEL 1 - Script
			//---------------------------------------------------------------
			DiagMenu.RegisterMenu(DiagMenuIDs.TRANSFER_VALUES_MENU, "TransferValues", DiagMenuIDs.SCRIPTS_MENU);
			{
				//---------------------------------------------------------------
				// LEVEL 2 - Script > TransferValues
				//---------------------------------------------------------------		
				DiagMenu.RegisterBool(DiagMenuIDs.TRANSFER_VALUES_SHOW, "", "ShowValues", DiagMenuIDs.TRANSFER_VALUES_MENU);
			}
			
			//---------------------------------------------------------------
			// LEVEL 1 - Script
			//---------------------------------------------------------------
			DiagMenu.RegisterMenu(DiagMenuIDs.CRAFTING_MENU, "Crafting", DiagMenuIDs.SCRIPTS_MENU);
			{
				//---------------------------------------------------------------
				// LEVEL 2 - Script > Crafting
				//---------------------------------------------------------------
				DiagMenu.RegisterBool(DiagMenuIDs.CRAFTING_GENERATE, "", "Generate Cache", DiagMenuIDs.CRAFTING_MENU);
				DiagMenu.RegisterBool(DiagMenuIDs.CRAFTING_INSTANT, "", "Debug Insta Crafting", DiagMenuIDs.CRAFTING_MENU);
				DiagMenu.RegisterBool(DiagMenuIDs.CRAFTING_DUMP, "", "Dump recipes to file", DiagMenuIDs.CRAFTING_MENU);
			}
			
			//---------------------------------------------------------------
			// LEVEL 1 - Script
			//---------------------------------------------------------------
			DiagMenu.RegisterMenu(DiagMenuIDs.CHEATS_MENU, "Cheats", DiagMenuIDs.SCRIPTS_MENU);
			{
				//---------------------------------------------------------------
				// LEVEL 2 - Script > Cheats
				//---------------------------------------------------------------
				DiagMenu.RegisterBool(DiagMenuIDs.CHEATS_MODIFIERS, "lalt+2", "Tick Modifiers", DiagMenuIDs.CHEATS_MENU);
					DiagMenu.SetValue(DiagMenuIDs.CHEATS_MODIFIERS, true);
				DiagMenu.RegisterBool(DiagMenuIDs.CHEATS_KILL_PLAYER, "", "Kill Player", DiagMenuIDs.CHEATS_MENU);
				DiagMenu.RegisterItem(DiagMenuIDs.CHEATS_INVINCIBILITY, "lalt+1", "Invincibility (IDDQD)", DiagMenuIDs.CHEATS_MENU, "Disabled,Partial,Full");
				DiagMenu.RegisterBool(DiagMenuIDs.CHEATS_DISABLE_STAMINA, "", "Disable stamina", DiagMenuIDs.CHEATS_MENU);
				DiagMenu.RegisterBool(DiagMenuIDs.CHEATS_RESET_PLAYER, "", "Reset Player", DiagMenuIDs.CHEATS_MENU);
				DiagMenu.RegisterBool(DiagMenuIDs.CHEATS_RESET_PLAYER_MAX, "lalt+3", "Reset Player Max", DiagMenuIDs.CHEATS_MENU);
				DiagMenu.RegisterBool(DiagMenuIDs.CHEATS_INVENTORY_ACCESS, "", "Inventory Access", DiagMenuIDs.CHEATS_MENU);
				DiagMenu.RegisterBool(DiagMenuIDs.CHEATS_FIX_ITEMS, "", "Fix Inventory Items", DiagMenuIDs.CHEATS_MENU);
			}
			
			//---------------------------------------------------------------
			// LEVEL 1 - Script
			//---------------------------------------------------------------
			DiagMenu.RegisterMenu(DiagMenuIDs.PLAYER_AGENTS_MENU, "Player Agents", DiagMenuIDs.SCRIPTS_MENU);
			{
				//---------------------------------------------------------------
				// LEVEL 2 - Script > Player Agents
				//---------------------------------------------------------------
				DiagMenu.RegisterBool(DiagMenuIDs.PLAYER_AGENTS_INJECTS_SHOW, "lalt+5", "Allow Inject Actions", DiagMenuIDs.PLAYER_AGENTS_MENU);
			}
			
			//---------------------------------------------------------------
			// LEVEL 1 - Script
			//---------------------------------------------------------------
			DiagMenu.RegisterMenu(DiagMenuIDs.SOFT_SKILLS_MENU, "Soft Skills", DiagMenuIDs.SCRIPTS_MENU);
			{
				//---------------------------------------------------------------
				// LEVEL 2 - Script > Soft Skills
				//---------------------------------------------------------------
				DiagMenu.RegisterBool(DiagMenuIDs.SOFT_SKILLS_SHOW_DEBUG, "", "Show Debug", DiagMenuIDs.SOFT_SKILLS_MENU);
				DiagMenu.RegisterBool(DiagMenuIDs.SOFT_SKILLS_TOGGLE_STATE, "", "Enable SoftSkills", DiagMenuIDs.SOFT_SKILLS_MENU);
				DiagMenu.RegisterBool(DiagMenuIDs.SOFT_SKILLS_TOGGLE_MODEL, "", "Enable linear model", DiagMenuIDs.SOFT_SKILLS_MENU);
				DiagMenu.RegisterRange(DiagMenuIDs.SOFT_SKILLS_SPECIALTY_VALUE, "", "Set specialty value", DiagMenuIDs.SOFT_SKILLS_MENU, "-1, 1, 0, 0.01");
			}
			
			//---------------------------------------------------------------
			// LEVEL 1 - Script
			//---------------------------------------------------------------
			DiagMenu.RegisterMenu(DiagMenuIDs.LIFESPAN_MENU, "Lifespan", DiagMenuIDs.SCRIPTS_MENU);
			{
				//---------------------------------------------------------------
				// LEVEL 2 - Script > Lifespan
				//---------------------------------------------------------------
				DiagMenu.RegisterBool(DiagMenuIDs.LIFESPAN_BLOODY_HANDS, "", "Bloody hands", DiagMenuIDs.LIFESPAN_MENU);
				DiagMenu.RegisterRange(DiagMenuIDs.LIFESPAN_PLAYTIME_UPDATE, "", "Playtime in minutes", DiagMenuIDs.LIFESPAN_MENU, "0, 600, 0, 10");
			}
			
			//---------------------------------------------------------------
			// LEVEL 1 - Script
			//---------------------------------------------------------------
			DiagMenu.RegisterMenu(DiagMenuIDs.MISC_MENU, "Misc", DiagMenuIDs.SCRIPTS_MENU);
			{
				//---------------------------------------------------------------
				// LEVEL 2 - Script > Misc
				//---------------------------------------------------------------
				DiagMenu.RegisterBool(DiagMenuIDs.MISC_DISABLE_PERSONAL_LIGHT, "", "Disable Personal Light", DiagMenuIDs.MISC_MENU);
				DiagMenu.RegisterBool(DiagMenuIDs.MISC_ITEM_DEBUG_ACTIONS, "", "Item Debug Actions", DiagMenuIDs.MISC_MENU); // Is enabled by default now
				DiagMenu.RegisterBool(DiagMenuIDs.MISC_LOG_PLAYER_STATS, "", "Log Player Stats", DiagMenuIDs.MISC_MENU);
				DiagMenu.RegisterMenu(DiagMenuIDs.MISC_ACTION_TARGETS_MENU, "Action Targets", DiagMenuIDs.MISC_MENU);
				{
					//---------------------------------------------------------------
					// LEVEL 3 - Script > Misc > Action Targets
					//---------------------------------------------------------------
					DiagMenu.RegisterBool(DiagMenuIDs.MISC_ACTION_TARGETS_NEW, "", "New AT Selection", DiagMenuIDs.MISC_ACTION_TARGETS_MENU, true);
					DiagMenu.RegisterBool(DiagMenuIDs.MISC_ACTION_TARGETS_DEBUG, "", "Show Debug", DiagMenuIDs.MISC_ACTION_TARGETS_MENU);
					DiagMenu.RegisterBool(DiagMenuIDs.MISC_ACTION_TARGETS_SELPOS_DEBUG, "", "Show selection pos debug", DiagMenuIDs.MISC_ACTION_TARGETS_MENU);
				}
				//---------------------------------------------------------------
				// LEVEL 2 - Script > Misc
				//---------------------------------------------------------------
				DiagMenu.RegisterBool(DiagMenuIDs.MISC_PERMANENT_CROSSHAIR, "", "Enable permanent crosshair", DiagMenuIDs.MISC_MENU);
				DiagMenu.RegisterBool(DiagMenuIDs.MISC_VEHICLE_GETOUT_BOX, "", "Debug transport freespace", DiagMenuIDs.MISC_MENU);
				DiagMenu.RegisterBool(DiagMenuIDs.MISC_VEHICLE_GETOUT_RAYCAST, "", "Debug transport get out raycast", DiagMenuIDs.MISC_MENU);
				DiagMenu.RegisterBool(DiagMenuIDs.MISC_TOGGLE_HUD, "", "Toggle HUD on/off", DiagMenuIDs.MISC_MENU);
				DiagMenu.RegisterBool(DiagMenuIDs.MISC_ENVIRONMENT_DEBUG, "", "Show Environment stats", DiagMenuIDs.MISC_MENU);
				DiagMenu.RegisterRange(DiagMenuIDs.MISC_DISPLAY_PLAYER_INFO, "", "Display Player Info", DiagMenuIDs.MISC_MENU, "0,2,0,1");
				DiagMenu.RegisterBool(DiagMenuIDs.MISC_UNIVERSAL_TEMPERATURE_SOURCES, "lalt+u", "Universal Temp Sources", DiagMenuIDs.MISC_MENU);
				DiagMenu.RegisterBool(DiagMenuIDs.MISC_DRAW_CHECKERBOARD, "", "Draw Checkerboard on screen", DiagMenuIDs.MISC_MENU);
				DiagMenu.RegisterBool(DiagMenuIDs.MISC_BULLET_IMPACT, "", "BulletImpact", DiagMenuIDs.MISC_MENU);
				DiagMenu.RegisterBool(DiagMenuIDs.MISC_PRESENCE_NOTIFIER_DBG, "", "Show Presence to AI dbg", DiagMenuIDs.MISC_MENU);
				DiagMenu.RegisterBool(DiagMenuIDs.MISC_GO_UNCONSCIOUS, "", "Go Unconscious", DiagMenuIDs.MISC_MENU);
				DiagMenu.RegisterBool(DiagMenuIDs.MISC_GO_UNCONSCIOUS_DELAYED, "", "Uncons. in 10sec", DiagMenuIDs.MISC_MENU);
				DiagMenu.RegisterBool(DiagMenuIDs.MISC_QUICK_RESTRAIN, "", "Quick Restrain", DiagMenuIDs.MISC_MENU);
				DiagMenu.RegisterMenu(DiagMenuIDs.MISC_HAIR_MENU, "Hair Hiding", DiagMenuIDs.MISC_MENU);
				{
					//---------------------------------------------------------------
					// LEVEL 3 - Script > Misc > Hair Hiding
					//---------------------------------------------------------------
					DiagMenu.RegisterBool(DiagMenuIDs.MISC_HAIR_DISPLAY_DEBUG, "", "Display Debug", DiagMenuIDs.MISC_HAIR_MENU);
					DiagMenu.RegisterRange(DiagMenuIDs.MISC_HAIR_LEVEL, "", "Hair Level#", DiagMenuIDs.MISC_HAIR_MENU, string.Format("0,%1,0,1", m_TotalHairLevelsAdjusted));
					DiagMenu.RegisterBool(DiagMenuIDs.MISC_HAIR_LEVEL_HIDE, "", "Toggle Hair Level", DiagMenuIDs.MISC_HAIR_MENU);
					DiagMenu.RegisterBool(DiagMenuIDs.MISC_HAIR_HIDE_ALL, "", "Hide/Show All", DiagMenuIDs.MISC_HAIR_MENU);
				}
				//---------------------------------------------------------------
				// LEVEL 2 - Script > Misc
				//---------------------------------------------------------------
				DiagMenu.RegisterBool(DiagMenuIDs.MISC_CAM_SHAKE, "", "Simulate Cam Shake", DiagMenuIDs.MISC_MENU);
				DiagMenu.RegisterBool(DiagMenuIDs.MISC_QUICK_FISHING, "", "Quick Fishing", DiagMenuIDs.MISC_MENU);
				DiagMenu.RegisterBool(DiagMenuIDs.MISC_HOLOGRAM, "lctrl+h", "Hologram placing debug", DiagMenuIDs.MISC_MENU);				
				DiagMenu.RegisterBool(DiagMenuIDs.MISC_SHOCK_IMPACT, "lalt+8", "ShockHitEffect", DiagMenuIDs.MISC_MENU);
				DiagMenu.RegisterBool(DiagMenuIDs.MISC_SHOW_PLUG_ARROWS, "", "Show Energy Manager Plug Arrows", DiagMenuIDs.MISC_MENU);
				DiagMenu.RegisterBool(DiagMenuIDs.MISC_BREATH_VAPOR_LVL, "", "Breath Vapor", DiagMenuIDs.MISC_MENU);
					DiagMenu.SetValue(DiagMenuIDs.MISC_BREATH_VAPOR_LVL, true);				
				DiagMenu.RegisterBool(DiagMenuIDs.MISC_TARGETABLE_BY_AI, "", "Toggle Targetable By AI", DiagMenuIDs.MISC_MENU);
					DiagMenu.SetValue(DiagMenuIDs.MISC_TARGETABLE_BY_AI, true);
				DiagMenu.RegisterMenu(DiagMenuIDs.MISC_HIT_INDICATION_MENU, "Hit Indication", DiagMenuIDs.MISC_MENU);
				{
					//---------------------------------------------------------------
					// LEVEL 3 - Script > Misc > Hit Indication
					//---------------------------------------------------------------
					DiagMenu.RegisterBool(DiagMenuIDs.MISC_HIT_INDICATION_DEBUG, "", "Enable Debug", DiagMenuIDs.MISC_HIT_INDICATION_MENU);
					DiagMenu.RegisterBool(DiagMenuIDs.MISC_HIT_INDICATION_SPAWN_HIT, "", "Spawn Hit", DiagMenuIDs.MISC_HIT_INDICATION_MENU);
					DiagMenu.RegisterRange(DiagMenuIDs.MISC_HIT_INDICATION_SPAWN_HIT_DIRECTION, "", "Spawn Hit Direction", DiagMenuIDs.MISC_HIT_INDICATION_MENU, "0, 359, 0, 1");
					DiagMenu.RegisterItem(DiagMenuIDs.MISC_HIT_INDICATION_MODE, "", "Mode", DiagMenuIDs.MISC_HIT_INDICATION_MENU, "Disabled,Static,Dynamic");
						DiagMenu.SetValue(DiagMenuIDs.MISC_HIT_INDICATION_MODE, 1);
					DiagMenu.RegisterItem(DiagMenuIDs.MISC_HIT_INDICATION_TYPE, "", "Type", DiagMenuIDs.MISC_HIT_INDICATION_MENU, "Splash,Spike,Arrow");
					DiagMenu.RegisterRange(DiagMenuIDs.MISC_HIT_INDICATION_DURATION, "", "Hit Duration Max", DiagMenuIDs.MISC_HIT_INDICATION_MENU, "0.1, 5.0, 1.0, 0.1");
					DiagMenu.RegisterRange(DiagMenuIDs.MISC_HIT_INDICATION_BREAKPOINT, "", "Hit Breakpoint", DiagMenuIDs.MISC_HIT_INDICATION_MENU, "0.0, 1.0, 0.8, 0.05");
					DiagMenu.RegisterRange(DiagMenuIDs.MISC_HIT_INDICATION_DISTANCE, "", "Distance Adjust", DiagMenuIDs.MISC_HIT_INDICATION_MENU, "0.0, 1.0, 0.0, 0.01");
					DiagMenu.RegisterRange(DiagMenuIDs.MISC_HIT_INDICATION_ROTATION, "", "Rotation Override", DiagMenuIDs.MISC_HIT_INDICATION_MENU, "0, 360, 0, 15");
					DiagMenu.RegisterRange(DiagMenuIDs.MISC_HIT_INDICATION_SCATTER, "", "Direction Scatter", DiagMenuIDs.MISC_HIT_INDICATION_MENU, "0, 90, 0, 1");
					DiagMenu.RegisterBool(DiagMenuIDs.MISC_HIT_INDICATION_DISABLE_PPE, "", "Disable Hit PPE", DiagMenuIDs.MISC_HIT_INDICATION_MENU);
				}			
				//---------------------------------------------------------------
				// LEVEL 2 - Script > Misc
				//---------------------------------------------------------------	
				DiagMenu.RegisterBool(DiagMenuIDs.MISC_FREEZE_ENTITY, "lalt+x", "Freeze entity", DiagMenuIDs.MISC_MENU);		
				DiagMenu.RegisterBool(DiagMenuIDs.MISC_CONNECTION_STATS, "lalt+4", "Show Connection Stats", DiagMenuIDs.MISC_MENU);
				DiagMenu.RegisterBool(DiagMenuIDs.MISC_PLAYER_SYMPTOMS_SHOW, "", "Show States", DiagMenuIDs.MISC_MENU);
			}
			
			//---------------------------------------------------------------
			// LEVEL 1 - Script
			//---------------------------------------------------------------
			DiagMenu.RegisterMenu(DiagMenuIDs.SIMULATE_MENU, "Simulate script", DiagMenuIDs.SCRIPTS_MENU);
			{
				//---------------------------------------------------------------
				// LEVEL 2 - Script > Simulate script
				//---------------------------------------------------------------
				DiagMenu.RegisterRange(DiagMenuIDs.SIMULATE_MODE, "", "Server/Client/Both", DiagMenuIDs.SIMULATE_MENU,"0,2,0,1");
				DiagMenu.RegisterBool(DiagMenuIDs.SIMULATE_INFINITE_LOOP, "", "Simulate infinite loop", DiagMenuIDs.SIMULATE_MENU);
				DiagMenu.RegisterBool(DiagMenuIDs.SIMULATE_NULL_POINTER, "", "Simulate null pointer", DiagMenuIDs.SIMULATE_MENU);
				DiagMenu.RegisterBool(DiagMenuIDs.SIMULATE_DIVISION_BY_ZERO, "", "Simulate division by 0", DiagMenuIDs.SIMULATE_MENU);
				DiagMenu.RegisterBool(DiagMenuIDs.SIMULATE_ERROR_FUNCTION, "", "Simulate Error() function", DiagMenuIDs.SIMULATE_MENU);
			}
			
			//---------------------------------------------------------------
			// LEVEL 1 - Script
			//---------------------------------------------------------------
			DiagMenu.RegisterMenu(DiagMenuIDs.MELEE_MENU, "Melee", DiagMenuIDs.SCRIPTS_MENU);
			{
				//---------------------------------------------------------------
				// LEVEL 2 - Script > Melee
				//---------------------------------------------------------------
				DiagMenu.RegisterBool(DiagMenuIDs.MELEE_DEBUG, "", "Enable Melee Debug", DiagMenuIDs.MELEE_MENU);
				DiagMenu.RegisterBool(DiagMenuIDs.MELEE_CONTINUOUS, "", "Continuous update", DiagMenuIDs.MELEE_MENU);
				DiagMenu.RegisterBool(DiagMenuIDs.MELEE_SHOW_TARGETS, "", "Show targets", DiagMenuIDs.MELEE_MENU);
				DiagMenu.RegisterBool(DiagMenuIDs.MELEE_DRAW_TARGETS, "", "Draw targets", DiagMenuIDs.MELEE_MENU);
				DiagMenu.RegisterBool(DiagMenuIDs.MELEE_DRAW_RANGE, "", "Draw range", DiagMenuIDs.MELEE_MENU);
				DiagMenu.RegisterBool(DiagMenuIDs.MELEE_DRAW_BLOCK_RANGE_AI, "", "Draw block range PVE", DiagMenuIDs.MELEE_MENU);
				DiagMenu.RegisterBool(DiagMenuIDs.MELEE_DRAW_BLOCK_RANGE_PVP, "", "Draw block range PVP", DiagMenuIDs.MELEE_MENU);
			}
			
			//---------------------------------------------------------------
			// LEVEL 1 - Script
			//---------------------------------------------------------------
			DiagMenu.RegisterMenu(DiagMenuIDs.WEAPON_MENU, "Weapon", DiagMenuIDs.SCRIPTS_MENU);
			{
				//---------------------------------------------------------------
				// LEVEL 2 - Script > Weapon
				//---------------------------------------------------------------
				DiagMenu.RegisterBool(DiagMenuIDs.WEAPON_PARTICLES, "", "Enable gun particles", DiagMenuIDs.WEAPON_MENU);
					DiagMenu.SetValue(DiagMenuIDs.WEAPON_PARTICLES, true);
				DiagMenu.RegisterBool(DiagMenuIDs.WEAPON_DEBUG, "", "Show Debug", DiagMenuIDs.WEAPON_MENU); // Doesn't do anything anymore currently
				DiagMenu.RegisterBool(DiagMenuIDs.WEAPON_AIM_NOISE, "", "Weapon Sway", DiagMenuIDs.WEAPON_MENU);
					DiagMenu.SetValue(DiagMenuIDs.WEAPON_AIM_NOISE, true);
				DiagMenu.RegisterBool(DiagMenuIDs.WEAPON_RECOIL, "", "Procedural Recoil", DiagMenuIDs.WEAPON_MENU);
					DiagMenu.SetValue(DiagMenuIDs.WEAPON_RECOIL, true);
				DiagMenu.RegisterBool(DiagMenuIDs.WEAPON_UNLIMITED_AMMO, "lalt+9", "Unlimited Ammo", DiagMenuIDs.WEAPON_MENU);
				DiagMenu.RegisterItem(DiagMenuIDs.WEAPON_BURST_VERSION, "lctrl+0", "Burst Version", DiagMenuIDs.WEAPON_MENU, "Hold, Press");
				DiagMenu.RegisterBool(DiagMenuIDs.WEAPON_CLAYMORE_DEBUG, "", "Claymore debugs", DiagMenuIDs.WEAPON_MENU);
			}
		
			//---------------------------------------------------------------
			// LEVEL 1 - Script
			//---------------------------------------------------------------
			DiagMenu.RegisterMenu(DiagMenuIDs.BLEEDING_MENU, "Bleeding", DiagMenuIDs.SCRIPTS_MENU);
			{
				//---------------------------------------------------------------
				// LEVEL 2 - Script > Bleeding
				//---------------------------------------------------------------
				DiagMenu.RegisterBool(DiagMenuIDs.BLEEDING_SOURCES, "", "Show Bleeding Sources", DiagMenuIDs.BLEEDING_MENU);
				DiagMenu.RegisterBool(DiagMenuIDs.BLEEDING_DISABLE_BLOOD_LOSS, "", "Disable Blood Loss", DiagMenuIDs.BLEEDING_MENU);
				DiagMenu.RegisterRange(DiagMenuIDs.BLEEDING_ACTIVATE_SOURCE, "", "Activate Source #", DiagMenuIDs.BLEEDING_MENU, "1, 32, 0, 1");
				DiagMenu.RegisterBool(DiagMenuIDs.BLEEDING_ACTIVATE_ALL_SOURCES, "", "Activate All Sources", DiagMenuIDs.BLEEDING_MENU);
				DiagMenu.RegisterItem(DiagMenuIDs.BLEEDING_ACTIVATE_SOURCE_LEVEL, "", "Activate Blsource Level", DiagMenuIDs.BLEEDING_MENU, "None, Low, Medium, High");
				DiagMenu.RegisterBool(DiagMenuIDs.BLEEDING_RELOAD, "", "Client Reload", DiagMenuIDs.BLEEDING_MENU);
				DiagMenu.RegisterMenu(DiagMenuIDs.BLEEDINGINDICATORS_MENU, "Bleeding Indication", DiagMenuIDs.BLEEDING_MENU);
				{
					//---------------------------------------------------------------
					// LEVEL 3 - Script > Bleeding > Bleeding Indication
					//---------------------------------------------------------------
					DiagMenu.RegisterBool(DiagMenuIDs.BLEEDINGINDICATORS, "", "Enable BleedingIndicators", DiagMenuIDs.BLEEDINGINDICATORS_MENU);
						DiagMenu.SetValue(DiagMenuIDs.BLEEDINGINDICATORS, true);
					DiagMenu.RegisterBool(DiagMenuIDs.BLEEDINGINDICATORS_USEVALUEOVERRIDES, "", "Enable Debug Overrides", DiagMenuIDs.BLEEDINGINDICATORS_MENU);
						DiagMenu.SetValue(DiagMenuIDs.BLEEDINGINDICATORS_USEVALUEOVERRIDES, false);
					DiagMenu.RegisterBool(DiagMenuIDs.BLEEDINGINDICATORS_DROP_RANDOMROTATION, "", "Enable Random Rotation", DiagMenuIDs.BLEEDINGINDICATORS_MENU);
						DiagMenu.SetValue(DiagMenuIDs.BLEEDINGINDICATORS_DROP_RANDOMROTATION, true);
					DiagMenu.RegisterRange(DiagMenuIDs.BLEEDINGINDICATORS_SEQUENCE_DURATION, "", "Sequence Duration", DiagMenuIDs.BLEEDINGINDICATORS_MENU, "0.2, 5.0, " + BleedingIndicationConstants.SEQUENCE_DURATION_DEFAULT.ToString() + ", 0.01");
					DiagMenu.RegisterRange(DiagMenuIDs.BLEEDINGINDICATORS_DROP_BASEDURATION, "", "Drop Anim Duration", DiagMenuIDs.BLEEDINGINDICATORS_MENU, "0.1, 5.0, " + BleedingIndicationConstants.DROP_DURATION_LOW.ToString() + ", 0.05");
					DiagMenu.RegisterRange(DiagMenuIDs.BLEEDINGINDICATORS_DROP_MINDELAY, "", "Drop Min Delay", DiagMenuIDs.BLEEDINGINDICATORS_MENU, "0.0, 5.0, " + BleedingIndicationConstants.SEQUENCE_DROP_DELAY_MIN_LOW.ToString() + ", 0.05");
					DiagMenu.RegisterRange(DiagMenuIDs.BLEEDINGINDICATORS_DROP_MAXDELAY, "", "Drop Max Delay", DiagMenuIDs.BLEEDINGINDICATORS_MENU, "0.0, 5.0, " + BleedingIndicationConstants.SEQUENCE_DROP_DELAY_MAX_LOW.ToString() + ", 0.05");
					DiagMenu.RegisterRange(DiagMenuIDs.BLEEDINGINDICATORS_DROP_PROGRESSTHRESHOLD, "", "Drop Progress Threshold", DiagMenuIDs.BLEEDINGINDICATORS_MENU, "0.1, 1.0, " + BleedingIndicationConstants.DROP_PROGRESS_THRESHOLD.ToString() + ", 0.05");
					DiagMenu.RegisterRange(DiagMenuIDs.BLEEDINGINDICATORS_DROP_STARTSIZE, "", "Drop Start Size", DiagMenuIDs.BLEEDINGINDICATORS_MENU, "0.1, 1.0, " + BleedingIndicationConstants.DROP_SIZE_START_LOW.ToString() + ", 0.01");
					DiagMenu.RegisterRange(DiagMenuIDs.BLEEDINGINDICATORS_DROP_ENDSIZE, "", "Drop End Size", DiagMenuIDs.BLEEDINGINDICATORS_MENU, "0.1, 1.0, " + BleedingIndicationConstants.DROP_SIZE_END_LOW.ToString() + ", 0.01");
					DiagMenu.RegisterRange(DiagMenuIDs.BLEEDINGINDICATORS_DROP_RNDSCALEMIN, "", "Drop Size Random Min", DiagMenuIDs.BLEEDINGINDICATORS_MENU, "0.0, 2.0, " + BleedingIndicationConstants.DROP_SIZE_VARIATION_MIN_LOW.ToString() + ", 0.01");
					DiagMenu.RegisterRange(DiagMenuIDs.BLEEDINGINDICATORS_DROP_RNDSCALEMAX, "", "Drop Size Random Max", DiagMenuIDs.BLEEDINGINDICATORS_MENU, "0.0, 2.0, " + BleedingIndicationConstants.DROP_SIZE_VARIATION_MAX_LOW.ToString() + ", 0.01");
					DiagMenu.RegisterRange(DiagMenuIDs.BLEEDINGINDICATORS_DROP_SCATTER, "", "Drop Scatter px", DiagMenuIDs.BLEEDINGINDICATORS_MENU, "0.0, 500.0, " + BleedingIndicationConstants.DROP_SCATTER_LOW.ToString() + ", 1.0");
					DiagMenu.RegisterRange(DiagMenuIDs.BLEEDINGINDICATORS_DROP_SLIDEDISTANCE, "", "Drop Slide Distance", DiagMenuIDs.BLEEDINGINDICATORS_MENU, "0.0, 500.0, 0, 0.1");
					DiagMenu.RegisterMenu(DiagMenuIDs.BLEEDINGINDICATORS_DROP_COLOR_MENU, "Color Debug", DiagMenuIDs.BLEEDINGINDICATORS_MENU);
					{
						//---------------------------------------------------------------
						// LEVEL 4 - Script > Bleeding > Bleeding Indication > Color Debug
						//---------------------------------------------------------------
						DiagMenu.RegisterBool(DiagMenuIDs.BLEEDINGINDICATORS_DROP_COLOR_ENABLEDESATURATION, "", "Desaturate", DiagMenuIDs.BLEEDINGINDICATORS_DROP_COLOR_MENU);
							DiagMenu.SetValue(DiagMenuIDs.BLEEDINGINDICATORS_DROP_COLOR_ENABLEDESATURATION, true);				
						DiagMenu.RegisterRange(DiagMenuIDs.BLEEDINGINDICATORS_DROP_COLOR_DESATURATIONEND, "", "Saturation End Value", DiagMenuIDs.BLEEDINGINDICATORS_DROP_COLOR_MENU, "0.0, 1.0, " + BleedingIndicationConstants.DROP_COLOR_DESATURATIONEND.ToString() + ", 0.05");
						DiagMenu.RegisterRange(DiagMenuIDs.BLEEDINGINDICATORS_DROP_COLOR_R, "", "Red", DiagMenuIDs.BLEEDINGINDICATORS_DROP_COLOR_MENU, "0.0, 255.0, " + BleedingIndicationConstants.DROP_COLOR_RED.ToString() + ", 1.0");
						DiagMenu.RegisterRange(DiagMenuIDs.BLEEDINGINDICATORS_DROP_COLOR_G, "", "Green", DiagMenuIDs.BLEEDINGINDICATORS_DROP_COLOR_MENU, "0.0, 255.0, " + BleedingIndicationConstants.DROP_COLOR_GREEN.ToString() + ", 1.0");
						DiagMenu.RegisterRange(DiagMenuIDs.BLEEDINGINDICATORS_DROP_COLOR_B, "", "Blue", DiagMenuIDs.BLEEDINGINDICATORS_DROP_COLOR_MENU, "0.0, 255.0, " + BleedingIndicationConstants.DROP_COLOR_BLUE.ToString() + ", 1.0");
						DiagMenu.RegisterRange(DiagMenuIDs.BLEEDINGINDICATORS_DROP_COLOR_A_START, "", "Alpha - Start", DiagMenuIDs.BLEEDINGINDICATORS_DROP_COLOR_MENU, "0.0, 255.0, " + BleedingIndicationConstants.DROP_COLOR_ALPHA_START.ToString() + ", 1.0");
						DiagMenu.RegisterRange(DiagMenuIDs.BLEEDINGINDICATORS_DROP_COLOR_A_END, "", "Alpha - End", DiagMenuIDs.BLEEDINGINDICATORS_DROP_COLOR_MENU, "0.0, 255.0, " + BleedingIndicationConstants.DROP_COLOR_ALPHA_END.ToString() + ", 1.0");
					}
				}
			}
			
			//---------------------------------------------------------------
			// LEVEL 1 - Script
			//---------------------------------------------------------------
			DiagMenu.RegisterMenu(DiagMenuIDs.LOGS_MENU, "Logs", DiagMenuIDs.SCRIPTS_MENU);
			{
				//---------------------------------------------------------------
				// LEVEL 2 - Script > Logs
				//---------------------------------------------------------------
				DiagMenu.RegisterBool(DiagMenuIDs.LOGS_ACTIONS, "", "Log Actions", DiagMenuIDs.LOGS_MENU);
				DiagMenu.RegisterBool(DiagMenuIDs.LOGS_WEAPONS, "", "Log Weapons", DiagMenuIDs.LOGS_MENU);
				DiagMenu.RegisterBool(DiagMenuIDs.LOGS_SYMPTOMS, "", "Log Symptoms", DiagMenuIDs.LOGS_MENU);
				DiagMenu.RegisterMenu(DiagMenuIDs.LOGS_INVENTORY_MENU, "Inventory Logs", DiagMenuIDs.LOGS_MENU);
				{
					//---------------------------------------------------------------
					// LEVEL 3 - Script > Logs > InventoryLogs
					//---------------------------------------------------------------
					DiagMenu.RegisterBool(DiagMenuIDs.LOGS_INVENTORY_MOVE, "", "Log Items move", DiagMenuIDs.LOGS_INVENTORY_MENU);
					DiagMenu.RegisterBool(DiagMenuIDs.LOGS_INVENTORY_RESERVATION, "", "Log Reservations", DiagMenuIDs.LOGS_INVENTORY_MENU);
					DiagMenu.RegisterBool(DiagMenuIDs.LOGS_INVENTORY_HFSM, "", "Log HandFSM", DiagMenuIDs.LOGS_INVENTORY_MENU);
				}
			}
			
			//---------------------------------------------------------------
			// LEVEL 1 - Script
			//---------------------------------------------------------------
			DiagMenu.RegisterMenu(DiagMenuIDs.TRIGGER_MENU, "Triggers", DiagMenuIDs.SCRIPTS_MENU);
			{
				//---------------------------------------------------------------
				// LEVEL 2 - Script > Triggers
				//---------------------------------------------------------------
				DiagMenu.RegisterBool(DiagMenuIDs.TRIGGER_DEBUG, "", "Show Triggers", DiagMenuIDs.TRIGGER_MENU);
				DiagMenu.RegisterBool(DiagMenuIDs.TRIGGER_PLAYER_DEBUG, "", "Show Player Touch", DiagMenuIDs.TRIGGER_MENU);
				#ifdef TRIGGER_DEBUG_BASIC
					DiagMenu.SetValue(DiagMenuIDs.TRIGGER_DEBUG, true);
					DiagMenu.SetValue(DiagMenuIDs.TRIGGER_PLAYER_DEBUG, true);
				#endif
			}

			//---------------------------------------------------------------
			// LEVEL 1 - Script
			//---------------------------------------------------------------
			DiagMenu.RegisterMenu(DiagMenuIDs.BASEBUILDING_MENU, "Base Building", DiagMenuIDs.SCRIPTS_MENU);
			{
				//---------------------------------------------------------------
				// LEVEL 2 - Script > Base Building
				//---------------------------------------------------------------
				DiagMenu.RegisterBool(DiagMenuIDs.BASEBUILDING_WOOD, "", "Spawn uses wood/metal", DiagMenuIDs.BASEBUILDING_MENU);
				DiagMenu.RegisterBool(DiagMenuIDs.BASEBUILDING_GATE, "", "Spawn Fence makes gate", DiagMenuIDs.BASEBUILDING_MENU);
			}
			
			//---------------------------------------------------------------
			// LEVEL 1 - Script
			//---------------------------------------------------------------
			DiagMenu.RegisterMenu(DiagMenuIDs.UNDERGROUND_MENU, "Underground Areas", DiagMenuIDs.SCRIPTS_MENU);
			{
				//---------------------------------------------------------------
				// LEVEL 2 - Script > Underground Areas
				//---------------------------------------------------------------
				DiagMenu.RegisterBool(DiagMenuIDs.UNDERGROUND_SHOW_BREADCRUMB, "", "Show Breadcrumbs", DiagMenuIDs.UNDERGROUND_MENU);
				DiagMenu.RegisterBool(DiagMenuIDs.UNDERGROUND_DISABLE_DARKENING, "lctrl+f", "Disable Darkening", DiagMenuIDs.UNDERGROUND_MENU);
			}
			
			//---------------------------------------------------------------
			// LEVEL 1 - Script
			//---------------------------------------------------------------
			DiagMenu.RegisterMenu(DiagMenuIDs.FINISHERS_MENU, "Finishers", DiagMenuIDs.SCRIPTS_MENU);
			{
				//---------------------------------------------------------------
				// LEVEL 2 - Script > Finishers
				//---------------------------------------------------------------
				DiagMenu.RegisterItem(DiagMenuIDs.FINISHERS_FINISHER_FORCED, "", "Forced Finisher: ", DiagMenuIDs.FINISHERS_MENU, "None,Liver,Neck");
			}
			
			//---------------------------------------------------------------
			// LEVEL 1 - Script
			//---------------------------------------------------------------
			DiagMenu.RegisterMenu(DiagMenuIDs.SOUNDS_MENU, "Script Sounds", DiagMenuIDs.SCRIPTS_MENU);
			{
				//---------------------------------------------------------------
				// LEVEL 2 - Script > Script Sounds
				//---------------------------------------------------------------
				DiagMenu.RegisterBool(DiagMenuIDs.SOUNDS_ITEM_IMPACT_SOUNDS, "", "Item impact sounds", DiagMenuIDs.SOUNDS_MENU);
			}
			
			//---------------------------------------------------------------
			// LEVEL 1 - Script
			//---------------------------------------------------------------
			DiagMenu.RegisterMenu(DiagMenuIDs.CAMERATOOLS_MENU, "Cinematic Camera Tools", DiagMenuIDs.SCRIPTS_MENU);
			{
				//---------------------------------------------------------------
				// LEVEL 2 - Script > Cinematic Camera Tools
				//---------------------------------------------------------------
				DiagMenu.RegisterBool(DiagMenuIDs.CAMERATOOLS_ENABLE_REMOTE_CAMERA, "", "Show remote camera", DiagMenuIDs.CAMERATOOLS_MENU);
				DiagMenu.RegisterBool(DiagMenuIDs.CAMERATOOLS_ENABLE_REMOTE_CAMERA_BROADCAST, "", "Broadcast camera", DiagMenuIDs.CAMERATOOLS_MENU);
			}
			//---------------------------------------------------------------
			// LEVEL 1 - Script
			//---------------------------------------------------------------
			DiagMenu.RegisterMenu(DiagMenuIDs.FEATURE_TIME_ACCEL_MENU, "Time Accel", DiagMenuIDs.SCRIPTS_MENU);
			{
				//---------------------------------------------------------------
				// LEVEL 2 - Script > Time Accel
				//---------------------------------------------------------------
				DiagMenu.RegisterBool(DiagMenuIDs.FEATURE_TIME_ACCEL, "lalt+0", "Apply Time Accel", DiagMenuIDs.FEATURE_TIME_ACCEL_MENU);
				DiagMenu.RegisterRange(DiagMenuIDs.FEATURE_TIME_ACCEL_VALUE1, "", "Accel Factor Big", DiagMenuIDs.FEATURE_TIME_ACCEL_MENU,"0,100,1,1");
				DiagMenu.RegisterRange(DiagMenuIDs.FEATURE_TIME_ACCEL_VALUE2, "", "Accel Factor Small", DiagMenuIDs.FEATURE_TIME_ACCEL_MENU,"0,0.95 ,0,0.05");
				DiagMenu.RegisterBool(DiagMenuIDs.FEATURE_TIME_ACCEL_UG_ENTRANCES, "", "Underground Entrances", DiagMenuIDs.FEATURE_TIME_ACCEL_MENU);
				DiagMenu.RegisterBool(DiagMenuIDs.FEATURE_TIME_ACCEL_UG_RESERVOIR, "", "Underground Reservoir", DiagMenuIDs.FEATURE_TIME_ACCEL_MENU);
				DiagMenu.RegisterBool(DiagMenuIDs.FEATURE_TIME_ACCEL_ENERGY_CONSUME, "", "Energy Consumption", DiagMenuIDs.FEATURE_TIME_ACCEL_MENU);
				DiagMenu.RegisterBool(DiagMenuIDs.FEATURE_TIME_ACCEL_FOOD_DECAY, "", "Decay", DiagMenuIDs.FEATURE_TIME_ACCEL_MENU);
			}
		}
	}
	
	//---------------------------------------------
	protected int GetModdedDiagID()
	{
		return ++m_ModdedDiagID;
	}
	
	//---------------------------------------------
	protected int GetModdedRootMenu()
	{
		return DiagMenuIDs.MODDED_MENU;
	}
	
	//---------------------------------------------
	protected void RegisterModdedDiagsIDs();
	
	//---------------------------------------------
	protected void RegisterModdedDiags()
	{
 		DiagMenu.RegisterMenu(DiagMenuIDs.MODDED_MENU, "Script - Modded", -1);
	}
	
	//---------------------------------------------
	void OnRPC(PlayerBase player, int rpc_type, ParamsReadContext ctx)
	{
		switch (rpc_type)
		{
			//---------------------------------------------------------------
			// Misc, multipurpose
			//---------------------------------------------------------------
			case ERPCs.DIAG_ITEM_DEBUG_ACTIONS:
			{
				if (ctx.Read(CachedObjectsParams.PARAM1_INT))
					ItemBase.SetDebugActionsMask(CachedObjectsParams.PARAM1_INT.param1);
				break;
			}
			
			
			//---------------------------------------------------------------
			// LEVEL 2 - Script > Player States
			//---------------------------------------------------------------
			case ERPCs.DIAG_PLAYER_SYMPTOMS_DEBUG_ON:
			case ERPCs.DIAG_PLAYER_SYMPTOMS_DEBUG_OFF:
			case ERPCs.DIAG_PLAYER_SYMPTOMS_DEBUG:
			{
				if (player.GetSymptomManager())
				{
					player.GetSymptomManager().OnRPCDebug(rpc_type, ctx);
				}
				break;
			}

			
			
			//---------------------------------------------------------------
			// LEVEL 2 - Script > Crafting
			//---------------------------------------------------------------
			case ERPCs.DIAG_CRAFTING_INSTANT:
			{
				if (ctx.Read(CachedObjectsParams.PARAM1_INT))
				{
					PluginRecipesManager pluginRecipesManager = PluginRecipesManager.Cast(GetPlugin(PluginRecipesManager));
					pluginRecipesManager.SetEnableDebugCrafting(CachedObjectsParams.PARAM1_INT.param1);
				}
				break;
			}
			
			
			//---------------------------------------------------------------
			// LEVEL 2 - Script > Cheats
			//---------------------------------------------------------------
			case ERPCs.DIAG_CHEATS_MODIFIERS:
			{
				if (ctx.Read(CachedObjectsParams.PARAM1_INT))
					player.SetModifiers(CachedObjectsParams.PARAM1_INT.param1);
				break;
			}
			
			//---------------------------------------------------------------
			case ERPCs.DIAG_CHEATS_KILL_PLAYER:
			{
				player.SetHealth("","", -1);
				break;	
			}
			
			//---------------------------------------------------------------
			case ERPCs.DIAG_CHEATS_INVINCIBILITY:
			{
				if (ctx.Read(CachedObjectsParams.PARAM1_INT))
				{
					switch (CachedObjectsParams.PARAM1_INT.param1)
					{
						case 0:
						{
							player.SetAllowDamage(true);
							player.SetCanBeDestroyed(true);
							break;
						}
						case 1:
						{
							player.SetAllowDamage(true);
							player.SetCanBeDestroyed(false);
							break;
						}
						case 2:
						{
							player.SetAllowDamage(false);
							break;
						}
					}
					break;
				}
			}
			
			//---------------------------------------------------------------
			case ERPCs.DIAG_CHEATS_DISABLE_STAMINA:
			{
				if (ctx.Read(CachedObjectsParams.PARAM1_INT))
					player.SetStaminaEnabled(CachedObjectsParams.PARAM1_INT.param1);
				break;
			}
			
			//---------------------------------------------------------------
			case ERPCs.DIAG_CHEATS_RESET_PLAYER:
			{
				player.ResetPlayer(false);
				break;
			}
			
			//---------------------------------------------------------------
			case ERPCs.DIAG_CHEATS_RESET_PLAYER_MAX:
			{
				player.ResetPlayer(true);
				break;
			}
			
			//---------------------------------------------------------------
			case ERPCs.DIAG_CHEATS_INVENTORY_ACCESS:
			{
				if (ctx.Read(CachedObjectsParams.PARAM1_INT))
					player.DEBUG_INVENTORY_ACCESS = CachedObjectsParams.PARAM1_INT.param1;
				break;
			}
			
			//---------------------------------------------------------------
			case ERPCs.DIAG_CHEATS_ITEMS_FIX:
			{
				player.FixAllInventoryItems();
				break;
			}
			
			//---------------------------------------------------------------
			// LEVEL 2 - Script > Soft Skills
			//---------------------------------------------------------------			
			case ERPCs.DIAG_SOFT_SKILLS_SHOW_DEBUG:
			{
				if (ctx.Read(CachedObjectsParams.PARAM1_BOOL))
				{
					if (CachedObjectsParams.PARAM1_BOOL.param1)
					{
						player.GetSoftSkillsManager().StartSynchTimer();
					}
					else
					{
						player.GetSoftSkillsManager().StopSynchTimer();
						player.GetSoftSkillsManager().ResetDebugWindow();
					}
				}
				break;
			}
			
			//---------------------------------------------------------------
			case ERPCs.DIAG_SOFT_SKILLS_TOGGLE_STATE:
			{
				if (ctx.Read(CachedObjectsParams.PARAM1_BOOL))
					player.GetSoftSkillsManager().SetSoftSkillsState(CachedObjectsParams.PARAM1_BOOL.param1);
				break;
			}
			
			//---------------------------------------------------------------
			case ERPCs.DIAG_SOFT_SKILLS_TOGGLE_MODEL:
			{
				if (ctx.Read(CachedObjectsParams.PARAM1_BOOL))
					player.GetSoftSkillsManager().SetLinearState(CachedObjectsParams.PARAM1_BOOL.param1) ;
				break;
			}
			
			//---------------------------------------------------------------
			case ERPCs.DIAG_SOFT_SKILLS_SPECIALTY_VALUE:
			{
				if (ctx.Read(CachedObjectsParams.PARAM1_FLOAT))
				{
					SoftSkillsManager softSkillsMngr = player.GetSoftSkillsManager();
					softSkillsMngr.SetSpecialtyLevel(CachedObjectsParams.PARAM1_FLOAT.param1);
					softSkillsMngr.SynchSpecialtyLevel();
					player.GetStatSpecialty().Set(CachedObjectsParams.PARAM1_FLOAT.param1);
				}
				break;
			}
			
			//---------------------------------------------------------------
			// LEVEL 2 - Script > Lifespan
			//---------------------------------------------------------------
			case ERPCs.DIAG_LIFESPAN_BLOODY_HANDS:
			{
				if (ctx.Read(CachedObjectsParams.PARAM1_BOOL))
				{
					PluginLifespan pluginLifespan = PluginLifespan.Cast(GetPlugin(PluginLifespan));
					pluginLifespan.UpdateBloodyHandsVisibility(player, CachedObjectsParams.PARAM1_BOOL.param1);
				}
				break;
			}

			//---------------------------------------------------------------
			case ERPCs.DIAG_LIFESPAN_PLAYTIME_UPDATE:
			{
				if (ctx.Read( CachedObjectsParams.PARAM1_FLOAT))
				{
					//reset playtime
					player.StatUpdate("playtime", player.StatGet("playtime") * -1);
					//set new playtime
					player.StatUpdate("playtime", CachedObjectsParams.PARAM1_FLOAT.param1 );
					player.SetLastShavedSeconds( 0 );
					//update lifespan
					PluginLifespan pluginLifespann = PluginLifespan.Cast(GetPlugin(PluginLifespan));
					pluginLifespann.ChangeFakePlaytime( player, CachedObjectsParams.PARAM1_FLOAT.param1 );
					pluginLifespann.UpdateLifespan( player, true );
				}
				break;
			}
			
			
			//---------------------------------------------------------------
			// LEVEL 2 - Script > Misc
			//---------------------------------------------------------------
			case ERPCs.DIAG_MISC_LOG_PLAYER_STATS:
			{
				if (ctx.Read(CachedObjectsParams.PARAM1_BOOL))
					player.GetPlayerStats().SetAllowLogs(CachedObjectsParams.PARAM1_BOOL.param1);
				break;
			}
						
			//---------------------------------------------------------------
			case ERPCs.DIAG_MISC_ENVIRONMENT_DEBUG:
			{
				if (ctx.Read(CachedObjectsParams.PARAM1_BOOL))
				{
					if (player.m_Environment)
						player.m_Environment.m_Debug = CachedObjectsParams.PARAM1_BOOL.param1;
				}
				break;
			}
			
			//---------------------------------------------------------------
			case ERPCs.DIAG_MISC_GO_UNCONSCIOUS:
			{
				GoUnconscious(player);
				break;
			}
			
			//---------------------------------------------------------------
			case ERPCs.DIAG_MISC_GO_UNCONSCIOUS_DELAYED:
			{
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(GoUnconscious, 10000, false, player);
				break;
			}
			
			//---------------------------------------------------------------
			case ERPCs.DIAG_MISC_QUICK_RESTRAIN:
			{
				if (ctx.Read(CachedObjectsParams.PARAM1_BOOL))
					player.SetQuickRestrain(CachedObjectsParams.PARAM1_BOOL.param1);
				break;
			}
			
			//---------------------------------------------------------------
			// LEVEL 3 - Script > Misc > Hair Hiding
			//---------------------------------------------------------------
			case ERPCs.DIAG_MISC_HAIR_LEVEL_HIDE:
			{
				if (ctx.Read(CachedObjectsParams.PARAM2_INT_INT))
				{
					player.SetHairLevelToHide(CachedObjectsParams.PARAM2_INT_INT.param1, CachedObjectsParams.PARAM2_INT_INT.param2, true);
					player.UpdateHairSelectionVisibility(true);
				}
				break;
			}
			
			//---------------------------------------------------------------
			// LEVEL 2 - Script > Misc
			//---------------------------------------------------------------
			case ERPCs.DIAG_MISC_QUICK_FISHING:
			{
				if (ctx.Read(CachedObjectsParams.PARAM1_BOOL))
					player.SetQuickFishing(CachedObjectsParams.PARAM1_BOOL.param1);
				break;
			}
			
			//---------------------------------------------------------------
			case ERPCs.DIAG_MISC_TARGETABLE_BY_AI:
			{
				if (ctx.Read(CachedObjectsParams.PARAM1_BOOL))
					player.m_CanBeTargetedDebug = CachedObjectsParams.PARAM1_BOOL.param1;
				break;
			}
			
			//---------------------------------------------------------------
			// LEVEL 2 - Script > Simulate script
			//---------------------------------------------------------------			
			case ERPCs.DIAG_SIMULATE_INFINITE_LOOP:
			{
				while ( true )
				{
					Print("simulating infinite loop");
				}
				break;
			}
			
			//---------------------------------------------------------------
			case ERPCs.DIAG_SIMULATE_NULL_POINTER:
			{
				PlayerBase NULL_player = null;
				NULL_player.SetHealth("","", -1);
				break;
			}
			
			//---------------------------------------------------------------
			case ERPCs.DIAG_SIMULATE_DIVISION_BY_ZERO:
			{
				int zero = 0;
				int division_by_zero = 1/zero;
				break;
			}
			
			//---------------------------------------------------------------
			case ERPCs.DIAG_SIMULATE_ERROR_FUNCTION:
			{
				ErrorEx("Simulated error");
				break;
			}
		
			
			//---------------------------------------------------------------
			// LEVEL 2 - Script > Weapon
			//---------------------------------------------------------------
			case ERPCs.DIAG_WEAPON_PARTICLES:
			{
				if (ctx.Read(CachedObjectsParams.PARAM1_BOOL))
					PrtTest.m_GunParticlesState = CachedObjectsParams.PARAM1_BOOL.param1;
				break;
			}
			
			//---------------------------------------------------------------
			case ERPCs.DIAG_WEAPON_RECOIL:
			{
				if (ctx.Read(CachedObjectsParams.PARAM1_BOOL))
					player.GetAimingModel().SetProceduralRecoilEnabled(CachedObjectsParams.PARAM1_BOOL.param1);
				break;
			}
			
			//---------------------------------------------------------------
			case ERPCs.DIAG_WEAPON_BURST_VERSION:
			{
				if (ctx.Read(CachedObjectsParams.PARAM1_INT))
					player.GetWeaponManager().SetBurstOption(CachedObjectsParams.PARAM1_INT.param1);
				break;
			}
			
			
			//---------------------------------------------------------------
			// LEVEL 2 - Script > Bleeding
			//---------------------------------------------------------------
			case ERPCs.DIAG_BLEEDING_DISABLE_BLOOD_LOSS:
			{
				if (ctx.Read(CachedObjectsParams.PARAM1_BOOL))
					player.GetBleedingManagerServer().SetBloodLoss(CachedObjectsParams.PARAM1_BOOL.param1);
				break;
			}
			
			//---------------------------------------------------------------
			case ERPCs.DIAG_BLEEDING_ACTIVATE_SOURCE:
			{
				if (ctx.Read(CachedObjectsParams.PARAM1_INT))
					player.GetBleedingManagerServer().DebugActivateBleedingSource(CachedObjectsParams.PARAM1_INT.param1);
				break;
			}
			
			//---------------------------------------------------------------		
			case ERPCs.DIAG_BLEEDING_ACTIVATE_ALL_SOURCES:
			{
				player.GetBleedingManagerServer().ActivateAllBS();
				break;
			}
			
			
			//---------------------------------------------------------------
			// LEVEL 2 - Script > Logs
			//---------------------------------------------------------------
			case ERPCs.DIAG_LOGS_ACTIONS:
			{
				if (ctx.Read(CachedObjectsParams.PARAM1_BOOL))
					LogManager.ActionLogEnable(CachedObjectsParams.PARAM1_BOOL.param1);
				break;
			}
			
			//---------------------------------------------------------------
			case ERPCs.DIAG_LOGS_WEAPONS:
			{
				if (ctx.Read(CachedObjectsParams.PARAM1_BOOL))
					LogManager.WeaponLogEnable(CachedObjectsParams.PARAM1_BOOL.param1);
				break;
			}
			
			//---------------------------------------------------------------
			case ERPCs.DIAG_LOGS_SYMPTOMS:
			{
				if (ctx.Read(CachedObjectsParams.PARAM1_BOOL))
					LogManager.SymptomLogEnable(CachedObjectsParams.PARAM1_BOOL.param1);
				break;
			}
			
			//---------------------------------------------------------------
			case ERPCs.DIAG_LOGS_INVENTORY_MOVE:
			{
				if (ctx.Read(CachedObjectsParams.PARAM1_BOOL))
					LogManager.InventoryMoveLogEnable(CachedObjectsParams.PARAM1_BOOL.param1);
				break;
			}
			
			//---------------------------------------------------------------
			case ERPCs.DIAG_LOGS_INVENTORY_RESERVATION:
			{
				if (ctx.Read(CachedObjectsParams.PARAM1_BOOL))
					LogManager.InventoryReservationLogEnable(CachedObjectsParams.PARAM1_BOOL.param1);
				break;
			}
			
			//---------------------------------------------------------------
			case ERPCs.DIAG_LOGS_INVENTORY_HFSM:
			{
				if (ctx.Read(CachedObjectsParams.PARAM1_BOOL))
					LogManager.InventoryHFSMLogEnable(CachedObjectsParams.PARAM1_BOOL.param1);
				break;
			}
			
			
			//---------------------------------------------------------------
			// LEVEL 2 - Script > Base Building
			//---------------------------------------------------------------
			case ERPCs.DIAG_BASEBUILDING_WOOD:
			{
				if (ctx.Read(CachedObjectsParams.PARAM1_BOOL) && DiagMenu.GetBool(DiagMenuIDs.BASEBUILDING_WOOD) != CachedObjectsParams.PARAM1_BOOL.param1)
					DiagMenu.SetValue(DiagMenuIDs.BASEBUILDING_WOOD, CachedObjectsParams.PARAM1_BOOL.param1);
				break;
			}
			
			//---------------------------------------------------------------
			case ERPCs.DIAG_BASEBUILDING_GATE:
			{
				if (ctx.Read(CachedObjectsParams.PARAM1_BOOL) && DiagMenu.GetBool(DiagMenuIDs.BASEBUILDING_GATE) != CachedObjectsParams.PARAM1_BOOL.param1)
					DiagMenu.SetValue(DiagMenuIDs.BASEBUILDING_GATE, CachedObjectsParams.PARAM1_BOOL.param1);
				break;
			}
			
			
			//---------------------------------------------------------------
			// LEVEL 2 - Script > Finishers
			//---------------------------------------------------------------
			case ERPCs.DIAG_FINISHERS_FORCE_FINISHER:
			{
				if (ctx.Read(CachedObjectsParams.PARAM1_INT))
					player.GetMeleeCombat().DebugSetForcedFinisherType(CachedObjectsParams.PARAM1_INT.param1);
				break;
			}
			
			
			//---------------------------------------------------------------
			// LEVEL 2 - Script > Cinematic Camera Tools
			//---------------------------------------------------------------
			case ERPCs.DIAG_CAMERATOOLS_CAM_DATA:
			{
				if (!player.m_CameraToolsMenuClient)
				{
					player.m_CameraToolsMenuClient = new CameraToolsMenuClient();
				}
				
				player.m_CameraToolsMenuClient.OnRPC(ctx);
				break;
			}
					
			
			//---------------------------------------------------------------
			// LEVEL 2 - Script > Time Accel
			//---------------------------------------------------------------
			case ERPCs.DIAG_TIMEACCEL:
			{
				TimeAccelParam tap = new TimeAccelParam(false, 0, 0);
				
				if (ctx.Read(tap))
				{
					FeatureTimeAccel.m_CurrentTimeAccel = tap;
					//Print("received from " + player);
					array<Man> players = new array<Man>();
					GetGame().GetPlayers(players);
					foreach (Man p : players)
					{
						if (p != player)// send to everyone except the one we receieved from
						{
							//Print("sending to " + p);
							GetGame().RPCSingleParam( p, ERPCs.DIAG_TIMEACCEL_CLIENT_SYNC, tap, true, p.GetIdentity());
						}					
					}
					
					/*
					Print(" -------- TimeAccelParam -------");
					Print(tap.param1);
					Print(tap.param2);
					Print(tap.param3);
					Print(" -------------------------------");
					Print(FeatureTimeAccel.GetFeatureTimeAccelEnabled(ETimeAccelCategories.UNDERGROUND_ENTRANCE));
					Print(FeatureTimeAccel.GetFeatureTimeAccelValue());
					Print(" ===============================");
					*/
				}
				break;
			}
			
			//---------------------------------------------------------------
			case ERPCs.DIAG_TIMEACCEL_CLIENT_SYNC:
			{
				TimeAccelParam tap2 = new TimeAccelParam(false, 0, 0);
				
				if (ctx.Read(tap2))
				{
					int bigFactor = tap2.param2;
					float smallFactor = tap2.param2 - bigFactor;
					SetTimeAccelMenuState(tap2.param1, bigFactor, smallFactor, tap2.param3);
					FeatureTimeAccel.m_CurrentTimeAccel = tap2;
				}
				break;
			}
		}
	}
	
	//---------------------------------------------
	void GoUnconscious(PlayerBase player)
	{
		if (player.IsUnconscious())
		{
			player.SetHealth("", "Shock", 100);
		}
		else
		{
			player.m_UnconsciousEndTime = -60;
			player.SetHealth("", "Shock", 0);
		}
	}
	
	//---------------------------------------------	
	void InitTimeAccel()
	{
		FeatureTimeAccel.m_CurrentTimeAccel = GetTimeAccelMenuState();
		
		string value;
		
		if ( GetGame().CommandlineGetParam("timeAccel", value))
		{
			TStringArray params = new TStringArray();
			
			value.Replace(" ","");
			value.Split(",", params);
			
			bool 	enable 		= params[0].ToInt();
			int 	timeAccelBig 	= params[1].ToInt();
			float 	timeAccelSmall 	= params[2].ToFloat();
			int 	bitMask 		= params[3].ToInt();
			
			SetTimeAccelMenuState(enable, timeAccelBig, timeAccelSmall, bitMask);
		}
	}
	
	TimeAccelParam GetTimeAccelMenuState()
	{
		bool 	enable 			= DiagMenu.GetBool(DiagMenuIDs.FEATURE_TIME_ACCEL);
		float 	timeAccelBig 	= DiagMenu.GetRangeValue(DiagMenuIDs.FEATURE_TIME_ACCEL_VALUE1);
		float 	timeAccelSmall 	= DiagMenu.GetRangeValue(DiagMenuIDs.FEATURE_TIME_ACCEL_VALUE2);
		float 	timeAccel 		= timeAccelBig + timeAccelSmall;
		int 	bitMask			= FeatureTimeAccel.GetTimeAccelBitmask();
	
		TimeAccelParam param 	= new TimeAccelParam(enable, timeAccel, bitMask);
		return param;
	}
	
	void SetTimeAccelMenuState(bool enable, int bigFactor, float smallFactor, int bitmask)
	{
		DiagMenu.SetValue(DiagMenuIDs.FEATURE_TIME_ACCEL, enable);
		DiagMenu.SetRangeValue(DiagMenuIDs.FEATURE_TIME_ACCEL_VALUE1, bigFactor);
		DiagMenu.SetRangeValue(DiagMenuIDs.FEATURE_TIME_ACCEL_VALUE2, smallFactor);
	
		for (int i = 0; i < EnumTools.GetEnumSize(ETimeAccelCategories); ++i)
		{
			int bit = 1 << i;
			bool on = (bit & bitmask);
			DiagMenu.SetValue(FeatureTimeAccel.GetDiagIDByCategory(bit), on);
		}
	}
#endif
}