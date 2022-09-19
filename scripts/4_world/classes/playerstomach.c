class StomachItem
{
	ref NutritionalProfile m_Profile;
	float m_Amount;
	int m_FoodStage;
	//bool m_IsLiquid;
	string m_ClassName;
	int m_Agents;
	
	void StomachItem(string class_name, float amount, NutritionalProfile profile,int foodstage, int agents)
	{
		m_Amount = amount;
		m_Profile = profile;
		//m_IsLiquid = is_liquid;
		m_FoodStage = foodstage;
		m_ClassName = class_name;
		m_Agents = agents;
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
	
	
	// "digests" given amount, "outs" nutritional components, and returns 'true' if leads to complete item digestion(no amount left)
	bool ProcessDigestion(float digestion_points, out float water, out float energy, out float toxicity, out float volume, out int agents, out float consumed_amount)
	{
		agents = m_Agents;
		consumed_amount = GetNutritions(digestion_points, m_Profile, water, energy, toxicity );
		m_Amount -= consumed_amount;
		volume = m_Profile.GetFullnessIndex() * m_Amount;
		return( m_Amount < 0.001 );
	}
	
	float GetNutritions( float digestion_points, NutritionalProfile profile, out float water, out float energy, out float toxicity )
	{	
		float energy_per_unit = profile.GetEnergy() / 100;
		float water_per_unit = profile.GetWaterContent() / 100;
		float toxicity_per_unit = profile.GetToxicity();
		float digestability = profile.GetDigestibility();
		
		if(digestability == 0)//if undefined
		{
			digestability = 1;
		}
		
		float consumed_quantity = digestion_points * digestability;
		
		if( m_Amount < consumed_quantity )
		{
			consumed_quantity = m_Amount;
		}
		if ( consumed_quantity > 0 )
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
	const int STORAGE_VERSION = 106;
	
		
	void PlayerStomach(PlayerBase player)
	{
		m_Player = player;
	}

	void ~PlayerStomach()
	{
		
	}
	
	
	float GetStomachVolume()
	{
		return m_StomachVolume;
	}
	
	void ClearContents()
	{
		m_StomachContents.Clear();
		m_StomachVolume = 0.0;
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
//		Print(classname);
//		Print(CHECKSUM);
		m_NamesToIDs.Insert(classname, id);
		m_IDsToNames.Insert(id, classname);
	}
	
	static string GetClassnameFromID(int id)
	{
		return m_IDsToNames.Get(id);
	}
	
	static int GetIDFromClassname(string name)
	{
		if(!m_NamesToIDs.Contains(name))
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
				scope = GetGame().ConfigGetInt( config_path + " " + child_name + " scope" );
				bool should_check = 1;
				if( config_path == "CfgVehicles" && scope == 0)
				{
					should_check = 0;
				}
				
				if ( should_check )
				{
					bool has_nutrition = GetGame().ConfigIsExisting(path + " Nutrition");
					bool has_stages = GetGame().ConfigIsExisting(path + " Food");
					
					if(has_nutrition || has_stages)
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
	
	
	void Update(float delta_time)
	{
		ProcessNutrients(delta_time);
	}
	
	void ProcessNutrients(float delta_time)
	{
		StomachItem item;
		int stomach_items_count = m_StomachContents.Count();
		m_DigestingType = 0;
		if(stomach_items_count == 0) 
			return;
		
		float digestion_points_per_item = (DIGESTION_POINTS / stomach_items_count) * delta_time;
		m_StomachVolume = 0;//reset, it's accumulated with each pass
		for(int i = 0; i < m_StomachContents.Count(); i ++)
		{
			item = m_StomachContents.Get(i);
			float water, energy, toxicity, volume, consumed_amount;
			int agents;
			if(item.ProcessDigestion( digestion_points_per_item, water, energy, toxicity, volume, agents, consumed_amount ))
			{
				m_StomachContents.Remove(i);
				i--;
			}
			m_StomachVolume += volume;
			m_Player.GetStatEnergy().Add(energy);
			m_Player.GetStatWater().Add(water);
			
			if(energy > 0)
			{
				m_DigestingType = m_DigestingType | DIGESTING_ENERGY;
			}
			
			if(water > 0)
			{
				m_DigestingType = m_DigestingType | DIGESTING_WATER;
			}
			
			DigestAgents(agents, consumed_amount);
		}
	}

	
	void DigestAgents(int agents, float quantity)
	{
		if(!agents)
			return;
		agents = agents & (~m_AgentTransferFilter);//filter against the agent filter mask
		int highest_bit = Math.Log2(agents) + 1;
		for(int i = 0; i < highest_bit;i++)
		{
			int agent = (1 << i)& agents;
			if( agent )
			{
				m_Player.m_AgentPool.DigestAgent(agent, quantity);
			}
		}
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
	
	void AddToStomach(string class_name, float amount, int food_stage = 0, int agents = 0)
	{
		if (GetIDFromClassname(class_name) == -1)
			return;
		bool is_liquid;
		
		NutritionalProfile profile = Liquid.GetNutritionalProfileByName(class_name);
		if(profile)
		{
			is_liquid = true;
		}
		else
		{
			profile = Edible_Base.GetNutritionalProfile(null,class_name, food_stage);
		}
		
		if(profile)
		{
			// sanity checks start
			if(amount > ACCEPTABLE_QUANTITY_MAX || amount < 0)
			{
				amount = 0;
			}
			if (food_stage < 0 || food_stage > ACCEPTABLE_FOODSTAGE_MAX)
			{
				food_stage = FoodStageType.RAW;
			}
			// sanity checks end
			
			agents = agents | profile.GetAgents();
			bool found = false;
			for(int i = 0; i < m_StomachContents.Count(); i++)
			{
				StomachItem stomach_item = m_StomachContents.Get(i);
				if(stomach_item.GetClassName() == class_name )
				{
					if (is_liquid || stomach_item.GetFoodStage() == food_stage)
					{
						stomach_item.AddAmount(amount);
						stomach_item.AddAgents(agents);
						found = true;
					}
				}
			}
			
			if(!found)
			{
				m_StomachContents.Insert(new StomachItem(class_name, amount, profile, food_stage, agents));
			}
		}
	}
	
	void OnStoreSave(ParamsWriteContext ctx)
	{
		ctx.Write( PlayerStomach.CHECKSUM );
		ctx.Write( m_StomachContents.Count() );
		StomachItem stomach_item;
		for(int i = 0; i < m_StomachContents.Count();i++)
		{
			stomach_item = m_StomachContents.Get(i);
			int id = PlayerStomach.GetIDFromClassname(stomach_item.m_ClassName);
			//Print("SAVE id:" + id);
			//Print("SAVE id_bit_offset:" + id_bit_offset);
			
			int write_result = stomach_item.m_FoodStage | ( id << id_bit_offset );
			write_result = write_result | ((int)stomach_item.m_Amount << quantity_bit_offset);
			ctx.Write( write_result );
			ctx.Write( stomach_item.m_Agents );
			//Print("SAVE write_result:" + write_result);
		}
		//Print("SAVE CHECKSUM:" + PlayerStomach.CHECKSUM);
	}
	
	bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		int checksum, count;
		if(!ctx.Read(checksum))
		{
			return false;
		}
		
		if(!ctx.Read(count))
		{
			return false;
		}
		for(int i = 0; i < count; i++)
		{
			int value, agents;
			if(!ctx.Read(value) )
			{
				return false;
			}	
			if(!ctx.Read(agents) )
			{
				return false;
			}
			if(checksum == CHECKSUM)//if checksum matches, add to stomach, otherwise throw the data away but go through the de-serialization to keep the stream intact
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
				AddToStomach(classname, amount, food_stage, agents);
			}
		}
		//Print("LOAD checksum:" + checksum);
		if(checksum != CHECKSUM)
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
			Param4<int,int,int,float> p4 = new Param4<int,int,int,float>(id, food_stage, agents, amount);
			object_out.Insert(p4);
		}
		Param1<float> p1 = new Param1<float>(m_StomachVolume);
		object_out.Insert(p1);
		return count;
		
	}
}