typedef ItemBase Inventory_Base;
typedef map<typename,ref ActionOverrideData>	TActionAnimOverrideMap;

class DummyItem extends ItemBase
{
	override bool CanPutAsAttachment(EntityAI parent)
	{
		return true;
	}
};

//const bool QUANTITY_DEBUG_REMOVE_ME = false;

class ItemBase extends InventoryItem
{	
	static ref map<typename, ref TInputActionMap> m_ItemTypeActionsMap = new map<typename, ref TInputActionMap>;
	TInputActionMap m_InputActionMap;
	static ref map<typename, ref TActionAnimOverrideMap> m_ItemActionOverrides = new map<typename, ref TActionAnimOverrideMap>; 
	
	bool	m_ActionsInitialize;
	
	static int	m_DebugActionsMask;
	bool		m_RecipesInitialized;
	// ============================================
	// Variable Manipulation System
	// ============================================
	// Quantity
	
	float 	m_VarQuantity;
	float 	m_VarQuantityPrev;//for client to know quantity changed during synchronization
	int		m_VarQuantityInit;
	int		m_VarQuantityMin;
	int		m_VarQuantityMax;
	int		m_Count;
	float	m_VarStackMax;
	float	m_StoreLoadedQuantity = float.LOWEST;
	// Wet
	float 	m_VarWet;
	float 	m_VarWetPrev;//for client to know wetness changed during synchronization
	float 	m_VarWetInit;
	float 	m_VarWetMin;
	float 	m_VarWetMax;
	// Cleanness
	int		m_Cleanness;
	int		m_CleannessInit;
	int		m_CleannessMin;
	int		m_CleannessMax;
	// impact sounds
	bool	m_WantPlayImpactSound;
	bool	m_CanPlayImpactSound = true;
	float	m_ImpactSpeed;
	int		m_ImpactSoundSurfaceHash;
	//
	float	m_HeatIsolation;
	float 	m_ItemModelLength;
	float	m_ItemAttachOffset; // Offset length for when the item is attached e.g. to weapon
	int  	m_LiquidContainerMask;
	int 	m_VarLiquidType;
	int 	m_ItemBehaviour; // -1 = not specified; 0 = heavy item; 1= onehanded item; 2 = twohanded item
	int 	m_QuickBarBonus;
	bool	m_IsBeingPlaced;
	bool	m_IsHologram;
	bool	m_IsTakeable;
	bool 	m_ThrowItemOnDrop;
	bool 	m_ItemBeingDroppedPhys;
	bool    m_CanBeMovedOverride;
	bool 	m_FixDamageSystemInit = false; //can be changed on storage version check
	bool 	can_this_be_combined; //Check if item can be combined
	bool 	m_CanThisBeSplit; //Check if item can be split
	bool	m_IsStoreLoad = false;
	bool	m_CanShowQuantity;
	bool	m_HasQuantityBar;
	protected bool m_CanBeDigged;
	protected bool m_IsResultOfSplit //! distinguish if item has been created as new or it came from splitting (server only flag)
	
	string	m_SoundAttType;
	// items color variables
	int 	m_ColorComponentR;
	int 	m_ColorComponentG;
	int 	m_ColorComponentB;
	int 	m_ColorComponentA;
	//-------------------------------------------------------
	
	// light source managing
	ItemBase m_LightSourceItem;
	
	ref TIntArray m_SingleUseActions;
	ref TIntArray m_ContinuousActions;
	ref TIntArray m_InteractActions;
	
	//==============================================
	// agent system
	private int	m_AttachedAgents;

	//! appears to be deprecated, legacy code
	void TransferModifiers(PlayerBase reciever);
	
	
	// Weapons & suppressors particle effects
	ref static map<int, ref array<ref WeaponParticlesOnFire>> 				m_OnFireEffect;
	ref static map<int, ref array<ref WeaponParticlesOnBulletCasingEject>> 	m_OnBulletCasingEjectEffect;
	ref map<int, ref array<ref WeaponParticlesOnOverheating>> 				m_OnOverheatingEffect;
	ref static map<string, int> m_WeaponTypeToID;
	static int m_LastRegisteredWeaponID = 0;
	
	// Overheating effects
	bool 								m_IsOverheatingEffectActive;
	float								m_OverheatingShots;
	ref Timer 							m_CheckOverheating;
	int 								m_ShotsToStartOverheating = 0; // After these many shots, the overheating effect begins
	int 								m_MaxOverheatingValue = 0; // Limits the number of shots that will be tracked
	float 								m_OverheatingDecayInterval = 1; // Timer's interval for decrementing overheat effect's lifespan
	ref array <ref OverheatingParticle> m_OverheatingParticles;
	
	protected ref TStringArray m_HeadHidingSelections;
	protected bool m_HideSelectionsBySlot;
	
	// Admin Log
	PluginAdminLog			m_AdminLog;
	
	// misc
	ref Timer 				m_PhysDropTimer;

	// Attachment Locking variables
	ref array<int> 				m_CompatibleLocks;
	protected int				m_LockType;
	protected ref EffectSound 	m_LockingSound;
	protected string 			m_LockSoundSet;
	
	// ItemSoundHandler 
	protected const int ITEM_SOUNDS_MAX = 63;	// optimize network synch  
	protected int m_SoundSyncPlay;				// id for sound to play
	protected int m_SoundSyncStop;				// id for sound to stop
	private ref ItemSoundHandler m_ItemSoundHandler;
	
	//temperature
	private float 				m_TemperaturePerQuantityWeight;
	
	// -------------------------------------------------------------------------
	void ItemBase()
	{
		SetEventMask(EntityEvent.INIT); // Enable EOnInit event
		m_SingleUseActions = new TIntArray;
		m_ContinuousActions = new TIntArray;
		m_InteractActions = new TIntArray;
		
		if (!GetGame().IsDedicatedServer())
		{
			if (HasMuzzle())
			{
				LoadParticleConfigOnFire(GetMuzzleID());
				
				if (m_ShotsToStartOverheating == 0)
				{
					LoadParticleConfigOnOverheating(GetMuzzleID());
				}
			}
			
			PreLoadSoundAttachmentType();
			m_ActionsInitialize = false;
		}
		
		m_OldLocation = null;
		
		if (GetGame().IsServer())
		{
			m_AdminLog = PluginAdminLog.Cast(GetPlugin(PluginAdminLog));
		}
		
		if (ConfigIsExisting("headSelectionsToHide"))
		{
			m_HeadHidingSelections = new TStringArray;
			ConfigGetTextArray("headSelectionsToHide",m_HeadHidingSelections);
		}
		
		m_HideSelectionsBySlot = false;
		if (ConfigIsExisting("hideSelectionsByinventorySlot"))
		{
			m_HideSelectionsBySlot = ConfigGetBool("hideSelectionsByinventorySlot");
		}
		
		m_QuickBarBonus = Math.Max(0, ConfigGetInt("quickBarBonus"));

		m_IsResultOfSplit = false;
		
		SetActionAnimOverrides();
	}
	
	override void InitItemVariables()
	{
		super.InitItemVariables();
		
		m_VarQuantityInit = ConfigGetInt("varQuantityInit");
		m_VarQuantity = m_VarQuantityInit;//should be by the CE, this is just a precaution
		m_VarQuantityMin = ConfigGetInt("varQuantityMin");
		m_VarQuantityMax = ConfigGetInt("varQuantityMax");
		m_VarStackMax = ConfigGetFloat("varStackMax");
		m_Count = ConfigGetInt("count");
		
		m_CanShowQuantity = ConfigGetBool("quantityShow");
		m_HasQuantityBar = ConfigGetBool("quantityBar");
		
		m_CleannessInit = ConfigGetInt("varCleannessInit");
		m_Cleanness = m_CleannessInit;
		m_CleannessMin = ConfigGetInt("varCleannessMin");
		m_CleannessMax = ConfigGetInt("varCleannessMax");
		
		m_WantPlayImpactSound = false;
		m_ImpactSpeed = 0.0;
		
		m_VarWetInit = ConfigGetFloat("varWetInit");
		m_VarWet = m_VarWetInit;
		m_VarWetMin = ConfigGetFloat("varWetMin");
		m_VarWetMax = ConfigGetFloat("varWetMax");
		
		m_LiquidContainerMask = ConfigGetInt("liquidContainerType");
		if (IsLiquidContainer() && GetQuantity() != 0)
			m_VarLiquidType = GetLiquidTypeInit();
		m_IsBeingPlaced = false;
		m_IsHologram = false;
		m_IsTakeable = true;
		m_CanBeMovedOverride = false;
		m_HeatIsolation = GetHeatIsolationInit();
		m_ItemModelLength = GetItemModelLength();
		m_ItemAttachOffset = GetItemAttachOffset();
		m_CanBeDigged = ConfigGetBool("canBeDigged");
		
		m_CompatibleLocks = new array<int>();
		ConfigGetIntArray("compatibleLocks", m_CompatibleLocks);
		m_LockType = ConfigGetInt("lockType");
		
		//Define if item can be split and set ability to be combined accordingly
		m_CanThisBeSplit = false;
		can_this_be_combined = false;
		if (ConfigIsExisting("canBeSplit"))
		{
			can_this_be_combined = ConfigGetBool("canBeSplit");
			m_CanThisBeSplit = can_this_be_combined;
		}
		
		m_ItemBehaviour = -1;
		if (ConfigIsExisting("itemBehaviour"))
			m_ItemBehaviour = ConfigGetInt("itemBehaviour");
		
		//RegisterNetSyncVariableInt("m_VariablesMask");
		if (HasQuantity()) RegisterNetSyncVariableFloat("m_VarQuantity", GetQuantityMin(), m_VarQuantityMax);
		RegisterNetSyncVariableFloat("m_VarWet", GetWetMin(), GetWetMax(), 2);
		RegisterNetSyncVariableInt("m_VarLiquidType");
		RegisterNetSyncVariableInt("m_Cleanness",0,1);
		
		RegisterNetSyncVariableBoolSignal("m_WantPlayImpactSound");
		RegisterNetSyncVariableFloat("m_ImpactSpeed");
		RegisterNetSyncVariableInt("m_ImpactSoundSurfaceHash");
		
		RegisterNetSyncVariableInt("m_ColorComponentR", 0, 255);
		RegisterNetSyncVariableInt("m_ColorComponentG", 0, 255);
		RegisterNetSyncVariableInt("m_ColorComponentB", 0, 255);
		RegisterNetSyncVariableInt("m_ColorComponentA", 0, 255);
		
		RegisterNetSyncVariableBool("m_IsBeingPlaced");
		RegisterNetSyncVariableBool("m_IsTakeable");
		RegisterNetSyncVariableBool("m_IsHologram");
		
		InitItemSounds();		
		if (m_ItemSoundHandler)
		{
			RegisterNetSyncVariableInt("m_SoundSyncPlay", 0, ITEM_SOUNDS_MAX);
			RegisterNetSyncVariableInt("m_SoundSyncStop", 0, ITEM_SOUNDS_MAX);
		}
		
		m_LockSoundSet = ConfigGetString("lockSoundSet");
		
		m_TemperaturePerQuantityWeight = 1.0;;
		if (ConfigIsExisting("temperaturePerQuantityWeight"))
			m_TemperaturePerQuantityWeight = ConfigGetFloat("temperaturePerQuantityWeight");
			
	}
	
	override int GetQuickBarBonus()
	{
		return m_QuickBarBonus;
	}

	void InitializeActions()
	{
		m_InputActionMap = m_ItemTypeActionsMap.Get(this.Type());
		if (!m_InputActionMap)
		{
			TInputActionMap iam = new TInputActionMap;
			m_InputActionMap = iam;
			SetActions();
			m_ItemTypeActionsMap.Insert(this.Type(), m_InputActionMap);
		}
	}
	
	override void GetActions(typename action_input_type, out array<ActionBase_Basic> actions)
	{
		if (!m_ActionsInitialize)
		{
			m_ActionsInitialize = true;
			InitializeActions();
		}
		
		actions = m_InputActionMap.Get(action_input_type);
	}
	
	void SetActions()
	{
		AddAction(ActionTakeItem);
		AddAction(ActionTakeItemToHands);
		AddAction(ActionWorldCraft);
		AddAction(ActionDropItem);
		AddAction(ActionAttachWithSwitch);
	}
	
	void SetActionAnimOverrides();	// Override action animation for specific item
	
	void AddAction(typename actionName)
	{
		ActionBase action = ActionManagerBase.GetAction(actionName);

		if (!action)
		{
			Debug.LogError("Action " + actionName + " dosn't exist!");
			return;
		}		
		
		typename ai = action.GetInputType();
		if (!ai)
		{
			m_ActionsInitialize = false;
			return;
		}
		
		array<ActionBase_Basic> action_array = m_InputActionMap.Get(ai);		
		if (!action_array)
		{
			action_array = new array<ActionBase_Basic>;
			m_InputActionMap.Insert(ai, action_array);
		}
		if (LogManager.IsActionLogEnable())
		{
			Debug.ActionLog(action.ToString() + " -> " + ai, this.ToString() , "n/a", "Add action");
		}
		
		if (action_array.Find(action) != -1)
		{
			Debug.Log("Action " + action.Type() + " already added to " + this + ", skipping!");
		}
		else
		{
			action_array.Insert(action);
		}
	}
	
	void RemoveAction(typename actionName)
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		ActionBase action = player.GetActionManager().GetAction(actionName);
		typename ai = action.GetInputType();
		array<ActionBase_Basic> action_array = m_InputActionMap.Get(ai);
		
		if (action_array)
		{
			action_array.RemoveItem(action);
		}
	}
	
	// Allows override of default action command per item, defined in the SetActionAnimOverrides() of the item's class
	// Set -1 for params which should stay in default state
	void OverrideActionAnimation(typename action, int commandUID, int stanceMask = -1, int commandUIDProne = -1)
	{
		ActionOverrideData overrideData = new ActionOverrideData();
		overrideData.m_CommandUID = commandUID;
		overrideData.m_CommandUIDProne = commandUIDProne;
		overrideData.m_StanceMask = stanceMask;
		
		TActionAnimOverrideMap actionMap = m_ItemActionOverrides.Get(action);
		if (!actionMap)	// create new map of action > overidables map
		{
			actionMap = new TActionAnimOverrideMap();
			m_ItemActionOverrides.Insert(action, actionMap);
		}
		
		actionMap.Insert(this.Type(), overrideData); // insert item -> overrides
		
	}
	
	void OnItemInHandsPlayerSwimStart(PlayerBase player);
	
	ScriptedLightBase GetLight();
	
	// Loads muzzle flash particle configuration from config and saves it to a map for faster access
	void LoadParticleConfigOnFire(int id)
	{
		if (!m_OnFireEffect)
			m_OnFireEffect = new map<int, ref array<ref WeaponParticlesOnFire>>;
		
		if (!m_OnBulletCasingEjectEffect)
			m_OnBulletCasingEjectEffect = new map<int, ref array<ref WeaponParticlesOnBulletCasingEject>>;
		
		string config_to_search = "CfgVehicles";
		string muzzle_owner_config;
		
		if (!m_OnFireEffect.Contains(id))
		{
			if (IsInherited(Weapon))
				config_to_search = "CfgWeapons";	
			
			muzzle_owner_config = config_to_search + " " + GetType() + " ";
			
			string config_OnFire_class = muzzle_owner_config + "Particles " + "OnFire ";
			
			int config_OnFire_subclass_count = GetGame().ConfigGetChildrenCount(config_OnFire_class);
			
			if (config_OnFire_subclass_count > 0)
			{
				array<ref WeaponParticlesOnFire> WPOF_array = new array<ref WeaponParticlesOnFire>;
				
				for (int i = 0; i < config_OnFire_subclass_count; i++)
				{
					string particle_class = "";
					GetGame().ConfigGetChildName(config_OnFire_class, i, particle_class);
					string config_OnFire_entry = config_OnFire_class + particle_class;
					WeaponParticlesOnFire WPOF = new WeaponParticlesOnFire(this, config_OnFire_entry);
					WPOF_array.Insert(WPOF);
				}
				
				
				m_OnFireEffect.Insert(id, WPOF_array);
			}
		}
		
		if (!m_OnBulletCasingEjectEffect.Contains(id))
		{
			config_to_search = "CfgWeapons"; // Bullet Eject efect is supported on weapons only.
			muzzle_owner_config = config_to_search + " " + GetType() + " ";
			
			string config_OnBulletCasingEject_class = muzzle_owner_config + "Particles " + "OnBulletCasingEject ";
			
			int config_OnBulletCasingEject_count = GetGame().ConfigGetChildrenCount(config_OnBulletCasingEject_class);
			
			if (config_OnBulletCasingEject_count > 0  &&  IsInherited(Weapon))
			{
				array<ref WeaponParticlesOnBulletCasingEject> WPOBE_array = new array<ref WeaponParticlesOnBulletCasingEject>;
				
				for (i = 0; i < config_OnBulletCasingEject_count; i++)
				{
					string particle_class2 = "";
					GetGame().ConfigGetChildName(config_OnBulletCasingEject_class, i, particle_class2);
					string config_OnBulletCasingEject_entry = config_OnBulletCasingEject_class + particle_class2;
					WeaponParticlesOnBulletCasingEject WPOBE = new WeaponParticlesOnBulletCasingEject(this, config_OnBulletCasingEject_entry);
					WPOBE_array.Insert(WPOBE);
				}
				
				
				m_OnBulletCasingEjectEffect.Insert(id, WPOBE_array);
			}
		}
	}
	
	// Loads muzzle flash particle configuration from config and saves it to a map for faster access
	void LoadParticleConfigOnOverheating(int id)
	{
		if (!m_OnOverheatingEffect)
			m_OnOverheatingEffect = new map<int, ref array<ref WeaponParticlesOnOverheating>>;
		
		if (!m_OnOverheatingEffect.Contains(id))
		{
			string config_to_search = "CfgVehicles";
			
			if (IsInherited(Weapon))
				config_to_search = "CfgWeapons";
			
			string muzzle_owner_config = config_to_search + " " + GetType() + " ";
			string config_OnOverheating_class = muzzle_owner_config + "Particles " + "OnOverheating ";
			
			if (GetGame().ConfigIsExisting(config_OnOverheating_class))
			{
				
				m_ShotsToStartOverheating = GetGame().ConfigGetFloat(config_OnOverheating_class + "shotsToStartOverheating");
				
				if (m_ShotsToStartOverheating == 0)
				{
					m_ShotsToStartOverheating = -1; // This prevents futher readings from config for future creations of this item
					string error = "Error reading config " + GetType() + ">Particles>OnOverheating - Parameter shotsToStartOverheating is configured wrong or is missing! Its value must be 1 or higher!";
					Error(error);
					return;
				}
				
				m_OverheatingDecayInterval = GetGame().ConfigGetFloat(config_OnOverheating_class + "overheatingDecayInterval");
				m_MaxOverheatingValue = GetGame().ConfigGetFloat(config_OnOverheating_class + "maxOverheatingValue");
				
				
				
				int config_OnOverheating_subclass_count = GetGame().ConfigGetChildrenCount(config_OnOverheating_class);
				array<ref WeaponParticlesOnOverheating> WPOOH_array = new array<ref WeaponParticlesOnOverheating>;
				
				for (int i = 0; i < config_OnOverheating_subclass_count; i++)
				{
					string particle_class = "";
					GetGame().ConfigGetChildName(config_OnOverheating_class, i, particle_class);
					string config_OnOverheating_entry = config_OnOverheating_class + particle_class;
					int  entry_type = GetGame().ConfigGetType(config_OnOverheating_entry);
					
					if (entry_type == CT_CLASS)
					{
						WeaponParticlesOnOverheating WPOF = new WeaponParticlesOnOverheating(this, config_OnOverheating_entry);
						WPOOH_array.Insert(WPOF);
					}
				}
				
				
				m_OnOverheatingEffect.Insert(id, WPOOH_array);
			}
		}
	}
	
	float GetOverheatingValue()
	{
		return m_OverheatingShots;
	}
	
	void IncreaseOverheating(ItemBase weapon, string ammoType, ItemBase muzzle_owner, ItemBase suppressor, string config_to_search)
	{
		if (m_MaxOverheatingValue > 0)
		{
			m_OverheatingShots++;
			
			if (!m_CheckOverheating)
					m_CheckOverheating = new Timer(CALL_CATEGORY_SYSTEM);
			
			m_CheckOverheating.Stop();
			m_CheckOverheating.Run(m_OverheatingDecayInterval, this, "OnOverheatingDecay");
			
			CheckOverheating(weapon, ammoType, muzzle_owner, suppressor, config_to_search);
		}
	}
	
	void CheckOverheating(ItemBase weapon = null, string ammoType = "", ItemBase muzzle_owner = null, ItemBase suppressor = null, string config_to_search = "")
	{
		if (m_OverheatingShots >= m_ShotsToStartOverheating  &&  IsOverheatingEffectActive())
			UpdateOverheating(weapon, ammoType, muzzle_owner, suppressor, config_to_search);
		
		if (m_OverheatingShots >= m_ShotsToStartOverheating  &&  !IsOverheatingEffectActive())
			StartOverheating(weapon, ammoType, muzzle_owner, suppressor, config_to_search);
		
		if (m_OverheatingShots < m_ShotsToStartOverheating  &&  IsOverheatingEffectActive())
			StopOverheating(weapon, ammoType, muzzle_owner, suppressor, config_to_search);
		
		if (m_OverheatingShots > m_MaxOverheatingValue)
		{
			m_OverheatingShots = m_MaxOverheatingValue;
		}
	}
	
	bool IsOverheatingEffectActive()
	{
		return m_IsOverheatingEffectActive;
	}
	
	void OnOverheatingDecay()
	{
		if (m_MaxOverheatingValue > 0)
			m_OverheatingShots -= 1 + m_OverheatingShots / m_MaxOverheatingValue; // The hotter a barrel is, the faster it needs to cool down.
		else
			m_OverheatingShots--;
		
		if (m_OverheatingShots <= 0)
		{
			m_CheckOverheating.Stop();
			m_OverheatingShots = 0;
		}
		else
		{
			if (!m_CheckOverheating)
				m_CheckOverheating = new Timer(CALL_CATEGORY_GAMEPLAY);
			
			m_CheckOverheating.Stop();
			m_CheckOverheating.Run(m_OverheatingDecayInterval, this, "OnOverheatingDecay");
		}
		
		CheckOverheating(this, "", this);
	}

	void StartOverheating(ItemBase weapon = null, string ammoType = "", ItemBase muzzle_owner = null, ItemBase suppressor = null, string config_to_search = "")
	{
		m_IsOverheatingEffectActive = true;
		ItemBase.PlayOverheatingParticles(this, ammoType, this, suppressor, "CfgWeapons");
	}
	
	void UpdateOverheating(ItemBase weapon = null, string ammoType = "", ItemBase muzzle_owner = null, ItemBase suppressor = null, string config_to_search = "")
	{
		KillAllOverheatingParticles();
		ItemBase.UpdateOverheatingParticles(this, ammoType, this, suppressor, "CfgWeapons");
		UpdateAllOverheatingParticles();
	}
	
	void StopOverheating(ItemBase weapon = null, string ammoType = "", ItemBase muzzle_owner = null, ItemBase suppressor = null, string config_to_search = "")
	{
		m_IsOverheatingEffectActive = false;
		ItemBase.StopOverheatingParticles(weapon, ammoType, muzzle_owner, suppressor, config_to_search);
	}
	
	void RegisterOverheatingParticle(Particle p, float min_heat_coef, float max_heat_coef, int particle_id, Object parent, vector local_pos, vector local_ori)
	{
		if (!m_OverheatingParticles)
			m_OverheatingParticles = new array<ref OverheatingParticle>;
		
		OverheatingParticle OP = new OverheatingParticle();
		OP.RegisterParticle(p);
		OP.SetOverheatingLimitMin(min_heat_coef);
		OP.SetOverheatingLimitMax(max_heat_coef);
		OP.SetParticleParams(particle_id, parent, local_pos, local_ori);
		
		m_OverheatingParticles.Insert(OP);
	}
	
	float GetOverheatingCoef()
	{
		if (m_MaxOverheatingValue > 0)
			return (m_OverheatingShots - m_ShotsToStartOverheating) / m_MaxOverheatingValue;
		
		return -1;
	}
	
	void UpdateAllOverheatingParticles()
	{
		if (m_OverheatingParticles)
		{
			float overheat_coef = GetOverheatingCoef();
			int count = m_OverheatingParticles.Count();
			
			for (int i = count; i > 0; --i)
			{
				int id = i - 1;
				OverheatingParticle OP = m_OverheatingParticles.Get(id);
				Particle p = OP.GetParticle();
				
				float overheat_min = OP.GetOverheatingLimitMin();
				float overheat_max = OP.GetOverheatingLimitMax();
				
				if (overheat_coef < overheat_min  &&  overheat_coef >= overheat_max)
				{
					if (p)
					{
						p.Stop();
						OP.RegisterParticle(null);
					}
				}
			}
		}
	}
	
	void KillAllOverheatingParticles()
	{
		if (m_OverheatingParticles)
		{
			for (int i = m_OverheatingParticles.Count(); i > 0; i--)
			{
				int id = i - 1;
				OverheatingParticle OP = m_OverheatingParticles.Get(id);
				
				if (OP)
				{
					Particle p = OP.GetParticle();
					
					if (p)
					{
						p.Stop();
					}
					
					delete OP;
				}
			}
			
			m_OverheatingParticles.Clear();
			delete m_OverheatingParticles;
		}
	}
	
	//! Infection chance while/after using this item, originally used for wound infection after bandaging, params 'system' and 'param' can allow usage by other systems as well
	float GetInfectionChance(int system = 0, Param param = null)
	{
		return 0.0;
	}
	
	
	float GetDisinfectQuantity(int system = 0, Param param1 = null)
	{
		return 250;//default value
	}
	
	float GetFilterDamageRatio() 
	{
		return 0;
	}
	
	//! Returns true if this item has a muzzle (weapons, suppressors)
	bool HasMuzzle()
	{
		if (IsInherited(Weapon)  ||  IsInherited(SuppressorBase))
			return true;
		
		return false;
	}
	
	//! Returns global muzzle ID. If not found, then it gets automatically registered.
	int GetMuzzleID()
	{
		if (!m_WeaponTypeToID)
			m_WeaponTypeToID = new map<string, int>;
		
		if (m_WeaponTypeToID.Contains(GetType()))
		{
			return m_WeaponTypeToID.Get(GetType());
		}
		else 
		{
			// Register new weapon ID
			m_WeaponTypeToID.Insert(GetType(), ++m_LastRegisteredWeaponID);
		}
		
		return m_LastRegisteredWeaponID;
	}
	
	/**
	\brief Re-sets DamageSystem changes
	\return storage version on which the config changes occured (default -1, to be overriden!)
	\note Significant changes to DamageSystem in item configs have to be re-set by increasing the storage version and overriding this method. Default return is -1 (does nothing).
	*/
	int GetDamageSystemVersionChange()
	{
		return -1;
	}
	
	
	
	// -------------------------------------------------------------------------
	void ~ItemBase()
	{
		if (GetGame() && GetGame().GetPlayer() && (!GetGame().IsDedicatedServer()))
		{
			PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
			int r_index = player.GetHumanInventory().FindUserReservedLocationIndex(this);

			if (r_index >= 0)
			{
					InventoryLocation r_il = new InventoryLocation;
					player.GetHumanInventory().GetUserReservedLocation(r_index,r_il);

					player.GetHumanInventory().ClearUserReservedLocationAtIndex(r_index);
					int r_type = r_il.GetType();
					if (r_type == InventoryLocationType.CARGO || r_type == InventoryLocationType.PROXYCARGO)
					{
						r_il.GetParent().GetOnReleaseLock().Invoke(this);
					}
					else if (r_type == InventoryLocationType.ATTACHMENT)
					{
						r_il.GetParent().GetOnAttachmentReleaseLock().Invoke(this, r_il.GetSlot());
					}
			
			}
			
			player.GetHumanInventory().ClearUserReservedLocation(this);
		}
		
		if (m_LockingSound)
			SEffectManager.DestroyEffect(m_LockingSound);
	}

	
	
	// -------------------------------------------------------------------------
	static int GetDebugActionsMask()
	{
		return ItemBase.m_DebugActionsMask;	
	}
	
	static bool HasDebugActionsMask(int mask)
	{
		return ItemBase.m_DebugActionsMask & mask;	
	}
	
	static void SetDebugActionsMask(int mask)
	{
		ItemBase.m_DebugActionsMask = mask;
	}
	
	static void AddDebugActionsMask(int mask)
	{
		ItemBase.m_DebugActionsMask |= mask;
	}
	
	static void RemoveDebugActionsMask(int mask)
	{
		ItemBase.m_DebugActionsMask &= ~mask;
	}
	
	static void ToggleDebugActionsMask(int mask)
	{
		if (HasDebugActionsMask(mask))
		{
			RemoveDebugActionsMask(mask);
		}
		else
		{
			AddDebugActionsMask(mask);
		}
	}
	
	// -------------------------------------------------------------------------
	void SetCEBasedQuantity()
	{
		if (GetEconomyProfile())
		{
			float q_max = GetEconomyProfile().GetQuantityMax();
			if (q_max > 0)
			{
				float q_min = GetEconomyProfile().GetQuantityMin();
				float quantity_randomized = Math.RandomFloatInclusive(q_min, q_max);
				
				if (HasComponent(COMP_TYPE_ENERGY_MANAGER))//more direct access for speed
				{
					ComponentEnergyManager comp = GetCompEM();
					if (comp && (comp.GetEnergyMaxPristine() || comp.GetEnergyAtSpawn()))//checking for a potential for energy, we need to check both values, as both are optional, only when both are set to 0, we know the item can't have energy
					{
						comp.SetEnergy0To1(quantity_randomized);
					}
				}
				else if (HasQuantity())
				{
					SetQuantityNormalized(quantity_randomized, false);
					//PrintString("<==> Normalized quantity for item: "+ GetType()+", qmin:"+q_min.ToString()+"; qmax:"+q_max.ToString()+";quantity:" +quantity_randomized.ToString());
				}
				
			}
		}
	}
	
	//! Locks this item in it's current attachment slot of its parent. This makes the "locked" icon visible in inventory over this item.
	void LockToParent()
	{
		EntityAI parent = GetHierarchyParent();
		
		if (parent)
		{
			InventoryLocation inventory_location_to_lock = new InventoryLocation;
			GetInventory().GetCurrentInventoryLocation(inventory_location_to_lock);
			parent.GetInventory().SetSlotLock(inventory_location_to_lock.GetSlot(), true);
		}
	}
	
	//! Unlocks this item from its attachment slot of its parent.
	void UnlockFromParent()
	{
		EntityAI parent = GetHierarchyParent();
		
		if (parent)
		{
			InventoryLocation inventory_location_to_unlock = new InventoryLocation;
			GetInventory().GetCurrentInventoryLocation(inventory_location_to_unlock);
			parent.GetInventory().SetSlotLock(inventory_location_to_unlock.GetSlot(), false);
		}
	}
	
	override void CombineItemsClient(EntityAI entity2, bool use_stack_max = true)
	{
		/*
		ref Param1<EntityAI> item = new Param1<EntityAI>(entity2);
		RPCSingleParam(ERPCs.RPC_ITEM_COMBINE, item, GetGame().GetPlayer());
		*/
		ItemBase item2 = ItemBase.Cast(entity2);
		
		if (GetGame().IsClient())
		{
			if (ScriptInputUserData.CanStoreInputUserData())
			{
				ScriptInputUserData ctx = new ScriptInputUserData;
				ctx.Write(INPUT_UDT_ITEM_MANIPULATION);
				ctx.Write(-1);
				ItemBase i1 = this; // @NOTE: workaround for correct serialization
				ctx.Write(i1);
				ctx.Write(item2);
				ctx.Write(use_stack_max);
				ctx.Write(-1);
				ctx.Send();
				
				if (IsCombineAll(item2, use_stack_max))
				{
					GetGame().GetPlayer().GetInventory().AddInventoryReservationEx(item2,null,GameInventory.c_InventoryReservationTimeoutShortMS);
				}
			}
		}
		else if (!GetGame().IsMultiplayer())
		{
			CombineItems(item2, use_stack_max);
		}
	}
	
	bool IsLiquidPresent()
	{
		return (GetLiquidType() != 0 && HasQuantity());
	}
	
	bool IsLiquidContainer()
	{
		return m_LiquidContainerMask != 0;
	}
	
	int GetLiquidContainerMask()
	{
		return m_LiquidContainerMask;
	}
	
	bool IsBloodContainer()
	{
		//m_LiquidContainerMask & GROUP_LIQUID_BLOOD ???
		return false;
	}
	
	bool IsNVG()
	{
		return false;
	}
	
	//! explosive 
	//! ------------
	bool IsExplosive()
	{
		return false;
	}
	
	string GetExplosiveTriggerSlotName()
	{
		return "";
	}
	
	//! ------------
	
	bool IsLightSource()
	{
		return false;
	}
	
	bool CanBeRepairedByCrafting()
	{
		return true;
	}
	
	//--- ACTION CONDITIONS
	//direction
	bool IsFacingPlayer(PlayerBase player, string selection)
	{
		return true;
	}
	
	bool IsPlayerInside(PlayerBase player, string selection)
	{
		return true;
	}
	
	override bool CanObstruct()
	{
		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		return !player || !IsPlayerInside(player, "");
	}
	
	override bool IsBeingPlaced()
	{
		return m_IsBeingPlaced;
	}
	
	void SetIsBeingPlaced(bool is_being_placed)
	{
		m_IsBeingPlaced = is_being_placed;
		if (!is_being_placed)
			OnEndPlacement();
		SetSynchDirty();
	}
	
	//server-side
	void OnEndPlacement() {}
	
	override bool IsHologram()
	{
		return m_IsHologram;
	}
	
	bool CanBeDigged()
	{
		return m_CanBeDigged;
	}
	
	int GetOnDigWormsAmount()
	{
		return 1;
	}
	
	bool CanMakeGardenplot()
	{
		return false;
	}
	
	void SetIsHologram(bool is_hologram)
	{
		m_IsHologram = is_hologram;
		SetSynchDirty();
	}
	/*
	protected float GetNutritionalEnergy()
	{
		Edible_Base edible = Edible_Base.Cast(this);
		return edible.GetFoodEnergy();
	}
	
	protected float GetNutritionalWaterContent()
	{
		Edible_Base edible = Edible_Base.Cast(this);
		return edible.GetFoodWater();
	}
	
	protected float GetNutritionalIndex()
	{
		Edible_Base edible = Edible_Base.Cast(this);
		return edible.GetFoodNutritionalIndex();
	}
	
	protected float GetNutritionalFullnessIndex()
	{
		Edible_Base edible = Edible_Base.Cast(this);
		return edible.GetFoodTotalVolume();
	}
	
	protected float GetNutritionalToxicity()
	{
		Edible_Base edible = Edible_Base.Cast(this);
		return edible.GetFoodToxicity();

	}
	*/
	
	
	// -------------------------------------------------------------------------
	override void OnMovedInsideCargo(EntityAI container)
	{
		super.OnMovedInsideCargo(container);
		
		MiscGameplayFunctions.RemoveAllAttachedChildrenByTypename(this, {Bolt_Base});
	}
	
	override void EEItemLocationChanged(notnull InventoryLocation oldLoc, notnull InventoryLocation newLoc)
	{
		super.EEItemLocationChanged(oldLoc,newLoc);
		
		PlayerBase new_player = null;
		PlayerBase old_player = null;
		
		if (newLoc.GetParent())
			new_player = PlayerBase.Cast(newLoc.GetParent().GetHierarchyRootPlayer());
		
		if (oldLoc.GetParent())
			old_player = PlayerBase.Cast(oldLoc.GetParent().GetHierarchyRootPlayer());
		
		if (old_player && oldLoc.GetType() == InventoryLocationType.HANDS)
		{
			int r_index = old_player.GetHumanInventory().FindUserReservedLocationIndex(this);

			if (r_index >= 0)
			{
					InventoryLocation r_il = new InventoryLocation;
					old_player.GetHumanInventory().GetUserReservedLocation(r_index,r_il);

					old_player.GetHumanInventory().ClearUserReservedLocationAtIndex(r_index);
					int r_type = r_il.GetType();
					if (r_type == InventoryLocationType.CARGO || r_type == InventoryLocationType.PROXYCARGO)
					{
						r_il.GetParent().GetOnReleaseLock().Invoke(this);
					}
					else if (r_type == InventoryLocationType.ATTACHMENT)
					{
						r_il.GetParent().GetOnAttachmentReleaseLock().Invoke(this, r_il.GetSlot());
					}
			
			}
		}
		
		if (newLoc.GetType() == InventoryLocationType.HANDS)
		{
			if (new_player)
				new_player.ForceStandUpForHeavyItems(newLoc.GetItem());
			
			if (new_player == old_player)
			{
				
				if (oldLoc.GetParent() && new_player.GetHumanInventory().LocationGetEntity(oldLoc) == NULL)
				{
					if (oldLoc.GetType() == InventoryLocationType.CARGO)
					{
						if (oldLoc.GetParent().GetInventory().TestAddEntityInCargoExLoc(oldLoc, false, false, false, true, false, false))
						{
							new_player.GetHumanInventory().SetUserReservedLocation(this,oldLoc);
						}
					}
					else
					{
						new_player.GetHumanInventory().SetUserReservedLocation(this,oldLoc);
					}
				}
				
				if (new_player.GetHumanInventory().FindUserReservedLocationIndex(this) >= 0)
				{
					int type = oldLoc.GetType();
					if (type == InventoryLocationType.CARGO || type == InventoryLocationType.PROXYCARGO)
					{
						oldLoc.GetParent().GetOnSetLock().Invoke(this);
					}
					else if (type == InventoryLocationType.ATTACHMENT)
					{
						oldLoc.GetParent().GetOnAttachmentSetLock().Invoke(this, oldLoc.GetSlot());
					}
				}
				if (!m_OldLocation)
				{
					m_OldLocation = new InventoryLocation;
				}
				m_OldLocation.Copy(oldLoc);
			}
			else
			{
				if (m_OldLocation)
				{
					m_OldLocation.Reset();
				}
			}
			
			GetGame().GetAnalyticsClient().OnItemAttachedAtPlayer(this,"Hands");	
		}
		else
		{
			if (new_player)
			{
				int res_index = new_player.GetHumanInventory().FindCollidingUserReservedLocationIndex(this, newLoc);
				if (res_index >= 0)
				{
					InventoryLocation il = new InventoryLocation;
					new_player.GetHumanInventory().GetUserReservedLocation(res_index,il);
					ItemBase it = ItemBase.Cast(il.GetItem());
					new_player.GetHumanInventory().ClearUserReservedLocationAtIndex(res_index);
					int rel_type = il.GetType();
					if (rel_type == InventoryLocationType.CARGO || rel_type == InventoryLocationType.PROXYCARGO)
					{
						il.GetParent().GetOnReleaseLock().Invoke(it);
					}
					else if (rel_type == InventoryLocationType.ATTACHMENT)
					{
						il.GetParent().GetOnAttachmentReleaseLock().Invoke(it, il.GetSlot());
					}
					//it.GetOnReleaseLock().Invoke(it);
				}
			}
			else if (old_player && newLoc.GetType() == InventoryLocationType.GROUND && m_ThrowItemOnDrop)
			{
				//ThrowPhysically(old_player, vector.Zero);
				m_ThrowItemOnDrop = false;
			}
		
			if (m_OldLocation)
			{
				m_OldLocation.Reset();
			}
		}
	}
	
	override void EOnContact(IEntity other, Contact extra)
	{
		if (m_CanPlayImpactSound)
		{
			int liquidType = -1;
			float impactSpeed = ProcessImpactSoundEx(other, extra, m_ConfigWeight, m_ImpactSoundSurfaceHash, liquidType);
			if (impactSpeed > 0.0)
			{
				m_ImpactSpeed  = impactSpeed;
				#ifndef SERVER
				PlayImpactSound(m_ConfigWeight, m_ImpactSpeed, m_ImpactSoundSurfaceHash);
				#else
				m_WantPlayImpactSound = true;
				SetSynchDirty();
				#endif
				m_CanPlayImpactSound = (liquidType == -1);// prevents further playing of the sound when the surface is a liquid type
			}
		}
		
		#ifdef SERVER
		if (GetCompEM() && GetCompEM().IsPlugged())
		{
			if (GetCompEM().GetCordLength() < vector.Distance(GetPosition(), GetCompEM().GetEnergySource().GetPosition()))
				GetCompEM().UnplugThis();
		}
		#endif
	}
	
	void RefreshPhysics();
	
	override void OnCreatePhysics()
	{
		RefreshPhysics();
	}
	
	override void OnItemAttachmentSlotChanged(notnull InventoryLocation oldLoc, notnull InventoryLocation newLoc)
	{
		
	}
	// -------------------------------------------------------------------------
	override void OnItemLocationChanged(EntityAI old_owner, EntityAI new_owner)
	{
		super.OnItemLocationChanged(old_owner, new_owner);
				
		PlayerBase relatedPlayer = PlayerBase.Cast(old_owner);
		PlayerBase playerNew = PlayerBase.Cast(new_owner);
		
		if (!relatedPlayer && playerNew)
			relatedPlayer = playerNew;
		
		if (relatedPlayer && relatedPlayer.GetPerformedActionID() != -1)
		{
			ActionManagerBase actionMgr = relatedPlayer.GetActionManager();
			if (actionMgr)
			{
				ActionBase currentAction = actionMgr.GetRunningAction();
				if (currentAction)
					currentAction.OnItemLocationChanged(this);
			}
		}
		
		Man ownerPlayerOld = null;
		Man ownerPlayerNew = null;
			
		if (old_owner)   
		{
			if (old_owner.IsMan())
			{
				ownerPlayerOld = Man.Cast(old_owner);
			}
			else
			{
				ownerPlayerOld = Man.Cast(old_owner.GetHierarchyRootPlayer());
			}
		}
		else 
		{
			if (new_owner && IsElectricAppliance() && GetCompEM() && GetCompEM().IsPlugged())
			{
				ActionBase action = ActionManagerBase.GetAction(ActionRepositionPluggedItem);
				
				if (!action || !playerNew || playerNew.GetPerformedActionID() != action.GetID())
				{
    				GetCompEM().UnplugThis();
				}
			}
		}
		
		if (new_owner)
		{
			if (new_owner.IsMan())
			{
				ownerPlayerNew = Man.Cast(new_owner);
			}
			else
			{
				ownerPlayerNew = Man.Cast(new_owner.GetHierarchyRootPlayer());
			}
		}
		
		if (ownerPlayerOld != ownerPlayerNew)
		{
			if (ownerPlayerOld)
			{
				array<EntityAI> subItemsExit = new array<EntityAI>;
				GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER,subItemsExit);
				for (int i = 0; i < subItemsExit.Count(); i++)
				{
					ItemBase itemExit = ItemBase.Cast(subItemsExit.Get(i));
					itemExit.OnInventoryExit(ownerPlayerOld);
				}
			}

			if (ownerPlayerNew)
			{
				array<EntityAI> subItemsEnter = new array<EntityAI>;
				GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER,subItemsEnter);
				for (int j = 0; j < subItemsEnter.Count(); j++)
				{
					ItemBase itemEnter = ItemBase.Cast(subItemsEnter.Get(j));
					itemEnter.OnInventoryEnter(ownerPlayerNew);
				}
			}
		}
		else if (ownerPlayerNew != null)
		{
			PlayerBase nplayer;
			if (PlayerBase.CastTo(nplayer, ownerPlayerNew))
			{
				array<EntityAI> subItemsUpdate = new array<EntityAI>;
				GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER,subItemsUpdate);
				for (int k = 0; k < subItemsUpdate.Count(); k++)
				{
					ItemBase itemUpdate = ItemBase.Cast(subItemsUpdate.Get(k));
					itemUpdate.UpdateQuickbarShortcutVisibility(nplayer);
				}
			}
		}
		
		if (old_owner)
			old_owner.OnChildItemRemoved(this);
		if (new_owner)
			new_owner.OnChildItemReceived(this);
	}

	// -------------------------------------------------------------------------------
	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);
		PlayerBase player = PlayerBase.Cast(GetHierarchyRootPlayer());
		if (player)
		{
			OnInventoryExit(player);
			
			if (player.IsAlive())
			{
				int r_index = player.GetHumanInventory().FindUserReservedLocationIndex(this);
				if (r_index >= 0)
				{			
					InventoryLocation r_il = new InventoryLocation;
					player.GetHumanInventory().GetUserReservedLocation(r_index,r_il);
	
					player.GetHumanInventory().ClearUserReservedLocationAtIndex(r_index);
					int r_type = r_il.GetType();
					if (r_type == InventoryLocationType.CARGO || r_type == InventoryLocationType.PROXYCARGO)
					{
						r_il.GetParent().GetOnReleaseLock().Invoke(this);
					}
					else if (r_type == InventoryLocationType.ATTACHMENT)
					{
						r_il.GetParent().GetOnAttachmentReleaseLock().Invoke(this, r_il.GetSlot());
					}
				
				}
				
				player.RemoveQuickBarEntityShortcut(this);
			}
		}
	}
	// -------------------------------------------------------------------------------
	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);

		//! item is able to explode in fire
		if (killer && killer.IsFireplace() && CanExplodeInFire())
		{
			if (GetTemperature() >= GameConstants.ITEM_TEMPERATURE_TO_EXPLODE_MIN)
			{
				if (IsMagazine())
				{
					if (Magazine.Cast(this).GetAmmoCount() > 0)
					{
						ExplodeAmmo();
					}
				}
				else
				{
					Explode(DamageType.EXPLOSION);
				}
			}
		}
	}
	
	override void OnWasAttached(EntityAI parent, int slot_id)
	{
		MiscGameplayFunctions.RemoveAllAttachedChildrenByTypename(this, {Bolt_Base});
		
		super.OnWasAttached(parent, slot_id);
		
		if (HasQuantity())
			UpdateNetSyncVariableFloat("m_VarQuantity", GetQuantityMin(), m_VarQuantityMax);
		
		PlayAttachSound(InventorySlots.GetSlotName(slot_id));
	}
	
	override void OnWasDetached(EntityAI parent, int slot_id)
	{
		super.OnWasDetached(parent, slot_id);
		
		if (HasQuantity())
			UpdateNetSyncVariableFloat("m_VarQuantity", GetQuantityMin(), m_VarQuantityMax);
	}
	
	override string ChangeIntoOnAttach(string slot)
	{
		int idx;
		TStringArray inventory_slots = new TStringArray;
		TStringArray attach_types = new TStringArray;
		
		ConfigGetTextArray("ChangeInventorySlot",inventory_slots);
		if (inventory_slots.Count() < 1) //is string
		{
			inventory_slots.Insert(ConfigGetString("ChangeInventorySlot"));
			attach_types.Insert(ConfigGetString("ChangeIntoOnAttach"));
		}
		else //is array
		{
			ConfigGetTextArray("ChangeIntoOnAttach",attach_types);
		}
		
		idx = inventory_slots.Find(slot);
		if (idx < 0)
			return "";
		
		return attach_types.Get(idx);
	}
	
	override string ChangeIntoOnDetach()
	{
		int idx = -1;
		string slot;
		
		TStringArray inventory_slots = new TStringArray;
		TStringArray detach_types = new TStringArray;
		
		this.ConfigGetTextArray("ChangeInventorySlot",inventory_slots);
		if (inventory_slots.Count() < 1) //is string
		{
			inventory_slots.Insert(this.ConfigGetString("ChangeInventorySlot"));
			detach_types.Insert(this.ConfigGetString("ChangeIntoOnDetach"));
		}
		else //is array
		{
			this.ConfigGetTextArray("ChangeIntoOnDetach",detach_types);
			if (detach_types.Count() < 1)
				detach_types.Insert(this.ConfigGetString("ChangeIntoOnDetach"));
		}
		
		for (int i = 0; i < inventory_slots.Count(); i++)
		{
			slot = inventory_slots.Get(i);
		}
		
		if (slot != "")
		{
			if (detach_types.Count() == 1)
				idx = 0;
			else
				idx = inventory_slots.Find(slot);
		}
		if (idx < 0)
			return "";
	
		return detach_types.Get(idx);
	}
	
	void ExplodeAmmo()
	{
		//timer
		ref Timer explode_timer = new Timer(CALL_CATEGORY_SYSTEM);
		
		//min/max time
		float min_time = 1;
		float max_time = 3;
		float delay = Math.RandomFloat(min_time, max_time);
		
		explode_timer.Run(delay, this, "DoAmmoExplosion");
	}
	
	void DoAmmoExplosion()
	{
		Magazine magazine = Magazine.Cast(this);
		int pop_sounds_count = 6;
		string pop_sounds[ 6 ] = { "ammopops_1","ammopops_2","ammopops_3","ammopops_4","ammopops_5","ammopops_6" };
		
		//play sound
		int sound_idx = Math.RandomInt(0, pop_sounds_count - 1);
		string sound_name = pop_sounds[ sound_idx ];
		GetGame().CreateSoundOnObject(this, sound_name, 20, false);
		
		//remove ammo count
		magazine.ServerAddAmmoCount(-1);
		
		//if condition then repeat -> ExplodeAmmo
		float min_temp_to_explode	= 100;		//min temperature for item to explode
				
		if (magazine.GetAmmoCount() > 0 && GetTemperature() >= min_temp_to_explode)	//TODO ? add check for parent -> fireplace
		{
			ExplodeAmmo();
		}
	}
	
	// -------------------------------------------------------------------------------
	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
		
		const int CHANCE_DAMAGE_CARGO = 4;
		const int CHANCE_DAMAGE_ATTACHMENT = 1;
		const int CHANCE_DAMAGE_NOTHING = 2;
		
		if (IsClothing() || IsContainer() || IsItemTent())
		{
			float dmg = damageResult.GetDamage("","Health") * -0.5;
			int chances;
			int rnd;
			
			if (GetInventory().GetCargo())
			{
				chances = CHANCE_DAMAGE_CARGO + CHANCE_DAMAGE_ATTACHMENT + CHANCE_DAMAGE_NOTHING;
				rnd = Math.RandomInt(0,chances);
				
				if (rnd < CHANCE_DAMAGE_CARGO)
				{
					DamageItemInCargo(dmg);
				}
				else if (rnd < (chances - CHANCE_DAMAGE_NOTHING))
				{
					DamageItemAttachments(dmg);
				}
			}
			else
			{
				chances = CHANCE_DAMAGE_ATTACHMENT + CHANCE_DAMAGE_NOTHING;
				rnd = Math.RandomInt(0,chances);
				
				if (rnd < CHANCE_DAMAGE_ATTACHMENT)
				{
					DamageItemAttachments(dmg);
				}
			}
		}
	}
	
	bool DamageItemInCargo(float damage)
	{
		if (GetInventory().GetCargo())
		{
			int item_count = GetInventory().GetCargo().GetItemCount();
			if (item_count > 0)
			{
				int random_pick = Math.RandomInt(0, item_count);
				ItemBase item = ItemBase.Cast(GetInventory().GetCargo().GetItem(random_pick));
				if (!item.IsExplosive())
				{
					item.AddHealth("","",damage);
					return true;
				}
			}
		}
		return false;
	}
	
	bool DamageItemAttachments(float damage)
	{
		int attachment_count = GetInventory().AttachmentCount();
		if (attachment_count > 0)
		{
			int random_pick = Math.RandomInt(0, attachment_count);
			ItemBase attachment = ItemBase.Cast(GetInventory().GetAttachmentFromIndex(random_pick));
			if (!attachment.IsExplosive())
			{
				attachment.AddHealth("","",damage);
				return true;
			}
		}
		return false;
	}
	
	override bool IsSplitable()
	{
		return m_CanThisBeSplit;
	}
	//----------------
	override bool CanBeSplit()
	{
		if (IsSplitable() && (GetQuantity() > 1))
			return GetInventory().CanRemoveEntity();
		
		return false;
	}

	protected bool ShouldSplitQuantity(float quantity)
	{
		// don't call 'CanBeSplit' here, too strict and will introduce a freeze-crash when dismantling fence with a fireplace nearby
		if (!IsSplitable())
			return false;

		// nothing to split?
		if (GetQuantity() <= 1)
			return false;

		// check if we should re-use the item instead of creating a new copy?
		// implicit cast to int, if 'IsSplitable' returns true, these values are assumed ints
		int delta = GetQuantity() - quantity;
		if (delta == 0)
			return false;

		// valid to split
		return true;
	}
	
	override void SplitIntoStackMaxClient(EntityAI destination_entity, int slot_id )
	{		
		if (GetGame().IsClient())
		{
			if (ScriptInputUserData.CanStoreInputUserData())
			{
				ScriptInputUserData ctx = new ScriptInputUserData;
				ctx.Write(INPUT_UDT_ITEM_MANIPULATION);
				ctx.Write(1);
				ItemBase i1 = this; // @NOTE: workaround for correct serialization
				ctx.Write(i1);
				ctx.Write(destination_entity);
				ctx.Write(true);
				ctx.Write(slot_id);
				ctx.Send();
			}
		}
		else if (!GetGame().IsMultiplayer())
		{
			SplitIntoStackMax(destination_entity, slot_id, PlayerBase.Cast(GetGame().GetPlayer()));
		}
	}

	void SplitIntoStackMax(EntityAI destination_entity, int slot_id, PlayerBase player)
	{		
		float split_quantity_new;
		ItemBase new_item;
		float quantity = GetQuantity();
		float stack_max = GetTargetQuantityMax(slot_id);
		InventoryLocation loc = new InventoryLocation;
		
		if (destination_entity && slot_id != -1 && InventorySlots.IsSlotIdValid(slot_id))
		{
			if (stack_max <= GetQuantity())
				split_quantity_new = stack_max;
			else
				split_quantity_new = GetQuantity();

			if (ShouldSplitQuantity(split_quantity_new))
			{
				new_item = ItemBase.Cast(destination_entity.GetInventory().CreateAttachmentEx(this.GetType(), slot_id));
				if (new_item)
				{
					new_item.SetResultOfSplit(true);
					MiscGameplayFunctions.TransferItemProperties(this, new_item);
					AddQuantity(-split_quantity_new, false, true);
					new_item.SetQuantity(split_quantity_new, false, true);
				}
			}
		}
		else if (destination_entity && slot_id == -1)
		{
			if (quantity > stack_max)
				split_quantity_new = stack_max;
			else
				split_quantity_new = quantity;
			
			if (ShouldSplitQuantity(split_quantity_new))
			{
				if (destination_entity.GetInventory().FindFreeLocationFor(this, FindInventoryLocationType.ANY, loc))
				{
					Object o = destination_entity.GetInventory().LocationCreateEntity(loc, GetType(), ECE_IN_INVENTORY, RF_DEFAULT);
					new_item = ItemBase.Cast(o);
				}

				if (new_item)
				{
					new_item.SetResultOfSplit(true);		
					MiscGameplayFunctions.TransferItemProperties(this, new_item);
					AddQuantity(-split_quantity_new, false, true);
					new_item.SetQuantity(split_quantity_new, false, true);
				}
			}
		}
		else
		{
			if (stack_max != 0)
			{
				if (stack_max < GetQuantity())
				{
					split_quantity_new = GetQuantity() - stack_max;
				}
				
				if (split_quantity_new == 0)
				{
					if (!GetGame().IsMultiplayer())
						player.PhysicalPredictiveDropItem(this);
					else
						player.ServerDropEntity(this);
					return;
				}
				
				if (ShouldSplitQuantity(split_quantity_new))
				{
					new_item = ItemBase.Cast(GetGame().CreateObjectEx(GetType(), player.GetWorldPosition(), ECE_PLACE_ON_SURFACE));
					
					if (new_item)
					{
						new_item.SetResultOfSplit(true);
						MiscGameplayFunctions.TransferItemProperties(this, new_item);
						SetQuantity(split_quantity_new, false, true);
						new_item.SetQuantity(stack_max, false, true);
						new_item.PlaceOnSurface();
					}
				}
			}
		}
	}
	
	override void SplitIntoStackMaxEx(EntityAI destination_entity, int slot_id)
	{		
		float split_quantity_new;
		ItemBase new_item;
		float quantity = GetQuantity();
		float stack_max = GetTargetQuantityMax(slot_id);
		InventoryLocation loc = new InventoryLocation;
		
		if (destination_entity && slot_id != -1 && InventorySlots.IsSlotIdValid(slot_id))
		{
			if (stack_max <= GetQuantity())
				split_quantity_new = stack_max;
			else
				split_quantity_new = GetQuantity();

			if (ShouldSplitQuantity(split_quantity_new))
			{
				new_item = ItemBase.Cast(destination_entity.GetInventory().CreateAttachmentEx(this.GetType(), slot_id));
				if (new_item)
				{
					new_item.SetResultOfSplit(true);
					MiscGameplayFunctions.TransferItemProperties(this, new_item);
					AddQuantity(-split_quantity_new, false, true);
					new_item.SetQuantity(split_quantity_new, false, true);
				}
			}
		}
		else if (destination_entity && slot_id == -1)
		{
			if (quantity > stack_max)
				split_quantity_new = stack_max;
			else
				split_quantity_new = quantity;
			
			if (ShouldSplitQuantity(split_quantity_new))
			{
				if (destination_entity.GetInventory().FindFreeLocationFor(this, FindInventoryLocationType.ANY, loc))
				{
					Object o = destination_entity.GetInventory().LocationCreateEntity(loc, GetType(), ECE_IN_INVENTORY, RF_DEFAULT);
					new_item = ItemBase.Cast(o);
				}

				if (new_item)
				{
					new_item.SetResultOfSplit(true);		
					MiscGameplayFunctions.TransferItemProperties(this, new_item);
					AddQuantity(-split_quantity_new, false, true);
					new_item.SetQuantity(split_quantity_new, false, true);
				}
			}
		}
		else
		{
			if (stack_max != 0)
			{
				if (stack_max < GetQuantity())
				{
					split_quantity_new = GetQuantity() - stack_max;
				}
				
				if (ShouldSplitQuantity(split_quantity_new))
				{
					new_item = ItemBase.Cast(GetGame().CreateObjectEx(GetType(),GetWorldPosition(), ECE_PLACE_ON_SURFACE));
					
					if (new_item)
					{
						new_item.SetResultOfSplit(true);
						MiscGameplayFunctions.TransferItemProperties(this, new_item);
						SetQuantity(split_quantity_new, false, true);
						new_item.SetQuantity(stack_max, false, true);
						new_item.PlaceOnSurface();
					}
				}
			}
		}
	}
	
	void SplitIntoStackMaxToInventoryLocationClient(notnull InventoryLocation dst)
	{
		if (GetGame().IsClient())
		{
			if (ScriptInputUserData.CanStoreInputUserData())
			{
				ScriptInputUserData ctx = new ScriptInputUserData;
				ctx.Write(INPUT_UDT_ITEM_MANIPULATION);
				ctx.Write(4);
				ItemBase thiz = this; // @NOTE: workaround for correct serialization
				ctx.Write(thiz);
				dst.WriteToContext(ctx);
				ctx.Send();
			}
		}
		else if (!GetGame().IsMultiplayer())
		{
			SplitIntoStackMaxToInventoryLocation(dst);
		}
	}
	
	void SplitIntoStackMaxCargoClient(EntityAI destination_entity, int idx, int row, int col)
	{
		if (GetGame().IsClient())
		{
			if (ScriptInputUserData.CanStoreInputUserData())
			{
				ScriptInputUserData ctx = new ScriptInputUserData;
				ctx.Write(INPUT_UDT_ITEM_MANIPULATION);
				ctx.Write(2);
				ItemBase dummy = this; // @NOTE: workaround for correct serialization
				ctx.Write(dummy);
				ctx.Write(destination_entity);
				ctx.Write(true);
				ctx.Write(idx);
				ctx.Write(row);
				ctx.Write(col);
				ctx.Send();
			}
		}
		else if (!GetGame().IsMultiplayer())
		{
			SplitIntoStackMaxCargo(destination_entity, idx, row, col);
		}
	}

	void SplitIntoStackMaxToInventoryLocation(notnull InventoryLocation dst)
	{
		SplitIntoStackMaxToInventoryLocationEx(dst);
	}
	
	ItemBase SplitIntoStackMaxToInventoryLocationEx(notnull InventoryLocation dst)
	{		
		float quantity = GetQuantity();
		float split_quantity_new;
		ItemBase new_item;
		if (dst.IsValid())
		{
			int slot_id = dst.GetSlot();
			float stack_max = GetTargetQuantityMax(slot_id);
			
			if (quantity > stack_max)
				split_quantity_new = stack_max;
			else
				split_quantity_new = quantity;

			if (ShouldSplitQuantity(split_quantity_new))
			{
				new_item = ItemBase.Cast(GameInventory.LocationCreateEntity(dst, this.GetType(), ECE_IN_INVENTORY, RF_DEFAULT));
				
				if (new_item)
				{
					new_item.SetResultOfSplit(true);
					MiscGameplayFunctions.TransferItemProperties(this,new_item);
					AddQuantity(-split_quantity_new, false, true);
					new_item.SetQuantity(split_quantity_new, false, true);
				}
				
				return new_item;
			}
		}
		
		return null;
	}
	
	void SplitIntoStackMaxCargo(EntityAI destination_entity, int idx, int row, int col)
	{		
		float quantity = GetQuantity();
		float split_quantity_new;
		ItemBase new_item;
		if (destination_entity)
		{
			float stackable = GetTargetQuantityMax();
			if (quantity > stackable)
				split_quantity_new = stackable;
			else
				split_quantity_new = quantity;
			
			if (ShouldSplitQuantity(split_quantity_new))
			{
				new_item = ItemBase.Cast(destination_entity.GetInventory().CreateEntityInCargoEx(this.GetType(), idx, row, col, false));
				if (new_item)
				{
					new_item.SetResultOfSplit(true);	
					MiscGameplayFunctions.TransferItemProperties(this,new_item);
					AddQuantity(-split_quantity_new, false, true);
					new_item.SetQuantity(split_quantity_new, false, true);
				}
			}
		}
	}
	
	void SplitIntoStackMaxHandsClient(PlayerBase player)
	{
		if (GetGame().IsClient())
		{
			if (ScriptInputUserData.CanStoreInputUserData())
			{
				ScriptInputUserData ctx = new ScriptInputUserData;
				ctx.Write(INPUT_UDT_ITEM_MANIPULATION);
				ctx.Write(3);
				ItemBase i1 = this; // @NOTE: workaround for correct serialization
				ctx.Write(i1);
				ItemBase destination_entity = this;
				ctx.Write(destination_entity);
				ctx.Write(true);
				ctx.Write(0);
				ctx.Send();
			}
		}
		else if (!GetGame().IsMultiplayer())
		{
			SplitIntoStackMaxHands(player);
		}
	}

	void SplitIntoStackMaxHands(PlayerBase player)
	{		
		float quantity = GetQuantity();
		float split_quantity_new;
		ref ItemBase new_item;
		if (player)
		{
			float stackable = GetTargetQuantityMax();
			if (quantity > stackable)
				split_quantity_new = stackable;
			else
				split_quantity_new = quantity;
			
			if (ShouldSplitQuantity(split_quantity_new))
			{
				EntityAI in_hands = player.GetHumanInventory().CreateInHands(this.GetType());
				new_item = ItemBase.Cast(in_hands);
				if (new_item)
				{		
					new_item.SetResultOfSplit(true);
					MiscGameplayFunctions.TransferItemProperties(this,new_item);
					AddQuantity(-split_quantity_new, false, true);
					new_item.SetQuantity(split_quantity_new, false, true);
				}
			}
		}
	}
	
	void SplitItemToInventoryLocation(notnull InventoryLocation dst)
	{		
		float quantity = GetQuantity();
		float split_quantity_new = Math.Floor(quantity * 0.5);
		
		if (!ShouldSplitQuantity(split_quantity_new))
			return;

		ItemBase new_item = ItemBase.Cast(GameInventory.LocationCreateEntity(dst, GetType(), ECE_IN_INVENTORY, RF_DEFAULT));

		if (new_item)
		{
			if (new_item.GetQuantityMax() < split_quantity_new)
			{
				split_quantity_new = new_item.GetQuantityMax();
			}
			
			new_item.SetResultOfSplit(true);
			MiscGameplayFunctions.TransferItemProperties(this, new_item);
			
			if (dst.IsValid() && dst.GetType() == InventoryLocationType.ATTACHMENT && split_quantity_new > 1)
			{
				AddQuantity(-1, false, true);
				new_item.SetQuantity(1, false, true);
			}
			else
			{
				AddQuantity(-split_quantity_new, false, true);
				new_item.SetQuantity(split_quantity_new, false, true);				
			}
		}	
	}
	
	void SplitItem(PlayerBase player)
	{
		float quantity = GetQuantity();
		float split_quantity_new = Math.Floor(quantity / 2);
		
		if (!ShouldSplitQuantity(split_quantity_new))
			return;
		
		InventoryLocation invloc = new InventoryLocation;
		bool found = player.GetInventory().FindFirstFreeLocationForNewEntity(GetType(), FindInventoryLocationType.ATTACHMENT, invloc);
		
		ItemBase new_item;
		new_item = player.CreateCopyOfItemInInventoryOrGroundEx(this, true);
		
		if (new_item)
		{
			if (new_item.GetQuantityMax() < split_quantity_new)
			{
				split_quantity_new = new_item.GetQuantityMax();
			}
			if (found && invloc.IsValid() && invloc.GetType() == InventoryLocationType.ATTACHMENT && split_quantity_new > 1)
			{
				AddQuantity(-1, false, true);
				new_item.SetQuantity(1, false, true);
			}
			else if (split_quantity_new > 1)
			{
				AddQuantity(-split_quantity_new, false, true);
				new_item.SetQuantity(split_quantity_new, false, true);
			}
		}
	}
	
	//! Called on server side when this item's quantity is changed. Call super.OnQuantityChanged(); first when overriding this event.
	void OnQuantityChanged(float delta)
	{
		SetWeightDirty();
		ItemBase parent = ItemBase.Cast(GetHierarchyParent());
		
		if (parent)
			parent.OnAttachmentQuantityChangedEx(this, delta);
		
		if (IsLiquidContainer())
		{
			if (GetQuantityNormalized() <= 0.0)
			{
				SetLiquidType(LIQUID_NONE);
			}
			else if (GetLiquidType() == LIQUID_NONE)
			{
				ErrorEx("Undefined liquid type quantity changed, please define liquid type first! Using init value.",ErrorExSeverity.INFO);
				SetLiquidType(GetLiquidTypeInit());
			}
		}
			
	}
	
	//! Called on server side when some attachment's quantity is changed. Call super.OnAttachmentQuantityChanged(item); first when overriding this event.
	void OnAttachmentQuantityChanged(ItemBase item)
	{
		// insert code here
	}
	
	//! Called on server side when some attachment's quantity is changed. Call super.OnAttachmentQuantityChanged(item); first when overriding this event.
	void OnAttachmentQuantityChangedEx(ItemBase item , float delta)
	{
		OnAttachmentQuantityChanged(item);
	}

	override void EEHealthLevelChanged(int oldLevel, int newLevel, string zone)
	{
		super.EEHealthLevelChanged(oldLevel,newLevel,zone);
		
		if (GetGame().IsServer())
		{
			if (newLevel == GameConstants.STATE_RUINED)
			{
				//! drops content of container when ruined in fireplace
				EntityAI parent = GetHierarchyParent();
				if (parent && parent.IsFireplace())
				{
					CargoBase cargo = GetInventory().GetCargo();
					if (cargo)
					{
						for (int i = 0; i < cargo.GetItemCount(); ++i)
						{
							parent.GetInventory().TakeEntityToInventory(InventoryMode.SERVER, FindInventoryLocationType.CARGO, cargo.GetItem(i));
						}
					}
				}
			}
			
			if (IsResultOfSplit())
			{
				// reset the splitting result flag, return to normal item behavior
				SetResultOfSplit(false);
				return;
			}

			if (m_Cleanness != 0 && oldLevel < newLevel && newLevel != 0)
			{
				SetCleanness(0);//unclean the item upon damage dealt
			}
		}
	}
		
	// just the split? TODO: verify
	override void OnRightClick()
	{
		super.OnRightClick();
		
		if (CanBeSplit() && !GetDayZGame().IsLeftCtrlDown() && !GetGame().GetPlayer().GetInventory().HasInventoryReservation(this,null))
		{
			if (GetGame().IsClient())
			{
				if (ScriptInputUserData.CanStoreInputUserData())
				{
					EntityAI root = GetHierarchyRoot();
					Man playerOwner = GetHierarchyRootPlayer();
					InventoryLocation dst = new InventoryLocation;
					
					// If we have no hierarchy root player and the root is the same as this item the source item is in the vicinity so we want to create the new split item there also					
					if (!playerOwner && root && root == this)
					{
						SetInventoryLocationToVicinityOrCurrent(root, dst);
					}
					else
					{
						// Check if we can place the new split item in the same parent where the source item is placed in or otherwise drop it in vicinity
						GetInventory().GetCurrentInventoryLocation(dst);
						if (!dst.GetParent() || dst.GetParent() && !dst.GetParent().GetInventory().FindFreeLocationFor(this, FindInventoryLocationType.CARGO, dst))
						{
							PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
							if (!player.GetInventory().FindFreeLocationFor(this, FindInventoryLocationType.CARGO, dst) || !playerOwner)
							{
								SetInventoryLocationToVicinityOrCurrent(root, dst);
							}
							else
							{
								dst.SetCargo(dst.GetParent(), this, dst.GetIdx(), dst.GetRow(), dst.GetCol(), dst.GetFlip());
								/*	hacky solution to check reservation of "this" item instead of null since the gamecode is checking null against null and returning reservation=true incorrectly
									this shouldnt cause issues within this scope*/
								if (GetGame().GetPlayer().GetInventory().HasInventoryReservation(this, dst))
								{
									SetInventoryLocationToVicinityOrCurrent(root, dst);
								}
								else
								{
									GetGame().GetPlayer().GetInventory().AddInventoryReservationEx(null, dst, GameInventory.c_InventoryReservationTimeoutShortMS);
								}
							}
						}
					}
					
					ScriptInputUserData ctx = new ScriptInputUserData;
					ctx.Write(INPUT_UDT_ITEM_MANIPULATION);
					ctx.Write(4);
					ItemBase thiz = this; // @NOTE: workaround for correct serialization
					ctx.Write(thiz);
					dst.WriteToContext(ctx);
					ctx.Write(true); // dummy
					ctx.Send();
				}
			}
			else if (!GetGame().IsMultiplayer())
			{
				SplitItem(PlayerBase.Cast(GetGame().GetPlayer()));
			}
		}
	}
		
	protected void SetInventoryLocationToVicinityOrCurrent(EntityAI root, inout InventoryLocation dst)
	{
		if (root)
		{
			vector m4[4];
			root.GetTransform(m4);
			dst.SetGround(this, m4);
		}
		else
		{
			GetInventory().GetCurrentInventoryLocation(dst);
		}
	}
	
	override bool CanBeCombined(EntityAI other_item, bool reservation_check = true, bool stack_max_limit = false)
	{
		//TODO: delete check zero quantity check after fix double posts hands fsm events
		if (!other_item || GetType() != other_item.GetType() || (IsFullQuantity() && other_item.GetQuantity() > 0) || other_item == this)
			return false;

		if (GetHealthLevel() == GameConstants.STATE_RUINED || other_item.GetHealthLevel() == GameConstants.STATE_RUINED)	
			return false;
		
		//can_this_be_combined = ConfigGetBool("canBeSplit");
		if (!can_this_be_combined)
			return false;

		
		Magazine mag = Magazine.Cast(this);
		if (mag)
		{
			if (mag.GetAmmoCount() >= mag.GetAmmoMax())
				return false;
			
			if (stack_max_limit)
			{
				Magazine other_mag = Magazine.Cast(other_item);
				if (other_item)
				{
					if (mag.GetAmmoCount() + other_mag.GetAmmoCount() > mag.GetAmmoMax())
						return false;
				}
			
			}
		}
		else
		{
			//TODO: delete check zero quantity check after fix double posts hands fsm events
			if (GetQuantity() >= GetQuantityMax() && other_item.GetQuantity() > 0 )	
				return false;
			
			if (stack_max_limit && (GetQuantity() + other_item.GetQuantity()  > GetQuantityMax()))
				return false;
		}

		PlayerBase player = null;
		if (CastTo(player, GetHierarchyRootPlayer())) //false when attached to player's attachment slot
		{
			if (player.GetInventory().HasAttachment(this))
				return false;
			
			if (player.IsItemsToDelete())
				return false;
		}

		if (reservation_check && (GetInventory().HasInventoryReservation(this, null) || other_item.GetInventory().HasInventoryReservation(other_item, null)))
			return false;
		
		int slotID;
		string slotName;
		if (GetInventory().GetCurrentAttachmentSlotInfo(slotID,slotName) && GetHierarchyParent().GetInventory().GetSlotLock(slotID))
			return false;

		return true;
	}
	
	bool IsCombineAll(ItemBase other_item, bool use_stack_max = false)
	{
		return ComputeQuantityUsed(other_item, use_stack_max) == other_item.GetQuantity();
	}
	
	bool IsResultOfSplit()
	{
		return m_IsResultOfSplit;
	}
	
	void SetResultOfSplit(bool value)
	{
		m_IsResultOfSplit = value;
	}
	
	int ComputeQuantityUsed(ItemBase other_item, bool use_stack_max = true)
	{
		return ComputeQuantityUsedEx(other_item, use_stack_max);
	}
	
	float ComputeQuantityUsedEx(ItemBase other_item, bool use_stack_max = true)
	{
		float other_item_quantity = other_item.GetQuantity();
		float this_free_space;
			
		float stack_max = GetQuantityMax();	
		
		this_free_space = stack_max - GetQuantity();
			
		if (other_item_quantity > this_free_space)
		{
			return this_free_space;
		}
		else
		{
			return other_item_quantity;
		}
	}
	
	override void CombineItemsEx(EntityAI entity2, bool use_stack_max = true)
	{
		CombineItems(ItemBase.Cast(entity2),use_stack_max);
	}
	
	void CombineItems(ItemBase other_item, bool use_stack_max = true)
	{
		if (!CanBeCombined(other_item, false))
			return;
		
		if (!IsMagazine() && other_item)
		{
			float quantity_used = ComputeQuantityUsedEx(other_item,use_stack_max);
			if (quantity_used != 0)
			{
				float hp1 = GetHealth01("","");
				float hp2 = other_item.GetHealth01("","");
				float hpResult = ((hp1*GetQuantity()) + (hp2*quantity_used));
				hpResult = hpResult / (GetQuantity() + quantity_used);

				hpResult *= GetMaxHealth();
				Math.Round(hpResult);
				SetHealth("", "Health", hpResult);

				AddQuantity(quantity_used);
				other_item.AddQuantity(-quantity_used);
			}
		}
		OnCombine(other_item);
	}

	void OnCombine(ItemBase other_item)
	{
		#ifdef SERVER
		if (!GetHierarchyRootPlayer() && GetHierarchyParent())
			GetHierarchyParent().IncreaseLifetimeUp();
		#endif
	};

	void GetRecipesActions(Man player, out TSelectableActionInfoArray outputList)
	{
		PlayerBase p = PlayerBase.Cast(player);
			
		array<int> recipesIds = p.m_Recipes;
		PluginRecipesManager moduleRecipesManager = PluginRecipesManager.Cast(GetPlugin(PluginRecipesManager));
		if (moduleRecipesManager)
		{
			EntityAI itemInHands = player.GetHumanInventory().GetEntityInHands();
			moduleRecipesManager.GetValidRecipes(ItemBase.Cast(this), ItemBase.Cast(itemInHands), recipesIds, p);
		}

		for (int i = 0;i < recipesIds.Count(); i++)
		{
			int key = recipesIds.Get(i);
			string recipeName = moduleRecipesManager.GetRecipeName(key);
			outputList.Insert(new TSelectableActionInfo(SAT_CRAFTING, key, recipeName));
		}
	}
	
	// -------------------------------------------------------------------------	
	override void GetDebugActions(out TSelectableActionInfoArrayEx outputList)
	{
		super.GetDebugActions(outputList);
			
		//quantity
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.ADD_QUANTITY, "Quantity +20%", FadeColors.LIGHT_GREY));
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.REMOVE_QUANTITY, "Quantity -20%", FadeColors.LIGHT_GREY));
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.SET_QUANTITY_0, "Set Quantity 0", FadeColors.LIGHT_GREY));
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.SET_MAX_QUANTITY, "Set Quantity Max", FadeColors.LIGHT_GREY));
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.SEPARATOR, "___________________________", FadeColors.RED));
		
		//health
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.ADD_HEALTH, "Health +20%", FadeColors.LIGHT_GREY));
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.REMOVE_HEALTH, "Health -20%", FadeColors.LIGHT_GREY));
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.DESTROY_HEALTH, "Health 0", FadeColors.LIGHT_GREY));
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.SEPARATOR, "___________________________", FadeColors.RED));
		//temperature
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.ADD_TEMPERATURE, "Temperature +20", FadeColors.LIGHT_GREY));
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.REMOVE_TEMPERATURE, "Temperature -20", FadeColors.LIGHT_GREY));
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.FLIP_FROZEN, "Toggle Frozen", FadeColors.LIGHT_GREY));
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.SEPARATOR, "___________________________", FadeColors.RED));
		
		//wet
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.ADD_WETNESS, "Wetness +20", FadeColors.LIGHT_GREY));
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.REMOVE_WETNESS, "Wetness -20", FadeColors.LIGHT_GREY));
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.SEPARATOR, "___________________________", FadeColors.RED));

		//liquidtype
		if (IsLiquidContainer())
		{
			outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.LIQUIDTYPE_UP, "LiquidType Next", FadeColors.LIGHT_GREY));
			outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.LIQUIDTYPE_DOWN, "LiquidType Previous", FadeColors.LIGHT_GREY));
			outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.SEPARATOR, "___________________________", FadeColors.RED));
		}
		
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.MAKE_SPECIAL, "Make Special", FadeColors.LIGHT_GREY));
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.SEPARATOR, "___________________________", FadeColors.RED));

		// watch
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.WATCH_ITEM, "Watch (CTRL-Z)", FadeColors.LIGHT_GREY));
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.WATCH_PLAYER, "Watch Player", FadeColors.LIGHT_GREY));
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.SEPARATOR, "___________________________", FadeColors.RED));
		
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.DELETE, "Delete", FadeColors.RED));

		InventoryLocation loc = new InventoryLocation();
		GetInventory().GetCurrentInventoryLocation(loc);
		if (!loc || loc.GetType() == InventoryLocationType.GROUND)
		{
			if (Gizmo_IsSupported())
				outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.GIZMO_OBJECT, "Gizmo Object", FadeColors.LIGHT_GREY));
			outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.GIZMO_PHYSICS, "Gizmo Physics (SP Only)", FadeColors.LIGHT_GREY)); // intentionally allowed for testing physics desync
		}

		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.SEPARATOR, "___________________________", FadeColors.RED));
	}
	
	// -------------------------------------------------------------------------	
	// -------------------------------------------------------------------------		
	// -------------------------------------------------------------------------	
	override bool OnAction(int action_id, Man player, ParamsReadContext ctx)
	{
		super.OnAction(action_id, player, ctx);

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

		if (action_id >= EActions.RECIPES_RANGE_START && action_id < EActions.RECIPES_RANGE_END)
		{
			PluginRecipesManager plugin_recipes_manager = PluginRecipesManager.Cast(GetPlugin(PluginRecipesManager));
			int idWithoutOffset = action_id - EActions.RECIPES_RANGE_START;
			PlayerBase p = PlayerBase.Cast(player);
			if (EActions.RECIPES_RANGE_START  < 1000)
			{
				float anim_length = plugin_recipes_manager.GetRecipeLengthInSecs(idWithoutOffset);
				float specialty_weight = plugin_recipes_manager.GetRecipeSpecialty(idWithoutOffset);
			}
		}
		#ifndef SERVER
		else if (action_id == EActions.WATCH_PLAYER)
		{
			PluginDeveloper.SetDeveloperItemClientEx(player);
		}
		#endif
		if (GetGame().IsServer())
		{
			if (action_id >= EActions.DEBUG_ITEM_WATCH_BUTTON_RANGE_START && action_id < EActions.DEBUG_ITEM_WATCH_BUTTON_RANGE_END)
			{
				int id = action_id - EActions.DEBUG_ITEM_WATCH_BUTTON_RANGE_START;
				OnDebugButtonPressServer(id + 1);
			}
			
			else if (action_id >= EActions.DEBUG_AGENTS_RANGE_INJECT_START && action_id < EActions.DEBUG_AGENTS_RANGE_INJECT_END)
			{
				int agent_id = action_id - EActions.DEBUG_AGENTS_RANGE_INJECT_START;
				InsertAgent(agent_id,100);
			}
	
			else if (action_id >= EActions.DEBUG_AGENTS_RANGE_REMOVE_START && action_id < EActions.DEBUG_AGENTS_RANGE_REMOVE_END)
			{
				int agent_id2 = action_id - EActions.DEBUG_AGENTS_RANGE_REMOVE_START;
				RemoveAgent(agent_id2);
			}
			
			else if (action_id == EActions.ADD_QUANTITY)
			{
				if (IsMagazine())
				{
					Magazine mag = Magazine.Cast(this);
					mag.ServerSetAmmoCount(mag.GetAmmoCount() + mag.GetAmmoMax() * 0.2);
				}
				else
				{
					AddQuantity(GetQuantityMax() * 0.2);
				}
				
				if (m_EM)
				{
					m_EM.AddEnergy(m_EM.GetEnergyMax() * 0.2);
				}
				//PrintVariables();
			}
						
			else if (action_id == EActions.REMOVE_QUANTITY) //Quantity -20%
			{
				if (IsMagazine())
				{
					Magazine mag2 = Magazine.Cast(this);
					mag2.ServerSetAmmoCount(mag2.GetAmmoCount() - mag2.GetAmmoMax() * 0.2);
				}
				else
				{
					AddQuantity(- GetQuantityMax() * 0.2);
				}
				if (m_EM)
				{
					m_EM.AddEnergy(- m_EM.GetEnergyMax() * 0.2);
				}
				//PrintVariables();
			}
			
			else if (action_id == EActions.SET_QUANTITY_0) //SetMaxQuantity
			{
				SetQuantity(0);
				
				if (m_EM)
				{
					m_EM.SetEnergy(0);
				}
			}
			
			else if (action_id == EActions.SET_MAX_QUANTITY) //SetMaxQuantity
			{
				SetQuantityMax();
				
				if (m_EM)
				{
					m_EM.SetEnergy(m_EM.GetEnergyMax());
				}
			}
	
			else if (action_id == EActions.ADD_HEALTH) 
			{
				AddHealth("","",GetMaxHealth("","Health")/5);
			}
			else if (action_id == EActions.REMOVE_HEALTH) 
			{
				AddHealth("","",-GetMaxHealth("","Health")/5);
			}
			else if (action_id == EActions.DESTROY_HEALTH) 
			{
				SetHealth01("","",0);
			}
			else if (action_id == EActions.WATCH_ITEM)
			{
				PluginItemDiagnostic mid = PluginItemDiagnostic.Cast(GetPlugin(PluginItemDiagnostic));
				mid.RegisterDebugItem(ItemBase.Cast(this), PlayerBase.Cast(player));
				#ifdef DEVELOPER
				SetDebugDeveloper_item(this);
				#endif
			}
			
			else if (action_id == EActions.ADD_TEMPERATURE)
			{
				AddTemperature(20);
				//PrintVariables();
			}
			
			else if (action_id == EActions.REMOVE_TEMPERATURE)
			{
				AddTemperature(-20);
				//PrintVariables();
			}
			
			else if (action_id == EActions.FLIP_FROZEN)
			{
				SetFrozen(!GetIsFrozen());
				//PrintVariables();
			}
			
			else if (action_id == EActions.ADD_WETNESS)
			{
				AddWet(GetWetMax()/5);
				//PrintVariables();
			}
			
			else if (action_id == EActions.REMOVE_WETNESS)
			{
				AddWet(-GetWetMax()/5);
				//PrintVariables();
			}
	
			else if (action_id == EActions.LIQUIDTYPE_UP)
			{
				int curr_type = GetLiquidType();
				SetLiquidType(curr_type * 2);
				//AddWet(1);
				//PrintVariables();
			}
			
			else if (action_id == EActions.LIQUIDTYPE_DOWN)
			{
				int curr_type2 = GetLiquidType();
				SetLiquidType(curr_type2 / 2);
			}

			else if (action_id == EActions.MAKE_SPECIAL)
			{
				auto debugParams = DebugSpawnParams.WithPlayer(player);
				OnDebugSpawnEx(debugParams);
			}
			
		}

		
		return false;
	}

	// -------------------------------------------------------------------------
	
	
	//! DEPRECATED use OnActivatedByItem below
	//! Called when this item is activated from a trip wire that was stepped on.
	void OnActivatedByTripWire();
	
	//! Called when this item is activated by other
	void OnActivatedByItem(notnull ItemBase item);

	//----------------------------------------------------------------
	//returns true if item is able to explode when put in fire
	bool CanExplodeInFire()
	{
		return false;
	}
	
	//----------------------------------------------------------------
	bool CanEat()
	{
		return true;
	}
	
	//----------------------------------------------------------------
	override bool IsIgnoredByConstruction()
	{
		return true;
	}
	
	//----------------------------------------------------------------
	//has FoodStages in config?
	bool HasFoodStage()
	{
		string config_path = string.Format("CfgVehicles %1 Food FoodStages", GetType());
		return GetGame().ConfigIsExisting(config_path);
	}
	
	//! overridden on Edible_Base; so we don't have to parse configs all the time
	FoodStage GetFoodStage()
	{
		return null;
	}
	
	bool CanBeCooked()
	{
		return false;
	}
	
	bool CanBeCookedOnStick()
	{
		return false;
	}		
	
	//! cooking-related effect methods
	void RefreshAudioVisualsOnClient( CookingMethodType cooking_method, bool is_done, bool is_empty, bool is_burned );
	void RemoveAudioVisualsOnClient();
	
	//----------------------------------------------------------------
	bool CanRepair(ItemBase item_repair_kit)
	{
		PluginRepairing module_repairing = PluginRepairing.Cast(GetPlugin(PluginRepairing));
		return module_repairing.CanRepair(this, item_repair_kit);
	}

	//----------------------------------------------------------------
	bool Repair(PlayerBase player, ItemBase item_repair_kit, float specialty_weight)
	{
		PluginRepairing module_repairing = PluginRepairing.Cast(GetPlugin(PluginRepairing));
		return module_repairing.Repair(player, this, item_repair_kit, specialty_weight);
	}

	//----------------------------------------------------------------
	int GetItemSize()
	{
		/*
		vector v_size = this.ConfigGetVector("itemSize");
		int v_size_x = v_size[0];
		int v_size_y = v_size[1];
		int size = v_size_x * v_size_y;
		return size;
		*/
		
		return 1;
	}
	
	//----------------------------------------------------------------
	//Override for allowing seemingly unallowed moves when two clients send a conflicting message simultaneously
	bool CanBeMovedOverride()
	{
		return m_CanBeMovedOverride;
	}
	
	//----------------------------------------------------------------
	//Override for allowing seemingly unallowed moves when two clients send a conflicting message simultaneously
	void SetCanBeMovedOverride(bool setting)
	{
		m_CanBeMovedOverride = setting;
	}
	
	//----------------------------------------------------------------
	/**
	\brief Send message to owner player in grey color
		\return \p void
		@code
			item_stone.MessageToOwnerStatus("Some Status Message");
		@endcode
	*/
	void MessageToOwnerStatus(string text)
	{
		PlayerBase player = PlayerBase.Cast(this.GetHierarchyRootPlayer());
		
		if (player)
		{
			player.MessageStatus(text);
		}
	}

	//----------------------------------------------------------------
	/**
	\brief Send message to owner player in yellow color
		\return \p void
		@code
			item_stone.MessageToOwnerAction("Some Action Message");
		@endcode
	*/
	void MessageToOwnerAction(string text)
	{
		PlayerBase player = PlayerBase.Cast(this.GetHierarchyRootPlayer());
		
		if (player)
		{
			player.MessageAction(text);
		}
	}

	//----------------------------------------------------------------
	/**
	\brief Send message to owner player in green color
		\return \p void
		@code
			item_stone.MessageToOwnerFriendly("Some Friendly Message");
		@endcode
	*/
	void MessageToOwnerFriendly(string text)
	{
		PlayerBase player = PlayerBase.Cast(this.GetHierarchyRootPlayer());
		
		if (player)
		{
			player.MessageFriendly(text);
		}
	}

	//----------------------------------------------------------------
	/**
	\brief Send message to owner player in red color
		\return \p void
		@code
			item_stone.MessageToOwnerImportant("Some Important Message");
		@endcode
	*/
	void MessageToOwnerImportant(string text)
	{
		PlayerBase player = PlayerBase.Cast(this.GetHierarchyRootPlayer());
		
		if (player)
		{
			player.MessageImportant(text);
		}
	}

	override bool IsItemBase()
	{
		return true;
	}

	// Checks if item is of questioned kind
	override bool KindOf(string tag)
	{
		bool found = false;
		string item_name = this.GetType();
		ref TStringArray item_tag_array = new TStringArray;
		GetGame().ConfigGetTextArray("cfgVehicles " + item_name + " itemInfo", item_tag_array);	
		
		int array_size = item_tag_array.Count();
		for (int i = 0; i < array_size; i++)
		{
			if (item_tag_array.Get(i) == tag)
			{
				found = true;
				break;
			}
		}
		return found;
	}

	
	override void OnRPC(PlayerIdentity sender, int rpc_type,ParamsReadContext ctx) 
	{
		//Debug.Log("OnRPC called");
		super.OnRPC(sender, rpc_type,ctx);
			
		//Play soundset for attachment locking (ActionLockAttachment.c)
		switch (rpc_type)
		{
			#ifndef SERVER
			case ERPCs.RPC_SOUND_LOCK_ATTACH:
				Param2<bool, string> p = new Param2<bool, string>(false, "");
					
				if (!ctx.Read(p))
					return;
			
				bool play = p.param1;
				string soundSet = p.param2;
				
				if (play)
				{
					if (m_LockingSound)
					{
						if (!m_LockingSound.IsSoundPlaying())
						{
							m_LockingSound = SEffectManager.PlaySound(soundSet, GetPosition(), 0, 0, true);
						}
					}
					else
					{
						m_LockingSound = SEffectManager.PlaySound(soundSet, GetPosition(), 0, 0, true);
					}
				}
				else
				{
					SEffectManager.DestroyEffect(m_LockingSound);
				}
			
			break;
			#endif
		
		}
		
		if (GetWrittenNoteData())
		{
			GetWrittenNoteData().OnRPC(sender, rpc_type,ctx);
		}
	}

	//-----------------------------
	// VARIABLE MANIPULATION SYSTEM
	//-----------------------------
	int NameToID(string name)
	{
		PluginVariables plugin = PluginVariables.Cast(GetPlugin(PluginVariables));
		return plugin.GetID(name);
	}

	string IDToName(int id)
	{
		PluginVariables plugin = PluginVariables.Cast(GetPlugin(PluginVariables));
		return plugin.GetName(id);
	}

	//! DEPRECATED (most likely)
	void OnSyncVariables(ParamsReadContext ctx)//with ID optimization
	{
		//Debug.Log("OnSyncVariables called for item:  "+ ToString(this.GetType()),"varSync");
		//read the flags
		int varFlags;
		if (!ctx.Read(varFlags))
			return;
		
		if (varFlags & ItemVariableFlags.FLOAT)
		{
			ReadVarsFromCTX(ctx);
		}
	}
	
	override void SerializeNumericalVars(array<float> floats_out)
	{
		//some variables handled on EntityAI level already!
		super.SerializeNumericalVars(floats_out);
		
		// the order of serialization must be the same as the order of de-serialization
		//--------------------------------------------
		if (IsVariableSet(VARIABLE_QUANTITY))
		{
			floats_out.Insert(m_VarQuantity);
		}
		//--------------------------------------------
		if (IsVariableSet(VARIABLE_WET))
		{
			floats_out.Insert(m_VarWet);
		}
		//--------------------------------------------
		if (IsVariableSet(VARIABLE_LIQUIDTYPE))
		{
			floats_out.Insert(m_VarLiquidType);
		}
		//--------------------------------------------
		if (IsVariableSet(VARIABLE_COLOR))
		{
			floats_out.Insert(m_ColorComponentR);
			floats_out.Insert(m_ColorComponentG);
			floats_out.Insert(m_ColorComponentB);
			floats_out.Insert(m_ColorComponentA);
		}
		//--------------------------------------------
		if (IsVariableSet(VARIABLE_CLEANNESS))
		{
			floats_out.Insert(m_Cleanness);
		}
	}
	
	override void DeSerializeNumericalVars(array<float> floats)
	{
		//some variables handled on EntityAI level already!
		super.DeSerializeNumericalVars(floats);
		
		// the order of serialization must be the same as the order of de-serialization
		int index = 0;
		int mask = Math.Round(floats.Get(index));
		
		index++;
		//--------------------------------------------
		if (mask & VARIABLE_QUANTITY)
		{
			if (m_IsStoreLoad)
			{
				SetStoreLoadedQuantity(floats.Get(index));
			}
			else
			{
				float quantity = floats.Get(index);
				SetQuantity(quantity, true, false, false, false);
			}
			index++;
		}
		//--------------------------------------------
		if (mask & VARIABLE_WET)
		{
			float wet = floats.Get(index);
			SetWet(wet);
			index++;
		}
		//--------------------------------------------
		if (mask & VARIABLE_LIQUIDTYPE)
		{
			int liquidtype = Math.Round(floats.Get(index));
			SetLiquidType(liquidtype);
			index++;
		}
		//--------------------------------------------
		if (mask & VARIABLE_COLOR)
		{
			m_ColorComponentR = Math.Round(floats.Get(index));
			index++;
			m_ColorComponentG = Math.Round(floats.Get(index));
			index++;
			m_ColorComponentB = Math.Round(floats.Get(index));
			index++;
			m_ColorComponentA = Math.Round(floats.Get(index));
			index++;
		}
		//--------------------------------------------
		if (mask & VARIABLE_CLEANNESS)
		{
			int cleanness = Math.Round(floats.Get(index));
			SetCleanness(cleanness);
			index++;
		}
	}
	
	override void WriteVarsToCTX(ParamsWriteContext ctx)
	{
		super.WriteVarsToCTX(ctx);

		//--------------------------------------------
		if (IsVariableSet(VARIABLE_QUANTITY))
		{
			ctx.Write(GetQuantity());
		}
		//--------------------------------------------
		if (IsVariableSet(VARIABLE_WET))
		{
			ctx.Write(GetWet());
		}
		//--------------------------------------------
		if (IsVariableSet(VARIABLE_LIQUIDTYPE))
		{
			ctx.Write(GetLiquidType());
		}
		//--------------------------------------------
		if (IsVariableSet(VARIABLE_COLOR))
		{
			int r,g,b,a;
			GetColor(r,g,b,a);
			ctx.Write(r);
			ctx.Write(g);
			ctx.Write(b);
			ctx.Write(a);
		}
		//--------------------------------------------
		if (IsVariableSet(VARIABLE_CLEANNESS))
		{
			ctx.Write(GetCleanness());
		}
	}
	
	override bool ReadVarsFromCTX(ParamsReadContext ctx, int version = -1)//with ID optimization
	{
		if (!super.ReadVarsFromCTX(ctx,version))
			return false;
		
		int intValue;
		float value;
		
		if (version < 140)
		{
			if (!ctx.Read(intValue))
				return false;
			
			m_VariablesMask = intValue;
		}
		
		if (m_VariablesMask & VARIABLE_QUANTITY)
		{
			if (!ctx.Read(value))
				return false;
			
			if (IsStoreLoad())
			{
				SetStoreLoadedQuantity(value);
			}
			else
			{
				SetQuantity(value, true, false, false, false);
			}
		}
		//--------------------------------------------
		if (version < 140)
		{
			if (m_VariablesMask & VARIABLE_TEMPERATURE)
			{
				if (!ctx.Read(value))
					return false;
				SetTemperatureDirect(value);
			}
		}
		//--------------------------------------------
		if (m_VariablesMask & VARIABLE_WET)
		{
			if (!ctx.Read(value))
				return false;
			SetWet(value);
		}
		//--------------------------------------------
		if (m_VariablesMask & VARIABLE_LIQUIDTYPE)
		{
			if (!ctx.Read(intValue))
				return false;
			SetLiquidType(intValue);
		}
		//--------------------------------------------
		if (m_VariablesMask & VARIABLE_COLOR)
		{
			int r,g,b,a;
			if (!ctx.Read(r))
				return false;
			if (!ctx.Read(g))
				return false;
			if (!ctx.Read(b))
				return false;
			if (!ctx.Read(a))
				return false;
			
			SetColor(r,g,b,a);
		}
		//--------------------------------------------
		if (m_VariablesMask & VARIABLE_CLEANNESS)
		{
			if (!ctx.Read(intValue))
				return false;
			SetCleanness(intValue);
		}
		//--------------------------------------------
		if (version >= 138 && version < 140)
		{
			if (m_VariablesMask & VARIABLE_TEMPERATURE)
			{
				if (!ctx.Read(intValue))
					return false;
				SetFrozen(intValue);
			}
		}
		
		return true;
	}

	//----------------------------------------------------------------
	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		m_IsStoreLoad = true;
		if (GetDamageSystemVersionChange() != -1 && version < GetDamageSystemVersionChange())
		{
			m_FixDamageSystemInit = true;
		}
		
		if (!super.OnStoreLoad(ctx, version))
		{
			m_IsStoreLoad = false;
			return false;
		}
		
		if (version >= 114)
		{
			bool hasQuickBarIndexSaved;
			
			if (!ctx.Read(hasQuickBarIndexSaved))
			{
				m_IsStoreLoad = false;
				return false;
			}
			
			if (hasQuickBarIndexSaved)
			{
				int itmQBIndex;
				
				//Load quickbar item bind
				if (!ctx.Read(itmQBIndex))
				{
					m_IsStoreLoad = false;
					return false;
				}
				
				PlayerBase parentPlayer = PlayerBase.Cast(GetHierarchyRootPlayer());				
				if (itmQBIndex != -1 && parentPlayer)
					parentPlayer.SetLoadedQuickBarItemBind(this, itmQBIndex);
			}
		}
		else
		{
			// Backup of how it used to be
			PlayerBase player;
			int itemQBIndex;
			if (version == int.MAX)
			{
				if (!ctx.Read(itemQBIndex))
				{
					m_IsStoreLoad = false;
					return false;
				}
			}
			else if (Class.CastTo(player, GetHierarchyRootPlayer()))
			{
				//Load quickbar item bind
				if (!ctx.Read(itemQBIndex))
				{
					m_IsStoreLoad = false;
					return false;
				}
				if (itemQBIndex != -1 && player)
					player.SetLoadedQuickBarItemBind(this,itemQBIndex);
			}
		}
		
		if (version < 140)
		{
			// variable management system
			if (!LoadVariables(ctx, version))
			{
				m_IsStoreLoad = false;
				return false;
			}
		}
		
		//agent trasmission system
		if (!LoadAgents(ctx, version))
		{
			m_IsStoreLoad = false;
			return false;
		}
		if (version >= 132)
		{
			RemotelyActivatedItemBehaviour raib = GetRemotelyActivatedItemBehaviour();
			if (raib)
			{
				if (!raib.OnStoreLoad(ctx,version))
				{
					m_IsStoreLoad = false;
					return false;
				}
			}
		}

		m_IsStoreLoad = false;
		return true;
	}

	//----------------------------------------------------------------

	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);
		
		PlayerBase player;
		if (PlayerBase.CastTo(player,GetHierarchyRootPlayer()))
		{
			ctx.Write(true); // Keep track of if we should actually read this in or not
			//Save quickbar item bind
			int itemQBIndex = -1;
			itemQBIndex = player.FindQuickBarEntityIndex(this);
			ctx.Write(itemQBIndex);	
		}
		else
		{
			ctx.Write(false); // Keep track of if we should actually read this in or not
		}
		
		SaveAgents(ctx);//agent trasmission system
		
		RemotelyActivatedItemBehaviour raib = GetRemotelyActivatedItemBehaviour();
		if (raib)
		{
			raib.OnStoreSave(ctx);
		}
	}
	//----------------------------------------------------------------

	override void AfterStoreLoad()
	{	
		super.AfterStoreLoad();
		
		if (m_FixDamageSystemInit)
		{
			PerformDamageSystemReinit();
		}

		if (GetStoreLoadedQuantity() != float.LOWEST)
		{
			SetQuantity(GetStoreLoadedQuantity());
			SetStoreLoadedQuantity(float.LOWEST);//IMPORTANT to do this !! we use 'm_StoreLoadedQuantity' inside SetQuantity to distinguish between initial quantity setting and the consequent(normal gameplay) calls
		}
	}
	
	override void EEOnAfterLoad()
	{
		super.EEOnAfterLoad();
		
		if (m_FixDamageSystemInit)
		{
			m_FixDamageSystemInit = false;
		}
		
		if (GetRemotelyActivatedItemBehaviour())
			GetRemotelyActivatedItemBehaviour().OnAfterLoad();
	}
	
	bool CanBeDisinfected()
	{
		return false;
	}
	
	
	//----------------------------------------------------------------
	override void OnVariablesSynchronized()
	{
		if (m_Initialized)
		{	
			#ifdef PLATFORM_CONSOLE
			//bruteforce it is
			if (IsSplitable())
			{
				UIScriptedMenu menu = GetGame().GetUIManager().FindMenu(MENU_INVENTORY);
				if (menu)
				{
					menu.Refresh();
				}
			}
			#endif
		}
				
		if (!dBodyIsDynamic(this) && m_WantPlayImpactSound)
		{
			PlayImpactSound(m_ConfigWeight, m_ImpactSpeed, m_ImpactSoundSurfaceHash);
			m_WantPlayImpactSound = false;
		}
		
		if (m_VarQuantity != m_VarQuantityPrev)
		{
			SetWeightDirty();
			m_VarQuantityPrev = m_VarQuantity;
		}
		if (m_VarWet != m_VarWetPrev)
		{
			OnWetChanged(m_VarWet,m_VarWetPrev);
			m_VarWetPrev = m_VarWet;
		}
		
		if (m_SoundSyncPlay != 0)
		{
			m_ItemSoundHandler.PlayItemSoundClient(m_SoundSyncPlay);
			m_SoundSyncPlay = 0;
		}
		if (m_SoundSyncStop != 0)
		{
			m_ItemSoundHandler.StopItemSoundClient(m_SoundSyncStop);
			m_SoundSyncStop = 0;
		}
			
		super.OnVariablesSynchronized();
	}

	//-------------------------	Quantity
	//----------------------------------------------------------------
	//! Set item quantity[related to varQuantity... config entry], destroy_config = true > if the quantity reaches varQuantityMin or lower and the item config contains the varQuantityDestroyOnMin = true entry, the item gets destroyed. destroy_forced = true means item gets destroyed when quantity reaches varQuantityMin or lower regardless of config setting, returns true if the item gets deleted
	override bool SetQuantity(float value, bool destroy_config = true, bool destroy_forced = false, bool allow_client = false, bool clamp_to_stack_max = true)
	{
		if (!IsServerCheck(allow_client))
			return false;
		
		if (!HasQuantity()) 
			return false;
		
		float min = GetQuantityMin();
		float max = GetQuantityMax();
		
		if (value <= (min + 0.001))
			value = min;
		
		if (value == min)
		{
			if (destroy_config)
			{
				bool dstr = ConfigGetBool("varQuantityDestroyOnMin");
				if (dstr)
				{
					m_VarQuantity = Math.Clamp(value, min, max);
					this.Delete();
					return true;
				}
			}
			else if (destroy_forced)
			{
				m_VarQuantity = Math.Clamp(value, min, max);
				this.Delete();
				return true;
			}
			// we get here if destroy_config IS true AND dstr(config destroy param) IS false;
			RemoveAllAgents();//we remove all agents when we got to the min value, but the item is not getting deleted
		}
		
		float delta = m_VarQuantity;
		m_VarQuantity = Math.Clamp(value, min, max);
		
		if (GetStoreLoadedQuantity() == float.LOWEST)//any other value means we are setting quantity from storage
		{
			delta = m_VarQuantity - delta;
			
			if (delta)
				OnQuantityChanged(delta);
		}
		
		SetVariableMask(VARIABLE_QUANTITY);
		
		return false;
	}

	//----------------------------------------------------------------
	//! add item quantity[related to varQuantity... config entry], destroy_config = true > if the quantity reaches varQuantityMin or lower and the item config contains the varQuantityDestroyOnMin = true entry, the item gets destroyed. destroy_forced = true means item gets destroyed when quantity reaches varQuantityMin or lower regardless of config setting, returns true if the item gets deleted
	bool AddQuantity(float value, bool destroy_config = true, bool destroy_forced = false)
	{	
		return SetQuantity(GetQuantity() + value, destroy_config, destroy_forced);
	}
	//----------------------------------------------------------------
	void SetQuantityMax()
	{
		float max = GetQuantityMax();
		SetQuantity(max);
	}
	
	override void SetQuantityToMinimum()
	{
		float min = GetQuantityMin();
		SetQuantity(min);
	}
	//----------------------------------------------------------------
	//! Sets quantity in normalized 0..1 form between the item's Min a Max values as defined by item's config(for Min 0 and Max 5000, setting 0.5 will result in value 2500)
	override void SetQuantityNormalized(float value, bool destroy_config = true, bool destroy_forced = false)
	{
		float value_clamped = Math.Clamp(value, 0, 1);//just to make sure
		int result = Math.Round(Math.Lerp(GetQuantityMin(), GetQuantityMax(), value_clamped));
		SetQuantity(result, destroy_config, destroy_forced);
	}
	
	//----------------------------------------------------------------
	//! Gets quantity in normalized 0..1 form between the item's Min a Max values as defined by item's config(for Min 0 and Max 5000, value 2500  will result in 0.5)
	override float GetQuantityNormalized()
	{
		return Math.InverseLerp(GetQuantityMin(), GetQuantityMax(),m_VarQuantity);
	}
	
	float GetQuantityNormalizedScripted()
	{
		return GetQuantityNormalized();
	}

	/*void SetAmmoNormalized(float value)
	{
		float value_clamped = Math.Clamp(value, 0, 1);
		Magazine this_mag = Magazine.Cast(this);
		int max_rounds = this_mag.GetAmmoMax();
		int result = value * max_rounds;//can the rounded if higher precision is required
		this_mag.SetAmmoCount(result);
	}*/
	//----------------------------------------------------------------
	override int GetQuantityMax()
	{
		int slot = -1;
		if (GetInventory())
		{
			InventoryLocation il = new InventoryLocation;
			GetInventory().GetCurrentInventoryLocation(il);
			slot = il.GetSlot();
		}
		
		return GetTargetQuantityMax(slot);
	}
	
	override int GetTargetQuantityMax(int attSlotID = -1)
	{
		float quantity_max = 0;
		
		if (IsSplitable()) //only stackable/splitable items can check for stack size
		{
			if (attSlotID != -1)
				quantity_max = InventorySlots.GetStackMaxForSlotId(attSlotID);
			
			if (quantity_max <= 0)
				quantity_max = m_VarStackMax;
		}
		
		if (quantity_max <= 0)
			quantity_max = m_VarQuantityMax;

		return quantity_max;
	}
	//----------------------------------------------------------------
	override int GetQuantityMin()
	{
		return m_VarQuantityMin;
	}
	//----------------------------------------------------------------
	int GetQuantityInit()
	{
		return m_VarQuantityInit;
	}
	
	//----------------------------------------------------------------
	override bool HasQuantity()
	{
		return !(GetQuantityMax() - GetQuantityMin() == 0);
	}

	override float GetQuantity()
	{
		return m_VarQuantity;
	}
	
	bool IsFullQuantity()
	{
		return GetQuantity() >= GetQuantityMax();
	}
	
	//Calculates weight of single item without attachments and cargo
	override float GetSingleInventoryItemWeightEx()
	{
		//this needs to be first stored inside local variables, when returned directly during inside  return call, the result is completely different due to enforce script bug
		float weightEx = GetWeightEx();//overall weight of the item
		float special = GetInventoryAndCargoWeight();//cargo and attachment weight
		return weightEx - special;
	}

	// Obsolete, use GetSingleInventoryItemWeightEx() instead
	float GetSingleInventoryItemWeight()
	{
		return GetSingleInventoryItemWeightEx();
	}
	
	override protected float GetWeightSpecialized(bool forceRecalc = false)
	{
		if (IsSplitable()) //quantity determines size of the stack
		{
			#ifdef DEVELOPER
			if (WeightDebug.m_VerbosityFlags & WeightDebugType.RECALC_FORCED)
			{
				WeightDebugData data1 = WeightDebug.GetWeightDebug(this);
				data1.SetCalcDetails("TIB1: " + GetConfigWeightModifiedDebugText() +" * " + GetQuantity()+"(quantity)");
			}
			#endif
			
			return GetQuantity() * GetConfigWeightModified();
		}
		else if (HasEnergyManager())// items with energy manager
		{
			#ifdef DEVELOPER
			if (WeightDebug.m_VerbosityFlags & WeightDebugType.RECALC_FORCED)
			{
				WeightDebugData data2 = WeightDebug.GetWeightDebug(this);
				data2.SetCalcDetails("TIB2: "+super.GetWeightSpecialized(forceRecalc)+"(contents weight) + " + GetConfigWeightModifiedDebugText() +" + " + GetCompEM().GetEnergy()+"(energy) * " + ConfigGetFloat("weightPerQuantityUnit") +"(weightPerQuantityUnit)");
			}
			#endif
			return super.GetWeightSpecialized(forceRecalc) + (GetCompEM().GetEnergy() * ConfigGetFloat("weightPerQuantityUnit")) + GetConfigWeightModified();
		}
		else//everything else
		{
			#ifdef DEVELOPER
			if (WeightDebug.m_VerbosityFlags & WeightDebugType.RECALC_FORCED)
			{
				WeightDebugData data3 = WeightDebug.GetWeightDebug(this);
				data3.SetCalcDetails("TIB3: "+super.GetWeightSpecialized(forceRecalc)+"(contents weight) + " + GetConfigWeightModifiedDebugText() +" + " + GetQuantity()+"(quantity) * " + ConfigGetFloat("weightPerQuantityUnit") +"(weightPerQuantityUnit))");
			}
			#endif
			return super.GetWeightSpecialized(forceRecalc) + (GetQuantity() * ConfigGetFloat("weightPerQuantityUnit")) + GetConfigWeightModified();
		}
	}

	//! Returns the number of items in cargo, otherwise returns 0(non-cargo objects). Recursive.
	int GetNumberOfItems()
	{
		int item_count = 0;
		ItemBase item;
		
		if (GetInventory().GetCargo() != NULL)
		{
			item_count = GetInventory().GetCargo().GetItemCount();
		}
		
		for (int i = 0; i < GetInventory().AttachmentCount(); i++)
		{
			Class.CastTo(item,GetInventory().GetAttachmentFromIndex(i));
			if (item)
				item_count += item.GetNumberOfItems();
		}
		return item_count;
	}
	
	//! Obsolete, use GetWeightEx instead
	float GetUnitWeight(bool include_wetness = true)
	{
		float weight = 0;
		float wetness = 1;
		if (include_wetness)
			wetness += GetWet();
		if (IsSplitable()) //quantity determines size of the stack
		{
			weight = wetness * m_ConfigWeight;
		}
		else if (IsLiquidContainer()) //is a liquid container, default liquid weight is set to 1. May revisit later?
		{
			weight = 1;
		}
		return weight;
	}
	
	//-----------------------------------------------------------------
	
	override void ClearInventory()
	{
		if ((GetGame().IsServer() || !GetGame().IsMultiplayer()) && GetInventory())
		{
			GameInventory inv = GetInventory();
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
		}
	}
	
	//-------------------------	Energy

	//----------------------------------------------------------------
	float GetEnergy()
	{
		float energy = 0;
		if (HasEnergyManager())
		{
			energy = GetCompEM().GetEnergy();
		}
		return energy;
	}
	
	
	override void OnEnergyConsumed()
	{
		super.OnEnergyConsumed();
		
		ConvertEnergyToQuantity();
	}

	override void OnEnergyAdded() 
	{
		super.OnEnergyAdded();
		
		ConvertEnergyToQuantity();
	}
	
	// Converts energy (from Energy Manager) to quantity, if enabled.
	void ConvertEnergyToQuantity()
	{
		if (GetGame().IsServer() && HasEnergyManager() && GetCompEM().HasConversionOfEnergyToQuantity())
		{
			if (HasQuantity())
			{
				float energy_0to1 = GetCompEM().GetEnergy0To1();
				SetQuantityNormalized(energy_0to1);
			}
		}
	}

	//----------------------------------------------------------------
	float GetHeatIsolationInit()
	{
		return ConfigGetFloat("heatIsolation");
	}

	float GetHeatIsolation()
	{
		return m_HeatIsolation;
	}

	float GetDryingIncrement(string pIncrementName)
	{
		string paramPath = string.Format("CfgVehicles %1 EnvironmentWetnessIncrements Drying %2", GetType(), pIncrementName);
		if (GetGame().ConfigIsExisting(paramPath))
			return GetGame().ConfigGetFloat(paramPath);
		
		return 0.0;
	}
	
	float GetSoakingIncrement(string pIncrementName)
	{
		string paramPath = string.Format("CfgVehicles %1 EnvironmentWetnessIncrements Soaking %2", GetType(), pIncrementName);
		if (GetGame().ConfigIsExisting(paramPath))
			return GetGame().ConfigGetFloat(paramPath);
		
		return 0.0;
	}
	//----------------------------------------------------------------
	override void SetWet(float value, bool allow_client = false)
	{
		if (!IsServerCheck(allow_client))
			return;
		
		float min = GetWetMin();
		float max = GetWetMax();
		
		float previousValue = m_VarWet;
		
		m_VarWet = Math.Clamp(value, min, max);
		
		if (previousValue != m_VarWet)
		{
			SetVariableMask(VARIABLE_WET);
			OnWetChanged(m_VarWet, previousValue);
		}
	}
	//----------------------------------------------------------------
	override void AddWet(float value)
	{
		SetWet(GetWet() + value);
	}
	//----------------------------------------------------------------
	override void SetWetMax()
	{
		SetWet(m_VarWetMax);
	}
	//----------------------------------------------------------------
	override float GetWet()
	{
		return m_VarWet;
	}
	//----------------------------------------------------------------
	override float GetWetMax()
	{
		return m_VarWetMax;
	}
	//----------------------------------------------------------------
	override float GetWetMin()
	{
		return m_VarWetMin;
	}
	//----------------------------------------------------------------
	override float GetWetInit()
	{
		return m_VarWetInit;
	}
	//----------------------------------------------------------------
	override void OnWetChanged(float newVal, float oldVal)
	{
		EWetnessLevel newLevel = GetWetLevelInternal(newVal);
		EWetnessLevel oldLevel = GetWetLevelInternal(oldVal);
		if (newLevel != oldLevel)
		{
			OnWetLevelChanged(newLevel,oldLevel);
		}
	}
	
	override void OnWetLevelChanged(EWetnessLevel newLevel, EWetnessLevel oldLevel)
	{
		SetWeightDirty();
	}
	
	override EWetnessLevel GetWetLevel()
	{
		return GetWetLevelInternal(m_VarWet);
	}

	//----------------------------------------------------------------
	
	override void SetStoreLoad(bool value)
	{
		m_IsStoreLoad = value;
	}
	
	override bool IsStoreLoad()
	{
		return m_IsStoreLoad;
	}
	
	override void SetStoreLoadedQuantity(float value)
	{
		m_StoreLoadedQuantity = value;
	}
	
	override float GetStoreLoadedQuantity()
	{
		return m_StoreLoadedQuantity;
	}
	
	//----------------------------------------------------------------
	
	float GetItemModelLength()
	{
		if (ConfigIsExisting("itemModelLength"))
		{
			return ConfigGetFloat("itemModelLength");
		}
		return 0;
	}
	
	float GetItemAttachOffset()
	{
		if (ConfigIsExisting("itemAttachOffset"))
		{
			return ConfigGetFloat("itemAttachOffset");
		}
		return 0;
	}
	
	override void SetCleanness(int value, bool allow_client = false)
	{
		if (!IsServerCheck(allow_client)) 
			return;
		
		int previousValue = m_Cleanness;
		
		m_Cleanness = Math.Clamp(value, m_CleannessMin, m_CleannessMax);
		
		if (previousValue != m_Cleanness)
			SetVariableMask(VARIABLE_CLEANNESS);
	}
	
	override int GetCleanness()
	{
		return m_Cleanness;
	}
	
	bool AllowFoodConsumption()
	{
		return true;
	}
	
	//----------------------------------------------------------------
	// ATTACHMENT LOCKING
	// Getters relevant to generic ActionLockAttachment
	int GetLockType()
	{
		return m_LockType;
	}
	
	string GetLockSoundSet()
	{
		return m_LockSoundSet;
	}

	//----------------------------------------------------------------
	//-------------------------	Color
	// sets items color variable given color components
	override void SetColor(int r, int g, int b, int a)
	{
		m_ColorComponentR = r;
		m_ColorComponentG = g;
		m_ColorComponentB = b;
		m_ColorComponentA = a;
		SetVariableMask(VARIABLE_COLOR);
	}
	//! gets item's color variable as components
	override void GetColor(out int r,out int g,out int b,out int a)
	{
		r = m_ColorComponentR;
		g = m_ColorComponentG;
		b = m_ColorComponentB;
		a = m_ColorComponentA;
	}
	
	bool IsColorSet()
	{
		return IsVariableSet(VARIABLE_COLOR);
	}
	
	//! Returns item's PROCEDURAL color as formated string, i.e. "#(argb,8,8,3)color(0.15,0.15,0.15,1.0,CO)"
	string GetColorString()
	{
		int r,g,b,a;
		GetColor(r,g,b,a);
		r = r/255;
		g = g/255;
		b = b/255;
		a = a/255;
		return MiscGameplayFunctions.GetColorString(r, g, b, a);
	}
	//----------------------------------------------------------------
	//-------------------------	LiquidType

	override void SetLiquidType(int value, bool allow_client = false)
	{
		if (!IsServerCheck(allow_client))
			return;
		
		int old = m_VarLiquidType;
		m_VarLiquidType = value;
		OnLiquidTypeChanged(old,value);
		SetVariableMask(VARIABLE_LIQUIDTYPE);
	}
	
	int GetLiquidTypeInit()
	{
		return ConfigGetInt("varLiquidTypeInit");
	}
	
	override int GetLiquidType()
	{
		return m_VarLiquidType;
	}
	
	protected void OnLiquidTypeChanged(int oldType, int newType)
	{
		if (newType == LIQUID_NONE && GetIsFrozen())
			SetFrozen(false);
	}

	//! To be called on moving item within character's inventory; 'player' should never be null
	void UpdateQuickbarShortcutVisibility(PlayerBase player)
	{
		player.SetEnableQuickBarEntityShortcut(this,!GetHierarchyParent() || GetHierarchyParent().GetInventory().AreChildrenAccessible());
	}
	
	// -------------------------------------------------------------------------
	//! Event called on item when it is placed in the player(Man) inventory, passes the owner as a parameter
	void OnInventoryEnter(Man player)
	{
		PlayerBase nplayer;
		if (PlayerBase.CastTo(nplayer, player))
		{
			m_CanPlayImpactSound = true;
			//nplayer.OnItemInventoryEnter(this);
			nplayer.SetEnableQuickBarEntityShortcut(this,!GetHierarchyParent() || GetHierarchyParent().GetInventory().AreChildrenAccessible());	
		}
	}
	
	// -------------------------------------------------------------------------
	//! Event called on item when it is removed from the player(Man) inventory, passes the old owner as a parameter
	void OnInventoryExit(Man player)
	{
		PlayerBase nplayer;
		if (PlayerBase.CastTo(nplayer,player))
		{		
			//nplayer.OnItemInventoryExit(this);
			nplayer.SetEnableQuickBarEntityShortcut(this,false);

		}
		
		//if (!GetGame().IsDedicatedServer())
		player.GetHumanInventory().ClearUserReservedLocationForContainer(this);
		
		
		if (HasEnergyManager())
		{
			GetCompEM().UpdatePlugState(); // Unplug the el. device if it's necesarry.
		}
	}

	// ADVANCED PLACEMENT EVENTS
	override void OnPlacementStarted(Man player) 
	{
		super.OnPlacementStarted(player);
		
		SetTakeable(false);
	}
	
	override void OnPlacementComplete(Man player, vector position = "0 0 0", vector orientation = "0 0 0")
	{
		if (m_AdminLog)
		{
			m_AdminLog.OnPlacementComplete(player, this);
		}
		
		super.OnPlacementComplete(player, position, orientation);
	}
		
	//-----------------------------
	// AGENT SYSTEM
	//-----------------------------
	//--------------------------------------------------------------------------
	bool ContainsAgent(int agent_id)
	{
		if (agent_id & m_AttachedAgents)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	//--------------------------------------------------------------------------
	override void RemoveAgent(int agent_id)
	{
		if (ContainsAgent(agent_id))
		{
			m_AttachedAgents = ~agent_id & m_AttachedAgents;
		}
	}

	//--------------------------------------------------------------------------
	override void RemoveAllAgents()
	{
		m_AttachedAgents = 0;
	}
	//--------------------------------------------------------------------------
	override void RemoveAllAgentsExcept(int agent_to_keep)
	{
		m_AttachedAgents = m_AttachedAgents & agent_to_keep;
	}
	// -------------------------------------------------------------------------
	override void InsertAgent(int agent, float count = 1)
	{
		if (count < 1)
			return;
		//Debug.Log("Inserting Agent on item: " + agent.ToString() +" count: " + count.ToString());
		m_AttachedAgents = (agent | m_AttachedAgents);
	}
	
	//!transfer agents from another item
	void TransferAgents(int agents)
	{
		m_AttachedAgents = (m_AttachedAgents | agents);
	}
	
	// -------------------------------------------------------------------------
	override int GetAgents()
	{
		return m_AttachedAgents;
	}
	//----------------------------------------------------------------------
	
	/*int GetContaminationType()
	{
		int contamination_type;
		
		const int CONTAMINATED_MASK = eAgents.CHOLERA | eAgents.INFLUENZA | eAgents.SALMONELLA | eAgents.BRAIN;
		const int POISONED_MASK = eAgents.FOOD_POISON | eAgents.CHEMICAL_POISON;
		const int NERVE_GAS_MASK = eAgents.CHEMICAL_POISON;
		const int DIRTY_MASK = eAgents.WOUND_AGENT;
		
		Edible_Base edible = Edible_Base.Cast(this);
		int agents = GetAgents();
		if (edible)
		{
			NutritionalProfile profile = Edible_Base.GetNutritionalProfile(edible);
			if (profile)
			{
				agents = agents | profile.GetAgents();//merge item's agents with nutritional agents
			}
		}
		if (agents & CONTAMINATED_MASK)
		{
			contamination_type = contamination_type | EContaminationTypes.ITEM_BADGE_CONTAMINATED;
		}
		if (agents & POISONED_MASK)
		{
			contamination_type = contamination_type | EContaminationTypes.ITEM_BADGE_POISONED;
		}
		if (agents & NERVE_GAS_MASK)
		{
			contamination_type = contamination_type | EContaminationTypes.ITEM_BADGE_NERVE_GAS;
		}
		if (agents & DIRTY_MASK)
		{
			contamination_type = contamination_type | EContaminationTypes.ITEM_BADGE_DIRTY;
		}
		
		return agents;
	}*/
	
	// -------------------------------------------------------------------------
	bool LoadAgents(ParamsReadContext ctx, int version)
	{
		if (!ctx.Read(m_AttachedAgents))
			return false;
		return true;
	}
	// -------------------------------------------------------------------------
	void SaveAgents(ParamsWriteContext ctx)
	{
		
		ctx.Write(m_AttachedAgents);
	}
	// -------------------------------------------------------------------------
	
	//! Roof check for entity, limited by time (anti-spam solution)
	override void CheckForRoofLimited(float timeTresholdMS = 3000)
	{
		super.CheckForRoofLimited(timeTresholdMS);
		
		float time = GetGame().GetTime();
		if ((time - m_PreviousRoofTestTime) >= timeTresholdMS)
		{
			m_PreviousRoofTestTime = time;
			SetRoofAbove(MiscGameplayFunctions.IsUnderRoof(this));
		}
	}
	
	// returns item's protection level against enviromental hazard, for masks with filters, returns the filters protection for valid filter, otherwise 0
	float GetProtectionLevel(int type, bool consider_filter = false, int system = 0)
	{
		if (IsDamageDestroyed() || (HasQuantity() && GetQuantity() <= 0))
		{
			return 0;
		}
		
		if (GetInventory().GetAttachmentSlotsCount() != 0)//is it an item with attachable filter ?
		{
			ItemBase filter = ItemBase.Cast(FindAttachmentBySlotName("GasMaskFilter"));
			if (filter)
				return filter.GetProtectionLevel(type, false, system);//it's a valid filter, return the protection
			else
				return 0;//otherwise return 0 when no filter attached
		}

		string subclassPath, entryName;

		switch (type)
		{
			case DEF_BIOLOGICAL:
				entryName = "biological";
				break;
			case DEF_CHEMICAL:
				entryName = "chemical";
				break;	
			default:
				entryName = "biological";
				break;
		}
		
		subclassPath = "CfgVehicles " + this.GetType() + " Protection ";
		
		return GetGame().ConfigGetFloat(subclassPath + entryName);
	}
	
	
	
	//! Called when entity is being created as new by CE/ Debug
	override void EEOnCECreate()
	{
		if (!IsMagazine())
			SetCEBasedQuantity();

		SetZoneDamageCEInit();
	}
	
	
	//-------------------------
	// OPEN/CLOSE USER ACTIONS
	//-------------------------
	//! Implementations only
	void Open();
	void Close();
	bool IsOpen()
	{
		return true;
	}

	override bool CanDisplayCargo()
	{
		return IsOpen();
	}
	
	
	// ------------------------------------------------------------
	// CONDITIONS
	// ------------------------------------------------------------
	override bool CanPutInCargo(EntityAI parent)
	{
		if (parent)
		{
			if (parent.IsInherited(DayZInfected))
				return true;

			if (!parent.IsRuined())
				return true;
		}
		
		return true;
	}	
	
	override bool CanPutAsAttachment(EntityAI parent)
	{
		if (!super.CanPutAsAttachment(parent))
		{
			return false;
		}
		
		if (!IsRuined() && !parent.IsRuined())
		{
			return true;
		}

		return false;
	}

	override bool CanReceiveItemIntoCargo(EntityAI item)
	{
		//removed 15.06. coz of loading from storage -> after load items in cargo was lost -> waiting for proper solution
		//if (GetHealthLevel() == GameConstants.STATE_RUINED)
		//	return false;
		
		return super.CanReceiveItemIntoCargo(item);
	}

	override bool CanReceiveAttachment(EntityAI attachment, int slotId)
	{
		//removed 15.06. coz of loading from storage -> after load items in cargo was lost -> waiting for proper solution
		//if (GetHealthLevel() == GameConstants.STATE_RUINED)
		//	return false;
		
		GameInventory attachmentInv = attachment.GetInventory();
		if (attachmentInv && attachmentInv.GetCargo() && attachmentInv.GetCargo().GetItemCount() > 0)
		{
			if (GetHierarchyParent() && !GetHierarchyParent().IsInherited(PlayerBase))
				return false;
		}
		
		InventoryLocation loc = new InventoryLocation();
		attachment.GetInventory().GetCurrentInventoryLocation(loc);
		if (loc && loc.IsValid() && !GetInventory().AreChildrenAccessible())
			return false;
		
		return super.CanReceiveAttachment(attachment, slotId);
	}
	
	override bool CanReleaseAttachment(EntityAI attachment)
	{
		if (!super.CanReleaseAttachment(attachment))
			return false;
		
		return GetInventory().AreChildrenAccessible();
	}
	
	/*override bool CanLoadAttachment(EntityAI attachment)
	{
		//removed 15.06. coz of loading from storage -> after load items in cargo was lost -> waiting for proper solution
		//if (GetHealthLevel() == GameConstants.STATE_RUINED)
		//	return false;
		
		GameInventory attachmentInv = attachment.GetInventory();
		if (attachmentInv && attachmentInv.GetCargo() && attachmentInv.GetCargo().GetItemCount() > 0)
		{
			bool boo = (GetHierarchyParent() && !GetHierarchyParent().IsInherited(PlayerBase));
			ErrorEx("CanLoadAttachment | this: " + this + " | attachment: " + attachment + " | boo: " + boo,ErrorExSeverity.INFO);
			
			if (GetHierarchyParent() && !GetHierarchyParent().IsInherited(PlayerBase))
				return false;
		}
		
		return super.CanLoadAttachment(attachment);
	}*/
	
	// Plays muzzle flash particle effects
	static void PlayFireParticles(ItemBase weapon, int muzzle_index, string ammoType, ItemBase muzzle_owner, ItemBase suppressor, string config_to_search)
	{
		int id = muzzle_owner.GetMuzzleID();
		array<ref WeaponParticlesOnFire> WPOF_array = m_OnFireEffect.Get(id);
		
		if (WPOF_array)
		{
			for (int i = 0; i < WPOF_array.Count(); i++)
			{
				WeaponParticlesOnFire WPOF = WPOF_array.Get(i);
				
				if (WPOF)
				{
					WPOF.OnActivate(weapon, muzzle_index, ammoType, muzzle_owner, suppressor, config_to_search);
				}
			}
		}
	}
	
	// Plays bullet eject particle effects (usually just smoke, the bullet itself is a 3D model and is not part of this function)
	static void PlayBulletCasingEjectParticles(ItemBase weapon, string ammoType, ItemBase muzzle_owner, ItemBase suppressor, string config_to_search)
	{
		int id = muzzle_owner.GetMuzzleID();
		array<ref WeaponParticlesOnBulletCasingEject> WPOBE_array = m_OnBulletCasingEjectEffect.Get(id);
		
		if (WPOBE_array)
		{
			for (int i = 0; i < WPOBE_array.Count(); i++)
			{
				WeaponParticlesOnBulletCasingEject WPOBE = WPOBE_array.Get(i);
				
				if (WPOBE)
				{
					WPOBE.OnActivate(weapon, 0, ammoType, muzzle_owner, suppressor, config_to_search);
				}
			}
		}
	}
	
	// Plays all weapon overheating particles
	static void PlayOverheatingParticles(ItemBase weapon, string ammoType, ItemBase muzzle_owner, ItemBase suppressor, string config_to_search)
	{
		int id = muzzle_owner.GetMuzzleID();
		array<ref WeaponParticlesOnOverheating> WPOOH_array = weapon.m_OnOverheatingEffect.Get(id);
		
		if (WPOOH_array)
		{
			for (int i = 0; i < WPOOH_array.Count(); i++)
			{
				WeaponParticlesOnOverheating WPOOH = WPOOH_array.Get(i);
				
				if (WPOOH)
				{
					WPOOH.OnActivate(weapon, 0, ammoType, muzzle_owner, suppressor, config_to_search);
				}
			}
		}
	}
	
	// Updates all weapon overheating particles
	static void UpdateOverheatingParticles(ItemBase weapon, string ammoType, ItemBase muzzle_owner, ItemBase suppressor, string config_to_search)
	{
		int id = muzzle_owner.GetMuzzleID();
		array<ref WeaponParticlesOnOverheating> WPOOH_array = weapon.m_OnOverheatingEffect.Get(id);
		
		if (WPOOH_array)
		{
			for (int i = 0; i < WPOOH_array.Count(); i++)
			{
				WeaponParticlesOnOverheating WPOOH = WPOOH_array.Get(i);
				
				if (WPOOH)
				{
					WPOOH.OnUpdate(weapon, ammoType, muzzle_owner, suppressor, config_to_search);
				}
			}
		}
	}
	
	// Stops overheating particles
	static void StopOverheatingParticles(ItemBase weapon, string ammoType, ItemBase muzzle_owner, ItemBase suppressor, string config_to_search)
	{
		int id = muzzle_owner.GetMuzzleID();
		array<ref WeaponParticlesOnOverheating> WPOOH_array = weapon.m_OnOverheatingEffect.Get(id);
		
		if (WPOOH_array)
		{
			for (int i = 0; i < WPOOH_array.Count(); i++)
			{
				WeaponParticlesOnOverheating WPOOH = WPOOH_array.Get(i);
				
				if (WPOOH)
				{
					WPOOH.OnDeactivate(weapon, ammoType, muzzle_owner, suppressor, config_to_search);
				}
			}
		}
	}
	
	//----------------------------------------------------------------
	//Item Behaviour - unified approach
	override bool IsHeavyBehaviour()
	{
		if (m_ItemBehaviour == 0)
		{	
			return true;
		}
		
		return false;
	}
	
	override bool IsOneHandedBehaviour()
	{
		if (m_ItemBehaviour == 1)
		{
			return true;	
		}
		
		return false;
	}
	
	override bool IsTwoHandedBehaviour()
	{
		if (m_ItemBehaviour == 2)
		{
			return true;
		}
			
		return false;
	}
	
	bool IsDeployable()
	{
		return false;
	}
	
	//!how long it takes to deploy this item in seconds
	float GetDeployTime()
	{
		return UATimeSpent.DEFAULT_DEPLOY;
	}
	

	//----------------------------------------------------------------
	// Item Targeting (User Actions)
	override void SetTakeable(bool pState)
	{
		m_IsTakeable = pState;
		SetSynchDirty();
	}

	override bool IsTakeable()
	{
		return m_IsTakeable;
	}
	
	// For cases where we want to show object widget which cant be taken to hands
	bool IsActionTargetVisible()
	{
		return false;
	}

	//! Attachment Sound Type getting from config file
	protected void PreLoadSoundAttachmentType()
	{
		string att_type = "None";

		if (ConfigIsExisting("soundAttType"))
		{
			att_type = ConfigGetString("soundAttType");
		}
		
		m_SoundAttType = att_type;
	}
	
	override string GetAttachmentSoundType()
	{	
		return m_SoundAttType;
	}
	
	//----------------------------------------------------------------
	//SOUNDS - ItemSoundHandler
	//----------------------------------------------------------------
	
	string GetPlaceSoundset();			// played when deploy starts
	string GetLoopDeploySoundset();		// played when deploy starts and stopped when it finishes
	string GetDeploySoundset();			// played when deploy sucessfully finishes
	string GetLoopFoldSoundset();		// played when fold starts and stopped when it finishes
	string GetFoldSoundset();			// played when fold sucessfully finishes
			
	ItemSoundHandler GetItemSoundHandler()
	{
		if (!m_ItemSoundHandler)
			m_ItemSoundHandler = new ItemSoundHandler(this);
		
		return m_ItemSoundHandler;
	}
	
	// override to initialize sounds
	protected void InitItemSounds()
	{
		if (GetPlaceSoundset() == string.Empty && GetDeploySoundset() == string.Empty && GetLoopDeploySoundset() == string.Empty)
			return;

		ItemSoundHandler handler = GetItemSoundHandler();

		if (GetPlaceSoundset() != string.Empty)
			handler.AddSound(SoundConstants.ITEM_PLACE, GetPlaceSoundset());
		
		if (GetDeploySoundset() != string.Empty)
			handler.AddSound(SoundConstants.ITEM_DEPLOY, GetDeploySoundset());
		
		SoundParameters params = new SoundParameters();
		params.m_Loop = true;
		if (GetLoopDeploySoundset() != string.Empty)
			handler.AddSound(SoundConstants.ITEM_DEPLOY_LOOP, GetLoopDeploySoundset(), params);
	}
	
	// Start sound using ItemSoundHandler
	void StartItemSoundServer(int id)
	{
		if (!GetGame().IsServer())
			return;
		
		m_SoundSyncPlay = id;
		SetSynchDirty();
		
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(ClearStartItemSoundServer);	// in case one is queued already
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(ClearStartItemSoundServer, 100);
	}
	
	// Stop sound using ItemSoundHandler
	void StopItemSoundServer(int id)
	{
		if (!GetGame().IsServer())
			return;
		
		m_SoundSyncStop = id;
		SetSynchDirty();
		
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(ClearStopItemSoundServer);	// in case one is queued already
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(ClearStopItemSoundServer, 100);
	}
	
	protected void ClearStartItemSoundServer()
	{
		m_SoundSyncPlay = 0;
	}
	
	protected void ClearStopItemSoundServer()
	{
		m_SoundSyncStop = 0;
	}
				
	//! Plays sound on item attach. Be advised, the config structure may slightly change in 1.11 update to allow for more complex use.
	void PlayAttachSound(string slot_type)
	{
		if (!GetGame().IsDedicatedServer())
		{
			if (ConfigIsExisting("attachSoundSet"))
			{
				string cfg_path = "";
				string soundset = "";
				string type_name = GetType();
				
				TStringArray cfg_soundset_array = new TStringArray;
				TStringArray cfg_slot_array = new TStringArray;
				ConfigGetTextArray("attachSoundSet",cfg_soundset_array);
				ConfigGetTextArray("attachSoundSlot",cfg_slot_array);
				
				if (cfg_soundset_array.Count() > 0 && cfg_soundset_array.Count() == cfg_slot_array.Count())
				{
					for (int i = 0; i < cfg_soundset_array.Count(); i++)
					{
						if (cfg_slot_array[i] == slot_type)
						{
							soundset = cfg_soundset_array[i];
							break;
						}
					}
				}
				
				if (soundset != "")
				{
					EffectSound sound =	SEffectManager.PlaySound(soundset, GetPosition());
					sound.SetAutodestroy(true);
				}
			}
		}
	}
	
	void PlayDetachSound(string slot_type)
	{
		//TODO - evaluate if needed and devise universal config structure if so
	}
	
	void OnApply(PlayerBase player);
	
	float GetBandagingEffectivity()
	{
		return 1.0;
	};
	//returns applicable selection
	array<string> GetHeadHidingSelection()
	{
		return m_HeadHidingSelections;
	}
	
	bool HidesSelectionBySlot()
	{
		return m_HideSelectionsBySlot;
	}
	
	WrittenNoteData GetWrittenNoteData() {};
	
	void StopItemDynamicPhysics()
	{
		SetDynamicPhysicsLifeTime(0.01);
		m_ItemBeingDroppedPhys = false;
	}
	
	void PerformDamageSystemReinit()
	{
		array<string> zone_names = new array<string>;
		GetDamageZones(zone_names);
		for (int i = 0; i < zone_names.Count(); i++)
		{
			SetHealthMax(zone_names.Get(i),"Health");
		}
		SetHealthMax("","Health");
	}
	
	//! Sets zone damages to match randomized global health set by CE (CE spawn only)
	void SetZoneDamageCEInit()
	{
		float global_health = GetHealth01("","Health");
		array<string> zones = new array<string>;
		GetDamageZones(zones);
		//set damage of all zones to match global health level
		for (int i = 0; i < zones.Count(); i++)
		{
			SetHealth01(zones.Get(i),"Health",global_health);
		}
	}
	
	//!DEPRECATED in use, but returns correct values nontheless. Check performed elsewhere
	bool IsCoverFaceForShave(string slot_name)
	{
		return IsExclusionFlagPresent(PlayerBase.GetFaceCoverageShaveValues());
	}
	
	void ProcessItemWetness(float delta, bool hasParent, bool hasRootAsPlayer, ItemBase refParentIB)
	{
		if (!hasRootAsPlayer)
		{
			if (refParentIB)
			{
				// parent is wet
				if ((refParentIB.GetWet() >= GameConstants.STATE_SOAKING_WET) && (m_VarWet < m_VarWetMax))
					AddWet(delta * GameConstants.WETNESS_RATE_WETTING_INSIDE);
				// parent has liquid inside
				else if ((refParentIB.GetLiquidType() != 0) && (refParentIB.GetQuantity() > 0) && (m_VarWet < m_VarWetMax))
					AddWet(delta * GameConstants.WETNESS_RATE_WETTING_LIQUID);
				// drying
				else if (m_VarWet > m_VarWetMin)						
					AddWet(-1 * delta * GetDryingIncrement("ground") * 2);
			}
			else
			{
				// drying on ground or inside non-itembase (car, ...)
				if (m_VarWet > m_VarWetMin)
					AddWet(-1 * delta * GetDryingIncrement("ground"));
			}
		}
	}
	
	void ProcessItemTemperature(float delta, bool hasParent, bool hasRootAsPlayer, ItemBase refParentIB)
	{
		if (CanHaveTemperature() && !IsSelfAdjustingTemperature() && !GetHierarchyRoot().IsSelfAdjustingTemperature())
		{
			float target = g_Game.GetMission().GetWorldData().GetBaseEnvTemperatureAtObject(this);
			if (GetTemperature() != target || !IsFreezeThawProgressFinished())
			{
				float heatPermCoef = 1.0;
				EntityAI ent = this;
				while (ent)
				{
					heatPermCoef *= ent.GetHeatPermeabilityCoef();
					ent = ent.GetHierarchyParent();
				}
				
				SetTemperatureEx(new TemperatureDataInterpolated(target,ETemperatureAccessTypes.ACCESS_WORLD,delta,GameConstants.TEMP_COEF_WORLD,heatPermCoef));
			}
		}
	}
	
	void HierarchyCheck(out bool hasParent, out bool hasRootAsPlayer, out ItemBase refParentIB)
	{
		// hierarchy check for an item to decide whether it has some parent and it is in some player inventory
		EntityAI parent = GetHierarchyParent();
		if (!parent)
		{
			hasParent = false;
			hasRootAsPlayer = false;
		}
		else
		{
			hasParent = true;
			hasRootAsPlayer = (GetHierarchyRootPlayer() != null);
			refParentIB = ItemBase.Cast(parent);
		}
	}
	
	protected void ProcessDecay(float delta, bool hasRootAsPlayer)
	{
		// this is stub, implemented on Edible_Base
	}
	
	bool CanDecay()
	{
		// return true used on selected food clases so they can decay
		return false;
	}
	
	protected bool CanProcessDecay()
	{
		// this is stub, implemented on Edible_Base class
		// used to determine whether it is still necessary for the food to decay
		return false;
	}
	
	protected bool CanHaveWetness()
	{
		// return true used on selected items that have a wetness effect
		return false;
	}
	
	//! Items cannot be consumed if frozen by default. Override for exceptions.
	bool CanBeConsumed(ConsumeConditionData data = null)
	{
		return !GetIsFrozen() && IsOpen();
	}
	
	override void ProcessVariables()
	{
		bool hasParent = false, hasRootAsPlayer = false;
		ItemBase refParentIB;
		
		bool wwtu = g_Game.IsWorldWetTempUpdateEnabled();
		bool foodDecay = g_Game.IsFoodDecayEnabled();
		
		if (wwtu || foodDecay)
		{
			bool processWetness = wwtu && CanHaveWetness();
			bool processTemperature = wwtu && CanHaveTemperature();
			bool processDecay  = foodDecay && CanDecay() && CanProcessDecay();
			
			if (processWetness || processTemperature || processDecay)
			{
				HierarchyCheck(hasParent, hasRootAsPlayer, refParentIB);
			
				if (processWetness)
					ProcessItemWetness(m_ElapsedSinceLastUpdate, hasParent, hasRootAsPlayer, refParentIB);					
				
				if (processTemperature)
					ProcessItemTemperature(m_ElapsedSinceLastUpdate, hasParent, hasRootAsPlayer, refParentIB);
			
				if (processDecay)
					ProcessDecay(m_ElapsedSinceLastUpdate, hasRootAsPlayer);
			}
		}
	}
	
	//! Used in heat comfort calculations only!
	float GetTemperaturePerQuantityWeight()
	{
		return m_TemperaturePerQuantityWeight * GameConstants.ITEM_TEMPERATURE_QUANTITY_WEIGHT_MULTIPLIER;
	}
	
	override float GetTemperatureFreezeThreshold()
	{
		if (IsLiquidContainer() && GetLiquidType() != LIQUID_NONE)
			return Liquid.GetFreezeThreshold(GetLiquidType());
		
		return super.GetTemperatureFreezeThreshold();
	}
	
	override float GetTemperatureThawThreshold()
	{
		if (IsLiquidContainer() && GetLiquidType() != LIQUID_NONE)
			return Liquid.GetThawThreshold(GetLiquidType());
		
		return super.GetTemperatureThawThreshold();
	}
	
	override float GetItemOverheatThreshold()
	{
		if (IsLiquidContainer() && GetLiquidType() != LIQUID_NONE)
			return Liquid.GetBoilThreshold(GetLiquidType());
		
		return super.GetItemOverheatThreshold();
	}
	
	override float GetTemperatureFreezeTime()
	{
		if (HasQuantity())
			return Math.Lerp(GameConstants.TEMPERATURE_TIME_FREEZE_MIN,Math.Max(GameConstants.TEMPERATURE_TIME_FREEZE_MIN,super.GetTemperatureFreezeTime()),GetQuantityNormalized());
		
		return super.GetTemperatureFreezeTime();
	}
	
	override float GetTemperatureThawTime()
	{
		if (HasQuantity())
			return Math.Lerp(GameConstants.TEMPERATURE_TIME_THAW_MIN,Math.Max(GameConstants.TEMPERATURE_TIME_FREEZE_MIN,super.GetTemperatureThawTime()),GetQuantityNormalized());
		
		return super.GetTemperatureThawTime();
	}
	
	//! from enviro source
	void AffectLiquidContainerOnFill(int liquid_type, float amount);
	//! from other liquid container source
	void AffectLiquidContainerOnTransfer(int liquidType, float amount, float sourceLiquidTemperature);
	
	bool IsCargoException4x3(EntityAI item)
	{
		return (item.IsKindOf("Cauldron") || item.IsKindOf("Pot") || item.IsKindOf("FryingPan") || item.IsKindOf("SmallProtectorCase") || (item.IsKindOf("PortableGasStove") && item.FindAttachmentBySlotName("CookingEquipment")));
	}
	
	void CopyScriptPropertiesFrom(EntityAI oldItem)
	{
		MiscGameplayFunctions.TransferItemProperties(oldItem, this);
	}
	
	//! Adds a light source child
	void AddLightSourceItem(ItemBase lightsource)
	{
		m_LightSourceItem = lightsource;
	}
	
	void RemoveLightSourceItem()
	{
		m_LightSourceItem = null;
	}
	
	ItemBase GetLightSourceItem()
	{
		return m_LightSourceItem;
	}
	
	//! returns an array of possible finishers
	array<int> GetValidFinishers()
	{
		return null;
	}
	
	//! If we need a different (handheld)item action widget displayed, the logic goes in here
	bool GetActionWidgetOverride(out typename name)
	{
		return false;
	}
	
	bool PairWithDevice(notnull ItemBase otherDevice)
	{
		if (GetGame().IsServer())
		{
			ItemBase explosive = otherDevice;
			RemoteDetonatorTrigger trg = RemoteDetonatorTrigger.Cast(this);
			if (!trg)
			{
				trg = RemoteDetonatorTrigger.Cast(otherDevice);
				explosive = this;
			}
			
			explosive.PairRemote(trg);
			trg.SetControlledDevice(explosive);
			
			int persistentID = RemotelyActivatedItemBehaviour.GeneratePersistentID();
			trg.SetPersistentPairID(persistentID);
			explosive.SetPersistentPairID(persistentID);
			
			return true;
		}
		return false;
	}
	
	//! generic effectivity as a bait for animal catching
	float GetBaitEffectivity()
	{
		float ret = 1.0;
		if (HasQuantity())
			ret *= GetQuantityNormalized();
		ret *= GetHealth01();
		
		return ret;
	}

	#ifdef DEVELOPER
	override void SetDebugItem()
	{
		super.SetDebugItem();
		_itemBase = this;
	}
	
	override string GetDebugText()
	{
		string text = super.GetDebugText();
		
		text += string.Format("Heat isolation(raw): %1\n", GetHeatIsolation());
		text += string.Format("Heat isolation(modified): %1\n", MiscGameplayFunctions.GetCurrentItemHeatIsolation(this));

		return text;
	}
	#endif
	
	bool CanBeUsedForSuicide()
	{
		return true;
	}
	
	///////////////////
	//DEPRECATED BELOW
	//////////////////
	// Backwards compatibility
	void ProcessItemWetnessAndTemperature(float delta, bool hasParent, bool hasRootAsPlayer, ItemBase refParentIB)
	{
		ProcessItemWetness(delta, hasParent, hasRootAsPlayer, refParentIB);
		ProcessItemTemperature(delta, hasParent, hasRootAsPlayer, refParentIB);
	}
	
	// replaced by ItemSoundHandler
	protected EffectSound m_SoundDeployFinish;
	protected EffectSound m_SoundPlace;
	protected EffectSound m_DeployLoopSoundEx;
	protected EffectSound m_SoundDeploy;
	bool m_IsPlaceSound;
	bool m_IsDeploySound;
	bool m_IsSoundSynchRemote;
	
	string GetDeployFinishSoundset();
	void PlayDeploySound();
	void PlayDeployFinishSound();
	void PlayPlaceSound();
	void PlayDeployLoopSoundEx();
	void StopDeployLoopSoundEx();
	void SoundSynchRemoteReset();
	void SoundSynchRemote();
	bool UsesGlobalDeploy(){return false;}
	bool CanPlayDeployLoopSound(){return false;}
	bool IsSoundSynchRemote(){return m_IsSoundSynchRemote;}
	bool IsPlaceSound(){return m_IsPlaceSound;}
	bool IsDeploySound(){return m_IsDeploySound;}
	void SetIsPlaceSound(bool is_place_sound);
	void SetIsDeploySound(bool is_deploy_sound);
}

EntityAI SpawnItemOnLocation(string object_name, notnull InventoryLocation loc, bool full_quantity)
{
	EntityAI entity = SpawnEntity(object_name, loc, ECE_IN_INVENTORY, RF_DEFAULT);
	if (entity)
	{
		bool is_item = entity.IsInherited(ItemBase);
		if (is_item && full_quantity)
		{
			ItemBase item = ItemBase.Cast(entity);
			item.SetQuantity(item.GetQuantityInit());
		}
	}
	else
	{
		ErrorEx("Cannot spawn entity: " + object_name,ErrorExSeverity.INFO);
		return NULL;
	}
	return entity;
}

void SetupSpawnedItem(ItemBase item, float health, float quantity)
{
	if (item)
	{
		if (health > 0)
			item.SetHealth("", "", health);
		
		if (item.CanHaveTemperature())
		{
			item.SetTemperatureDirect(GameConstants.ITEM_TEMPERATURE_NEUTRAL_ZONE_MIDDLE);
			if (item.CanFreeze())
				item.SetFrozen(false);
		}
		
		if (item.HasEnergyManager())
		{
			if (quantity >= 0)
			{		
				item.GetCompEM().SetEnergy0To1(quantity);
			}
			else
			{
				item.GetCompEM().SetEnergy(Math.AbsFloat(quantity));
			}
		}
		else if (item.IsMagazine())
		{
			Magazine mag = Magazine.Cast(item);
			if (quantity >= 0)
			{		
				mag.ServerSetAmmoCount(mag.GetAmmoMax() * quantity);
			}
			else
			{		
				mag.ServerSetAmmoCount(Math.AbsFloat(quantity));
			}
			
		}
		else
		{
			if (quantity >= 0)
			{		
				item.SetQuantityNormalized(quantity, false);
			}
			else
			{
				item.SetQuantity(Math.AbsFloat(quantity));
			}
			
		}
	}
}

#ifdef DEVELOPER
ItemBase _itemBase;//watched item goes here(LCTRL+RMB->Watch)
#endif
