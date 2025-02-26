class StomachItem
{
	ref NutritionalProfile m_Profile;
	float m_Amount;
	int m_FoodStage;
	//bool m_IsLiquid;
	string m_ClassName;
	int m_Agents;
	protected float m_Temperature;
	
	void StomachItem(string class_name, float amount, NutritionalProfile profile,int foodstage, int agents, float temperature)
	{
		m_Amount = amount;
		m_Profile = profile;
		//m_IsLiquid = is_liquid;
		m_FoodStage = foodstage;
		m_ClassName = class_name;
		m_Agents = agents;
		m_Temperature = temperature;
	}
	
	string GetClassName()
	{
		return m_ClassName;
	}
	
	/*
	void SetLiquid(bool is_liquid)
	{
		m_IsLiquid = is_liquid;
	}
	
	bool IsLiquid()
	{
		return m_IsLiquid;
	}*/
		
	int GetFoodStage()
	{
		return m_FoodStage;
	}
	
	void SetFoodStage(int food_stage)
	{
		m_FoodStage = food_stage;
	}
	
	// returns grams or ml's of this item/liquid
	float GetAmount()
	{
		return m_Amount;
	}
	
	// adds grams or ml's of this item/liquid
	void AddAmount(float amount)
	{
		m_Amount += amount;
	}
	
	void AddAgents(int agents)
	{
		m_Agents = m_Agents | agents;
	}
	
	float GetTemperature()
	{
		return m_Temperature;
	}
	
	// adjust temperature based on the fraction of the new amount compared to the original amount
	void AddTemperature(float temperature, float fraction)
	{
		float currentTempFraction = (1 - 1/fraction) * m_Temperature;
		float newTempFraction = (1/fraction) * temperature;
		
		m_Temperature = currentTempFraction + newTempFraction;
	}
	
	
	// "digests" given amount, "outs" nutritional components, and returns 'true' if leads to complete item digestion(no amount left)
	bool ProcessDigestion(float digestion_points, out float water, out float energy, out float toxicity, out float volume, out int agents, out float consumed_amount)
	{
		agents = m_Agents;
		consumed_amount = GetNutritions(digestion_points, m_Profile, water, energy, toxicity);
		m_Amount -= consumed_amount;
		volume = m_Profile.GetFullnessIndex() * m_Amount;
		return(m_Amount < 0.001);
	}
	
	float GetNutritions(float digestion_points, NutritionalProfile profile, out float water, out float energy, out float toxicity)
	{
		float energy_per_unit = profile.GetEnergy() / 100;
		float water_per_unit = profile.GetWaterContent() / 100;
		float toxicity_per_unit = profile.GetToxicity();
		float digestability = profile.GetDigestibility();
		
		if (digestability == 0)//if undefined
		{
			digestability = 1;
		}
		
		float consumed_quantity = digestion_points * digestability;
		
		if (m_Amount < consumed_quantity)
		{
			consumed_quantity = m_Amount;
		}
		if (consumed_quantity > 0)
		{
			water = consumed_quantity * water_per_unit;
			energy = consumed_quantity * energy_per_unit;
			toxicity = consumed_quantity * toxicity_per_unit;
		}
		return consumed_quantity;
	}
}

class PlayerStomach
{
	const int DIGESTING_WATER = 1;
	const int DIGESTING_ENERGY = 2;
	
	const int quantity_bit_offset = 16;
	const int id_bit_offset = 4;//based on food stage count(+1 for safe measure)
	static int CHECKSUM;
	const float DIGESTION_POINTS = PlayerConstants.DIGESTION_SPEED;
	const int ACCEPTABLE_QUANTITY_MAX = 32768;
	const int ACCEPTABLE_FOODSTAGE_MAX = FoodStageType.COUNT - 1;
	static ref map<string, int> m_NamesToIDs = new map<string, int>;
	static ref map<int, string> m_IDsToNames = new map<int, string>;
	static const bool m_InitData = PlayerStomach.InitData();
	ref array<ref StomachItem> m_StomachContents = new array<ref StomachItem>;
	int m_AgentTransferFilter;//bit mask that prevents agents set in the mask from passing to the player
	bool 	m_Digesting;
	int 	m_DigestingType;
	PlayerBase m_Player;
	float m_StomachVolume;
	protected float m_StomachTemperature;
	const int STORAGE_VERSION = 106;
	
		
	void PlayerStomach(PlayerBase player)
	{
		m_Player = player;
	}

	
	float GetStomachVolume()
	{
		return m_StomachVolume;
	}
	
	float GetStomachTemperature()
	{
		return m_StomachTemperature;
	}
	
	void ClearContents()
	{
		m_StomachContents.Clear();
		m_StomachVolume = 0.0;
		m_StomachTemperature = 0;
	}
	
	// Reduce content of the stomach by provided percentage
	void ReduceContents(float percent)
	{
		percent = Math.Clamp(percent, 0, 100);
		float reduction = percent * 0.01;
		
		foreach (StomachItem item : m_StomachContents)
		{
			item.AddAmount( -(item.GetAmount() * reduction) );
		}
		
		m_StomachVolume -= m_StomachVolume * reduction;
	}
	
	void SetAgentTransferFilter(int filter_agents)
	{
		m_AgentTransferFilter;
	}
	
	int GetAgentTransferFilter()
	{
		return m_AgentTransferFilter;
	}
	
	static void RegisterItem(string classname, int id)
	{
		int hash = classname.Hash();
		CHECKSUM = CHECKSUM^hash; //xor hash vs current checksum
		m_NamesToIDs.Insert(classname, id);
		m_IDsToNames.Insert(id, classname);
	}
	
	static string GetClassnameFromID(int id)
	{
		return m_IDsToNames.Get(id);
	}
	
	static int GetIDFromClassname(string name)
	{
		if (!m_NamesToIDs.Contains(name))
			return -1;
		return m_NamesToIDs.Get(name);
	}
	
	static bool InitData()
	{
		TStringArray all_paths = new TStringArray;
		
		all_paths.Insert("CfgVehicles");
		all_paths.Insert("cfgLiquidDefinitions");
		
		string config_path;
		string child_name;
		int scope;
		string path;
		int consumable_count;
		for(int i = 0; i < all_paths.Count(); i++)
		{
			config_path = all_paths.Get(i);
			int children_count = GetGame().ConfigGetChildrenCount(config_path);
			
			for(int x = 0; x < children_count; x++)
			{
				GetGame().ConfigGetChildName(config_path, x, child_name);
				path = config_path + " " + child_name;
				scope = GetGame().ConfigGetInt(config_path + " " + child_name + " scope");
				bool should_check = 1;
				if (config_path == "CfgVehicles" && scope == 0)
				{
					should_check = 0;
				}
				
				if (should_check)
				{
					bool has_nutrition = GetGame().ConfigIsExisting(path + " Nutrition");
					bool has_stages = GetGame().ConfigIsExisting(path + " Food");
					
					if (has_nutrition || has_stages)
					{
						//Print("child name:" + child_name);
						RegisterItem(child_name, consumable_count);//consumable_count value serves as an unique ID for each item
						consumable_count++;
					}
				}
			}
		}
		//Print("consumable_count " + consumable_count);
		return true;
	}
	
	int GetStorageVersion()
	{
		return STORAGE_VERSION;
	}
	
	bool IsDigesting()
	{
		return (m_DigestingType != 0);
	}
	
	int GetDigestingType()
	{
		return m_DigestingType;
	}
	
	protected void UpdateStomachTemperature()
	{
		m_StomachTemperature = 0;
		
		int stomachItemsCount = m_StomachContents.Count();
		if (stomachItemsCount == 0)
			return;
		
		StomachItem item;
	
		for (int i = 0; i < stomachItemsCount; i++)
		{
			item = m_StomachContents[i];
			m_StomachTemperature += item.GetTemperature();
		}
		
		m_StomachTemperature = m_StomachTemperature / stomachItemsCount;
	}
	
	void Update(float delta_time)
	{
		ProcessNutrients(delta_time);
	}
	
	void ProcessNutrients(float delta_time)
	{
		m_DigestingType = 0;
		
		StomachItem item;
		int stomachItemsCount = m_StomachContents.Count();
		if (stomachItemsCount == 0) 
			return;
		
		float digestionPointsPerItem = (DIGESTION_POINTS / stomachItemsCount) * delta_time;
		m_StomachVolume = 0;//reset, it's accumulated with each pass
		for (int i = stomachItemsCount - 1; i >= 0; i--)
		{
			item = m_StomachContents[i];
			float water, energy, toxicity, volume, consumedAmount;
			int agents;

			if (item.ProcessDigestion(digestionPointsPerItem, water, energy, toxicity, volume, agents, consumedAmount))
			{
				m_StomachContents.Remove(i);
				UpdateStomachTemperature();
			}

			m_StomachVolume += volume;
			m_Player.GetStatEnergy().Add(energy);
			m_Player.GetStatWater().Add(water);
			
			if (energy > 0)
				m_DigestingType |= DIGESTING_ENERGY;
			
			if (water > 0)
				m_DigestingType |= DIGESTING_WATER;
			
			//! try amount from nutritions/food stage first
			float amountOfAgents = item.m_Profile.m_AgentsPerDigest;
			if (amountOfAgents == 0)
				amountOfAgents = consumedAmount;
			
			//! food poisoning specific override by bloody hands
			if ((item.m_Agents & eAgents.FOOD_POISON) == eAgents.FOOD_POISON && m_Player.HasBloodyHands())
				amountOfAgents = Math.Max(amountOfAgents, PlayerConstants.BLOODY_HANDS_FOOD_POISON_AGENT_INCREMENT);

			DigestAgents(agents, amountOfAgents);
		}
	}
	
	void DigestAgents(int agents, float quantity)
	{
		if (!agents)
			return;

		agents = agents & (~m_AgentTransferFilter);//filter against the agent filter mask
		int highestBit = Math.Log2(agents) + 1;
		for (int i = 0; i < highestBit; ++i)
		{
			int agent = (1 << i)& agents;
			if (agent)
			{
				float rndPct = Math.RandomFloatInclusive(PlayerConstants.STOMACH_DIGEST_AGENT_RANDOM_MIN, PlayerConstants.STOMACH_DIGEST_AGENT_RANDOM_MAX);
				if (rndPct != 0)
					quantity += quantity * rndPct;
				m_Player.m_AgentPool.DigestAgent(agent, quantity);
			}
		}
	}

	float GetVolumeContainingAgent(eAgents agent)
	{
		float amountByAgent = 0.0;
		foreach (StomachItem item : m_StomachContents)
		{
			if ((item.m_Agents & agent) == agent)
				amountByAgent += item.m_Amount;
		}
		
		return amountByAgent;
	}

	float GetVolumeContainingAgent01(eAgents agent)
	{
		float amount = GetVolumeContainingAgent(agent);
		if (amount > 0.0)
			return Math.InverseLerp(0.0, GetStomachVolume(), amount);
		
		return 0.0;
	}

	void PrintUpdate()
	{
		Print("================================");
		for(int i = 0; i < m_StomachContents.Count(); i++)
		{
			string itemname = m_StomachContents.Get(i).m_ClassName;
			float amount = m_StomachContents.Get(i).GetAmount();
			int food_stage = m_StomachContents.Get(i).GetFoodStage();
			int agents = m_StomachContents.Get(i).m_Agents;
			Print(">");
			Print("itemname:" + itemname);
			Print("amount:" + amount);
			Print("food_stage:" + food_stage);
			Print("agents:" + agents);
			Print(">");
		}
		Print("m_StomachVolume:" + m_StomachVolume);
		Print("================================");
	}
	
	void AddToStomach(string class_name, float amount, int food_stage = 0, int agents = 0, float temperature = 0)
	{
		if (GetIDFromClassname(class_name) == -1)
			return;
		
		NutritionalProfile profile = Liquid.GetNutritionalProfileByName(class_name);
		if (!profile)
			profile = Edible_Base.GetNutritionalProfile(null, class_name, food_stage);
		
		if (profile)
		{
			// sanity checks start
			if (amount > ACCEPTABLE_QUANTITY_MAX || amount < 0)
				amount = 0;

			if (food_stage < 0 || food_stage > ACCEPTABLE_FOODSTAGE_MAX)
				food_stage = FoodStageType.RAW;
			// sanity checks end
			
			agents = agents | profile.GetAgents();
			bool found = false;
			int count = m_StomachContents.Count();
			for(int i = 0; i < count; i++)
			{
				StomachItem stomachItem = m_StomachContents.Get(i);
				if (stomachItem.GetClassName() == class_name && stomachItem.m_Agents == agents)
				{
					if (profile.IsLiquid() || stomachItem.GetFoodStage() == food_stage)
					{
						float fraction = 1;
						if (amount != 0)
							fraction = (stomachItem.m_Amount + amount) / amount;							
						
						stomachItem.AddTemperature(temperature, fraction);
						stomachItem.AddAmount(amount);
						stomachItem.AddAgents(agents); //nutrition profile agents
						found = true;
					}
				}
			}
			
			if (!found)
				m_StomachContents.Insert(new StomachItem(class_name, amount, profile, food_stage, agents, temperature));
			
			UpdateStomachTemperature();
		}
	}
	
	void OnStoreSave(ParamsWriteContext ctx)
	{
		ctx.Write(PlayerStomach.CHECKSUM);
		ctx.Write(m_StomachContents.Count());
		StomachItem stomachItem;
		for(int i = 0; i < m_StomachContents.Count();i++)
		{
			stomachItem = m_StomachContents.Get(i);
			int id = PlayerStomach.GetIDFromClassname(stomachItem.m_ClassName);
			//Print("SAVE id:" + id);
			//Print("SAVE id_bit_offset:" + id_bit_offset);
			
			int writeResult = stomachItem.m_FoodStage | (id << id_bit_offset);
			writeResult = writeResult | ((int)stomachItem.m_Amount << quantity_bit_offset);
			ctx.Write(writeResult);
			ctx.Write(stomachItem.m_Agents);
			ctx.Write((int)stomachItem.GetTemperature());
			//Print("SAVE writeResult:" + writeResult);
		}
		//Print("SAVE CHECKSUM:" + PlayerStomach.CHECKSUM);
	}
	
	bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		int checksum, count;
		if (!ctx.Read(checksum))
		{
			return false;
		}
		
		if (!ctx.Read(count))
		{
			return false;
		}
		for (int i = 0; i < count; ++i)
		{
			int value, agents, temperature;
			if (!ctx.Read(value))
				return false;

			if (!ctx.Read(agents))
				return false;
			
			if (version >= 140 && !ctx.Read(temperature))
				return false;

			if (checksum == CHECKSUM)//if checksum matches, add to stomach, otherwise throw the data away but go through the de-serialization to keep the stream intact
			{
				int amount = value >> quantity_bit_offset;//isolate amount bits
				int id_mask = Math.Pow(2, quantity_bit_offset) - 1;
				int id = (id_mask & value) >> id_bit_offset;//isolate id bits
				int food_mask = Math.Pow(2, id_bit_offset) - 1;
				int food_stage = value & food_mask;
				//Print("LOAD value:" + value);
				//Print("LOAD id:" + id);
				//Print("LOAD id_bit_offset:" + id_bit_offset);
				//Print("LOAD food_stage:" + food_stage);
				string classname = GetClassnameFromID(id);
				AddToStomach(classname, amount, food_stage, agents, temperature);
			}
		}
		//Print("LOAD checksum:" + checksum);
		if (checksum != CHECKSUM)
		{
			Print("Stomach checksum fail");
		}
		return true;
	}

	int GetDebugObject(array<ref Param> object_out)
	{
		int count = m_StomachContents.Count();
		for(int i = 0; i < m_StomachContents.Count();i++)
		{
			int id = PlayerStomach.GetIDFromClassname(m_StomachContents.Get(i).m_ClassName);
			int food_stage = m_StomachContents.Get(i).m_FoodStage;
			int agents = m_StomachContents.Get(i).m_Agents;
			float amount = m_StomachContents.Get(i).m_Amount;
			float temperature = m_StomachContents.Get(i).GetTemperature();
			Param5<int,int,int,float,float> p5 = new Param5<int,int,int,float,float>(id, food_stage, agents, amount, temperature);
			object_out.Insert(p5);
		}
		Param1<float> p1 = new Param1<float>(m_StomachVolume);
		object_out.Insert(p1);
		Param1<float> paramTemp = new Param1<float>(m_StomachTemperature);
		object_out.Insert(paramTemp);
		return count;
		
	}
}