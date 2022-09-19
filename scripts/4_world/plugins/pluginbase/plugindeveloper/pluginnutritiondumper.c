
class PluginNutritionDumper extends PluginBase
{
	/*
	ref TStringArray m_AllPaths = new TStringArray;
	ref TStringArray m_AllLines = new TStringArray;
	ref map<string, int> m_ParamPool = new map<string, int>;
	
	string config_path;
	string child_name;
	int scope;
	string path;
	PlayerBase m_Player;
	override void OnInit()
	{
		m_AllPaths.Insert("CfgVehicles");
		m_AllPaths.Insert("cfgLiquidDefinitions");
		m_Player = PlayerBase.Cast(GetGame().GetPlayer());
	}
	
	void CheckInit()
	{
		m_AllLines.Clear();
		string line = "Classname(stage),energy,water,toxicity,fullnessIndex,nutritionalIndex";
		m_AllLines.Insert(line);
		
		for(int i = 0; i < m_AllPaths.Count(); i++)
		{
			config_path = m_AllPaths.Get(i);
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
						EntityAI item = PlayerBase.Cast(GetGame().GetPlayer()).SpawnEntityOnGroundOnCursorDir(child_name,1);
						Edible_Base edible = Edible_Base.Cast(item);
						if(edible)
						{
							//Print("spawning " + child_name);
							line = "";
							NutritionalProfile profile;
							if(!has_stages)
							{
								profile =  edible.GetNutritionalProfile();
								line = BuildLine(child_name, profile);
								m_AllLines.Insert(line);
								//Print(line);
							}
							else
							{
								for(int z = FoodStageType.RAW; z < FoodStageType.COUNT; z++)
								{
									if( z != FoodStageType.RAW ) 
										edible.ChangeFoodStage(z);
									profile =  edible.GetNutritionalProfile();
									string itemname = child_name + "(stage " + z.ToString()+")";
									line = BuildLine(itemname, profile);
									m_AllLines.Insert(line);
								}
							}
						}
					}
				}
			}
		}
	
		SaveToFile("nutritional_values.csv");
	}
	
	
	protected void SaveToFile(string filename)
	{
		FileHandle file = OpenFile(filename, FileMode.WRITE);
		if( file!=0 )
		{
			for(int i = 0; i < m_AllLines.Count(); i++)
			{
				FPrintln(file,m_AllLines.Get(i));
				Print(m_AllLines.Get(i));
			}
			
		}

	}	
	
	string BuildLine(string item_name, NutritionalProfile profile)
	{
		string line = item_name+",";
		line = line + profile.GetEnergy()+",";
		line = line + profile.GetWaterContent()+",";
		line = line + profile.GetToxicity()+",";
		line = line + profile.GetFullnessIndex()+",";
		line = line + profile.GetNutritionalIndex();
		return line;
	}
	*/
}