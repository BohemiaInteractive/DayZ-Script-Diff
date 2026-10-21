//! static parts info, lives on EntityType level. Instanced data in 'ConstructionPart' class
class ConstructionPartTypeData
{
	protected string 	m_Name;				//un-localized text name that is displayed ingame. Localized via getter method.
	protected string 	m_PartName;			//config class name
	protected string 	m_MainPartName;		//main (parent) config class name
	protected string 	m_PartPath; //raw part path, stored for deferred init
	protected typename 	m_CustomInstanceTypename; //set only for custom types, contains type of custom part instances
	protected int		m_Id;				//a number used for synchronization and persistence purposes, must be unique and withing sync limit (total number of bits in all sync/persistence variables)
	protected int 		m_BuildActionTypeMask; //values from EConstructionTools
	protected int 		m_BuildIndicationTypeMask; //values from EConstructionTools, used for indication only!
	protected int 		m_DismantleActionTypeMask; //values from EConstructionTools
	protected int 		m_MaterialType; //used for determinig effect type (wood vs. metal sounds, for example)
	protected int 		m_HandFBAnimOverride = -1; //override ID of the FB animation for hand construction (tool anims are driven by tools)
	protected bool 		m_IsBase;			//defines if this part is the foundation of the whole construction
	protected bool 		m_IsGate;			//defines part gate state
	protected bool 		m_IsUtilityPart; //! Special helper parts with invalid IDs
	protected bool 		m_TriggersUtilityPartUpdate; //! speficies whether a part state change toggles utility part update step. Maybe default should be 'true'??
	protected bool 		m_DamageHandsOnBuild;
	protected bool 		m_CanPartDecayConfig; //! determined by, in sequence: 'can_decay_general' in class Construction > 'can_decay' on the part itself
	protected float 	m_PartDecayThresholdConfig;
	protected float 	m_PartDecayRateConfig;
	protected ref array<string> m_RequiredPartTypes; //list of part types required by this part
	protected ref array<string> m_ConflictedPartTypes; //list of part types conflicting with this one
	protected ref array<string> m_CollisionData; //Names of min/max memory point selections
	protected ref array<string> m_InsiderBoxPairNames; //Names of min/max pairs for insider box considerations
	protected ref array<string> m_ItemFallingData; //Names of min/max memory point selections
	protected ref array<string> m_EssentialAttachmentSlots; //Names of att. slots that are tied to the constructed part in some way
	protected ref array<string> m_RelatedGUICategoryNames; //listing of related 'GUIInventoryAttachmentsProps' subclasses (part is required to display them, in this iteration)
	protected ref array<ref ConstructionMaterialData> m_MaterialsData;
	
	//rebuilding data below
	protected string m_BuildTextOverride = "";
	protected string m_DismantleTextOverride = "";
	protected ref array<string> m_PhxGeomNames;	//used in 'Handle<operation>Physics' methods instead m_PartName
	protected ref array<int> m_PhxGeomBehaviorInverted;	//dictates behavior of geom selections from 'm_PhxGeomNames'. 0 == matches the phase of the animation, non-zero == inverted
	protected ref array<string> m_ProxyRoadwayNames;	//used in 'Handle<operation>ProxyRoadways' methods instead m_PartName
	protected ref array<int> m_ProxyRoadwayBehaviorInverted;	//dictates behavior of ProxyRoadway from 'm_ProxyRoadwayNames'. 0 == matches the phase of the animation, non-zero == inverted
	protected ref array<string> m_AnimSourceNameOverrides;	//used by 'SetAnimationPhase' instead of m_PartName
	protected ref array<int> m_AnimSourceBehaviorOverridesInverted;	//dictates behavior of animSources from 'm_AnimSourceNameOverrides'. 0 == matches the hide/show state of the animation, non-zero == inverted
	protected ref TConstructionPartVariantArray m_ReqPartVariants;
	protected ref ConstructionPartStaticsSupportData m_StaticsData;
	protected ref ConstructionPartEffectsData m_EffectsData;
	
	void InitPartTypeData(PartTypeInitData initData)
	{
		m_PartPath = initData.m_PartPath;
		
		InitBasicStrings(initData);
		InitRequiredParts();
		InitActionMasks();
		InitPartMaterials();
		InitMaterialType();
		InitConflictedParts();
		InitCollisionData();
		InitInsiderBoxData();
		InitItemFallingData();
		InitPartDecayData();
		InitHandAnimOverride();
		InitEssentialAttachmentSlots();
		InitGuiAttachmentCategoryLinks();
		InitConstructionActionDamage();
		InitUtilityPartInfo();
		InitUtilityBuildData();
		InitRequiredPartVariants();
		InitStaticsSupportPartData();
		InitPartEffectsData();
		InitOptionalOverrides();
		InitCustomPartTypeProperties(initData);
	}
	
	/////////////////
	//Protected Inits
	/////////////////
	protected void InitBasicStrings(PartTypeInitData partTypeData)
	{
		string name = "";
		g_Game.ConfigGetTextRaw(m_PartPath + " name", name);
		g_Game.FormatRawConfigStringKeys(name);
		m_Name = name;
		m_PartName = partTypeData.m_PartName;
		m_MainPartName = partTypeData.m_MainPartName;
		m_Id = g_Game.ConfigGetInt(m_PartPath + " id");
		m_IsBase = g_Game.ConfigGetInt(m_PartPath + " is_base");
		m_IsGate = g_Game.ConfigGetInt(m_PartPath + " is_gate");
	}
	
	protected void InitRequiredParts()
	{
		string cfgPath = m_PartPath + " required_parts";
		m_RequiredPartTypes = new array<string>();
		g_Game.ConfigGetTextArray(cfgPath, m_RequiredPartTypes);
	}
	
	protected void InitActionMasks()
	{
		string path = m_PartPath + " build_action_type";
		if (g_Game.ConfigIsExisting(path))
		{
			m_BuildActionTypeMask = g_Game.ConfigGetInt(path);
			SanitizeBuildIndicationMask();
		}
		
		path = m_PartPath + " dismantle_action_type";
		if (g_Game.ConfigIsExisting(path))
			m_DismantleActionTypeMask = g_Game.ConfigGetInt(path);
	}
	
	protected void InitPartMaterials()
	{
		m_MaterialsData = new array<ref ConstructionMaterialData>();
		
		string path = m_PartPath + " Materials";
		if (g_Game.ConfigIsExisting(path))
		{
			int	count = g_Game.ConfigGetChildrenCount(path);
			for (int i = 0; i < count; ++i)
			{
				string childName;
				g_Game.ConfigGetChildName(path, i, childName);
				
				string matPath = path + " " + childName;
				ConstructionMaterialData dataObj = new ConstructionMaterialData(matPath);
				m_MaterialsData.Insert(dataObj);
			}
		}
	}
	
	protected void InitMaterialType()
	{
		m_MaterialType = ConstructionMaterialType.MATERIAL_NONE;
		string path = m_PartPath + " material_type";
		if (g_Game.ConfigIsExisting(path))
		{
			m_MaterialType = g_Game.ConfigGetInt(path);
		}
	}
	
	protected void InitConflictedParts()
	{
		string path = m_PartPath + " conflicted_parts";
		m_ConflictedPartTypes = new array<string>();
		g_Game.ConfigGetTextArray(path, m_ConflictedPartTypes);
	}
	
	//! collision checks when building, if configured
	protected void InitCollisionData()
	{
		//collision box data
		m_CollisionData = new array<string>();
		string path = m_PartPath + " collision_data";
		if (g_Game.ConfigIsExisting(path))
		{
			g_Game.ConfigGetTextArray(path, m_CollisionData);
		}
	}
	
	//! 'inside' box data to help determine whether the character is inside. Modelspace.
	protected void InitInsiderBoxData()
	{
		//insider box data
		string path = m_PartPath + " insider_minMaxs";
		if (g_Game.ConfigIsExisting(path))
		{
			//read from config and validate
			array<string> tmp = new array<string>();
			g_Game.ConfigGetTextArray(path, tmp);
			
			int count = tmp.Count();
			if (count == 0)
			{
				tmp = null;
				return;
			}
			else if (count % 2 != 0)
			{
				ErrorEx("odd number of insider_minMaxs configured for " + m_PartPath);
				tmp = null;
				return;
			}
			
			m_InsiderBoxPairNames = new array<string>();
			m_InsiderBoxPairNames.Copy(tmp);
		}
	}
	
	//! item falling on dismantle/destruct, if configured.
	protected void InitItemFallingData()
	{
		m_ItemFallingData = new array<string>();
		string path = m_PartPath + " item_falling_data";
		if (g_Game.ConfigIsExisting(path))
		{
			g_Game.ConfigGetTextArray(path, m_ItemFallingData);
		}
	}
	
	//
	protected void InitPartDecayData()
	{
		//can decay
		string path = m_PartPath + " can_part_decay";
		if (g_Game.ConfigIsExisting(path))
			SetCanDecayConfig(g_Game.ConfigGetInt(path));
		
		//decay threshold
		m_PartDecayThresholdConfig = ConstructionBase.DEFAULT_DECAY_THRESHOLD;
		path = m_PartPath + " part_decay_threshold";
		if (g_Game.ConfigIsExisting(path))
			m_PartDecayThresholdConfig = g_Game.ConfigGetFloat(path);
		m_PartDecayThresholdConfig = Math.Clamp(m_PartDecayThresholdConfig, 0.0, 1.0);
		
		//decay rate
		m_PartDecayRateConfig = ConstructionBase.DEFAULT_DECAY_RATE;
		path = m_PartPath + " part_decay_rate";
		if (g_Game.ConfigIsExisting(path))
			m_PartDecayRateConfig = g_Game.ConfigGetFloat(path);
	}
	
	protected void InitHandAnimOverride()
	{
		string path = m_PartPath + " hand_build_FB_anim_override";
		if (g_Game.ConfigIsExisting(path))
			m_HandFBAnimOverride = g_Game.ConfigGetInt(path);
	}
	
	//! attachment slots somehow considered 'essential' to the part
	protected void InitEssentialAttachmentSlots()
	{
		m_EssentialAttachmentSlots = new array<string>();
		string path = m_PartPath + " essential_attachments";
		if (g_Game.ConfigIsExisting(path))
		{
			g_Game.ConfigGetTextArray(path, m_EssentialAttachmentSlots);
		}
	}
	
	//! which 'GUIInventoryAttachmentsProps' subclasses it relates to
	protected void InitGuiAttachmentCategoryLinks()
	{
		m_RelatedGUICategoryNames = new array<string>();
		string path = m_PartPath + " related_GUI_categories";
		if (g_Game.ConfigIsExisting(path))
		{
			g_Game.ConfigGetTextArray(path, m_RelatedGUICategoryNames);
		}
	}
	
	//! if hands get damaged on build (and possibly other overrides later)
	protected void InitConstructionActionDamage()
	{
		m_DamageHandsOnBuild = true;
		string path = m_PartPath + " damage_hands_on_build";
		if (g_Game.ConfigIsExisting(path))
		{
			m_DamageHandsOnBuild = g_Game.ConfigGetInt(path);
		}
	}
	
	protected void InitUtilityPartInfo()
	{
		string path = m_PartPath + " is_utility";
		m_IsUtilityPart = g_Game.ConfigGetInt(path);
	}
	
	//! not all parts need to invoke utility update step, config needs to specify
	protected void InitUtilityBuildData()
	{
		string path = m_PartPath + " toggle_utility_part_update";
		if (g_Game.ConfigIsExisting(path))
		{
			m_TriggersUtilityPartUpdate = g_Game.ConfigGetInt(path);
		}
	}
	
	protected void InitRequiredPartVariants()
	{
		string path = m_PartPath + " RequiredPartsVariants";
		if (g_Game.ConfigIsExisting(path))
		{
			int count = g_Game.ConfigGetChildrenCount(path);
			if (count > 0)
			{
				if (!m_ReqPartVariants)
				{
					m_ReqPartVariants = new TConstructionPartVariantArray();
					
					for (int i = 0; i < count; ++i)
					{
						string variantPath = "";
						g_Game.ConfigGetChildName(path, i, variantPath);
						variantPath = path + " " + variantPath + " partVariants";
						array<string> tmp = new array<string>();
						g_Game.ConfigGetTextArray(variantPath, tmp);
						m_ReqPartVariants.Insert(tmp);
					}
				}
			}
		}
	}
	
	protected void InitStaticsSupportPartData()
	{
		m_StaticsData = new ConstructionPartStaticsSupportData();
		m_StaticsData.Init(m_PartPath);
	}
	
	//! 'partPath' not necessary here, using part data directly, override as needed on unique part handlers
	protected void InitPartEffectsData()
	{
		m_EffectsData = new ConstructionPartEffectsData();
		m_EffectsData.Init(m_PartPath, this);
	}
	
	protected void InitOptionalOverrides()
	{
		//strings
		string path = m_PartPath + " build_text_override";
		string tmpString = "";
		
		if (g_Game.ConfigIsExisting(path))
		{
			g_Game.ConfigGetTextRaw(path, tmpString);
			g_Game.FormatRawConfigStringKeys(tmpString);
		}
		m_BuildTextOverride = tmpString;
		
		path = m_PartPath + " dismantle_text_override";
		tmpString = "";
		if (g_Game.ConfigIsExisting(path))
		{
			g_Game.ConfigGetTextRaw(path, tmpString);
			g_Game.FormatRawConfigStringKeys(tmpString);
		}
		m_DismantleTextOverride = tmpString;
		/////////////////////////////////////////////////////////////
		//arrays
		//geoms
		path = m_PartPath + " physics_geom_names";
		m_PhxGeomNames = {};
		if (g_Game.ConfigIsExisting(path))
		{
			g_Game.ConfigGetTextArray(path, m_PhxGeomNames);
		}
		
		path = m_PartPath + " physics_geom_behaviors_invert";
		m_PhxGeomBehaviorInverted = {};
		if (g_Game.ConfigIsExisting(path))
		{
			g_Game.ConfigGetIntArray(path, m_PhxGeomBehaviorInverted);
		}
		
		//sanity check, just in case
		if (m_PhxGeomNames.Count() != m_PhxGeomBehaviorInverted.Count())
		{
			m_PhxGeomNames.Clear();
			m_PhxGeomBehaviorInverted.Clear();
			ErrorEx("'physics_geom_names' and 'physics_geom_behaviors_invert' counts do not match for part " + m_PartName);
		}
		/////////////////////////////////////////////////////////////
		//animSources
		path = m_PartPath + " override_animsource_names";
		m_AnimSourceNameOverrides = {};
		if (g_Game.ConfigIsExisting(path))
		{
			g_Game.ConfigGetTextArray(path, m_AnimSourceNameOverrides);
		}
		
		path = m_PartPath + " override_animsource_behaviors_invert";
		m_AnimSourceBehaviorOverridesInverted = {};
		if (g_Game.ConfigIsExisting(path))
		{
			g_Game.ConfigGetIntArray(path, m_AnimSourceBehaviorOverridesInverted);
		}
		
		//sanity check, just in case
		if (m_AnimSourceNameOverrides.Count() != m_AnimSourceBehaviorOverridesInverted.Count())
		{
			m_AnimSourceNameOverrides.Clear();
			m_AnimSourceBehaviorOverridesInverted.Clear();
		}
		/////////////////////////////////////////////////////////////
		//proxy roadways
		path = m_PartPath + " proxyroadway_names";
		m_ProxyRoadwayNames = {};
		if (g_Game.ConfigIsExisting(path))
		{
			g_Game.ConfigGetTextArray(path, m_ProxyRoadwayNames);
		}
		
		path = m_PartPath + " proxyroadway_behaviors_invert";
		m_ProxyRoadwayBehaviorInverted = {};
		if (g_Game.ConfigIsExisting(path))
		{
			g_Game.ConfigGetIntArray(path, m_ProxyRoadwayBehaviorInverted);
		}
		
		//sanity check, just in case
		if (m_ProxyRoadwayNames.Count() != m_ProxyRoadwayBehaviorInverted.Count())
		{
			m_ProxyRoadwayNames.Clear();
			m_ProxyRoadwayBehaviorInverted.Clear();
			ErrorEx("'proxyroadway_names' and 'proxyroadway_behaviors_invert' counts do not match for part " + m_PartName);
		}
	}
	
	//! Generic init method for custom part type properties
	protected void InitCustomPartTypeProperties(PartTypeInitData initData);
	
	//////////////////////////////
	//setters, use with caution!//
	//////////////////////////////
	
	void SetCanDecayConfig(bool can)
	{
		m_CanPartDecayConfig = can;
	}
	
	void SetCustomInstanceType(string name)
	{
		if (!m_CustomInstanceTypename)
			m_CustomInstanceTypename = name.ToType();
	}
	
	///////////
	//Getters//
	///////////
	string GetPartPath()
	{
		return m_PartPath;
	}
	
	string GetNameLocalized()
	{
		return Widget.TranslateString(m_Name);
	}
	
	string GetPartName()
	{
		return m_PartName;
	}

	string GetMainPartName()
	{
		return m_MainPartName;
	}
	
	int GetId()
	{
		return m_Id;
	}
	
	bool DamageToolOnBuild()
	{
		return true;
	}
	
	bool DamageHandsOnBuild()
	{
		return m_DamageHandsOnBuild;
	}
	
	bool DamageToolOnDeconstruct()
	{
		return true;
	}
	
	bool DamageHandsOnDeconstruct()
	{
		return true;
	}
	
	bool IsUtilityPart()
	{
		return m_IsUtilityPart;
	}
	
	bool TriggersUtilityPartsUpdate()
	{
		return m_TriggersUtilityPartUpdate;
	}
	
	bool CanPartDecayConfig()
	{
		return m_CanPartDecayConfig;
	}
	
	typename GetCustomInstanceType()
	{
		return m_CustomInstanceTypename;
	}
	
	float GetPartDecayRateConfig()
	{
		return m_PartDecayRateConfig;
	}
	
	float GetPartDecayThresholdConfig()
	{
		return m_PartDecayThresholdConfig;
	}
	
	int GetHandBuildFBAnimOverride()
	{
		return m_HandFBAnimOverride;
	}
	
	bool CanSimulateItemFalling()
	{
		return m_ItemFallingData.Count() == 2;
	}
	
	bool HasEssentialAttachmentSlots()
	{
		return m_EssentialAttachmentSlots.Count() > 0;
	}
	
	bool HasRelatedGUICategories()
	{
		return m_RelatedGUICategoryNames.Count() > 0;
	}
	
	bool IsBase()
	{
		return m_IsBase;
	}
	
	bool IsGate()
	{
		return m_IsGate;
	}
	
	array<string> GetRequiredPartTypes()
	{
		return m_RequiredPartTypes;
	}
	
	array<string> GetConflictedPartTypes()
	{
		return m_ConflictedPartTypes;
	}
	
	//! strings of min/max selections (should be single points in memory LOD)
	array<string> GetCollisionData()
	{
		return m_CollisionData;
	}
	
	array<string> GetInsiderBoxPairNames()
	{
		return m_InsiderBoxPairNames;
	}
	
	//! strings of min/max selections (should be single points in memory LOD)
	array<string> GetItemFallingData()
	{
		return m_ItemFallingData;
	}
	
	array<string> GetEssentialAttachmentSlots()
	{
		return m_EssentialAttachmentSlots;
	}
	
	array<string> GetRelatedGUICategoryNames()
	{
		return m_RelatedGUICategoryNames;
	}
	
	string GetBuildTextOverride()
	{
		return Widget.TranslateString(m_BuildTextOverride);
	}
	
	string GetDismantleTextOverride()
	{
		return Widget.TranslateString(m_DismantleTextOverride);
	}
	
	//! returns AnimSource name to use in 'SetAnimationPhase'
	string GetPartAnimSourceName()
	{
		return m_PartName;
	}
	
	//! returns bone name to use in 'ProxyPhysics' methods (translated to BoneIdx down the line)
	string GetPartBoneName()
	{
		return m_PartName;
	}
	
	//! rough material type, used for effect playback
	int GetMaterialType()
	{
		return m_MaterialType;
	}
	
	int GetBuildActionTypeMask()
	{
		return m_BuildActionTypeMask;
	}
	
	int GetBuildIndicationTypeMask()
	{
		return m_BuildIndicationTypeMask;
	}
	
	int GetDismantleActionTypeMask()
	{
		return m_DismantleActionTypeMask;
	}
	
	array<ref ConstructionMaterialData> GetMaterialsData()
	{
		return m_MaterialsData;
	}
	
	bool IsReversible()
	{
		return false;
	}
	
	TConstructionPartVariantArray GetRequiredPartsTypeVariantArray()
	{
		return m_ReqPartVariants;
	}
	
	ConstructionPartStaticsSupportData GetPartTypeStaticsSupportData()
	{
		return m_StaticsData;
	}
	
	bool GetPartTypeSupportProvided(int category, out int providedStrength)
	{
		return m_StaticsData.GetSupportProvided(category, providedStrength);
	}
	
	bool GetPartTypeSupportRequirement(int category, out int requiredStrength)
	{
		return m_StaticsData.GetSupportRequirement(category, requiredStrength);
	}
	
	set<int> GetPartTypeConfiguredSupportCategories()
	{
		return m_StaticsData.GetConfiguredSupportCategories();
	}
	
	array<string> GetPartTypeAnimSourceNameOverrides()
	{
		return m_AnimSourceNameOverrides;
	}
	
	array<int> GetPartTypeAnimSourceBehaviorInverts()
	{
		return m_AnimSourceBehaviorOverridesInverted;
	}
	
	array<string> GetPartTypePhxGeomNames()
	{
		return m_PhxGeomNames;
	}
	
	array<int> GetPartTypePhxGeomBehaviorInverts()
	{
		return m_PhxGeomBehaviorInverted;
	}
	
	array<string> GetPartTypeProxyRoadwayNames()
	{
		return m_ProxyRoadwayNames;
	}
	
	//! Note, this will likely change after release, take care when modding
	array<int> GetPartTypeProxyRoadwayBehaviorInverts()
	{
		return m_ProxyRoadwayBehaviorInverted;
	}
	
	ConstructionPartEffectsData GetPartTypeEffectsData()
	{
		return m_EffectsData;
	}
	
	///////////
	//Utility//
	///////////
	
	//! Solves for category icon overlaps (in vanilla configuration)
	protected void SanitizeBuildIndicationMask()
	{
		m_BuildIndicationTypeMask = m_BuildActionTypeMask;
		
		if (m_BuildIndicationTypeMask & EConstructionTools.TOOL_NAIL && m_BuildIndicationTypeMask & EConstructionTools.TOOL_MOUNT_LOG)
			m_BuildIndicationTypeMask &= ~EConstructionTools.TOOL_MOUNT_LOG;
		
		if (m_BuildIndicationTypeMask & EConstructionTools.TOOL_DIG_BASE && m_BuildIndicationTypeMask & EConstructionTools.TOOL_EXCAVATION)
			m_BuildIndicationTypeMask &= ~EConstructionTools.TOOL_EXCAVATION;
	}
	
	sealed protected void ErrorConfigUndefined(string undefinedParam, string path)
	{
		ErrorEx("'" + undefinedParam + "'parameter undefined for " + path);
	}
}