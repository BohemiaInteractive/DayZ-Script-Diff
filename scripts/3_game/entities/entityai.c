enum EWetnessLevel
{
	DRY,
	DAMP,
	WET,
	SOAKING,
	DRENCHED
}

enum SurfaceAnimationBone
{
	LeftFrontLimb = 0,
	RightFrontLimb,
	LeftBackLimb,
	RightBackLimb
}

enum PlantType
{
	TREE_HARD		= 1000,
	TREE_SOFT		= 1001,
	BUSH_HARD		= 1002,	
	BUSH_SOFT		= 1003,
}

enum WeightUpdateType
{
	FULL = 0,
	ADD,
	REMOVE,
	RECURSIVE_ADD,
	RECURSIVE_REMOVE
}

enum EItemManipulationContext
{
	UPDATE, //generic operation
	ATTACHING,
	DETACHING,
}

//! icon visibility, meant to be used in a bitmask
enum EInventoryIconVisibility
{
	ALWAYS = 0,
	HIDE_VICINITY = 1,
	//further values yet unused, but nice to have anyway
	HIDE_PLAYER_CONTAINER = 2,
	HIDE_HANDS_SLOT = 4
}

//!EXCLUSIVITY values, restrict attachment combinations
enum EAttExclusions
{
	OCCUPANCY_INVALID = -1,
	//Legacy relations
	LEGACY_EYEWEAR_HEADGEAR,
	LEGACY_EYEWEAR_MASK,
	LEGACY_HEADSTRAP_HEADGEAR,
	LEGACY_HEADSTRAP_MASK,
	LEGACY_HEADGEAR_MASK,
	LEGACY_HEADGEAR_EYEWEWEAR,
	LEGACY_HEADGEAR_HEADSTRAP,
	LEGACY_MASK_HEADGEAR,
	LEGACY_MASK_EYEWEWEAR,
	LEGACY_MASK_HEADSTRAP,
	//
	EXCLUSION_HEADGEAR_HELMET_0, //full helmet
	//EXCLUSION_HEADGEAR_HELMET_0_A, //example of another 'vector' of potential conflict, like between helmet and eyewear..otherwise the other non-helmet entities would collide through the 'EXCLUSION_HEADSTRAP_0' value.
	EXCLUSION_HEADSTRAP_0,
	EXCLUSION_MASK_0,
	EXCLUSION_MASK_1,
	EXCLUSION_MASK_2, //Mostly Gasmasks
	EXCLUSION_MASK_3, //bandana mask special behavior
	EXCLUSION_GLASSES_REGULAR_0,
	EXCLUSION_GLASSES_TIGHT_0,
	//values to solve the edge-cases with shaving action
	SHAVING_MASK_ATT_0,
	SHAVING_HEADGEAR_ATT_0,
	SHAVING_EYEWEAR_ATT_0,
}

class DebugSpawnParams
{
	Man m_Player;

	static DebugSpawnParams WithPlayer(Man player)
	{
		DebugSpawnParams params = new DebugSpawnParams();
		params.m_Player = player;
		return params;
	}
};

class TSelectableActionInfoArrayEx extends array<ref Param> {}
typedef Param3<int, int, string> TSelectableActionInfo;
typedef Param4<int, int, string, int> TSelectableActionInfoWithColor;

class EntityAI extends Entity
{
	bool 								m_DeathSyncSent;
	bool 								m_KilledByHeadshot;
	bool 								m_PreparedToDelete = false;
	bool 								m_RefresherViable = false;
	bool								m_WeightDirty = 1;
	protected bool 						m_RoofAbove = false;
	private ref map<int,ref set<int>>	m_AttachmentExclusionSlotMap; //own masks for different slots <slot,mask>. Kept on instance to better respond to various state changes
	private ref set<int>				m_AttachmentExclusionMaskGlobal; //additional mask values and simple item values. Independent of slot-specific behavior!
	private ref set<int> 				m_AttachmentExclusionMaskChildren; //additional mask values and simple item values
	
	ref DestructionEffectBase			m_DestructionBehaviourObj;
	
	ref KillerData 						m_KillerData;
	private ref HiddenSelectionsData	m_HiddenSelectionsData;
	
	const int DEAD_REPLACE_DELAY		= 250;
	const int DELETE_CHECK_DELAY		= 100;
	
	ref array<EntityAI> 			m_AttachmentsWithCargo;
	ref array<EntityAI> 			m_AttachmentsWithAttachments;
	ref InventoryLocation 			m_OldLocation;
	
	protected ref DamageZoneMap 	m_DamageZoneMap;
	private ref map<int, string> 	m_DamageDisplayNameMap = new map<int, string>; //values are localization keys as strings, use 'Widget.TranslateString' method to get the localized one
	
	float							m_Weight;
	float							m_WeightEx;
	float 							m_ConfigWeight = ConfigGetInt("weight");
	protected bool 					m_CanDisplayWeight;
	private float 					m_LastUpdatedTime; //CE update time
	protected float					m_ElapsedSinceLastUpdate; //CE update time
	protected float 				m_PreviousRoofTestTime = 0;
	
	protected UTemperatureSource m_UniversalTemperatureSource;
	
	bool m_PendingDelete = false;
	bool m_Initialized = false;
	bool m_TransportHitRegistered = false;
	vector m_TransportHitVelocity;
	
	// ============================================
	// Variable Manipulation System
	// ============================================
	int		m_VariablesMask;//this holds information about which vars have been changed from their default values
	
	// Temperature
	float m_VarTemperature;
	float m_VarTemperatureInit;
	float m_VarTemperatureMin;
	float m_VarTemperatureMax;
	float m_VarTemperatureFreezeThreshold;
	float m_VarTemperatureThawThreshold;
	float m_VarTemperatureFreezeTime;
	float m_VarTemperatureThawTime;
	float m_VarTemperatureOverheatTime;
	float m_VarHeatPermeabilityCoef;
	
	protected ref TemperatureAccessComponent m_TAC;
	
	protected bool 		m_IsFrozen;
	protected bool 		m_IsFrozenLocal;
	protected float 	m_FreezeThawProgress;
	
	protected float 	m_OverheatProgress;
	
	//---------------------------------------------
	
	//Called on item attached to this item (EntityAI item, string slot, EntityAI parent)
	protected ref ScriptInvoker		m_OnItemAttached;
	//Called on item detached from this item (EntityAI item, string slot, EntityAI parent)
	protected ref ScriptInvoker		m_OnItemDetached;
	//Called when an item is added to the cargo of this item (EntityAI item, EntityAI parent)
	protected ref ScriptInvoker		m_OnItemAddedIntoCargo;
	//Called when an item is removed from the cargo of this item (EntityAI item, EntityAI parent)
	protected ref ScriptInvoker		m_OnItemRemovedFromCargo;
	//Called when an item is moved around in the cargo of this item (EntityAI item, EntityAI parent)
	protected ref ScriptInvoker		m_OnItemMovedInCargo;
	//Called when an item is flipped around in cargo (bool flip)
	protected ref ScriptInvoker		m_OnItemFlipped;
	//Called when an items view index is changed 
	protected ref ScriptInvoker		m_OnViewIndexChanged;
	//Called when an location in this item is reserved (EntityAI item) - cargo
	protected ref ScriptInvoker		m_OnSetLock;
	//Called when this item is unreserved (EntityAI item) - cargo
	protected ref ScriptInvoker		m_OnReleaseLock;
	//Called when an location in this item is reserved (EntityAI item) - attachment
	protected ref ScriptInvoker		m_OnAttachmentSetLock;
	//Called when this item is unreserved (EntityAI item) - attachment
	protected ref ScriptInvoker		m_OnAttachmentReleaseLock;	
	//Called when this entity is hit
	protected ref ScriptInvoker		m_OnHitByInvoker;	
	//Called when this entity is killed
	protected ref ScriptInvoker		m_OnKilledInvoker;

	private ref map<eAgents, float> m_BloodInfectionChanceCached; //! cache blood infection chance (cfgVehicles-><entity>->Skinning->BloodInfectionSettings)
	
	#ifdef DEVELOPER
	float m_LastFTChangeTime;;
	float m_PresumedTimeRemaining;
	#endif
	
	void EntityAI()
	{
		// Set up the Energy Manager
		string type = GetType();
		string param_access_energy_sys = "CfgVehicles " + type + " EnergyManager ";
		bool is_electic_device = GetGame().ConfigIsExisting(param_access_energy_sys);

		if (is_electic_device) // TO DO: Check if this instance is a hologram (advanced placement). If Yes, then do not create Energy Manager component.
		{
			CreateComponent(COMP_TYPE_ENERGY_MANAGER);
			RegisterNetSyncVariableBool("m_EM.m_IsSwichedOn");
			RegisterNetSyncVariableBool("m_EM.m_CanWork");
			RegisterNetSyncVariableBool("m_EM.m_IsPlugged");
			RegisterNetSyncVariableInt("m_EM.m_EnergySourceNetworkIDLow");
			RegisterNetSyncVariableInt("m_EM.m_EnergySourceNetworkIDHigh");
			RegisterNetSyncVariableFloat("m_EM.m_Energy");
		}
		
		// Item preview index
		RegisterNetSyncVariableInt( "m_ViewIndex", 0, 99 );
		// Refresher signalization
		RegisterNetSyncVariableBool("m_RefresherViable");
		
		m_AttachmentsWithCargo			= new array<EntityAI>();
		m_AttachmentsWithAttachments	= new array<EntityAI>();
		m_LastUpdatedTime = 0.0;
		m_ElapsedSinceLastUpdate = 0.0;
		
		m_CanDisplayWeight = ConfigGetBool("displayWeight");
		
		InitDamageZoneMapping();
		InitDamageZoneDisplayNameMapping();
		InitItemVariables();
		
		m_HiddenSelectionsData = new HiddenSelectionsData( GetType() );
		
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(DeferredInit,34);

		m_BloodInfectionChanceCached = new map<eAgents, float>();
	}
	
	void ~EntityAI()
	{
		
	}
	
	void InitItemVariables()
	{
		m_VarTemperatureInit = ConfigGetFloat("varTemperatureInit");
		m_VarTemperatureMin = ConfigGetFloat("varTemperatureMin");
		m_VarTemperatureMax = ConfigGetFloat("varTemperatureMax");
		
		if (ConfigIsExisting("varTemperatureFreezePoint"))
			m_VarTemperatureFreezeThreshold = ConfigGetFloat("varTemperatureFreezePoint");
		else
			m_VarTemperatureFreezeThreshold = float.LOWEST;
		
		if (ConfigIsExisting("varTemperatureThawPoint"))
			m_VarTemperatureThawThreshold = ConfigGetFloat("varTemperatureThawPoint");
		else
			m_VarTemperatureThawThreshold = float.LOWEST;
		
		m_VarTemperatureFreezeTime = Math.Clamp(ConfigGetFloat("varTemperatureFreezeTime"),1,float.MAX);
		m_VarTemperatureThawTime = Math.Clamp(ConfigGetFloat("varTemperatureThawTime"),1,float.MAX);
		if (ConfigIsExisting("varTemperatureOverheatTime"))
			m_VarTemperatureOverheatTime = ConfigGetFloat("varTemperatureOverheatTime");
		else
			m_VarTemperatureOverheatTime = -1;
		
		if (ConfigIsExisting("varHeatPermeabilityCoef"))
			m_VarHeatPermeabilityCoef = ConfigGetFloat("varHeatPermeabilityCoef");
		else
			m_VarHeatPermeabilityCoef = 1;
		
		if (CanHaveTemperature())
		{
			RegisterNetSyncVariableFloat("m_VarTemperature", GetTemperatureMin(),GetTemperatureMax());
			RegisterNetSyncVariableBool("m_IsFrozen");
			
			if (GetGame().IsServer())
				m_TAC = new TemperatureAccessComponent(this);
			
			if (!GetGame().IsMultiplayer() || GetGame().IsClient())
				m_FreezeThawProgress = -1;
		}
	}
	
	void DeferredInit()
	{
		m_Initialized = true;
	}
	
	bool IsInitialized()
	{
		return m_Initialized;
	}
	
	//! should the item's icon be hidden in any part of the inventory?
	int GetHideIconMask()
	{
		return EInventoryIconVisibility.ALWAYS;
	}

	private ref ComponentsBank m_ComponentsBank;
	ComponentEnergyManager m_EM; // This reference is necesarry due to synchronization, since it's impossible to synchronize values from a component :(

	//! CreateComponent
	Component CreateComponent(int comp_type, string extended_class_name="")
	{
		return GetComponent(comp_type, extended_class_name);
	}

	//! GetComponent
	Component GetComponent(int comp_type, string extended_class_name="")
	{
		if ( m_ComponentsBank == NULL )
			m_ComponentsBank = new ComponentsBank(this);
		
		return m_ComponentsBank.GetComponent(comp_type, extended_class_name);
	}

	//! DeleteComponent
	bool DeleteComponent(int comp_type)
	{
		return m_ComponentsBank.DeleteComponent(comp_type);
	}
	
	string GetDestructionBehaviour()
	{
		return "";
	}
	
	bool IsDestructionBehaviour()
	{
		return false;
	}
	
	//! IsComponentExist
	bool HasComponent(int comp_type)
	{
		if ( m_ComponentsBank )
			return m_ComponentsBank.IsComponentAlreadyExist(comp_type);
		
		return false;
	}
	
	//! Calculates if the max lifetime is higher than refresher frequency (i.e. gets kept alive by refresher) 
	void MaxLifetimeRefreshCalc()
	{
		if ( (!GetGame().IsMultiplayer() || GetGame().IsServer()) && GetEconomyProfile() )
		{
			float lifetime = GetEconomyProfile().GetLifetime();
			int frequency = GetCEApi().GetCEGlobalInt("FlagRefreshFrequency");
			if ( frequency <= 0 )
			{
				frequency = GameConstants.REFRESHER_FREQUENCY_DEFAULT;
			}
			
			if ( frequency <= lifetime )
			{
				m_RefresherViable = true;
				SetSynchDirty();
			}
		}
	}
	
	bool IsRefresherSignalingViable()
	{
		if (IsRuined())
		{
			return false;
		}
		return m_RefresherViable;
	}
	
	#ifdef DEVELOPER
	override void SetDebugItem()
	{
		super.SetDebugItem();
		_item = this;
	}
	#endif
	
	
	//! Initializes script-side map of damage zones and their components (named selections in models)
	void InitDamageZoneMapping()
	{
		m_DamageZoneMap = new DamageZoneMap;
		DamageSystem.GetDamageZoneMap(this,m_DamageZoneMap);
	}
	
	//! Initialize map of damage zone display names for more optimized retrieval
	void InitDamageZoneDisplayNameMapping()
	{	
		string path_base;
		string path;
		string component_name;
		
		if ( IsWeapon() )
		{
			path_base = CFG_WEAPONSPATH;
		}
		else if ( IsMagazine() )
		{
			path_base = CFG_MAGAZINESPATH;
		}
		else
		{
			path_base = CFG_VEHICLESPATH;
		}
		
		path_base = string.Format( "%1 %2 DamageSystem DamageZones", path_base, GetType() );
		
		if ( !GetGame().ConfigIsExisting(path_base) )
		{
			component_name = GetDisplayName();
			GetGame().FormatRawConfigStringKeys(component_name);
			m_DamageDisplayNameMap.Insert( "".Hash(), component_name );
		}
		else
		{
			TStringArray zone_names = new TStringArray;
			GetDamageZones( zone_names );
			
			for ( int i = 0; i < zone_names.Count(); i++ )
			{
				path = string.Format( "%1 %2 displayName", path_base, zone_names[i] );
				
				if (GetGame().ConfigIsExisting(path) && GetGame().ConfigGetTextRaw(path,component_name))
				{
					GetGame().FormatRawConfigStringKeys(component_name);
					m_DamageDisplayNameMap.Insert( zone_names[i].Hash(), component_name );
				}
			}
		}
	}
	
	protected float ConvertNonlethalDamage(float damage, DamageType damageType)
	{
		return 0.0;
	}
	
	//! DEPRECATED - for legacy purposes
	float ConvertNonlethalDamage(float damage)
	{
		return 0.0;
	}
	
	DamageZoneMap GetEntityDamageZoneMap()
	{
		return m_DamageZoneMap;
	}
	
	map<int, string> GetEntityDamageDisplayNameMap()
	{
		return m_DamageDisplayNameMap;
	}
	
	//! 'displayWeight' in item config
	bool CanDisplayWeight()
	{
		return m_CanDisplayWeight;
	}

	//! Log
	void Log(string msg, string fnc_name = "n/a")
	{
		Debug.Log(msg, "Object", "n/a", fnc_name, this.GetType());
	}

	//! LogWarning
	void LogWarning(string msg, string fnc_name = "n/a")
	{
		Debug.LogWarning(msg, "Object", "n/a", fnc_name, this.GetType());
	}

	//! LogError
	void LogError(string msg, string fnc_name = "n/a")
	{
		Debug.LogError(msg, "Object", "n/a", fnc_name, this.GetType());
	}

	///@{ Skinning
	bool IsSkinned()
	{
		return GetCompBS() && GetCompBS().IsSkinned();
	}

	void SetAsSkinned()
	{
		if (GetCompBS())
			GetCompBS().SetAsSkinned();
	}

	bool CanBeSkinnedWith(EntityAI tool)
	{
		if ( !IsSkinned()  &&  tool )
			if ( !IsAlive() )
				return true;
		return false;
	}
	
	float GetSkinningBloodInfectionChance(eAgents type)
	{
		float value = 0.0;
		if (m_BloodInfectionChanceCached.Find(type, value))
			return value;
		
		return value;
	}
	
	protected void CacheSkinningBloodInfectionChance(eAgents type)
	{
		string basePath = string.Format("cfgVehicles %1 Skinning BloodInfectionSettings", GetType());
		if (g_Game.ConfigIsExisting(basePath))
		{
			string agentName = EnumTools.EnumToString(eAgents, type);
			agentName.ToLower();
			float value = g_Game.ConfigGetFloat(string.Format("%1 %2 chance", basePath, agentName));
			m_BloodInfectionChanceCached.Set(type, value);
		}
	}
	///@} Skinning

	// ITEM TO ITEM FIRE DISTRIBUTION
	//! Override this method to return TRUE when this item has or can provide fire. Evaluated on server and client.
	bool HasFlammableMaterial()
	{
		return false;
	}
	
	//! Override this method so it checks whenever this item can be ignited right now or not. Evaluated on Server and Client.
	bool CanBeIgnitedBy(EntityAI igniter = NULL)
	{
		return false;
	}
	
	//! Override this method and check if the given item can be ignited right now by this one. Evaluated on Server and Client.
	bool CanIgniteItem(EntityAI ignite_target = NULL)
	{
		return false;
	}
	
	//! Override this method and make it so it returns whenever this item is on fire right now or not. Evaluated on Server and Client.
	bool IsIgnited()
	{
		if (m_EM)
			return m_EM.IsWorking();
		return false;
	}
	
	// Change return value to true if last detached item cause disassemble of item - different handlig some inventory operations
	bool DisassembleOnLastDetach()
	{
		return false;
	}
	
	bool IsBasebuildingKit()
	{
		return false;
	}
	
	bool IsCookware()
	{
		return false;
	}
	
	//! Should return false if you want to disable hologram rotation
	bool PlacementCanBeRotated()
	{
		return true;
	}
	
	//! Executed on Server when this item ignites some target item
	void OnIgnitedTarget( EntityAI target_item)
	{
		
	}
	
	//! Executed on Server when some item ignited this one
	void OnIgnitedThis( EntityAI fire_source)
	{
		
	}
	
	//! Executed on Server when this item failed to ignite target item
	void OnIgnitedTargetFailed( EntityAI target_item)
	{
		
	}
	
	//! Executed on Server when some item failed to ignite this one
	void OnIgnitedThisFailed( EntityAI fire_source)
	{
		
	}
	
	//! Final evaluation just before the target item is actually ignited. Evaluated on Server.
	bool IsTargetIgnitionSuccessful(EntityAI item_target)
	{
		return true;
	}
	
	//! Final evaluation just before this item is actually ignited from fire source. Evaluated on Server.
	bool IsThisIgnitionSuccessful(EntityAI item_source = NULL)
	{
		return true;
	}
	// End of fire distribution ^
	
	// ADVANCED PLACEMENT EVENTS
	void OnPlacementStarted(Man player);		
	void OnHologramBeingPlaced(Man player);
	void OnPlacementComplete(Man player, vector position = "0 0 0", vector orientation = "0 0 0");
	void OnPlacementCancelled(Man player);
	
	bool CanBePlaced(Man player, vector position)
	{
		return true;
	}

	//! Method which returns message why object can't be placed at given position
	string CanBePlacedFailMessage( Man player, vector position )
	{
		return "";
	}
	
	//! Do the roof check when placing this?
	bool DoPlacingHeightCheck()
	{
		return false;
	}
	
	//! used as script-side override of distance for specific height checks
	float HeightCheckOverride()
	{
		return 0.0;
	}
	
	//! used as script-side override of start pos for specific height checks
	float HeightStartCheckOverride()
	{
		return 0.0;
	}
	
	//! is this container empty or not, checks both cargo and attachments
	bool IsEmpty()
	{
		return (!HasAnyCargo() && GetInventory().AttachmentCount() == 0);
	}
	
	//! returns just the configured 'canBeSplit' bool
	bool IsSplitable()
	{
		return false;
	}
	
	//! returns current splitability
	bool CanBeSplit()
	{
		return false;
	}

	//! is this container empty or not, checks only cargo
	bool HasAnyCargo()
	{
		CargoBase cargo = GetInventory().GetCargo();
		
		if(!cargo) return false;//this is not a cargo container
		
		if( cargo.GetItemCount() > 0 )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	
	array<EntityAI> GetAttachmentsWithCargo()
	{
		return m_AttachmentsWithCargo;
	}
	
	array<EntityAI> GetAttachmentsWithAttachments()
	{
		return m_AttachmentsWithAttachments;
	}
	
	//is there any roof above
	bool IsRoofAbove()
	{
		return m_RoofAbove;
	}
	
	void SetRoofAbove(bool state)
	{
		m_RoofAbove = state;
	}
	
	//! Roof check for entity, limited by time (anti-spam solution)
	void CheckForRoofLimited(float timeTresholdMS = 3000);

	int GetAgents() { return 0; }
	void RemoveAgent(int agent_id);
	void RemoveAllAgents();
	void RemoveAllAgentsExcept(int agent_to_keep);
	void InsertAgent(int agent, float count = 1);

	override bool IsEntityAI() { return true; }
	
	bool IsInventoryVisible()
	{
		return !( GetParent() || GetHierarchyParent() );
	}
	
	bool IsPlayer()
	{
		return false;
	}
	
	bool IsAnimal()
	{
		return false;
	}
	
	bool IsZombie()
	{
		return false;
	}
	
	bool IsZombieMilitary()
	{
		return false;
	}
	
	bool IsIgnoredByConstruction()
	{
		return IsDamageDestroyed();
	}
	
	bool CanBeTargetedByAI(EntityAI ai)
	{
		if (ai && ai.IsBeingBackstabbed())
		{
			return false;
		}
		
		if ( !dBodyIsActive( this ) && !IsMan() )
			return false;
		return !IsDamageDestroyed();
	}
	
	bool CanBeBackstabbed()
	{
		return false;
	}
	
	/**@brief Delete this object in next frame
	 * @return \p void
	 *
	 * @code
	 *			ItemBase item = GetGame().GetPlayer().CreateInInventory("GrenadeRGD5");
	 *			item.Delete();
	 * @endcode
	**/
	override void Delete()
	{
		m_PendingDelete = true;
		super.Delete();
	}
	
	void DeleteOnClient()
	{
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Call(GetGame().ObjectDeleteOnClient, this);
	}
	
	// delete synchronized between server and client
	void DeleteSafe()
	{
		if (GetHierarchyRootPlayer() == null || (GetHierarchyRootPlayer() && !GetHierarchyRootPlayer().IsAlive()))
		{
			Delete();
		}
		else
		{
			if (GetGame().IsServer() && GetGame().IsMultiplayer())
				GetHierarchyRootPlayer().JunctureDeleteItem(this);
			else
				GetHierarchyRootPlayer().AddItemToDelete(this);
		}
	}
	
	//legacy, wrong name, use 'DeleteSafe()' instead
	void DeleteSave()
	{		
		DeleteSafe();
	}
	
	bool IsSetForDeletion()
	{
		return IsPreparedToDelete() || m_PendingDelete || ToDelete() || IsPendingDeletion();
	}
	
	override bool CanBeActionTarget()
	{
		if (super.CanBeActionTarget())
		{
			return !IsSetForDeletion();
		}
		else
		{
			return false;
		}
	}
	
	void SetPrepareToDelete()
	{
		m_PreparedToDelete = true;
	}
	
	bool IsPreparedToDelete()
	{
		return m_PreparedToDelete;
	}
	
	
	void CheckForDestroy()
	{
		if (IsPrepareToDelete())
		{
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TryDelete, DELETE_CHECK_DELAY, false);
		}
	}
	
	bool IsPrepareToDelete()
	{
		return false;
	}
	
	bool TryDelete()
	{
		if (!IsPrepareToDelete())
		{
			Debug.Log("TryDelete - not ready for deletion");
			return false;
		}

		if (GetGame().HasInventoryJunctureItem(this))
		{
			Debug.Log("TryDelete - deferred call");
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TryDelete, DELETE_CHECK_DELAY, false);
			return false;
		}

		OnBeforeTryDelete();
		Debug.Log("TryDelete - OnBeforeTryDelete end");
		DeleteSafe();
		Debug.Log("TryDelete - DeleteSafe end");

		return true;
	}
	
	void OnBeforeTryDelete();
	
	//! Returns root of current hierarchy (for example: if this entity is in Backpack on gnd, returns Backpack)
	proto native EntityAI GetHierarchyRoot();

	//! Returns root of current hierarchy cast to Man
	proto native Man GetHierarchyRootPlayer();
	
	//! Returns direct parent of current entity
	proto native EntityAI GetHierarchyParent();

	//! Get economy item profile (if assigned, otherwise null)
	proto native CEItemProfile GetEconomyProfile();

	// !returns the number of levels bellow the hierarchy root this entity is at
	int GetHierarchyLevel(int lvl = 0)
	{
		if (!GetHierarchyParent())
			return lvl;

		return GetHierarchyParent().GetHierarchyLevel(lvl+1);
	}
	
	void OnInventoryInit()
	{
		InitAttachmentExclusionValues();
	}
	
	//! Called upon object creation
	void EEInit()
	{
		if (GetInventory())
		{
			GetInventory().EEInit();
			m_AttachmentsWithCargo.Clear();
			m_AttachmentsWithAttachments.Clear();
			for ( int i = 0; i < GetInventory().AttachmentCount(); i++ )
			{
				EntityAI attachment = GetInventory().GetAttachmentFromIndex( i );
				if ( attachment )
				{
					if ( attachment.GetInventory().GetCargo() )
					{
						m_AttachmentsWithCargo.Insert( attachment );
					}
					
					if ( attachment.GetInventory().GetAttachmentSlotsCount() > 0 )
					{
						m_AttachmentsWithAttachments.Insert( attachment );
					}
				}
			}
		}
		
		MaxLifetimeRefreshCalc();
		
		if (CanHaveTemperature() && GetGame().IsServer())
			InitTemperature();
	}
	
	//! Called right before object deleting
	void EEDelete(EntityAI parent)
	{
		m_PendingDelete = true;
		GetInventory().EEDelete(parent);
		
		if (m_EM)
			m_EM.OnDeviceDestroyed();
	}
	
	override void OnExplosionEffects(Object source, Object directHit, int componentIndex, string surface, vector pos, vector surfNormal, float energyFactor, float explosionFactor, bool isWater, string ammoType) 
	{
		super.OnExplosionEffects(source, directHit, componentIndex, surface, pos, surfNormal, energyFactor, explosionFactor, isWater, ammoType);
		#ifndef SERVER
		g_Game.GetWorld().AddEnvShootingSource(pos, 1.0);
		#endif
		if (m_DestructionBehaviourObj && m_DestructionBehaviourObj.HasExplosionDamage())
		{
			m_DestructionBehaviourObj.OnExplosionEffects(source, directHit, componentIndex, surface, pos, surfNormal, energyFactor, explosionFactor, isWater, ammoType);
		}
	}
	
	
	void OnItemLocationChanged(EntityAI old_owner, EntityAI new_owner) { }
	void OnChildItemRemoved(InventoryItem item) { }
	void OnChildItemReceived(InventoryItem item) { }
	
	void OnItemAttachmentSlotChanged (notnull InventoryLocation oldLoc, notnull InventoryLocation newLoc) {}
	
	void EEItemLocationChanged (notnull InventoryLocation oldLoc, notnull InventoryLocation newLoc)
	{
		EntityAI old_owner = oldLoc.GetParent();
		EntityAI new_owner = newLoc.GetParent();
		OnItemLocationChanged(old_owner, new_owner);
		
		if (oldLoc.GetType() == InventoryLocationType.ATTACHMENT && newLoc.GetType() == InventoryLocationType.ATTACHMENT)
		{
			OnItemAttachmentSlotChanged(oldLoc,newLoc);
		}
		
		if (oldLoc.GetType() == InventoryLocationType.ATTACHMENT)
		{
			if (old_owner)
				OnWasDetached(old_owner, oldLoc.GetSlot());
			else
				Error("EntityAI::EEItemLocationChanged - detached, but old_owner is null");
		}
		
		if (newLoc.GetType() == InventoryLocationType.ATTACHMENT)
		{
			if (new_owner)
				OnWasAttached(newLoc.GetParent(), newLoc.GetSlot());
			else
				Error("EntityAI::EEItemLocationChanged - attached, but new_owner is null");
		}
		
		if (oldLoc.GetType() == InventoryLocationType.HANDS)
		{
			Man.Cast(oldLoc.GetParent()).OnItemInHandsChanged();
		}
		
		if (newLoc.GetType() == InventoryLocationType.HANDS)
		{
			Man.Cast(newLoc.GetParent()).OnItemInHandsChanged();
		}
	}

	//! Called from 'IEntity.AddChild'
	void EEParentedTo(EntityAI parent)
	{
	}

	//! Called from 'IEntity.RemoveChild' or 'IEntity.AddChild' when hierarchy changes
	void EEParentedFrom(EntityAI parent)
	{
	}
	
	void EEInventoryIn (Man newParentMan, EntityAI diz, EntityAI newParent)
	{
	}
	void EEInventoryOut (Man oldParentMan, EntityAI diz, EntityAI newParent)
	{
		m_LastUpdatedTime = GetGame().GetTickTime();
		
		if (GetInventory() && newParent == null)
		{
			GetInventory().ResetFlipCargo();
		}
	}

	void EEAmmoChanged()
	{
		SetWeightDirty();
	}

	void EEHealthLevelChanged(int oldLevel, int newLevel, string zone)
	{
		// Notify potential parent that this item was ruined
		EntityAI parent = GetHierarchyParent();
		
		if (newLevel == GameConstants.STATE_RUINED)
		{
			if (parent)
			{
				parent.OnAttachmentRuined(this);
			}
			if (!zone)
			{
				OnDamageDestroyed(oldLevel);
			}
			AttemptDestructionBehaviour(oldLevel,newLevel, zone);
		}
	}
	
	//! Called when the health gets to the min value, 'oldLevel' is previous health level, 'oldLevel' -1 means this entity was just spawned
	void OnDamageDestroyed(int oldLevel);
	
	void AttemptDestructionBehaviour(int oldLevel, int newLevel, string zone) 
	{
		if (IsDestructionBehaviour() && GetDestructionBehaviour())
		{
			typename destType = GetDestructionBehaviour().ToType();
			
			if (destType)
			{
				if (!m_DestructionBehaviourObj)
				{
					m_DestructionBehaviourObj = DestructionEffectBase.Cast(destType.Spawn());
				}
				
				if (m_DestructionBehaviourObj)
				{
					m_DestructionBehaviourObj.OnHealthLevelChanged(this, oldLevel, newLevel, zone);
				}
			}
			else
			{
				ErrorEx("Incorrect destruction behaviour type, make sure the class returned in 'GetDestructionBehaviour()' is a valid type inheriting from 'DestructionEffectBase'");
			}
		}
	}
	
	
	void SetTakeable(bool pState);
	
	//! called on server when the entity is killed
	void EEKilled(Object killer)
	{
		if (m_OnKilledInvoker)
			m_OnKilledInvoker.Invoke(this, killer);

		GetGame().GetAnalyticsServer().OnEntityKilled(killer, this);
		
		if (ReplaceOnDeath())
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(DeathUpdate, DEAD_REPLACE_DELAY, false);
	}
	
	bool ReplaceOnDeath()
	{
		return false;
	}
	
	string GetDeadItemName()
	{
		return "";
	}
	
	bool KeepHealthOnReplace()
	{
		return false;
	}
	
	void DeathUpdate()
	{
		EntityAI dead_entity = EntityAI.Cast( GetGame().CreateObjectEx( GetDeadItemName(), GetPosition(), ECE_OBJECT_SWAP, RF_ORIGINAL ) );
		dead_entity.SetOrientation(GetOrientation());
		if (KeepHealthOnReplace())
			dead_entity.SetHealth(GetHealth());

		DeleteSafe();
	}
	
	//! Called when some attachment of this parent is ruined. Called on server and client side.
	void OnAttachmentRuined(EntityAI attachment);
	
	void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		if (m_OnHitByInvoker)
			m_OnHitByInvoker.Invoke(this, damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
		#ifdef DEVELOPER
		//Print("EEHitBy: " + this + "; damageResult:"+ damageResult.GetDamage("","") +"; damageType: "+ damageType +"; source: "+ source +"; component: "+ component +"; dmgZone: "+ dmgZone +"; ammo: "+ ammo +"; modelPos: "+ modelPos);
		#endif
	}
	
	// called only on the client who caused the hit
	void EEHitByRemote(int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos)
	{
		
	}
	
	// !Called on PARENT when a child is attached to it.
	void EEItemAttached(EntityAI item, string slot_name)
	{
		int slotId = InventorySlots.GetSlotIdFromString(slot_name);
		PropagateExclusionValueRecursive(item.GetAttachmentExclusionMaskAll(slotId),slotId); //Performed from parent to avoid event order issues on swap
		SetWeightDirty();

		if ( m_ComponentsBank != NULL )
		{
			for ( int comp_key = 0; comp_key < COMP_TYPE_COUNT; ++comp_key )
			{
				if ( m_ComponentsBank.IsComponentAlreadyExist(comp_key) )
				{
					m_ComponentsBank.GetComponent(comp_key).Event_OnItemAttached(item, slot_name);
				}
			}
		}		
		
		// Energy Manager
		if ( m_EM && item.GetCompEM())
			m_EM.OnAttachmentAdded(item);
		
		if ( item.GetInventory().GetCargo() )
			m_AttachmentsWithCargo.Insert( item );
		
		if ( item.GetInventory().GetAttachmentSlotsCount() > 0 )
			m_AttachmentsWithAttachments.Insert( item );
		
		if ( m_OnItemAttached )
			m_OnItemAttached.Invoke( item, slot_name, this );
	}
	
	void SwitchItemSelectionTexture(EntityAI item, string slot_name);
	void SwitchItemSelectionTextureEx(EItemManipulationContext context, Param par = null);
	
	// !Called on PARENT when a child is detached from it.
	void EEItemDetached(EntityAI item, string slot_name)
	{
		int slotId = InventorySlots.GetSlotIdFromString(slot_name);
		ClearExclusionValueRecursive(item.GetAttachmentExclusionMaskAll(slotId),slotId); //Performed from parent to avoid event order issues on swap
		SetWeightDirty();
		
		if ( m_ComponentsBank != NULL )
		{
			for ( int comp_key = 0; comp_key < COMP_TYPE_COUNT; ++comp_key )
			{
				if ( m_ComponentsBank.IsComponentAlreadyExist(comp_key) )
				{
					m_ComponentsBank.GetComponent(comp_key).Event_OnItemDetached(item, slot_name);
				}
			}
		}
		
		// Energy Manager
		if (m_EM && item.GetCompEM())
			m_EM.OnAttachmentRemoved(item);
		
		if ( m_AttachmentsWithCargo.Find( item ) > -1 )
			m_AttachmentsWithCargo.RemoveItem( item );
		
		if ( m_AttachmentsWithAttachments.Find( item ) > -1 )
			m_AttachmentsWithAttachments.RemoveItem( item );
		
				
		if ( m_OnItemDetached )
			m_OnItemDetached.Invoke( item, slot_name, this );
	}

	void EECargoIn(EntityAI item)
	{
		SetWeightDirty();
		
		if( m_OnItemAddedIntoCargo )
			m_OnItemAddedIntoCargo.Invoke( item, this );
			
		item.OnMovedInsideCargo(this);
	}

	void EECargoOut(EntityAI item)
	{
		SetWeightDirty();
		
		if( m_OnItemRemovedFromCargo )
			m_OnItemRemovedFromCargo.Invoke( item, this );
			
		item.OnRemovedFromCargo(this);
	}

	void EECargoMove(EntityAI item)
	{
		if( m_OnItemMovedInCargo )
			m_OnItemMovedInCargo.Invoke( item, this );
		item.OnMovedWithinCargo(this);
	}
	
	ScriptInvoker GetOnItemAttached()
	{
		if( !m_OnItemAttached )
			m_OnItemAttached = new ScriptInvoker;
		return m_OnItemAttached;
	}
	
	ScriptInvoker GetOnItemDetached()
	{
		if( !m_OnItemDetached )
			m_OnItemDetached = new ScriptInvoker;
		return m_OnItemDetached;
	}
	
	ScriptInvoker GetOnItemAddedIntoCargo()
	{
		if( !m_OnItemAddedIntoCargo )
			m_OnItemAddedIntoCargo = new ScriptInvoker;
		return m_OnItemAddedIntoCargo;
	}
	
	ScriptInvoker GetOnItemRemovedFromCargo()
	{
		if( !m_OnItemRemovedFromCargo )
			m_OnItemRemovedFromCargo = new ScriptInvoker;
		return m_OnItemRemovedFromCargo;
	}
	
	ScriptInvoker GetOnItemMovedInCargo()
	{
		if( !m_OnItemMovedInCargo )
			m_OnItemMovedInCargo = new ScriptInvoker;
		return m_OnItemMovedInCargo;
	}
	
	ScriptInvoker GetOnItemFlipped()
	{
		if( !m_OnItemFlipped )
			m_OnItemFlipped = new ScriptInvoker;
		return m_OnItemFlipped;
	}
	
	ScriptInvoker GetOnViewIndexChanged()
	{
		if( !m_OnViewIndexChanged )
			m_OnViewIndexChanged = new ScriptInvoker;
		return m_OnViewIndexChanged;
	}
	
	ScriptInvoker GetOnSetLock()
	{
		if( !m_OnSetLock )
			m_OnSetLock = new ScriptInvoker;
		return m_OnSetLock;
	}
	
	ScriptInvoker GetOnReleaseLock()
	{
		if( !m_OnReleaseLock )
			m_OnReleaseLock = new ScriptInvoker;
		return m_OnReleaseLock;
	}
	
	ScriptInvoker GetOnAttachmentSetLock()
	{
		if( !m_OnAttachmentSetLock )
			m_OnAttachmentSetLock = new ScriptInvoker;
		return m_OnAttachmentSetLock;
	}
	
	ScriptInvoker GetOnAttachmentReleaseLock()
	{
		if( !m_OnAttachmentReleaseLock )
			m_OnAttachmentReleaseLock = new ScriptInvoker;
		return m_OnAttachmentReleaseLock;
	}
	
	ScriptInvoker GetOnHitByInvoker()
	{
		if ( !m_OnHitByInvoker )
			m_OnHitByInvoker = new ScriptInvoker;
		return m_OnHitByInvoker;
	}
	
	ScriptInvoker GetOnKilledInvoker()
	{
		if ( !m_OnKilledInvoker )
			m_OnKilledInvoker = new ScriptInvoker;
		return m_OnKilledInvoker;
	}
	
	
	//! Called when this item enters cargo of some container
	void OnMovedInsideCargo(EntityAI container)
	{
		if (m_EM)
			m_EM.HandleMoveInsideCargo(container);
	}
	
	//! Called when this item exits cargo of some container
	void OnRemovedFromCargo(EntityAI container)
	{
		
	}
	
	//! Called when this item moves within cargo of some container
	void OnMovedWithinCargo(EntityAI container)
	{
	
	}
	
	//! Called when entity is part of "connected system" and being restored after load
	void EEOnAfterLoad()
	{
		// ENERGY MANAGER
		// Restore connections between devices which were connected before server restart
		if ( m_EM && m_EM.GetRestorePlugState() )
		{
			int b1 = m_EM.GetEnergySourceStorageIDb1();
			int b2 = m_EM.GetEnergySourceStorageIDb2();
			int b3 = m_EM.GetEnergySourceStorageIDb3();
			int b4 = m_EM.GetEnergySourceStorageIDb4();

			// get pointer to EntityAI based on this ID
			EntityAI potential_energy_source = GetGame().GetEntityByPersitentID(b1, b2, b3, b4); // This function is available only in this event!
			
			// IMPORTANT!
			// Object IDs acquired here become INVALID when electric devices are transfered to another server while in plugged state (like Flashlight plugged into its attachment 9V battery)
			// To avoid issues, these items must be excluded from this system of restoring plug state so they don't unintentionally plug to incorrect devices through these invalid IDs.
			// Therefore their plug state is being restored withing the EEItemAttached() event while being excluded by the following 'if' conditions...
			
			bool is_attachment = false;
			
			if (potential_energy_source)
				is_attachment = GetInventory().HasAttachment(potential_energy_source);
			
			if ( !is_attachment	&&	potential_energy_source )
				is_attachment = potential_energy_source.GetInventory().HasAttachment(this);
			
			if ( potential_energy_source && potential_energy_source.GetCompEM() /*&& potential_energy_source.HasEnergyManager()*/ && !is_attachment )
				m_EM.PlugThisInto(potential_energy_source); // restore connection
		}
	}
	
	//! Called when entity is being created as new by CE/ Debug
	void EEOnCECreate()
	{
	}

	//! Called when entity is being loaded from DB or Storage (after all children loaded)
	void AfterStoreLoad()
	{
	}
	
	//! Called when an item fails to get loaded into the inventory of an entity and gets dropped
	void OnBinLoadItemsDropped()
	{
		if (GetHierarchyRootPlayer())
			GetHierarchyRootPlayer().SetProcessUIWarning(true);
	}
	
	//! Sets all animation values to 1, making them INVISIBLE if they are configured in models.cfg in such way. These selections must also be defined in the entity's config class in 'AnimationSources'. 
	void HideAllSelections()
	{
		string cfg_path = "cfgVehicles " + GetType() + " AnimationSources";
		
		if ( GetGame().ConfigIsExisting(cfg_path) )
		{
			int	selections = GetGame().ConfigGetChildrenCount(cfg_path);
			
			for (int i = 0; i < selections; i++)
			{
				string selection_name;
				GetGame().ConfigGetChildName(cfg_path, i, selection_name);
				HideSelection(selection_name);
			}
		}
	}
	
	//! Sets all animation values to 0, making them VISIBLE if they are configured in models.cfg in such way. These selections must also be defined in the entity's config class in 'AnimationSources'. 
	void ShowAllSelections()
	{
		string cfg_path = "cfgVehicles " + GetType() + " AnimationSources";
		
		if ( GetGame().ConfigIsExisting(cfg_path) )
		{
			int	selections = GetGame().ConfigGetChildrenCount(cfg_path);
			
			for (int i = 0; i < selections; i++)
			{
				string selection_name;
				GetGame().ConfigGetChildName(cfg_path, i, selection_name);
				ShowSelection(selection_name);
			}
		}
	}
	
	/**@fn		CanReceiveAttachment
	 * @brief calls this->CanReceiveAttachment(attachment)
	 * @return	true if action allowed
	 *
	 * @note: return scriptConditionExecute(this, attachment, "CanReceiveAttachment");
	 **/
	bool CanReceiveAttachment (EntityAI attachment, int slotId)
	{
		//generic occupancy check
		return CheckAttachmentReceiveExclusion(attachment,slotId);
	}
	
	/**@fn		CanLoadAsAttachment
	 * @brief	calls this->CanLoadAsAttachment(attachment), is called on server start when loading in the storage
	 * @return	true if action allowed
	 *
	 * @note: return scriptConditionExecute(this, attachment, "CanLoadAsAttachment");
	 **/
	bool CanLoadAttachment(EntityAI attachment)
	{
		return true;
	}

	/**@fn		CanPutAsAttachment
	 * @brief	calls this->CanPutAsAttachment(parent)
	 * @param[in] parent	\p	target entity this is trying to attach to
	 * @return	true if action allowed
	 *
	 * @note: engine code is scriptConditionExecute(this, parent, "CanPutAsAttachment")
	 **/
	bool CanPutAsAttachment (EntityAI parent)
	{
		return !IsHologram();
	}

	//If return true, item can be attached even from parent to this. Item will be switched during proccess. (only hands)
	bool CanSwitchDuringAttach(EntityAI parent)
	{
		return false;
	}
	/**@fn		CanReleaseAttachment
	 * @brief calls this->CanReleaseAttachment(attachment)
	 * @return	true if action allowed
	 *
	 * @note: return scriptConditionExecute(this, attachment, "CanReleaseAttachment");
	 **/
	bool CanReleaseAttachment (EntityAI attachment)
	{
		if( attachment && attachment.GetInventory() && GetInventory() )
		{
			InventoryLocation il = new InventoryLocation();
			attachment.GetInventory().GetCurrentInventoryLocation( il );
			if( il.IsValid() )
			{
				int slot = il.GetSlot();
				return !GetInventory().GetSlotLock( slot );
			}
		}
		return true;
	}
	/**@fn		CanDetachAttachment
	 * @brief	calls this->CanDetachAttachment(parent)
	 * @return	true if action allowed
	 *
	 * @note: return scriptConditionExecute(this, parent, "CanDetachAttachment");
	 **/
	bool CanDetachAttachment (EntityAI parent)
	{
		return true;
	}
	
	bool CanBeFSwaped()
	{
		return true;
	}
	
	bool CanCombineAttachment(notnull EntityAI e, int slot, bool stack_max_limit = false)
	{
		EntityAI att = GetInventory().FindAttachment(slot);
		if(att)
			return att.CanBeCombined(e, true, stack_max_limit);
		return false;
	}	
	
	bool CanBeCombined(EntityAI other_item, bool reservation_check = true, bool stack_max_limit = false )
	{
		return false;
	}
	
	void CombineItemsEx(EntityAI entity2, bool use_stack_max = false );
	
	void SplitIntoStackMaxEx(EntityAI destination_entity, int slot_id);
	
	void CombineItemsClient(EntityAI entity2, bool use_stack_max = false )
	{}
	
	void SplitIntoStackMaxClient(EntityAI destination_entity, int slot_id);
		
	/**@fn		CanReceiveItemIntoCargo
	 * @brief	calls this->CanReceiveItemIntoCargo(item)
	 * @return	true if action allowed
	 *
	 * @note: return scriptConditionExecute(this, item, "CanReceiveItemIntoCargo");
	 **/
	bool CanReceiveItemIntoCargo(EntityAI item)
	{
		if (GetInventory() && GetInventory().GetCargo())
			return GetInventory().GetCargo().CanReceiveItemIntoCargo(item);
		
		return true;
	}
	
	/**@fn		CanLoadItemIntoCargo
	 * @brief	calls this->CanLoadItemIntoCargo(item), is called on server start when loading in the storage
	 * @return	true if action allowed
	 *
	 * @note: return scriptConditionExecute(this, item, "CanLoadItemIntoCargo");
	 **/
	bool CanLoadItemIntoCargo(EntityAI item)
	{
		return true;
	}
	
	/**@fn		CanPutInCargo
	 * @brief	calls this->CanPutInCargo(parent)
	 * @return	true if action allowed
	 *
	 * @note: return scriptConditionExecute(this, parent, "CanPutInCargo");
	 **/
	bool CanPutInCargo (EntityAI parent)
	{
		return !IsHologram();
	}

	/**@fn		CanSwapItemInCargo
	 * @brief	calls this->CanSwapItemInCargo(child_entity, new_entity)
	 * @return	true if action allowed
	 *
	 * @note: return ScriptConditionExecute(GetOwner(), child_entity, "CanSwapItemInCargo", new_entity);
	 **/
	bool CanSwapItemInCargo (EntityAI child_entity, EntityAI new_entity)
	{
		if (GetInventory() && GetInventory().GetCargo())
			return GetInventory().GetCargo().CanSwapItemInCargo(child_entity, new_entity);
		
		return true;
	}

	/**@fn		CanReleaseCargo
	 * @brief	calls this->CanReleaseCargo(cargo)
	 * @return	true if action allowed
	 *
	 * @note: return scriptConditionExecute(this, cargo, "CanReleaseCargo");
	 **/
	bool CanReleaseCargo (EntityAI cargo)
	{
		return true;
	}

	/**@fn		CanRemoveFromCargo
	 * @brief	calls this->CanRemoveFromCargo(parent)
	 * @return	true if action allowed
	 *
	 * @note: return scriptConditionExecute(this, parent, "CanRemoveFromCargo");
	 **/
	bool CanRemoveFromCargo (EntityAI parent)
	{
		return true;
	}
	
	/**@fn		CanReceiveItemIntoInventory
	 * @brief calls this->CanReceiveItemIntoInventory(item)
	 * @return	true if action allowed
	 *
	 * @note: return scriptConditionExecute(this, , "CanReceiveItemIntoInventory");
	 **/
	/*bool CanReceiveItemIntoInventory (EntityAI entity_ai)
	{
		return true;
	}*/

	/**@fn		CanPutInInventory
	 * @brief calls this->CanPutInInventory(parent)
	 * @return	true if action allowed
	 *
	 * @note: return scriptConditionExecute(this, parent, "ConditionIntoInventory");
	 **/
	/*bool CanPutInInventory (EntityAI parent)
	{
		return true;
	}*/

	/**@fn		CanReceiveItemIntoHands
	 * @brief	calls this->CanReceiveItemIntoHands(item_to_hands)
	 * @return	true if action allowed
	 *
	 * @note: scriptConditionExecute(this, item_to_hands, "CanReceiveItemIntoHands");
	 **/
	bool CanReceiveItemIntoHands (EntityAI item_to_hands)
	{
		return true;
	}
	
	bool AreChildrenAccessible()
	{
		InventoryLocation lcn = new InventoryLocation();
		EntityAI ent = this;
		int attachmentDepth = 0;
		while (ent)
		{
			if (ent.GetInventory().GetCurrentInventoryLocation(lcn) && lcn.IsValid())
			{
				if (lcn.GetType() == InventoryLocationType.CARGO || lcn.GetType() == InventoryLocationType.PROXYCARGO)
				{
					return false;
				}
				
				//hands treated as regular attachment here
				if (lcn.GetType() == InventoryLocationType.ATTACHMENT || lcn.GetType() == InventoryLocationType.HANDS)
				{
					attachmentDepth++;
				}
			}
			
			ent = ent.GetHierarchyParent();
		}
		
		return attachmentDepth <= GameConstants.INVENTORY_MAX_REACHABLE_DEPTH_ATT;
	}
	
	bool IsBeingPlaced()
	{
		return false;
	}
	
	override bool IsHologram()
	{
		return false;
	}
	
	bool CanSaveItemInHands (EntityAI item_in_hands)
	{
		return true;
	}

	/**@fn		CanPutIntoHands
	 * @brief calls this->CanPutIntoHands(parent)
	 * @return	true if action allowed
	 *
	 * @note: return scriptConditionExecute(this, parent, "CanPutIntoHands");
	 **/
	bool CanPutIntoHands (EntityAI parent)
	{
		return !IsHologram();
	}

	/**@fn		CanReleaseFromHands
	 * @brief calls this->CanReleaseFromHands(handheld)
	 * @return	true if action allowed
	 *
	 * @note: scriptConditionExecute(this, handheld, "CanReleaseFromHands");
	 **/
	bool CanReleaseFromHands (EntityAI handheld)
	{
		return true;
	}

	/**@fn		CanRemoveFromHands
	 * @brief	calls this->CanRemoveFromHands(parent)
	 * @return	true if action allowed
	 *
	 * @note: return scriptConditionExecute(this, parent, "CanRemoveFromHands");
	 **/
	bool CanRemoveFromHands (EntityAI parent)
	{
		return true;
	}

	/**@fn		CanDisplayAttachmentSlot
	 * @param	slot_name->name of the attachment slot that will or won't be displayed
	 * @return	true if attachment icon can be displayed in UI (inventory)
	 **/	
	bool CanDisplayAttachmentSlot( string slot_name )
	{
		Debug.LogWarning("Obsolete function - use CanDisplayAttachmentSlot with slot id parameter");
		return InventorySlots.GetShowForSlotId(InventorySlots.GetSlotIdFromString(slot_name));
	}
	
	/**@fn		CanDisplayAttachmentSlot
	 * @param	slot_id->id of the attachment slot that will or won't be displayed
	 * @return	true if attachment icon can be displayed in UI (inventory)
	 **/	
	bool CanDisplayAttachmentSlot( int slot_id )
	{
		return InventorySlots.GetShowForSlotId(slot_id);
	}
	
	/**@fn		CanDisplayAnyAttachmentSlot
	 * @return	true if any attachment slot can be shown
	 **/	
	bool CanDisplayAnyAttachmentSlot()
	{
		int count = GetInventory().GetAttachmentSlotsCount();
		int slotID;
		for (int i = 0; i < count; i++)
		{
			slotID = GetInventory().GetAttachmentSlotId(i);
			if (CanDisplayAttachmentSlot(slotID))
			{
				return true;
			}
		}
		
		return false;
	}

	/**@fn		CanDisplayAttachmentCategory
	 * @param	category_name->name of the attachment category that will or won't be displayed
	 * @return	true if attachment icon can be displayed in UI (inventory)
	 **/		
	bool CanDisplayAttachmentCategory( string category_name )
	{
		return true;
	}
	
	/**@fn		CanDisplayCargo
	 * @return	true if cargo can be displayed in UI (inventory)
	 **/		
	bool CanDisplayCargo()
	{
		return GetInventory().GetCargo() != null;
	}
	
	/**@fn		CanAssignToQuickbar
	 * @return	true if item can be assigned to quickbar safely
	 **/		
	bool CanAssignToQuickbar()
	{
		return true;
	}
	
	/**@fn		CanAssignAttachmentsToQuickbar
	 * @return	true if attached item can be assigned to quickbar safely
	 **/
	bool CanAssignAttachmentsToQuickbar()
	{
		return true;
	}
	
	/**@fn		IgnoreOutOfReachCondition
	 * @return	if true, attachment condition for out of reach (inventory) will be ignored
	 **/		
	bool IgnoreOutOfReachCondition()
	{
		return GetHierarchyRootPlayer() == GetGame().GetPlayer();
	}	

	// !Called on CHILD when it's attached to parent.
	void OnWasAttached( EntityAI parent, int slot_id );
	
	// !Called on CHILD when it's detached from parent.
	void OnWasDetached( EntityAI parent, int slot_id )
	{
		if (!IsFlagSet(EntityFlags.VISIBLE))
		{
			SetInvisible(false);
			OnInvisibleSet(false);
			SetInvisibleRecursive(false,parent);
		}
	}
	
	void OnCargoChanged() { }
	
	bool IsTakeable()
	{
		return false;
	}
	
	proto native GameInventory GetInventory();
	proto native void CreateAndInitInventory();
	proto native void DestroyInventory();
		
	int GetSlotsCountCorrect()
	{
		if (GetInventory())
			return GetInventory().GetAttachmentSlotsCount();
		else
			return -1;
	}

	EntityAI FindAttachmentBySlotName(string slot_name)
	{
		if (g_Game && GetInventory())
		{
			int slot_id = InventorySlots.GetSlotIdFromString(slot_name);
			if (slot_id != InventorySlots.INVALID)
				return GetInventory().FindAttachment(slot_id); 
		}
		return null;
	}
	
	// Check whether attachmnent slot is reserved
	bool IsSlotReserved(int slotID)
	{
		Man player = GetHierarchyRootPlayer();
		if (!player)
			return false;
		
		HumanInventory inv = player.GetHumanInventory();
		if (!inv || inv.GetUserReservedLocationCount() == 0)
			return false;
		
		int id = inv.FindFirstUserReservedLocationIndexForContainer(this);
		InventoryLocation loc = new InventoryLocation();
		if (id == -1)
			return false;
		
		inv.GetUserReservedLocation(id, loc);

		if (loc.GetSlot() != slotID)
			return false;
		
		return true;		
	}

	/**@fn		IsLockedInSlot
	 * @return	true if entity is locked in attachment slot
	 **/	
	bool IsLockedInSlot()
	{
		EntityAI parent = GetHierarchyParent();
		if ( parent )
		{
			InventoryLocation inventory_location = new InventoryLocation();
			GetInventory().GetCurrentInventoryLocation( inventory_location );
			
			return parent.GetInventory().GetSlotLock( inventory_location.GetSlot() );
		}
		
		return false;
	}
	
	/**
	\brief Put item anywhere into this entity (as attachment or into cargo, recursively)
	*/
	bool PredictiveTakeEntityToInventory (FindInventoryLocationType flags, notnull EntityAI item)
	{
		if ( GetGame().IsMultiplayer() )
			return GetInventory().TakeEntityToInventory(InventoryMode.JUNCTURE, flags, item);
		else
			return GetInventory().TakeEntityToInventory(InventoryMode.PREDICTIVE, flags, item);
	}
	bool LocalTakeEntityToInventory (FindInventoryLocationType flags, notnull EntityAI item)
	{
		return GetInventory().TakeEntityToInventory(InventoryMode.LOCAL, flags, item);
	}
	bool ServerTakeEntityToInventory (FindInventoryLocationType flags, notnull EntityAI item)
	{
		return GetInventory().TakeEntityToInventory(InventoryMode.SERVER, flags, item);
	}
	bool PredictiveTakeEntityToTargetInventory (notnull EntityAI target, FindInventoryLocationType flags, notnull EntityAI item)
	{
		if ( GetGame().IsMultiplayer() )
			return GetInventory().TakeEntityToTargetInventory(InventoryMode.JUNCTURE, target, flags, item);
		else
			return GetInventory().TakeEntityToTargetInventory(InventoryMode.PREDICTIVE, target, flags, item);
	}
	bool LocalTakeEntityToTargetInventory (notnull EntityAI target, FindInventoryLocationType flags, notnull EntityAI item)
	{
		return GetInventory().TakeEntityToTargetInventory(InventoryMode.LOCAL, target, flags, item);
	}
	bool ServerTakeEntityToTargetInventory (notnull EntityAI target, FindInventoryLocationType flags, notnull EntityAI item)
	{
		return GetInventory().TakeEntityToTargetInventory(InventoryMode.SERVER, target, flags, item);
	}
	/**
	\brief Put item into into cargo
	*/
	bool PredictiveTakeEntityToCargo (notnull EntityAI item)
	{
		if ( GetGame().IsMultiplayer() )
			return GetInventory().TakeEntityToCargo(InventoryMode.JUNCTURE, item);
		else
			return GetInventory().TakeEntityToCargo(InventoryMode.PREDICTIVE, item);
	}
	bool LocalTakeEntityToCargo (notnull EntityAI item)
	{
		return GetInventory().TakeEntityToCargo(InventoryMode.LOCAL, item);
	}
	bool ServerTakeEntityToCargo (notnull EntityAI item)
	{
		return GetInventory().TakeEntityToCargo(InventoryMode.SERVER, item);
	}

	bool PredictiveTakeEntityToTargetCargo (notnull EntityAI target, notnull EntityAI item)
	{
		if ( GetGame().IsMultiplayer() )
			return GetInventory().TakeEntityToTargetCargo(InventoryMode.JUNCTURE, target, item);
		else
			return GetInventory().TakeEntityToTargetCargo(InventoryMode.PREDICTIVE, target, item);
	}
	bool LocalTakeEntityToTargetCargo (notnull EntityAI target, notnull EntityAI item)
	{
		return GetInventory().TakeEntityToTargetCargo(InventoryMode.LOCAL, target, item);
	}
	bool ServerTakeEntityToTargetCargo (notnull EntityAI target, notnull EntityAI item)
	{
		return GetInventory().TakeEntityToTargetCargo(InventoryMode.SERVER, target, item);
	}
	/**
	\brief Put item into into cargo on specific cargo location
	*/
	bool PredictiveTakeEntityToCargoEx (notnull EntityAI item, int idx, int row, int col)
	{
		if ( GetGame().IsMultiplayer() )
			return GetInventory().TakeEntityToCargoEx(InventoryMode.JUNCTURE, item, idx, row, col);
		else
			return GetInventory().TakeEntityToCargoEx(InventoryMode.PREDICTIVE, item, idx, row, col);
	}
	bool LocalTakeEntityToCargoEx (notnull EntityAI item, int idx, int row, int col)
	{
		return GetInventory().TakeEntityToCargoEx(InventoryMode.LOCAL, item, idx, row, col);
	}

	bool PredictiveTakeEntityToTargetCargoEx (notnull CargoBase cargo, notnull EntityAI item, int row, int col)
	{
		if ( GetGame().IsMultiplayer() )
			return GetInventory().TakeEntityToTargetCargoEx(InventoryMode.JUNCTURE, cargo, item, row, col);
		else
			return GetInventory().TakeEntityToTargetCargoEx(InventoryMode.PREDICTIVE, cargo, item, row, col);
	}
	bool LocalTakeEntityToTargetCargoEx (notnull CargoBase cargo, notnull EntityAI item, int row, int col)
	{
		return GetInventory().TakeEntityToTargetCargoEx(InventoryMode.LOCAL, cargo, item, row, col);
	}
	bool ServerTakeEntityToTargetCargoEx (notnull CargoBase cargo, notnull EntityAI item, int row, int col)
	{
		return GetInventory().TakeEntityToTargetCargoEx(InventoryMode.SERVER, cargo, item, row, col);
	}
	/**
	\brief Returns if item can be added as attachment on specific slot. Note that slot index IS NOT slot ID! Slot ID is defined in DZ/data/config.cpp
	*/
	bool PredictiveTakeEntityAsAttachmentEx (notnull EntityAI item, int slot)
	{
		if ( GetGame().IsMultiplayer() )
			return GetInventory().TakeEntityAsAttachmentEx(InventoryMode.JUNCTURE, item, slot);
		else
			return GetInventory().TakeEntityAsAttachmentEx(InventoryMode.PREDICTIVE, item, slot);
	}
	bool LocalTakeEntityAsAttachmentEx (notnull EntityAI item, int slot)
	{
		return GetInventory().TakeEntityAsAttachmentEx(InventoryMode.LOCAL, item, slot);
	}
	bool ServerTakeEntityAsAttachmentEx (notnull EntityAI item, int slot)
	{
		return GetInventory().TakeEntityAsAttachmentEx(InventoryMode.SERVER, item, slot);
	}

	bool PredictiveTakeEntityToTargetAttachmentEx (notnull EntityAI target, notnull EntityAI item, int slot)
	{
		if ( GetGame().IsMultiplayer() )
			return GetInventory().TakeEntityAsTargetAttachmentEx(InventoryMode.JUNCTURE, target, item, slot);
		else
			return GetInventory().TakeEntityAsTargetAttachmentEx(InventoryMode.PREDICTIVE, target, item, slot);
	}
	bool LocalTakeEntityToTargetAttachmentEx (notnull EntityAI target, notnull EntityAI item, int slot)
	{
		return GetInventory().TakeEntityAsTargetAttachmentEx(InventoryMode.LOCAL, target, item, slot);
	}
	bool ServerTakeEntityToTargetAttachmentEx (notnull EntityAI target, notnull EntityAI item, int slot)
	{
		return GetInventory().TakeEntityAsTargetAttachmentEx(InventoryMode.SERVER, target, item, slot);
	}

	bool PredictiveTakeEntityToTargetAttachment (notnull EntityAI target, notnull EntityAI item)
	{
		if ( GetGame().IsMultiplayer() )
			return GetInventory().TakeEntityAsTargetAttachment(InventoryMode.JUNCTURE, target, item);
		else
			return GetInventory().TakeEntityAsTargetAttachment(InventoryMode.PREDICTIVE, target, item);
	}
	bool LocalTakeEntityToTargetAttachment (notnull EntityAI target, notnull EntityAI item)
	{
		return GetInventory().TakeEntityAsTargetAttachment(InventoryMode.LOCAL, target, item);
	}
	bool ServerTakeEntityToTargetAttachment (notnull EntityAI target, notnull EntityAI item)
	{
		return GetInventory().TakeEntityAsTargetAttachment(InventoryMode.SERVER, target, item);
	}

	bool PredictiveTakeToDst (notnull InventoryLocation src, notnull InventoryLocation dst)
	{
		if ( GetGame().IsMultiplayer() )
			return GetInventory().TakeToDst(InventoryMode.JUNCTURE, src, dst);
		else
			return GetInventory().TakeToDst(InventoryMode.PREDICTIVE, src, dst);
	}
	bool LocalTakeToDst (notnull InventoryLocation src, notnull InventoryLocation dst)
	{
		return GetInventory().TakeToDst(InventoryMode.LOCAL, src, dst);
	}
	bool ServerTakeToDst (notnull InventoryLocation src, notnull InventoryLocation dst)
	{
		return GetInventory().TakeToDst(InventoryMode.SERVER, src, dst);
	}

	/**
	\brief Put item into as attachment
	*/
	bool PredictiveTakeEntityAsAttachment(notnull EntityAI item)
	{
		if (GetGame().IsMultiplayer())
			return GetInventory().TakeEntityAsAttachment(InventoryMode.JUNCTURE, item);
		else
			return GetInventory().TakeEntityAsAttachment(InventoryMode.PREDICTIVE, item);
	}
	bool LocalTakeEntityAsAttachment (notnull EntityAI item)
	{
		return GetInventory().TakeEntityAsAttachment(InventoryMode.LOCAL, item);
	}
	bool ServerTakeEntityAsAttachment (notnull EntityAI item)
	{
		return GetInventory().TakeEntityAsAttachment(InventoryMode.SERVER, item);
	}

	bool PredictiveDropEntity(notnull EntityAI item)
	{
		return false;
	}
	
	bool LocalDropEntity(notnull EntityAI item)
	{
		return false;
	}
	
	bool ServerDropEntity(notnull EntityAI item)
	{
		return false;
	}

	/**
	\brief Get attached entity by type
	*/
	EntityAI GetAttachmentByType(typename type)
	{
		for ( int i = 0; i < GetInventory().AttachmentCount(); i++ )
		{
			EntityAI attachment = GetInventory().GetAttachmentFromIndex( i );
			if ( attachment && attachment.IsInherited( type ) )
				return attachment;
		}
		return NULL;
	}

	/**
	\brief Get attached entity by config type name
	*/
	EntityAI GetAttachmentByConfigTypeName(string type)
	{
		for ( int i = 0; i < GetInventory().AttachmentCount(); i++ )
		{
			EntityAI attachment = GetInventory().GetAttachmentFromIndex ( i );
			if ( attachment.IsKindOf ( type ) )
				return attachment;
			}
		return NULL;
	}
	/**
	\brief Returns if item can be dropped out from this entity
	*/
	bool CanDropEntity(notnull EntityAI item)
	{
		return true;
	}

	EntityAI SpawnInInventoryOrGroundPos(string object_name, GameInventory inv, vector pos)
	{
		if (inv)
		{
			EntityAI res = inv.CreateInInventory(object_name);
			if (res)
			{
				return res;
			}
		}

		return SpawnEntityOnGroundPos(object_name, pos);
	}

	/**
	 **/
	EntityAI SpawnEntityOnGroundPos(string object_name, vector pos)
	{
		InventoryLocation il = new InventoryLocation();
		vector mat[4];
		Math3D.MatrixIdentity4(mat);
		mat[3] = pos;
		il.SetGround(NULL, mat);
		return SpawnEntity(object_name, il,ECE_PLACE_ON_SURFACE,RF_DEFAULT);
	}
	/**
	 **/
	EntityAI SpawnEntityOnGround(string object_name, vector mat[4])
	{
		InventoryLocation il = new InventoryLocation();
		il.SetGround(NULL, mat);
		return SpawnEntity(object_name, il,ECE_PLACE_ON_SURFACE,RF_DEFAULT);
	}
	
	//----------------------------------------------------------------

	bool CanSwapEntities(EntityAI otherItem, InventoryLocation otherDestination, InventoryLocation destination)
	{
		return true;
	}
	
	// Forward declarations to allow lower modules to access properties that are modified from higher modules
	// These are mainly used within the ItemBase
	void SetWet(float value, bool allow_client = false);
	void AddWet(float value);
	void SetWetMax();

	float GetWet()
	{
		return 0;
	}
	
	float GetWetMax()
	{
		return 0;
	}

	float GetWetMin()
	{
		return 0;
	}
	
	float GetWetInit()
	{
		return 0;
	}
	
	bool HasWetness()
	{
		return GetWetMax() - GetWetMin() != 0;
	}
	
	void OnWetChanged(float newVal, float oldVal);

	void OnWetLevelChanged(EWetnessLevel newLevel, EWetnessLevel oldLevel);
	// ! Returns current wet level of the entity
	EWetnessLevel GetWetLevel();
	
	// ! Calculates wet level from a given wetness, to get level of an entity, use 'GetWetLevel()' instead
	static EWetnessLevel GetWetLevelInternal(float wetness)
	{
		if (wetness < GameConstants.STATE_DAMP)
		{
			return EWetnessLevel.DRY;
		}
		else if (wetness < GameConstants.STATE_WET)
		{
			return EWetnessLevel.DAMP;
		}
		else if (wetness < GameConstants.STATE_SOAKING_WET)
		{
			return EWetnessLevel.WET;
		}
		else if (wetness < GameConstants.STATE_DRENCHED)
		{
			return EWetnessLevel.SOAKING;
		}
		return EWetnessLevel.DRENCHED;
	}
	//----------------------------------------------------------------
	bool HasQuantity()
	{
		return false;
	}
	
	bool SetQuantity(float value, bool destroy_config = true, bool destroy_forced = false, bool allow_client = false, bool clamp_to_stack_max = true);
	
	float GetQuantity()
	{
		return 0;
	}
	
	void SetQuantityNormalized(float value, bool destroy_config = true, bool destroy_forced = false);
			
	float GetQuantityNormalized()
	{
		return 0;
	}
	
	int GetQuantityMax()
	{
		return 0;
	}
	
	int GetQuantityMin()
	{
		return 0;
	}
	
	void SetQuantityToMinimum();
	
	int GetTargetQuantityMax(int attSlotID = -1)
	{
		return 0;
	}
	
	int GetQuickBarBonus()
	{
		return 0;
	}

	//----------------------------------------------------------------
	
	bool UseConfigInitTemperature()
	{
		return (IsMan() || IsAnimal() || IsZombie()) && IsAlive() || IsCorpse();
	}
	
	protected void InitTemperature()
	{
		EntityAI rootParent = GetHierarchyRoot();
		bool isParentAliveOrganism = false;
		if (rootParent && rootParent != this)
			isParentAliveOrganism = (rootParent.IsMan() || rootParent.IsAnimal() || rootParent.IsZombie()) && rootParent.IsAlive();
		
		if (UseConfigInitTemperature())
		{
			SetTemperatureDirect(m_VarTemperatureInit);
		}
		else if (isParentAliveOrganism) //living player's inventory etc.
		{
			SetTemperatureDirect(rootParent.GetTemperature());
		}
		else
		{
			SetTemperatureDirect(g_Game.GetMission().GetWorldData().GetBaseEnvTemperatureAtObject(this));
		}
		
		SetFrozen(GetTemperature() < GetTemperatureFreezeThreshold());
	}
	
	void SetTemperatureDirect(float value, bool allow_client = false)
	{
		if (!IsServerCheck(allow_client)) 
			return;
		
		float min = GetTemperatureMin();
		float max = GetTemperatureMax();
		float previousValue = m_VarTemperature;
		m_VarTemperature = Math.Clamp(value, min, max);
		
		if (previousValue != m_VarTemperature)
			SetVariableMask(VARIABLE_TEMPERATURE);
	}
	
	//! not really deprecated, but missing context info from TemperatureData. Default values used instead.
	void SetTemperature(float value, bool allow_client = false)
	{
		/*#ifdef DEVELOPER
		ErrorEx("Obsolete 'SetTemperature' called! Metadata will be extrapolated from base values.");
		#endif*/
		
		if (!IsServerCheck(allow_client)) 
			return;
		
		SetTemperatureEx(new TemperatureData(value));
	}
	
	void AddTemperature(float value)
	{
		SetTemperature(value + GetTemperature());
	}
	
	////////////////////////////////////////////
	//! sets temperature, handles base overheating and freezing state progression logics
	void SetTemperatureEx(TemperatureData data)
	{
		#ifdef DEVELOPER
		m_LastFTChangeTime = -1;
		m_PresumedTimeRemaining = -1;
		#endif
		
		if (!CanHaveTemperature())
		{
			Debug.Log("SetTemperatureEx | entity " + this + " does not have temperature range defined!");
			return;
		}
		
		if (!m_TAC.TryAccessSource(data))
			return;
		
		if (!IsServerCheck(false)) 
			return;
		
		InterpolateTempData(TemperatureDataInterpolated.Cast(data));
		float target = Math.Clamp(data.m_AdjustedTarget, GetTemperatureMin(), GetTemperatureMax());
		float delta;
		//overheating
		if (CanItemOverheat())
		{
			if (GetTemperature() > GetItemOverheatThreshold())
				delta = data.m_AdjustedTarget - GetTemperature();
			else
				delta = data.m_AdjustedTarget - GetItemOverheatThreshold();
			
			HandleItemOverheating(delta,data);
		}
		
		//freezing, can obstruct temperature change
		if (CanFreeze())
		{
			if (!m_IsFrozen)
			{
				delta = target - GetTemperatureFreezeThreshold();
				if (target < GetTemperatureFreezeThreshold())
				{
					//first crossing the threshold
					if (m_VarTemperature >= GetTemperatureFreezeThreshold()) //going DOWN or STAYING AT THRESHOLD, FREEZING;
					{
						SetTemperatureDirect(GetTemperatureFreezeThreshold());
					}
					else //going UP, still FREEZING
					{
						SetTemperatureDirect(target);
					}
					HandleFreezingProgression(delta,data);
				}
				else
				{
					SetTemperatureDirect(target);
					if (target > GetTemperatureFreezeThreshold())
						HandleFreezingProgression(delta,data);
				}
			}
			else
			{
				delta = target - GetTemperatureThawThreshold();
				if (target > GetTemperatureThawThreshold())
				{
					//first crossing the threshold
					if (m_VarTemperature <= GetTemperatureThawThreshold()) //going UP, THAWING
					{
						SetTemperatureDirect(GetTemperatureThawThreshold());
					}
					else //going DOWN, still THAWING
					{
						SetTemperatureDirect(target);
					}
					HandleFreezingProgression(delta,data);
				}
				else
				{
					SetTemperatureDirect(target);
					if (target < GetTemperatureThawThreshold())
						HandleFreezingProgression(delta,data);
				}
			}
		}
		else //!CanFreeze
		{
			SetTemperatureDirect(target);
		}
	}
	
	//! refreshes access without setting temperature, keeps the source locked in
	void RefreshTemperatureAccess(TemperatureData data)
	{
		if (!CanHaveTemperature())
		{
			Debug.Log("RefreshTemperatureAccess | entity " + this + " does not have temperature range defined!");
			return;
		}
		
		m_TAC.TryAccessSource(data);
	}
	
	void InterpolateTempData(TemperatureDataInterpolated data)
	{
		if (data)
			data.InterpolateTemperatureDelta(GetTemperature());
	}
	////////////////////////////////////////////
	
	//! presumably for debug purposes?
	void SetTemperatureMax()
	{
		SetTemperatureDirect(GetTemperatureMax());
		SetFrozen(GetTemperature() < GetTemperatureFreezeThreshold());
	}
	
	float GetTemperature()
	{
		return m_VarTemperature;
	}
	
	float GetTemperatureInit()
	{
		return m_VarTemperatureInit;
	}
	
	float GetTemperatureMin()
	{
		return m_VarTemperatureMin;
	}

	float GetTemperatureMax()
	{
		return m_VarTemperatureMax;
	}
	
	//! specifically for cooking system, to get heat source target temperatures
	bool GetCookingTargetTemperature(out float temperature)
	{
		return false;
	}
	
	/**
	\brief Returns temperature change speed multiplier for this item and all its children (multiplicative interaction)
	\note Values > 1 accelerate, values in <0,1) interval decelerate. Values < 0 should not be used here, but I'm not your mom.
	*/
	float GetHeatPermeabilityCoef()
	{
		return m_VarHeatPermeabilityCoef;
	}
	
	float GetTemperatureFreezeThreshold()
	{
		return m_VarTemperatureFreezeThreshold;
	}
	
	float GetTemperatureThawThreshold()
	{
		return m_VarTemperatureThawThreshold;
	}
	
	float GetTemperatureFreezeTime()
	{
		return m_VarTemperatureFreezeTime;
	}
	
	float GetTemperatureThawTime()
	{
		return m_VarTemperatureThawTime;
	}
	
	//! on server only
	float GetFreezeThawProgress()
	{
		return m_FreezeThawProgress;
	}
	
	//! on server only
	bool IsFreezeThawProgressFinished()
	{
		return m_FreezeThawProgress <= 0.0 || m_FreezeThawProgress >= 1.0;
	}
	
	//! 0->1 when freezing, 1->0 when thawing
	protected void SetFreezeThawProgress(float val)
	{
		m_FreezeThawProgress = val;
	}
	
	bool CanFreeze()
	{
		return CanHaveTemperature() && GetTemperatureFreezeThreshold() > GetTemperatureMin() && GetTemperatureThawThreshold() < GetTemperatureMax();
	}
	
	bool GetIsFrozen()
	{
		return m_IsFrozen;
	}
	
	void SetFrozen(bool frozen)
	{
		if (!CanFreeze() && frozen)
			return;
		
		bool previous = m_IsFrozen;
		m_IsFrozen = frozen;
		SetFreezeThawProgress(frozen);
		
		if (previous != frozen)
		{
			SetSynchDirty();
			OnFreezeStateChangeServer();
		}
	}
	
	protected void HandleFreezingProgression(float deltaHeat, TemperatureData data)
	{
		float progressVal = m_FreezeThawProgress;
		float progressDelta = 1;
		
		if (deltaHeat > 0)
			progressDelta = -1;
		
		if (data.m_UpdateTimeInfo == -1)
			progressDelta = (-deltaHeat / GameConstants.TEMPERATURE_RATE_AVERAGE_ABS) * GameConstants.TEMPERATURE_FREEZETHAW_LEGACY_COEF; //reverse-calculate the progress if actual time is not available
		else
			progressDelta *= data.m_UpdateTimeInfo;
		
		if (progressDelta == 0)
			return;
		
		float changeTimeDefault;
		float changeTimeMin;
		float changeTime;
		
		if (!m_IsFrozen)
		{
			changeTimeDefault = GetTemperatureFreezeTime();
			changeTimeMin = GameConstants.TEMPERATURE_TIME_FREEZE_MIN;
		}
		else
		{
			changeTimeDefault = GetTemperatureThawTime();
			changeTimeMin = GameConstants.TEMPERATURE_TIME_THAW_MIN;
		}
		
		float coef = data.m_UpdateTimeCoef;
		if (deltaHeat < 0) //use cooling coef when freezing (mostly just to make sure)
			coef = GameConstants.TEMP_COEF_COOLING_GLOBAL;
		
		if (coef != 0)
			changeTimeDefault *= 1/coef;
		
		changeTime = Math.Lerp(Math.Max(changeTimeDefault,changeTimeMin),changeTimeMin,data.m_InterpolatedFraction);
		progressVal = progressVal + progressDelta / changeTime;
		
		float remnantTemp = 0;
		if (!m_IsFrozen && progressVal >= 1)
		{
			SetFrozen(true);
			if (progressVal > 1.0)
			{
				if (data.m_UpdateTimeInfo == -1)
					remnantTemp = (progressVal - 1) * changeTime * GameConstants.TEMPERATURE_RATE_AVERAGE_ABS * -1;
				else
					remnantTemp = (((progressVal - 1) * changeTime) / progressDelta) * deltaHeat;
			}
		}
		else if (m_IsFrozen && progressVal <= 0)
		{
			SetFrozen(false);
			if (progressVal < 0.0)
			{
				if (data.m_UpdateTimeInfo == -1)
					remnantTemp = -progressVal * changeTime * GameConstants.TEMPERATURE_RATE_AVERAGE_ABS;
				else
					remnantTemp = ((-progressVal * changeTime) / progressDelta) * deltaHeat;
			}
		}
		else
		{
			if ((progressDelta < 0 && !m_IsFrozen) || (progressDelta > 0 && m_IsFrozen))
				progressVal = (progressDelta * GameConstants.TEMPERATURE_FREEZETHAW_ACCELERATION_COEF) / changeTime + m_FreezeThawProgress;
			
			SetFreezeThawProgress(Math.Clamp(progressVal,0,1));
		}
		
		if (remnantTemp >= GameConstants.TEMPERATURE_SENSITIVITY_THRESHOLD)//discards tiny values
			SetTemperatureDirect(GetTemperature() + remnantTemp);
		
		#ifdef DEVELOPER
		if (progressVal > 0 && progressVal < 1)
		{
			m_LastFTChangeTime = changeTime;
			if (!m_IsFrozen)
				m_PresumedTimeRemaining = (1 - progressVal) * changeTime;
			else
				m_PresumedTimeRemaining = progressVal * changeTime;
		}
		#endif
	}
	
	void OnFreezeStateChangeClient();
	void OnFreezeStateChangeServer();
	
	//----------------------------------------------------------------
	//! Overheat time CAN be 0, GameConstants.TEMPERATURE_TIME_OVERHEAT_MIN is ignored if so
	bool CanItemOverheat()
	{
		return GetItemOverheatTime() >= 0;
	}
	
	//! if undefined, max temperature used as default
	float GetItemOverheatThreshold()
	{
		return GetTemperatureMax();
	}
	
	//! any configured value >= 0 will simulate overheating
	float GetItemOverheatTime()
	{
		return m_VarTemperatureOverheatTime;
	}
	
	bool IsItemOverheated()
	{
		return m_OverheatProgress >= 1;
	}
	
	float GetItemOverheatProgress()
	{
		return m_OverheatProgress;
	}
	
	void SetItemOverheatProgress(float val, float deltaTime = 0)
	{
		float previous = m_OverheatProgress;
		m_OverheatProgress = Math.Clamp(val,0,1);
		
		if (m_OverheatProgress >= 1)
		{
			if (previous < 1)
				OnItemOverheatStart();
			
			OnItemOverheat(deltaTime);
		}
		else if (previous >= 1)
		{
			OnItemOverheatEnd();
		}
	}
	
	//! override to implement desired overheat behavior on entity
	protected void OnItemOverheatStart();
	protected void OnItemOverheat(float deltaTime); //! note, that the deltaTime could be reverse-calculated and not totally accurate
	protected void OnItemOverheatEnd();
	
	protected void HandleItemOverheating(float deltaHeat, TemperatureData data)
	{
		float deltaTime = 1;
		float progressVal = m_OverheatProgress;
		
		if (deltaHeat < 0)
			deltaTime = -1;
		
		if (data.m_UpdateTimeInfo == -1)
			deltaTime = deltaHeat / GameConstants.TEMPERATURE_RATE_AVERAGE_ABS; //reverse-calculate the progress if actual time is not available
		else
			deltaTime *= data.m_UpdateTimeInfo;
		
		if (GetItemOverheatTime() > 0)
		{
			float changeTime = Math.Lerp(Math.Max(GameConstants.TEMPERATURE_TIME_OVERHEAT_MIN,GetItemOverheatTime()),GameConstants.TEMPERATURE_TIME_OVERHEAT_MIN,Math.Clamp(data.m_InterpolatedFraction,0,1));
			progressVal += deltaTime / changeTime;
		}
		else
		{
			if (deltaHeat < 0)
				progressVal = 0;
			else if (deltaHeat > 0)
				progressVal = 1;
		}
		
		SetItemOverheatProgress(Math.Clamp(progressVal,0,1),deltaTime);
	}
	//----------------------------------------------------------------
	
	void SetLiquidType(int value, bool allow_client = false);
	int GetLiquidType()
	{
		return 0;
	}
	
	//----------------------------------------------------------------
	void SetColor(int r, int g, int b, int a);
	void GetColor(out int r,out int g,out int b,out int a)
	{
		r = -1;
		g = -1;
		b = -1;
		a = -1;
	}
	
	//----------------------------------------------------------------
	
	void SetStoreLoad(bool value);
	bool IsStoreLoad()
	{
		return false;
	}
	
	void SetStoreLoadedQuantity(float value);
	float GetStoreLoadedQuantity()
	{
		return 0.0;
	}
	
	//----------------------------------------------------------------
	
	void SetCleanness(int value, bool allow_client = false);
	int GetCleanness()
	{
		return 0;
	}
	
	//----------------------------------------------------------------
	
	bool IsServerCheck(bool allow_client)
	{
		if (g_Game.IsServer())
			return true;
		
		if (allow_client)
			return true;

		if (GetGame().IsClient() && GetGame().IsMultiplayer()) 
		{
			Error("Attempting to change variable client side, variables are supposed to be changed on server only !!");
			return false;
		}

		return true;
	}
	
	//----------------------------------------------------------------
		
	HiddenSelectionsData GetHiddenSelectionsData()
	{
		return m_HiddenSelectionsData;
	}
	
	//! Returns index of the string found in cfg array 'hiddenSelections'. If it's not found then it returns -1.
	int GetHiddenSelectionIndex( string selection )
	{
		if (m_HiddenSelectionsData)
			return m_HiddenSelectionsData.GetHiddenSelectionIndex( selection );

		return -1;
	}
	
	//! Returns the hiddenSelectionsTextures array from the object's config
	override TStringArray GetHiddenSelections()
	{
		if (m_HiddenSelectionsData)
			return m_HiddenSelectionsData.m_HiddenSelections;
		else
			return super.GetHiddenSelections();
	}
	
	//! Returns the hiddenSelectionsTextures array from the object's config
	override TStringArray GetHiddenSelectionsTextures()
	{
		if (m_HiddenSelectionsData)
			return m_HiddenSelectionsData.m_HiddenSelectionsTextures;
		else
			return super.GetHiddenSelectionsTextures();
	}
	
	//! Returns the hiddenSelectionsMaterials array from the object's config
	override TStringArray GetHiddenSelectionsMaterials()
	{
		if (m_HiddenSelectionsData)
			return m_HiddenSelectionsData.m_HiddenSelectionsMaterials;
		else
			return super.GetHiddenSelectionsMaterials();
	}
	
	/**
	 * @fn		EntityPlaceOnSurfacePointWithRotation
	 * @brief	applies correct rotation according to WRUtils::EntityPlacementRotation(type->GetRotationFlags())
	 *
	 * @param[out]	trans	\p		the transform to apply the correct rotation on
	 * @param[in]	pos		\p		position to check
	 * @param[in]	dx		\p		up vector x to align to
	 * @param[in]	dz		\p		up vector z to align to
	 * @param[in]	fAngle	\p		angle to position
	 * @param[in]	align	\p		align to surface
	 **/	
	proto native void PlaceOnSurfaceRotated(out vector trans[4], vector pos, float dx = 0, float dz = 0, float fAngle = 0, bool align = false);

	/**
	 * @fn		RegisterNetSyncVariableBool
	 * @brief	registers bool variable synchronized over network
	 *
	 * @param[in]	variableName	\p		which variable should be synchronized
	 **/	
	proto native void RegisterNetSyncVariableBool(string variableName);
	
	/**
	 * @fn		RegisterNetSyncVariableBoolSignal
	 * @brief	when bool variable is true, it's sent to clients and become false again
	 *
	 * @param[in]	variableName	\p		which variable should be synchronized
	 **/	
	proto native void RegisterNetSyncVariableBoolSignal(string variableName);

	/**
	 * @fn		RegisterNetSyncVariableInt
	 * @brief	registers int variable synchronized over network
	 *
	 * @param[in]	variableName	\p		which variable should be synchronized
	 * @param[in]	minValue		\p		minimal value used for quantization (when minValue == maxValue, no quatization is done)
	 * @param[in]	maxValue		\p		maximal value used for quantization (when minValue == maxValue, no quatization is done)
	 **/	
	proto native void RegisterNetSyncVariableInt(string variableName, int minValue = 0, int maxValue = 0);
	
	/**
	 * @fn		RegisterNetSyncVariableFloat
	 * @brief	registers float variable synchronized over network
	 *
	 * @param[in]	variableName	\p		which variable should be synchronized
	 * @param[in]	minValue		\p		minimal value used for quantization (when minValue == maxValue, no quatization is done)
	 * @param[in]	maxValue		\p		maximal value used for quantization (when minValue == maxValue, no quatization is done)
	 * @param[in]	precision		\p		precision in number of digits after decimal point
	 **/	
	proto native void RegisterNetSyncVariableFloat(string variableName, float minValue = 0, float maxValue = 0, int precision = 1);
	
	/**
	 * @fn		RegisterNetSyncVariableObject
	 * @brief	registers object variable synchronized over network, only synchronizes if network id is assigned. Doesn't handle object despawn on client
	 *
	 * @param[in]	variableName	\p		which variable should be synchronized
	 **/	
	proto native void RegisterNetSyncVariableObject(string variableName);
	
	proto native void UpdateNetSyncVariableInt(string variableName, float minValue = 0, float maxValue = 0);
	proto native void UpdateNetSyncVariableFloat(string variableName, float minValue = 0, float maxValue = 0, int precision = 1);

	proto native void SwitchLight(bool isOn);

	//! Simple hidden selection state; 0 == hidden
	proto native void SetSimpleHiddenSelectionState(int index, bool state);
	proto native bool IsSimpleHiddenSelectionVisible(int index);
	
	//! Change texture in hiddenSelections
	proto native void SetObjectTexture(int index, string texture_name);
	proto native owned string GetObjectTexture(int index);
	//! Change material in hiddenSelections
	proto native void SetObjectMaterial(int index, string mat_name);
	proto native owned string GetObjectMaterial(int index);
		
	proto native bool	IsPilotLight();
	proto native void SetPilotLight(bool isOn);

	/**
	\brief Engine calls this function to collect data from entity to store for persistence (on server side).
	@code
	void OnStoreSave(ParamsWriteContext ctx)
	{
		// dont forget to propagate this call trough class hierarchy!
		super.OnStoreSave(ctx);

		// write any data (using params) you want to store
		int   a = 5;
		float b = 6.0;

		ctx.Write(a);
		ctx.Write(b);
	}
	@endcode
	*/
	void OnStoreSave(ParamsWriteContext ctx)
	{
		// Saving of energy related states
		if ( m_EM )
		{		
			// Save energy amount
			ctx.Write( m_EM.GetEnergy() );
			
			// Save passive/active state
			ctx.Write( m_EM.IsPassive() );
			
			// Save ON/OFF state
			ctx.Write( m_EM.IsSwitchedOn() );
			
			// Save plugged/unplugged state
			ctx.Write( m_EM.IsPlugged() );
			
			// ENERGY SOURCE
			// Save energy source IDs
			EntityAI energy_source = m_EM.GetEnergySource();
			int b1 = 0;
			int b2 = 0;
			int b3 = 0;
			int b4 = 0;

			if (energy_source)
			{
				energy_source.GetPersistentID(b1, b2, b3, b4);
			}

			ctx.Write( b1 ); // Save energy source block 1
			ctx.Write( b2 ); // Save energy source block 2
			ctx.Write( b3 ); // Save energy source block 3
			ctx.Write( b4 ); // Save energy source block 4
		}
		
		// variable management system
		SaveVariables(ctx);
	}
	
	/**
	\brief Called when data is loaded from persistence (on server side).
	@code

	void OnStoreLoad(ParamsReadContext ctx, int version)
	{
		// dont forget to propagate this call trough class hierarchy!
		if ( !super.OnStoreLoad(ctx, version) )
			return false;

		// read data loaded from game database (format and order of reading must be the same as writing!)
		int a;
		if ( !ctx.Read(a) )
			return false;

		float b;
		if ( !ctx.Read(b) )
			return false;

		return true;
	}
	@endcode
	*/
	
	bool OnStoreLoad (ParamsReadContext ctx, int version)
	{
		// Restoring of energy related states
		if ( m_EM )
		{
			// Load energy amount
			float f_energy = 0;
			if ( !ctx.Read( f_energy ) )
				f_energy = 0;
			m_EM.SetEnergy(f_energy);
			
			// Load passive/active state
			bool b_is_passive = false;
			if ( !ctx.Read( b_is_passive ) )
				return false;
			m_EM.SetPassiveState(b_is_passive);
			
			// Load ON/OFF state
			bool b_is_on = false;
			if ( !ctx.Read( b_is_on ) )
			{
				m_EM.SwitchOn();
				return false;
			}
			
			// Load plugged/unplugged state
			bool b_is_plugged = false;
			if ( !ctx.Read( b_is_plugged ) )
				return false;
			
			// ENERGY SOURCE
			if ( version <= 103 )
			{
				// Load energy source ID low
				int i_energy_source_ID_low = 0; // Even 0 can be valid ID!
				if ( !ctx.Read( i_energy_source_ID_low ) )
					return false;
				
				// Load energy source ID high
				int i_energy_source_ID_high = 0; // Even 0 can be valid ID!
				if ( !ctx.Read( i_energy_source_ID_high ) )
					return false;
			}
			else
			{
				int b1 = 0;
				int b2 = 0;
				int b3 = 0;
				int b4 = 0;

				if ( !ctx.Read(b1) ) return false;
				if ( !ctx.Read(b2) ) return false;
				if ( !ctx.Read(b3) ) return false;
				if ( !ctx.Read(b4) ) return false;

				if ( b_is_plugged )
				{
					// Because function GetEntityByPersitentID() cannot be called here, ID values must be stored and used later.
					m_EM.StoreEnergySourceIDs( b1, b2, b3, b4 );
					m_EM.RestorePlugState(true);
				}
			}

			if (b_is_on)
			{
				m_EM.SwitchOn();
			}
		}
		
		if (version >= 140)
		{
			// variable management system
			if (!LoadVariables(ctx, version))
				return false;
		}
		
		return true;
	}

	//! Sets object synchronization dirty flag, which signalize that object wants to be synchronized (take effect only in MP on server side)
	proto native void SetSynchDirty();

	/**
	\brief Called on clients after receiving synchronization data from server.
	*/
	void OnVariablesSynchronized()
	{
		if ( m_EM )
		{
			if ( GetGame().IsMultiplayer() )
			{
				bool is_on = m_EM.IsSwitchedOn();
				
				if (is_on != m_EM.GetPreviousSwitchState())
				{
					if (is_on)
						m_EM.SwitchOn();
					else
						m_EM.SwitchOff();
				}
				
				int id_low = m_EM.GetEnergySourceNetworkIDLow();
				int id_High = m_EM.GetEnergySourceNetworkIDHigh();
				
				EntityAI energy_source = EntityAI.Cast( GetGame().GetObjectByNetworkId(id_low, id_High) );
				
				if (energy_source)
				{
					ComponentEnergyManager esem = energy_source.GetCompEM();
					
					if ( !esem )
					{
						string object = energy_source.GetType();
						Error("Synchronization error! Object " + object + " has no instance of the Energy Manager component!");
					}
					
					m_EM.PlugThisInto(energy_source);
					
				}
				else
				{
					m_EM.UnplugThis();
				}
				
				m_EM.DeviceUpdate();
				m_EM.StartUpdates();
			}
		}
		
		if (m_IsFrozen != m_IsFrozenLocal && !GetGame().IsDedicatedServer())
		{
			m_IsFrozenLocal = m_IsFrozen;
			OnFreezeStateChangeClient();
		}
	}
	
	//-----------------------------
	// VARIABLE MANIPULATION SYSTEM
	//-----------------------------
	//!'true' if this variable has ever been changed from default
	bool IsVariableSet(int variable)
	{
		return (variable & m_VariablesMask);
	}
	
	void SetVariableMask(int variable)
	{
		m_VariablesMask = variable | m_VariablesMask; 
		if (GetGame().IsServer()) 
		{
			SetSynchDirty();
		}
	}
	
	//!Removes variable from variable mask, making it appear as though the variable has never been changed from default
	void RemoveItemVariable(int variable)
	{
		m_VariablesMask = ~variable & m_VariablesMask;
	}
	
	void TransferVariablesFloat(array<float> float_vars)
	{
		DeSerializeNumericalVars(float_vars);
	}
		
	array<float> GetVariablesFloat()
	{
		CachedObjectsArrays.ARRAY_FLOAT.Clear();
		SerializeNumericalVars(CachedObjectsArrays.ARRAY_FLOAT);
		return CachedObjectsArrays.ARRAY_FLOAT;
	}
	
	void SaveVariables(ParamsWriteContext ctx)
	{
		//first set the flags
		int varFlags = 0;
		
		if (m_VariablesMask)
			varFlags = ItemVariableFlags.FLOAT;
		
		ctx.Write(varFlags);
		//-------------------
		//now serialize the variables
		
		//floats
		if (m_VariablesMask)
			WriteVarsToCTX(ctx);
	}
	
	//----------------------------------------------------------------
	bool LoadVariables(ParamsReadContext ctx, int version = -1)
	{
		int varFlags;
		
		//read the flags
		if (!ctx.Read(varFlags))
		{
			return false;
		}
		
		//--------------
		if (varFlags & ItemVariableFlags.FLOAT)
		{
			if (!ReadVarsFromCTX(ctx, version))
				return false;
		}
		return true;
	}
	
	//! Writes to storage CTX
	void WriteVarsToCTX(ParamsWriteContext ctx)
	{
		ctx.Write(m_VariablesMask);
		
		//--------------------------------------------
		if (IsVariableSet(VARIABLE_TEMPERATURE))
		{
			ctx.Write(GetTemperature());
			ctx.Write((int)GetIsFrozen());
		}
	}
	
	//! Reads from storage CTX
	bool ReadVarsFromCTX(ParamsReadContext ctx, int version = -1)//with ID optimization
	{
		if (version < 140)
			return true;
		
		int intValue;
		float value;			
		
		if (!ctx.Read(intValue))
			return false;
		
		m_VariablesMask = intValue; //necessary for higher implement overrides. Hope it does not bork some init somewhere.
		
		//--------------------------------------------
		if (m_VariablesMask & VARIABLE_TEMPERATURE)
		{
			if (!ctx.Read(value))
				return false;
			SetTemperatureDirect(value);
			
			if (!ctx.Read(intValue))
				return false;
			SetFrozen(intValue);
		}
		
		return true;
	}
	
	void SerializeNumericalVars(array<float> floats_out)
	{
		// the order of serialization must be the same as the order of de-serialization
		floats_out.Insert(m_VariablesMask);
		
		//--------------------------------------------
		if (IsVariableSet(VARIABLE_TEMPERATURE))
		{
			floats_out.Insert(m_VarTemperature);
			floats_out.Insert((float)GetIsFrozen());
			floats_out.Insert((float)GetFreezeThawProgress());
		}
	}
	
	void DeSerializeNumericalVars(array<float> floats)
	{
		// the order of serialization must be the same as the order of de-serialization
		int index = 0;
		int mask = Math.Round(floats.Get(index));
		
		index++;
		//--------------------------------------------
		if (mask & VARIABLE_TEMPERATURE)
		{
			float temperature = floats.Get(index);
			SetTemperatureDirect(temperature);
			floats.RemoveOrdered(index);
			
			bool frozen = Math.Round(floats.Get(index));
			SetFrozen(frozen);
			floats.RemoveOrdered(index);
			
			float FTProgress = floats.Get(index);
			SetFreezeThawProgress(FTProgress);
			floats.RemoveOrdered(index);
		}
	}
	
	//----------------------------------------------------------------
	
	proto native void SetAITargetCallbacks(AbstractAITargetCallbacks callbacks);

	override void EOnFrame(IEntity other, float timeSlice)
	{
		if ( m_ComponentsBank != NULL )
		{
			for ( int comp_key = 0; comp_key < COMP_TYPE_COUNT; ++comp_key )
			{
				if ( m_ComponentsBank.IsComponentAlreadyExist(comp_key) )
				{
					m_ComponentsBank.GetComponent(comp_key).Event_OnFrame(other, timeSlice);
				}
			}
		}
	}
	
	string GetDebugText()
	{
		string text = string.Empty;

		text += "Weight: " + GetWeightEx() + "\n";
		text += "Disabled: " + GetIsSimulationDisabled() + "\n";
		#ifdef SERVER
		if (GetEconomyProfile())
			text += "CE Lifetime default: " + (int)GetEconomyProfile().GetLifetime() + "\n";
		text += "CE Lifetime remaining: " + (int)GetLifetime() + "\n";
		#endif
		
		ComponentEnergyManager compEM = GetCompEM();
		if (compEM)
		{
			text += "Energy Source: " + Object.GetDebugName(compEM.GetEnergySource()) + "\n";
			text += "Switched On: " + compEM.IsSwitchedOn() + "\n";
			text += "Is Working: " + compEM.IsWorking() + "\n";
		}

		return text;
	}
	
	
	void GetDebugButtonNames(out string button1, out string button2, out string button3, out string button4){}//DEPRICATED, USE GetDebugActions / OnAction
	void OnDebugButtonPressClient(int button_index){}//DEPRICATED, USE GetDebugActions / OnAction
	void OnDebugButtonPressServer(int button_index){}//DEPRICATED, USE GetDebugActions / OnAction
	
	
	Shape DebugBBoxDraw()
	{
		return GetComponent(COMP_TYPE_ETITY_DEBUG).DebugBBoxDraw();
	}

	void DebugBBoxSetColor(int color)
	{
		GetComponent(COMP_TYPE_ETITY_DEBUG).DebugBBoxSetColor(color);
	}

	void DebugBBoxDelete()
	{
		GetComponent(COMP_TYPE_ETITY_DEBUG).DebugBBoxDelete();
	}

	Shape DebugDirectionDraw(float distance = 1)
	{
		return GetComponent(COMP_TYPE_ETITY_DEBUG).DebugDirectionDraw(distance);
	}

	void DebugDirectionSetColor(int color)
	{
		GetComponent(COMP_TYPE_ETITY_DEBUG).DebugDirectionSetColor(color);
	}

	void DebugDirectionDelete()
	{
		GetComponent(COMP_TYPE_ETITY_DEBUG).DebugDirectionDelete();
	}

	//! Hides selection of the given name. Must be configed in config.cpp and models.cfg
	void HideSelection( string selection_name )
	{
		if ( !ToDelete() )
		{
			SetAnimationPhase ( selection_name, 1 ); // 1 = hide, 0 = unhide!
		}
	}

	//! Shows selection of the given name. Must be configed in config.hpp and models.cfg
	void ShowSelection( string selection_name )
	{
		if ( !ToDelete() )
		{
			SetAnimationPhase ( selection_name, 0 ); // 1 = hide, 0 = unhide!
		}
	}

	//! Returns blocks of bits of persistence id of this entity.
	//! This id stays the same even after server restart.
	proto void GetPersistentID( out int b1, out int b2, out int b3, out int b4 );

	//! Set (override) remaining economy lifetime (seconds)
	proto native void SetLifetime( float fLifeTime );
	//! Get remaining economy lifetime (seconds)
	proto native float GetLifetime();
	//! Reset economy lifetime to default (seconds)
	proto native void IncreaseLifetime();

	//! Set (override) max economy lifetime per entity instance (seconds)
	proto native void SetLifetimeMax( float fLifeTime );
	//! Get max economy lifetime per instance - default is from DB (seconds)
	proto native float GetLifetimeMax();

	//! Reset economy lifetime to default across entity hierarchy all the way to the topmost entity
	void IncreaseLifetimeUp()
	{
		IncreaseLifetime();
		if (GetHierarchyParent())
			GetHierarchyParent().IncreaseLifetimeUp();
	}	


	// BODY STAGING
	//! Use this to access Body Staging component on dead character. Returns NULL if the given object lacks such component.
	ComponentBodyStaging GetCompBS()
	{
		if ( HasComponent(COMP_TYPE_BODY_STAGING) )
			return ComponentBodyStaging.Cast( GetComponent(COMP_TYPE_BODY_STAGING) );
		return NULL;
	}

	///@{ energy manager
	//! ENERGY MANAGER:Documentation: Confluence >> Camping & Squatting >> Electricity >> Energy Manager functionalities
	//! Use this to access Energy Manager component on your device. Returns NULL if the given object lacks such component.
	ComponentEnergyManager GetCompEM()
	{
		if (m_EM)
			return m_EM;
		
		if ( HasComponent(COMP_TYPE_ENERGY_MANAGER) )
			return ComponentEnergyManager.Cast( GetComponent(COMP_TYPE_ENERGY_MANAGER) );
		return NULL;
	}

	//! If this item has class EnergyManager in its config then it returns true. Otherwise returns false.
	bool HasEnergyManager()
	{
		return HasComponent(COMP_TYPE_ENERGY_MANAGER);
	}

	// ------ Public Events for Energy manager component. Overwrite these and put your own functionality into them. ------

	//! Energy manager event: Called only once when this device starts doing its work
	void OnWorkStart() {}

	//! Energy manager event: Called every device update if its supposed to do some work. The update can be every second or at random, depending on its manipulation.
	void OnWork( float consumed_energy ) {}

	//! Energy manager event: Called when the device stops working (was switched OFF or ran out of energy)
	void OnWorkStop() {}

	//! Energy manager event: Called when the device is switched on
	void OnSwitchOn() {}

	//! Energy manager event: Called when the device is switched OFF
	void OnSwitchOff() {}

	//! Energy manager event: Called when this device is plugged into some energy source
	void OnIsPlugged(EntityAI source_device) {}

	//! Energy manager event: Called when this device is UNPLUGGED from the energy source
	void OnIsUnplugged( EntityAI last_energy_source ) {}

	//! Energy manager event: When something is plugged into this device
	void OnOwnSocketTaken( EntityAI device ) {}

	//! Energy manager event: When something is UNPLUGGED from this device
	void OnOwnSocketReleased( EntityAI device ) {}

	//! Energy manager event: Object's initialization. Energy Manager is fully initialized by this point.
	void OnInitEnergy() {}

	//! Energy manager event: Called when energy was consumed on this device. ALWAYS CALL super.OnEnergyConsumed() !!!
	void OnEnergyConsumed() {}

	//! Energy manager event: Called when energy was added on this device. ALWAYS CALL super.OnEnergyAdded() !!!
	void OnEnergyAdded() {}
	///@} energy manager
	
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);

		if ( GetGame().IsClient() )
		{
			switch (rpc_type)
			{
				// BODY STAGING - server => client synchronization of skinned state.
				case ERPCs.RPC_BS_SKINNED_STATE:
				{
					Param1<bool> p_skinned_state= new Param1<bool>(false);
					if (ctx.Read(p_skinned_state))
					{
						float state = p_skinned_state.param1;
						if (state && GetCompBS())
							GetCompBS().SetAsSkinnedClient();
					}
					break;
				}
				
				case ERPCs.RPC_EXPLODE_EVENT:
				{
					OnExplodeClient();					
					break;
				}
			}
		}
	}

	#ifdef DIAG_DEVELOPER
	void FixEntity()
	{
		if (!(GetGame().IsServer()))
			return;
		SetFullHealth();
		
		if (GetInventory())
		{
			int i = 0;
			int AttachmentsCount = GetInventory().AttachmentCount();
			if (AttachmentsCount > 0)
			{
				for (i = 0; i < AttachmentsCount; i++)
				{
					GetInventory().GetAttachmentFromIndex(i).FixEntity();
				}
			}
		
			CargoBase cargo = GetInventory().GetCargo();
			if (cargo)
			{
				int cargoCount = cargo.GetItemCount();
				for (i = 0; i < cargoCount; i++)
				{
					cargo.GetItem(i).FixEntity();
				}
			}
		}
	}
	#endif

	float GetWetWeightModifier()
	{
		return CfgGameplayHandler.GetWetnessWeightModifiers()[GetWetLevel()];
	}

	float GetConfigWeightModified()
	{
		return m_ConfigWeight * GetWetWeightModifier();	
	}

	#ifdef DEVELOPER
	string GetConfigWeightModifiedDebugText()
	{
		if (WeightDebug.m_VerbosityFlags & WeightDebugType.RECALC_FORCED)
		{
			return "(" + m_ConfigWeight + "(config weight) * " + GetWetWeightModifier() + "(Wetness Modifier))";
		}
		return string.Empty;
	}
	#endif
	

	//Obsolete, use GetWeightEx()
	int GetWeight()
	{
		return GetWeightEx();
	}

	void ClearWeightDirty()
	{
		//Print("ent:" + this + " - ClearWeightDirty");
		m_WeightDirty = 0;
	}

	void SetWeightDirty()
	{
		#ifdef DEVELOPER
		if (WeightDebug.m_VerbosityFlags & WeightDebugType.SET_DIRTY_FLAG)
		{
			Print("---------------------------------------");
			Print("ent:" + this + " - SetWeightDirty");
			if (WeightDebug.m_VerbosityFlags & WeightDebugType.DUMP_STACK)
			{
				DumpStack();
			}
			Print("---------------------------------------");
		}
		#endif
		m_WeightDirty = 1;
		if (GetHierarchyParent())
		{
			GetHierarchyParent().SetWeightDirty();
		}
	}
	// returns weight of all cargo and attachments
	float GetInventoryAndCargoWeight(bool forceRecalc = false)
	{
		float totalWeight;
		if (GetInventory())
		{
			int i = 0;
			int AttachmentsCount = GetInventory().AttachmentCount();
			if (AttachmentsCount > 0)
			{
				for (i = 0; i < AttachmentsCount; i++)
				{
					totalWeight += GetInventory().GetAttachmentFromIndex(i).GetWeightEx(forceRecalc);
				}
			}
		
			CargoBase cargo = GetInventory().GetCargo();
			if (cargo)
			{
				int cargoCount = cargo.GetItemCount();
				for (i = 0; i < cargoCount; i++)
				{
					totalWeight += cargo.GetItem(i).GetWeightEx(forceRecalc);
				}
			}
		}
		return totalWeight;	
	}
	//! returns weight of the entity in a way that's specific to the entity type and is internal to the weight system calculation, to obtain entity's weight, use the 'GetWeightEx' method instead
	protected float GetWeightSpecialized(bool forceRecalc = false)
	{
		return GetInventoryAndCargoWeight(forceRecalc);
	}

	//! returns overall weight of the entity, 'forceRecalc = true' is meant to be used only when debugging, using it in gameplay code is higly inadvisable as it bypasses the weight caching and has adverse effect on performance
	//this method is not meant to be overriden, to adjust weight calculation for specific item type, override 'GetWeightSpecialized(bool forceRecalc = false)' instead
	float GetWeightEx(bool forceRecalc = false)
	{
		if (m_WeightDirty || forceRecalc)//recalculate
		{
			m_WeightEx = GetWeightSpecialized(forceRecalc);
			ClearWeightDirty();
		
			#ifdef DEVELOPER
			if (WeightDebug.m_VerbosityFlags & WeightDebugType.RECALC_FORCED)
			{
				WeightDebug.GetWeightDebug(this).SetWeight(m_WeightEx);
			}
			if (WeightDebug.m_VerbosityFlags & WeightDebugType.RECALC_DIRTY)
			{
				Print("ent:" + this + " - Dirty Recalc");
				if (WeightDebug.m_VerbosityFlags & WeightDebugType.DUMP_STACK)
				{
					DumpStack();
				}
			}
			#endif
		}

		return m_WeightEx;
	}
	
	void UpdateWeight(WeightUpdateType updateType = WeightUpdateType.FULL, float weightAdjustment = 0);
	
	float GetSingleInventoryItemWeightEx(){}

	void GetDebugActions(out TSelectableActionInfoArrayEx outputList)
	{
		//fix entity
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.FIX_ENTITY, "Fix Entity", FadeColors.LIGHT_GREY));
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.SEPARATOR, "___________________________", FadeColors.RED));
	
		//weight
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.GET_TOTAL_WEIGHT, "Print Weight", FadeColors.LIGHT_GREY));
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.GET_TOTAL_WEIGHT_RECALC, "Print Weight Verbose", FadeColors.LIGHT_GREY));
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.GET_PLAYER_WEIGHT, "Print Player Weight", FadeColors.LIGHT_GREY));
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.GET_PLAYER_WEIGHT_RECALC, "Print Player Weight Verbose", FadeColors.LIGHT_GREY));
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.SEPARATOR, "___________________________", FadeColors.RED));
	}

	bool OnAction(int action_id, Man player, ParamsReadContext ctx)
	{
		if (action_id == EActions.FIX_ENTITY)
		{
			#ifdef DIAG_DEVELOPER
			FixEntity();
			#endif
		}
		else if (action_id == EActions.GET_TOTAL_WEIGHT) //Prints total weight of item + its contents
		{
			WeightDebug.ClearWeightDebug();
			#ifndef SERVER
			Debug.Log("======================== "+  GetType() +" =================================");
			#endif
			Debug.Log("Weight:" + GetWeightEx().ToString());
			Debug.Log("Weight excluding cargo and attachments:" + GetSingleInventoryItemWeightEx());
			Debug.Log("----------------------------------------------------------------------------------------------");
		}
		else if (action_id == EActions.GET_TOTAL_WEIGHT_RECALC) //Prints total weight of item + its contents
		{
			WeightDebug.ClearWeightDebug();
			WeightDebug.SetVerbosityFlags(WeightDebugType.RECALC_FORCED);
			#ifndef SERVER
			Debug.Log("======================== "+  GetType() +" RECALC ===========================");
			#endif
			Debug.Log("Weight:" + GetWeightEx(true).ToString());
			Debug.Log("Weight excluding cargo and attachments:" + GetSingleInventoryItemWeightEx());
			WeightDebug.PrintAll(this);
			Debug.Log("----------------------------------------------------------------------------------------------");
			WeightDebug.SetVerbosityFlags(0);
		}
		else if (action_id == EActions.GET_PLAYER_WEIGHT) //Prints total weight of item + its contents
		{
			WeightDebug.ClearWeightDebug();
			#ifndef SERVER
			Debug.Log("======================== PLAYER: "+player+" ===========================");
			#endif
			Debug.Log("New overall weight Player:"+player.GetWeightEx().ToString());

			Debug.Log("----------------------------------------------------------------------------------------------");
		}
		else if (action_id == EActions.GET_PLAYER_WEIGHT_RECALC) //Prints total weight of item + its contents
		{
			WeightDebug.ClearWeightDebug();
			WeightDebug.SetVerbosityFlags(WeightDebugType.RECALC_FORCED);
			#ifndef SERVER
			Debug.Log("======================== PLAYER RECALC: "+player+" ===========================");
			#endif
			Debug.Log("New overall weight Player:"+player.GetWeightEx(true).ToString());
			WeightDebug.PrintAll(player);
			Debug.Log("----------------------------------------------------------------------------------------------");
			WeightDebug.SetVerbosityFlags(0);
		}
		return false;
	}

	///@{ view index
	//! Item view index is used to setup which camera will be used in item view widget in inventory.
	//! With this index you can setup various camera angles for different item states (e.g. fireplace, weapons).
	int m_ViewIndex = 0;
	
	//! Sets item preview index
	void SetViewIndex( int index )
	{
		m_ViewIndex = index;
		
		if( GetGame().IsServer() ) 
		{
			SetSynchDirty();
		}
	}
	
	//! Returns item preview index !!!! IF OVERRIDING with more dynamic events call GetOnViewIndexChanged() in constructor on client !!!!
	int GetViewIndex()
	{
		if ( MemoryPointExists( "invView2" ) )
		{
			#ifdef PLATFORM_WINDOWS
			InventoryLocation il = new InventoryLocation();
			GetInventory().GetCurrentInventoryLocation( il );
			InventoryLocationType type = il.GetType();
			switch ( type )
			{
				case InventoryLocationType.CARGO:
				{
					return 0;
				}
				case InventoryLocationType.ATTACHMENT:
				{
					return 1;
				}
				case InventoryLocationType.HANDS:
				{
					return 0;
				}
				case InventoryLocationType.GROUND:
				{
					return 1;
				}
				case InventoryLocationType.PROXYCARGO:
				{
					return 0;
				}
				default:
				{
					return 0;
				}
			}			
			#endif
			
			#ifdef PLATFORM_CONSOLE
			return 1;
			#endif
		}
		return 0;
	}
	///@} view index
	
	//! Returns hit component for the Entity (overriden for each Type - PlayerBase, DayZInfected, DayZAnimal, etc.)
	string GetHitComponentForAI()
	{
		Debug.LogError("EntityAI: HitComponentForAI not set properly for that entity (" + GetType() + ")");
		//! returns Global so it is obvious you need to configure that properly on entity
		return "";
	}

	//! returns default hit component for the Entity (overriden for each Type - PlayerBase, DayZInfected, DayZAnimal, etc.)
	string GetDefaultHitComponent()
	{
		Debug.LogError("EntityAI: DefaultHitComponent not set properly for that entity (" + GetType() + ")");
		//! returns Global so it is obvious you need to configure that properly on entity
		return "";
	}
	
	//! returns default hit position component name for the Entity (overriden by type if needed - used mainly as support for impact particles)
	string GetDefaultHitPositionComponent()
	{
		Debug.LogError("EntityAI: DefaultHitPositionComponent not set for that entity (" + GetType() + ")");
		return "";
	}
	
	array<string> GetSuitableFinisherHitComponents()
	{
		Debug.LogError("EntityAI: SuitableFinisherHitComponents not set for that entity (" + GetType() + ")");
		return null;
	}
	
	vector GetDefaultHitPosition()
	{
		Debug.LogError("EntityAI: DefaultHitPosition not set for that entity (" + GetType() + ")");
		return vector.Zero;
	}
	
	//! value is related to EMeleeTargetType
	int GetMeleeTargetType()
	{
		return EMeleeTargetType.ALIGNABLE;
	}
	
	//! returns sound type of attachment (used for clothing and weapons on DayZPlayerImplement, paired with Anim*Type enum from DayZAnimEvents)
	string GetAttachmentSoundType()
	{
		return "None";
	}
	
	//! returns item behaviour of item (more in ItemBase)
	bool IsHeavyBehaviour()
	{
		return false;
	}
	
	//! returns item behaviour of item (more in ItemBase)
	bool IsOneHandedBehaviour()
	{
		return false;
	}
	
	//! returns item behaviour of item (more in ItemBase)
	bool IsTwoHandedBehaviour()
	{
		return false;
	}
	
	string ChangeIntoOnAttach(string slot) {}
	string ChangeIntoOnDetach() {}
	
	//! returns true used on selected items that have a temperature effect and can processes temperature changes
	bool CanHaveTemperature()
	{
		return GetTemperatureMin() < GetTemperatureMax();
	}
	
	bool IsSelfAdjustingTemperature()
	{
		return false;
	}
	
	/**
	\brief Central economy calls this function whenever going over all the entities 
	@code
	void OnCEUpdate()
	{
		// dont forget to propagate this call trough class hierarchy! - always at the start of the function
		super.OnCEUpdate();
	
		// use m_ElapsedSinceLastUpdate for time-related purposes
	}
	@endcode
	*/
	void OnCEUpdate()
	{
		float currentTime = GetGame().GetTickTime();
		if (m_LastUpdatedTime == 0)
			m_LastUpdatedTime = currentTime;
		
		m_ElapsedSinceLastUpdate = currentTime - m_LastUpdatedTime; 
		m_LastUpdatedTime = currentTime;
		
		ProcessVariables();
	}
	
	void ProcessVariables()
	{
		//currently only temperature on EntityAI
		if (g_Game.IsWorldWetTempUpdateEnabled())
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
					
					SetTemperatureEx(new TemperatureDataInterpolated(target,ETemperatureAccessTypes.ACCESS_WORLD,m_ElapsedSinceLastUpdate,GameConstants.TEMP_COEF_WORLD,heatPermCoef));
				}
			}
		}
	}
	
	void OnDebugSpawnEx(DebugSpawnParams params)
	{
		OnDebugSpawn();
	}
	
	void OnDebugSpawn() 
	{
		array<string> slots = new array<string>;
		ConfigGetTextArray("Attachments", slots);
		
		array<string> mags = new array<string>;
		ConfigGetTextArray("magazines", mags);
		
		//-------
		
		TStringArray all_paths = new TStringArray;
		
		all_paths.Insert(CFG_VEHICLESPATH);
		all_paths.Insert(CFG_MAGAZINESPATH);
		all_paths.Insert(CFG_WEAPONSPATH);
		
		string config_path;
		string child_name;
		int scope;
		string path;
		int consumable_count;
		
		for (int i = 0; i < all_paths.Count(); i++)
		{
			config_path = all_paths.Get(i);
			int children_count = GetGame().ConfigGetChildrenCount(config_path);
			
			for (int x = 0; x < children_count; x++)
			{
				GetGame().ConfigGetChildName(config_path, x, child_name);
				path = config_path + " " + child_name;
				scope = GetGame().ConfigGetInt( config_path + " " + child_name + " scope" );
				bool should_check = 1;
				if ( config_path == "CfgVehicles" && scope == 0)
				{
					should_check = 0;
				}
				
				if ( should_check )
				{
					string inv_slot;
					GetGame().ConfigGetText( config_path + " " + child_name + " inventorySlot",inv_slot );
					for (int z = 0; z < slots.Count(); z++)
					{
						if (slots.Get(z) == inv_slot)
						{
							this.GetInventory().CreateInInventory( child_name );
							continue;
							//Print("matching attachment: " + child_name + " for inv. slot name:" +inv_slot);
						}
					}
				}
			}
		}
	};
	
	override EntityAI ProcessMeleeItemDamage(int mode = 0)
	{
		if (GetGame().IsServer())
			AddHealth("","Health",-MELEE_ITEM_DAMAGE);
		return this;
	}

	//! Returns liquid throughput coeficient
	float GetLiquidThroughputCoef()
	{
		return LIQUID_THROUGHPUT_DEFAULT;
	}
	
	string GetInvulnerabilityTypeString()
	{
		return "";
	}
	
	void ProcessInvulnerabilityCheck(string servercfg_param)
	{
		if ( GetGame() && GetGame().IsMultiplayer() && GetGame().IsServer() )
		{
			int invulnerability;
			switch (servercfg_param)
			{
				case "disableContainerDamage":
					invulnerability = CfgGameplayHandler.GetDisableContainerDamage();
				break;
				
				case "disableBaseDamage":
					invulnerability = CfgGameplayHandler.GetDisableBaseDamage();
				break;
			}
				
			if (invulnerability > 0)
			{
				SetAllowDamage(false);
			}
		}
	}

	void SetBayonetAttached(bool pState, int slot_idx = -1) {};
	bool HasBayonetAttached() {};
	int GetBayonetAttachmentIdx() {};
	
	void SetButtstockAttached(bool pState, int slot_idx = -1) {};
	bool HasButtstockAttached() {};
	int GetButtstockAttachmentIdx() {};
	
	void SetInvisibleRecursive(bool invisible, EntityAI parent = null, array<int> attachments = null)
	{
		array<int> childrenAtt = new array<int>;
		array<int> attachmentsArray = new array<int>;
		if (attachments)
			attachmentsArray.Copy(attachments);
		else
		{
			for (int i = 0; i < GetInventory().GetAttachmentSlotsCount(); i++)
			{
				attachmentsArray.Insert(GetInventory().GetAttachmentSlotId(i));
			}
		}
		
		EntityAI item;
		
		foreach( int slot : attachmentsArray )
		{
			if( parent )
				item = parent.GetInventory().FindAttachment(slot);
			else
				item = this;//GetInventory().FindAttachment(slot);
			
			if( item )
			{
				if( item.GetInventory().AttachmentCount() > 0 )
				{
					for(i = 0; i < item.GetInventory().GetAttachmentSlotsCount(); i++)
					{
						childrenAtt.Insert(item.GetInventory().GetAttachmentSlotId(i));
					}
					
					SetInvisibleRecursive(invisible,item,childrenAtt);
				}
				
				item.SetInvisible(invisible);
				item.OnInvisibleSet(invisible);
			}
		}
	}
	
	void SoundHardTreeFallingPlay()
	{
		EffectSound sound =	SEffectManager.PlaySound( "hardTreeFall_SoundSet", GetPosition() );
		sound.SetAutodestroy( true );
	}
		
	void SoundSoftTreeFallingPlay()
	{
		EffectSound sound =	SEffectManager.PlaySound( "softTreeFall_SoundSet", GetPosition() );
		sound.SetAutodestroy( true );
	}
		
	void SoundHardBushFallingPlay()
	{
		EffectSound sound =	SEffectManager.PlaySound( "hardBushFall_SoundSet", GetPosition() );
		sound.SetAutodestroy( true );
	}
		
	void SoundSoftBushFallingPlay()
	{
		EffectSound sound =	SEffectManager.PlaySound( "softBushFall_SoundSet", GetPosition() );
		sound.SetAutodestroy( true );
	}
	
	void RegisterTransportHit(Transport transport)
	{
		if (!m_TransportHitRegistered)
		{	
			m_TransportHitRegistered = true; 
			m_TransportHitVelocity = GetVelocity(transport);
			Car car;
			Boat boat;
			float damage;
			vector impulse;
			
			// a different attempt to solve hits from "standing" car to the players
			if (Car.CastTo(car, transport))
			{
				if (car.GetSpeedometerAbsolute() > 2 )
				{
					damage = m_TransportHitVelocity.Length();
					ProcessDirectDamage(DT_CUSTOM, transport, "", "TransportHit", "0 0 0", damage);
				}
				else
				{
					m_TransportHitRegistered = false;
				}

				// compute impulse and apply only if the body dies
				if (IsDamageDestroyed() && car.GetSpeedometerAbsolute() > 3)
				{
					impulse = 40 * m_TransportHitVelocity;
					impulse[1] = 40 * 1.5;
					dBodyApplyImpulse(this, impulse);
				}
			}
			else if (Boat.CastTo(boat, transport))
			{
				Human player = Human.Cast(this);
				if (player && player.PhysicsGetLinkedEntity() == boat)	// standing on boat
				{
					m_TransportHitRegistered = false;
					return;
				} 
					
				if (m_TransportHitVelocity.Normalize() > 5)	// 5 m/s
				{
					damage = m_TransportHitVelocity.Length() * 0.5;
					ProcessDirectDamage(DT_CUSTOM, transport, "", "TransportHit", "0 0 0", damage);
				}
				else
					m_TransportHitRegistered = false;
			}			
			else //old solution just in case if somebody use it
			{
				// avoid damage because of small movements
				if (m_TransportHitVelocity.Length() > 0.1)
				{
					damage = m_TransportHitVelocity.Length();
					ProcessDirectDamage(DT_CUSTOM, transport, "", "TransportHit", "0 0 0", damage);
				}
				else
				{
					m_TransportHitRegistered = false;
				}
				
				// compute impulse and apply only if the body dies
				if (IsDamageDestroyed() && m_TransportHitVelocity.Length() > 0.3)
				{
					impulse = 40 * m_TransportHitVelocity;
					impulse[1] = 40 * 1.5;
					dBodyApplyImpulse(this, impulse);
				}
			}
		}
	}
	
	bool GetInventoryHandAnimation(notnull InventoryLocation loc, out int value)
	{
		value = -1;
		return false;
	}
	
	bool TranslateSlotFromSelection(string selection_name, out int slot_id)
	{
		return false;
	}
	
	//! Universal Temperature Sources Helpers
	bool IsUniversalTemperatureSource()
	{
		return GetUniversalTemperatureSource() != null && GetUniversalTemperatureSource().IsActive();
	}
	
	UTemperatureSource GetUniversalTemperatureSource()
	{
		return m_UniversalTemperatureSource;
	}
	
	void SetUniversalTemperatureSource(UTemperatureSource uts)
	{
		m_UniversalTemperatureSource = uts;
	}
	
	vector GetUniversalTemperatureSourcePosition()
	{
		return GetPosition();
	}

	//! Remotely controlled devices helpers
	RemotelyActivatedItemBehaviour GetRemotelyActivatedItemBehaviour();

	void PairRemote(notnull EntityAI trigger);

	void UnpairRemote();

	EntityAI GetPairDevice();

	void SetPersistentPairID(int id)
	{
		RemotelyActivatedItemBehaviour raib = GetRemotelyActivatedItemBehaviour();
		if (raib)
		{
			raib.SetPersistentPairID(id);
		}
	}

	//! Turnable Valve behaviour
	bool HasTurnableValveBehavior();
	bool IsValveTurnable(int pValveIndex);
	int GetTurnableValveIndex(int pComponentIndex);
	void ExecuteActionsConnectedToValve(int pValveIndex);

//////////////////////////////////
// attachment exclusion section //
//////////////////////////////////
	private void InitAttachmentExclusionValues()
	{
		m_AttachmentExclusionSlotMap = new map<int,ref set<int>>();
		m_AttachmentExclusionMaskGlobal = new set<int>;
		m_AttachmentExclusionMaskChildren = new set<int>();
	
		int count = GetInventory().GetSlotIdCount();
		//no sense in performing inits for something that cannot be attached anywhere (hand/lefthand and some other 'special' slots are the reason for creating 'new' sets above)
		if (count == 0)
			return;
		
		InitInherentSlotExclusionMap();
		InitGlobalExclusionValues();
		InitLegacyConfigExclusionValues();
	}
	
	//! map stored on instance to better respond to various state changes
	private void InitInherentSlotExclusionMap()
	{
		int count = GetInventory().GetSlotIdCount();
		//starting with the INVALID slot, so it is always in the map of attachable items
		SetAttachmentExclusionMaskSlot(InventorySlots.INVALID,GetAttachmentExclusionInitSlotValue(InventorySlots.INVALID));
		
		int slotId;
		for (int i = 0; i < count; i++) 
		{
			slotId = GetInventory().GetSlotId(i);
			SetAttachmentExclusionMaskSlot(slotId,GetAttachmentExclusionInitSlotValue(slotId));
		}
	}
	
	//! override this to modify slot behavior for specific items, or just set 'm_AttachmentExclusionMaskGlobal' value for simple items
	protected set<int> GetAttachmentExclusionInitSlotValue(int slotId)
	{
		set<int> dflt = new set<int>;
		return dflt;
	}
	
	//Initiated last, and only for items that do not have others defined already
	protected void InitLegacyConfigExclusionValues()
	{
		bool performLegacyInit = InitLegacyExclusionCheck();
		
		//adding implicit slot info AFTER the check is performed
		InitLegacySlotExclusionValuesImplicit();
		
		if (performLegacyInit)
			InitLegacySlotExclusionValuesDerived();
	}

	//returns 'false' if the script initialization 
	protected bool InitLegacyExclusionCheck()
	{
		//first check the globals
		if (m_AttachmentExclusionMaskGlobal.Count() > 0)
			return false;
		
		//now the map
		int count = m_AttachmentExclusionSlotMap.Count();
		if (count > 1) //more than InventorySlots.INVALID
		{
			for (int i = 0; i < count; i++)
			{
				int countSet = m_AttachmentExclusionSlotMap.GetElement(i).Count();
				if (countSet > 0) //SOMETHING is defined
				{
					return false;
				}
			}
		}
		
		return true;
	}

	/**@fn		InitLegacySlotExclusionValuesImplicit
	 * @brief	adding base one-directional relations between headgear, masks, eyewear, and headstraps (exception)
	 *
	 * @note: 'InitLegacyConfigExclusionValues' adds them the other way if the item does not have any script-side exclusions AND has some legacy config parameter.
	**/
	protected void InitLegacySlotExclusionValuesImplicit()
	{
		int slotId;
		int slotCount = GetInventory().GetSlotIdCount();
		for (int i = 0; i < slotCount; i++) 
		{
			slotId = GetInventory().GetSlotId(i);
			set<int> tmp;
			switch (slotId)
			{
				case InventorySlots.HEADGEAR:
				{
					tmp = new set<int>;
					tmp.Copy(GetAttachmentExclusionInitSlotValue(slotId));
					tmp.Insert(EAttExclusions.LEGACY_HEADGEAR_MASK);
					tmp.Insert(EAttExclusions.LEGACY_HEADGEAR_HEADSTRAP);
					tmp.Insert(EAttExclusions.LEGACY_HEADGEAR_EYEWEWEAR);
					SetAttachmentExclusionMaskSlot(slotId,tmp);
					break;
				}
				
				case InventorySlots.MASK:
				{
					tmp = new set<int>;
					tmp.Copy(GetAttachmentExclusionInitSlotValue(slotId));
					tmp.Insert(EAttExclusions.LEGACY_MASK_HEADGEAR);
					tmp.Insert(EAttExclusions.LEGACY_MASK_HEADSTRAP);
					tmp.Insert(EAttExclusions.LEGACY_MASK_EYEWEWEAR);
					SetAttachmentExclusionMaskSlot(slotId,tmp);
					break;
				}
				
				case InventorySlots.EYEWEAR:
				{
					tmp = new set<int>;
					tmp.Copy(GetAttachmentExclusionInitSlotValue(slotId));
					if (ConfigGetBool("isStrap"))
					{
						tmp.Insert(EAttExclusions.LEGACY_HEADSTRAP_HEADGEAR);
						tmp.Insert(EAttExclusions.LEGACY_HEADSTRAP_MASK);
					}
					else
					{
						tmp.Insert(EAttExclusions.LEGACY_EYEWEAR_HEADGEAR);
						tmp.Insert(EAttExclusions.LEGACY_EYEWEAR_MASK);
					}
					SetAttachmentExclusionMaskSlot(slotId,tmp);
					break;
				}
			}
		}
	}
	
	protected void InitLegacySlotExclusionValuesDerived()
	{
		int slotId;
		int slotCount = GetInventory().GetSlotIdCount();
		for (int i = 0; i < slotCount; i++) 
		{
			slotId = GetInventory().GetSlotId(i);
			set<int> tmp;
			switch (slotId)
			{
				case InventorySlots.HEADGEAR:
				{
					tmp = new set<int>;
					tmp.Copy(GetAttachmentExclusionMaskSlot(slotId));
					if (ConfigGetBool("noNVStrap"))
					{
						tmp.Insert(EAttExclusions.LEGACY_HEADSTRAP_HEADGEAR);
					}
					if (ConfigGetBool("noMask"))
					{
						tmp.Insert(EAttExclusions.LEGACY_MASK_HEADGEAR);
					}
					if (ConfigGetBool("noEyewear"))
					{
						tmp.Insert(EAttExclusions.LEGACY_EYEWEAR_HEADGEAR);
					}
					SetAttachmentExclusionMaskSlot(slotId,tmp);
					break;
				}
				
				case InventorySlots.MASK:
				{
					tmp = new set<int>;
					tmp.Copy(GetAttachmentExclusionMaskSlot(slotId));
					if (ConfigGetBool("noNVStrap"))
					{
						tmp.Insert(EAttExclusions.LEGACY_HEADSTRAP_MASK);
					}
					if (ConfigGetBool("noHelmet"))
					{
						tmp.Insert(EAttExclusions.LEGACY_HEADGEAR_MASK);
					}
					if (ConfigGetBool("noEyewear"))
					{
						tmp.Insert(EAttExclusions.LEGACY_EYEWEAR_MASK);
					}
					SetAttachmentExclusionMaskSlot(slotId,tmp);
					break;
				}
				
				case InventorySlots.EYEWEAR:
				{
					tmp = new set<int>;
					tmp.Copy(GetAttachmentExclusionMaskSlot(slotId));
					if (ConfigGetBool("isStrap"))
					{
						if (ConfigGetBool("noHelmet"))
						{
							tmp.Insert(EAttExclusions.LEGACY_HEADGEAR_HEADSTRAP);
						}
						if (ConfigGetBool("noMask"))
						{
							tmp.Insert(EAttExclusions.LEGACY_MASK_HEADSTRAP);
						}
					}
					else
					{
						if (ConfigGetBool("noHelmet"))
						{
							tmp.Insert(EAttExclusions.LEGACY_HEADGEAR_EYEWEWEAR);
						}
						if (ConfigGetBool("noMask"))
						{
							tmp.Insert(EAttExclusions.LEGACY_MASK_EYEWEWEAR);
						}
					}
					SetAttachmentExclusionMaskSlot(slotId,tmp);
					break;
				}
			}
		}
	}

	//! override to init part of the mask, independent of slot-specific behavior
	protected void InitGlobalExclusionValues();
	
	//! to help with item staging exclusions
	protected void AddSingleExclusionValueGlobal(EAttExclusions value)
	{
		if (m_AttachmentExclusionMaskGlobal.Find(value) == -1)
			m_AttachmentExclusionMaskGlobal.Insert(value);
	}
	
	//! to help with item staging exclusions
	protected void ClearSingleExclusionValueGlobal(EAttExclusions value)
	{
		int idx = m_AttachmentExclusionMaskGlobal.Find(value);
		if (idx != -1)
			m_AttachmentExclusionMaskGlobal.Remove(idx);
	}
	
	protected void SetAttachmentExclusionMaskGlobal(set<int> values)
	{
		m_AttachmentExclusionMaskGlobal.Clear();
		m_AttachmentExclusionMaskGlobal.Copy(values);
	}
	
	//! sets values for specific slot
	protected void SetAttachmentExclusionMaskSlot(int slotId, set<int> values)
	{
		if (m_AttachmentExclusionSlotMap)
		{
			m_AttachmentExclusionSlotMap.Set(slotId,values);
		}
		else
			ErrorEx("m_AttachmentExclusionSlotMap not available! Fill the 'inventorySlot[]' in the " + this + " config file.");
	}
	
	private void PropagateExclusionValueRecursive(set<int> values, int slotId)
	{
		if (values && values.Count() != 0)
		{
			set<int> passThis;
			InventoryLocation lcn = new InventoryLocation();
			GetInventory().GetCurrentInventoryLocation(lcn);
			if (CheckExclusionAccessPropagation(lcn.GetSlot(), slotId, values, passThis))
			{
				m_AttachmentExclusionMaskChildren.InsertSet(passThis);
				EntityAI parent = GetHierarchyParent();
				if (parent)
					parent.PropagateExclusionValueRecursive(passThis,lcn.GetSlot());
			}
		}
	}
	
	private void ClearExclusionValueRecursive(set<int> values, int slotId)
	{
		if (values && values.Count() != 0)
		{
			set<int> passThis;
			InventoryLocation lcn = new InventoryLocation();
			GetInventory().GetCurrentInventoryLocation(lcn);
			if (CheckExclusionAccessPropagation(lcn.GetSlot(), slotId, values, passThis))
			{
				int count = passThis.Count();
				for (int i = 0; i < count; i++)
				{
					m_AttachmentExclusionMaskChildren.RemoveItem(passThis[i]);
				}
				EntityAI parent = GetHierarchyParent();
				if (parent)
					parent.ClearExclusionValueRecursive(passThis,lcn.GetSlot());
			}
		}
	}
	
	//! Slot-specific, children (attachments), and additional (state etc.) masks combined
	set<int> GetAttachmentExclusionMaskAll(int slotId)
	{
		set<int> values = new set<int>();
		set<int> slotValues = GetAttachmentExclusionMaskSlot(slotId);
		if (slotValues)
			values.InsertSet(slotValues);
		values.InsertSet(m_AttachmentExclusionMaskGlobal);
		values.InsertSet(m_AttachmentExclusionMaskChildren);
		
		return values;
	}
	
	//! Specific slot behavior
	set<int> GetAttachmentExclusionMaskSlot(int slotId)
	{
		return m_AttachmentExclusionSlotMap.Get(slotId);
	}
	
	//! Global mask value, independent of slot-specific behavior!
	set<int> GetAttachmentExclusionMaskGlobal()
	{
		return m_AttachmentExclusionMaskGlobal;
	}
	
	//! Mask value coming from the item's attachments
	set<int> GetAttachmentExclusionMaskChildren()
	{
		return m_AttachmentExclusionMaskChildren;
	}
	
	//! checks if any attachment or item state would interfere with this being attached into a different slot (Headgear -> Mask)
	private bool HasInternalExclusionConflicts(int targetSlot)
	{
		set<int> targetSlotValues = GetAttachmentExclusionMaskSlot(targetSlot);
		if (targetSlotValues) //can be null, if so, no conflict
		{
			set<int> additionalValues = new set<int>(); //NOT slot values
			additionalValues.InsertSet(GetAttachmentExclusionMaskGlobal());
			additionalValues.InsertSet(GetAttachmentExclusionMaskChildren());
			
			if (additionalValues.Count() > 0)
			{
				int countTarget = targetSlotValues.Count();
				for (int i = 0; i < countTarget; i++)
				{
					if (additionalValues.Find(targetSlotValues[i]) != -1)
					{
						return true;
					}
				}
			}
		}
		return false;
	}
	
	//! checks 'this' if the incoming flag is present for the current state (slot behavior and others)
	protected bool IsExclusionFlagPresent(set<int> values)
	{
		int slotId;
		string slotName;
		GetInventory().GetCurrentAttachmentSlotInfo(slotId,slotName); //if currently attached, treat it accordingly
		
		set<int> currentSlotValuesAll = GetAttachmentExclusionMaskAll(slotId);
		int count = values.Count();
		for (int i = 0; i < count; i++)
		{
			if (currentSlotValuesAll.Find(values[i]) != -1)
				return true;
		}
		return false;
	}
	
	//! Gets flag from what is effectively an owner
	protected bool IsExclusionFlagPresentRecursive(set<int> values, int targetSlot)
	{
		if (values && values.Count() != 0)
		{
			InventoryLocation lcn = new InventoryLocation();
			GetInventory().GetCurrentInventoryLocation(lcn);
			EntityAI parent = GetHierarchyParent();
			set<int> passThis;
			if (CheckExclusionAccessCondition(lcn.GetSlot(),targetSlot, values, passThis))
			{
				if (parent && parent != this) //we reached root if false
				{
					return parent.IsExclusionFlagPresentRecursive(passThis,lcn.GetSlot());
				}
			}
			return IsExclusionFlagPresent(passThis);
		}
		
		return false;
	}
	
	//!
	protected bool CheckExclusionAccessCondition(int occupiedSlot, int targetSlot, set<int> value, inout set<int> adjustedValue)
	{
		bool occupiedException = occupiedSlot == InventorySlots.HANDS || occupiedSlot == InventorySlots.SHOULDER || occupiedSlot == InventorySlots.MELEE || occupiedSlot == InventorySlots.LEFTHAND;
		bool targetException = targetSlot == InventorySlots.HANDS || targetSlot == InventorySlots.SHOULDER || targetSlot == InventorySlots.MELEE || targetSlot == InventorySlots.LEFTHAND;
		
		if (occupiedException)
		{
			adjustedValue = value;
			return false;
		}
		
		if (targetException)
		{
			adjustedValue = null;
			return false;
		}
		
		AdjustExclusionAccessCondition(occupiedSlot,targetSlot,value,adjustedValue);
		return adjustedValue.Count() != 0;
	}
	
	//!if we want to filter 
	protected void AdjustExclusionAccessCondition(int occupiedSlot, int testedSlot, set<int> value, inout set<int> adjustedValue)
	{
		adjustedValue = value;
	}

	//! special propagation contition
	protected bool CheckExclusionAccessPropagation(int occupiedSlot, int targetSlot, set<int> value, inout set<int> adjustedValue)
	{
		bool occupiedException = occupiedSlot == InventorySlots.HANDS || occupiedSlot == InventorySlots.SHOULDER || occupiedSlot == InventorySlots.MELEE || occupiedSlot == InventorySlots.LEFTHAND;
		bool targetException = targetSlot == InventorySlots.HANDS || targetSlot == InventorySlots.SHOULDER || targetSlot == InventorySlots.MELEE || targetSlot == InventorySlots.LEFTHAND || targetSlot == InventorySlots.INVALID;
		
		if (targetException)
		{
			adjustedValue = null;
			return false;
		}
		
		AdjustExclusionAccessPropagation(occupiedSlot,targetSlot,value,adjustedValue);
		return adjustedValue.Count() != 0;
	}
	
	//!if we want to filter propagation specifically; DO NOT override unless you know what you are doing.
	protected void AdjustExclusionAccessPropagation(int occupiedSlot, int testedSlot, set<int> value, inout set<int> adjustedValue)
	{
		AdjustExclusionAccessCondition(occupiedSlot,testedSlot,value,adjustedValue);
	}
	
	//! checks specifically for att. exclusion conflicts before att. receive
	bool CheckAttachmentReceiveExclusion(EntityAI attachment, int slotId)
	{
		EntityAI currentAtt = GetInventory().FindAttachment(slotId);
		bool hasInternalConflict = attachment.HasInternalExclusionConflicts(slotId);
		set<int> diff;
		InventoryLocation curLoc = new InventoryLocation();
		if (currentAtt) //probably a swap or same-type swap
		{
			diff = attachment.GetAttachmentExclusionMaskAll(slotId);
			diff.RemoveItems(currentAtt.GetAttachmentExclusionMaskAll(slotId));
			if (diff.Count() == 0)
			{
				return !hasInternalConflict;
			}
			else
			{
				return !hasInternalConflict && !IsExclusionFlagPresentRecursive(diff,slotId);
			}
		}
		else if (attachment.GetInventory().GetCurrentInventoryLocation(curLoc) && curLoc.GetType() == InventoryLocationType.ATTACHMENT)
		{
			EntityAI rootOwner = attachment.GetHierarchyRoot();
			if (rootOwner && rootOwner == this.GetHierarchyRoot()) //attachment within the same exclusion hierarchy context
			{
				diff = attachment.GetAttachmentExclusionMaskAll(slotId);
				diff.RemoveItems(attachment.GetAttachmentExclusionMaskAll(curLoc.GetSlot()));
				if (diff.Count() == 0)
				{
					return !hasInternalConflict;
				}
				else
				{
					return !hasInternalConflict && !IsExclusionFlagPresentRecursive(diff,slotId);
				}
			}
		}
		return !hasInternalConflict && !IsExclusionFlagPresentRecursive(attachment.GetAttachmentExclusionMaskAll(slotId),slotId);
	}

	bool IsManagingArrows()
	{
		return false;
	}

	ArrowManagerBase GetArrowManager()
	{
		return null;
	}

	void SetFromProjectile(ProjectileStoppedInfo info)
	{
	}

	void ClearInventory();
};

#ifdef DEVELOPER
void SetDebugDeveloper_item(Object entity)//without a setter,the place where the setting happens is near impossible to find as way too many hits for "_item" exist
{
	if (entity)
		entity.SetDebugItem();

}
Object _item;//watched item goes here(LCTRL+RMB->Watch)
#endif
