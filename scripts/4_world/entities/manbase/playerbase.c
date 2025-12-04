class PlayerBase extends ManBase
{
	const int 						SIMPLIFIED_SHOCK_CAP = 63;
	const int 						SHAKE_LEVEL_MAX = 7;
	private int						m_LifeSpanState;
	private int						m_LastShavedSeconds;
	private int						m_BloodType;
	private bool					m_HasBloodTypeVisible;
	private bool					m_LiquidTendencyDrain; //client-side only - Obsolete
	private bool 					m_FlagRaisingTendency;
	private int						m_HasBloodyHandsVisible;

	protected bool					m_HasHeatBuffer;
	protected int 					m_HeatBufferStage;
	protected float					m_HeatBufferDynamicMax;

	protected bool 					m_IsInColdArea;
	protected bool 					m_PlayerLoaded;
	protected bool 					m_PlayerDisconnectProcessed;
	protected bool 					m_ProcessUIWarning;
	protected int 					m_LocalRefreshAnimStateIdx;
	protected int 					m_RefreshAnimStateIdx;
	private int 					m_StoreLoadVersion;
	const int 						ACT_STORE_SAVE_VERSION = 4;
	protected int 					m_LifespanLevelLocal; //control variable for change calls
	protected int 					m_AnimCommandStarting; //signals the command that is about to start the next frame (e.g. Swim, Fall...)
	EUndergroundPresence			m_UndergroundPresence;
	
	private PluginPlayerStatus		m_ModulePlayerStatus;
	PluginConfigEmotesProfile 		m_ConfigEmotesProfile;
	private PluginLifespan 			m_ModuleLifespan;
	protected PluginPresenceNotifier  m_PresenceNotifier;
	
	protected ref UndergroundHandlerClient	m_UndergroundHandler;
	protected ref UndergroundBunkerHandlerClient	m_UndergroundBunkerHandler;
	ref PlayerStats 				m_PlayerStats;
	PluginRecipesManager 			m_ModuleRecipesManager;
	ref BleedingSourcesManagerServer 	m_BleedingManagerServer;
	ref BleedingSourcesManagerRemote 	m_BleedingManagerRemote;

	ref ModifiersManager			m_ModifiersManager;
	ref NotifiersManager 			m_NotifiersManager;
	ref protected ActionManagerBase	m_ActionManager;
	ref PlayerAgentPool				m_AgentPool;
	int								m_Agents;
	ref Environment					m_Environment;
	ref EmoteManager 				m_EmoteManager;
	ref SymptomManager				m_SymptomManager;
	ref VirtualHud 					m_VirtualHud;
	ref StaminaHandler				m_StaminaHandler;
	ref InjuryAnimationHandler		m_InjuryHandler;
	ref ShockHandler				m_ShockHandler; //New shock handler
	ref SoftSkillsManager			m_SoftSkillsManager;
	ref StanceIndicator				m_StanceIndicator;
	ref TransferValues				m_TrasferValues;
	ref DebugMonitorValues			m_DebugMonitorValues;
	const int 						OVERLOAD_LIMIT = 30000;
	float 							m_CargoLoad;
	float							m_VisibilityCoef;
	float 							m_OriginalSlidePoseAngle;
	int								m_SoundEvent;
	int								m_SoundEventParam;
	int 							m_FliesIndex;
	bool							m_SoundEventSent;	
	ref Param2<float,float>			m_UAProgressParam;
	ref Param2<int,int> 			m_UAParam;
	ref Param3<float,float,bool>	m_StaminaParam;
	ref Param1<string> 				m_UAParamMessage;
	ref Param1<float> 				m_UnconParam;
	ref Param1<float> 				m_DeathDarkeningParam;
	ref DamageDealtEffect			m_DamageDealtEffect;
	ref EffectRadial				m_EffectRadial;
	ref FlashbangEffect				m_FlashbangEffect;
	ref ShockDealtEffect 			m_ShockDealtEffect;
	ref EffectParticle 				m_FliesEff;
	ref TInputActionMap				m_InputActionMap; // Backwards compatibility
	ref TInputActionMap				m_InputActionMapControled;
	ref TInputActionMap				m_InputActionMapAsTarget;
	bool							m_ActionsInitialize;
	//ref CraftingMeta 				m_CraftingMeta;
	ref HiddenSelectionsData		m_EmptyGloves;
	ref WeaponManager				m_WeaponManager;
	ref CraftingManager 			m_CraftingManager;
	ref ArrowManagerPlayer			m_ArrowManager;
	ref InventoryActionHandler 		m_InventoryActionHandler;
	ref protected QuickBarBase		m_QuickBarBase;
	ref PlayerSoundManagerServer	m_PlayerSoundManagerServer;
	ref PlayerSoundManagerClient	m_PlayerSoundManagerClient;
	ref HeatComfortAnimHandler		m_HCAnimHandler;
	ref EffectSound					m_SoundFliesEffect;
	bool 							m_QuickBarHold;
	bool							m_QuickBarFT = false;
	Hud 							m_Hud;
	protected int 					m_RecipePick;
	protected bool					m_IsHoldingBreath;
	protected bool					m_IsInWater;
	float							m_LastPostFrameTickTime;
	//AbstractWave 					m_SaySoundWave;
	ref Timer						m_DeathCheckTimer;
	bool 							m_CorpseProcessing;
	int 							m_CorpseState;
	protected int 					m_CorpseStateLocal;
	protected bool 					m_IsHeadingRestricted;
	protected ref HeadingRestrictData m_HeadingRestrictData;
	
	int 							m_PersistentFlags;
	int								m_StaminaState;
	float							m_UnconsciousTime;
	int 							m_ShockSimplified;
	float							m_CurrentShock; //Used to synchronize shock between server and client, utilized by ShockHandler
	bool							m_IsRestrained;
	bool							m_IsRestrainedLocal;
	bool 							m_IsRestrainStarted;
	bool 							m_IsRestrainStartedLocal;
	bool 							m_IsRestrainPrelocked;
	bool							m_ImmunityBoosted;
	bool							m_AreHandsLocked; //Currently only used to block quickbar usage after canceling placement
	float							m_UnconsciousVignetteTarget = 2;
	float							m_CameraSwayModifier = 0.2;
	float 							m_LastShockHitTime;
	DayZPlayerCameraBase 			m_CurrentCamera;
	int								m_BleedingBits;
	vector 							m_DirectionToCursor;
	vector 							m_DefaultHitPosition;
	int								m_DiseaseCount;
	int								m_HealingsCount;
	protected bool					m_AllowQuickRestrain;
	protected bool					m_AllowQuickFishing;
	protected int					m_Shakes;
	protected int					m_ShakesForced;
	int								m_BreathVapour;
	int 							m_HealthLevel;
	int 							m_MixedSoundStates;
	int 							m_QuickBarBonus = 0;
	bool							m_IsVehicleSeatDriver;
	float							m_UnconsciousEndTime = 0;
	int								m_BleedingSourceCount;
	Head_Default 					m_CharactersHead;
	EPulseType						m_PulseType;
	PlayerBase						m_CheckPulseLastTarget;
	int 							m_ForceInjuryAnimMask;
	bool 							m_HideHairAnimated;
	string 							m_DecayedTexture;
	protected ref array<EntityAI>	m_ItemsToDelete;
	int								m_BrokenLegState = eBrokenLegs.NO_BROKEN_LEGS; //Describe the current leg state, can go bellow 0, cannot be used directly to obtain broken legs state, use GetBrokenLegs() instead
	int								m_LocalBrokenState = eBrokenLegs.NO_BROKEN_LEGS;
	bool							m_BrokenLegsJunctureReceived;
	ref EffectSound 				m_BrokenLegSound;
	const string 					SOUND_BREAK_LEG = "broken_leg_SoundSet";
	bool							m_CanPlayBrokenLegSound; //Used to check if sound has already been played
	static bool						DEBUG_INVENTORY_ACCESS = false;
	static ref array<string>		m_BleedingSourcesLow; //Stores all LOWER body part bleeding sources
	static ref array<string>		m_BleedingSourcesUp; //Stores all UPPER body part bleeding sources EXCLUDING HEAD
	float 							m_UnconRefillModifier = 1;
	ref protected RandomGeneratorSyncManager m_RGSManager;

	
	// CONTAMINATED AREA RELATED 
	bool 							m_ContaminatedAreaEffectEnabled;
	const string 					CONTAMINATED_AREA_AMBIENT = "ContaminatedArea_SoundSet";//obsolete, use EffectTrigger::GetAmbientSoundsetName() instead
	EffectSound 					m_AmbientContamination;
	protected int					m_ContaminatedAreaCount;
	protected int					m_EffectAreaCount;
	protected bool					m_InsideEffectArea;
	protected bool					m_InsideEffectAreaPrev;
	protected EffectTrigger			m_CurrentEffectTrigger;
	
	// effect areas
	protected EEffectAreaType		m_InEffectAreaType;	// flags for identifying which area the player is in, synchronized by EffectTriggers
	protected ref map<EEffectAreaType, int> m_EffectAreaOverlap 		= new map<EEffectAreaType, int>();		// type -> how many of such areas is player currently in
	protected ref map<EEffectAreaType, float> m_EffectAreaTimeToTick 	= new map<EEffectAreaType, float>();	// type -> how long has player spent in an area since last effect tick
	static ref array<Man> 			m_ServerPlayers = new array<Man>;
	protected bool 					m_CanDisplayHitEffectPPE;
	
	protected string				m_CachedPlayerName;//cached from player identity
	protected string				m_CachedPlayerID;//cached from player identity
	
	#ifdef DIAG_DEVELOPER
	int m_IsInsideTrigger;
	bool m_CanBeTargetedDebug; //server-side only
	ref array<EntityAI> m_PresetItems = new array<EntityAI>;
	bool m_PresetSpawned;
	ref CameraToolsMenuClient m_CameraToolsMenuClient;
	protected PluginTargetTemperature m_TargetTemperatureDiag;
	#endif	
	
	#ifdef DEVELOPER
	Transport m_GetInVehicleTransportDebug;
	#endif
	
	#ifdef DIAG_DEVELOPER
	ref Bot							m_Bot;
	#endif
	
	//Temp QuickBar load	
	ref array<ref Param2<EntityAI,int> >	m_aQuickBarLoad;
	
	//hologram
	ref Hologram m_HologramServer;
	ref Hologram m_HologramLocal;
	vector m_LocalProjectionPosition = "0 0 0 ";
	vector m_LocalProjectionOrientation = "0 0 0 "; 

	//Sheduler
	float m_LastTick = -1;
	int m_AddModifier = -1;
	//crafting start
	int m_RecipeID = -1;
	EntityAI m_Item1;
	EntityAI m_Item2;
	bool m_IsCraftingReady;
	float m_RecipeAnimLength;
	vector m_CraftingInitialPos;
	ref array<int> m_Recipes;
	//crafting end
	ref SoundParams m_SaySoundParams;
	ref SoundObjectBuilder m_SaySoundBuilder;
	ref SoundObject m_SaySoundObject;
	string m_SaySoundLastSetName;
	//input control
	bool m_ActionQBControl;
	
	//Action data for base building actions
	ref ConstructionActionData m_ConstructionActionData;
	//Action data for fireplace (indoor)
	vector m_LastFirePoint;
	float m_LastFirePointRot;
	int m_LastFirePointIndex;
	
	bool ItemToInventory;
	bool m_IsFighting = false;
	bool m_IsDrowning;//Set only via the setter SetDrowning

	// debug monitor
	int m_DebugMonitorEnabled;
	
	//map
	ActionUnfoldMapCB m_hac;
	bool m_MapOpen;
	bool m_MapCloseRequestProcessed; //DEPRECATED
	protected bool m_MapClosingSyncSent;
	protected float m_LastMapScale = -1.0;
	protected vector m_LastMapPos;
	protected ref MapNavigationBehaviour 	m_MapNavigationBehaviour;
	
	//inventory soft locking
	protected bool 					m_InventorySoftLocked; //obsolete, count should be checked instead. Left in here for legacy's sake.
	protected int 					m_InventorySoftLockCount = 0;
	static ref TStringArray			m_QBarItems;
	
	//Analytics
	string							m_DatabaseID;
	ref Timer						m_AnalyticsTimer;
	
	//! melee stats
	protected bool 					m_MeleeDebug;
	//protected bool 					m_CheckMeleeItem;
	protected ItemBase 				m_CheckMeleeItem;
	
	//! user actions & symptoms
	protected string 				m_UALastMessage;
	protected ref Timer 			m_UALastMessageTimer;
	protected int					m_PerformedAnimActionID = -1; 	// animated action being performed, -1 if none
	protected int 					m_ActivePrimarySymptomID = 0; 	// active primary symptom, 0 if none, symptom has to be overriden to allow sync
	
	bool 							m_WorkingNVGHeadset; //Deprecated
	bool 							m_LoweredNVGHeadset;
	protected ref array<int> 		m_ActiveNVTypes;
	//bool 							m_PreviousNVGState;
	int								m_SyncedModifiers;			
	int								m_SyncedModifiersPrev;			
	PluginAdminLog 					m_AdminLog; 
	ref PlayerStomach m_PlayerStomach;
	
	protected static Particle m_ContaminatedAroundPlayer;
	protected static Particle m_ContaminatedAroundPlayerTiny;
	
	protected PlayerStat<float> m_StatWater;
	protected PlayerStat<float> m_StatToxicity;
	protected PlayerStat<float> m_StatEnergy;
	protected PlayerStat<float> m_StatHeatComfort;
	protected PlayerStat<float> m_StatTremor;
	protected PlayerStat<int> m_StatWet
	protected PlayerStat<int> m_StatBloodType
	protected PlayerStat<float> m_StatDiet
	protected PlayerStat<float> m_StatStamina
	protected PlayerStat<float> m_StatSpecialty
	protected PlayerStat<float> m_StatHeatBuffer
	
	//!effect widgets
	protected GameplayEffectWidgets_base 	m_EffectWidgets;
	protected ref array<int> 				m_ProcessAddEffectWidgets;
	protected ref array<int> 				m_ProcessRemoveEffectWidgets;
	
	//postprocess processing
	protected ref array<int> 				m_ProcessAddGlassesEffects;
	protected ref array<int> 				m_ProcessRemoveGlassesEffects;
	
	protected EMedicalDrugsType m_MedicalDrugsActive;

	//! invokers	
	protected ref ScriptInvoker		m_OnUnconsciousStart;
	protected ref ScriptInvoker		m_OnUnconsciousStop;
	
	private ref map<eAgents, float> m_BloodyHandsPenaltyChancePerAgent;
	
	void PlayerBase()
	{	
		Init();
	}
	
	void Init()
	{
		SetEventMask(EntityEvent.INIT|EntityEvent.POSTFRAME|EntityEvent.FRAME);
		m_StoreLoadVersion = 0;
		m_IsCraftingReady = false;
		m_Recipes = new array<int>;
		m_LastShavedSeconds = 0;
		m_BloodType = 0;
		m_HasBloodTypeVisible = false;
		m_LifeSpanState = 0;
		m_LifespanLevelLocal = -1;
		m_CorpseState = 0;
		m_CorpseStateLocal = 0;
		m_HasBloodyHandsVisible = 0;
		m_PlayerLoaded = false;
		m_PlayerSelected = false;
		m_ProcessUIWarning = false;
		m_FlagRaisingTendency = true;
		m_LiquidTendencyDrain = false;
		m_UAParamMessage = new Param1<string>("");
		m_UnconParam = new Param1<float>(0);
		m_DeathDarkeningParam = new Param1<float>(0);
		m_UAParam = new Param2<int,int>(0,0);
		m_UAProgressParam = new Param2<float,float>(0,0);
		m_QuickBarBase = new QuickBarBase(this);
		m_QuickBarBonus = Math.Max(ConfigGetInt("quickBarBonus"), 0);
		m_CargoLoad = 0;
		m_VisibilityCoef = 1.0;
		m_Hud = GetGame().GetMission().GetHud();
		m_RecipePick = 0;
		m_ActionQBControl = false;
		m_QuickBarHold = false;
		m_HideHairAnimated = true;
		m_WorkingNVGHeadset = false;
		m_LoweredNVGHeadset = false;
		m_AreHandsLocked = false;
		m_ItemsToDelete = new array<EntityAI>;
		m_AnimCommandStarting = HumanMoveCommandID.None;
		m_EmptyGloves = new HiddenSelectionsData(ConfigGetString("emptyGloves"));
		m_CanDisplayHitEffectPPE = true;
		m_HeadingRestrictData = new HeadingRestrictData();
		
		#ifdef DIAG_DEVELOPER
			m_CanBeTargetedDebug = true;
			//RegisterNetSyncVariableBool("m_CanBeTargetedDebug");
		#endif
		
		m_AnalyticsTimer = new Timer(CALL_CATEGORY_SYSTEM);

		m_StaminaHandler = new StaminaHandler(this);//current stamina calculation
		m_InjuryHandler = new InjuryAnimationHandler(this);
		m_ShockHandler = new ShockHandler(this); //New shock handler
		m_HCAnimHandler = new HeatComfortAnimHandler(this);
		m_PlayerStats = new PlayerStats(this);//player stats
		m_ArrowManager = new ArrowManagerPlayer(this);
		
		if (GetGame().IsServer())
		{
			m_PlayerStomach = new PlayerStomach(this);
			m_NotifiersManager = new NotifiersManager(this); // player notifiers 
			m_AgentPool = new PlayerAgentPool(this); // agent pool manager
			m_BleedingManagerServer = new BleedingSourcesManagerServer(this);
			m_Environment = new Environment(this);//environment effects on player

			m_ModifiersManager = new ModifiersManager(this); // player modifiers
			m_PlayerSoundManagerServer = new PlayerSoundManagerServer(this);
			m_VirtualHud = new VirtualHud(this);
			
			m_AdminLog = PluginAdminLog.Cast(GetPlugin(PluginAdminLog));
		}
		
		m_SymptomManager = new SymptomManager(this); // state manager
		m_ModuleRecipesManager = PluginRecipesManager.Cast(GetPlugin(PluginRecipesManager));
		
		m_TrasferValues = new TransferValues(this);
		m_EmoteManager = new EmoteManager(this);
		m_SoftSkillsManager = new SoftSkillsManager(this);//Soft Skills calculation
		
		m_WeaponManager = new WeaponManager(this);
		m_DebugMonitorValues = new DebugMonitorValues(this);
		m_RGSManager = new RandomGeneratorSyncManager(this);
		
		if (!GetGame().IsDedicatedServer())
		{
			m_MeleeDebug = false;
			m_UALastMessage = "";
			m_UALastMessageTimer = new Timer;
			m_InventoryActionHandler = new InventoryActionHandler(this);
			m_BleedingManagerRemote = new BleedingSourcesManagerRemote(this);
			m_PlayerSoundManagerClient = new PlayerSoundManagerClient(this);
			m_StanceIndicator = new StanceIndicator(this);
			m_ActionsInitialize = false;
			ClientData.AddPlayerBase(this);

			m_ProcessAddEffectWidgets = new array<int>;
			m_ProcessRemoveEffectWidgets = new array<int>;
			m_ActiveNVTypes = new array<int>;
			m_ProcessAddGlassesEffects = new array<int>;
			m_ProcessRemoveGlassesEffects = new array<int>;
		}

		m_ActionManager = NULL;
		
		m_ConfigEmotesProfile = PluginConfigEmotesProfile.Cast(GetPlugin(PluginConfigEmotesProfile));
		
		if (GetDayZGame().IsDebug())
		{
			PluginConfigDebugProfileFixed m_ConfigDebugProfileFixed = PluginConfigDebugProfileFixed.Cast(GetPlugin(PluginConfigDebugProfileFixed));
			PluginConfigDebugProfile m_ConfigDebugProfile = PluginConfigDebugProfile.Cast(GetPlugin(PluginConfigDebugProfile));
			PluginDeveloper m_Developer = PluginDeveloper.Cast(GetPlugin(PluginDeveloper));
			
			PlayerBase player = this;
			
			if (m_ConfigDebugProfile)
			{
				string default_preset = m_ConfigDebugProfile.GetDefaultPreset();
				if (player && default_preset != "")
				{
					TStringArray preset_array = new TStringArray;
					m_ConfigDebugProfileFixed.GetPresetItems(default_preset, preset_array);
					
					bool is_preset_fixed = true;
					if (preset_array.Count() == 0)
					{
						m_ConfigDebugProfile.GetPresetItems(default_preset, preset_array);
						is_preset_fixed = false;
					}
					
					if (preset_array.Count() > 0)
					{
						m_Developer.ClearInventory(player);
						
						for (int i = 0; i < preset_array.Count(); i++)
						{
							float health = -1;
							int quantity = -1;
							if (is_preset_fixed)
							{
								health = m_ConfigDebugProfileFixed.GetItemHealth(default_preset, i);
								quantity = m_ConfigDebugProfileFixed.GetItemQuantity(default_preset, i);
							}
							else
							{
								health = m_ConfigDebugProfile.GetItemHealth(default_preset, i);
								quantity = m_ConfigDebugProfile.GetItemQuantity(default_preset, i);
							}
							m_Developer.SpawnEntityInPlayerInventory(player, preset_array.Get(i), health, quantity);
						}
					}
				}
			}
		}
		
		m_MedicalDrugsActive = EMedicalDrugsType.NONE;
		
		m_MapNavigationBehaviour			= new MapNavigationBehaviour(this);
		
		m_ModulePlayerStatus	= PluginPlayerStatus.Cast(GetPlugin(PluginPlayerStatus));
		m_ModuleLifespan		= PluginLifespan.Cast(GetPlugin(PluginLifespan));
		
		m_BleedingSourcesLow = new array<string>;
		m_BleedingSourcesLow.Insert("RightToeBase");
		m_BleedingSourcesLow.Insert("RightFoot");
		m_BleedingSourcesLow.Insert("LeftToeBase");
		m_BleedingSourcesLow.Insert("LeftFoot");
		m_BleedingSourcesLow.Insert("RightUpLegRoll");
		m_BleedingSourcesLow.Insert("RightUpLeg");
		m_BleedingSourcesLow.Insert("RightLegRoll");
		m_BleedingSourcesLow.Insert("RightLeg");
		m_BleedingSourcesLow.Insert("LeftUpLegRoll");
		m_BleedingSourcesLow.Insert("LeftUpLeg");
		m_BleedingSourcesLow.Insert("LeftLegRoll");
		m_BleedingSourcesLow.Insert("LeftLeg");
		
		m_BleedingSourcesUp = new array<string>;
		m_BleedingSourcesUp.Insert("RightForeArmRoll");
		m_BleedingSourcesUp.Insert("LeftForeArmRoll");
		m_BleedingSourcesUp.Insert("RightForeArm");
		m_BleedingSourcesUp.Insert("RightArmRoll");
		m_BleedingSourcesUp.Insert("RightArm");
		m_BleedingSourcesUp.Insert("RightShoulder");
		m_BleedingSourcesUp.Insert("LeftForeArm");
		m_BleedingSourcesUp.Insert("LeftArmRoll");
		m_BleedingSourcesUp.Insert("LeftArm");
		m_BleedingSourcesUp.Insert("LeftShoulder");
		m_BleedingSourcesUp.Insert("Spine3");
		m_BleedingSourcesUp.Insert("Spine2");
		m_BleedingSourcesUp.Insert("Spine1");
		m_BleedingSourcesUp.Insert("Spine");
		m_BleedingSourcesUp.Insert("Pelvis");
		m_BleedingSourcesUp.Insert("Neck");
		
		RegisterNetSyncVariableInt("m_PersistentFlags", 0, 1<<(EnumTools.GetEnumSize(PersistentFlag) + 1));
		RegisterNetSyncVariableInt("m_LifeSpanState", LifeSpanState.BEARD_NONE, LifeSpanState.COUNT);
		RegisterNetSyncVariableInt("m_BloodType", 0, 128);
		RegisterNetSyncVariableInt("m_ShockSimplified",0, SIMPLIFIED_SHOCK_CAP);
		RegisterNetSyncVariableInt("m_SoundEvent",0, EPlayerSoundEventID.ENUM_COUNT - 1);
		RegisterNetSyncVariableInt("m_SoundEventParam", 0, ((EPlayerSoundEventParam.ENUM_COUNT - 1) * 2) - 1);
		RegisterNetSyncVariableInt("m_StaminaState",0, eStaminaState.COUNT - 1);
		RegisterNetSyncVariableInt("m_BleedingBits");
		RegisterNetSyncVariableInt("m_Shakes", 0, SHAKE_LEVEL_MAX);
		RegisterNetSyncVariableInt("m_BreathVapour", 0, BREATH_VAPOUR_LEVEL_MAX);
		RegisterNetSyncVariableInt("m_HealthLevel", eInjuryHandlerLevels.PRISTINE, eInjuryHandlerLevels.RUINED);
		RegisterNetSyncVariableInt("m_MixedSoundStates", 0, eMixedSoundStates.COUNT - 1);
		RegisterNetSyncVariableInt("m_CorpseState",-PlayerConstants.CORPSE_STATE_DECAYED,PlayerConstants.CORPSE_STATE_DECAYED);//do note the negative min, negative sign denotes a special meaning
		RegisterNetSyncVariableInt("m_RefreshAnimStateIdx",0,3);
		RegisterNetSyncVariableInt("m_BrokenLegState", -eBrokenLegs.BROKEN_LEGS_SPLINT, eBrokenLegs.BROKEN_LEGS_SPLINT);
		RegisterNetSyncVariableInt("m_SyncedModifiers", 0, ((eModifierSyncIDs.LAST_INDEX - 1) * 2) - 1);
		RegisterNetSyncVariableInt("m_HasBloodyHandsVisible", 0, eBloodyHandsTypes.LAST_INDEX - 1);
		RegisterNetSyncVariableInt("m_ActionSoundCategoryHash");
		RegisterNetSyncVariableInt("m_PerformedAnimActionID", -1, 2048); // generous upper limit for modding reasons
		RegisterNetSyncVariableInt("m_ActivePrimarySymptomID", 0, SymptomIDs.LAST_INDEX);

		RegisterNetSyncVariableInt("m_HeatBufferStage", 0, HeatBufferMdfr.NUMBER_OF_STAGES - 1);
		RegisterNetSyncVariableFloat("m_HeatBufferDynamicMax", 0.0, 1.0, 2);
		
		RegisterNetSyncVariableBool("m_IsUnconscious");
		RegisterNetSyncVariableBool("m_IsRestrained");
		RegisterNetSyncVariableBool("m_IsInWater");
		RegisterNetSyncVariableBool("m_InsideEffectArea");
		
		RegisterNetSyncVariableBool("m_HasBloodTypeVisible");
		RegisterNetSyncVariableBool("m_IsRestrainStarted");
		RegisterNetSyncVariableBool("m_IsRestrainPrelocked");
		RegisterNetSyncVariableBool("m_HasHeatBuffer");
		RegisterNetSyncVariableBool("m_IsInColdArea");
		
		RegisterNetSyncVariableFloat("m_CurrentShock"); //Register shock synchronized variable
		
		m_OriginalSlidePoseAngle = GetSlidePoseAngle();
		
		//! sets default hit position and cache it here (mainly for impact particles)
		m_DefaultHitPosition = SetDefaultHitPosition(GetDayZPlayerType().GetDefaultHitPositionComponent());
		
		m_DecayedTexture = ConfigGetString("decayedTexture");
		m_FliesIndex = -1;
		
		m_BloodyHandsPenaltyChancePerAgent = new map<eAgents, float>();
		
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Call(this.OnPlayerLoaded);
	}
	
	ScriptInvoker GetOnUnconsciousStart()
	{
		if (!m_OnUnconsciousStart)
			m_OnUnconsciousStart = new ScriptInvoker();

		return m_OnUnconsciousStart;
	}
	
	ScriptInvoker GetOnUnconsciousStop()
	{
		if (!m_OnUnconsciousStop)
			m_OnUnconsciousStop = new ScriptInvoker();

		return m_OnUnconsciousStop;
	}

	void AddMedicalDrugsInUse(EMedicalDrugsType drugType)
	{
		if ((m_MedicalDrugsActive & drugType) != drugType)
			m_MedicalDrugsActive = m_MedicalDrugsActive | drugType;
	}
	
	void RemoveMedicalDrugsInUse(EMedicalDrugsType drugType)
	{
		if ((m_MedicalDrugsActive & drugType) == drugType)
			m_MedicalDrugsActive &= ~drugType;
	}
	
	bool IsChelationActive()
	{
		return (m_MedicalDrugsActive & EMedicalDrugsType.CHELATION) == EMedicalDrugsType.CHELATION;
	}
	
	bool IsAntibioticsActive()
	{
		return (m_MedicalDrugsActive & EMedicalDrugsType.ANTIBIOTICS) == EMedicalDrugsType.ANTIBIOTICS;
	}
	
	void SetSoundCategoryHash(int hash)
	{
		m_ActionSoundCategoryHash = hash;
		SetSynchDirty();
	}
	
	void SetPerformedActionID(int id)
	{
		m_PerformedAnimActionID = id;
		SetSynchDirty();
	}
	
	int GetPerformedActionID()
	{
		return m_PerformedAnimActionID;
	}
	
	void SetActivePrimarySymptomID(SymptomIDs id)
	{
		m_ActivePrimarySymptomID = id;
		SetSynchDirty();
	}
	
	SymptomIDs GetActivePrimarySymptomID()
	{
		return m_ActivePrimarySymptomID;
	}
	
	void RequestTriggerEffect(EffectTrigger trigger, int ppeIdx = -1, int aroundId = ParticleList.CONTAMINATED_AREA_GAS_AROUND, int tinyId = ParticleList.CONTAMINATED_AREA_GAS_TINY,  string soundset = "",  bool partDynaUpdate = false, int newBirthRate = 0, bool forceUpdate = false)
	{
		//do note multiple triggers can be calling this method within a single frame depending on the contaminated area setup and dynamic gameplay events(such as gas grenades being deployed)
		
		if (!m_InsideEffectArea)
			return;
		bool allow = false;
		if (!m_CurrentEffectTrigger)
		{
			allow = true;
		}
		else if (trigger == m_CurrentEffectTrigger && forceUpdate)
		{
			allow = true;
			//Print("ON Enabling effect FORCED " + trigger);
		}
		else if (trigger.GetEffectsPriority() >  m_CurrentEffectTrigger.GetEffectsPriority())
		{
			RemoveCurrentEffectTrigger();
			allow = true;
		}
		if (allow)
		{
			/*
			Print("--------------------------------------------------");
			Print("ON Enabling effect " + trigger);
			Print("------------> soundset " + soundset);
			Print("--------------------------------------------------");
			*/
			m_CurrentEffectTrigger = trigger;
			SetContaminatedEffectEx( true, ppeIdx, aroundId, tinyId, soundset,partDynaUpdate, newBirthRate );
		}
	}
	
	
	void RemoveCurrentEffectTrigger()
	{
		if (m_CurrentEffectTrigger)
		{
			/*
			Print("--------------------------------------------------");
			Print("OFF Disabling effect");
			Print("--------------------------------------------------");
			*/
			SetContaminatedEffectEx( false, m_CurrentEffectTrigger.m_PPERequester );
			m_CurrentEffectTrigger = null;
		}
	}
	
	void IncreaseContaminatedAreaCount()
	{
		if (m_ContaminatedAreaCount == 0)
			OnContaminatedAreaEnterServer();
		m_ContaminatedAreaCount++;
	}
	
	void DecreaseContaminatedAreaCount()
	{
		m_ContaminatedAreaCount--;
		if (m_ContaminatedAreaCount <= 0)
		{
			m_ContaminatedAreaCount = 0;
			OnContaminatedAreaExitServer();
		}
	}	
	void IncreaseEffectAreaCount()
	{
		if (m_EffectAreaCount == 0)
		{
			m_InsideEffectArea = true;
			SetSynchDirty();
			OnPlayerIsNowInsideEffectAreaBeginServer();
		}
		m_EffectAreaCount++;
	}
	
	void DecreaseEffectAreaCount()
	{
		m_EffectAreaCount--;
		if (m_EffectAreaCount <= 0)
		{
			m_EffectAreaCount = 0;
			m_InsideEffectArea = false;
			SetSynchDirty();
			OnPlayerIsNowInsideEffectAreaEndServer();
		}
	}
	
	void SetInEffectAreaTypeFlag(EEffectAreaType flag)
	{
		int areaCount = m_EffectAreaOverlap.Get(flag);
		
	 	if (areaCount > 0)
			m_EffectAreaOverlap.Set(flag, m_EffectAreaOverlap.Get(flag) + 1);
		else 
			m_EffectAreaOverlap.Set(flag, 1);
		
		m_InEffectAreaType |= flag;
	}
	
	void UnsetInEffectAreaTypeFlag(EEffectAreaType flag)
	{
		int areaCount = m_EffectAreaOverlap.Get(flag);
		
		if (areaCount > 1)
			m_EffectAreaOverlap.Set(flag, m_EffectAreaOverlap.Get(flag) - 1);
		else 
		{
			m_EffectAreaOverlap.Set(flag, 0);
			m_InEffectAreaType &= ~flag;
		}
	}
	
	bool IsInEffectArea(EEffectAreaType area)
	{
		if (m_InEffectAreaType & area)
			return true;
		
		return false;
	}
	
	// Server side update when player is present in any effect area
	protected void OnUpdateEffectAreaServer(float timeSlice)
	{
		float timeSpent;
		
		if (IsInEffectArea(EEffectAreaType.HOT_SPRING))
		{
			float waterLevel = GetCurrentWaterLevel();
			if (waterLevel > 0.5)
			{
				timeSpent = m_EffectAreaTimeToTick.Get(EEffectAreaType.HOT_SPRING);
				timeSpent += timeSlice;
				if (timeSpent > HotSpringTrigger.HEAT_DAMAGE_TICK_TIME)
				{
					m_EffectAreaTimeToTick.Set(EEffectAreaType.HOT_SPRING, 0);
					HotSpringTrigger.ApplyEffects(this);
				}
				else 
					m_EffectAreaTimeToTick.Set(EEffectAreaType.HOT_SPRING, timeSpent);
			}
		}
		
		if (IsInEffectArea(EEffectAreaType.VOLCANIC))
		{
			timeSpent = m_EffectAreaTimeToTick.Get(EEffectAreaType.VOLCANIC);
			timeSpent += timeSlice;
			if (timeSpent > VolcanicTrigger.HEAT_DAMAGE_TICK_TIME)
			{
				m_EffectAreaTimeToTick.Set(EEffectAreaType.VOLCANIC, 0);
				VolcanicTrigger.ApplyEffects(this);
			}
			else 
				m_EffectAreaTimeToTick.Set(EEffectAreaType.VOLCANIC, timeSpent);
		}
	}
	
	protected void OnContaminatedAreaEnterServer()
	{
		GetModifiersManager().ActivateModifier( eModifiers.MDF_AREAEXPOSURE );
		//Print("Contaminated ENTERED <------------------------------------------------------------- FINAL");
	}
	
	protected void OnContaminatedAreaExitServer()
	{
		GetModifiersManager().DeactivateModifier( eModifiers.MDF_AREAEXPOSURE );
		//Print("Contaminated LEFT <------------------------------------------------------------- FINAL");
	}	
	
	protected void OnPlayerIsNowInsideEffectAreaBeginServer()
	{
		//Print("ANY EFFECT AREA ENTERED <------------------------------------------------------------- FINAL");
	}
	
	protected void OnPlayerIsNowInsideEffectAreaEndServer()
	{
		//Print("ANY EFFECT AREA LEFT <------------------------------------------------------------- FINAL");
	}
	
	protected void OnPlayerIsNowInsideEffectAreaBeginClient()
	{
		//Print("ANY EFFECT AREA ENTERED CLIENT <------------------------------------------------------------- FINAL");
	}
	
	protected void OnPlayerIsNowInsideEffectAreaEndClient()
	{
		RemoveCurrentEffectTrigger();
		//Print("ANY EFFECT AREA LEFT CLIENT <------------------------------------------------------------- FINAL");
	}
	
	//! Returns item that's on this player's attachment slot. Parameter slot_type should be a string from config parameter 'itemInfo[]' like "Legs", "Headgear" and so on.
	ItemBase GetItemOnSlot(string slot_type)
	{
		int slot_id = InventorySlots.GetSlotIdFromString(slot_type);
		EntityAI item_EAI = this.GetInventory().FindAttachment(slot_id);
		ItemBase item_IB = ItemBase.Cast(item_EAI);
		
		if (item_EAI  &&  !item_IB)
		{
			string str = "Warning! GetItemOnSlot() >> found item on slot " + slot_type + " can't be cast to ItemBase! Found item is " + item_EAI.GetType() + " and the player is " + GetType() + "!";
			Error(str);
			return null;
		}
		
		return item_IB;
	}
	
	//! Returns item on player's head. For an example, a headtorch.
	ItemBase GetItemOnHead()
	{
		return GetItemOnSlot("Headgear");
	}
	
	override int GetQuickBarBonus()
	{
		return m_QuickBarBonus;
	}
	
	//--------------------------------------------------------------------------
	// Inventory overrides
	//--------------------------------------------------------------------------
	override bool CanDropEntity (notnull EntityAI item)
	{
		if (GetInventory().HasInventoryReservation(item, null))
		{
			return false;
		}
		
		if (IsRestrained())
		{
			if (GetHumanInventory().GetEntityInHands() == item)
				return false;
		}
		
		return true; 
	}

	//--------------------------------------------------------------------------
	// PLAYER DAMAGE EVENT HANDLING
	//--------------------------------------------------------------------------
	
	int GetBreathVapourLevel()
	{
		return m_BreathVapour;
	}
	
	// adds state to and syncs sound states variable which is used to generate sound client-side
	void UnsetMixedSoundState(eMixedSoundStates state)
	{
		int bit_mask_remove = ~state;
		
		eMixedSoundStates new_states = m_MixedSoundStates & bit_mask_remove;

		if (new_states != m_MixedSoundStates)
		{
			m_MixedSoundStates = new_states;
			SetSynchDirty();
		}
	}
	
	// adds state to and syncs sound states variable which is used to generate sound client-side
	void SetMixedSoundState(eMixedSoundStates state)
	{
		eMixedSoundStates new_states = m_MixedSoundStates | state;
		
		if (new_states != m_MixedSoundStates)
		{
			m_MixedSoundStates = new_states;
			SetSynchDirty();
		}
	}
	
	override bool IsPlayer()
	{
		return true;
	}
	
	bool IsBleeding()
	{
		return (m_BleedingBits != 0);
	}
	
	void SetBleedingBits(int bits)
	{
		if (m_BleedingBits != bits)
		{
			if (m_BleedingBits == 0)
			{
				OnBleedingBegin();
			}
			else if (bits == 0)
			{
				OnBleedingEnd();
			}
		}
		m_BleedingBits = bits;
		SetSynchDirty();
	}
	
	int GetBleedingBits()
	{
		return m_BleedingBits;
	}
	
	void IncreaseDiseaseCount()
	{
		m_DiseaseCount++;
	}
	
	void DecreaseDiseaseCount()
	{
		m_DiseaseCount--;
	}	
	
	void IncreaseHealingsCount()
	{
		m_HealingsCount++;
	}
	
	void DecreaseHealingsCount()
	{
		m_HealingsCount--;
	}
	
	
	bool HasHealings()
	{
		return m_HealingsCount;//0 for no healings, 1+ for healings, gets truncated to 1
	}
	
	bool HasDisease()
	{
		return m_DiseaseCount;//0 for no disease, 1+ for disease, gets truncated to 1
	}
	
	
	EPulseType GetPulseType()
	{
		return m_PulseType;
	}
	
	void SetPulseType(EPulseType pulse_type)
	{
		m_PulseType = pulse_type;
	}
	
	void QueueAddEffectWidget(array<int> effects)
	{
		m_ProcessAddEffectWidgets.InsertArray(effects);
	}
	
	void QueueRemoveEffectWidget(array<int> effects)
	{
		m_ProcessRemoveEffectWidgets.InsertArray(effects);
	}
	
	void QueueAddGlassesEffect(int id)
	{
		m_ProcessAddGlassesEffects.Insert(id);
	}
	
	void QueueRemoveGlassesEffect(int id)
	{
		m_ProcessRemoveGlassesEffects.Insert(id);
	}
	
	DamageDealtEffect GetDamageDealtEffect()
	{
		return m_DamageDealtEffect;
	}
	
	override void SpawnDamageDealtEffect()
	{
		if (m_DamageDealtEffect)
		{
			delete m_DamageDealtEffect;
		}
		
		if (m_CanDisplayHitEffectPPE)
		{
			m_DamageDealtEffect = new DamageDealtEffect();
		}
	}
	
	void SpawnDamageDealtEffect2(Param param1 = null, Param param2 = null)
	{
		if (m_EffectRadial)
		{
			delete m_EffectRadial;
		}
		
		m_EffectRadial = new EffectRadial(param1,param2);
	}
	
	FlashbangEffect GetFlashbangEffect()
	{
		return m_FlashbangEffect;
	}
	
	void SpawnFlashbangEffect(PlayerBase player, bool visual)
	{
		if (m_FlashbangEffect)
		{
			m_FlashbangEffect.Stop();
			delete m_FlashbangEffect;
		}
		
		m_FlashbangEffect = new FlashbangEffect(player, visual);
	}
	
	ShockDealtEffect GetShockEffect()
	{
		return m_ShockDealtEffect;
	}
	
	void SpawnShockEffect(float intensity_max)
	{
		if (m_ShockDealtEffect)
		{
			delete m_ShockDealtEffect;
		}
		
		m_ShockDealtEffect = new ShockDealtEffect(intensity_max);
	}
	
	override void EEKilled(Object killer)
	{
		//Print(Object.GetDebugName(this) + " STS=" + GetSimulationTimeStamp() + " event EEKilled, player has died at STS=" + GetSimulationTimeStamp());
		
		if (m_AdminLog)
		{
			m_AdminLog.PlayerKilled(this, killer);
		}
		
		if (GetBleedingManagerServer()) 
			delete GetBleedingManagerServer();

		// kill character in database
		if (GetHive())
		{
			GetHive().CharacterKill(this);
		}
	
		// disable voice communication
		GetGame().EnableVoN(this, false); 
		if (!GetGame().IsDedicatedServer())
			ClientData.RemovePlayerBase(this);
		GetSymptomManager().OnPlayerKilled();
		
		if (GetEconomyProfile() && !m_CorpseProcessing && m_CorpseState == 0 && GetGame().GetMission().InsertCorpse(this))
		{
			m_CorpseProcessing = true;
			//Print("EEKilled - processing corpse");
		}
		
		if (GetGame().IsMultiplayer() && GetGame().IsServer())
		{
			if (GetGame().GetMission())
			{
				GetGame().GetMission().SyncRespawnModeInfo(GetIdentity());
			}
		}
		
		super.EEKilled(killer);
	}

	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
		
		if (m_AdminLog)
		{
			m_AdminLog.PlayerHitBy(damageResult, damageType, this, source, component, dmgZone, ammo);
		}
		
		if (damageResult != null && damageResult.GetDamage(dmgZone, "Shock") > 0)
		{
			m_LastShockHitTime = GetGame().GetTime();
			
			if (!IsUnconscious())
			{
				if (GetGame().ConfigIsExisting("cfgAmmo " + ammo + " unconRefillModifier"))
				{
					m_UnconRefillModifier = GetGame().ConfigGetInt("cfgAmmo " + ammo + " unconRefillModifier");
				}
				else
				{
					m_UnconRefillModifier = 1;
				}
			}
		}
		
		 //! DamageType.EXPLOSION & FlashGrenade
		if (damageType == DamageType.EXPLOSION && ammo == "FlashGrenade_Ammo")
		{
			GetStaminaHandler().DepleteStamina(EStaminaModifiers.OVERALL_DRAIN);
		}
		
		//new bleeding computation
		//---------------------------------------
		if (damageResult != null && GetBleedingManagerServer())
		{
			float dmg = damageResult.GetDamage(dmgZone, "Blood");
			GetBleedingManagerServer().ProcessHit(dmg, source, component, dmgZone, ammo, modelPos);
		}
		//---------------------------------------
		
		#ifdef DIAG_DEVELOPER
		if (DiagMenu.GetBool(DiagMenuIDs.MELEE_DEBUG))
			Print("EEHitBy() | " + GetDisplayName() + " hit by " + source.GetDisplayName() + " to " + dmgZone);
		
		PluginRemotePlayerDebugServer plugin_remote_server = PluginRemotePlayerDebugServer.Cast(GetPlugin(PluginRemotePlayerDebugServer));
		if (plugin_remote_server)
		{
			plugin_remote_server.OnDamageEvent(this, damageResult);
		}
		#endif
		
		if (GetGame().IsDebugMonitor())
			m_DebugMonitorValues.SetLastDamage(source.GetDisplayName());
		
		if (m_ActionManager)
			m_ActionManager.Interrupt();
		
		int transferShockToDamageCoef = g_Game.ConfigGetInt(string.Format("%1 %2 DamageApplied transferShockToDamage", CFG_AMMO, ammo));
		if (transferShockToDamageCoef == 1)
		{
			//Print("PlayerBase | EEHitBy | nonlethal hit");
			AddHealth("", "Health", -ConvertNonlethalDamage(damageResult.GetDamage(dmgZone, "Shock"), damageType));
			if (dmgZone != "Head")
				AddHealth(dmgZone, "Health", -damageResult.GetDamage(dmgZone, "Shock")); //Also deal damage to zone health, no dmg reduction
		}
		
		if (GetGame().IsServer())
		{
			if (GetHealth("RightLeg", "Health") <= 1 || GetHealth("LeftLeg", "Health") <= 1 || GetHealth("RightFoot", "Health") <= 1 || GetHealth("LeftFoot", "Health") <= 1)
			{
				if (GetModifiersManager().IsModifierActive(eModifiers.MDF_BROKEN_LEGS))//effectively resets the modifier
				{
					GetModifiersManager().DeactivateModifier(eModifiers.MDF_BROKEN_LEGS);
				}
				GetModifiersManager().ActivateModifier(eModifiers.MDF_BROKEN_LEGS);
			}
			
			if (ammo == "Bullet_CupidsBolt" && IsAlive())
			{
				DamageSystem.ResetAllZones(this);
				m_ModifiersManager.ResetAll();
				m_ModifiersManager.ActivateModifier(eModifiers.MDF_IMMUNITYBOOST);
				
				// bleeding sources
				if (m_BleedingManagerServer)
					m_BleedingManagerServer.RemoveAllSources();
				
				// Stats
				if (GetPlayerStats())
				{
					int bloodType 		= GetStatBloodType().Get();
					float energyValue 	= GetStatEnergy().Get();
					float waterValue 	= GetStatWater().Get();
					float heatBuffer	= GetStatHeatBuffer().Get();
					float heatComfort	= GetStatHeatComfort().Get();

					GetPlayerStats().ResetAllStats();

					GetStatBloodType().Set(bloodType);
					GetStatWater().Set(waterValue);
					GetStatEnergy().Set(energyValue);
					GetStatHeatBuffer().Set(heatBuffer);
					GetStatHeatComfort().Set(heatComfort);
				}
	
				// Agents
				if (m_AgentPool)
					m_AgentPool.RemoveAllAgents();
				
				if (m_StaminaHandler)
					m_StaminaHandler.SetStamina(GameConstants.STAMINA_MAX);
				
				// uncon
				if (IsUnconscious())
					DayZPlayerSyncJunctures.SendPlayerUnconsciousness(this, false);
			}
		}
		
		m_ShockHandler.CheckValue(true);
		
		//analytics
		GetGame().GetAnalyticsServer().OnEntityHit(source, this);
	}
	
	override void EEHitByRemote(int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos)
	{
		super.EEHitByRemote(damageType, source, component, dmgZone, ammo, modelPos);

		if (m_MeleeFightLogic.IsInBlock())
		{
			EffectSound sound = SEffectManager.PlaySoundOnObject("BlockingAttack_SoundSet", this);
			sound.SetAutodestroy(true);
		}
	}
	
	override void EEDelete(EntityAI parent)
	{
		SEffectManager.DestroyEffect(m_FliesEff);
		StopSoundSet(m_SoundFliesEffect);

		if (GetArrowManager())
			GetArrowManager().ClearArrows();
	}

	override protected float ConvertNonlethalDamage(float damage, DamageType damageType)
	{
		if (damageType == DamageType.FIRE_ARM)
			return damage * GameConstants.NL_DAMAGE_FIREARM_CONVERSION_PLAYERS;
		
		return super.ConvertNonlethalDamage(damage, damageType);
	}
	
	/** Call only on client or single player PlayerBase
 	 *  (as GetGame().GetPlayer())
	 */
	override void OnReceivedHit(ImpactEffectsData hitData)
	{
		if (GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_CLIENT)
		{
			float shakeStrength = Math.InverseLerp(0, 500, hitData.m_InSpeed.Length());
			GetCurrentCamera().SpawnCameraShake(shakeStrength);
			OnPlayerRecievedHit();
		}
					
		if (hitData.m_AmmoType == "Bullet_CupidsBolt")
			Ammo_CupidsBolt.PlayOnHitParticle(hitData.m_Position);
	}

	// DEPRECATED by method OnReceivedHit
	override void OnPlayerRecievedHit()
	{
		#ifndef NO_GUI
		if (m_MeleeFightLogic.IsInBlock())
		{
			EffectSound sound = SEffectManager.PlaySoundOnObject("BlockingAttack_SoundSet", this);
			sound.SetAutodestroy(true);
			return;
		}

		SpawnDamageDealtEffect();
		CachedObjectsParams.PARAM2_FLOAT_FLOAT.param1 = 32;
		CachedObjectsParams.PARAM2_FLOAT_FLOAT.param2 = 0.3;
		SpawnDamageDealtEffect2(CachedObjectsParams.PARAM2_FLOAT_FLOAT);
		
		CloseMapEx(true);		
		#endif
	}
	
	void OnPlayerReceiveFlashbangHitStart(bool visual)
	{
		SpawnFlashbangEffect(this, visual);
	}

	void OnPlayerReceiveFlashbangHitEnd() {}

	//--------------------------------------------------------------------------	
	//! Melee helpers
	//--------------------------------------------------------------------------

	//! select & return hit component for attacking AI
	override string GetHitComponentForAI()
	{
		return GetDayZPlayerType().GetHitComponentForAI();
	}

	//! returns default hit component (fallback)
	override string GetDefaultHitComponent()
	{
		return GetDayZPlayerType().GetDefaultHitComponent();
	}
	
	override vector GetDefaultHitPosition()
	{
		return m_DefaultHitPosition;
	}
	
	//! returns list of suitable finisher hit components (defined on base entity/entity type)
	override array<string> GetSuitableFinisherHitComponents()
	{
		return GetDayZPlayerType().GetSuitableFinisherHitComponents();
	}
	
	protected vector SetDefaultHitPosition(string pSelection)
	{
		return GetSelectionPositionMS(pSelection);
	}

	//--------------------------------------------------------------------------
	//!DEPRECATED!
	void AddPossibleCoverFaceForShave()
	{
		 m_FaceCoveredForShaveLayers++;
	}
	//!DEPRECATED!
	void RemovePossibleCoverFaceForShave()
	{
		 m_FaceCoveredForShaveLayers--;
	} 
	//--------------------------------------------------------------------------
	override void EEItemAttached(EntityAI item, string slot_name)
	{
		super.EEItemAttached(item, slot_name);

		ItemBase itemIB = ItemBase.Cast(item);
		SwitchItemSelectionTexture(item, slot_name);
		Param1<PlayerBase> p = new Param1<PlayerBase>(this);
		item.SwitchItemSelectionTextureEx(EItemManipulationContext.ATTACHING, p);
		m_QuickBarBase.updateSlotsCount();
		CalculateVisibilityForAI();
		UpdateShoulderProxyVisibility(item, slot_name);
		
		HideHairSelections(itemIB,true);
		
		GetGame().GetAnalyticsClient().OnItemAttachedAtPlayer(item, slot_name);
		Clothing clothing = Clothing.Cast(item);
		if (clothing)
		{
			if (!GetGame().IsDedicatedServer())
			{
				if (clothing.GetEffectWidgetTypes())
				{
					QueueAddEffectWidget(clothing.GetEffectWidgetTypes());
				}
				
				if (clothing.GetGlassesEffectID() > -1)
				{
					QueueAddGlassesEffect(clothing.GetGlassesEffectID());
				}
				
				GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(UpdateCorpseStateVisual, 200, false);//sometimes it takes a while to load in
				UpdateCorpseStateVisual();//....but if possible, we don't want a delay
			}
			else if (GetGame().IsServer())
			{
				if (clothing.IsGasMask())
				{
					GetModifiersManager().ActivateModifier(eModifiers.MDF_MASK);
				}
			}
			
			clothing.UpdateNVGStatus(this,true);
		}
		
		AdjustBandana(item,slot_name);
		AdjustShemag(item,slot_name);
	}
	
	override void EEItemDetached(EntityAI item, string slot_name)
	{
		super.EEItemDetached(item, slot_name);
		ItemBase item_base = ItemBase.Cast(item);
		SwitchItemSelectionTexture(item, slot_name);
		item.SwitchItemSelectionTextureEx(EItemManipulationContext.DETACHING);
		m_QuickBarBase.updateSlotsCount();
		CalculateVisibilityForAI();
		
		HideHairSelections(item_base,false);
		
		Clothing clothing = Clothing.Cast(item);
		
		if (clothing)
		{
			if (!GetGame().IsDedicatedServer())
			{
				if (clothing.GetEffectWidgetTypes())
				{
					QueueRemoveEffectWidget(clothing.GetEffectWidgetTypes());
				}
				
				if (clothing.GetGlassesEffectID() > -1)
				{
					QueueRemoveGlassesEffect(clothing.GetGlassesEffectID());
				}
			}
			
			if (GetGame().IsServer())
			{
				if (clothing.IsGasMask())
				{
					GetModifiersManager().DeactivateModifier(eModifiers.MDF_MASK);
				}
			}
			
			clothing.UpdateNVGStatus(this, false, true);
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(UpdateCorpseStateVisual, 200, false);//sometimes it takes a while to load in
			UpdateCorpseStateVisual();//....but if possible, we don't want a delay
		}
	}

	void UpdateShoulderProxyVisibility(EntityAI item, string slot_name)
	{
		string slot = slot_name;
		bool boo;
		boo = item.IsWeapon();
		
		if (slot == "Melee")
		{
			SetSimpleHiddenSelectionState(SIMPLE_SELECTION_MELEE_RIFLE,boo);
			SetSimpleHiddenSelectionState(SIMPLE_SELECTION_MELEE_MELEE,!boo);
		}
		else if (slot == "Shoulder")
		{
			SetSimpleHiddenSelectionState(SIMPLE_SELECTION_SHOULDER_RIFLE,boo);
			SetSimpleHiddenSelectionState(SIMPLE_SELECTION_SHOULDER_MELEE,!boo);
		}
	}
	
	override void SwitchItemSelectionTexture(EntityAI item, string slot_name)
	{
		super.SwitchItemSelectionTexture(item,slot_name);
		
		//shirt + armband
		EntityAI armband = FindAttachmentBySlotName("Armband");
		if (slot_name == "Body" && armband)
		{
			Param1<PlayerBase> p = new Param1<PlayerBase>(this);
			armband.SwitchItemSelectionTextureEx(EItemManipulationContext.UPDATE,p);
		}
	}
	
	void RemoveAllItems()
	{
		array<EntityAI> itemsArray = new array<EntityAI>;
		ItemBase item;
		GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, itemsArray);
		
		for (int i = 0; i < itemsArray.Count(); i++)
		{
			Class.CastTo(item, itemsArray.Get(i));
			if (item && !item.IsInherited(SurvivorBase))	GetInventory().LocalDestroyEntity(item);
		}
	}
	
	bool GetHitPPEEnabled()
	{
		return m_CanDisplayHitEffectPPE;
	}
	
	void SetHitPPEEnabled(bool enabled)
	{
		m_CanDisplayHitEffectPPE = enabled;
	}
	
	bool IsMale()
	{
		if (ConfigGetBool("woman") != 1)
		{
			return true;
		}
		return false;
	}
	
	int GetVoiceType()
	{
		int voice_type = ConfigGetInt("voiceType");
		
		if (voice_type == 0)
		{
			voice_type = 1;
		}
		
		return voice_type;
	}
	
	PlayerSoundManagerServer GetPlayerSoundManagerServer()
	{
		return m_PlayerSoundManagerServer;
	}
		
	
	UndergroundHandlerClient GetUndergroundHandler()
	{
		if (!m_UndergroundHandler && IsAlive())
		{
			m_UndergroundHandler = new UndergroundHandlerClient(this);
		}
		
		return m_UndergroundHandler;
	}
	
	UndergroundBunkerHandlerClient GetUndergroundBunkerHandler()
	{
		if (!m_UndergroundBunkerHandler && IsAlive())
		{
			m_UndergroundBunkerHandler = new UndergroundBunkerHandlerClient(this);
		}
		
		return m_UndergroundBunkerHandler;
	}
	
	void KillUndergroundHandler()
	{
		m_UndergroundHandler = null;
		m_UndergroundBunkerHandler = null;
	}
	// --------------------------------------------------
	// User Actions
	//---------------------------------------------------
	void SetActions(out TInputActionMap InputActionMap)
	{
		AddAction(ActionOpenDoors, InputActionMap);
		AddAction(ActionCloseDoors, InputActionMap);
		AddAction(ActionLockedDoors, InputActionMap);
		AddAction(ActionEnterLadder, InputActionMap);
		AddAction(ActionExitLadder, InputActionMap);
		
		//-----------CheckIfCanMOveToTarget----------
		AddAction(ActionStartEngine, InputActionMap);
		AddAction(ActionStartEngineBoat, InputActionMap);
		AddAction(ActionStopEngine, InputActionMap);
		AddAction(ActionStopEngineBoat, InputActionMap);
		AddAction(ActionSwitchSeats, InputActionMap);
		AddAction(ActionOpenCarDoors,InputActionMap);
		AddAction(ActionCloseCarDoors,InputActionMap);
		AddAction(ActionUncoverHeadSelf, InputActionMap);
		AddAction(ActionDrinkPondContinuous, InputActionMap);
		AddAction(ActionEatSnowContinuous, InputActionMap);
		
		AddAction(ActionIgniteFireplaceByAir, InputActionMap);
		AddAction(ActionMineBushByHand, InputActionMap);
		
		AddAction(ActionUngagSelf, InputActionMap);
		AddAction(ActionWashHandsWater, InputActionMap);
		AddAction(ActionWashHandsSnow, InputActionMap);
		AddAction(ActionGetOutTransport, InputActionMap);
		
		AddAction(ActionUnrestrainTargetHands, InputActionMap);
		AddAction(ActionTakeArrow, InputActionMap);
		AddAction(ActionTakeArrowToHands, InputActionMap);
		AddAction(ActionOperatePanelPowerStation, InputActionMap);
	}
	
	void SetActions() // Backwards compatibility, not recommended to use
	{
	}
	
	void SetActionsRemoteTarget(out TInputActionMap InputActionMap)
	{
		AddAction(ActionCPR, InputActionMap);
		AddAction(ActionUncoverHeadTarget, InputActionMap);
		AddAction(ActionUngagTarget, InputActionMap);
		AddAction(ActionPullBodyFromTransport, InputActionMap);
		AddAction(ActionCheckPulseTarget, InputActionMap);
	}
	
	void SetActionsRemoteTarget() // Backwards compatibility, not recommended to use
	{
	}
	
	void InitializeActions()
	{
		// Backwards compatibility
		if (IsControlledPlayer())
		{
			m_InputActionMap = new TInputActionMap;
			SetActions();
		}
		else
		{
			m_InputActionMap = new TInputActionMap;
			SetActionsRemoteTarget();
		}
		//////////////////////////
		
		m_InputActionMapControled = new TInputActionMap;
		SetActions(m_InputActionMapControled);

		m_InputActionMapAsTarget = new TInputActionMap;
		SetActionsRemoteTarget(m_InputActionMapAsTarget);
	}
	
	override void GetActions(typename action_input_type, out array<ActionBase_Basic> actions)
	{
		if (!m_ActionsInitialize)
		{
			m_ActionsInitialize = true;
			InitializeActions();
		}

		// Backwards compatibility
		array<ActionBase_Basic> bcActions = m_InputActionMap.Get(action_input_type);
		if (!bcActions)
		{
			if (IsControlledPlayer())
				actions = m_InputActionMapControled.Get(action_input_type);
			else
				actions = m_InputActionMapAsTarget.Get(action_input_type);
		}
		else
		{
			if (!actions)
				actions = new array<ActionBase_Basic>();
			
			if (IsControlledPlayer())
				actions.InsertAll(m_InputActionMapControled.Get(action_input_type));
			else
				actions.InsertAll(m_InputActionMapAsTarget.Get(action_input_type));
			
			actions.InsertAll(bcActions);
		}
	}
	
	void AddAction(typename actionName, out TInputActionMap InputActionMap)
	{
		ActionBase action = GetActionManager().GetAction(actionName);

		typename ai = action.GetInputType();
		if (!ai)
		{
			m_ActionsInitialize = false;
			return;
		}
		
		ref array<ActionBase_Basic> action_array = InputActionMap.Get(ai);
		
		if (!action_array)
		{
			action_array = new array<ActionBase_Basic>;
			InputActionMap.Insert(ai, action_array);
		}
		action_array.Insert(action);
	}
	
	void AddAction(typename actionName) // Backwards compatibility, not recommended to use
	{
		AddAction(actionName, m_InputActionMap);
	}
	
	void RemoveAction(typename actionName, out TInputActionMap InputActionMap)
	{
		ActionBase action = GetActionManager().GetAction(actionName);
		typename ai = action.GetInputType();
		ref array<ActionBase_Basic> action_array = InputActionMap.Get(ai);
		
		if (action_array)
		{
			for (int i = 0; i < action_array.Count(); i++)
			{
				if (action == action_array.Get(i))
				{
					action_array.Remove(i);
				}
			}
			action_array = new array<ActionBase_Basic>;
			InputActionMap.Insert(ai, action_array);
		}
		action_array.Insert(action); 
	}
	
	void RemoveAction(typename actionName) // Backwards compatibility, not recommended to use
	{
		RemoveAction(actionName, m_InputActionMap);
	}
	
	int GetCurrentRecipe()
	{
		return m_RecipePick;
	}
	
	void SetNextRecipe()
	{
		m_RecipePick++;
	}
	
	void SetFirstRecipe()
	{
		m_RecipePick = 0;
	}
	
	//---------------------------------------------------
	
	
	ActionManagerBase GetActionManager()
	{
		return m_ActionManager;
	}
	
	EmoteManager GetEmoteManager()
	{
		return m_EmoteManager;
	}
	
	RandomGeneratorSyncManager GetRandomGeneratorSyncManager()
	{
		return m_RGSManager;
	}
	
	//!called every command handler tick when player is on ladder
	override void OnLadder(float delta_time, HumanMovementState pState)
	{
		ProcessHandDamage(delta_time, pState);
	}
	
	void ProcessHandDamage(float delta_time, HumanMovementState pState)
	{
		if (GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_SERVER || !GetGame().IsMultiplayer())
		{
			if (pState.m_iMovement == DayZPlayerConstants.MOVEMENTIDX_SLIDE)
			{
				//Print("sliding down");
				EntityAI gloves = GetInventory().FindAttachment(InventorySlots.GLOVES);
			
				if (gloves && gloves.GetHealthLevel() < 4)
				{
					gloves.AddHealth("","", PlayerConstants.GLOVES_DAMAGE_SLIDING_LADDER_PER_SEC * delta_time);
					return;
				}
				
				if (Math.RandomFloat01() < PlayerConstants.CHANCE_TO_BLEED_SLIDING_LADDER_PER_SEC * delta_time)
				{
					if (Math.RandomFloat01() < 0.5)
					{
						if (GetBleedingManagerServer().AttemptAddBleedingSourceBySelection("LeftForeArmRoll"))
						{
							SendSoundEvent(EPlayerSoundEventID.INJURED_LIGHT);
						}
					}
					else
					{
						if (GetBleedingManagerServer().AttemptAddBleedingSourceBySelection("RightForeArmRoll"))
						{
							SendSoundEvent(EPlayerSoundEventID.INJURED_LIGHT);
						}
					}
				}
				
			}
		}
	}
	
	override void ProcessFeetDamageServer(int pUserInt)
	{
		//InventorySlots.HEADGEAR
		EntityAI shoes = GetInventory().FindAttachment(InventorySlots.FEET);
		
		string surface;	int liquid;
		GetGame().SurfaceUnderObject(this, surface, liquid);
		float modifier_surface = Surface.GetParamFloat(surface, "footDamage");
		
		if (shoes && shoes.GetHealthLevel() < 4)
		{
			shoes.AddHealth("", "", -1 * modifier_surface * PlayerConstants.SHOES_MOVEMENT_DAMAGE_PER_STEP * (float)PlayerConstants.CHECK_EVERY_N_STEP);

			return;
		}

		float rnd = Math.RandomFloat01();
		float modifier_movement = GetFeetDamageMoveModifier();
		float chance = modifier_movement * modifier_surface * PlayerConstants.BAREFOOT_MOVEMENT_BLEED_MODIFIER * (float)PlayerConstants.CHECK_EVERY_N_STEP;
		if (rnd < chance)
		{
			if (pUserInt % 2 == 0)
			{
				if (GetBleedingManagerServer().AttemptAddBleedingSourceBySelection("RightFoot"))
					SendSoundEvent(EPlayerSoundEventID.INJURED_LIGHT);
				
			}
			else
			{
				if (GetBleedingManagerServer().AttemptAddBleedingSourceBySelection("LeftFoot"))
					SendSoundEvent(EPlayerSoundEventID.INJURED_LIGHT);
			}
		}
	}
	
	float GetFeetDamageMoveModifier()
	{
		float modifier;
		switch(m_MovementState.m_iMovement)
		{
			case DayZPlayerConstants.MOVEMENTIDX_SPRINT:
				modifier = 1;
				break
			case DayZPlayerConstants.MOVEMENTIDX_RUN:
				modifier = 0.5;
				break 
			case DayZPlayerConstants.MOVEMENTIDX_WALK:
				modifier = 0;
				break
		}
		return modifier;
	}
	
	void SetStamina(int value, int range)
	{
		if (m_ModulePlayerStatus)
			m_ModulePlayerStatus.SetStamina(value, range);
	}
	
	#ifdef DIAG_DEVELOPER
	void SetStaminaDisabled(bool value)
	{
		if (m_StaminaHandler)
			m_StaminaHandler.SetStaminaDisabled(value);
	}
	#endif
	
	void SetQuickRestrain(bool enable)
	{
		m_AllowQuickRestrain = enable;
	}
	
	void SetQuickFishing(bool enable)
	{
		m_AllowQuickFishing = enable;
	}
	
	bool IsQuickRestrain()
	{
		return m_AllowQuickRestrain;
	}
	
	bool IsQuickFishing()
	{
		return m_AllowQuickFishing;
	}
	
	PlayerStats GetPlayerStats()
	{
		return m_PlayerStats;	
	}

	CraftingManager GetCraftingManager()
	{
		return m_CraftingManager;		
	}
	
	StaminaHandler GetStaminaHandler()
	{
		return m_StaminaHandler;		
	}
	
	override WeaponManager GetWeaponManager()
	{
		return m_WeaponManager;		
	}
	
	override ArrowManagerBase GetArrowManager()
	{
		return m_ArrowManager;
	}
	
	bool CanBeRestrained()
	{
		if (IsInVehicle() || IsRaised() || IsSwimming() || IsClimbing() || IsClimbingLadder() || IsRestrained() || !GetWeaponManager() || GetWeaponManager().IsRunning() || !GetActionManager() || GetActionManager().GetRunningAction() != null || IsMapOpen())
		{
			return false;
		}
		if (GetThrowing() && GetThrowing().IsThrowingModeEnabled())
		{
			return false;
		}
		return true;
	}
	
	void SetRestrainStarted(bool restrain_started)
	{
		m_IsRestrainStarted = restrain_started;
		SetSynchDirty();
	}
	
	bool IsRestrainStarted()
	{
		return m_IsRestrainStarted;
	}
	
	void SetRestrainPrelocked(bool restrain_prelock)
	{
		m_IsRestrainPrelocked = restrain_prelock;
		SetSynchDirty();
	}
	
	bool IsRestrainPrelocked()
	{
		return m_IsRestrainPrelocked;
	}
	
	void SetRestrained(bool	is_restrained)
	{
		m_IsRestrained = is_restrained;
		SetSynchDirty();
	}
	
	override bool IsRestrained()
	{
		return m_IsRestrained;
	}
	
	void OnRestrainStartedChangeClient()
	{
		if (m_IsRestrainStarted && IsControlledPlayer())
		{
			if (GetGame().GetUIManager().IsMenuOpen(MENU_RADIAL_QUICKBAR))
				GetGame().GetUIManager().FindMenu(MENU_RADIAL_QUICKBAR).Close();
			if (GetGame().GetUIManager().IsMenuOpen(MENU_INVENTORY))
				GetGame().GetMission().HideInventory();
		}
	}
	
	void OnRestrainChangeClient()
	{
		if (m_IsRestrained && IsControlledPlayer())
		{
			if (GetGame().GetUIManager().IsMenuOpen(MENU_RADIAL_QUICKBAR))
				GetGame().GetUIManager().FindMenu(MENU_RADIAL_QUICKBAR).Close();
			if (GetGame().GetUIManager().IsMenuOpen(MENU_INVENTORY))
				GetGame().GetMission().HideInventory();
		}
	}
	
	override bool IsInventoryVisible()
	{
		return true;
	}
	
	bool CanManipulateInventory()
	{
		if (IsControlledPlayer())
		{
			return !IsRestrained() && !IsRestrainPrelocked() && !IsRestrainStarted();
		}
		return true;
	}
	
	override bool CanReleaseAttachment (EntityAI attachment)
	{
		return super.CanReleaseAttachment(attachment);
	}
	
	override bool CanReleaseCargo (EntityAI cargo)
	{
		return super.CanReleaseCargo(cargo);
	}
	
	override bool CanReceiveItemIntoCargo(EntityAI item)
	{
		return super.CanReceiveItemIntoCargo(item);
	}
	
	override bool CanSwapItemInCargo (EntityAI child_entity, EntityAI new_entity)
	{
		return super.CanSwapItemInCargo(child_entity, new_entity);
	}
	
	override bool CanReceiveItemIntoHands(EntityAI item_to_hands)
	{
		if (IsInVehicle())
			return false;
		
		if (!CanPickupHeavyItem(item_to_hands))
			return false;
		
		return super.CanReceiveItemIntoHands(item_to_hands);
	}
	

	int GetCraftingRecipeID()
	{
		if (GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_CLIENT)
		{
			return GetCraftingManager().GetRecipeID();
		}
		else
		{
			return m_RecipeID;
		}		
	}
	
	void SetCraftingRecipeID(int recipeID)
	{
		if (GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_CLIENT)
		{
			GetCraftingManager().SetRecipeID(recipeID);
		}
		else
		{
			m_RecipeID = recipeID;			
		}		
	}
	
	// --------------------------------------------------
	// Action data for base building actions
	//---------------------------------------------------	
	ConstructionActionData GetConstructionActionData()
	{
		return m_ConstructionActionData;
	}
	
	void ResetConstructionActionData()
	{
		if (m_ConstructionActionData)
		{
			m_ConstructionActionData.ResetActionIndexes();
		}
	}
	
	// --------------------------------------------------
	// Action data for fireplace (indoor)
	//---------------------------------------------------	
	vector GetLastFirePoint()
	{
		return m_LastFirePoint;
	}	
	float GetLastFirePointRot()
	{
		return m_LastFirePointRot;
	}	
	int GetLastFirePointIndex()
	{
		return m_LastFirePointIndex;
	}

	void SetLastFirePoint(vector last_fire_point)
	{
		m_LastFirePoint = last_fire_point;
	}
	void SetLastFirePointRot(float last_fire_point_rot)
	{
		m_LastFirePointRot = last_fire_point_rot;
	}
	void SetLastFirePointIndex(int last_fire_point_index)
	{
		m_LastFirePointIndex = last_fire_point_index;
	}

	// --------------------------------------------------
	// QuickBar
	//---------------------------------------------------
	
	void RemoveQuickBarEntityShortcut(EntityAI entity)
	{
		int index = m_QuickBarBase.FindEntityIndex(entity);
		if (index != -1)
			m_QuickBarBase.SetEntityShortcut(entity,-1);
	}
	//---------------------------------------------------
	void SetEnableQuickBarEntityShortcut(EntityAI entity, bool value)
	{
		int index = m_QuickBarBase.FindEntityIndex(entity);
		if (index != -1)
			m_QuickBarBase.SetShotcutEnable(index,value);

	}
	//---------------------------------------------------
	int FindQuickBarEntityIndex(EntityAI entity)
	{
		int index;
		index = m_QuickBarBase.FindEntityIndex(entity);

		if (m_QuickBarBase.GetEntity(index) == NULL)
			return -1;

		return index;
	}
	//---------------------------------------------------
	
	int GetQuickBarSize()
	{
		return m_QuickBarBase.GetSize();
	}
	//---------------------------------------------------	
	EntityAI GetQuickBarEntity(int index)
	{
		return m_QuickBarBase.GetEntity(index);
	}
	//---------------------------------------------------
	void UpdateQuickBarEntityVisibility(EntityAI entity)
	{
		int i = FindQuickBarEntityIndex(entity);
		if (i >= 0)
			m_QuickBarBase.UpdateShotcutVisibility(i);
	}
	//---------------------------------------------------
	void SetQuickBarEntityShortcut(EntityAI entity, int index, bool force = false)
	{
		m_QuickBarBase.SetEntityShortcut(entity, index, force);
	}
	//---------------------------------------------------
	void OnQuickbarSetEntityRequest(ParamsReadContext ctx)
	{
		m_QuickBarBase.OnSetEntityRequest(ctx);
	}
	//---------------------------------------------------	

	// Applies splint on all limbs.
	void ApplySplint()
	{
		float add_health_coef = 0.33;
		// The idea is to slightly increase health of broken limb so the player is still limping. Using more splints will help but each time less. 100% recovery can be achieved only through long term healing.
		if (GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS)
		{
			AddHealth("LeftLeg", 	"Health",	(GetMaxHealth("LeftLeg", "Health")  - GetHealth("LeftLeg", "Health") ) * add_health_coef 	);
			AddHealth("RightLeg",	"Health",	(GetMaxHealth("RightLeg", "Health") - GetHealth("RightLeg", "Health")) * add_health_coef 	);
			AddHealth("RightFoot",	"Health",	(GetMaxHealth("RightFoot", "Health") - GetHealth("RightFoot", "Health")) * add_health_coef 	);
			AddHealth("LeftFoot",	"Health",	(GetMaxHealth("LeftFoot", "Health") - GetHealth("LeftFoot", "Health")) * add_health_coef 	);
		}
	}
	
	void ProcessDrowning(float dT)
	{
		GetStaminaHandler().DepleteStamina(EStaminaModifiers.DROWN,dT);
		
		#ifndef SERVER
		//PP EFFECTS
		PPERequester_Drowning req = PPERequester_Drowning.Cast(PPERequesterBank.GetRequester(PPERequesterBank.REQ_DROWNING));
		req.SetStamina01(GetStaminaHandler().GetStaminaNormalized());
		#endif
	}
	
	void SpawnDrowningBubbles()
	{
		if (!GetDrowningWaterLevelCheck())
			return;
		float bubbleFrequency 	= Math.Lerp(PlayerConstants.DROWNING_BUBBLE_FREQUENCY_MIN, PlayerConstants.DROWNING_BUBBLE_FREQUENCY_MAX, GetStaminaHandler().GetSyncedStaminaNormalized());
		int boneIdx 			= GetBoneIndexByName("Head");
		
		if (boneIdx != -1)
		{
			Particle p = ParticleManager.GetInstance().PlayInWorld(ParticleList.DROWNING_BUBBLES, "-0.03 0.15 0");
			if (p)
				AddChild(p, boneIdx);
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SpawnDrowningBubbles, bubbleFrequency);
		}
	}
	
	void ProcessHoldBreath(float dT)
	{
		if (IsTryingHoldBreath() && CanStartConsumingStamina(EStaminaConsumers.HOLD_BREATH))
		{
			if (!m_IsHoldingBreath)
			{
				OnHoldBreathStart();
				m_IsHoldingBreath = true;
			}
		}
		else if (!IsTryingHoldBreath() || !CanConsumeStamina(EStaminaConsumers.HOLD_BREATH))
		{
			if (m_IsHoldingBreath) OnHoldBreathEnd();
			m_IsHoldingBreath = false;
		}
	}
	
	void OnHoldBreathStart()
	{
		//SendSoundEvent(SoundSetMap.GetSoundSetID("holdBreath_male_Char_SoundSet"));
		RequestSoundEventEx(EPlayerSoundEventID.HOLD_BREATH, true, EPlayerSoundEventParam.SKIP_CONTROLLED_PLAYER);
	}
	
	void OnHoldBreathExhausted()
	{
		RequestSoundEventEx(EPlayerSoundEventID.EXHAUSTED_BREATH, true, EPlayerSoundEventParam.SKIP_CONTROLLED_PLAYER);
	}
	
	void OnHoldBreathEnd()
	{
		//SendSoundEvent(SoundSetMap.GetSoundSetID("releaseBreath_male_Char_SoundSet"));
		RequestSoundEventEx(EPlayerSoundEventID.RELEASE_BREATH, true, EPlayerSoundEventParam.SKIP_CONTROLLED_PLAYER);
	}
	
	override bool IsHoldingBreath()
	{
		return m_IsHoldingBreath;
	}
	
	override bool IsRefresherSignalingViable()
	{
		return false;
	}
	
	override bool IsSelfAdjustingTemperature()
	{
		return IsAlive();
	}
	
	eMixedSoundStates GetMixedSoundStates()
	{
		return m_MixedSoundStates;
	}
	
	AbstractWave SaySoundSet(string name)
	{
		if (m_SaySoundLastSetName != name)
		{
			m_SaySoundParams = new SoundParams(name);
			m_SaySoundBuilder = new SoundObjectBuilder(m_SaySoundParams);

			m_SaySoundLastSetName = name;
		}
		
		m_SaySoundObject = m_SaySoundBuilder.BuildSoundObject();
		m_SaySoundObject.SetPosition(GetPosition());
		return GetGame().GetSoundScene().Play3D(m_SaySoundObject, m_SaySoundBuilder);
	}

	EntityAI FindCargoByBaseType(string searched_item)
	{
		EntityAI attachment;
		string item_name;
		int attcount = this.GetInventory().AttachmentCount();
		
		for (int att = 0; att < attcount; att++)
		{	
			attachment = GetInventory().GetAttachmentFromIndex(att);
			if (attachment.IsItemBase())
			{
				item_name = attachment.GetType();
				if (GetGame().IsKindOf(item_name, searched_item))
				{
					return attachment;
				}
			}
		}
		return NULL;
	}

	void InitEditor()
	{
		if (GetGame().IsDebug())
		{
			if (!GetGame().IsMultiplayer()  &&  GetGame().GetPlayer()  &&  GetGame().GetPlayer().GetID() == this.GetID())
			{
				PluginSceneManager scene_editor = PluginSceneManager.Cast(GetPlugin(PluginSceneManager));
				scene_editor.InitLoad();
			}
		}
	}
	
	//---------------------------------------------------------------------------------------------------------------------------
	void OnPlayerLoaded()
	{
		InitEditor();
		
		//! initialize EnvironmentExposure data; waits for PlayerStats load
		if (m_Environment)
				m_Environment.Init();
		
		if (GetGame().IsMultiplayer() || GetGame().IsServer())
		{
			if (m_ModuleLifespan)
			{
				m_ModuleLifespan.SynchLifespanVisual(this, m_LifeSpanState, m_HasBloodyHandsVisible, m_HasBloodTypeVisible, m_BloodType);
			}
		}
		
		if (IsControlledPlayer())//true only on client for the controlled character
		{
			if (!m_VirtualHud)
				m_VirtualHud = new VirtualHud(this);
			if (m_Hud)
			{
				m_Hud.UpdateBloodName();
				PPEManagerStatic.GetPPEManager().StopAllEffects(PPERequesterCategory.GAMEPLAY_EFFECTS);
				GetGame().GetUIManager().CloseAll();
				GetGame().GetMission().SetPlayerRespawning(false);
				GetGame().GetMission().OnPlayerRespawned(this);
				
				m_Hud.ShowQuickbarUI(true);
				m_Hud.UpdateQuickbarGlobalVisibility();
				m_Hud.InitHeatBufferUI(this);
				m_Hud.OnPlayerLoaded();
			}
			m_EffectWidgets = GetGame().GetMission().GetEffectWidgets();
		}
		if (!GetGame().IsDedicatedServer())
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(UpdateCorpseStateVisual, 2000, false);//sometimes it takes a while to load in
			m_PlayerSoundEventHandler = new PlayerSoundEventHandler(this);
			m_ReplaceSoundEventHandler = new ReplaceSoundEventHandler(this);
		}
		int slotId = InventorySlots.GetSlotIdFromString("Head");
		m_CharactersHead = Head_Default.Cast(GetInventory().FindPlaceholderForSlot(slotId));
		CheckHairClippingOnCharacterLoad();
		UpdateHairSelectionVisibility();
		PreloadDecayTexture();
		
		Weapon_Base wpn = Weapon_Base.Cast(GetItemInHands());
		if (wpn)
		{
			if (GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_REMOTE)
			{
				wpn.DelayedValidateAndRepair();
			}
			else
			{
				wpn.ValidateAndRepair();
			}
		}

		m_PlayerLoaded = true;
	}
	
	void SetPlayerDisconnected(bool state)
	{
		m_PlayerDisconnectProcessed = state;
	}
	
	bool IsPlayerDisconnected()
	{
		return m_PlayerDisconnectProcessed;
	}

	// --------------------------------------------------
	// Advanced placement
	//---------------------------------------------------
	
	//get hologram
	Hologram GetHologramServer()
	{
		return m_HologramServer;
	}
	
	Hologram GetHologramLocal()
	{
		return m_HologramLocal;
	}
	
	void PlacingStartServer(ItemBase item)
	{
		if (item)
		{
			m_HologramServer = new Hologram(this, GetLocalProjectionPosition(), item);
			GetHologramServer().SetProjectionPosition(GetLocalProjectionPosition());
			GetHologramServer().SetProjectionOrientation(GetLocalProjectionOrientation());
			GetHologramServer().GetProjectionEntity().OnPlacementStarted(this);
			GetHologramServer().CheckPowerSource();
			GetHologramServer().RefreshVisual();
		}
	}
	
	void PlacingStartLocal(ItemBase item)
	{
		if (GetGame().IsMultiplayer() && GetGame().IsServer())
			return;
		
		if (item)
		{
			m_HologramLocal = new Hologram(this, GetLocalProjectionPosition(), item);
			GetHologramLocal().GetProjectionEntity().OnPlacementStarted(this);
		}
	}
	
	void PlacingCancelServer()
	{
		EntityAI entityInHands = GetHumanInventory().GetEntityInHands();
		
		if (IsPlacingServer())
		{
			GetHologramServer().CheckPowerSource();
			GetHologramServer().GetParentEntity().OnPlacementCancelled(this);
			
			delete m_HologramServer;

			return;
		}
		else if (entityInHands && entityInHands.HasEnergyManager())
		{
			if (entityInHands.GetCompEM().IsPlugged())
				entityInHands.OnPlacementCancelled(this);
		}
	}
	
	//Called when item placement is canceled, will prevent quickbar usage until the item the player was placing is back in hands
	void LockHandsUntilItemHeld()
	{
		m_AreHandsLocked = true;
	}
	
	void PlacingCancelLocal()
	{
		EntityAI entityInHands = GetHumanInventory().GetEntityInHands();
		if (entityInHands && entityInHands.HasEnergyManager())
		{
			if (entityInHands.GetCompEM().IsPlugged())
				entityInHands.OnPlacementCancelled(this);
		}

		delete m_HologramLocal;
	}
	
	void PlacingCompleteServer()
	{
		SetLocalProjectionPosition(vector.Zero);
		delete m_HologramServer;	
	}
	
	void PlacingCompleteLocal()
	{
		SetLocalProjectionPosition(vector.Zero);
		delete m_HologramLocal;
	}
	
	bool IsPlacingServer()
	{
		if (m_HologramServer)
			return true;

		return false;
	}
	
	bool IsPlacingLocal()
	{
		if (m_HologramLocal)
			return true;
		
		return false;
	}
	
	void SetDrowning(bool enable)
	{
		if (enable != m_IsDrowning)
		{
			if (enable)
				OnDrowningStart();
			else
				OnDrowningEnd();
		}

		m_IsDrowning = enable;
	}
	
	void OnDrowningStart()
	{
		#ifndef SERVER
		if (IsControlledPlayer())
		{
			PPERequester_Drowning req = PPERequester_Drowning.Cast(PPERequesterBank.GetRequester(PPERequesterBank.REQ_DROWNING));
			req.Start();
		}
		//Particles
		SpawnDrowningBubbles();
		#endif
	}
	
	void OnDrowningEnd()
	{
		#ifndef SERVER
		if (IsControlledPlayer())
		{
			PPERequester_Drowning req = PPERequester_Drowning.Cast(PPERequesterBank.GetRequester(PPERequesterBank.REQ_DROWNING));
			req.Stop();
		}
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(SpawnDrowningBubbles);
		#endif
	}
	
	bool TogglePlacingServer(int userDataType, ParamsReadContext ctx)
	{	
		if (userDataType == INPUT_UDT_ADVANCED_PLACEMENT)
		{
			PlacingCancelServer();
			return true;
		}
		
		return false;
	}
	
	void RequestResetADSSync()
	{
		if (GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_CLIENT && GetGame().IsMultiplayer())
		{
			m_ProcessResetADS = true;
			ProcessADSSyncResetRequest();
		}
		else if (!GetGame().IsMultiplayer())
		{
			m_ResetADS = true;
		}
	}
	
	void ProcessADSSyncResetRequest()
	{
		if (ScriptInputUserData.CanStoreInputUserData())
		{
			ScriptInputUserData ctx = new ScriptInputUserData;
			ctx.Write(INPUT_UDT_RESET_ADS);
			ctx.Send();
			
			m_ProcessResetADS = false;
		}
	}
	
	//! server only
	bool ResetADSPlayerSync(int userDataType, ParamsReadContext ctx)
	{
		if (userDataType == INPUT_UDT_RESET_ADS)
		{
			ScriptJunctureData pCtx = new ScriptJunctureData;
			SendSyncJuncture(DayZPlayerSyncJunctures.SJ_ADS_RESET,pCtx);
			return true;
		}
		
		return false;	
	}
	
	override bool CanPlaceItem(EntityAI item)
	{
		if (m_UndergroundPresence > EUndergroundPresence.NONE)
		{
			TStringSet disallowedUndergroundTypes = CfgGameplayHandler.GetDisallowedTypesInUnderground();
			foreach (string t: disallowedUndergroundTypes)
			{
				if (item.IsKindOf(t))
					return false;
			}
		}
		return true;
	}
	
	void SetUnderground(EUndergroundPresence presence)
	{
		m_UndergroundPresence = presence;
	}
	
	void TogglePlacingLocal(ItemBase item = null)
	{	
		if (IsPlacingLocal())
		{
			if (GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_CLIENT && GetGame().IsMultiplayer())
			{
				if (ScriptInputUserData.CanStoreInputUserData())
				{
					ScriptInputUserData ctx = new ScriptInputUserData;
					ctx.Write(INPUT_UDT_ADVANCED_PLACEMENT);
					ctx.Send();
					
					PlacingCancelLocal();
				}
			}
		}
		else if (!item)
		{
			PlacingStartLocal(GetItemInHands());
		}
		else
		{
			PlacingStartLocal(item);
		}
	}

	void SetLocalProjectionPosition(vector local_position)
	{
		m_LocalProjectionPosition = local_position;
		
		#ifdef DEVELOPER
		if (IsCLIParam("hologramLogs"))
		{
			Debug.Log(string.Format("SetLocalProjectionPosition | pos: %1", m_LocalProjectionPosition), "hologramLogs");
		}
		#endif
	}
	
	void SetLocalProjectionOrientation(vector local_orientation)
	{
		m_LocalProjectionOrientation = local_orientation;
	}
	
	vector GetLocalProjectionPosition()
	{
		return m_LocalProjectionPosition;
	}
	
	vector GetLocalProjectionOrientation()
	{
		return m_LocalProjectionOrientation;
	}
	
	void SetInColdArea(bool state)
	{
		if (m_IsInColdArea != state)
		{
			m_IsInColdArea = state;
			SetSynchDirty();
		}
	}
	
	bool GetInColdArea()
	{
		return m_IsInColdArea;
	}
	
	// ------------------------------------------------------------------------

	//! water contact (driven by Environment)
	void SetInWater(bool pState)
	{
		m_IsInWater = pState;
		SetSynchDirty();
	}

	bool IsInWater()
	{
		return m_IsInWater;
	}

	// -------------------------------------------------------------------------
	void ~PlayerBase()
	{
		if (GetGame() && (!GetGame().IsDedicatedServer()))
		{
			ClientData.RemovePlayerBase(this);
			SetContaminatedEffectEx(false);
		}
		
		SEffectManager.DestroyEffect(m_FliesEff);
		StopSoundSet(m_SoundFliesEffect);
	}

	void OnCameraChanged(DayZPlayerCameraBase new_camera)
	{			
		m_CameraSwayModifier = new_camera.GetWeaponSwayModifier();
		m_CurrentCamera = new_camera;
	}
	
	DayZPlayerCamera GetCurrentPlayerCamera()
	{
		return m_CurrentCamera;
	}
	
	bool IsCurrentCameraAimedAtGround()
	{
		if (!m_CurrentCamera)
			return false;
		
		return m_CurrentCamera.GetCurrentPitch() < PlayerConstants.CAMERA_THRESHOLD_PITCH;
	} 
	
	BleedingSourcesManagerServer GetBleedingManagerServer()
	{
		return m_BleedingManagerServer;
	}
	
	BleedingSourcesManagerRemote GetBleedingManagerRemote()
	{
		return m_BleedingManagerRemote;
	}

	SymptomManager GetSymptomManager()
	{
		return m_SymptomManager;
	}
	VirtualHud GetVirtualHud()
	{
		if (!m_VirtualHud)
		{
			m_VirtualHud = new VirtualHud(this);
		}
		return m_VirtualHud;
	}

	TransferValues GetTransferValues()
	{
		return m_TrasferValues;
	}

	DebugMonitorValues GetDebugMonitorValues()
	{
		return m_DebugMonitorValues;
	}
	
	//--------------------------------------------------------------------------
	void OnScheduledTick(float deltaTime)
	{
		if (!IsPlayerSelected() || !IsAlive()) 
			return;
		if (m_ModifiersManager)
			m_ModifiersManager.OnScheduledTick(deltaTime);
		if (m_NotifiersManager) 
			m_NotifiersManager.OnScheduledTick();
		if (m_TrasferValues) 
			m_TrasferValues.OnScheduledTick(deltaTime);
		if (m_VirtualHud) 
			m_VirtualHud.OnScheduledTick();
		if (GetBleedingManagerServer()) 
			GetBleedingManagerServer().OnTick(deltaTime);
		if (m_Environment)
			m_Environment.Update(deltaTime);

		// Check if electric device needs to be unplugged
		ItemBase heldItem = GetItemInHands();
		if (heldItem && heldItem.HasEnergyManager() && heldItem.GetCompEM().IsPlugged())
		{
			// Now we know we are working with an electric device which is plugged into a power source.
			EntityAI placed_entity = heldItem;
					
			// Unplug the device when the player is too far from the power source.
			placed_entity.GetCompEM().UpdatePlugState();
		}
	}
	
	void OnCommandHandlerTick(float delta_time, int pCurrentCommandID)
	{
		if (!IsAlive())
		{
			if (!m_DeathSyncSent && m_KillerData)
			{
				SyncEvents.SendEntityKilled(this, m_KillerData.m_Killer, m_KillerData.m_MurderWeapon, m_KillerData.m_KillerHiTheBrain);
				m_DeathSyncSent = true;
			}
			return;
		}
		if (m_DebugMonitorValues)
			m_DebugMonitorValues.OnScheduledTick(delta_time);
		if (GetSymptomManager())
			GetSymptomManager().OnTick(delta_time, pCurrentCommandID, m_MovementState);//needs to stay in command handler tick as it's playing animations
		//if (GetBleedingManagerServer()) GetBleedingManagerServer().OnTick(delta_time);
		
		DayZPlayerInstanceType instType = GetInstanceType();		
		if (instType == DayZPlayerInstanceType.INSTANCETYPE_CLIENT)
		{ 
			if (m_PlayerSoundEventHandler)
				m_PlayerSoundEventHandler.OnTick(delta_time);
				
			if (m_ProcessRemoveEffectWidgets && m_ProcessRemoveEffectWidgets.Count() > 0)
			{
				m_EffectWidgets.RemoveActiveEffects(m_ProcessRemoveEffectWidgets);
			}
			
			if (m_ProcessAddEffectWidgets && m_ProcessAddEffectWidgets.Count() > 0)
			{
				m_EffectWidgets.AddActiveEffects(m_ProcessAddEffectWidgets);
			}
			
			HandleGlassesEffect();
			
			if (m_ContaminatedAreaEffectEnabled)
				ContaminatedParticleAdjustment();
			
			#ifdef DIAG_DEVELOPER
			if (m_WeaponDebug)
			{
				m_WeaponDebug.OnCommandHandlerUpdate();
			}
			#endif
	
			m_ProcessAddEffectWidgets.Clear(); //clears array for remotes as well
			m_ProcessRemoveEffectWidgets.Clear(); //clears array for remotes as well
		}
		
		m_AnimCommandStarting = HumanMoveCommandID.None;

		if (CfgGameplayHandler.GetAllowStaminaAffectInertia())
			UpdateMovementInertia();
		
		if (m_ProcessResetADS)
			ProcessADSSyncResetRequest();
	}
	
	//! Update movement inertia based on stamina available
	void UpdateMovementInertia()
	{
		HumanCommandMove hcm = GetCommand_Move();
		if (hcm)
		{
			float staminaPercentage = GetStaminaHandler().GetSyncedStaminaNormalized();
			hcm.SetTurnSpanSprintModifier(2 - staminaPercentage);
			hcm.SetRunSprintFilterModifier((2 - staminaPercentage) * 0.5);
			hcm.SetDirectionSprintFilterModifier(2 - staminaPercentage);				
		}
	}
	
	bool m_ShowDbgUI = true;
	int m_DbgListSelection = 0;
	float m_DbgSliderValue = 0.0;
	ref array<string> m_DbgOptions = {"jedna", "dva", "tri"};
	
	NotifiersManager GetNotifiersManager()
	{
		return m_NotifiersManager;
	}

	//--------------------------------------------------------------------------
	void OnTick()
	{
		float deltaT = (GetGame().GetTime() - m_LastTick) * 0.001;
		if (m_LastTick < 0)
			deltaT = 0; //first tick protection

		m_LastTick = GetGame().GetTime();
		
		OnScheduledTick(deltaT);				
	}

	// -------------------------------------------------------------------------
	override void EEItemIntoHands(EntityAI item)
	{
		super.EEItemIntoHands(item);

		if (item)
		{
			MiscGameplayFunctions.RemoveAllAttachedChildrenByTypename(item, {Bolt_Base});

			Weapon_Base w;
			if (Class.CastTo(w, item))
			{
				w.ResetWeaponAnimState();
				
				HumanCommandMove cm = GetCommand_Move();
				if (cm)
				{
					cm.SetMeleeBlock(false);
					GetMeleeFightLogic().SetBlock(false);
				}
			}

			//! fixes situation where raise is canceling some manipulation with heavy item(ex. TakeItemToHands), forces normal stance
			if (item.IsHeavyBehaviour() && IsRaised())
			{
				HumanCommandMove cm2 = GetCommand_Move();
				if (cm2)
				{
					cm2.ForceStance(DayZPlayerConstants.STANCEIDX_ERECT);
				}
			}
			
			SurrenderDummyItem dummyItem;
			if (Class.CastTo(dummyItem, item) && GetEmoteManager())
				GetEmoteManager().ForceSurrenderState(true);
			
			OnItemInHandsChanged();
		}
	}
	
	override void EEItemOutOfHands(EntityAI item)
	{
		super.EEItemOutOfHands(item);

		if (IsPlacingLocal())
		{
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Call(TogglePlacingLocal);
		}
		
		SurrenderDummyItem dummyItem;
		if (Class.CastTo(dummyItem, item) && GetEmoteManager())
			GetEmoteManager().ForceSurrenderState(false);
		
		OnItemInHandsChanged();
	}
	
	PlayerStomach GetStomach()
	{
		return m_PlayerStomach;
	}
	
	override void CommandHandler(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)	
	{
		EvaluateDamageHit(pCurrentCommandID);

		// lower implement 
		super.CommandHandler(pDt,pCurrentCommandID,pCurrentCommandFinished);

		vector playerPosition = PhysicsGetPositionWS();

		HumanInputController hic = GetInputController();
		
		CheckZeroSoundEvent();
		CheckSendSoundEvent();
		
		ProcessADDModifier();

		if (m_BrokenLegsJunctureReceived)//was there a change in broken legs state ?
		{
			m_BrokenLegsJunctureReceived = false;
			bool initial = m_BrokenLegState < 0;//negative values indicate initial activation

			if (GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS)
			{
				DropHeavyItem();
				if (initial)
				{
					BrokenLegForceProne();
					hic.ResetADS();
					GetUApi().GetInputByID(UATempRaiseWeapon).Supress();
					ExitSights();
				}
			}
		}
		
		if (IsFireWeaponRaised() || m_IsHoldingBreath)
		{
			ProcessHoldBreath(pDt);
		}

		ActionManagerBase		mngr = GetActionManager();
		
		if (m_AreHandsLocked && GetHumanInventory().GetEntityInHands())
		{
			m_AreHandsLocked = false;
		}
		
		// freelook camera memory for weapon raycast
		if (hic.CameraIsFreeLook() && m_DirectionToCursor == vector.Zero)
		{
			m_DirectionToCursor = GetGame().GetCurrentCameraDirection();
		}
		else if (!hic.CameraIsFreeLook() && m_DirectionToCursor != vector.Zero)
		{
			m_DirectionToCursor = vector.Zero;
		}
		
		if (m_WeaponManager)
		{
			m_WeaponManager.Update(pDt);
		}
		if (m_EmoteManager && IsPlayerSelected()) 
		{
			m_EmoteManager.Update(pDt);	
		}
		if (m_RGSManager)
		{
			 m_RGSManager.Update();
		}
		if (m_StanceIndicator) 
		{
			m_StanceIndicator.Update();
		}
		if (m_StaminaHandler) 
		{
			m_StaminaHandler.Update(pDt, pCurrentCommandID);
		}
		if (m_InjuryHandler)
		{
			m_InjuryHandler.Update(pDt);
		}
		if (m_HCAnimHandler)
		{
			m_HCAnimHandler.Update(pDt, m_MovementState);
		}
		if (m_ShockHandler)
		{
			m_ShockHandler.Update(pDt);
		}
			
		if (GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_SERVER || !GetGame().IsMultiplayer())
		{
			GetPlayerSoundManagerServer().Update();
			ShockRefill(pDt);
			FreezeCheck();
		}
		
		if (m_IsDrowning)
		{
			ProcessDrowning(pDt);
		}
		UpdateDelete();
		
		HandleDamageHit(pCurrentCommandID);
		
		if (mngr && hic)
		{
			mngr.Update(pCurrentCommandID);

			HumanCommandUnconscious		hcu = GetCommand_Unconscious();
			HumanCommandVehicle			hcv = GetCommand_Vehicle();

			if (!m_UnconsciousDebug)
			{
				//! When the player should be and is unconscious 
				if (m_ShouldBeUnconscious && m_IsUnconscious)
				{
					if (hcu)
					{
						//! Player can start floating while unconscious
						m_Swimming.m_bWasSwimming |= hcu.IsInWater();
					}

					if (m_Swimming.m_bWasSwimming)
					{
						m_LastCommandBeforeUnconscious = DayZPlayerConstants.COMMANDID_SWIM;
					}

					OnUnconsciousUpdate(pDt, m_LastCommandBeforeUnconscious);
				}
				//! When the player will be unconscious, or is being blocked from being unconscious
				else if (m_ShouldBeUnconscious)
				{
					//! If the player is getting in/out or switching seats, delay unconsciousness until after animation has finished
					bool isTransitioning = hcv && (hcv.IsGettingIn() || hcv.IsGettingOut() || hcv.IsSwitchSeat());

					if (pCurrentCommandID == DayZPlayerConstants.COMMANDID_UNCONSCIOUS)
					{
						OnUnconsciousUpdate(pDt, m_LastCommandBeforeUnconscious);

						m_IsUnconscious = true;
						OnUnconsciousStart();
					}
					//! Death gate - prevent unlikely occurence of death -> unconsciousness transition
					//! Fall gate - prevent unconciousness animation from playing while falling, doesn't look good
					//! Transition gate - prevent unconciousness while previous command is transitioning
					else if (pCurrentCommandID != DayZPlayerConstants.COMMANDID_DEATH && pCurrentCommandID != DayZPlayerConstants.COMMANDID_FALL && !isTransitioning)
					{
						m_LastCommandBeforeUnconscious = pCurrentCommandID;

						if (hcv)
						{
							m_TransportCache = hcv.GetTransport();	
						}
						else
						{
							m_TransportCache = null;
						}
						
						//! TODO: rework vehicle command Knockout back to force player prone after they have exited the vehicle
						m_JumpClimb.CheckAndFinishJump();
						StartCommand_Unconscious(0);
						SetFallYDiff(playerPosition[1]);
					}
				}
				//! When the player is waking up
				else if (m_IsUnconscious)
				{
					//! Make sure the player is actually unconscious
					if (hcu && pCurrentCommandID == DayZPlayerConstants.COMMANDID_UNCONSCIOUS)
					{
						OnUnconsciousUpdate(pDt, m_LastCommandBeforeUnconscious);

						//! protection for a player being broken when attempting to wake them up too early into unconsciousness
						if (m_UnconsciousTime > 2)
						{
							int wakeUpStance = DayZPlayerConstants.STANCEIDX_PRONE;

							//! Don't set the stance if we are swimming or in a vehicle, stance change animation could play
							if (m_Swimming.m_bWasSwimming || m_LastCommandBeforeUnconscious == DayZPlayerConstants.COMMANDID_VEHICLE)
								wakeUpStance = -1;

							hcu.WakeUp(wakeUpStance);

							m_IsUnconscious = false;
							OnUnconsciousStop(pCurrentCommandID);
						}
					}
					else
					{
						//! Maybe instead error out or notify of possible desync?
						if (IsAlive())
						{
							m_IsUnconscious = false;
							OnUnconsciousStop(pCurrentCommandID);
						}
					}
				}
			}
			
			// quickbar use
			int quickBarSlot = hic.IsQuickBarSlot();
			if (quickBarSlot && IsAlive())
			{
				if (hic.IsQuickBarSingleUse())
				{
					OnQuickBarSingleUse(quickBarSlot);
					//Print("PlayerBase.c IsQuickBarSingleUse - slot: " + quickBarSlot.ToString());
				}
				if (hic.IsQuickBarContinuousUseStart() && ((!GetGame().IsDedicatedServer()) && !GetGame().GetUIManager().GetMenu()))
				{
					OnQuickBarContinuousUseStart(quickBarSlot);
					//Print("PlayerBase.c IsQuickBarContinuousUseStart - slot: " + quickBarSlot.ToString());
				}
				if (hic.IsQuickBarContinuousUseEnd() && ((!GetGame().IsDedicatedServer())))
				{
					OnQuickBarContinuousUseEnd(quickBarSlot);
					//Print("PlayerBase.c IsQuickBarContinuousUseEnd - slot: " + quickBarSlot.ToString());
				}
			}
			
			/*if ((pCurrentCommandID == DayZPlayerConstants.COMMANDID_ACTION || pCurrentCommandID == DayZPlayerConstants.COMMANDID_MOVE || pCurrentCommandID == DayZPlayerConstants.COMMANDID_LADDER || pCurrentCommandID == DayZPlayerConstants.COMMANDID_SWIM)) 
			{
				mngr.Update(); // checks for suitable action and sets it
			}*/
		}
			
		if (m_StaminaHandler && hic)
		{
			HumanCommandMove hcm = GetCommand_Move();		
			bool isSwimmingOrClimbing = GetCommand_Swim() || GetCommand_Climb() || GetCommand_Ladder();		
			bool isStaminaLimitAppliable = hcm || isSwimmingOrClimbing;	
			
			if (isStaminaLimitAppliable)
			{
				hic.LimitsDisableSprint(!(CanConsumeStamina(EStaminaConsumers.SPRINT) && CanSprint()));
			}
		}

		//map closing - feel free to move to different "update" if it does not belong here
		if (IsMapOpen())
		{
			if (!GetGame().IsDedicatedServer())
			{
				if (!CfgGameplayHandler.GetUse3DMap() && !GetGame().GetUIManager().IsMenuOpen(MENU_MAP))
				{
					CloseMapEx(false);
				}
				else if (CfgGameplayHandler.GetUse3DMap())
				{
					if (IsMapCallbackCancelInput())
					{
						CloseMapEx(true);
					}
					else if (IsMapCallbackEndInput())
					{
						CloseMapEx(false);
					}
				}
			}
		}
		
		
		#ifdef DIAG_DEVELOPER
		if (m_Bot)
			m_Bot.OnUpdate(pDt);
		#endif
		
		if (m_CheckMeleeItem && (!GetGame().IsDedicatedServer()))
		{
			CheckMeleeItemDamage(m_CheckMeleeItem);
			SetCheckMeleeItem();
		}
		
		#ifdef DEVELOPER
		TryGetInVehicleDebug();
		#endif
		
		OnCommandHandlerTick(pDt, pCurrentCommandID);
	}
	
	#ifdef DEVELOPER
	void SetGetInVehicleDebug(EntityAI ent)
	{
		m_GetInVehicleTransportDebug = Transport.Cast(ent);
	}
	
	void TryGetInVehicleDebug()
	{
		if (m_GetInVehicleTransportDebug)
		{
			HumanCommandVehicle vehCommand = StartCommand_Vehicle(m_GetInVehicleTransportDebug, 0, 0);
			if (vehCommand)
			{
				vehCommand.SetVehicleType(m_GetInVehicleTransportDebug.GetAnimInstance());
			}
			m_GetInVehicleTransportDebug = null;
		}
	}
	#endif
	
	//MAP handling
	//! DEPRECATED; terminates map animation callback and re-enables controls
	void CloseMap()
	{
		CloseMapEx(false);
	}
	
	void CloseMapEx(bool cancelled)
	{
		if (m_hac && !GetMapClosingSyncSent())
		{
			if (ScriptInputUserData.CanStoreInputUserData())
			{
				if (GetGame().IsMultiplayer() && GetGame().IsClient())
				{
					ActionManagerClient mngr_client;
					CastTo(mngr_client, GetActionManager());
					
					if (cancelled)
					{
						mngr_client.RequestInterruptAction();
					}
					else
					{
						mngr_client.RequestEndAction();
					}
					
					GetGame().GetMission().RemoveActiveInputExcludes({"map"});
					GetGame().GetMission().RemoveActiveInputRestriction(EInputRestrictors.MAP);
				}
				else if (!GetGame().IsMultiplayer())
				{
					int command_ID = DayZPlayerConstants.CMD_ACTIONINT_END;
					if (cancelled)
					{
						command_ID = DayZPlayerConstants.CMD_ACTIONINT_INTERRUPT;
					}
					m_hac.InternalCommand(command_ID);
				}
				SetMapClosingSyncSet(true);
			}
		}
		
		if (!GetGame().IsDedicatedServer())
		{
			if (GetGame().GetUIManager().IsMenuOpen(MENU_MAP))
			{
				GetGame().GetUIManager().FindMenu(MENU_MAP).Close();
				if (m_Hud)
				{
					m_Hud.ShowQuickbarPlayer(true);
				}
				
				GetGame().GetMission().RemoveActiveInputExcludes({"map"});
				GetGame().GetMission().RemoveActiveInputRestriction(EInputRestrictors.MAP);
			}
		}
	}
	
	void SetMapOpen(bool state)
	{
		m_MapOpen = state;
	}
	
	bool IsMapOpen()
	{
		return m_MapOpen;
	}
	
	void SetMapClosingSyncSet(bool state)
	{
		m_MapClosingSyncSent = state;
	}
	
	bool GetMapClosingSyncSent()
	{
		return m_MapClosingSyncSent;
	}
	
	bool IsMapCallbackCancelInput()
	{
		if (IsRaised())
		{
			return true;
		}
		
		return false;
	}
	
	bool IsMapCallbackEndInput()
	{
		if (GetUApi().GetInputByID(UADefaultAction).LocalPress() || GetUApi().GetInputByID(UAUIMenu).LocalPress() || GetUApi().GetInputByID(UAGear).LocalPress())
		{
			return true;
		}
		
		return false;
	}
	
	MapNavigationBehaviour GetMapNavigationBehaviour()
	{
		return m_MapNavigationBehaviour;
	}
	
	void MapNavigationItemInPossession(EntityAI item)
	{
		if (GetMapNavigationBehaviour())
		{
			GetMapNavigationBehaviour().OnItemInPlayerPossession(item);
		}
	}
	
	void MapNavigationItemNotInPossession(EntityAI item)
	{
		if (GetMapNavigationBehaviour())
		{
			GetMapNavigationBehaviour().OnItemNotInPlayerPossession(item);
		}
	}
	
	void SetCheckMeleeItem(ItemBase item = null)
	{
		m_CheckMeleeItem = item;
	}
	
	void SetShakesForced(int value)
	{
		m_ShakesForced = value;
	}
	
	void FreezeCheck()
	{
		int level;
		if (m_ShakesForced > 0)
		{
			level = m_ShakesForced;
		}
		else
		{
			float heat_comfort = GetStatHeatComfort().Get();
			if (heat_comfort <= PlayerConstants.THRESHOLD_HEAT_COMFORT_MINUS_WARNING)
			{
				float value = Math.InverseLerp(PlayerConstants.THRESHOLD_HEAT_COMFORT_MINUS_WARNING, PlayerConstants.THRESHOLD_HEAT_COMFORT_MINUS_CRITICAL,heat_comfort);
				level = Math.Lerp(1,7,value);
				level = Math.Clamp(value,1,7);
				/*
				value = Math.Clamp(value,0,1);
				level = Math.Round(value * SHAKE_LEVEL_MAX);//translate from normalized value to levels
				*/
			}
		}
		if (level != m_Shakes)
		{
			m_Shakes = level;
			SetSynchDirty();
		}
	}

	override bool IsLanded(int pCurrentCommandID)
	{
		if (super.IsLanded(pCurrentCommandID))
		{
			return true;
		}

		//! Handle fall damage for unconscious
		if (pCurrentCommandID == DayZPlayerConstants.COMMANDID_UNCONSCIOUS)
		{
			bool wasFalling = m_IsUnconsciousFalling;
			m_IsUnconsciousFalling = PhysicsIsFalling(false);
			return wasFalling && !m_IsUnconsciousFalling;
		}

		//! No fall damage for players currently being damaged, animation is temporary

		return false;
	}

	override bool OnLand(int pCurrentCommandID, FallDamageData fallDamageData)
	{
		if (super.OnLand(pCurrentCommandID, fallDamageData))
		{
			return true;
		}

		//! Nothing happens for unconscious
		//! Nothing happens for being damaged

		return false;
	}

	override bool IsAlreadyInFallingCommand(int pCurrentCommandID)
	{
		if (super.IsAlreadyInFallingCommand(pCurrentCommandID))
		{
			return true;
		}

		//! Don't switch to falling command if unconscious 
		//! Don't switch to falling command if being damaged
		return pCurrentCommandID == DayZPlayerConstants.COMMANDID_UNCONSCIOUS || pCurrentCommandID == DayZPlayerConstants.COMMANDID_DAMAGE;
	}
	
	void OnUnconsciousStart()
	{
		CloseInventoryMenu();
		
		if (GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_CLIENT)
		{
			GetGame().GetMission().GetHud().ShowHudUI(false);
			SetInventorySoftLock(true);
			if (m_LastCommandBeforeUnconscious != DayZPlayerConstants.COMMANDID_VEHICLE)
			{
				EntityAI entity_in_hands = GetHumanInventory().GetEntityInHands();
				if (entity_in_hands && CanDropEntity(entity_in_hands) && !IsRestrained() && !IsSurrendered())
				{
					DropItem(ItemBase.Cast(entity_in_hands));
				}
			}
			GetGame().GetSoundScene().SetSoundVolume(0,2);
			m_EffectWidgets.AddSuspendRequest(EffectWidgetSuspends.UNCON);
		}
		
		if (GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_SERVER || (!GetGame().IsMultiplayer() && GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_CLIENT))
		{
			SetSynchDirty();
			
			if (m_LastCommandBeforeUnconscious == DayZPlayerConstants.COMMANDID_VEHICLE)
			{
				if (m_TransportCache)
					m_TransportCache.MarkCrewMemberUnconscious(m_TransportCache.CrewMemberIndex(this));
			}
			 
			// disable voice communication
			GetGame().EnableVoN(this, false);
			
			if (m_AdminLog)
			{
				m_AdminLog.UnconStart(this);
			}
			
			// When we fall uncon we force out of block
			if (GetMeleeFightLogic())
			{
				GetMeleeFightLogic().SetBlock(false);
			}
		}
		
		SetMasterAttenuation("UnconsciousAttenuation");
		
		if (m_OnUnconsciousStart)
			m_OnUnconsciousStart.Invoke(this);
	}
	
	void OnUnconsciousStop(int pCurrentCommandID)
	{	
		m_UnconRefillModifier =1;
		SetSynchDirty();
		m_UnconsciousTime = 0;
		m_UnconsciousVignetteTarget = 2;
		if (GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_CLIENT) 
		{
			GetGame().GetMission().GetHud().ShowHudUI(true);
			if (pCurrentCommandID != DayZPlayerConstants.COMMANDID_DEATH)
			{
				GetGame().GetSoundScene().SetSoundVolume(g_Game.m_volume_sound,1);
				PPERequester_UnconEffects requester = PPERequester_UnconEffects.Cast(PPERequesterBank.GetRequester(PPERequester_UnconEffects));
				requester.FadeOutEffect(); //fading
				GetGame().GetMission().GetHud().ShowQuickbarUI(true);
				if (GetGame().GetUIManager().IsDialogVisible())
				{
					GetGame().GetUIManager().CloseDialog();
				}
				if (GetGame().GetUIManager().IsMenuOpen(MENU_RESPAWN_DIALOGUE))
				{
					GetGame().GetUIManager().FindMenu(MENU_RESPAWN_DIALOGUE).Close();
				}
			}
			SetInventorySoftLock(false);
			m_EffectWidgets.RemoveSuspendRequest(EffectWidgetSuspends.UNCON);
		}
		if (GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_SERVER)
		{
			// enable voice communication
			if (IsAlive())
				GetGame().EnableVoN(this, true);
			
			if (m_AdminLog)
			{
				m_AdminLog.UnconStop(this);
			}
		}
		
		SetMasterAttenuation("");
		
		if (m_OnUnconsciousStop)
			m_OnUnconsciousStop.Invoke(this);
	}

	void OnUnconsciousUpdate(float pDt, int last_command)
	{
		m_UnconsciousTime += pDt;
		if (GetGame().IsServer())
		{
			int shock_simplified = SimplifyShock();
			
			if (m_ShockSimplified != shock_simplified)
			{
				m_ShockSimplified = shock_simplified;
				SetSynchDirty();
			}

			if (m_UnconsciousTime > PlayerConstants.UNCONSCIOUS_IN_WATER_TIME_LIMIT_TO_DEATH && last_command == DayZPlayerConstants.COMMANDID_SWIM)
			{
				PluginAdminLog adm = PluginAdminLog.Cast(GetPlugin(PluginAdminLog));
				adm.PlayerKilledByDrowningUncon(this);

				SetHealth("","",-100);
			}
		}
		if (GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_CLIENT)
		{
			GetGame().GetMission().GetHud().ShowQuickbarUI(false);
			if (GetPulseType() == EPulseType.REGULAR)
			{
				float shock_simple_normalized = GetSimplifiedShockNormalized();
	
				float sin = Math.Sin(m_UnconsciousTime * 0.35);
				float sin_normalized = (sin + 1) / 2;
				if (sin_normalized < 0.05)
				{
					m_UnconsciousVignetteTarget = (1 - shock_simple_normalized / 3) * 2/*vignette max*/;
				}
				float vignette = Math.Lerp(2,m_UnconsciousVignetteTarget, sin_normalized);
				m_UnconParam.param1 = vignette;
				PPERequesterBank.GetRequester(PPERequester_UnconEffects).Start(m_UnconParam);
			}
		}
	}
	
	int SimplifyShock()
	{
		int shock = Math.Lerp(0, SIMPLIFIED_SHOCK_CAP, GetHealth("","Shock") / GetMaxHealth("","Shock"));
		shock = Math.Clamp(shock, 0, SIMPLIFIED_SHOCK_CAP);
		return shock;
	}
	
	float GetSimplifiedShockNormalized()
	{
		return (m_ShockSimplified / SIMPLIFIED_SHOCK_CAP);
	}
	
   	override bool IsUnconscious()
	{
		return m_MovementState.m_CommandTypeId == DayZPlayerConstants.COMMANDID_UNCONSCIOUS || m_IsUnconscious;
	}
	
	override bool IsUnconsciousStateOnly()
	{
		return m_IsUnconscious;
	}
	
	override bool CanBeTargetedByAI(EntityAI ai)
	{
		#ifdef DIAG_DEVELOPER
		if (!m_CanBeTargetedDebug)
		{
			return false;
		}
		#endif
		
		return super.CanBeTargetedByAI(ai) && !IsUnconscious() && !IsInVehicle();
	}
	
	void GiveShock(float shock)
	{
		AddHealth("","Shock",shock);
	}
	
	
	void OnRestrainStart()
	{
		CloseInventoryMenu();
		GetGame().GetMission().RemoveActiveInputExcludes({"inventory"},true);
		GetGame().GetMission().RemoveActiveInputRestriction(EInputRestrictors.INVENTORY);
	}
	
	//! functionality moved to ShockMdfr::OnTick
	void ShockRefill(float pDt);
	
	//BrokenLegs
	// -----------------------

	eBrokenLegs GetBrokenLegs()
	{
		return Math.AbsInt(m_BrokenLegState);//negative value denotes first time activation
	}
	
	//Server side
	void SetBrokenLegs(int stateId)
	{
		m_BrokenLegState = stateId;
		
		DayZPlayerSyncJunctures.SendBrokenLegsEx(this, stateId);
		eBrokenLegs state = GetBrokenLegs();//m_BrokenLegState can go bellow 0, cannot be used directly
		
		if (state == eBrokenLegs.NO_BROKEN_LEGS)
		{
			m_InjuryHandler.m_ForceInjuryAnimMask = m_InjuryHandler.m_ForceInjuryAnimMask & ~eInjuryOverrides.BROKEN_LEGS;
			m_InjuryHandler.m_ForceInjuryAnimMask = m_InjuryHandler.m_ForceInjuryAnimMask & ~eInjuryOverrides.BROKEN_LEGS_SPLINT;
		}
		else if (state == eBrokenLegs.BROKEN_LEGS)
		{
			SetLegHealth();
		}
		else if (state == eBrokenLegs.BROKEN_LEGS_SPLINT)
		{
			// handle splint here
		}

		SetSynchDirty();
	}

	//Update of state
	void UpdateBrokenLegs(int stateId)
	{
		eBrokenLegs state = GetBrokenLegs();
		//Raise broken legs flag and force to prone
		if (state != eBrokenLegs.NO_BROKEN_LEGS)
		{
			if (state == eBrokenLegs.BROKEN_LEGS_SPLINT)
			{
				if (m_MovementState.m_iStanceIdx != DayZPlayerConstants.STANCEIDX_PRONE)
				{
					m_InjuryHandler.m_ForceInjuryAnimMask = m_InjuryHandler.m_ForceInjuryAnimMask & ~eInjuryOverrides.PRONE_ANIM_OVERRIDE;
					m_InjuryHandler.m_ForceInjuryAnimMask = m_InjuryHandler.m_ForceInjuryAnimMask & ~eInjuryOverrides.BROKEN_LEGS;
					m_InjuryHandler.m_ForceInjuryAnimMask = m_InjuryHandler.m_ForceInjuryAnimMask | eInjuryOverrides.BROKEN_LEGS_SPLINT;
				}
				m_InjuryHandler.CheckValue(false);

			}
			else if (state == eBrokenLegs.BROKEN_LEGS)
			{

				if (m_MovementState.m_iStanceIdx != DayZPlayerConstants.STANCEIDX_PRONE)
				{
					m_InjuryHandler.m_ForceInjuryAnimMask = m_InjuryHandler.m_ForceInjuryAnimMask & ~eInjuryOverrides.PRONE_ANIM_OVERRIDE;
					m_InjuryHandler.m_ForceInjuryAnimMask = m_InjuryHandler.m_ForceInjuryAnimMask & ~eInjuryOverrides.BROKEN_LEGS_SPLINT;
					m_InjuryHandler.m_ForceInjuryAnimMask = m_InjuryHandler.m_ForceInjuryAnimMask | eInjuryOverrides.BROKEN_LEGS;
				}
				
				BrokenLegWalkShock();
				m_InjuryHandler.CheckValue(false);

			}
		}
		else if (state == eBrokenLegs.NO_BROKEN_LEGS)
		{
			m_InjuryHandler.m_ForceInjuryAnimMask = m_InjuryHandler.m_ForceInjuryAnimMask & ~eInjuryOverrides.BROKEN_LEGS;
			m_InjuryHandler.m_ForceInjuryAnimMask = m_InjuryHandler.m_ForceInjuryAnimMask & ~eInjuryOverrides.BROKEN_LEGS_SPLINT;
		}
		
	}
	
	void BreakLegSound()
	{
		PlaySoundSet(m_BrokenLegSound, SOUND_BREAK_LEG, 0.1, 0.1);
	}
	void BrokenLegForceProne(bool forceOverride = false)
	{
		if (!IsInWater() && !IsSwimming() && !IsClimbingLadder() && !IsInVehicle() && !IsClimbing() && DayZPlayerUtils.PlayerCanChangeStance(this, DayZPlayerConstants.STANCEIDX_PRONE))
		{
			if (m_MovementState.m_iStanceIdx != DayZPlayerConstants.STANCEIDX_PRONE && m_MovementState.m_iStanceIdx != DayZPlayerConstants.STANCEIDX_RAISEDPRONE)
			{
				EntityAI attachment;
				Class.CastTo(attachment, GetItemOnSlot("Splint_Right"));
				if (attachment && attachment.GetType() == "Splint_Applied")
				{
					attachment.Delete();
				}
				
				m_ShockHandler.SetShock(PlayerConstants.BROKEN_LEGS_INITIAL_SHOCK);
				m_ShockHandler.CheckValue(true);
				
				
				if (m_ShockHandler.GetCurrentShock() >= 25) //Prevent conflict with unconsciousness by not forcing prone when going uncon (25 shock or less left)
				{
					
					//calcels user action
					HumanCommandActionCallback cmd = GetCommand_Action();
					if (cmd)
					{
						cmd.Cancel();
					}
					
					StopHandEvent();
					//Get command move and verify not null
					HumanCommandMove hcm = StartCommand_Move();//not sure why this is here
					hcm = GetCommand_Move(); 
					if (hcm)
					{
						hcm.ForceStance(DayZPlayerConstants.STANCEIDX_PRONE);
					}
				}
			}
		}

		m_JumpClimb.CheckAndFinishJump();
	}
	
	//Used to inflict shock when player is walking (only inflicted on Update timer)
	void BrokenLegWalkShock()
	{
		//No need to pursue here if player is prone as the following logic is not applied
		if (m_MovementState.m_iStanceIdx != DayZPlayerConstants.STANCEIDX_PRONE && m_MovementState.m_iStanceIdx != DayZPlayerConstants.STANCEIDX_RAISEDPRONE)
		{
			float avgLegHealth = GetHealth("RightLeg","") + GetHealth("LeftLeg","") + GetHealth("RightFoot","") + GetHealth("LeftFoot","");
			avgLegHealth *= 0.25; //divide by 4 to make the average leg health;
			
			if (IsSwimming())
			{
				//swimming other than forward is not detectable in script other than through velocity check
				vector v;
				PhysicsGetVelocity(v);
				
				if (v.LengthSq() > 0)
				{
					m_ShockHandler.SetShock(PlayerConstants.BROKEN_LEGS_SHOCK_SWIM);
				}
			}
			else if (m_MovementState.m_iMovement != 0)
			{
				if (IsClimbingLadder())
				{
					MovingShock(avgLegHealth, PlayerConstants.BROKEN_LEGS_HIGH_SHOCK_WALK, PlayerConstants.BROKEN_LEGS_MID_SHOCK_WALK, PlayerConstants.BROKEN_LEGS_LOW_SHOCK_WALK);
				}
				else if (m_MovementState.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_ERECT || m_MovementState.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_RAISEDERECT)
				{
					if (m_MovementState.m_iMovement > 1)//only jog and faster
					{
						MovingShock(avgLegHealth, PlayerConstants.BROKEN_LEGS_HIGH_SHOCK_WALK, PlayerConstants.BROKEN_LEGS_MID_SHOCK_WALK, PlayerConstants.BROKEN_LEGS_LOW_SHOCK_WALK);
					}
				}
				else if (m_MovementState.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_CROUCH || m_MovementState.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_RAISEDCROUCH)
				{
					//any speed other than idle
					MovingShock(avgLegHealth, PlayerConstants.BROKEN_LEGS_HIGH_SHOCK_WALK * PlayerConstants.BROKEN_CROUCH_MODIFIER, PlayerConstants.BROKEN_LEGS_MID_SHOCK_WALK * PlayerConstants.BROKEN_CROUCH_MODIFIER, PlayerConstants.BROKEN_LEGS_LOW_SHOCK_WALK * PlayerConstants.BROKEN_CROUCH_MODIFIER);
				}
			}
			else 
			{
				//Here apply shock if player is standing or crouched and STANDING STILL
				m_ShockHandler.SetShock(PlayerConstants.BROKEN_LEGS_STAND_SHOCK);
			}
			
			m_ShockHandler.CheckValue(true);
		}
	}
	
	//Always use the same thresholds but allow passing of different shock values
	void MovingShock(float legHealth, float highShock, float midShock, float lowShock)
	{
		if (legHealth <= PlayerConstants.BROKEN_LEGS_LOW_HEALTH_THRESHOLD)
		{
			//Inflict "high shock"
			m_ShockHandler.SetShock(highShock);
		}
		else if (legHealth >= PlayerConstants.BROKEN_LEGS_HIGH_HEALTH_THRESHOLD)
		{
			//Inflict "low shock"
			m_ShockHandler.SetShock(lowShock);
		}
		else
		{
			//If neither high nore low, inflict "mid shock"
			m_ShockHandler.SetShock(midShock);
		}
	}
	
	void DealShock(float dmg)
	{
		Param1<float> damage = new Param1<float>(0);
		damage.param1 = dmg;
		GetGame().RPCSingleParam(this, ERPCs.RPC_SHOCK, damage, true, GetIdentity());
	}
	
	//Prevent player from picking up heavy items when legs are broken
	override bool CanPickupHeavyItem(notnull EntityAI item)
	{
		if (item.IsHeavyBehaviour() && (GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS || GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS_SPLINT))
			return false;

		return super.CanPickupHeavyItem(item);
	}
	
	//Set all leg zones' health to 0 in order to limit emergent behaviour and prevent confusion as to how broken legs really work
	void SetLegHealth()
	{
		SetHealth("RightLeg", "", 0);
		SetHealth("RightFoot", "", 0);
		SetHealth("LeftLeg", "", 0);
		SetHealth("LeftFoot", "", 0);
	}
	
	void DropHeavyItem()
	{
		ItemBase itemInHands = GetItemInHands();
		if (itemInHands && itemInHands.IsHeavyBehaviour())
			DropItem(itemInHands);
	}
	
	bool IsWearingSplint()
	{
		EntityAI attachment;
		Class.CastTo(attachment, GetItemOnSlot("Splint_Right"));
		if (attachment && attachment.GetType() == "Splint_Applied")
		{
			return true;
		}
		return false;		
	}
	
	// -----------------------
	
		
	override void OnCommandSwimStart()
	{
		ItemBase itemInHands = GetItemInHands();
		if (itemInHands)
		{
			if (itemInHands.IsHeavyBehaviour())
			{
				TryHideItemInHands(false);
				#ifdef SERVER
				ServerDropEntity(itemInHands); // Let server handle the hand item drop
				#endif
			}
			else
			{
				TryHideItemInHands(true);
			}
			itemInHands.OnItemInHandsPlayerSwimStart(this);
		}
		m_AnimCommandStarting = HumanMoveCommandID.CommandSwim;
		
		if (GetInventory())
			GetInventory().LockInventory(LOCK_FROM_SCRIPT);
		
		CloseInventoryMenu();
		GetDayZGame().GetBacklit().OnSwimmingStart();
		
		AbortWeaponEvent();
		GetWeaponManager().DelayedRefreshAnimationState(10);
		RequestHandAnimationStateRefresh();
		
		GetGame().GetMission().AddActiveInputExcludes({"swimming"});
	}
	
	override void OnCommandSwimFinish()
	{
		TryHideItemInHands(false, true);
		
		if (GetInventory())
			GetInventory().UnlockInventory(LOCK_FROM_SCRIPT);
		
		GetDayZGame().GetBacklit().OnSwimmingStop();
		
		GetWeaponManager().RefreshAnimationState();
		
		GetGame().GetMission().RemoveActiveInputExcludes({"swimming"});
	}
	
	override void OnCommandLadderStart()
	{
		m_AnimCommandStarting = HumanMoveCommandID.CommandLadder;
		TryHideItemInHands(true);
		
		if (GetInventory())
			GetInventory().LockInventory(LOCK_FROM_SCRIPT);
		
		CloseInventoryMenu();
		
		GetGame().GetMission().AddActiveInputExcludes({"ladderclimbing"});
	}
	
	override void OnCommandLadderFinish()
	{
		TryHideItemInHands(false, true);
		
		if (GetInventory())
			GetInventory().UnlockInventory(LOCK_FROM_SCRIPT);
		
		GetGame().GetMission().RemoveActiveInputExcludes({"ladderclimbing"});
	}
	
	override void OnCommandFallStart()
	{
		m_AnimCommandStarting = HumanMoveCommandID.CommandFall;
		
		if (GetInventory())
			GetInventory().LockInventory(LOCK_FROM_SCRIPT);
		
		CloseInventoryMenu();
		
		AbortWeaponEvent();		
		GetWeaponManager().DelayedRefreshAnimationState(10);
		RequestHandAnimationStateRefresh();
	}
	
	override void OnCommandFallFinish()
	{
		if (GetInventory())
			GetInventory().UnlockInventory(LOCK_FROM_SCRIPT);
		
		// properly finish jump in case it was interrupted by smth (heavy hit for example in the FOV expoit)		
		m_JumpClimb.CheckAndFinishJump();
		
		GetWeaponManager().RefreshAnimationState();
	}
	
	override void OnCommandClimbStart()
	{
		m_AnimCommandStarting = HumanMoveCommandID.CommandClimb;
		
		if (GetInventory())
			GetInventory().LockInventory(LOCK_FROM_SCRIPT);
		
		CloseInventoryMenu();
		
		AbortWeaponEvent();
		GetWeaponManager().DelayedRefreshAnimationState(10);
		RequestHandAnimationStateRefresh();
	}
	
	override void OnCommandClimbFinish()
	{
		if (GetInventory())
			GetInventory().UnlockInventory(LOCK_FROM_SCRIPT);
		
		GetWeaponManager().RefreshAnimationState();
	}
	
	override void OnCommandVehicleStart()
	{
		m_AnimCommandStarting = HumanMoveCommandID.CommandVehicle;
		
		if (GetInventory())
			GetInventory().LockInventory(LOCK_FROM_SCRIPT);
		
		CloseInventoryMenu();
		
		ItemBase itemInHand = GetItemInHands();
		EntityAI itemOnHead = FindAttachmentBySlotName("Headgear");

		if (itemInHand && itemInHand.GetCompEM())
			itemInHand.GetCompEM().SwitchOff();

		TryHideItemInHands(true);

		if (itemOnHead && itemOnHead.GetCompEM())
			itemOnHead.GetCompEM().SwitchOff();
		
		GetGame().GetMission().AddActiveInputExcludes({"vehicledriving"});
	}
	
	override void OnCommandVehicleFinish()
	{
		if (GetInventory())
			GetInventory().UnlockInventory(LOCK_FROM_SCRIPT);
		
		if (!m_ShouldBeUnconscious)
			TryHideItemInHands(false, true);
		
		GetGame().GetMission().RemoveActiveInputExcludes({"vehicledriving"});
	}	
	
	override void OnCommandMelee2Start()
	{
		m_AnimCommandStarting = HumanMoveCommandID.CommandMelee2;
		m_IsFighting = true;
		
		AbortWeaponEvent();	
		GetWeaponManager().DelayedRefreshAnimationState(10);
		RequestHandAnimationStateRefresh();
	}
	
	override void OnCommandMelee2Finish()
	{
		RunFightBlendTimer();
		
		GetWeaponManager().RefreshAnimationState();
		
	}
	
	override void OnCommandDeathStart()
	{	
		m_AnimCommandStarting = HumanMoveCommandID.CommandDeath;

		AbortWeaponEvent();	
		GetWeaponManager().DelayedRefreshAnimationState(10);
		RequestHandAnimationStateRefresh();
		
		super.OnCommandDeathStart();
	}
	
	override void OnJumpStart()
	{
		m_ActionManager.OnJumpStart();
		
		AbortWeaponEvent();
		GetWeaponManager().DelayedRefreshAnimationState(10);
		RequestHandAnimationStateRefresh();
		CloseMapEx(true);
	}
	
	override void OnJumpEnd(int pLandType = 0)
	{
		if (m_PresenceNotifier)
		{
			switch (pLandType)
			{
				case HumanCommandFall.LANDTYPE_NONE:
				case HumanCommandFall.LANDTYPE_LIGHT:
					m_PresenceNotifier.ProcessEvent(EPresenceNotifierNoiseEventType.LAND_LIGHT);
					break;
				case HumanCommandFall.LANDTYPE_MEDIUM:
				case HumanCommandFall.LANDTYPE_HEAVY:
					m_PresenceNotifier.ProcessEvent(EPresenceNotifierNoiseEventType.LAND_HEAVY);
					break;
			}
		}
		
		GetWeaponManager().RefreshAnimationState();
	}
	
	bool IsStance(int stance, int stanceMask)
	{
		return ((1 << stance) & stanceMask) != 0;
	}
	
	override void OnStanceChange(int previousStance, int newStance)
	{
		int prone = DayZPlayerConstants.STANCEMASK_PRONE | DayZPlayerConstants.STANCEMASK_RAISEDPRONE;
		int notProne = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_RAISEDERECT | DayZPlayerConstants.STANCEMASK_RAISEDCROUCH;

		if (IsStance(previousStance, DayZPlayerConstants.STANCEMASK_PRONE) && IsStance(newStance, DayZPlayerConstants.STANCEMASK_ERECT))
			m_SprintedTimePerStanceMin = PlayerConstants.FULL_SPRINT_DELAY_FROM_PRONE;
		
		if (IsStance(previousStance, DayZPlayerConstants.STANCEMASK_PRONE) && IsStance(previousStance, DayZPlayerConstants.STANCEMASK_CROUCH) || (IsStance(previousStance, DayZPlayerConstants.STANCEMASK_CROUCH) && IsStance(newStance, DayZPlayerConstants.STANCEMASK_ERECT)))
			m_SprintedTimePerStanceMin = PlayerConstants.FULL_SPRINT_DELAY_FROM_CROUCH;
		
		if ((IsStance(previousStance, prone) && IsStance(newStance, notProne)) || (IsStance(previousStance, notProne) && IsStance(newStance, prone)))
		{
			AbortWeaponEvent();
			GetWeaponManager().RefreshAnimationState();
		}
	}
	
	override bool CanChangeStance(int previousStance, int newStance)
	{
		// Check if the player is trying to perform restricted action while changing stance
		if (GetActionManager() && GetActionManager().GetRunningAction() && !GetActionManager().GetRunningAction().CanBePerformedWhileChangingStance())
			return false;

		// Check if the player is playing a throwing animation
		if (GetThrowing().IsThrowingAnimationPlaying())
			return false;
		
		// don't allow base stance change, only raised hands change
		if (IsRolling()) 
		{
			if (Math.AbsInt(previousStance - newStance) == 3)
				return true;
			
			return false;
		}
	
	    // Check if the player is going to crouch or raised crouch
	    if (newStance == DayZPlayerConstants.STANCEIDX_CROUCH || newStance == DayZPlayerConstants.STANCEIDX_RAISEDCROUCH) 
	    {
	        return GetCurrentWaterLevel() <= GetDayZPlayerType().CommandSwimSettingsW().m_fToErectLevel;
	    }
	
	    // Check if the player is going to prone
	    if (newStance == DayZPlayerConstants.STANCEIDX_PRONE || newStance == DayZPlayerConstants.STANCEIDX_RAISEDPRONE) 
	    {
	        return GetCurrentWaterLevel() <= GetDayZPlayerType().CommandSwimSettingsW().m_fToCrouchLevel;
	    }
		
	    // The player can change stance if none of the conditions above are met
	    return true;
	}

	
	override void OnCommandMoveStart()
	{
		if (GetGame().IsServer())
		{
			//In case player changes stance through a different command, we refresh the anim overrides
			int prone = DayZPlayerConstants.STANCEMASK_PRONE | DayZPlayerConstants.STANCEMASK_RAISEDPRONE;
			if (!IsPlayerInStance(prone))
			{
				m_InjuryHandler.m_ForceInjuryAnimMask = m_InjuryHandler.m_ForceInjuryAnimMask & ~eInjuryOverrides.PRONE_ANIM_OVERRIDE;
				ForceUpdateInjuredState();
			}
		}
	}
	
	override bool CanRoll()
	{
		if (!CanConsumeStamina(EStaminaConsumers.ROLL)) 
			return false;
		
		if (IsInFBEmoteState() || m_EmoteManager.m_MenuEmote)
		{
			return false;
		}
		
		return IsPlayerInStance(DayZPlayerConstants.STANCEMASK_PRONE | DayZPlayerConstants.STANCEMASK_RAISEDPRONE) && GetCommand_Move() && !GetCommand_Move().IsOnBack();
	}
	
	override void OnRollStart(bool isToTheRight)
	{
		DepleteStamina(EStaminaModifiers.ROLL);
		
		if (GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS)
			m_ShockHandler.SetShock(PlayerConstants.BROKEN_LEGS_ROLL_SHOCK);
	}
	
	override void OnRollFinish()
	{
	}
	
	void OnJumpOutVehicleFinish(float carSpeed)
	{ 
		string surfaceType;
		int liquidType;
		GetGame().SurfaceUnderObject(this, surfaceType, liquidType);
	}
		
	void OnVehicleSwitchSeat(int seatIndex)
	{
	}
	
	override void OnVehicleSeatDriverEnter()
	{
		m_IsVehicleSeatDriver = true;
		if (m_Hud)
			m_Hud.ShowVehicleInfo();
		
#ifdef FEATURE_NETWORK_RECONCILIATION
		PlayerIdentity identity = GetIdentity();
		if (identity)
		{
			Pawn pawn = Pawn.Cast(GetParent());

			//! Hand off control to the vehicle - null is valid
			identity.Possess(pawn);
		}
#endif
	}
	
	override void OnVehicleSeatDriverLeft()
	{
#ifdef FEATURE_NETWORK_RECONCILIATION
		PlayerIdentity identity = GetIdentity();

		//! Don't possess the player if we are in the vehicle and unconscious, we are still the driver!
		if (identity && !m_ShouldBeUnconscious)
		{
			//! And now we want to resume control of the player 
			identity.Possess(this);
		}
#endif
		
		m_IsVehicleSeatDriver = false;
		if (m_Hud)
			m_Hud.HideVehicleInfo();
	}
	
	override void OnThrowingModeChange(bool change_to_enabled)
	{
		if (change_to_enabled)
		{
			PlacingCancelLocal();
			PlacingCancelServer();
		}
	}
	
	override void EOnFrame(IEntity other, float timeSlice)
	{
		if (GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_CLIENT)
		{
			#ifndef NO_GUI
			m_Hud.Update(timeSlice);
			
			if (IsControlledPlayer() && m_EffectWidgets && m_EffectWidgets.IsAnyEffectRunning())
			{
				m_EffectWidgets.Update(timeSlice);
			}
			#endif

			if (m_UndergroundHandler)
				m_UndergroundHandler.Tick(timeSlice);
			
			if (m_UndergroundBunkerHandler)
				m_UndergroundBunkerHandler.Tick(timeSlice);
		}
		
		if (m_InEffectAreaType != 0 && GetGame().IsServer())
			OnUpdateEffectAreaServer(timeSlice);
	}
	
	override void EOnPostFrame(IEntity other, int extra)
	{
		float delta_time = (GetGame().GetTime() - m_LastPostFrameTickTime) / 1000;
		m_LastPostFrameTickTime = GetGame().GetTime();
		if (GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_CLIENT)
		{
			if (GetDamageDealtEffect())
			{
				if (IsAlive())
					GetDamageDealtEffect().Update(delta_time);
				else
					delete GetDamageDealtEffect();
			}
			
			if (m_EffectRadial)
			{
				if (IsAlive())
				{
					m_EffectRadial.Update(delta_time);
				}
				else
					m_EffectRadial = null;
			}
				
			
			if (GetFlashbangEffect())
			{
				if (IsAlive())
				{
					GetFlashbangEffect().Update(delta_time);
				}
				else
				{
					GetFlashbangEffect().Stop();
					delete GetFlashbangEffect();
				}
			}
			
			if (GetShockEffect())
			{
				if (IsAlive())
					GetShockEffect().Update(delta_time);
				else
					delete GetShockEffect();
			}

			m_InventoryActionHandler.OnUpdate();
		}
			
		#ifdef DIAG_DEVELOPER			
		DiagOnPostFrame(other, extra);
		#endif
	}
	
#ifdef DIAG_DEVELOPER	
	protected void DiagOnPostFrame(IEntity other, int extra)
	{
		if (GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_CLIENT)
		{
			if (m_WeaponDebug)
			{
				m_WeaponDebug.OnPostFrameUpdate();
			}

			if (GetBleedingManagerRemote())
			{
				GetBleedingManagerRemote().OnUpdate();
			}
			
			if (m_MeleeCombat)
			{
				if (DiagMenu.GetBool(DiagMenuIDs.MELEE_DEBUG))
				{
					m_MeleeDebug = true;
					m_MeleeCombat.Debug(GetItemInHands(), m_MeleeCombat.GetHitType());
				}
				else if (!DiagMenu.GetBool(DiagMenuIDs.MELEE_DEBUG) && m_MeleeDebug)
				{
					m_MeleeDebug = false;
					m_MeleeCombat.Debug(GetItemInHands(), m_MeleeCombat.GetHitType());
				}
			}
			
			if (DiagMenu.GetBool(DiagMenuIDs.SOUNDS_ITEM_IMPACT_SOUNDS))
				InventoryItem.DrawImpacts();
			
			if (GetPluginManager())
			{
				PluginDrawCheckerboard drawCheckerboard = PluginDrawCheckerboard.Cast(GetPluginManager().GetPluginByType(PluginDrawCheckerboard));
				if (drawCheckerboard && !drawCheckerboard.IsActive())
				{
					drawCheckerboard.ShowWidgets(DiagMenu.GetBool(DiagMenuIDs.MISC_DRAW_CHECKERBOARD));
				}
			}
			
			if (m_PresenceNotifier)
			{
				m_PresenceNotifier.EnableDebug(DiagMenu.GetBool(DiagMenuIDs.MISC_PRESENCE_NOTIFIER_DBG));
			}
			
			if (m_TargetTemperatureDiag)
			{
				m_TargetTemperatureDiag.ShowDebug(DiagMenu.GetBool(DiagMenuIDs.MISC_TARGET_TEMPERATURE));
			}
			
			if (DiagMenu.GetBool(DiagMenuIDs.TRIGGER_PLAYER_DEBUG))
			{
				vector minmax[2];
				GetCollisionBox(minmax);
				
				int color = COLOR_RED_A;
				if (m_IsInsideTrigger)
					color = COLOR_GREEN_A;
				
				Shape dbgShape = Debug.DrawBoxEx(minmax[0], minmax[1], color, ShapeFlags.TRANSP|ShapeFlags.NOZWRITE|ShapeFlags.ONCE);
				
				vector mat[4];
				GetTransform(mat);
				dbgShape.CreateMatrix(mat);
				dbgShape.SetMatrix(mat);
			}
		}
		
		if (m_Environment && m_Environment.m_Debug)
		{
			#ifdef SERVER
			EnvDebugData envDebugData = m_Environment.GetEnvDebugData();
			GetGame().RPCSingleParam(this, ERPCs.DIAG_MISC_ENVIRONMENT_DEBUG_DATA, envDebugData, false, GetIdentity());
			#else
			m_Environment.ShowEnvDebugPlayerInfo(DiagMenu.GetBool(DiagMenuIDs.MISC_ENVIRONMENT_DEBUG));
			#endif
		}
		
		if (m_FallDamage && m_FallDamage.m_Debug)
		{
			#ifdef SERVER
			FallDamageDebugData fallDamageDebugData = m_FallDamage.GetFallDamageDebugData();
			GetGame().RPCSingleParam(this, ERPCs.DIAG_MISC_FALLDAMAGE_DEBUG_DATA, fallDamageDebugData, false, GetIdentity());
			#else
			m_FallDamage.ShowFallDamageDebugInfo(DiagMenu.GetBool(DiagMenuIDs.MISC_FALLDAMAGE_DEBUG));
			#endif
		}
	}
	
	override void OnEnterTrigger(ScriptedEntity trigger)
	{
		super.OnEnterTrigger(trigger);

		++m_IsInsideTrigger;
	}
	
	override void OnLeaveTrigger(ScriptedEntity trigger)
	{
		super.OnLeaveTrigger(trigger);

		--m_IsInsideTrigger;
	}
#endif

	void StaminaHUDNotifier(bool show)
	{
		if (m_Hud)
			m_Hud.SetStaminaBarVisibility(show);
	}
	
	override void DepleteStaminaEx(EStaminaModifiers modifier, float dT = -1, float coef = 1.0)
	{
		if (GetStaminaHandler())
			GetStaminaHandler().DepleteStaminaEx(modifier,dT,coef);
	}

	override bool CanConsumeStamina(EStaminaConsumers consumer)
	{
		if (!GetStaminaHandler())
			return false;
		
		bool val = (GetStaminaHandler().HasEnoughStaminaFor(consumer) && !IsRestrained() && !IsInFBEmoteState());
		
		if (!val)
			StaminaHUDNotifier(false);

		return val;
	}
	
	override bool CanStartConsumingStamina(EStaminaConsumers consumer)
	{
		if (!GetStaminaHandler())
			return false;
		
		bool val = (GetStaminaHandler().HasEnoughStaminaToStart(consumer) && !IsRestrained() && !IsInFBEmoteState());
		
		if (!val)
			StaminaHUDNotifier(false);

		return val;
	}
	
	override bool CanClimb(int climbType, SHumanCommandClimbResult climbRes)
	{
		if (GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS)
			return false;
			
		if (climbType == 1 && !CanConsumeStamina(EStaminaConsumers.VAULT))
			return false;		
		
		//! vault from water allowed
		if (IsSwimming())
			return true;
		
		if (climbType == 2 && (!CanConsumeStamina(EStaminaConsumers.CLIMB) || GetBrokenLegs() != eBrokenLegs.NO_BROKEN_LEGS))
			return false;

		if (climbType > 0 && m_InjuryHandler && m_InjuryHandler.GetInjuryAnimValue() >= InjuryAnimValues.LVL3)
			return false;

		return super.CanClimb(climbType,climbRes);
	}

	override bool CanJump()
	{
		if (GetBrokenLegs() != eBrokenLegs.NO_BROKEN_LEGS)
		{	
			return false;
		}
		
		if (!CanConsumeStamina(EStaminaConsumers.JUMP))
			return false;

		//! disables jump when player is significantly injured
		if (m_InjuryHandler && m_InjuryHandler.GetInjuryAnimValue() >= InjuryAnimValues.LVL3)
			return false;
		
		if (IsInFBEmoteState() || m_EmoteManager.m_MenuEmote)
		{
			return false;
		}	

		return super.CanJump();
	}
	
	bool IsJumpInProgress()
	{
		return m_JumpClimb.m_bIsJumpInProgress;
	}
	
	bool IsTargetInActiveRefresherRange(EntityAI target)
	{
		array<vector> temp = new array<vector>;
		temp = GetGame().GetMission().GetActiveRefresherLocations();
		int count = temp.Count();
		if (count > 0)
		{
			vector pos = target.GetPosition();
			for (int i = 0; i < count; i++)
			{
				if (vector.Distance(pos,temp.Get(i)) < GameConstants.REFRESHER_RADIUS)
					return true;
			}
			
			return false;
		}
		else
		{
			return false;
		}
	}
	
	void RequestHandAnimationStateRefresh()
	{
		if ((GetGame().IsMultiplayer() && GetGame().IsServer()))
		{
			m_RefreshAnimStateIdx++;
			if (m_RefreshAnimStateIdx > 3)
				m_RefreshAnimStateIdx = 0;
			SetSynchDirty();
		}
	}
	
	void RefreshHandAnimationState(int delay = 0)
	{
		if (delay == 0)
		{
			GetItemAccessor().OnItemInHandsChanged(true);
			GetItemAccessor().ResetWeaponInHands();
		}
		else
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(RefreshHandAnimationState,delay,false,0);
		}
	}
	
	// -------------------------------------------------------------------------
	// 		USER ACTIONS
	// -------------------------------------------------------------------------

	bool IsFacingTarget(Object target)
	{
		vector pdir = GetDirection();
		vector ptv = target.GetPosition() - GetPosition();
		pdir.Normalize();
		ptv.Normalize();

		if (Math.AbsFloat(pdir[0]-ptv[0]) < 0.5 && Math.AbsFloat(pdir[2]-ptv[2]) < 0.5)
		{
			return true;
		}
		return false;
	}

	//---------------------------------------------------------
	void OnQuickBarSingleUse(int slotClicked)
	{
		if (GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_SERVER)
			return;
		
		if (m_AreHandsLocked)
			return; //Player is in the short window of time after interrupting placement of an item and before getting it back in hands

		if (GetInventory().IsInventoryLocked() || IsEmotePlaying())
			return;
		
		if (GetThrowing().IsThrowingModeEnabled() || GetThrowing().IsThrowingAnimationPlaying())
			return;
		
		if (IsRaised() || GetCommand_Melee() || IsSwimming() || IsClimbingLadder() || IsClimbing() || IsRestrained() || IsRestrainPrelocked())
			return;
		
		if (GetDayZPlayerInventory().IsProcessing() || IsItemsToDelete())
			return;
		
		if (GetActionManager().GetRunningAction() != null)
			return;
		
		if (GetWeaponManager() && GetWeaponManager().IsRunning())
			return;
		
		if (!ScriptInputUserData.CanStoreInputUserData())
			return;
		
		//TODO MW change locking method
		//if (GetDayZPlayerInventory().HasLockedHands())
		//	return;
			
		EntityAI quickBarEntity = GetQuickBarEntity(slotClicked - 1);//GetEntityInQuickBar(slotClicked - 1);
		
		if (!quickBarEntity)
			return;
		
		Magazine mag;
		Weapon_Base wpn;
		
		if (Class.CastTo(mag, quickBarEntity) && Class.CastTo(wpn, mag.GetHierarchyParent()))
			return;

		EntityAI inHandEntity = GetHumanInventory().GetEntityInHands();
				
		if (!GetDayZPlayerInventory().IsIdle())
			return; // player is already performing some animation

		InventoryLocation handInventoryLocation = new InventoryLocation;
		handInventoryLocation.SetHands(this,quickBarEntity);
		if (this.GetInventory().HasInventoryReservation(quickBarEntity, handInventoryLocation))
			return;
		
		if (inHandEntity == quickBarEntity)
		{
			if (GetHumanInventory().CanRemoveEntityInHands())
			{
				if (LogManager.IsSyncLogEnable()) syncDebugPrint("[QB] Stash - PredictiveMoveItemFromHandsToInventory HND=" + Object.GetDebugName(inHandEntity));
				PredictiveMoveItemFromHandsToInventory();
			}
		}
		else
		{
			InventoryLocation invLocQBItem = new InventoryLocation;
			quickBarEntity.GetInventory().GetCurrentInventoryLocation(invLocQBItem);
			if (GetInventory().HasInventoryReservation(quickBarEntity,invLocQBItem))
				return;
				
			if (inHandEntity)
			{
				InventoryLocation Reserved_Item_il = new InventoryLocation;
				
				InventoryLocation inHandEntityFSwapDst = new InventoryLocation;
				inHandEntity.GetInventory().GetCurrentInventoryLocation(inHandEntityFSwapDst);

				int index = GetHumanInventory().FindUserReservedLocationIndex(inHandEntity);
				if (index >= 0)
					GetHumanInventory().GetUserReservedLocation(index, Reserved_Item_il);
				
				if (Reserved_Item_il)
					inHandEntityFSwapDst.CopyLocationFrom(Reserved_Item_il, true);
				
				if (index < 0 && GameInventory.CanSwapEntitiesEx(quickBarEntity, inHandEntity))
				{
					if (LogManager.IsSyncLogEnable()) syncDebugPrint("[QB] PredictiveSwapEntities QB=" + Object.GetDebugName(quickBarEntity) + " HND=" + Object.GetDebugName(inHandEntity));
					PredictiveSwapEntities(quickBarEntity, inHandEntity);
				}
				else if (GameInventory.CanForceSwapEntitiesEx(quickBarEntity, handInventoryLocation, inHandEntity, inHandEntityFSwapDst))
				{
					if (LogManager.IsSyncLogEnable()) syncDebugPrint("[QB] Swap - PredictiveForceSwapEntities HND=" + Object.GetDebugName(inHandEntity) +  " QB=" + Object.GetDebugName(quickBarEntity) + " fswap_dst=" + InventoryLocation.DumpToStringNullSafe(inHandEntityFSwapDst));
					PredictiveForceSwapEntities(quickBarEntity, inHandEntity, inHandEntityFSwapDst);
				}
			}
			else
			{
				if (GetInventory().HasInventoryReservation(quickBarEntity,handInventoryLocation))
					return;
				
				if (GetInventory().CanAddEntityIntoHands(quickBarEntity))
				{
					if (LogManager.IsSyncLogEnable()) syncDebugPrint("[QB] Stash - PredictiveTakeEntityToHands QB=" + Object.GetDebugName(quickBarEntity));
					PredictiveTakeEntityToHands(quickBarEntity);
				}
			}
		}	
	}
	//---------------------------------------------------------
	void OnQuickBarContinuousUseStart(int slotClicked)
	{
		if (GetInventory().IsInventoryLocked())
			return;
		
		if (IsSwimming() || IsClimbingLadder() || GetCommand_Melee() || IsClimbing() || IsRestrained() || IsRestrainPrelocked())
			return;
		
		ItemBase quickBarItem = ItemBase.Cast(GetQuickBarEntity(slotClicked - 1));

		if (GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_CLIENT)
		{
			ItemBase itemInHands = ItemBase.Cast(GetHumanInventory().GetEntityInHands());

			if (itemInHands != quickBarItem)
			{
				ActionManagerClient amc = ActionManagerClient.Cast(GetActionManager());

				if (amc.CanPerformActionFromQuickbar(itemInHands, quickBarItem))
				{
					amc.PerformActionFromQuickbar(itemInHands, quickBarItem);
				}
				else
				{
					if (IsRaised() || GetCommand_Melee())
						return;

					amc.ForceTarget(quickBarItem);
					m_QuickBarFT = true;
				}
			}
		}
		m_QuickBarHold = true;
	}
	//---------------------------------------------------------
	void OnQuickBarContinuousUseEnd(int slotClicked)
	{
		if (m_QuickBarHold)
		{
			if ( GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_CLIENT)
			{
				ActionManagerClient am = ActionManagerClient.Cast(GetActionManager());
				
				if (m_ActionQBControl)
				{
					ActionBase action = am.GetRunningAction(); 
					if (action)
					{
						if (!action.GetInput().IsActive())
						{
							am.EndActionInput();
						}
					
					}
				}
				
				if (m_QuickBarFT)
				{
					am.ClearForceTarget();
					m_QuickBarFT = false;
				}
			}
		}
		m_QuickBarHold = false;
	}
	void SetActionEndInput(ActionBase action)
	{
		m_ActionQBControl = !action.GetInput().IsActive();
	}
	
	bool IsQBControl()
	{
		return m_ActionQBControl;
	}
	void ResetActionEndInput()
	{
		m_ActionQBControl = false;
	}
	
	//---------------------------------------------------------
	//	RADIAL QUICKBAR AND RELOAD ACTIONS
	//---------------------------------------------------------
	//the same functionality as normal quick bar slot key press
	void RadialQuickBarSingleUse(int slotClicked)
	{
		OnQuickBarSingleUse(slotClicked);				
	}
	
	//removed the need for holding down quick bar slot key
	void RadialQuickBarCombine(int slotClicked)
	{
		EntityAI quickBarEntity = GetQuickBarEntity(slotClicked - 1);
		EntityAI entity_in_hands = GetHumanInventory().GetEntityInHands();
		
		ReloadWeapon(entity_in_hands, quickBarEntity);	
	}

	//removed the need for holding down quick bar slot key
	void QuickReloadWeapon(EntityAI weapon)
	{
		EntityAI magazine = GetMagazineToReload(weapon);
		ReloadWeapon(weapon, magazine);
	}
	
	//Reload weapon with given magazine
	void ReloadWeapon(EntityAI weapon, EntityAI magazine)
	{
		if (GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_CLIENT)
		{
			ActionManagerClient mngr_client;
			CastTo(mngr_client, GetActionManager());
			
			if (mngr_client && FirearmActionLoadMultiBulletRadial.Cast(mngr_client.GetRunningAction()))
			{
				mngr_client.Interrupt();
			}
			else if (GetHumanInventory().GetEntityInHands()!= magazine)
			{
				Weapon_Base wpn;
				Magazine mag;
				Class.CastTo(wpn,  weapon);
				Class.CastTo(mag,  magazine);
				if (GetWeaponManager().CanUnjam(wpn))
				{
					GetWeaponManager().Unjam();
				}
				else if (GetWeaponManager().CanAttachMagazine(wpn, mag))
				{
					GetWeaponManager().AttachMagazine(mag);
				}
				else if (GetWeaponManager().CanSwapMagazine(wpn, mag))
				{
					GetWeaponManager().SwapMagazine(mag);
				}
				else if (GetWeaponManager().CanLoadBullet(wpn, mag))
				{
					//GetWeaponManager().LoadMultiBullet(mag);

					ActionTarget atrg = new ActionTarget(mag, this, -1, vector.Zero, -1.0);
					if (mngr_client && !mngr_client.GetRunningAction() && mngr_client.GetAction(FirearmActionLoadMultiBulletRadial).Can(this, atrg, wpn))
						mngr_client.PerformActionStart(mngr_client.GetAction(FirearmActionLoadMultiBulletRadial), atrg, wpn);
				}
			}
		}		
	}

	//returns compatible magazine from player inventory with highest ammo count
	EntityAI GetMagazineToReload(EntityAI weapon)
	{
		Weapon_Base weapon_base = Weapon_Base.Cast(weapon);
		WeaponManager weapon_manager = GetWeaponManager();
		EntityAI magazine_to_reload;
		
		EntityAI ammo_magazine;				//magazine
		int last_ammo_magazine_count;
		int ammo_magazine_count;
		
		EntityAI ammo_pile;					//ammo pile
		int last_ammo_pile_count;
		int ammo_pile_count;
		
		//Get all magazines in (player) inventory
		for (int att_i = 0; att_i < GetInventory().AttachmentCount(); ++att_i)
		{
			EntityAI attachment = GetInventory().GetAttachmentFromIndex(att_i);
			ref CargoBase attachment_cargo = attachment.GetInventory().GetCargo();
			
			if (attachment_cargo)
			{
				for (int cgo_i = 0; cgo_i < attachment_cargo.GetItemCount(); ++cgo_i)
				{
					EntityAI cargo_item = attachment_cargo.GetItem(cgo_i);
					
					//check for proper magazine
					if (cargo_item.IsMagazine())
					{
						Magazine magazine = Magazine.Cast(cargo_item);
						ammo_pile_count = magazine.GetAmmoCount();
						
						//magazines (get magazine with max ammo count)
						if (weapon_manager.CanAttachMagazine(weapon_base, magazine) || weapon_manager.CanSwapMagazine(weapon_base, magazine))
						{
							if (ammo_pile_count > 0)
							{
								if (last_ammo_magazine_count == 0)
								{
									ammo_magazine = magazine;
									last_ammo_magazine_count = ammo_pile_count;
								}
								else
								{
									if (last_ammo_magazine_count < ammo_pile_count)
									{
										ammo_magazine = magazine;
										last_ammo_magazine_count = ammo_pile_count;
									}
								}
							}
						}	
						//bullets (get ammo pile with min ammo count)
						else if (weapon_manager.CanLoadBullet(weapon_base, magazine))
						{
							if (ammo_pile_count > 0)
							{
								if (last_ammo_pile_count == 0)
								{
									ammo_pile = magazine;
									last_ammo_pile_count = ammo_pile_count;
								}
								else
								{
									if (last_ammo_pile_count > ammo_pile_count)
									{
										ammo_pile = magazine;
										last_ammo_pile_count = ammo_pile_count;
									}
								}
							}
						}							
					}
				}
			}
		}

		//prioritize magazine
		if (ammo_magazine)
		{
			return ammo_magazine;		
		}
		
		return ammo_pile;
	}
	//---------------------------------------------------------
	
	
	//---------------------------------------------------------
	void OnSpawnedFromConsole()
	{
		if (GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_SERVER || !GetGame().IsMultiplayer())
		{
			GetGame().GetMission().AddDummyPlayerToScheduler(this);
			OnSelectPlayer();
		}

#ifdef DIAG_DEVELOPER
		if (!m_Bot)
		{
			m_Bot = new Bot(this);
			m_Bot.Start();
		}
#endif
	}
	
	//---------------------------------------------------------
	//Obsolete
	bool GetLiquidTendencyDrain()
	{
		return m_LiquidTendencyDrain;
	}
	//---------------------------------------------------------
	//Obsolete
	void SetLiquidTendencyDrain(bool state)
	{
		m_LiquidTendencyDrain = state;
	}
	
	//---------------------------------------------------------
	//! DEPRECATED
	bool GetFlagTendencyRaise()
	{
		return m_FlagRaisingTendency;
	}
	//---------------------------------------------------------
	//! DEPRECATED
	void SetFlagTendencyRaise(bool state)
	{
		m_FlagRaisingTendency = state;
	}
	
	override SoundOnVehicle PlaySound(string sound_name, float range, bool create_local = false)
	{
		return BetaSound.SaySound(this, sound_name, range, false);
	}
	
	float GetPlayerLoad()
	{
		return GetWeightEx();
	}
	
	bool IsOverloaded()
	{
		return GetWeightEx() >= OVERLOAD_LIMIT;
	}

	void SetPlayerLoad(float load)//Deprecated
	{
		m_CargoLoad = load;
		//Print("m_CargoLoad: " + m_CargoLoad);
		//Log(ToString(this) + "'s load weight is " + ftoa(m_CargoLoad) + " g.", LogTemplates.TEMPLATE_PLAYER_WEIGHT);
	}

	//Deprecated, will be overrid by other method calls (in order to ensure stamina calculation is properly set)
	void AddPlayerLoad(float addedload)//Deprecated
	{
		float newload = GetPlayerLoad() + addedload;
		SetPlayerLoad(newload);
		
		// Do not need -> Log is in SetPlayerLoad
		//PrintString(ToString(this) + "'s load weight is " + ToString(m_CargoLoad) + " g.");
	}

	bool IsItemInInventory(EntityAI entity)
	{
		return GetInventory().HasEntityInInventory(entity);
	}

	override bool NeedInventoryJunctureFromServer(notnull EntityAI item, EntityAI currParent, EntityAI newParent)
	{
		if (GetGame().IsMultiplayer())
		{
			DayZPlayerInstanceType t = GetInstanceType();
			switch (t)
			{
				case DayZPlayerInstanceType.INSTANCETYPE_AI_SINGLEPLAYER:
					return false;
				case DayZPlayerInstanceType.INSTANCETYPE_CLIENT:
				case DayZPlayerInstanceType.INSTANCETYPE_SERVER:
				case DayZPlayerInstanceType.INSTANCETYPE_AI_SERVER:
				case DayZPlayerInstanceType.INSTANCETYPE_AI_REMOTE:
				case DayZPlayerInstanceType.INSTANCETYPE_REMOTE:
					return true; // Might help mitigate "megabugged" (desync)
				
					if (LogManager.IsSyncLogEnable()) syncDebugPrint("[syncinv] " + GetDebugName(this) + " STS=" + GetSimulationTimeStamp() + " NeedInventoryJunctureFromServer item=" + Object.GetDebugName(item) + " currPar=" + currParent + " newPar=" + newParent);
					
					bool i_owned = GetHumanInventory().HasEntityInInventory(item);
					
					bool cp_owned = false;
					if (currParent)
						cp_owned = GetHumanInventory().HasEntityInInventory(currParent);
					
					bool np_owned = false;
					if (newParent)
						np_owned = GetHumanInventory().HasEntityInInventory(newParent);

					bool all_owned = i_owned && cp_owned && (np_owned || (newParent == null));
					if (LogManager.IsSyncLogEnable()) syncDebugPrint("[syncinv] " + GetDebugName(this) + " STS=" + GetSimulationTimeStamp() + " NeedInventoryJunctureFromServer=" + !all_owned + " i_pwn=" + i_owned + " cp_pwn=" + cp_owned + " np_pwn=" + np_owned);
					
					return !all_owned;
				default:
					Error("[syncinv] unsupported instance type t=" + t);
					break;
			}
		}
		return false;
	}
	
	
	override protected float GetWeightSpecialized(bool forceRecalc = false)
	{
		float totalWeight = super.GetWeightSpecialized(forceRecalc);
		
		ItemBase itemHands = GetItemInHands();
		if (itemHands) // adds weight of item carried in hands
		{
			totalWeight += itemHands.GetWeightEx(forceRecalc);
		}
		#ifdef DEVELOPER
		if (WeightDebug.m_VerbosityFlags & WeightDebugType.RECALC_FORCED)
		{
			float itemInHandsWeight = totalWeight - super.GetWeightSpecialized(forceRecalc);
			WeightDebugData dta = WeightDebug.GetWeightDebug(this);
			dta.SetCalcDetails("TMan: " + (super.GetWeightSpecialized(forceRecalc)+ itemInHandsWeight) +"(contents weight[includes item in hands])");
		}
		#endif
		return totalWeight;
	}
	
	void CalculateVisibilityForAI()
	{
		const int VISIBILITY_SLOTS_COUNT = 10;

		int attcount = GetInventory().AttachmentCount();
		float sumVisibility = 0;
		float countVisibility = 0;
		float visibilityMean = 0;

		if (attcount > 0)
		{
			for (int att = 0; att < attcount; att++)
			{
				EntityAI attachment = GetInventory().GetAttachmentFromIndex(att);
				if (attachment.IsClothing())
				{
					ClothingBase clothing;
					Class.CastTo(clothing, attachment);
					sumVisibility += clothing.GetItemVisibility();
					countVisibility++;
				}
			}

			visibilityMean = (sumVisibility + (VISIBILITY_SLOTS_COUNT - countVisibility)) / VISIBILITY_SLOTS_COUNT;
			SetVisibilityCoef(visibilityMean);
		}
		else
		{
			visibilityMean = 1;
			SetVisibilityCoef(visibilityMean);
		}
	}
	
	void SetVisibilityCoef(float pVisibility)
	{
		m_VisibilityCoef = pVisibility;
	}
	
	float GetVisibilityCoef()
	{
		return m_VisibilityCoef;
	}

	
	int GetShakeLevel()
	{
		return m_Shakes;
	}
	
	
	EStatLevels GetStatLevelHealth()
	{
		float health = GetHealth("","");
		return GetStatLevel(health, PlayerConstants.SL_HEALTH_CRITICAL, PlayerConstants.SL_HEALTH_LOW, PlayerConstants.SL_HEALTH_NORMAL, PlayerConstants.SL_HEALTH_HIGH);
	}
	
	EStatLevels GetStatLevelToxicity()
	{
		float toxicity = GetStatToxicity().Get();
		return GetStatLevel(toxicity, PlayerConstants.SL_HEALTH_CRITICAL, PlayerConstants.SL_HEALTH_LOW, PlayerConstants.SL_HEALTH_NORMAL, PlayerConstants.SL_HEALTH_HIGH);
	}
	
	EStatLevels GetStatLevelBlood()
	{
		float blood = GetHealth("","Blood");
		return GetStatLevel(blood, PlayerConstants.SL_BLOOD_CRITICAL, PlayerConstants.SL_BLOOD_LOW, PlayerConstants.SL_BLOOD_NORMAL, PlayerConstants.SL_BLOOD_HIGH);
	}
	
	EStatLevels GetStatLevelEnergy()
	{
		float energy = GetStatEnergy().Get();
		return GetStatLevel(energy, PlayerConstants.SL_ENERGY_CRITICAL, PlayerConstants.SL_ENERGY_LOW, PlayerConstants.SL_ENERGY_NORMAL, PlayerConstants.SL_ENERGY_HIGH);
	}
	
	EStatLevels GetStatLevelWater()
	{
		float water = GetStatWater().Get();
		return GetStatLevel(water, PlayerConstants.SL_WATER_CRITICAL, PlayerConstants.SL_WATER_LOW, PlayerConstants.SL_WATER_NORMAL, PlayerConstants.SL_WATER_HIGH);
	}
	
	EStatLevels GetImmunityLevel()
	{
		float immunity = GetImmunity();
		if (m_ImmunityBoosted)
			return EStatLevels.GREAT;
		return GetStatLevel(immunity, PlayerConstants.IMMUNITY_THRESHOLD_LEVEL_CRITICAL, PlayerConstants.IMMUNITY_THRESHOLD_LEVEL_LOW, PlayerConstants.IMMUNITY_THRESHOLD_LEVEL_NORMAL, PlayerConstants.IMMUNITY_THRESHOLD_LEVEL_HIGH);
	}
	
	//-------------------------------------
	float GetBordersImmunity()
	{
		float immunity = GetImmunity();
		float immmunity_max = 1;
		return GetStatLevelBorders(immunity, PlayerConstants.IMMUNITY_THRESHOLD_LEVEL_CRITICAL, PlayerConstants.IMMUNITY_THRESHOLD_LEVEL_LOW, PlayerConstants.IMMUNITY_THRESHOLD_LEVEL_NORMAL, PlayerConstants.IMMUNITY_THRESHOLD_LEVEL_HIGH, immmunity_max);
	}
	
	
	float GetStatBordersHealth()
	{
		float health = GetHealth("","");
		float health_max = GetMaxHealth("","");
		return GetStatLevelBorders(health, PlayerConstants.SL_HEALTH_CRITICAL, PlayerConstants.SL_HEALTH_LOW, PlayerConstants.SL_HEALTH_NORMAL, PlayerConstants.SL_HEALTH_HIGH, health_max);
	}
	
	float GetStatBordersToxicity()
	{
		float toxicity = GetStatToxicity().Get();
		float toxicity_max = GetStatToxicity().GetMax();
		return GetStatLevelBorders(toxicity, PlayerConstants.SL_HEALTH_CRITICAL, PlayerConstants.SL_HEALTH_LOW, PlayerConstants.SL_HEALTH_NORMAL, PlayerConstants.SL_HEALTH_HIGH, toxicity_max);
	}
	
	float GetStatBordersBlood()
	{
		float blood = GetHealth("","Blood");
		float blood_max = GetMaxHealth("","Blood");
		return GetStatLevelBorders(blood, PlayerConstants.SL_BLOOD_CRITICAL, PlayerConstants.SL_BLOOD_LOW, PlayerConstants.SL_BLOOD_NORMAL, PlayerConstants.SL_BLOOD_HIGH, blood_max);
	}
	
	float GetStatBordersEnergy()
	{
		float energy = GetStatEnergy().Get();
		float energy_max = GetStatEnergy().GetMax();
		return GetStatLevelBorders(energy, PlayerConstants.SL_ENERGY_CRITICAL, PlayerConstants.SL_ENERGY_LOW, PlayerConstants.SL_ENERGY_NORMAL, PlayerConstants.SL_ENERGY_HIGH, energy_max);
	}
	
	float GetStatBordersWater()
	{
		float water = GetStatWater().Get();
		float water_max = GetStatWater().GetMax();
		return GetStatLevelBorders(water, PlayerConstants.SL_WATER_CRITICAL, PlayerConstants.SL_WATER_LOW, PlayerConstants.SL_WATER_NORMAL, PlayerConstants.SL_WATER_HIGH, water_max);
	}
	
	//------------------------------------
	
	float GetStatLevelBorders(float stat_value, float critical, float low, float normal, float high, float max)
	{
		if (stat_value <= critical)
		{	
			return Math.InverseLerp(0, critical, stat_value);
		}
		if (stat_value <= low)
		{
			return Math.InverseLerp(critical, low, stat_value);
		}
		if (stat_value <= normal)
		{
			return Math.InverseLerp(low, normal, stat_value);
		}
		if (stat_value <= high)
		{
			return Math.InverseLerp(normal, high, stat_value);
		}
		return Math.InverseLerp(high, max, stat_value);
	}
	
	EStatLevels GetStatLevel(float stat_value, float critical, float low, float normal, float high)
	{
		if (stat_value <= critical)
		{
			return EStatLevels.CRITICAL;
		}
		if (stat_value <= low)
		{
			return EStatLevels.LOW;
		}
		if (stat_value <= normal)
		{
			return EStatLevels.MEDIUM;
		}
		if (stat_value <= high)
		{
			return EStatLevels.HIGH;
		}
		return EStatLevels.GREAT;
	}
	
	void SetImmunityBoosted(bool boosted)
	{
		m_ImmunityBoosted = boosted;
	}
	
	
	
	//!returns player's immunity strength between 0..1
	float GetImmunity()
	{
		float immunity;
		if (GetPlayerStats()) 
		{
			float max_health = GetMaxHealth("GlobalHealth", "Health") + 0.01;//addition to prevent divisioin by zero in case of some messup
			float max_blood = GetMaxHealth("GlobalHealth", "Blood") + 0.01;//addition to prevent divisioin by zero in case of some messup
			float energy_normalized = GetStatEnergy().Get() / PlayerConstants.SL_ENERGY_MAX;
			float water_normalized = GetStatWater().Get() / PlayerConstants.SL_WATER_MAX;
			float health_normalized = GetHealth("GlobalHealth", "Health") / max_health;
			float blood_normalized = GetHealth("GlobalHealth", "Blood") / max_blood;
			immunity = energy_normalized + water_normalized + health_normalized + blood_normalized;
			immunity = immunity / 4;//avg
			immunity =  Math.Clamp(immunity,0,1);
		}
		return immunity;
	}
	
	bool IsSprinting()
	{
		return m_MovementState.m_iMovement == DayZPlayerConstants.MOVEMENT_SPRINT;
	}

	bool CanSprint()
	{
		ItemBase item = GetItemInHands();
		if (IsRaised() || (item && item.IsHeavyBehaviour()))
			return false;
		
		if (item && GetThrowing() && GetThrowing().IsThrowingModeEnabled())
			return false;
		
		if (GetBrokenLegs() != eBrokenLegs.NO_BROKEN_LEGS)
			return false;
		
		return true;
	}
	
	bool IsInProne()
	{
		return m_MovementState.IsInProne();
	}
	
	bool IsInRasedProne()
	{
		return m_MovementState.IsInRaisedProne();
	}	
	
	bool IsLeaning()
	{
		return m_MovementState.IsLeaning();
	}	
	
	bool IsRolling()
	{
		return GetCommand_Move() && GetCommand_Move().IsInRoll();
	}
	
	bool IsClimbing()
	{
		return m_MovementState.m_CommandTypeId == DayZPlayerConstants.COMMANDID_CLIMB;
	}
	
	bool IsFalling()
	{
		return m_MovementState.m_CommandTypeId == DayZPlayerConstants.COMMANDID_FALL;
	}
	override bool IsFighting()
	{
		return m_IsFighting;
	}
	
	bool IsEmotePlaying()
	{
		return m_EmoteManager && m_EmoteManager.IsEmotePlaying();
	}
	
	override bool IsIgnoredByConstruction()
	{
		return IsRuined();
	}
	
	/*!
	Returns whether the specified movement command(s) are starting the next frame.
	Use caution when using this for anything player animation-related, some commands may require the movement state to be truly active (wait until next update)
	*/
	bool AnimCommandCheck(HumanMoveCommandID mask)
	{
		return m_AnimCommandStarting & mask;
	}
	
	void RunFightBlendTimer()
	{
		if (!m_FightEndBlendTimer)
			m_FightEndBlendTimer = new Timer();
		if (!m_FightEndBlendTimer.IsRunning())
		{
			m_FightEndBlendTimer.Run(PlayerConstants.MELEE2_MOVEMENT_BLEND_DELAY,this,"EndFighting");
		}
	}
	
	void EndFighting()
	{
		m_IsFighting = false;
	}
	
	bool CheckMeleeItemDamage(ItemBase item)
	{
		//on-hit drop of melee weapons
		//bayonets & buttstocks
		if (item && item.GetHierarchyParent() && item.GetHierarchyParent().IsWeapon() && item.IsRuined()/*&& item.FindAttachmentBySlotName()*/)
		{
			PlayAttachmentDropSound();
			return DropItem(item);
		}
		//handheld items themselves
		if (item && item == GetItemInHands() && item.IsRuined())
		{
			return PredictiveDropEntity(item);
		}
		return false;
	}
	
	void PlayAttachmentDropSound()
	{
		//TODO
	}
	
	// -------------------------------------------------------------------------
	override void RPC(int rpc_type, array<ref Param> params, bool guaranteed, PlayerIdentity recipient = NULL)
	{
		super.RPC(rpc_type, params, guaranteed, recipient);
	}

	// -------------------------------------------------------------------------
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		
		switch (rpc_type)
		{
			#ifndef SERVER
			
			case ERPCs.RPC_PLAYER_STAT:
				if (GetPlayerStats())
					GetPlayerStats().OnRPC(ctx);
			break;
			
			case ERPCs.RPC_SYNC_DISPLAY_STATUS:
				if (GetVirtualHud()) 
				{
					GetVirtualHud().OnRPC(ctx);
				}
			break;
	  
			case ERPCs.RPC_PLAYER_SYMPTOM_ON:
				if (GetSymptomManager()) 
				{
					GetSymptomManager().OnRPC(ERPCs.RPC_PLAYER_SYMPTOM_ON, ctx);
				}
			break;
			 
			case ERPCs.RPC_PLAYER_SYMPTOM_OFF:
				if (GetSymptomManager()) 
				{
					GetSymptomManager().OnRPC(ERPCs.RPC_PLAYER_SYMPTOM_OFF, ctx);
				}
			break;
			
			case ERPCs.RPC_DAMAGE_VALUE_SYNC:
				if (m_TrasferValues) 
					m_TrasferValues.OnRPC(ctx);
			break; 
			
			case ERPCs.RPC_USER_ACTION_MESSAGE:
				if (!GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_CLIENT) 
				{
					break;
				}
				if (ctx.Read(m_UAParamMessage))
				{
					string actiontext = m_UAParamMessage.param1;
					MessageAction(actiontext);
				}
			break;
			
			case ERPCs.RPC_SOFT_SKILLS_SPECIALTY_SYNC:
				ref Param1<float> p_synch = new Param1<float>(0);
				ctx.Read(p_synch);
				float specialty_level = p_synch.param1;
				GetSoftSkillsManager().SetSpecialtyLevel(specialty_level);
			break;

			case ERPCs.RPC_SOFT_SKILLS_STATS_SYNC:
				ref Param5<float, float, float, float, bool> p_debug_synch = new Param5<float, float ,float, float, bool>(0, 0, 0, 0, false);
				ctx.Read(p_debug_synch);
				float general_bonus_before = p_debug_synch.param1;
				float general_bonus_after = p_debug_synch.param2;
				float last_UA_value = p_debug_synch.param3;
				float cooldown_value = p_debug_synch.param4;
				float cooldown_active = p_debug_synch.param5;
				GetSoftSkillsManager().SetGeneralBonusBefore(general_bonus_before);
				GetSoftSkillsManager().SetGeneralBonusAfter(general_bonus_after);
				GetSoftSkillsManager().SetLastUAValue(last_UA_value);
				GetSoftSkillsManager().SetCoolDownValue(cooldown_value);
				GetSoftSkillsManager().SetCoolDown(cooldown_active);
			break;
	
			case ERPCs.RPC_WARNING_ITEMDROP:
			{
				if (GetGame().IsClient() && GetGame().GetUIManager() && !GetGame().GetUIManager().FindMenu(MENU_WARNING_ITEMDROP))
				{
					GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Call(GetGame().GetUIManager().EnterScriptedMenu,MENU_WARNING_ITEMDROP,null);
					GetGame().GetMission().AddActiveInputExcludes({"menu"});
				}
				break;
			}	
			
			case ERPCs.RPC_WARNING_TELEPORT:
			{
				if (GetGame().IsClient() && GetGame().GetUIManager() && !GetGame().GetUIManager().FindMenu(MENU_WARNING_TELEPORT))
				{
					GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Call(GetGame().GetUIManager().EnterScriptedMenu,MENU_WARNING_TELEPORT,null);
					GetGame().GetMission().AddActiveInputExcludes({"menu"});
				}
				break;
			}	
			case ERPCs.RPC_INIT_SET_QUICKBAR:
				ref Param1<int> count = new Param1<int>(0); 
				if (ctx.Read(count))
				{
					for (int i = 0; i < count.param1 ; i++)
					{
						m_QuickBarBase.OnSetEntityRPC(ctx);	
					}
				}
			break;
			
			case ERPCs.RPC_SYNC_THERMOMETER:
			{
				float value;
				if (ctx.Read(value))
					m_Hud.SetTemperature(value.ToString() + "#degrees_celsius");
				break;
			}
			
			
			case ERPCs.RPC_CHECK_PULSE:
				ctx.Read(CachedObjectsParams.PARAM1_INT);
				EPulseType pulse;
				if ((CachedObjectsParams.PARAM1_INT.param1 & ActionCheckPulse.TARGET_IRREGULAR_PULSE_BIT) == 0)
				{
					pulse = EPulseType.REGULAR;
				}
				else
				{
					pulse = EPulseType.IRREGULAR;
				}
			
				int blood_level = ~ActionCheckPulse.TARGET_IRREGULAR_PULSE_BIT & CachedObjectsParams.PARAM1_INT.param1;
				if (m_CheckPulseLastTarget)
				{
					m_CheckPulseLastTarget.SetLastUAMessage(ActionCheckPulse.GetPulseMessage(pulse, blood_level));
				}
			break;
			
			//! uses contants outside of ERPCs !!!!
			//woodcutting
			case PlantType.TREE_HARD:
				SoundHardTreeFallingPlay();
			break;
			
			case PlantType.TREE_SOFT:
				SoundSoftTreeFallingPlay();
			break;
			
			case PlantType.BUSH_HARD:
				SoundHardBushFallingPlay();
			break;
			
			case PlantType.BUSH_SOFT:
				SoundSoftBushFallingPlay();
			break;
			
			case ERPCs.RPC_DEBUG_MONITOR_FLT:
				if (m_DebugMonitorValues)
				{
					m_DebugMonitorValues.OnRPCFloat(ctx);
				}
			break;
			
			case ERPCs.RPC_DEBUG_MONITOR_STR:
				if (m_DebugMonitorValues) 
				{
					m_DebugMonitorValues.OnRPCString(ctx);
				}
			break;
			#endif

			
			/*
			case ERPCs.RPC_CRAFTING_INVENTORY_INSTANT:
				ref Param3<int, ItemBase, ItemBase> craftParam = new Param3<int, ItemBase, ItemBase>(-1, NULL, NULL);
				if (ctx.Read(craftParam))
				{
					m_ModuleRecipesManager.PerformRecipeServer(craftParam.param1, craftParam.param2, craftParam.param3, this);				
				}
			break;
			*/
						

			
			#ifdef DIAG_DEVELOPER
			case ERPCs.DEV_RPC_AGENT_RESET:
			{
				bool val;
				if (ctx.Read(val))
					m_AgentPool.RemoveAllAgents();
				break;
			}
						
			case ERPCs.DEV_PLAYER_DEBUG_REQUEST:
			{
				PluginRemotePlayerDebugServer plugin_remote_server = PluginRemotePlayerDebugServer.Cast(GetPlugin(PluginRemotePlayerDebugServer));
				plugin_remote_server.OnRPC(ctx, this);
				break;
			}
			
			case ERPCs.DEV_PLAYER_DEBUG_DATA:
			{
				PluginRemotePlayerDebugClient plugin_remote_client = PluginRemotePlayerDebugClient.Cast(GetPlugin(PluginRemotePlayerDebugClient));
				PluginDeveloper plugin_dev = PluginDeveloper.Cast(GetPlugin(PluginDeveloper));
				if (plugin_dev.m_ScriptConsole)	
					plugin_dev.m_ScriptConsole.OnRPCEx(rpc_type, ctx);
				else
					plugin_remote_client.OnRPC(ctx);				
				break;
			}
			
			case ERPCs.DEV_AGENT_GROW:
			{
				m_AgentPool.RemoteGrowRequestDebug(ctx);
				break;
			}	

			case ERPCs.RPC_ITEM_DIAG_CLOSE:
			{
				PluginItemDiagnostic mid = PluginItemDiagnostic.Cast(GetPlugin(PluginItemDiagnostic));
				mid.StopWatchRequest(this);
				break;
			}
			
			//! Universal Temperature Sources debug
			case ERPCs.DEV_REQUEST_UTS_DEBUG:
			{
				PluginUniversalTemperatureSourceServer plugin_uts_server = PluginUniversalTemperatureSourceServer.Cast(GetPlugin(PluginUniversalTemperatureSourceServer));
				plugin_uts_server.OnRPC(ctx, this);
				break;
			}
			
			case ERPCs.DEV_UTS_DEBUG_DATA:
			{
				PluginUniversalTemperatureSourceClient plugin_uts_client = PluginUniversalTemperatureSourceClient.Cast(GetPlugin(PluginUniversalTemperatureSourceClient));
				plugin_uts_client.OnRPC(ctx);
				break;
			}
			//! ---
			#endif
			
			#ifdef DEVELOPER
			case ERPCs.DEV_RPC_SERVER_SCRIPT:
			{
				//PluginItemDiagnostic plugin = PluginItemDiagnostic.Cast(GetPlugin(PluginItemDiagnostic));
				//SetDebugDeveloper_item(plugin.GetWatchedItem(this));//!! needs to be inside DEVELOPER ifdef
				
				if (ctx.Read(CachedObjectsParams.PARAM1_STRING))
				{
					_player = this;
					string code = CachedObjectsParams.PARAM1_STRING.param1;
					bool success = GetGame().ExecuteEnforceScript("void scConsMain() \n{\n" + code + "\n}\n", "scConsMain");
					CachedObjectsParams.PARAM1_BOOL.param1 = success;
					RPCSingleParam(ERPCs.DEV_RPC_SERVER_SCRIPT_RESULT, CachedObjectsParams.PARAM1_BOOL,true, GetIdentity());
				}
				break;
			}
			
			case ERPCs.DEV_RPC_SERVER_SCRIPT_RESULT:
			{
				PluginDeveloper dev = PluginDeveloper.Cast(GetPlugin(PluginDeveloper));
				if (dev.m_ScriptConsole)
						dev.m_ScriptConsole.OnRPCEx(rpc_type, ctx);
				break;
			}
			
			case ERPCs.DEV_SET_DEV_ITEM:
			{
				Param1<EntityAI> ent = new Param1<EntityAI>(null);
				if (ctx.Read(ent) && ent.param1)
				{
					PluginItemDiagnostic mid2 = PluginItemDiagnostic.Cast(GetPlugin(PluginItemDiagnostic));
					mid2.RegisterDebugItem(ent.param1, this);
				}
				//SetDebugDeveloper_item(this);
				break;
			}

			case ERPCs.RPC_ITEM_DIAG:
			{
				Param1<EntityAI> p1 = new Param1<EntityAI>(null);
				if (ctx.Read(p1))
				{
					PluginItemDiagnostic plgn = PluginItemDiagnostic.Cast(GetPlugin(PluginItemDiagnostic));
					plgn.OnRPC(p1.param1, ctx);
				}
				break;
			}
			#endif
		}
	
		#ifdef DIAG_DEVELOPER
			PluginDeveloper module_rc = PluginDeveloper.Cast(GetPlugin(PluginDeveloper));
			if (module_rc) 
				module_rc.OnRPC(this, rpc_type, ctx);		
			
			PluginDeveloperSync module_rcs = PluginDeveloperSync.Cast(GetPlugin(PluginDeveloperSync));
			if (module_rcs) 
				module_rcs.OnRPC(this, rpc_type, ctx);
		
			#ifdef SERVER
				PluginDiagMenu plugin_diag_menu = PluginDiagMenu.Cast(GetPlugin(PluginDiagMenuServer));
			#else
				PluginDiagMenu plugin_diag_menu = PluginDiagMenu.Cast(GetPlugin(PluginDiagMenuClient));
			#endif
		
			if (plugin_diag_menu) 
				plugin_diag_menu.OnRPC(this, rpc_type, ctx);
		#endif
	}
	
	override void OnGameplayDataHandlerSync()
	{
		super.OnGameplayDataHandlerSync();
		
		UpdateLighting();
		GetGame().GetMission().SetRespawnModeClient(CfgGameplayHandler.GetDisableRespawnDialog());
		SetHitPPEEnabled(CfgGameplayHandler.GetHitIndicationPPEEnabled());
	}
	
	void UpdateLighting()
	{
		Mission mission = GetGame().GetMission();
		if (mission)
		{
			WorldLighting wLighting = mission.GetWorldLighting();
			if (wLighting)
				wLighting.SetGlobalLighting(CfgGameplayHandler.GetLightingConfig());
		}
	}
	
	void SetContaminatedEffectEx(bool enable, int ppeIdx = -1, int aroundId = ParticleList.CONTAMINATED_AREA_GAS_AROUND, int tinyId = ParticleList.CONTAMINATED_AREA_GAS_TINY,  string soundset = "",  bool partDynaUpdate = false, int newBirthRate = 0)
	{
		if (enable) // enable
		{
			// We assume that if this is set to true the PPE is already active
			if (m_ContaminatedAreaEffectEnabled == enable)
				return;
			
			if (aroundId !=0)
			{
				if (!m_ContaminatedAroundPlayer)
				{
					m_ContaminatedAroundPlayer = ParticleManager.GetInstance().PlayInWorld(aroundId, GetPosition());
				}
				// First entry in an area with dynamic tweaks to particles
				if (partDynaUpdate)
				{
					m_ContaminatedAroundPlayer.SetParameter(0, EmitorParam.BIRTH_RATE, newBirthRate);
				}
			}
			
			if (!m_ContaminatedAroundPlayerTiny && tinyId !=0)
			{
				m_ContaminatedAroundPlayerTiny = ParticleManager.GetInstance().PlayInWorld(tinyId,  GetPosition());
			}
			
			if (ppeIdx != -1)
			{
				PPERequesterBase ppeRequester;
				if (Class.CastTo(ppeRequester, PPERequesterBank.GetRequester(ppeIdx)))
					ppeRequester.Start();
			}
			
			// We start playing the ambient sound
			if (!m_AmbientContamination && soundset != "")
				PlaySoundSetLoop(m_AmbientContamination, soundset, 0.1, 0.1);
			
			SetSoundControllerOverride("contamination",1,SoundControllerAction.Overwrite);
		}
		else // disable
		{
			if (m_ContaminatedAroundPlayer)
			{
				m_ContaminatedAroundPlayer.Stop();
				m_ContaminatedAroundPlayer = null;
			}
			
			if (m_ContaminatedAroundPlayerTiny)
			{
				m_ContaminatedAroundPlayerTiny.Stop();
				m_ContaminatedAroundPlayerTiny = null;
			}
			if (ppeIdx != -1)
				PPERequesterBank.GetRequester(ppeIdx).Stop(new Param1<bool>(true)); //fade out
			
			// We stop the ambient sound
			if (m_AmbientContamination)
				StopSoundSet(m_AmbientContamination);
			
			SetSoundControllerOverride("contamination",0,SoundControllerAction.None);
			// We make sure to reset the state
		}
		m_ContaminatedAreaEffectEnabled = enable;
	}
	
	void SetContaminatedEffect(bool enable, int ppeIdx = -1, int aroundId = ParticleList.CONTAMINATED_AREA_GAS_AROUND, int tinyId = ParticleList.CONTAMINATED_AREA_GAS_TINY, bool partDynaUpdate = false, int newBirthRate = 0)
	{
		SetContaminatedEffectEx(enable, ppeIdx, aroundId, tinyId, "", partDynaUpdate, newBirthRate);
	}
	
	// -------------------------------------------------------------------------
	void UpdateCorpseState()
	{
		UpdateCorpseStateVisual();
		if (m_CorpseState > 0 && !GetIsFrozen())
			SetDecayEffects(Math.AbsInt(m_CorpseState));
		else
			SetDecayEffects();//no params means remove the effects
		m_CorpseStateLocal = m_CorpseState;
	}

	// -------------------------------------------------------------------------
	override void EEHealthLevelChanged(int oldLevel, int newLevel, string zone)
	{
		super.EEHealthLevelChanged(oldLevel, newLevel, zone);
		if (!GetGame().IsDedicatedServer())
		{
			if (newLevel == GameConstants.STATE_RUINED)
			{
				ClearLastUAMessage();
			}
			if (m_CorpseState != 0)
			{
				GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(UpdateCorpseState, 0, false);
			}
		}
	}
	
	
	// -------------------------------------------------------------------------
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		if (m_ModuleLifespan)
		{
			m_ModuleLifespan.SynchLifespanVisual(this, m_LifeSpanState, m_HasBloodyHandsVisible, m_HasBloodTypeVisible, m_BloodType);
			
			if (m_LifespanLevelLocal != m_LifeSpanState) //client solution, lifespan level changed
			{
				m_LifespanLevelLocal = m_LifeSpanState;
				UpdateHairSelectionVisibility();
			}	
		}
		
		CheckSoundEvent();
		if (GetBleedingManagerRemote() && IsPlayerLoaded())
		{
			GetBleedingManagerRemote().OnVariablesSynchronized(GetBleedingBits());
		}
		
		if (m_CorpseStateLocal != m_CorpseState && (IsPlayerLoaded() || IsControlledPlayer()))
		{
			UpdateCorpseState();
		}
		
		if (m_RefreshAnimStateIdx != m_LocalRefreshAnimStateIdx)
		{
			RefreshHandAnimationState(396); //mean animation blend time
			m_LocalRefreshAnimStateIdx = m_RefreshAnimStateIdx;
		}
		if (m_InsideEffectArea != m_InsideEffectAreaPrev)
		{
			if (m_InsideEffectArea)
			{
				OnPlayerIsNowInsideEffectAreaBeginClient();
			}
			else
			{
				OnPlayerIsNowInsideEffectAreaEndClient();
			}
			m_InsideEffectAreaPrev = m_InsideEffectArea;
		}
		
		//-------MODIFIERS START--------
		if (m_SyncedModifiers != m_SyncedModifiersPrev)
		{
			int diff = (m_SyncedModifiers & ~m_SyncedModifiersPrev) | (~m_SyncedModifiers & m_SyncedModifiersPrev);//XOR gets us a mask for modifiers that changed by either deactivating, or activating

			if (eModifierSyncIDs.MODIFIER_SYNC_DROWNING & diff)//has this modifier's status as active/inactive changed ?
			{
				if (eModifierSyncIDs.MODIFIER_SYNC_DROWNING & m_SyncedModifiers)//is this modifier currently active ? if so, it means it's been just activated
				{
					SetDrowning(true);
				}
				else
				{
					SetDrowning(false);
				}
				
			}
			
			m_SyncedModifiersPrev = m_SyncedModifiers;
		}
		//-------MODIFIERS END--------
		
		HandleBrokenLegsSync();
		
		//! Remotely Activated Item Behaviour refresh on item is in hands
		ItemBase itemHands = GetItemInHands();
		if (itemHands)
		{
			RemotelyActivatedItemBehaviour raib = itemHands.GetRemotelyActivatedItemBehaviour();
			if (raib)
				raib.OnVariableSynchronized();
		}
		
		//restrain events
		if (m_IsRestrainedLocal != m_IsRestrained)
		{
			m_IsRestrainedLocal = m_IsRestrained;
			OnRestrainChangeClient();
		}
		
		if (m_IsRestrainStartedLocal != m_IsRestrainStarted)
		{
			m_IsRestrainStartedLocal = m_IsRestrainStarted;
			OnRestrainStartedChangeClient();
		}
	}
	
	//! Checks whether modifier (which has syncing enabled) is currently active, works on both Client and Server
	bool IsSyncedModifierActive(eModifierSyncIDs modifier)
	{
		return (m_SyncedModifiers & m_SyncedModifiersPrev);
	}
	
	void HandleBrokenLegsSync()
	{
		if (m_BrokenLegState != m_LocalBrokenState)
		{
			m_LocalBrokenState = m_BrokenLegState;
			if (GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS)
			{
				if (m_BrokenLegState < 0)
					BreakLegSound();
			}
		}
	}

	void FixAllInventoryItems()
	{
		array<EntityAI> items = new array<EntityAI>();
		GetInventory().EnumerateInventory(InventoryTraversalType.INORDER, items);
		foreach	(EntityAI item : items)
		{
			item.SetHealthMax("", "");
			item.SetWet(item.GetWetInit());
			item.SetTemperatureDirect(GameConstants.ITEM_TEMPERATURE_NEUTRAL_ZONE_MIDDLE);
			item.SetFrozen(false);
		}
	}
	
	void OnInventoryMenuOpen();	
	void OnInventoryMenuClose();
	
	//--------------------------------------------------------------------------
	void OnSelectPlayer()
	{
		//Print("PlayerBase | OnSelectPlayer()");
		m_PlayerSelected = true;
		
		m_QuickBarBase.updateSlotsCount();
		
		m_WeaponManager.SortMagazineAfterLoad();
		
		PlayerIdentity identity = GetIdentity();
		
#ifdef FEATURE_NETWORK_RECONCILIATION
		if (identity)
		{
			//! TODO(kumarjac): vhc: Check if it causes further issues with SelectPlayer while a player is leaving freecam
			identity.Possess(this);
		}
#endif

		if (GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_SERVER || !GetGame().IsMultiplayer())
		{
			if (identity)
			{
				m_CachedPlayerID = identity.GetId();
				m_CachedPlayerName  = identity.GetName();
			}
			
			//! add callbacks for ai target system
			SetAITargetCallbacks(new AITargetCallbacksPlayer(this));

			array<ref Param> params = new array<ref Param>;	
			if (m_aQuickBarLoad)
			{
				int count = m_aQuickBarLoad.Count();
				Param1<int> paramCount = new Param1<int>(count);
				params.Insert(paramCount);
				for (int i = 0; i < count; i++)
				{
					m_QuickBarBase.OnSetEntityNoSync(m_aQuickBarLoad.Get(i).param1,m_aQuickBarLoad.Get(i).param2);
					params.Insert(m_aQuickBarLoad.Get(i));
				}					
							
				if (count > 0 && GetGame().IsMultiplayer())
				{
					GetGame().RPC(this, ERPCs.RPC_INIT_SET_QUICKBAR, params, true, identity);
				}
				m_aQuickBarLoad = NULL;
			}		
			
			GetSoftSkillsManager().InitSpecialty(GetStatSpecialty().Get());
			GetModifiersManager().SetModifiers(true);
			
			SetSynchDirty();
			
			if (GetGame().IsMultiplayer())
			{
				//Drop item warning
				if (m_ProcessUIWarning)
				{
					GetGame().RPCSingleParam(this, ERPCs.RPC_WARNING_ITEMDROP, null, true, identity);
					m_ProcessUIWarning = false;
				}
				
				GetGame().GetMission().SyncRespawnModeInfo(identity);
			}
		}
		
		if (GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_SERVER)
		{
			m_ActionManager = new ActionManagerServer(this);
			m_ConstructionActionData = new ConstructionActionData();
			
			CheckForGag();
		}
		else if (GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_CLIENT)
		{
			m_ActionManager = new ActionManagerClient(this);
			m_CraftingManager = new CraftingManager(this,m_ModuleRecipesManager);
			m_ConstructionActionData = new ConstructionActionData();
			
		}
		else if (GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_AI_SINGLEPLAYER)
		{
			m_ActionManager = new ActionManagerServer(this);
		}
		
		if (GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_CLIENT)
		{
			if (GetGame().GetMission())
			{
				GetGame().GetMission().ResetGUI();
				// force update player
				GetDayZGame().GetBacklit().UpdatePlayer(true);
			}
					
			m_DeathCheckTimer = new Timer();
			m_DeathCheckTimer.Run(0.1, this, "CheckDeath", null, true);
			PPEManagerStatic.GetPPEManager().StopAllEffects(PPERequesterCategory.ALL);
			CheckForBurlap();
			
			int characterCount = GetGame().GetMenuData().GetCharactersCount() - 1;
			int idx = GetGame().GetMenuData().GetLastPlayedCharacter();
			if (idx == GameConstants.DEFAULT_CHARACTER_MENU_ID || idx > characterCount)
			{
				GetGame().GetCallQueue(CALL_CATEGORY_GUI).Call(SetNewCharName);
			}

			GetGame().GetMission().EnableAllInputs(true);
			
			m_PresenceNotifier = PluginPresenceNotifier.Cast(GetPlugin(PluginPresenceNotifier));
			m_PresenceNotifier.Init(this);
			#ifdef DIAG_DEVELOPER
			m_TargetTemperatureDiag = PluginTargetTemperature.Cast(GetPlugin(PluginTargetTemperature));
			m_TargetTemperatureDiag.Init(this);
			#endif
			OnGameplayDataHandlerSync();//only here for legacy reasons
		}
		
#ifdef BOT
		m_Bot = new Bot(this);
		m_Bot.Start(true, new MyBotTrigger(this));
#endif
	}
		
	override void SimulateDeath(bool state)
	{
		super.SimulateDeath(state);

		m_UndergroundHandler = null;
		m_UndergroundBunkerHandler = null;
		if (m_EffectWidgets)
		{
			m_EffectWidgets.StopAllEffects();
			m_EffectWidgets.ClearSuspendRequests();
		}
	}
	
	void SetNewCharName()
	{
		g_Game.GetMenuData().SaveCharacter(false, true);
		g_Game.GetMenuData().SetCharacterName(g_Game.GetMenuData().GetLastPlayedCharacter(), g_Game.GetMenuDefaultCharacterData(false).GetCharacterName());
		g_Game.GetMenuData().SaveCharactersLocal();
	}

	void CheckForBurlap()
	{
		if (GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_CLIENT)
		{
			BurlapSackCover attachment;
			Class.CastTo(attachment, GetInventory().FindAttachment(InventorySlots.HEADGEAR));
			
			PPERequester_BurlapSackEffects req;
			if (Class.CastTo(req,PPERequesterBank.GetRequester(PPERequesterBank.REQ_BURLAPSACK)))
			{
				if (attachment)
				{
					req.Start();
				}
				else if (req.IsRequesterRunning())
				{
					PPERequesterBank.GetRequester(PPERequester_BurlapSackEffects).Stop();
				}
			}
		}
	}
	
	void CheckForGag()
	{
		#ifdef SERVER
		Clothing item;
		Class.CastTo(item, GetInventory().FindAttachment(InventorySlots.MASK));
		if (!item)
		{
			Class.CastTo(item, GetInventory().FindAttachment(InventorySlots.HEADGEAR));
		}
		
		if (item && item.IsObstructingVoice())
		{
			item.MutePlayer(this,true);
		}
		else //should probably check for relevant types before removing mumbling and obstruction specifically..
		{
			GetGame().SetVoiceEffect(this, VoiceEffectMumbling, false);
			GetGame().SetVoiceEffect(this, VoiceEffectObstruction, false);
		}
		#endif
	}
	
	void UpdateMaskBreathWidget(notnull MaskBase mask, bool is_start = false)
	{
		if (is_start)
			m_EffectWidgets.ResetMaskUpdateCount();
		float resistance = 0;
		if (mask.HasIntegratedFilter() || mask.IsExternalFilterAttached())
			resistance = 1 - mask.GetFilterQuantity01();

		
		m_EffectWidgets.OnVoiceEvent(resistance);
		m_EffectWidgets.IncreaseMaskUpdateCount();
	
	}
	
	
	// -------------------------------------------------------------------------
	void OnVoiceEvent(PlayerSoundEventBase voice_event)
	{
		if (m_EffectWidgets)
		{
			MaskBase mask = MaskBase.Cast(GetInventory().FindAttachment(InventorySlots.MASK));
			if (mask)
			{
				UpdateMaskBreathWidget(mask, true);
				//m_EffectWidgets.SetBreathIntensityStamina(GetStaminaHandler().GetStaminaCap(),GetStaminaHandler().GetStamina());
			}
		}
	}
	
	// -------------------------------------------------------------------------
	//! called every cmd handler tick during a playback of PlayerSoundEvent, m_ProcessPlaybackEvent needs to be enabled on the PlayerSoundEvent
	void OnVoiceEventPlayback(PlayerSoundEventBase voice_event, AbstractWave callback, float playback_time)
	{
		if (m_EffectWidgets)
		{
			MaskBase mask = MaskBase.Cast(GetInventory().FindAttachment(InventorySlots.MASK));
			if (mask)
			{
				if (m_EffectWidgets.m_MaskWidgetUpdateCount < 2 && callback.GetLength() > 2 && playback_time > 0.5 )
					UpdateMaskBreathWidget(mask);
			}
		}
	}
	

	// -------------------------------------------------------------------------
	override bool OnInputUserDataProcess(int userDataType, ParamsReadContext ctx)
	{
		if (super.OnInputUserDataProcess(userDataType, ctx))
			return true;
		
		if (m_QuickBarBase.OnInputUserDataProcess(userDataType, ctx))
			return true;
		
		if (m_WeaponManager.OnInputUserDataProcess(userDataType, ctx))
			return true;

		if (HandleRemoteItemManipulation(userDataType, ctx))
			return true;
		
		if (userDataType == INPUT_UDT_INVENTORY && GetHumanInventory().OnInputUserDataProcess(ctx))
			return true;
		
		if (TogglePlacingServer(userDataType, ctx))
			return true;
		
		if (ResetADSPlayerSync(userDataType, ctx))
			return true;
		
		string uid;
		bool mute;
		if (userDataType == INPUT_UDT_USER_MUTE_XBOX)
		{
			if (ctx.Read(uid) && ctx.Read(mute))
			{
				GetGame().MutePlayer(uid, GetIdentity().GetPlainId(), mute);
				// commented because plainID should not be present in logs
				//Print("Player: " + GetIdentity().GetId() + " set mute for " + uid + " to " + mute);
			}
		}
		
		if (m_EmoteManager && userDataType == INPUT_UDT_GESTURE)
			return m_EmoteManager.OnInputUserDataProcess(userDataType, ctx);
		
		if (userDataType == INPUT_UDT_WEAPON_LIFT_EVENT)
			return ReadLiftWeaponRequest(userDataType, ctx);
		
		if (m_ActionManager)
			return m_ActionManager.OnInputUserDataProcess(userDataType, ctx);
		return false;
	}
	
	float GetHealthRegenSpeed()
	{
		float blood =  GetHealth("GlobalHealth", "Blood");
		float blood_scale_normalized = Math.InverseLerp(PlayerConstants.BLOOD_THRESHOLD_FATAL, GetMaxHealth("", "Blood"), blood);
		blood_scale_normalized = Math.Clamp(blood_scale_normalized,0,1);
		return Math.Lerp(PlayerConstants.HEALTH_REGEN_MIN, PlayerConstants.HEALTH_REGEN_MAX, blood_scale_normalized);
	}

	bool HandleRemoteItemManipulation(int userDataType, ParamsReadContext ctx)
	{
		if (userDataType == INPUT_UDT_ITEM_MANIPULATION)
		{
			int type = -1;
			ItemBase item1 = null;
			ItemBase item2 = null;
			bool use_stack_max = false;
			int slot_id = -1;

			if (!ctx.Read(type))
				return false;
			
			if (type == 4)
			{
				if (!ctx.Read(item1))
					return false;
				
				InventoryLocation dst = new InventoryLocation;
				if (item1 && dst.ReadFromContext(ctx))
				{
					//Print(InventoryLocation.DumpToStringNullSafe(dst));
					bool dummy;
					if (ctx.Read(dummy))
						item1.SplitItemToInventoryLocation(dst);
					else
						item1.SplitIntoStackMaxToInventoryLocation(dst);
					return true;
				}
				return false;
			}
			
			if (!ctx.Read(item1))
				return false;
			if (!ctx.Read(item2))
				return false;
			if (!ctx.Read(use_stack_max))
				return false;
			if (!ctx.Read(slot_id))
				return false;
			
			if (type == -1 && item1 && item2)//combine
			{
				item1.CombineItems(item2, use_stack_max);
			}
			else if (type == 1 && item1)
			{
				if (use_stack_max)
					item1.SplitIntoStackMax(item2, slot_id, this);
				else
					item1.SplitItem(this);
			}
			else if (type == 2 && item1)
			{
				int row, col;
				if (!ctx.Read(row))
					return false;
				if (!ctx.Read(col))
					return false;
				item1.SplitIntoStackMaxCargo(item2, slot_id, row, col);
			}
			else if (type == 3 && item1)
			{
				item1.SplitIntoStackMaxHands(this);
			}
			return true;
		}
		return false;
	}
	
	// -------------------------------------------------------------------------
	ItemBase GetItemInHands()
	{
		HumanInventory humanInventory = GetHumanInventory();
		if (humanInventory)
			return ItemBase.Cast(humanInventory.GetEntityInHands());

		return null;
	}

	//--------------------------------------------------------------------------


	override EntityAI SpawnEntityOnGroundPos(string object_name, vector pos)
	{
		if (GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_SERVER || !GetGame().IsMultiplayer())
		{
			bool is_AI = GetGame().IsKindOf(object_name, "DZ_LightAI");
			if (is_AI)
			{
				return SpawnAI(object_name, pos);
			}
			else
			{
				InventoryLocation inv_loc = new InventoryLocation;
				vector mtx[4];
				Math3D.MatrixIdentity4(mtx);
				mtx[3] = pos;
				inv_loc.SetGround(null, mtx);
				
				int flags = ECE_PLACE_ON_SURFACE;
				#ifdef DEVELOPER
				if (g_Game.IsKindOf(object_name, "Boat"))
					flags = ECE_KEEPHEIGHT;
				#endif

				return EntityAI.Cast(GetGame().CreateObjectEx(object_name, inv_loc.GetPos(), flags));
			}
		}
		return null;
	}
	
	EntityAI SpawnEntityOnGroundRaycastDispersed(string object_name, float raycastDistance = DEFAULT_SPAWN_DISTANCE, float radius = UAItemsSpreadRadius.DEFAULT)
	{
		vector posRandom = MiscGameplayFunctions.GetRandomizedPositionVerifiedPlayer(this,raycastDistance,radius,this);
		return SpawnEntityOnGroundPos(object_name,posRandom);
	}

	EntityAI SpawnEntityOnGroundOnCursorDir(string object_name, float distance)
	{
		vector position = GetPosition() + (GetDirection() * distance);
		return SpawnEntityOnGroundPos(object_name, position);
	}

	EntityAI SpawnAI(string object_name, vector pos)
	{
		bool is_ai = GetGame().IsKindOf(object_name, "DZ_LightAI");
		if (is_ai)
		{
			return EntityAI.Cast(GetGame().CreateObjectEx(object_name, pos, ECE_PLACE_ON_SURFACE|ECE_INITAI|ECE_EQUIP_ATTACHMENTS));
		}
		return NULL;
	}
	
	//--------------------------------------------------------------------------
	bool DropItem(ItemBase item)
	{
		bool can_be_dropped = CanDropEntity(item);
		if (can_be_dropped)
		{
			can_be_dropped = PredictiveDropEntity(item);
		}

		vector 	pos_spawn	= GetPosition() + GetDirection();
		pos_spawn[0]		= pos_spawn[0] + Math.RandomFloat(-0.2, 0.2);
		pos_spawn[2]		= pos_spawn[2] + Math.RandomFloat(-0.2, 0.2);

		item.SetPosition(pos_spawn);
		item.PlaceOnSurface();
		return can_be_dropped;
	}

	// -------------------------------------------------------------------------
	/**
	/brief Creates \p ItemBase in players inventory. If player dont have enough slots for new item, item will be created on the ground under player.
		\param item_name \p string Class name of ItemBase in configs
		\returns \p ItemBase Created item
		@code
			ItemBase item = g_Game.GetPlayer().CreateInInventory("Consumable_GardenLime", "cargo_weapon");
		@endcode
	*/
	
	
	EntityAI CreateInInventory(string item_name, string cargo_type = "", bool full_quantity = false)
	{
		InventoryLocation inv_loc = new InventoryLocation;
		if (GetInventory().FindFirstFreeLocationForNewEntity(item_name, FindInventoryLocationType.ANY, inv_loc))
		{
			return SpawnItemOnLocation(item_name, inv_loc, full_quantity);
		}
		return NULL;
	}
	
	ItemBase CreateCopyOfItemInInventory (ItemBase src)
	{
		InventoryLocation loc = new InventoryLocation;
		string t = src.GetType();
		if (GetInventory().FindFirstFreeLocationForNewEntity(t, FindInventoryLocationType.CARGO, loc))
		{
			bool locked = GetGame().HasInventoryJunctureDestination(this, loc);
			if (locked)
			{
				Print("Warning: Split: CreateCopyOfItemInInventory - Cannot create entity at locked inventory at loc=" + InventoryLocation.DumpToStringNullSafe(loc));
				return null;
			}
			ItemBase dst = ItemBase.Cast(GetInventory().LocationCreateLocalEntity(loc, t, ECE_IN_INVENTORY, RF_DEFAULT));
			if (dst)
			{
				MiscGameplayFunctions.TransferItemProperties(src, dst);

				GetGame().RemoteObjectTreeCreate(dst);
				
				Print("CreateCopyOfItemInInventory - created " + dst.GetName() + " at loc=" + InventoryLocation.DumpToStringNullSafe(loc));
			}
			return dst;
		}
		return NULL;
	}
	
	ItemBase CreateCopyOfItemInInventoryOrGround(ItemBase src)
	{
		return CreateCopyOfItemInInventoryOrGroundEx(src, false);
	}
	
	ItemBase CreateCopyOfItemInInventoryOrGroundEx(ItemBase src, bool markItemAsSplitResult = false)
	{
		ItemBase dst = CreateCopyOfItemInInventory(src);
		if (!dst)
		{
			dst = ItemBase.Cast(SpawnEntityOnGroundPos(src.GetType(), this.GetPosition()));
			dst.PlaceOnSurface();
			dst.SetResultOfSplit(markItemAsSplitResult);
			MiscGameplayFunctions.TransferItemProperties(src, dst);
		}
		
		return dst;
	}



	// -------------------------------------------------------------------------
	/**
	colorStatusChannel
	colorAction
	colorFriendly
	colorImportant
	*/
	void Message(string text, string style)
	{
		if (GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_SERVER)
		{
			GetGame().ChatMP(this, text, style);
		}
		else
		{
			GetGame().Chat(text, style);
		}
	}

	// -------------------------------------------------------------------------
	void MessageStatus(string text)
	{
		Message(text, "colorStatusChannel");
	}

	// -------------------------------------------------------------------------
	void MessageAction(string text)
	{
		Message(text, "colorAction");
	}

	// -------------------------------------------------------------------------
	void MessageFriendly(string text)
	{
		Message(text, "colorFriendly");
	}

	// -------------------------------------------------------------------------
	void MessageImportant(string text)
	{
		Message(text, "colorImportant");
	}

	void CloseInventoryMenu()
	{
		if (GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_CLIENT)
		{
#ifndef NO_GUI
			UIScriptedMenu menu = GetGame().GetUIManager().GetMenu();
			if (menu && (menu.GetID() == MENU_INVENTORY || menu.GetID() == MENU_INSPECT))
			{
				GetGame().GetUIManager().CloseAll();
				GetGame().GetMission().RemoveActiveInputExcludes({"inventory"},false);
				GetGame().GetMission().RemoveActiveInputRestriction(EInputRestrictors.INVENTORY);
			}
#endif
		}
	}
	
	// -------------------------------------------------------------------------
	/**
	/brief Delete all items in inventory
		\returns \p void
		@code
			PlayerBase player = GetGame().GetPlayer();
			player.ClearInventory();
		@endcode
	*/
	override void ClearInventory()
	{
		if ((GetGame().IsServer() || !GetGame().IsMultiplayer()) && GetInventory())
		{
			GameInventory inv = PlayerBase.Cast(this).GetInventory();
			array<EntityAI> items = new array<EntityAI>;
			inv.EnumerateInventory(InventoryTraversalType.INORDER, items);
			for (int i = 0; i < items.Count(); i++)
			{
				ItemBase item = ItemBase.Cast(items.Get(i));
				if (item)
				{
					GetGame().ObjectDelete(item);
				}
			}

			ItemBase item_in_hands = ItemBase.Cast(GetHumanInventory().GetEntityInHands());

			if (item_in_hands)
			{
				LocalDestroyEntityInHands();
			}
		}
	}
	
	//! Drops all clothes/wearables this character is carrying on themselves.
	void DropAllItems()
	{
		array<EntityAI> itemsArray = new array<EntityAI>;
		ItemBase item;
		GetInventory().EnumerateInventory(InventoryTraversalType.LEVELORDER, itemsArray);
		int count = itemsArray.Count();
		
		for (int i = 0; i < itemsArray.Count(); i++)
		{
			Class.CastTo(item, itemsArray.Get(i));
			
			if (item  &&  !item.IsInherited(SurvivorBase))	
			{
				ServerDropEntity(item);
			}
		}
	}

	string GetPlayerClass()
	{
		string type;
		GetGame().ObjectGetType(this, type);
		return type;
	}

	// --------------------------------------------------
	// Lifespan
	//---------------------------------------------------
	
	void ShavePlayer()
	{
		SetLastShavedSeconds(StatGet(AnalyticsManagerServer.STAT_PLAYTIME));

		m_ModuleLifespan.UpdateLifespan(this, true);
	}

	bool CanShave()
	{
		if (!HasCoveredFaceForShave() && GetLifeSpanState())
		{
			return true;
		}
		else
		{
			return false;			
		}
	}
	
	
	override void OnParticleEvent(string pEventType, string pUserString, int pUserInt)
	{
		super.OnParticleEvent(pEventType ,pUserString, pUserInt);
		
		if (!GetGame().IsDedicatedServer())
		{
			if (pUserInt == 123456) // 123456 is ID for vomiting effect. The current implementation is WIP.
			{
				PlayerBase player = PlayerBase.Cast(this);
				int boneIdx = player.GetBoneIndexByName("Head");
				
				if (boneIdx != -1)
				{
					EffectParticle eff;
				
					if (m_SyncedModifiers & eModifierSyncIDs.MODIFIER_SYNC_CONTAMINATION2)
					{
						eff = new EffVomitBlood();
					}
					else
					{
						eff = new EffVomit();
					}
					
					eff.SetDecalOwner(player);
					eff.SetAutodestroy(true);
					SEffectManager.PlayInWorld(eff, vector.Zero);
					Particle p = eff.GetParticle();
					player.AddChild(p, boneIdx);
				}
			}
		}
	}
	
	
	bool CanSpawnBreathVaporEffect()
	{
		if (!ToDelete() && IsAlive() && !IsSwimming() && !m_IsDrowning)
		{
			return true;
		}
		return false;
	}
	

	void ProcessADDModifier()
	{
		if (m_AddModifier != -1)
		{
			HumanCommandAdditives ad = GetCommandModifier_Additives();
			if (ad)
				ad.StartModifier(m_AddModifier);
			
			m_AddModifier = -1;
		}
	}
	
	void SpawnBreathVaporEffect()
	{
		//Print("SpawnBreathVaporEffect:"+GetGame().GetTime());
		int boneIdx = GetBoneIndexByName("Head");
		if (boneIdx != -1)
		{
			Particle p;
			switch (m_BreathVapour)
			{
				case 1:
					p = ParticleManager.GetInstance().PlayInWorld(ParticleList.BREATH_VAPOUR_LIGHT, "-0.03 0.15 0");
					break;
				case 2:
					p = ParticleManager.GetInstance().PlayInWorld(ParticleList.BREATH_VAPOUR_MEDIUM, "-0.03 0.15 0");
					break;
				case 3:
					p = ParticleManager.GetInstance().PlayInWorld(ParticleList.BREATH_VAPOUR_HEAVY, "-0.03 0.15 0");
					break;
				default:
					break;
			}
			
			if (p)
				AddChild(p, boneIdx);
		}
	}
	
	// returns 'true' if the player is submerged under water deep enough so that we consider him/her to be eligible for drowning, do note some other conditions may apply for actual drowning to be turned on
	bool GetDrowningWaterLevelCheck()
	{
		int index = GetBoneIndexByName("head");
		vector pos = GetBonePositionWS(index);
		float depth = g_Game.GetWaterDepth(pos);
		
		if (IsSwimming())
		{
			return depth > PlayerConstants.DROWNING_SWIMMING_THRESHOLD;
		}
		else if (IsUnconscious())
		{
			return depth > PlayerConstants.DROWNING_UNCONSCIOUS_THRESHOLD;
		}
		return depth > PlayerConstants.DROWNING_DEFAULT_THRESHOLD;
	}

	void SetLifeSpanStateVisible(int show_state)
	{
		bool state_changed;
		if (show_state != m_LifeSpanState)
			state_changed = true;
		m_LifeSpanState = show_state;
		SetSynchDirty();
		
		if (state_changed) //server only, client solution in OnVariablesSynchronized()
		{
			//SendLifespanSyncEvent(m_LifeSpanState);
			
			UpdateHairSelectionVisibility();
		}
	}
	
	int GetLifeSpanState()
	{
		return m_LifeSpanState;
	}
	
	int GetLastShavedSeconds()
	{
		return m_LastShavedSeconds;
	}

	void SetLastShavedSeconds(int last_shaved_seconds)
	{
		m_LastShavedSeconds = last_shaved_seconds;
	}
	
	bool HasCoveredFaceForShave()
	{
		return IsExclusionFlagPresent(GetFaceCoverageShaveValues());
	}
	
	//! returns a set of face covering values
	static set<int> GetFaceCoverageShaveValues()
	{
		set<int> ret = new set<int>;
		ret.Insert(EAttExclusions.SHAVING_MASK_ATT_0);
		ret.Insert(EAttExclusions.SHAVING_HEADGEAR_ATT_0);
		//ret.Insert(EAttExclusions.SHAVING_EYEWEAR_ATT_0);
		
		return ret;
	}
	
	eBloodyHandsTypes HasBloodyHandsEx()
	{
		return m_HasBloodyHandsVisible;
	}
	
	bool HasBloodyHands()
	{
		return m_HasBloodyHandsVisible;
	}

	void SetBloodyHands(bool show)
	{
		SetBloodyHandsBase(show);
	}
	
	void SetBloodyHandsEx(eBloodyHandsTypes type)
	{
		SetBloodyHandsBase(type);
	}
	
	private void SetBloodyHandsBase(int type)
	{
		m_HasBloodyHandsVisible = type;
		SetSynchDirty();
		
		#ifdef DIAG_DEVELOPER
		#ifndef SERVER
		if (IsControlledPlayer())
		{
			bool enable = type;
			DiagMenu.SetValue(DiagMenuIDs.LIFESPAN_BLOODY_HANDS, enable);
		}
		#endif
		#endif
	}
	
	int GetBloodyHandsPenaltyAgents()
	{
		if (HasBloodyHands())
		{
			if (GetInventory().FindAttachment(InventorySlots.GLOVES))
				return 0;

			float randomValue = Math.RandomFloat01();
			if (GetBloodyHandsPenaltyChancePerAgent(eAgents.SALMONELLA) >= randomValue)
				return eAgents.SALMONELLA;

			return eAgents.FOOD_POISON;
		}
		
		return 0;
	}
	
	float GetBloodyHandsPenaltyChancePerAgent(eAgents type)
	{
		float value = 0.0;
		if (m_BloodyHandsPenaltyChancePerAgent.Find(type, value))
			return value;
		
		return value;
	}
	
	//! increments the chances per agents
	void SetBloodyHandsPenaltyChancePerAgent(eAgents type, float amount)
	{
		float storedValue = GetBloodyHandsPenaltyChancePerAgent(type);
		if (amount > storedValue)
			m_BloodyHandsPenaltyChancePerAgent.Set(type, Math.Clamp(amount, 0.0, 1.0));
	}
	
	void ClearBloodyHandsPenaltyChancePerAgent(eAgents type)
	{
		m_BloodyHandsPenaltyChancePerAgent.Set(type, 0.0);
	}
	
	bool HasBloodTypeVisible()
	{
		return m_HasBloodTypeVisible;
	}
	
	void SetBloodTypeVisible(bool show)
	{
		m_HasBloodTypeVisible = show;
		SetSynchDirty();
	}
			
	int GetBloodType()
	{
		return m_BloodType;
	}
	
	void SetBloodType(int blood_type)
	{
		m_BloodType = blood_type;
		SetSynchDirty();
	}

	// --------------------------------------------------
	// Soft Skills
	//---------------------------------------------------
	
	SoftSkillsManager GetSoftSkillsManager()
	{
		return m_SoftSkillsManager;
	}
	
	void CheckDeath()
	{
		if (IsPlayerSelected() && !IsAlive())
		{
			SimulateDeath(true);
			m_DeathCheckTimer.Stop();
		}
	}
	
	// -------------------------------------------------------------------------
	
	// --------------------------------------------------
	// AI Presence
	//---------------------------------------------------
	
	//! Return actual noise presence of player
	int GetNoisePresenceInAI()
	{
		if (m_PresenceNotifier)
		{
			return m_PresenceNotifier.GetNoisePresence();
		}

		return 0;
	}

	// is a bit on in the persistent flags
	bool IsPersistentFlag(PersistentFlag bit)
	{
		return (m_PersistentFlags & bit);
	}
	// turn on/off a bit in the persistent flag
	void SetPersistentFlag(PersistentFlag bit, bool enable)
	{
		if (enable)//turn on bit
			m_PersistentFlags = (m_PersistentFlags | bit);
		else//turn off bit
			m_PersistentFlags = ((~bit) & m_PersistentFlags);
		
	}

	
	// -------------------------------------------------------------------------

	int GetStoreLoadVersion()
	{
		return m_StoreLoadVersion;
	}

	override void OnStoreSave(ParamsWriteContext ctx)
	{
		//Print("OnStoreSave");
		if (GetGame().SaveVersion() < 102)
		{
			ctx.Write(ACT_STORE_SAVE_VERSION);//to be removed after we push 102+
		}

		super.OnStoreSave(ctx);

		GetHumanInventory().OnStoreSave(ctx); // FSM of hands
		OnStoreSaveLifespan(ctx);

		if (GetDayZGame().IsServer() && GetDayZGame().IsMultiplayer())
		{
			GetPlayerStats().SaveStats(ctx);// save stats
			m_ModifiersManager.OnStoreSave(ctx);// save modifiers
			m_AgentPool.OnStoreSave(ctx);//save agents
			GetSymptomManager().OnStoreSave(ctx);//save states
			if (GetBleedingManagerServer())
			{
				GetBleedingManagerServer().OnStoreSave(ctx);//save bleeding sources
			}
			m_PlayerStomach.OnStoreSave(ctx);
			ctx.Write(GetBrokenLegs());
			//ctx.Write(m_LocalBrokenState);
			SaveAreaPersistenceFlag(ctx);

			HumanCommandLadder ladder = GetCommand_Ladder();
			if (ladder)
			{
				ctx.Write(true);
				ctx.Write(ladder.GetLogoutPosition());
			}
			else
			{
				ctx.Write(false);
			}
			
			ArrowManagerPlayer arrowManager = ArrowManagerPlayer.Cast(GetArrowManager());
			arrowManager.Save(ctx);
		}
	}

	
	void SaveAreaPersistenceFlag(ParamsWriteContext ctx)
	{
		if (GetModifiersManager())
			SetPersistentFlag(PersistentFlag.AREA_PRESENCE, GetModifiersManager().IsModifierActive(eModifiers.MDF_AREAEXPOSURE));//set the flag for player's presence in contaminated area
		ctx.Write(m_PersistentFlags);
	}
	
	
	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		//Print("---- PlayerBase OnStoreLoad START ----, version: "+version);
		m_aQuickBarLoad = new array<ref Param2<EntityAI,int>>;
		
		// todo :: this should be after base call !!!!
		if (version < 102)//write removed in v. 102
		{
			if (!ctx.Read(m_StoreLoadVersion))
				return false;
		}

		if (!super.OnStoreLoad(ctx, version))
			return false;

		// FSM of hands
		if (!GetHumanInventory().OnStoreLoad(ctx, version))
			return false;

		if (!OnStoreLoadLifespan(ctx, version))
			return false;

		if (GetDayZGame().IsServer() && GetDayZGame().IsMultiplayer())
		{
			if (!GetPlayerStats().LoadStats(ctx, version)) // load stats
			{
				Print("---- failed to load PlayerStats  ----");
				return false;
			}
			
			if (!m_ModifiersManager.OnStoreLoad(ctx, version))
			{
				Print("---- failed to load ModifiersManager, read fail  ----");
				return false;
			}
			
			if (!m_AgentPool.OnStoreLoad(ctx, version))
			{
				Print("---- failed to load AgentPool, read fail  ----");
				return false;
			}
			
			if (!GetSymptomManager().OnStoreLoad(ctx, version))
			{
				Print("---- failed to load SymptomManager, read fail  ----");
				return false;
			}
			
			if (!GetBleedingManagerServer().OnStoreLoad(ctx, version))
			{
				Print("---- failed to load BleedingManagerServer, read fail  ----");
				return false;
			}
			
			if (!m_PlayerStomach.OnStoreLoad(ctx, version))
			{
				Print("---- failed to load PlayerStomach, read fail  ----");
				return false;
			}
			
			//Check for broken leg value
			if (version >= 116)
			{
				if (!ctx.Read(m_BrokenLegState))
				{
					return false;
				}
				if (version <= 126)// WHILE >= 116
				{
					if (!ctx.Read(m_LocalBrokenState))
					{
						return false;
					}
				}
			}
			//Load persistent flags value
			if (version >= 125 && (!ctx.Read(m_PersistentFlags)))
			{
				Print("---- failed to load Persistent Flags, read fail  ----");
				return false;
			}

			if (version >= 131)
			{
				bool onLadder;
				if (!ctx.Read(onLadder))
				{
					return false;
				}

				if (onLadder)
				{
					vector position;
					if (!ctx.Read(position))
					{
						return false;
					}

					Hive hive = GetHive();
					if (!hive || !hive.CharacterIsLoginPositionChanged(this))
					{
						SetPosition(position);
					}
				}
			}
		
			if (version >= 134)
			{
				ArrowManagerPlayer arrowManager = ArrowManagerPlayer.Cast(GetArrowManager());
				arrowManager.Load(ctx);
			}
		}
		
		Print("---- PlayerBase OnStoreLoad SUCCESS ----");
		return true;
	}

	override void AfterStoreLoad()
	{
		GetHumanInventory().OnAfterStoreLoad();
		if (m_EmoteManager)
			m_EmoteManager.AfterStoreLoad();
		
		if (GetPlayerStats())
			GetPlayerStats().OnAfterStoreLoad();
		
		PlayerRestrictedAreaInstance pra;
		vector currentPos = GetPosition();
		if (g_Game.GetGameState() != DayZGameState.MAIN_MENU && CfgPlayerRestrictedAreaHandler.IsInitialized() && CfgPlayerRestrictedAreaHandler.IsPointInPlayerRestrictedArea(currentPos,pra))
		{
			//vector safePos = pra.GetClosestSafePos3D(currentPos);
			vector safePos = pra.GetRandomSafePos3D(currentPos);
			if (MiscGameplayFunctions.TeleportPlayerToSafeLocation3D(this,safePos) && m_AdminLog)
				m_AdminLog.PlayerTeleportedLog(this,currentPos,safePos,"Spawning in Player Restricted Area: " + pra.areaName);
		}
	}

	void OnStoreSaveLifespan(ParamsWriteContext ctx)
	{		
		ctx.Write(m_LifeSpanState);	
		ctx.Write(m_LastShavedSeconds);	
		ctx.Write(m_HasBloodyHandsVisible);
		ctx.Write(m_HasBloodTypeVisible);
		ctx.Write(m_BloodType);
	}

	bool OnStoreLoadLifespan(ParamsReadContext ctx, int version)
	{	
		int lifespan_state = 0;
		if (!ctx.Read(lifespan_state))
			return false;
		m_LifeSpanState = lifespan_state;
		
		int last_shaved = 0;
		if (!ctx.Read(last_shaved))
			return false;
		m_LastShavedSeconds = last_shaved;
		
		if (version < 122)
		{
			bool bloody_hands_old;
			if (!ctx.Read(bloody_hands_old))
			return false;
			m_HasBloodyHandsVisible = bloody_hands_old;
		}
		else
		{
			int bloody_hands = 0;
			if (!ctx.Read(bloody_hands))
				return false;
			m_HasBloodyHandsVisible = bloody_hands;
		}
		
		
		bool blood_visible = false;
		if (!ctx.Read(blood_visible))
			return false;
		m_HasBloodTypeVisible = blood_visible;
		
		int blood_type = 0;
		if (!ctx.Read(blood_type))
			return false;
		m_BloodType = blood_type;

		return true;
	}
	
	vector m_PlayerOldPos;
	void UpdatePlayerMeasures()
	{
		int hour, minute, second;
		GetHourMinuteSecond(hour, minute, second);
		float distance;
		distance = StatGet(AnalyticsManagerServer.STAT_DISTANCE);
		if (m_AnalyticsTimer)
		{
			StatsEventMeasuresData data = new StatsEventMeasuresData();
			data.m_CharacterId = g_Game.GetDatabaseID();
			data.m_TimeInterval = m_AnalyticsTimer.GetRemaining();
			data.m_DaytimeHour = hour;
			data.m_PositionStart = m_PlayerOldPos;
			data.m_PositionEnd = GetPosition();
			data.m_DistanceOnFoot = distance;
			Analytics.PlayerMeasures(data);
		}

		m_PlayerOldPos = GetPosition();
	}

	void OnConnect()
	{
		Debug.Log("Player connected:"+this.ToString(),"Connect");

		// analytics
		GetGame().GetAnalyticsServer().OnPlayerConnect(this);
		
		m_PlayerOldPos = GetPosition();
		if (m_AnalyticsTimer)
			m_AnalyticsTimer.Run(60, this, "UpdatePlayerMeasures", null, true);
		
		//construction action data
		ResetConstructionActionData();
	}
	
	void OnReconnect()
	{
		Debug.Log("Player reconnected:"+this.ToString(),"Reconnect");
		
		//construction action data
		
		ResetConstructionActionData();
	}
	
	void OnDisconnect()
	{
		Debug.Log("Player disconnected:"+this.ToString(),"Connect");
		
		// analytics
		// force update of the stats
		// if player disconnect too soon, UpdatePlayersStats() is not called
		GetGame().GetAnalyticsServer().OnPlayerDisconnect(this);
		
		StatsEventDisconnectedData data = new StatsEventDisconnectedData();
		data.m_CharacterId = g_Game.GetDatabaseID();
		data.m_Reason = "Disconnected";
		Analytics.PlayerDisconnected(data);
		
		if (m_AnalyticsTimer)
			m_AnalyticsTimer.Stop();
		UpdatePlayerMeasures();
		
		SetPlayerDisconnected(true);
	}

	void SetModifiers(bool enable)
	{
		GetModifiersManager().SetModifiers(enable);
	}
	
	bool Consume(PlayerConsumeData data)
	{
		PluginTransmissionAgents plugin = PluginTransmissionAgents.Cast(GetPlugin(PluginTransmissionAgents));
		int pollution = GetGame().GetMission().GetWorldData().GetPollution();
		float temperature = 0;
		
		if (data.m_Type == EConsumeType.ENVIRO_POND || data.m_Type == EConsumeType.ENVIRO_WELL || data.m_Type == EConsumeType.ENVIRO_SNOW)
		{
			if (data.m_Type != EConsumeType.ENVIRO_WELL)
			{
				if (pollution & EPollution.HEAVYMETAL)
					data.m_Agents = data.m_Agents | eAgents.HEAVYMETAL;

				if (data.m_Type == EConsumeType.ENVIRO_POND)
					data.m_Agents = data.m_Agents | eAgents.CHOLERA;
			}

			temperature = GetGame().GetMission().GetWorldData().GetLiquidTypeEnviroTemperature(data.m_LiquidType);
			m_PlayerStomach.AddToStomach(Liquid.GetLiquidClassname(LIQUID_WATER), data.m_Amount, 0 , data.m_Agents, temperature);
			
			return true;
		}
		
		Edible_Base edibleItem = Edible_Base.Cast(data.m_Source);
		if (!edibleItem || !edibleItem.CanBeConsumed())
			return false;

		if (data.m_Type == EConsumeType.ITEM_SINGLE_TIME || data.m_Type == EConsumeType.ITEM_CONTINUOUS)
		{
			data.m_Agents = edibleItem.FilterAgents(data.m_Agents | edibleItem.GetAgents());
			temperature = edibleItem.GetTemperature();

			if (data.m_Type == EConsumeType.ITEM_SINGLE_TIME)
				plugin.TransmitAgents(edibleItem, this, AGT_UACTION_CONSUME, data.m_Amount);

			if (edibleItem.IsLiquidContainer())
			{
				int liquidType = edibleItem.GetLiquidType();
				string liquidClassName = Liquid.GetLiquidClassname(liquidType);
				if (liquidClassName.Length() == 0)
					Error("Error! Trying to add unknown liquid to stomach with item=" + Object.GetDebugName(edibleItem) + " data.m_Type=" + data.m_Type + " liquid_type=" + liquidType);

				m_PlayerStomach.AddToStomach(liquidClassName, data.m_Amount, 0, data.m_Agents, temperature);
			}
			else
			{
				int foodStageType;
				if (edibleItem.GetFoodStage())
					foodStageType = edibleItem.GetFoodStage().GetFoodStageType();

				m_PlayerStomach.AddToStomach(data.m_Source.GetType(), data.m_Amount, foodStageType, data.m_Agents, temperature);
			}

			edibleItem.Consume(data.m_Amount, this);

			return true;

		}

		return false;	
	}

	void ShowUnconsciousScreen(bool show)
	{
	#ifndef NO_GUI
		if (show)
		{
			GetGame().GetUIManager().ScreenFadeIn(0, "You are Unconscious", FadeColors.BLACK, FadeColors.WHITE);
			PrintString("Fade in");
		}
		else
		{
			GetGame().GetUIManager().ScreenFadeOut(0);
			PrintString("Fade out");
		}
	#endif
	}
	
	override void RequestSoundEventEx(EPlayerSoundEventID id, bool from_server_and_client = false, int param = 0)
	{
		if (from_server_and_client && GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_CLIENT)
		{
			PlaySoundEventEx(id, false, false, param);
			return;
		}
		SendSoundEventEx(id, param);
	}
	
	override void RequestSoundEvent(EPlayerSoundEventID id, bool from_server_and_client = false)
	{
		RequestSoundEventEx(id, from_server_and_client);
	}

	override protected void SendSoundEvent(EPlayerSoundEventID id)
	{
		SendSoundEventEx(id);
	}
	
	override protected void SendSoundEventEx(EPlayerSoundEventID id, int param = 0)
	{
		if (!GetGame().IsServer())
		{
			return;
		}
		m_SoundEvent = id;
		m_SoundEventParam = param;
		SetSynchDirty();
		
		if (!GetGame().IsMultiplayer()) 
		{
			CheckSoundEvent();
		}
		
		//PrintString(GetGame().GetTime().ToString() + " Set SoundEvent, id:" + id.ToString());
	}
	
	void CheckZeroSoundEvent()
	{
		if (m_SoundEvent != 0 && m_SoundEventSent)
		{
			m_SoundEvent = 0;
			m_SoundEventParam = 0;
			m_SoundEventSent = false;
			SetSynchDirty();
		}
	}
	
	void CheckSendSoundEvent()
	{
		if (m_SoundEvent!= 0 && !m_SoundEventSent)
		{
			m_SoundEventSent = true;
		}
	}
	
	override bool PlaySoundEventType(ESoundEventType soundType, int soundEventID, int param = 0)
	{
		if (m_ReplaceSoundEventHandler)
			return m_ReplaceSoundEventHandler.PlayReplaceSound(soundEventID, soundType, param);
		
		return false;
	}
	
	override bool PlaySoundEvent(EPlayerSoundEventID id, bool from_anim_system = false, bool is_from_server = false)
	{
		return PlaySoundEventEx(id, from_anim_system, is_from_server);
	}
	
	override bool PlaySoundEventEx(EPlayerSoundEventID id, bool from_anim_system = false, bool is_from_server = false, int param = 0)
	{
		if (!m_PlayerSoundEventHandler)
			return false;

		return m_PlayerSoundEventHandler.PlayRequestEx(id, is_from_server, param);
	}
	
	PlayerSoundEventHandler GetPlayerSoundEventHandler()
	{
		return m_PlayerSoundEventHandler;
	}
	
	void OnBleedingBegin();
	void OnBleedingEnd();
	
	void OnBleedingSourceAdded()
	{
		m_BleedingSourceCount++;
		if (IsControlledPlayer())
		{
			if (!GetGame().IsDedicatedServer())
			{
				DisplayElementBadge dis_elm = DisplayElementBadge.Cast(GetVirtualHud().GetElement(eDisplayElements.DELM_BADGE_BLEEDING));
				if (dis_elm)
				{
					dis_elm.SetValue(GetBleedingSourceCount());
				}
				
				
				//Print("----------bleeding_SoundSet----------");
				SEffectManager.PlaySoundOnObject("bleeding_SoundSet", this);
			}
		}
	}
	
	void OnBleedingSourceRemoved()
	{
		m_BleedingSourceCount--;
		if (GetGame().IsServer())
		{
			ArrowManagerBase arrowManager = GetArrowManager();
			if (GetBleedingSourceCount() > 0)
			{
				arrowManager.DropFirstArrow();
			}
			else
			{
				arrowManager.DropAllArrows();
			}
		}
		
		if (IsControlledPlayer())
		{
			if (!GetGame().IsDedicatedServer())
			{
				//Print("GetBleedingSourceCount() "+GetBleedingSourceCount());
				DisplayElementBadge dis_elm = DisplayElementBadge.Cast(GetVirtualHud().GetElement(eDisplayElements.DELM_BADGE_BLEEDING));
				if (dis_elm)
				{
					dis_elm.SetValue(GetBleedingSourceCount());
				}
			}
		}
	}
	
	
	void OnBleedingSourceRemovedEx(ItemBase item)
	{
		OnBleedingSourceRemoved();
	}
	
	int GetBleedingSourceCount()
	{
		return m_BleedingSourceCount;
	}

	// for debug purposes, should reset some systems like Modifiers, Stats, Damage etc.....
	void ResetPlayer(bool set_max)
	{
		#ifdef DIAG_DEVELOPER
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
			GetStomach().ClearContents();

			DamageSystem.ResetAllZones(this);
			GetModifiersManager().ResetAll();
			
			// bleeding sources
			if (m_BleedingManagerServer)
				m_BleedingManagerServer.RemoveAllSources();
			
			// Stats
			if (GetPlayerStats())
			{
				int bloodType = GetStatBloodType().Get();
				GetPlayerStats().ResetAllStats();
				GetStatBloodType().Set(bloodType);
			}

			// Agents
			if (m_AgentPool)
				m_AgentPool.RemoveAllAgents();
			
			if (m_StaminaHandler)
				m_StaminaHandler.SetStamina(GameConstants.STAMINA_MAX);
			
			// uncon
			if (IsUnconscious())
				DayZPlayerSyncJunctures.SendPlayerUnconsciousness(this, false);
			
			// set max
			if (set_max)
			{
				GetStatWater().Set(GetStatWater().GetMax());
				GetStatEnergy().Set(GetStatEnergy().GetMax());
			}
			
			// fix up inventory
			FixAllInventoryItems();
			
			//remove bloody hands
			PluginLifespan moduleLifespan = PluginLifespan.Cast(GetPlugin(PluginLifespan));
			moduleLifespan.UpdateBloodyHandsVisibilityEx(this, eBloodyHandsTypes.CLEAN);
			ClearBloodyHandsPenaltyChancePerAgent(eAgents.SALMONELLA);
			
			if (GetArrowManager())
				GetArrowManager().ClearArrows();

		}

		// client + single + server
		HumanCommandVehicle vehCmd = GetCommand_Vehicle();
		if (vehCmd)
		{
			Transport transport = vehCmd.GetTransport();
			if (transport)
				transport.FixEntity();
		}
		#endif
	}
	
	void CheckSoundEvent()
	{
		if (m_SoundEvent != 0)
		{
			PlaySoundEventEx(m_SoundEvent, false, true,m_SoundEventParam);
			m_SoundEvent = 0;
			m_SoundEventParam = 0;
		}
		
		// cancelling marked interrupted sounds
		if (m_PerformedAnimActionID == -1)
		{
			if (m_PerformedActionSounds.Count() == 0)
				return;
			
			foreach (AbstractWave sound : m_PerformedActionSounds)
			{
				if (sound)
					sound.Stop();
			}
			
			m_PerformedActionSounds.Clear();
		}
	}
	
	void TestSend()
	{
		if (GetGame().IsClient()) return;
		RequestSoundEvent(1234);
		//Math.RandomInt(1,4096)
	}
	
	void SetStaminaState(eStaminaState state)
	{
		if (state != m_StaminaState)
		{
			m_StaminaState = state;
			//PrintString("m_StaminaState:"+m_StaminaState.ToString());
			SetSynchDirty();
		}
	}
	
	int GetStaminaState()
	{
		return m_StaminaState;
	}
	
	void UpdateQuickBarExtraSlots()
	{
		m_QuickBarBase.updateSlotsCount();
	}

	bool Save()
	{
		// saved player must be alive and not captive
		if (GetPlayerState() == EPlayerStates.ALIVE && !IsRestrained())
		{
			GetHive().CharacterSave(this);
			Debug.Log("Player "+this.ToString()+ " saved as alive");
			
			return true;
		}
		return false;		
	}

	// agent transfer
	//! chance between [0..1] , distance in meters
	void SpreadAgentsEx(float distance = 3,float chance = 0.25)
	{
		if (Math.RandomFloat01() > chance)
			return;
		
		GetGame().GetPlayers(m_ServerPlayers);
		float dist_check = distance * distance;//make it sq
		PluginTransmissionAgents plugin = PluginTransmissionAgents.Cast(GetPlugin(PluginTransmissionAgents));
		
		foreach (Man target: m_ServerPlayers)
		{
			if (vector.DistanceSq(GetWorldPosition(), target.GetWorldPosition()) < dist_check && target != this)
			{
				plugin.TransmitAgents(this, target, AGT_AIRBOURNE_BIOLOGICAL, 1);
			}
		}
	}
	
	void SpreadAgents()//legacy method
	{
		SpreadAgentsEx(3,1);
	}

	//--------------------------------------------------------------------------------------------
	override int GetAgents()
	{
		return m_AgentPool.GetAgents();
	}

	//--------------------------------------------------------------------------------------------
	override void RemoveAgent(int agent_id) //removes a single agent type from the player agent pool
	{
		m_AgentPool.RemoveAgent(agent_id);
	}

	//--------------------------------------------------------------------------------------------
	override void RemoveAllAgents()
	{
		m_AgentPool.RemoveAllAgents();
	}


	//--------------------------------------------------------------------------------------------
	override void InsertAgent(int agent, float count = 1) //adds a single agent type to the player agent pool
	{
		m_AgentPool.AddAgent(agent,count);
	}

	//--------------------------------------------------------------------------------------------
	int GetSingleAgentCount(int agent_id)
	{
		return m_AgentPool.GetSingleAgentCount(agent_id);
	}

	//--------------------------------------------------------------------------------------------
	float GetSingleAgentCountNormalized(int agent_id)
	{
		int max_count = PluginTransmissionAgents.GetAgentMaxCount(agent_id);
		return m_AgentPool.GetSingleAgentCount(agent_id) / max_count;
	}

	float GetTotalAgentCount()
	{
		return m_AgentPool.GetTotalAgentCount();
	}

	void PrintAgents()
	{
		m_AgentPool.PrintAgents();
	}

	void ImmuneSystemTick(float value, float deltaT)
	{
		m_AgentPool.ImmuneSystemTick(value, deltaT);
	}
	
	void AntibioticsAttack(float value)
	{
		m_AgentPool.DrugsAttack(EMedicalDrugsType.ANTIBIOTICS, value);
	}
	
	void SetTemporaryResistanceToAgent(int agent, float time)
	{
		m_AgentPool.SetTemporaryResistance(agent, time);
	}
	
	float GetTemporaryResistanceToAgent(int agent)
	{
		return m_AgentPool.GetTemporaryResistance(agent);
	}

	//Get aim (player crosshair) position
	vector GetAimPosition()
	{
		float min_distance = 0.5;		//min distance, default = 5m
		
		vector from = GetGame().GetCurrentCameraPosition();
		vector to = from + (GetGame().GetCurrentCameraDirection() * min_distance);
		vector contactPos;
		vector contactDir;
		int contactComponent;
		
		DayZPhysics.RaycastRV(from, to, contactPos, contactDir, contactComponent, NULL, NULL, NULL, false, true);
		
		return contactPos;
	}

	
	bool CanEatAndDrink()
	{
		ItemBase mask = ItemBase.Cast(GetInventory().FindAttachment(InventorySlots.MASK));
		return (!mask || (mask && mask.AllowFoodConsumption()));
	}
	
	bool CanConsumeFood(ConsumeConditionData data = null)
	{
		return true;
	}
	
	//get modifier manager
	ModifiersManager GetModifiersManager()
	{
		return m_ModifiersManager;
	}

	PlayerStat<float> GetStatWater()
	{
		if (!m_StatWater && GetPlayerStats())
		{
			m_StatWater = PlayerStat<float>.Cast(GetPlayerStats().GetStatObject(EPlayerStats_current.WATER));
		}
		return m_StatWater;
	}
	
	PlayerStat<float> GetStatToxicity()
	{
		if (!m_StatToxicity && GetPlayerStats())
		{
			m_StatToxicity = PlayerStat<float>.Cast(GetPlayerStats().GetStatObject(EPlayerStats_current.TOXICITY));
		}
		return m_StatToxicity;
	}

	PlayerStat<float> GetStatEnergy()
	{
		if (!m_StatEnergy && GetPlayerStats()) 
		{
			m_StatEnergy = PlayerStat<float>.Cast(GetPlayerStats().GetStatObject(EPlayerStats_current.ENERGY));
		}
		return m_StatEnergy;
	}
	
	PlayerStat<float> GetStatHeatComfort()
	{
		if (!m_StatHeatComfort && GetPlayerStats()) 
		{
			m_StatHeatComfort = PlayerStat<float>.Cast(GetPlayerStats().GetStatObject(EPlayerStats_current.HEATCOMFORT));
		}
		return m_StatHeatComfort;
	}
	
	PlayerStat<float> GetStatTremor()
	{
		if (!m_StatTremor && GetPlayerStats()) 
		{
			m_StatTremor = PlayerStat<float>.Cast(GetPlayerStats().GetStatObject(EPlayerStats_current.TREMOR));
		}
		return m_StatTremor;
	}
	
	PlayerStat<int> GetStatWet()
	{
		if (!m_StatWet && GetPlayerStats()) 
		{
			m_StatWet = PlayerStat<int>.Cast(GetPlayerStats().GetStatObject(EPlayerStats_current.WET));
		}
		return m_StatWet;
	}
	
	PlayerStat<float> GetStatDiet()
	{
		if (!m_StatDiet && GetPlayerStats()) 
		{
			m_StatDiet = PlayerStat<float>.Cast(GetPlayerStats().GetStatObject(EPlayerStats_current.DIET));
		}
		return m_StatDiet;
	}
	
	PlayerStat<float> GetStatStamina()
	{
		if (!m_StatStamina && GetPlayerStats()) 
		{
			m_StatStamina = PlayerStat<float>.Cast(GetPlayerStats().GetStatObject(EPlayerStats_current.STAMINA));
		}
		return m_StatStamina;
	}
		
	PlayerStat<float> GetStatSpecialty()
	{
		if (!m_StatSpecialty && GetPlayerStats()) 
		{
			m_StatSpecialty = PlayerStat<float>.Cast(GetPlayerStats().GetStatObject(EPlayerStats_current.SPECIALTY));
		}
		return m_StatSpecialty;
	}
	
	PlayerStat<int> GetStatBloodType()
	{
		if (!m_StatBloodType && GetPlayerStats()) 
		{
			m_StatBloodType = PlayerStat<int>.Cast(GetPlayerStats().GetStatObject(EPlayerStats_current.BLOODTYPE));
		}
		return m_StatBloodType;
	}
	
	PlayerStat<float> GetStatHeatBuffer()
	{
		if (!m_StatHeatBuffer && GetPlayerStats())
		{
			m_StatHeatBuffer = PlayerStat<float>.Cast(GetPlayerStats().GetStatObject(EPlayerStats_current.HEATBUFFER));
		}
		return m_StatHeatBuffer;
	}
	
	void SetHeatBufferDynamicMax(float value)
	{
		m_HeatBufferDynamicMax = value;
		SetSynchDirty();
	}
	
	float GetHeatBufferDynamicMax()
	{
		return m_HeatBufferDynamicMax;
	}
	
	void ToggleHeatBufferVisibility(bool show)
	{
		m_HasHeatBuffer = show;
		SetSynchDirty();
	}
	
	void ToggleHeatBufferVisibility(int heatbufferStage)
	{
		m_HeatBufferStage = heatbufferStage;
		SetSynchDirty();
	}
	
	//! UA Last Message
	string GetLastUAMessage()
	{
		return m_UALastMessage;
	}

	void SetLastUAMessage(string pMsg)
	{
		m_UALastMessage = pMsg;

		if (m_UALastMessageTimer.IsRunning())
		{
			m_UALastMessageTimer.Stop();
		}
		
		m_UALastMessageTimer.Run(PlayerConstants.LAST_UA_MSG_LIFETIME, this, "ClearLastUAMessage", null);
	}

	protected void ClearLastUAMessage()
	{
		if (m_UALastMessageTimer.IsRunning())
		{
			m_UALastMessageTimer.Stop();
		}
		
		m_UALastMessage = "";
	}

	//! -------------------------------------------------------------------------
	//! Injured stat
	
	//! update injured state immediately
	void ForceUpdateInjuredState()
	{
		m_InjuryHandler.CheckValue(true);
	}
	
	override bool OnAction(int action_id, Man player, ParamsReadContext ctx)
	{
#ifdef DIAG_DEVELOPER
		if (m_Bot && action_id > EActions.PLAYER_BOT_INTERNAL_START && action_id < EActions.PLAYER_BOT_END)
		{
			m_Bot.StartAction(action_id);
			return true;
		}
#endif
		
		if (super.OnAction(action_id, player, ctx))
			return true;

		if (GetGame().IsClient() || !GetGame().IsMultiplayer())
		{
			switch (action_id)
			{
				case EActions.GIZMO_OBJECT:
					GetGame().GizmoSelectObject(this);
					return true;
				case EActions.GIZMO_PHYSICS:
					GetGame().GizmoSelectPhysics(GetPhysics());
					return true;
			}
		}
	
		if (GetGame().IsServer())
		{
			switch (action_id)
			{
				case EActions.DELETE:
					Delete();
					return true;
			}
		}

		return false;
	}
	
	// -------------------------------------------------------------------------
	override void GetDebugActions(out TSelectableActionInfoArrayEx outputList)
	{
		int i;
		
		PluginTransmissionAgents pluginTransmissionAgents = PluginTransmissionAgents.Cast(GetPlugin(PluginTransmissionAgents));
		
#ifdef DIAG_DEVELOPER
		if (pluginTransmissionAgents && !(m_Bot || GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_AI_REMOTE))
#else
		if (pluginTransmissionAgents && !(GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_AI_REMOTE))
#endif
		{
			map<int, string> agentList = pluginTransmissionAgents.GetSimpleAgentList();
			
			if (agentList)
			{
				foreach (int tid, string tname : agentList)
				{				
					string injectName = "Inject " + tname;
					string removeName = "Remove " + tname;
					
					outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.DEBUG_AGENTS_RANGE_INJECT_START + tid, injectName, Colors.WHITE));
					outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.DEBUG_AGENTS_RANGE_REMOVE_START + tid, removeName, Colors.WHITE));
				}
			}

			outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.SEPARATOR, "___________________________", FadeColors.RED));
		}
		
#ifdef DIAG_DEVELOPER
		if (m_Bot || GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_AI_REMOTE)
		{
			typename e = EActions;
			
			int cnt = e.GetVariableCount();
			int val;
	
			for (i = 0; i < cnt; i++)
			{
				if (!e.GetVariableValue(null, i, val))
					continue;
				
				if (val <= EActions.PLAYER_BOT_INTERNAL_START)
					continue;
				
				if (val == EActions.PLAYER_BOT_START)
				{
					outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.SEPARATOR, "___________________________", FadeColors.RED));
					continue;
				}
				
				if (val >= EActions.PLAYER_BOT_END)
					break;
				
				string name = e.GetVariableName(i);
				
				outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, val, name, FadeColors.LIGHT_GREY));
			}

			outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.SEPARATOR, "___________________________", FadeColors.RED));
		}
#endif

		super.GetDebugActions(outputList);

		if (Gizmo_IsSupported())
			outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.GIZMO_OBJECT, "Gizmo Object", FadeColors.LIGHT_GREY));
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.GIZMO_PHYSICS, "Gizmo Physics (SP Only)", FadeColors.LIGHT_GREY)); // intentionally allowed for testing physics desync
		if (GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_AI_SINGLEPLAYER) // Prevent deleting ourselves
		{
			outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.DELETE, "Delete", FadeColors.RED));
		}
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.SEPARATOR, "___________________________", FadeColors.RED));
	}		

	//-------------------------------------------------------------
	//!
	//! SyncJunctures
	//! 

	override void OnSyncJuncture(int pJunctureID, ParamsReadContext pCtx)
	{
		super.OnSyncJuncture(pJunctureID, pCtx);
			
		switch(pJunctureID)
		{
			case DayZPlayerSyncJunctures.SJ_INJURY:
				eInjuryHandlerLevels level;
				bool enable;
				DayZPlayerSyncJunctures.ReadInjuryParams(pCtx, enable, level);
				m_InjuryHandler.SetInjuryCommandParams(enable, level);
				break;
			case DayZPlayerSyncJunctures.SJ_PLAYER_STATES:
				GetSymptomManager().SetAnimation(pCtx);
				break;
			case DayZPlayerSyncJunctures.SJ_QUICKBAR_SET_SHORTCUT:		
				OnQuickbarSetEntityRequest(pCtx);
				break;
			case DayZPlayerSyncJunctures.SJ_INVENTORY:
				GetInventory().OnInventoryJunctureFromServer(pCtx);
				break;
			case DayZPlayerSyncJunctures.SJ_INVENTORY_REPAIR:
				GetInventory().OnInventoryJunctureRepairFromServer(pCtx);
				break;
			case DayZPlayerSyncJunctures.SJ_INVENTORY_FAILURE:
				GetInventory().OnInventoryJunctureFailureFromServer(pCtx);
				break;
			case DayZPlayerSyncJunctures.SJ_ACTION_INTERRUPT:
			case DayZPlayerSyncJunctures.SJ_ACTION_ACK_ACCEPT:
			case DayZPlayerSyncJunctures.SJ_ACTION_ACK_REJECT:
				m_ActionManager.OnSyncJuncture(pJunctureID,pCtx);
				break;
			case DayZPlayerSyncJunctures.SJ_WEAPON_ACTION_ACK_ACCEPT:
			case DayZPlayerSyncJunctures.SJ_WEAPON_ACTION_ACK_REJECT:
			case DayZPlayerSyncJunctures.SJ_WEAPON_SET_JAMMING_CHANCE:
				m_WeaponManager.OnSyncJuncture(pJunctureID,pCtx);
				break;
			case DayZPlayerSyncJunctures.SJ_UNCONSCIOUSNESS:
				DayZPlayerSyncJunctures.ReadPlayerUnconsciousnessParams(pCtx, m_ShouldBeUnconscious);
				break;
			case DayZPlayerSyncJunctures.SJ_PLAYER_ADD_MODIFIER:
				GetSymptomManager().SetAnimation(pCtx);
				break;
			case DayZPlayerSyncJunctures.SJ_KURU_REQUEST:
				float amount;
				if (DayZPlayerSyncJunctures.ReadKuruRequest(pCtx, amount))
				{
					if (GetAimingModel() && IsFireWeaponRaised()) 
					{
						GetAimingModel().RequestKuruShake(amount);
					}
				}
				break;
			case DayZPlayerSyncJunctures.SJ_GESTURE_REQUEST :
				m_EmoteManager.OnSyncJuncture(pJunctureID, pCtx);
				break;
			case DayZPlayerSyncJunctures.SJ_WEAPON_LIFT: // Obsolete
				SetLiftWeapon(pJunctureID, pCtx);
				break;
			case DayZPlayerSyncJunctures.SJ_ADS_RESET:
				m_ResetADS = true;
				break;
			case DayZPlayerSyncJunctures.SJ_DELETE_ITEM:
				SetToDelete(pCtx);
				break;
			case DayZPlayerSyncJunctures.SJ_BROKEN_LEGS:
				m_BrokenLegsJunctureReceived = true;
				DayZPlayerSyncJunctures.ReadBrokenLegsParamsEx(pCtx, m_BrokenLegState);
			break;
			case DayZPlayerSyncJunctures.SJ_SHOCK :
				DayZPlayerSyncJunctures.ReadShockParams(pCtx, m_CurrentShock);
			break;
			case DayZPlayerSyncJunctures.SJ_STAMINA:
				m_StaminaHandler.OnSyncJuncture(pJunctureID, pCtx);
			break;
			case DayZPlayerSyncJunctures.SJ_STAMINA_MISC:
				m_StaminaHandler.OnSyncJuncture(pJunctureID, pCtx);
			break;
			#ifdef DEVELOPER
			case DayZPlayerSyncJunctures.SJ_DEBUG_GET_IN_VEHICLE:
				EntityAI vehicle;
				DayZPlayerSyncJunctures.ReadGetInVehicleParams(pCtx, vehicle);
				SetGetInVehicleDebug(vehicle);
			break;
			#endif
		}
	}
	
	bool IsItemsToDelete()
	{
		return m_ItemsToDelete.Count() > 0;
	}
	
	void SetToDelete(ParamsReadContext pCtx)
	{
		EntityAI item; 
		pCtx.Read(item);
		AddItemToDelete(item);
	}
	
	override void AddItemToDelete(EntityAI item)
	{
		if (item)
		{
			item.SetPrepareToDelete();
			m_ItemsToDelete.Insert(item);
		}
	}
	
	bool CanDeleteItems()
	{
		return !(GetThrowing().IsThrowingAnimationPlaying() || GetDayZPlayerInventory().IsProcessing() || (GetActionManager() && GetActionManager().GetRunningAction()));
	}
	
	override void JunctureDeleteItem(EntityAI item)
	{
		DayZPlayerSyncJunctures.SendDeleteItem(this, item);
	} 
	
	void UpdateDelete()
	{
		int count = m_ItemsToDelete.Count();
		if (count > 0)
		{
			if (CanDeleteItems())
			{
				EntityAI itemToDelete;
				
				if (GetGame().IsClient() && GetGame().IsMultiplayer())
				{
					InventoryLocation il = new InventoryLocation();
					for (int i = count - 1; i >= 0 ; i--)
					{
						itemToDelete = m_ItemsToDelete.Get(i);
						if (itemToDelete != null)
						{
							itemToDelete.GetInventory().GetCurrentInventoryLocation(il);
						}
						
						if (itemToDelete == null || (GetItemInHands() == null && il.GetType() == InventoryLocationType.UNKNOWN))
						{
							m_ItemsToDelete.Remove(i);
						}
					}
				}
				else
				{
					for (int j = count - 1; j >= 0 ; j--)
					{
						itemToDelete = m_ItemsToDelete.Get(j);
						if (itemToDelete == null)
						{
							m_ItemsToDelete.Remove(j);
						}
						else
						{
							itemToDelete.Delete();
							m_ItemsToDelete.Remove(j);
						}
					}
				}
			}
		}
	}
	
	override bool	HeadingModel(float pDt, SDayZPlayerHeadingModel pModel)
	{
		if (GetActionManager()) // disable character turning while performing actions
		{
			ActionContinuousBase action = ActionContinuousBase.Cast(GetActionManager().GetRunningAction());
			if (action && action.IsFullBody(this) && action.IsCameraLockOnPerform())
			{				
				if (!m_IsHeadingRestricted)
				{
					m_IsHeadingRestricted = true;
					m_HeadingRestrictData.InitData(pModel.m_fHeadingAngle, action.GetCameraLRAngle());
				}
				
				return DayZPlayerImplementHeading.RestrictHeading(pDt, pModel, m_fLastHeadingDiff, m_HeadingRestrictData);
			}
			else
				m_IsHeadingRestricted = false;
		}

		//! during fullbody gestures - disables character turning
		if (GetEmoteManager().IsControllsLocked())
		{
			HumanItemAccessor hia = GetItemAccessor();
			HumanItemBehaviorCfg hibcfg = hia.GetItemInHandsBehaviourCfg();
			if (hibcfg && hibcfg.m_StanceRotation[m_MovementState.m_iStanceIdx] == DayZPlayerConstants.ROTATION_DISABLE)
			{
				return DayZPlayerImplementHeading.NoHeading(pDt, pModel, m_fLastHeadingDiff);
			}
			else
			{
				m_fLastHeadingDiff = 0;
				return DayZPlayerImplementHeading.ClampHeading(pDt, pModel, m_fLastHeadingDiff);
			}
		}
		
		return super.HeadingModel(pDt, pModel);
	}
	
	override bool IsInventorySoftLocked()
	{
		return m_InventorySoftLockCount > 0;
	}
	
	//! 'soft lock' meaning inventory screen cannot be displayed, but mechanically, inventory operations are still possible
	override void SetInventorySoftLock(bool status)
	{
		if (status)
			m_InventorySoftLockCount++;
		else
			m_InventorySoftLockCount--;
		
		if (m_InventorySoftLockCount < 0)
			m_InventorySoftLockCount = 0;
		
		m_InventorySoftLocked = IsInventorySoftLocked();
	}
	
	void SetLoadedQuickBarItemBind(EntityAI entity, int index)
	{
		if (m_aQuickBarLoad)
			m_aQuickBarLoad.Insert(new Param2<EntityAI, int>(entity,index));
	}
	
	override bool IsLiftWeapon()
	{
		return m_LiftWeapon_player;
	}
		
	override float GetWeaponObstruction()
	{
		return m_ObstructWeapon_player;
	}
		
	override bool IsWeaponObstructionBlockingADS()
	{
		Weapon_Base weapon = Weapon_Base.Cast(GetItemInHands());
		if (!weapon) 
			return false;
		
		// Recompute the relative obstruction [0 ... 1] value into actual distance ...
		float obstruction = GetWeaponObstruction();
			
		// ... unless there is no obstruction to begin with at which point the
		// additional checks are rendered competely irrelevant.
		if (obstruction <= 0.0)
		{
			return false;
		}
			
		float penetration = weapon.GetObstructionPenetrationDistance(obstruction);
		bool  inSights    = m_CameraIronsight || m_CameraOptics;
		// Create a threshold that allows the user to remain in ADS for lesser obstruction
		// values, significantly reducing the continuous state changes near the edge.
		if (inSights)
		{
			return penetration > 0.040; // 4.0 cm
		}
		// Prevent the user from entering ADS if there is even a tiny fraction of obstruction,
		// further reinforcing the statement above.
		else
		{
			return penetration > 0.005; // 0.5 cm
		}
	}
	
	//Server
	bool ReadLiftWeaponRequest(int userDataType, ParamsReadContext ctx)
	{
		bool state;
		float obstruct;
		ctx.Read(state);
		ctx.Read(obstruct);
		SetLiftWeapon(state, obstruct);
		
		return true;
	}
	
	void SetLiftWeapon(int pJunctureID, ParamsReadContext ctx) // Obsolete
	{
		bool state;
		float obstruct;
		ctx.Read(state);
		ctx.Read(obstruct);
		
		SetLiftWeapon(state, obstruct);
		
		//Print("SetLiftWeapon | STS: " + GetSimulationTimeStamp());
	}
	
	void SetLiftWeapon(bool state, float obstruct = 0)
	{
		m_ProcessLiftWeaponState = state;
		m_ProcessLiftWeapon = true;
		m_ProcessObstructWeapon = obstruct;
	}
	
	//! Client-side only
	void SendLiftWeaponSync(bool state, float obstruct = 0)
	{
		HumanCommandWeapons	hcw;
			
		bool liftChange = m_LiftWeapon_player != state;
		bool obstructChange = Math.AbsFloat(m_ObstructWeapon_player-obstruct) > 0.03;
		
		// Apply state immediately
		m_LiftWeapon_player = state;
		m_ObstructWeapon_player = obstruct;
		
		if (liftChange)
		{
			GetWeaponManager().OnLiftWeapon();
		}
			
		if (!liftChange && !obstructChange) 
		{
			// insignificant difference
			return;
		}
			
		// Notify server to apply same state
		if (GetGame().IsMultiplayer() && GetGame().IsClient())
		{
			ScriptInputUserData ctx = new ScriptInputUserData;
			if (!ctx.CanStoreInputUserData())
			{
				// ctx not available??
				return;
			}
			
			ctx.Write(INPUT_UDT_WEAPON_LIFT_EVENT);
			ctx.Write(state);
			ctx.Write(obstruct);
			ctx.Send();
		}
	}
	
	override void CheckLiftWeapon()
	{
		// lift weapon check
		if (GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_CLIENT)
		{
			Weapon_Base weap;
			if (Weapon_Base.CastTo(weap, GetItemInHands()))
			{
				Object hitObject;
				float obstruct;
				bool limited = weap.LiftWeaponCheckEx(this, obstruct, hitObject);
				if (weap.UseWeaponObstruction(this, obstruct, hitObject))
				{
					limited = false;
				}
				else
				{
					obstruct =  0.0;
				}
				
				obstruct = Math.Clamp( obstruct, 0, 1 );
				
				if (m_LiftWeapon_player != limited || m_ObstructWeapon_player != obstruct)
				{
					SendLiftWeaponSync(limited, obstruct);
				}
			}
			else if (m_LiftWeapon_player || m_ObstructWeapon_player > 0)
			{
				SendLiftWeaponSync(false);
			}
		}
	}
	
	override void ProcessLiftWeapon()
	{
		if (m_ProcessLiftWeapon)
		{
			bool liftChange = m_LiftWeapon_player != m_ProcessLiftWeaponState;
			if (liftChange)
			{
				GetWeaponManager().OnLiftWeapon();
			}
			m_LiftWeapon_player = m_ProcessLiftWeaponState;
			m_ObstructWeapon_player = m_ProcessObstructWeapon;
			m_ProcessLiftWeapon = false;
			
			//Debug.Log("SimulationStamp_server: " + this.GetSimulationTimeStamp());
		}
	}
	
	//! state 'true' == hide
	override void HideClothing(ItemOptics optic, bool state)
	{
		super.HideClothing(optic, state);
		array<int> clothingArray = new array<int>;
		Clothing clothes;
		
		if (state && DayZPlayerCameraOptics.Cast(m_CurrentCamera))
		{
			if (GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_CLIENT)
			{
				clothingArray.Insert(InventorySlots.BACK);
				clothingArray.Insert(InventorySlots.SHOULDER);
				clothingArray.Insert(InventorySlots.MELEE);
				if (optic && optic.GetCurrentStepFOV() < GameConstants.DZPLAYER_CAMERA_FOV_IRONSIGHTS)
				{
					clothingArray.Insert(InventorySlots.BODY);
					clothingArray.Insert(InventorySlots.VEST);
				}
				
				SetInvisibleRecursive(true,this,clothingArray);
			}
		}
		//showing should be instant (called directly, not via CallLater)
		else
		{
			if (GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_CLIENT)
			{
				clothingArray = {InventorySlots.BACK,InventorySlots.BODY,InventorySlots.VEST,InventorySlots.SHOULDER,InventorySlots.MELEE};
				
				SetInvisibleRecursive(false,this,clothingArray);
			}
		}
	}
	
	void RequestUnconsciousness(bool enable)
	{
		DayZPlayerSyncJunctures.SendPlayerUnconsciousness(this, enable);
	}
	
	override void SetDeathDarknessLevel(float duration, float tick_time)
	{
		super.SetDeathDarknessLevel(duration, tick_time);
		
		if (IsControlledPlayer())
		{
			float actual_tick = tick_time;
			if (GetDamageDealtEffect())
				m_DamageDealtEffect = null;
		
			if (GetFlashbangEffect())
				m_FlashbangEffect = null;
			
			float progress;
			if (duration > 0)
				progress = 1 - ((duration - m_DeathDarkeningCurrentTime) / duration);
			
			m_DeathDarkeningCurrentTime += actual_tick;
			
			if (!IsAlive() && IsPlayerSelected())
			{
				m_DeathDarkeningParam.param1 = progress;
				PPERequesterBank.GetRequester(PPERequester_DeathDarkening).Start(m_DeathDarkeningParam);
			}
			
			if (m_DeathDarkeningCurrentTime >= duration)
			{
				StopDeathDarkeningEffect();
			}
		}
		else
		{
			StopDeathDarkeningEffect();
		}
	}
	
	override bool IsInFBEmoteState()
	{
		if (!IsEmotePlaying())
			return false;
		if ((m_EmoteManager.m_Callback && m_EmoteManager.m_Callback.m_IsFullbody) || m_EmoteManager.m_IsSurrendered)
		{
			return true;
		}
		return false;
	}
	
	//! ends surrender, originally intended for surrender->restrained transitioning
	void EndSurrenderRequest(SurrenderData data = null)
	{
		if (m_EmoteManager)
			m_EmoteManager.EndSurrenderRequest(data);
	}
	
	bool IsSurrendered()
	{
		if (m_EmoteManager)
			return m_EmoteManager.m_IsSurrendered;
		
		return false;
	}
	
	bool IsPlayerLoaded()
	{
		return m_PlayerLoaded;
	}
	
	//disconnected, caused problems. Awaiting refactor
	override void CheckAnimationOverrides()
	{
		//FB gesture slideposeangle override
		if (IsInFBEmoteState() && IsControlledPlayer() && IsPlayerSelected())
		{
			OverrideSlidePoseAngle(Math.PI2);
		}
		else if (!IsInFBEmoteState() && IsControlledPlayer() && IsPlayerSelected())
		{
			OverrideSlidePoseAngle(m_OriginalSlidePoseAngle);
		}
		
		//+add more, if needed
	}
	
	/*void OnSurrenderEnd()
	{
	}*/
	
	bool CanRedirectToWeaponManager (notnull EntityAI item, out bool isActionPossible)
	{
		isActionPossible = false;
		Magazine mag = Magazine.Cast(item);
		Weapon_Base wpn = Weapon_Base.Cast(item.GetHierarchyParent());
		if (mag && wpn)
		{
			if (GetWeaponManager().CanDetachMagazine(wpn, mag))
			{
				//Print("[inv] PlayerBase.CanRedirectToWeaponManager OK, can detach mag=" + mag + " from wpn=" + wpn);
				isActionPossible = true;
			}
			else
			{
				//Print("[inv] PlayerBase.CanRedirectToWeaponManager cannot detach mag=" + mag + " from wpn=" + wpn);
			}
			return true;
		}
		return false;
	}
	
	// Inventory actions with redirection to weapon manager
	override bool PredictiveTakeEntityToTargetInventory (notnull EntityAI target, FindInventoryLocationType flags, notnull EntityAI item)
	{
		bool can_detach;
		if (CanRedirectToWeaponManager(item,can_detach))
		{
			InventoryLocation il = new InventoryLocation();
			if (can_detach && target.GetInventory().FindFreeLocationFor(item, flags, il))
			{
				return GetWeaponManager().DetachMagazine(il);
			}
			return false;
		}
		return super.PredictiveTakeEntityToTargetInventory(target, flags, item);
	}

	override bool PredictiveTakeEntityToInventory (FindInventoryLocationType flags, notnull EntityAI item)
	{
		bool can_detach;
		if (CanRedirectToWeaponManager(item,can_detach))
		{
			InventoryLocation il = new InventoryLocation();
			if (can_detach && GetInventory().FindFreeLocationFor(item, flags, il))
			{
				return GetWeaponManager().DetachMagazine(il);
			}
			return false;
		}
		return super.PredictiveTakeEntityToInventory(flags, item);
	}
	
	override bool PredictiveTakeEntityToTargetAttachment (notnull EntityAI target, notnull EntityAI item)
	{
		Weapon_Base parentWpn = Weapon_Base.Cast(target);
		Magazine mag = Magazine.Cast(item);
		if (parentWpn && mag)
		{
			if (GetWeaponManager().CanAttachMagazine(parentWpn, mag))
				return GetWeaponManager().AttachMagazine(mag);

			return false;
		}
		return super.PredictiveTakeEntityToTargetAttachment(target, item);
	}
	
	override bool PredictiveTakeEntityToTargetAttachmentEx (notnull EntityAI target, notnull EntityAI item, int slot)
	{
		Weapon_Base parentWpn = Weapon_Base.Cast(target);
		Magazine mag = Magazine.Cast(item);
		if (parentWpn && mag)
		{
			if (target.CanReceiveAttachment(item,slot) && GetWeaponManager().CanAttachMagazine(parentWpn, mag))
				return GetWeaponManager().AttachMagazine(mag);

			return false;
		}
		return super.PredictiveTakeEntityToTargetAttachmentEx(target, item,slot);
	}
	
	override bool PredictiveTakeEntityToTargetCargo (notnull EntityAI target, notnull EntityAI item)
	{
		bool can_detach;
		if (CanRedirectToWeaponManager(item,can_detach))
		{
			InventoryLocation il = new InventoryLocation();
			if (can_detach && target.GetInventory().FindFreeLocationFor(item, FindInventoryLocationType.CARGO, il))
			{
				return GetWeaponManager().DetachMagazine(il);	
			}
			return false;	
		}
		return super.PredictiveTakeEntityToTargetCargo(target,item);
	}
	
	override bool PredictiveTakeEntityToTargetCargoEx (notnull CargoBase cargo, notnull EntityAI item, int row, int col)
	{
		bool can_detach;
		if (CanRedirectToWeaponManager(item, can_detach))
		{
			if (can_detach)
			{
				InventoryLocation dst = new InventoryLocation;
				dst.SetCargoAuto(cargo, item, row, col, item.GetInventory().GetFlipCargo());

				return GetWeaponManager().DetachMagazine(dst);
			}
			return false;
		}		
		return super.PredictiveTakeEntityToTargetCargoEx (cargo, item, row, col);
	}
	
	override bool PredictiveDropEntity (notnull EntityAI item)
	{
		bool can_detach;
		if (CanRedirectToWeaponManager(item,can_detach))
		{
			if (can_detach)
			{
				vector m4[4];
				Math3D.MatrixIdentity4(m4);
				
				//! We don't care if a valid transform couldn't be found, we just want to preferably use it instead of placing on the player
				GameInventory.PrepareDropEntityPos(this, item, m4, false, GameConstants.INVENTORY_ENTITY_DROP_OVERLAP_DEPTH);
				InventoryLocation il = new InventoryLocation;
				il.SetGround(item, m4);
				return GetWeaponManager().DetachMagazine(il);
			}
			return false;
		}
		return super.PredictiveDropEntity(item);
	}
	
	override bool PredictiveSwapEntities (notnull EntityAI item1, notnull EntityAI item2)
	{
		//Print("PlayerBase | PredictiveSwapEntities");
		Magazine swapmag1 = Magazine.Cast(item1);
		Magazine swapmag2 = Magazine.Cast(item2);

		if (swapmag1 && swapmag2)
		{
			Weapon_Base parentWpn;

			if (Class.CastTo(parentWpn, swapmag1.GetHierarchyParent()))
			{
				if (GetWeaponManager().CanSwapMagazine(parentWpn, swapmag2))
				{
					Print("[inv] PlayerBase.PredictiveSwapEntities: swapping mag1=" + swapmag1 + " to parent wpn=" + parentWpn + " of mag1=" + swapmag1);
					return GetWeaponManager().SwapMagazine(swapmag2);
				}
				else
				{
					Print("[inv] PlayerBase.PredictiveSwapEntities: can not swap magazines");
					return false;
				}
			}

			if (Class.CastTo(parentWpn, swapmag2.GetHierarchyParent()))
			{
				if (GetWeaponManager().CanSwapMagazine(parentWpn, swapmag1))
				{
					Print("[inv] PlayerBase.PredictiveSwapEntities: swapping mag1=" + swapmag1 + " to parent wpn=" + parentWpn + " of mag2=" + swapmag2);
					return GetWeaponManager().SwapMagazine(swapmag1);
				}
				else
				{
					Print("[inv] PlayerBase.PredictiveSwapEntities: can not swap magazines");
					return false;
				}
			}
		}
		
		EntityAI item_hands;
		EntityAI item_ground;
		if (IsSwapBetweenHandsAndGroundLargeItem(item1,item2,item_hands,item_ground) && !m_ActionManager.GetRunningAction())
		{
			ActionManagerClient mngr_client;
			CastTo(mngr_client,m_ActionManager);
			
			ActionTarget atrg = new ActionTarget(item_ground,null,-1,vector.Zero,-1.0);
			if (mngr_client.GetAction(ActionSwapItemToHands).Can(this,atrg,ItemBase.Cast(item_hands)))
			{
				mngr_client.PerformActionStart(mngr_client.GetAction(ActionSwapItemToHands),atrg,ItemBase.Cast(item_hands));
				return true;
			}
			return super.PredictiveSwapEntities(item1, item2);
		}
		else
			return super.PredictiveSwapEntities(item1, item2);
	}
	
	override bool PredictiveForceSwapEntities (notnull EntityAI item1, notnull EntityAI item2, notnull InventoryLocation item2_dst)
	{
		InventoryLocation il = new InventoryLocation;	
		if (item1.IsHeavyBehaviour() && item1.GetInventory().GetCurrentInventoryLocation(il) && il.GetType() == InventoryLocationType.GROUND && !m_ActionManager.GetRunningAction())
		{		
			//Print("override bool PredictiveForceSwapEntities (notnull EntityAI item1, notnull EntityAI item2, notnull InventoryLocation item2_dst)");
			ActionManagerClient mngr_client;
			CastTo(mngr_client,m_ActionManager);
			
			ActionTarget atrg = new ActionTarget(item1,null,-1,vector.Zero,-1.0);
			if (mngr_client.GetAction(ActionSwapItemToHands).Can(this,atrg,ItemBase.Cast(item2)))
			{
				mngr_client.PerformActionStart(mngr_client.GetAction(ActionSwapItemToHands),atrg,ItemBase.Cast(item2));
			}
			return true;
		}
		else
			return super.PredictiveForceSwapEntities(item1, item2, item2_dst);
	}
	
	override void PredictiveTakeEntityToHands(EntityAI item)
	{
		if (item.IsHeavyBehaviour() && !m_ActionManager.GetRunningAction() && !item.GetHierarchyParent())
		{
			ActionManagerClient mngr_client;
			if (CastTo(mngr_client,m_ActionManager))
			{
				ActionTarget atrg = new ActionTarget(item,null,-1,vector.Zero,-1.0);
				
				if (mngr_client.GetAction(ActionTakeItemToHands).Can(this,atrg,null))
				{
					mngr_client.PerformActionStart(mngr_client.GetAction(ActionTakeItemToHands),atrg,null);
				}
				/*).Can(this,
				mngr_client.ActionStart(mngr_client.GetAction(ActionTakeItemToHands),mngr_client.FindActionTarget(),null);
				return;*/
			}
		}
		else
			super.PredictiveTakeEntityToHands(item);
	}

	override bool PredictiveTakeToDst (notnull InventoryLocation src, notnull InventoryLocation dst)
	{
		EntityAI item = src.GetItem();
		if (item)
		{
			bool can_detach;
		
			if (CanRedirectToWeaponManager(item,can_detach))
			{
				if (can_detach)
				{
					return GetWeaponManager().DetachMagazine(dst);	
				}
				return false;	
			}
			return super.PredictiveTakeToDst(src,dst);
		}
		return false;
	}
	
	bool IsSwapBetweenHandsAndGroundLargeItem(notnull EntityAI item1, notnull EntityAI item2, out EntityAI item_hands, out EntityAI item_ground)
	{
		InventoryLocation il = new InventoryLocation;
		if (item1.GetInventory().GetCurrentInventoryLocation(il) && il.GetType() == InventoryLocationType.HANDS)
			item_hands = item1;
		if (item2.GetInventory().GetCurrentInventoryLocation(il) && il.GetType() == InventoryLocationType.HANDS)
			item_hands = item2;
		if (item1.GetInventory().GetCurrentInventoryLocation(il) && il.GetType() == InventoryLocationType.GROUND)
			item_ground = item1;
		if (item2.GetInventory().GetCurrentInventoryLocation(il) && il.GetType() == InventoryLocationType.GROUND)
			item_ground = item2;
		
		return item_hands && item_ground && item_ground.IsHeavyBehaviour();
	}
	
	//! Dynamic hair hiding
	void SetHairLevelToHide(int level, bool state, bool was_debug = false)
	{
		if (was_debug && GetInstanceType() != DayZPlayerInstanceType.INSTANCETYPE_CLIENT)
			return;
		
		if (!m_CharactersHead)
		{
			ErrorEx("No valid head detected on character!");
			return;
		}
		
		if (level == -1) //hide/show ALL
		{
			m_HideHairAnimated = !m_HideHairAnimated;
			for (int i = 0; i < m_CharactersHead.m_HeadHairSelectionArray.Count(); ++i)
			{
				//m_CharactersHead.SetSimpleHiddenSelectionState(i,m_HideHairAnimated);
				SelectionTranslation stt = SelectionTranslation.Cast(m_CharactersHead.m_HeadHairHidingStateMap.Get(i));
				stt.SetSelectionState(m_HideHairAnimated);
				m_CharactersHead.m_HeadHairHidingStateMap.Set(i, stt);
#ifdef DIAG_DEVELOPER
#ifndef SERVER
				PluginDiagMenuClient diagmenu = PluginDiagMenuClient.Cast(GetPlugin(PluginDiagMenuClient));
				diagmenu.m_HairHidingStateMap.Set(i, m_HideHairAnimated);
#endif
#endif
			}
		}
		else //hide/show selected level only
		{
			bool switchState;
			if (was_debug)
			{
#ifdef DIAG_DEVELOPER
#ifndef SERVER
				PluginDiagMenuClient diagmenuu = PluginDiagMenuClient.Cast(GetPlugin(PluginDiagMenuClient));
				switchState = !diagmenuu.m_HairHidingStateMap.Get(level);
				diagmenuu.m_HairHidingStateMap.Set(level, switchState);
#endif
#endif
			}
			else
			{
				switchState = !state;
			}
			//m_CharactersHead.SetSimpleHiddenSelectionState(level,switchState);
			stt = SelectionTranslation.Cast(m_CharactersHead.m_HeadHairHidingStateMap.Get(level));
			stt.SetSelectionState(switchState);
			m_CharactersHead.m_HeadHairHidingStateMap.Set(level, stt); //nescessary?
		}
	}
	
	void HideHairSelections(ItemBase item, bool state)
	{
		if (!item || !item.GetHeadHidingSelection() || !m_CharactersHead)
			return;
		
		int slot_id; //item currently attached (or detaching from) here
		string slot_name; //item currently attached (or detaching from) here
		string str
		int idx = 0;
		int i;
		int count;
		item.GetInventory().GetCurrentAttachmentSlotInfo(slot_id,slot_name);
		
		if (item.HidesSelectionBySlot())
		{
			count = item.GetInventory().GetSlotIdCount();
			for (i = 0; i < count; i++)
			{
				if (item.GetInventory().GetSlotId(i) == slot_id)
				{
					str = item.GetHeadHidingSelection().Get(i);
					idx = m_CharactersHead.m_HeadHairSelectionArray.Find(str);
					if (idx != -1)
						SetHairLevelToHide(idx,state);
					#ifdef DEVELOPER
					else
						Debug.Log("No valid selection '" + str + "' found on head of " + GetType() + ". Verify the p3d, model config, and the 'HAIR_HIDING_SELECTIONS' macro in basicDefines.");
					#endif
				}
			}
		}
		else
		{
			count = item.GetHeadHidingSelection().Count();
			for (i = 0; i < count; i++)
			{
				str = item.GetHeadHidingSelection().Get(i);
				idx = m_CharactersHead.m_HeadHairSelectionArray.Find(str);
				if (idx != -1)
					SetHairLevelToHide(idx,state);
				#ifdef DEVELOPER
				else
					Debug.Log("No valid selection '" + str + "' found on head of " + GetType() + ". Verify the p3d, model config, and the 'HAIR_HIDING_SELECTIONS' macro in basicDefines.");
				#endif
			}
		}
		UpdateHairSelectionVisibility();
	}
	
	void UpdateHairSelectionVisibility(bool was_debug = false)
	{
		if (!m_CharactersHead)
			return;
		bool shown;
		bool exception_hidden = false;
		int i;
		int count = m_CharactersHead.m_HeadHairHidingStateMap.Count();
		SelectionTranslation stt;
		
		//hide/show beard
		if (IsMale() && m_CharactersHead.GetBeardIndex() > -1 && !was_debug)
		{
			SetHairLevelToHide(m_CharactersHead.GetBeardIndex(),GetLifeSpanState() != LifeSpanState.BEARD_EXTRA);
		}
		
		//show all first
		for (i = 0; i < count; i++)
		{
			m_CharactersHead.SetSimpleHiddenSelectionState(i,true);
		}
		//then carve it up
		for (i = 0; i < count; i++)
		{
			stt = m_CharactersHead.m_HeadHairHidingStateMap.Get(i);
			shown = stt.GetSelectionState();
			if (!shown)
			{
				if (/*IsMale() && */!m_CharactersHead.IsHandlingException())
				{
					m_CharactersHead.SetSimpleHiddenSelectionState(i,shown);
					UpdateTranslatedSelections(stt);
					//Print("hidden idx: " + i);
				}
				else
				{
					exception_hidden = true;
				}
			}
		}
		
		//exceptions handled differently; hides hair
		if (exception_hidden)
		{
			m_CharactersHead.SetSimpleHiddenSelectionState(m_CharactersHead.GetHairIndex(),false);
			if (IsMale())
				m_CharactersHead.SetSimpleHiddenSelectionState(m_CharactersHead.GetBeardIndex(),false);
		}
	}
	
	void UpdateTranslatedSelections(SelectionTranslation stt)
	{
		int index;
		array<int> translatedSelectinosArray = stt.GetTranslatedSelections();
		for (int i = 0; i < translatedSelectinosArray.Count(); i++)
		{
			index = translatedSelectinosArray.Get(i);
			//if (index > -1)
				m_CharactersHead.SetSimpleHiddenSelectionState(index,false); //safe this way, only hiding/carving from shown parts
		}
	}
	
	//! helper method for re-checking hairhiding on character load
	void CheckHairClippingOnCharacterLoad()
	{
		ItemBase headgear = ItemBase.Cast(GetInventory().FindAttachment(InventorySlots.HEADGEAR));
		ItemBase mask = ItemBase.Cast(GetInventory().FindAttachment(InventorySlots.MASK));
		
		HideHairSelections(headgear,true);
		HideHairSelections(mask,true);
	}
	
	bool IsNVGWorking()
	{
		return m_ActiveNVTypes && m_ActiveNVTypes.Count() > 0;
	}
	
	bool IsNVGLowered()
	{
		return m_LoweredNVGHeadset;
	}
	
	int GetNVType()
	{
		if (!m_ActiveNVTypes || m_ActiveNVTypes.Count() == 0)
		{
			return NVTypes.NONE;
		}
		else
		{
			return m_ActiveNVTypes[m_ActiveNVTypes.Count() - 1];
		}
	}
	
	array<int> GetNVTypesArray()
	{
		return m_ActiveNVTypes;
	}
	
	//!Deprecated
	void SetNVGWorking(bool state)
	{
		//Deprecated, below is for legacy's sake
		AddActiveNV(NVTypes.NV_GOGGLES);
	}
	
	void SetNVGLowered(bool state)
	{
		m_LoweredNVGHeadset = state;
	}
	
	void AddActiveNV(int type)
	{
		if (!m_ActiveNVTypes || (GetGame().IsMultiplayer() && GetGame().IsServer()))
		{
			#ifdef DEVELOPER
				Error("AddActiveNV | illegal server-side call!");
			#endif
			
			return;
		}
		
		if (m_ActiveNVTypes.Find(type) == -1)
			m_ActiveNVTypes.Insert(type);
	}
	
	void RemoveActiveNV(int type)
	{
		if (!m_ActiveNVTypes || (GetGame().IsMultiplayer() && GetGame().IsServer()))
		{
			#ifdef DEVELOPER
				Error("RemoveActiveNV | illegal server-side call!");
			#endif
			
			return;
		}
		
		if (m_ActiveNVTypes.Find(type) != -1)
			m_ActiveNVTypes.RemoveItem(type);
	}
	
	void ResetActiveNV()
	{
		m_ActiveNVTypes.Clear();
	}
	
#ifdef DEVELOPER
	override string GetDebugText()
	{
		string text = super.GetDebugText();
		/*
		text += "GetMovementTimeToStrafeJog:" +  CfgGameplayHandler.GetMovementTimeToStrafeJog() + "\n";
		text += "GetMovementTimeToStrafeSprint:" +  CfgGameplayHandler.GetMovementTimeToStrafeSprint()+ "\n";
		
		SHumanCommandMoveSettings moveSettings = GetDayZPlayerType().CommandMoveSettingsW();
		if (moveSettings)
		{
			text += "hmcs.m_fDirFilterTimeout:" + moveSettings.m_fDirFilterTimeout + "\n";
			text += "hmcs.m_fDirFilterSprintTimeout:" + moveSettings.m_fDirFilterSprintTimeout+ "\n";
		}*/
		
		return text;
	}

	
	
	void DEBUGRotateNVG()
	{
		NVGoggles nvg;
		
		if (FindAttachmentBySlotName("Eyewear"))
		{
			nvg = NVGoggles.Cast(FindAttachmentBySlotName("Eyewear").FindAttachmentBySlotName("NVG"));
		}
		else if (FindAttachmentBySlotName("Headgear"))
		{
			nvg = NVGoggles.Cast(FindAttachmentBySlotName("Headgear").FindAttachmentBySlotName("NVG"));
		}
		
		if (nvg)
		{
			nvg.RotateGoggles(nvg.m_IsLowered);
		}
	}
#endif
	
	void AdjustBandana(EntityAI item, string slot_name)
	{
		if (Bandana_ColorBase.Cast(item))
		{
			if (slot_name == "Headgear")
			{
				item.SetSimpleHiddenSelectionState(0,1);
				item.SetSimpleHiddenSelectionState(1,0);
			}
			else if (slot_name == "Mask")
			{
				item.SetSimpleHiddenSelectionState(0,0);
				item.SetSimpleHiddenSelectionState(1,1);
			}
		}
	}
	
	void AdjustShemag(EntityAI item, string slot_name)
	{
		if (Shemag_ColorBase.Cast(item))
		{
			if (slot_name == "Headgear")
			{
				item.SetSimpleHiddenSelectionState(0,1);
				item.SetSimpleHiddenSelectionState(1,0);
			}
			else if (slot_name == "Mask")
			{
				item.SetSimpleHiddenSelectionState(0,0);
				item.SetSimpleHiddenSelectionState(1,1);
			}
		}
	}
	
	//client-side
	void UpdateCorpseStateVisual()
	{
		//Print("---Prettying up corpses... | " + GetGame().GetTime() + " | " + this + " | " + GetType() + "---");
		//Print("m_DecayedTexture = " + m_DecayedTexture);
		int state = Math.AbsInt(m_CorpseState);//negative sign denotes a special meaning(state was forced to a live player), but we are only intetested in the positive value here
		if (state == PlayerConstants.CORPSE_STATE_DECAYED)
		{
			EntityAI bodypart;
			ItemBase item;
			
			string path;
			int idx;
			int slot_id;
			array<string> bodyparts = {"Gloves","Body","Legs","Feet"};
			
			for (int i = 0; i < bodyparts.Count(); i++)
			{
				slot_id = InventorySlots.GetSlotIdFromString(bodyparts.Get(i));
				bodypart = GetInventory().FindPlaceholderForSlot(slot_id);
				item = ItemBase.Cast(GetInventory().FindAttachment(slot_id));
				
				if (bodypart)
				{
					path = "cfgVehicles " + bodypart.GetType();
					idx = bodypart.GetHiddenSelectionIndex("personality");
					if (idx > -1)
					{
						bodypart.SetObjectTexture(idx,m_DecayedTexture);
					}
				}
				if (item)
				{
					path = "cfgVehicles " + item.GetType();
					idx = item.GetHiddenSelectionIndex("personality");
					if (idx > -1)
					{
						item.SetObjectTexture(idx,m_DecayedTexture);
					}
				}
			}
			
			SetFaceTexture(m_DecayedTexture);
		}
	}
	
	void SetDecayEffects(int effect = -1)
	{
		int boneIdx = GetBoneIndexByName("Spine2");
		Particle p;
		
		switch (effect)
		{
			case PlayerConstants.CORPSE_STATE_MEDIUM :
				//play medium sound/flies particle
				if (!m_FliesEff)
					m_FliesEff = new EffSwarmingFlies();
				
				if (m_FliesEff && !SEffectManager.IsEffectExist(m_FliesIndex))
				{
					m_FliesEff.SetDecalOwner(this);
					m_FliesIndex = SEffectManager.PlayOnObject(m_FliesEff, this, "0 0.25 0");
					p = m_FliesEff.GetParticle();
					AddChild(p, boneIdx);
					if (!m_SoundFliesEffect)
					{
						PlaySoundSetLoop(m_SoundFliesEffect, "Flies_SoundSet", 1.0, 1.0);
					}
				}
			break;
			case PlayerConstants.CORPSE_STATE_DECAYED :
				//play serious sound/flies particle
				if (!m_FliesEff)
					m_FliesEff = new EffSwarmingFlies();
				
				if (m_FliesEff && !SEffectManager.IsEffectExist(m_FliesIndex))
				{
					m_FliesEff.SetDecalOwner(this);
					m_FliesIndex = SEffectManager.PlayOnObject(m_FliesEff, this, "0 0.25 0");
					p = m_FliesEff.GetParticle();
					AddChild(p, boneIdx);
					if (!m_SoundFliesEffect)
					{
						PlaySoundSetLoop(m_SoundFliesEffect, "Flies_SoundSet", 1.0, 1.0);
					}
				}
			break;
			//remove
			default:
				SEffectManager.DestroyEffect(m_FliesEff);
				StopSoundSet(m_SoundFliesEffect);
			break;
		}
	}
	
	void PreloadDecayTexture()
	{
		int idx = GetHiddenSelectionIndex("decay_preload");
		if (idx > -1)
			SetObjectTexture(idx, m_DecayedTexture);
	}
	
	void SetLastMapInfo(float scale, vector pos)
	{
		m_LastMapScale = scale;
		m_LastMapPos = pos;
	}
	
	bool GetLastMapInfo(out float scale, out vector pos)
	{
		scale = m_LastMapScale;
		pos = m_LastMapPos;
		
		return m_LastMapScale != -1.0;
	}
	
	override bool PhysicalPredictiveDropItem(EntityAI entity, bool heavy_item_only = true)
	{
		vector dir;
		ItemBase item = ItemBase.Cast(entity);
		
		//! from hands
		if (GetItemInHands() == item)
		{
			return PredictiveDropEntity(item);
		}
		else
		{
			ActionManagerClient actionManager = ActionManagerClient.Cast(m_ActionManager);
			if (actionManager)
			{
				ActionTarget actionTarget = new ActionTarget(null, null, -1, vector.Zero, -1);
				if (actionManager.GetAction(ActionDropItemSimple).Can(this, actionTarget, item))
				{
					actionManager.PerformActionStart(actionManager.GetAction(ActionDropItemSimple), actionTarget, item);
					return true;
				}
			}
			else
				return true;
		}
		
		return false;
	}
	
	override void SetProcessUIWarning(bool state)
	{
		m_ProcessUIWarning = state;
	}
	
	void dmgDebugPrint(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		/*Print("ProjectileDebugging | Damage Health: " + damageResult.GetDamage(dmgZone,"Health") + " | Component: " + component + " | Zone: " + dmgZone + "| Timestamp: " + GetSimulationTimeStamp());
		Print("ProjectileDebugging | speedCoef: " + speedCoef);
		Print("ProjectileDebugging | GetWorldTime(): " + GetWorldTime());
		Print("-----------------------------------------------");*/
	}
	
	//Use this method to process additionnal dmg to legs specifically (must use the dmg system to fire relevant events)
	//Legzone is a dummy to inflict damage once and NOT transfer damage to other zones. All leg zones will drop to 0 anyway
	void DamageAllLegs(float inputDmg)
	{
		array<string> legZones = new array<string>;
		legZones.Insert("LeftLeg");
		legZones.Insert("RightLeg");
		legZones.Insert("RightFoot");
		legZones.Insert("LeftFoot");
		
		foreach (string legZone : legZones)
			DecreaseHealth(legZone, "", inputDmg);
	}
	
	//! tries to hide item in player's hands, some exceptions for various movement states
	void TryHideItemInHands(bool hide, bool force = false)
	{
		if (!hide && ((!IsSwimming() && !IsClimbingLadder() && !IsInVehicle() && !AnimCommandCheck(HumanMoveCommandID.CommandSwim | HumanMoveCommandID.CommandLadder | HumanMoveCommandID.CommandVehicle)) || force))
		{
			GetItemAccessor().HideItemInHands(false);
		}
		else
		{
			GetItemAccessor().HideItemInHands(true);
		}
	}
	
	//!
	bool CheckAndExecuteStackSplit(FindInventoryLocationType flags, notnull EntityAI item, notnull EntityAI target)
	{
		InventoryLocation il = new InventoryLocation;
		if (target.GetInventory().FindFreeLocationFor(item, flags, il))
		{
			return CheckAndExecuteStackSplitToInventoryLocation(il,item);
		}
		return false;
	}
	
	bool CheckAndExecuteStackSplitToInventoryLocation(InventoryLocation il, notnull EntityAI item)
	{
		int slotID = il.GetSlot();
		float stackableTarget = item.GetTargetQuantityMax(slotID);
		if (stackableTarget > 0 && stackableTarget < item.GetQuantity())
		{
			ItemBase itemB;
			if (CastTo(itemB, item))
			{
				itemB.SplitIntoStackMaxToInventoryLocationClient(il);
				return true;
			}
		}
		return false;
	}
	
	override protected bool TakeEntityToInventoryImpl(InventoryMode mode, FindInventoryLocationType flags, notnull EntityAI item)
	{
		if (CheckAndExecuteStackSplit(flags, item, this))
		{
			if (LogManager.IsSyncLogEnable()) syncDebugPrint("[inv] " + GetDebugName(this) + " STS=" + GetSimulationTimeStamp() + " Take2Inv(SPLIT) item=" + GetDebugName(item));
			return true;
		}
		
		return super.TakeEntityToInventoryImpl(mode, flags, item);
	}
	
	override protected bool TakeEntityToCargoImpl(InventoryMode mode, notnull EntityAI item)
	{
		if (CheckAndExecuteStackSplit(FindInventoryLocationType.CARGO, item, this))
		{
			if (LogManager.IsSyncLogEnable()) syncDebugPrint("[inv] " + GetDebugName(this) + " STS=" + GetSimulationTimeStamp() + " Take2Cgo(SPLIT) item=" +GetDebugName(item));
			return true;
		}
		
		return super.TakeEntityToCargoImpl(mode, item);
	}
	
	override protected bool TakeEntityAsAttachmentImpl(InventoryMode mode, notnull EntityAI item)
	{
		if (CheckAndExecuteStackSplit(FindInventoryLocationType.ATTACHMENT, item, this))
		{
			if (LogManager.IsSyncLogEnable()) syncDebugPrint("[inv] " + GetDebugName(this) + " STS=" + GetSimulationTimeStamp() + " Take2Att(SPLIT) item=" + GetDebugName(item));
			return true;
		}
		
		return super.TakeEntityAsAttachmentImpl(mode, item);
	}
	
	override void TakeEntityToHandsImpl(InventoryMode mode, EntityAI item)
	{
		if (CheckAndExecuteStackSplit(FindInventoryLocationType.HANDS, item, this))
		{
			if (LogManager.IsSyncLogEnable()) syncDebugPrint("[inv] " + GetDebugName(this) + " STS=" + GetSimulationTimeStamp() + " Take2Hands(SPLIT) item=" + GetDebugName(item));
			return;
		}
		
		super.TakeEntityToHandsImpl(mode, item);
	}
	
	override protected bool TakeEntityToTargetInventoryImpl(InventoryMode mode, notnull EntityAI target, FindInventoryLocationType flags, notnull EntityAI item)
	{
		if (CheckAndExecuteStackSplit(flags, item, target))
		{
			if (LogManager.IsSyncLogEnable()) syncDebugPrint("[inv] " + GetDebugName(this) + " STS=" + GetSimulationTimeStamp() + " Take2TargetInv(SPLIT) item=" + GetDebugName(item));
			return true;
		}
		
		return super.TakeEntityToTargetInventoryImpl(mode, target, flags, item);
	}
	
	override protected bool TakeEntityToTargetCargoImpl(InventoryMode mode, notnull EntityAI target, notnull EntityAI item)
	{
		if (CheckAndExecuteStackSplit(FindInventoryLocationType.CARGO, item, target))
		{
			if (LogManager.IsSyncLogEnable()) syncDebugPrint("[inv] " + GetDebugName(this) + " STS=" + GetSimulationTimeStamp() + " Take2TargetCgo(SPLIT) item=" + GetDebugName(item));
			return true;
		}
		
		return super.TakeEntityToTargetCargoImpl(mode, target, item);
	}
	
	override protected bool TakeEntityToTargetAttachmentImpl(InventoryMode mode, notnull EntityAI target, notnull EntityAI item)
	{
		if (CheckAndExecuteStackSplit(FindInventoryLocationType.ATTACHMENT, item, target))
		{
			if (LogManager.IsSyncLogEnable()) syncDebugPrint("[inv] " + GetDebugName(this) + " STS=" + GetSimulationTimeStamp() + " Take2TargetAtt(SPLIT) item=" + GetDebugName(item));
			return true;
		}
		
		return super.TakeEntityToTargetAttachmentImpl(mode, target, item);
	}
	
	override protected bool TakeToDstImpl(InventoryMode mode, notnull InventoryLocation src, notnull InventoryLocation dst)
	{
		if (CheckAndExecuteStackSplitToInventoryLocation(dst, dst.GetItem()))
		{
			if (LogManager.IsSyncLogEnable()) syncDebugPrint("[inv] " + GetDebugName(this) + " STS=" + GetSimulationTimeStamp() + " Take2Dst(SPLIT) item=" + GetDebugName(dst.GetItem()));
			return true;
		}
		
		return super.TakeToDstImpl(mode, src, dst);
	}
	
	override vector GetCenter()
	{
		return GetBonePositionWS(GetBoneIndexByName("spine3"));
	}
	
	
	// contaminated areas - temp stuff 
	void ContaminatedParticleAdjustment()
	{
		if (GetCommand_Move() && m_ContaminatedAroundPlayer && m_ContaminatedAroundPlayerTiny)
		{
			float playerSpeed = GetCommand_Move().GetCurrentMovementSpeed();
			//Print(playerSpeed);
			
			// 1 - prone, crouch 
			// 2 - jog 
			// 3 - sprint 
			float particleLifetime = 5.25;
			float particleSpeed = 0.25;
			if (playerSpeed >= 1)
			{
				particleLifetime = 3.5;
				particleSpeed = 3.25;
			}
			if (playerSpeed >= 2)
			{
				particleLifetime = 2.5;
				particleSpeed = 5.25;
			}
			if (playerSpeed >= 3)
			{
				particleLifetime = 1.5;
				particleSpeed = 8.25;
			}
			m_ContaminatedAroundPlayer.SetParameter(0, EmitorParam.LIFETIME, particleLifetime);
			m_ContaminatedAroundPlayer.SetParameter(1, EmitorParam.LIFETIME, particleLifetime);
			m_ContaminatedAroundPlayer.SetParameter(2, EmitorParam.LIFETIME, particleLifetime);
			m_ContaminatedAroundPlayer.SetParameter(3, EmitorParam.LIFETIME, particleLifetime);
			
			m_ContaminatedAroundPlayerTiny.SetParameter(0, EmitorParam.VELOCITY, particleSpeed);
			vector transform[4];
			GetTransform(transform);
			m_ContaminatedAroundPlayer.SetTransform(transform);
			m_ContaminatedAroundPlayerTiny.SetTransform(transform);
		}
	}
	
	void HandleGlassesEffect()
	{
		if (!IsControlledPlayer())
			return;
		int i;
		
		if (m_ProcessRemoveGlassesEffects)
		{
			for (i = 0; i < m_ProcessRemoveGlassesEffects.Count(); i++)
			{
				PPERequesterBank.GetRequester(m_ProcessRemoveGlassesEffects[i]).Stop();
			}
			m_ProcessRemoveGlassesEffects.Clear();
		}
		
		if (m_ProcessAddGlassesEffects)
		{
			for (i = 0; i < m_ProcessAddGlassesEffects.Count(); i++)
			{
				PPERequesterBank.GetRequester(m_ProcessAddGlassesEffects[i]).Start();
			}
			m_ProcessAddGlassesEffects.Clear();
		}
	}
	
	override void AddArrow(Object arrow, int componentIndex, vector closeBonePosWS, vector closeBoneRotWS)
	{
		CachedObjectsArrays.ARRAY_STRING.Clear();
		GetActionComponentNameList(componentIndex, CachedObjectsArrays.ARRAY_STRING, LOD.NAME_FIRE);
		
		int pivot = componentIndex;
		int newPivot = -1;
		string compName;
		for (int i = 0; i < CachedObjectsArrays.ARRAY_STRING.Count() && newPivot == -1; i++)
		{
			compName = CachedObjectsArrays.ARRAY_STRING.Get(i);
			newPivot = GetBoneIndexByName(compName);
			
		}
		
		if (newPivot != -1)
		{
			pivot = newPivot;
			
		}
		
		vector parentTransMat[4];
		vector arrowTransMat[4];
		
		arrow.GetTransform(arrowTransMat);
		
		if (pivot == -1)
		{
			GetTransformWS(parentTransMat);
		}
		else
		{
			vector rotMatrix[3];
			Math3D.YawPitchRollMatrix(closeBoneRotWS * Math.RAD2DEG,rotMatrix);
			
			parentTransMat[0] = rotMatrix[0];
			parentTransMat[1] = rotMatrix[1];
			parentTransMat[2] = rotMatrix[2];
			parentTransMat[3] = closeBonePosWS;
		}
		
		Math3D.MatrixInvMultiply4(parentTransMat, arrowTransMat, arrowTransMat);
		Math3D.MatrixOrthogonalize4(arrowTransMat);
		arrow.SetTransform(arrowTransMat);
		
		AddChild(arrow, pivot);
		
		#ifdef SERVER
		// creating bleeding source
		BleedingSourcesManagerServer bleedingManager = GetBleedingManagerServer();
		if (bleedingManager)
		{
			if (!bleedingManager.AttemptAddBleedingSourceBySelection(compName))
				bleedingManager.AttemptAddBleedingSourceBySelection("Pelvis");//fallback, if we can't attach bleeding source to the fallback location because there already is another one, it's fine, we are just trying make sure there is at least one
		}
		#endif
	}
	
	override bool IsManagingArrows()
	{
		return true;
	}	
	
	string GetCachedName()
	{
		return m_CachedPlayerName;
	}
	
	string GetCachedID()
	{
		return m_CachedPlayerID;
	}
	
	override void OnFreezeStateChangeClient()
	{
		super.OnFreezeStateChangeClient();
		
		UpdateCorpseState();
	}

	bool HasHeatBuffer()
	{
		return m_HasHeatBuffer;
	}
	
	int GetHeatBufferStage()
	{
		return m_HeatBufferStage;
	}
	
	//!	----------
	//! DEPRECATED
	//! ----------
	
	static ref array<Object> SPREAD_AGENTS_OBJECTS = new array<Object>;
	static ref array<CargoBase> SPREAD_AGENTS_PROXY_CARGOS = new array<CargoBase>;
	
	private int	m_FaceCoveredForShaveLayers = 0;
	int m_AntibioticsActive;

	void SwitchItemTypeAttach(EntityAI item, string slot)
	{
		if (!GetGame().IsServer())
			return;
	}
	
	void SwitchItemTypeDetach(EntityAI item, string slot);
	void AddToEnvironmentTemperature(float pTemperature);

	void IncreaseAntibioticsCount()
	{
		AddMedicalDrugsInUse(EMedicalDrugsType.ANTIBIOTICS);
	}	

	void DecreaseAntibioticsCount()
	{
		RemoveMedicalDrugsInUse(EMedicalDrugsType.ANTIBIOTICS);
	}
	
	bool Consume(ItemBase source, float amount, EConsumeType consume_type)
	{
		PlayerConsumeData consumeData = new PlayerConsumeData();
		consumeData.m_Type = consume_type;
		consumeData.m_Amount = amount;
		consumeData.m_Source = source;
		consumeData.m_Agents = 0;

		return Consume(consumeData);
	}

	bool HasStaminaRemaining()
	{
		if (!GetStaminaHandler())
			return false;
		
		return GetStaminaHandler().GetStamina() > 0;
	}

	void SetBloodyHandsPenalty()
	{
		InsertAgent(eAgents.SALMONELLA, 1);
	}
	
	//!Deprecated
	override void DepleteStamina(EStaminaModifiers modifier, float dT = -1)
	{
		if (GetStaminaHandler())
			GetStaminaHandler().DepleteStamina(modifier,dT);
	}
}


#ifdef DEVELOPER
PlayerBase _player;
#endif
