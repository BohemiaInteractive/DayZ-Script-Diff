//extendable!
class LiquidInfo
{
	string m_LiquidClassName;
	string m_LiquidDisplayName;
	int m_LiquidType;
	float m_TemperatureLiquidFreezeThreshold = float.LOWEST;
	float m_TemperatureLiquidThawThreshold = float.LOWEST;
	float m_TemperatureLiquidBoilThreshold = Cooking.LIQUID_BOILING_POINT;
	float m_Flammability;
	
	ref NutritionalProfile m_NutriProfile;
	
	void LiquidInfo(string className, NutritionalProfile profile)
	{
		m_NutriProfile = profile;
		Init(className);
	}
	
	protected void Init(string className)
	{
		string path = "cfgLiquidDefinitions " + className;
		m_LiquidClassName = className;
		g_Game.ConfigGetTextRaw(string.Format("%1 displayName", path), m_LiquidDisplayName);
		g_Game.FormatRawConfigStringKeys(m_LiquidDisplayName);
		m_LiquidType = g_Game.ConfigGetInt(string.Format("%1 type", path));
		if (g_Game.ConfigIsExisting(string.Format("%1 liquidFreezeThreshold", path)))
			m_TemperatureLiquidFreezeThreshold = g_Game.ConfigGetFloat(string.Format("%1 liquidFreezeThreshold", path));
		if (g_Game.ConfigIsExisting(string.Format("%1 liquidThawThreshold", path)))
			m_TemperatureLiquidThawThreshold = g_Game.ConfigGetFloat(string.Format("%1 liquidThawThreshold", path));
		if (g_Game.ConfigIsExisting(string.Format("%1 liquidBoilingThreshold", path)))
			m_TemperatureLiquidBoilThreshold = g_Game.ConfigGetFloat(string.Format("%1 liquidBoilingThreshold", path));
		m_Flammability = g_Game.ConfigGetFloat(string.Format("%1 flammability", path));
	}
}

class Liquid
{
	static ref map<int, ref NutritionalProfile> m_AllLiquidsByType = new map<int, ref NutritionalProfile>; //DEPRECATED!
	static ref map<string, ref NutritionalProfile> m_AllLiquidsByName = new map<string, ref NutritionalProfile>; //DEPRECATED!
	static ref map<int, ref LiquidInfo> m_LiquidInfosByType = new map<int, ref LiquidInfo>;
	static ref map<string, ref LiquidInfo> m_LiquidInfosByName = new map<string, ref LiquidInfo>;
	static bool m_Init = InitAllLiquids();
	
	static string GetLiquidClassname(int liquid_type)
	{
		LiquidInfo info = m_LiquidInfosByType.Get(liquid_type);
		if (info)
		{
			return info.m_LiquidClassName;
		}
		
		return "";
	}
	
	static bool InitAllLiquids()
	{
		if (!g_Game)
			return false;
		
		string cfg_classname = "cfgLiquidDefinitions";
		string property_value = "NULL_PROPERTY";
		int cfg_item_count = g_Game.ConfigGetChildrenCount(cfg_classname);

		for (int i = 0; i < cfg_item_count; i++)
		{
			string liquid_class_name;
			g_Game.ConfigGetChildName(cfg_classname, i, liquid_class_name);
			string liquid_full_path = string.Format("%1 %2",cfg_classname, liquid_class_name);
			int config_liquid_type = g_Game.ConfigGetInt(string.Format("%1 type", liquid_full_path));
			
			NutritionalProfile profile = SetUpNutritionalProfile(config_liquid_type, liquid_class_name);
			LiquidInfo info = new LiquidInfo(liquid_class_name, profile);
			m_LiquidInfosByType.Insert(config_liquid_type, info);
			m_LiquidInfosByName.Insert(liquid_class_name, info);
			
			//legacy stuff
			m_AllLiquidsByType.Insert(config_liquid_type, profile);
			m_AllLiquidsByName.Insert(liquid_class_name, profile);
		}
		return true;
	}
	
	//---------------------------------------------------------------------------------------------------------
	static void Transfer(ItemBase source_ent, ItemBase target_ent, float quantity = -1)
	{
		if (!Liquid.CanTransfer(source_ent, target_ent))
			return;
		
		float source_quantity 	= source_ent.GetQuantity();
		float target_quantity 	= target_ent.GetQuantity();
		float targetCfgWeight 	= target_ent.m_ConfigWeight;
		int source_liquid_type 	= source_ent.GetLiquidType();
		
		float available_capacity = target_ent.GetQuantityMax() - target_quantity;
		float quantity_to_transfer;
		//transfers all
		if (quantity == -1)
		{
			quantity_to_transfer = Math.Clamp(source_quantity,0,available_capacity);
		}
		//transfers exact ammount
		else
		{
			quantity_to_transfer = Math.Clamp(Math.Min(source_quantity,quantity),0,available_capacity);
		}
		
		PluginTransmissionAgents m_mta = PluginTransmissionAgents.Cast(GetPlugin(PluginTransmissionAgents));
		m_mta.TransmitAgents(source_ent, target_ent, AGT_TRANSFER_COPY);
		
		source_ent.AddQuantity(-quantity_to_transfer);
		
		float retultTemp = (source_ent.GetTemperature() * quantity_to_transfer + target_ent.GetTemperature() * (targetCfgWeight + target_quantity)) / (targetCfgWeight + target_quantity + quantity_to_transfer);
		target_ent.SetTemperature(retultTemp);
		
		AffectContainerOnTransfer(target_ent,source_liquid_type,quantity_to_transfer,source_ent.GetTemperature());
		
		Liquid.FillContainer(target_ent,source_liquid_type,quantity_to_transfer);
	}
	
	static bool CanTransfer(ItemBase source_ent, ItemBase target_ent)
	{
		if (!source_ent || !target_ent)
			return false;
		
		Barrel_ColorBase barrelTarget = Barrel_ColorBase.Cast(target_ent);
		Barrel_ColorBase barrelSource = Barrel_ColorBase.Cast(source_ent);
		if ((barrelTarget && !barrelTarget.IsOpen()) || (barrelSource && !barrelSource.IsOpen()))
		{
			return false;
		}
		
		if (source_ent.GetIsFrozen())
		{
			return false;
		}
		
		float source_quantity = source_ent.GetQuantity();
		if (source_quantity <= 0)
		{
			//Debug.Log("source has no quantity", "LiquidTransfer");
			return false;//if there is nothing to transfer
		}
		
		int source_liquid_type 	= source_ent.GetLiquidType();	
		if (source_liquid_type < 1) 
		{
			//Debug.Log("source has some quantity, but does not have a valid liquidType set, liquidType = "+ToString(source_liquid_type), "LiquidTransfer");
			return false;//if source is not a container
		}
		
		if (!CanFillContainer(target_ent,source_liquid_type))
		{
			return false;
		}
		
		return true;
	}
	
	static void FillContainer(ItemBase container, int liquid_type, float amount)
	{
		if (!CanFillContainer(container,liquid_type))
		{
			return;
		}
		//filling
		container.SetLiquidType(liquid_type);
		container.AddQuantity(amount);
	}

	//! Filled from any enviro source (fuel feed, pond, snow...)
	static void FillContainerEnviro(ItemBase container, int liquid_type, float amount, bool inject_agents = true)
	{
		float containerCfgWeight = container.m_ConfigWeight;
		float retultTemp = (GetLiquidTypeEnviroTemperature(liquid_type) * amount + container.GetTemperature() * (containerCfgWeight + container.GetQuantity())) / (container.GetQuantity() + containerCfgWeight + amount);
		container.SetTemperature(retultTemp);
		AffectContainerOnFill(container,liquid_type,amount);
		
		FillContainer(container, TranslateLiquidType(liquid_type), amount);
		
		if (inject_agents)
		{
			PluginTransmissionAgents plugin = PluginTransmissionAgents.Cast(GetPlugin(PluginTransmissionAgents));
			int agtSource;
			switch (liquid_type)
			{
				case LIQUID_FRESHWATER:
				case LIQUID_RIVERWATER:
				case LIQUID_STILLWATER:
					agtSource = AGT_WATER_POND;
					break;
				case LIQUID_SNOW:
					agtSource = AGT_SNOW;
					break;
				case LIQUID_HOTWATER:
					agtSource = AGT_WATER_HOT_SPRING;
					break;
				default:
					agtSource = AGT_NONE;
					break;
			}
			
			plugin.TransmitAgents(NULL, container, agtSource, amount);
		}
	}
	
	//! from enviro source
	static void AffectContainerOnFill(ItemBase container, int liquid_type, float amount)
	{
		container.AffectLiquidContainerOnFill(liquid_type,amount);
	}
	
	static void AffectContainerOnTransfer(ItemBase container, int liquidType, float amount, float sourceLiquidTransfer)
	{
		container.AffectLiquidContainerOnTransfer(liquidType,amount,sourceLiquidTransfer);
	}
	
	static bool IsLiquidDrinkWater(int liquidType)
	{
		if (liquidType & (LIQUID_GROUP_DRINKWATER))
			return true;
		return false;
	}
	
	//! Translates 'administrative' liquid types into liquid types with valid config class
	static int TranslateLiquidType(int liquidType)
	{
		if (IsLiquidDrinkWater(liquidType))
			return LIQUID_WATER;
		else
			return liquidType;
	}
	
	static bool CanFillContainer(ItemBase container, int liquid_type, bool ignore_fullness_check = false)
	{
		if (!container) 
			return false;
		
		bool is_container_full = container.IsFullQuantity();
		
		if (is_container_full && !ignore_fullness_check)
		{
			//Debug.Log("container is full", "LiquidTransfer");
			return false;
			
		}
		int container_mask = container.GetLiquidContainerMask();
		
		if (container_mask == 0)
		{
			//Debug.Log("target is not a container", "LiquidTransfer");
			return false;//if the target liquidContainerType is set to 0
		}
		
		if ((liquid_type & container_mask) == 0)
		{
			//Debug.Log("target liquidContainerType does not support this liquid type", "LiquidTransfer");
			return false;
		}
		
		float container_quantity 	= container.GetQuantity();
		
		int container_liquid_type 	= container.GetLiquidType();
		
		if (container_quantity > 0 && container_liquid_type != liquid_type) 
		{
			//Debug.Log("target is not empty AND is of different liquid type than liquid_type added in", "LiquidTransfer");
			return false;
		}
		return true;
	}
	
	//! Gets liquid temperature from the enviroment
	/*!
		\param[in] liquidType Type of liquid.
	*/
	static float GetLiquidTypeEnviroTemperature(int liquidType)
	{
		float ret = g_Game.GetMission().GetWorldData().GetLiquidTypeEnviroTemperature(liquidType);
		//ret = Math.Max(ret,GetLiquidFreezeThreshold(liquidType));
		
		return ret;
	}
	
	private static string GetLiquidConfigProperty(int liquid_type, string property_name, bool is_nutrition_property = false)
	{
		string cfg_classname = "cfgLiquidDefinitions";
		string property_value = "NULL_PROPERTY";
		if (!g_Game)
			return property_value;
		
		int cfg_item_count = g_Game.ConfigGetChildrenCount(cfg_classname);

		for (int i = 0; i < cfg_item_count; i++)
		{
			string liquid_class_name;
			g_Game.ConfigGetChildName(cfg_classname, i, liquid_class_name);
			string liquid_full_path = string.Format("%1 %2", cfg_classname, liquid_class_name);
			int config_liquid_type = g_Game.ConfigGetInt(string.Format("%1 type", liquid_full_path));
			
			if (config_liquid_type == liquid_type)// found the specific class, now lets extract the values
			{
				if (!is_nutrition_property) 
				{
					g_Game.ConfigGetText(string.Format("%1 %2", liquid_full_path, property_name), property_value);
					return property_value;
				}
				else
				{
					g_Game.ConfigGetText(string.Format("%1 Nutrition %2", liquid_full_path, property_name), property_value);
					return property_value;
				}
			}
		}
		return property_value;
	}
	
	static NutritionalProfile GetNutritionalProfileByType(int liquid_type)
	{
		LiquidInfo info = m_LiquidInfosByType.Get(liquid_type);
		if (info && info.m_NutriProfile)
			return info.m_NutriProfile;
		
		return null;
	}
	
	static NutritionalProfile GetNutritionalProfileByName(string class_name)
	{
		LiquidInfo info = m_LiquidInfosByName.Get(class_name);
		if (info && info.m_NutriProfile)
			return info.m_NutriProfile;
		
		return null;
	}
	
	static NutritionalProfile SetUpNutritionalProfile(int liquid_type, string liquid_class_name)
	{
		NutritionalProfile profile = new NutritionalProfile();
		profile.m_Energy = Liquid.GetLiquidConfigProperty(liquid_type, "energy", true).ToFloat();
		profile.m_NutritionalIndex = Liquid.GetLiquidConfigProperty(liquid_type, "nutritionalIndex", true).ToFloat();
		profile.m_FullnessIndex = Liquid.GetLiquidConfigProperty(liquid_type, "fullnessIndex", true).ToFloat();
		profile.m_WaterContent = Liquid.GetLiquidConfigProperty(liquid_type, "water", true).ToFloat();
		profile.m_Toxicity = Liquid.GetLiquidConfigProperty(liquid_type, "toxicity", true).ToFloat();
		profile.m_Agents = Liquid.GetLiquidConfigProperty(liquid_type, "agents", true).ToInt();
		profile.m_Digestibility = Liquid.GetLiquidConfigProperty(liquid_type, "digestibility", true).ToFloat();
		profile.m_AgentsPerDigest = Liquid.GetLiquidConfigProperty(liquid_type, "agentsPerDigest", true).ToFloat();

		profile.MarkAsLiquid(liquid_type, liquid_class_name);
		return profile;
	}
	
	static int GetAgents(int liquid_type)
	{
		return m_LiquidInfosByType.Get(liquid_type).m_NutriProfile.GetAgents();
	}
	
	static int GetAgentsPerDigest(int liquidType)
	{
		return m_LiquidInfosByType.Get(liquidType).m_NutriProfile.m_AgentsPerDigest;
	}
	
	static float GetToxicity(int liquid_type)
	{
		return m_LiquidInfosByType.Get(liquid_type).m_NutriProfile.GetToxicity();
	}
	
	static float GetWaterContent(int liquid_type)
	{
		return m_LiquidInfosByType.Get(liquid_type).m_NutriProfile.GetWaterContent();
	}
	
	static float GetEnergy(int liquid_type)
	{
		return m_LiquidInfosByType.Get(liquid_type).m_NutriProfile.GetEnergy();
	}
	
	static float GetNutritionalIndex(int liquid_type)
	{
		return m_LiquidInfosByType.Get(liquid_type).m_NutriProfile.GetNutritionalIndex();
	}
	
	static string GetDisplayName(int liquid_type)
	{
		return m_LiquidInfosByType.Get(liquid_type).m_LiquidDisplayName;
	}
	
	static float GetFlammability(int liquid_type)
	{
		return m_LiquidInfosByType.Get(liquid_type).m_Flammability;
	}
	
	static float GetFullness(int liquid_type)
	{
		return m_LiquidInfosByType.Get(liquid_type).m_NutriProfile.GetFullnessIndex();
	}
	
	static float GetDigestibility(int liquid_type)
	{
		return m_LiquidInfosByType.Get(liquid_type).m_NutriProfile.GetDigestibility();
	}
	
	static float GetFreezeThreshold(int liquid_type)
	{
		return m_LiquidInfosByType.Get(liquid_type).m_TemperatureLiquidFreezeThreshold;
	}
	
	static float GetThawThreshold(int liquid_type)
	{
		return m_LiquidInfosByType.Get(liquid_type).m_TemperatureLiquidThawThreshold;
	}
	
	static float GetBoilThreshold(int liquid_type)
	{
		return m_LiquidInfosByType.Get(liquid_type).m_TemperatureLiquidBoilThreshold;
	}
	
///////////////////////////
//deprecated methods below
///////////////////////////
	static string GetName(int liquid_type)
	{
		return Liquid.GetLiquidConfigProperty(liquid_type, "name");
	}
};