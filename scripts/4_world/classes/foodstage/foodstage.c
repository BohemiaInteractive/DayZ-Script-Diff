enum FoodStageType
{
	NONE		= 0,			//food stage not define
	RAW			= 1,			//default
	BAKED		= 2,
	BOILED		= 3,
	DRIED		= 4,
	BURNED		= 5,
	ROTTEN		= 6,
	
	COUNT						//for net sync purposes
}

// Used to make getting data more readable
enum eCookingPropertyIndices
{
	MIN_TEMP 	= 0,
	COOK_TIME 	= 1,
	MAX_TEMP 	= 2
}

class FoodStage
{
	protected FoodStageType m_FoodStageTypeClientLast;
	protected FoodStageType m_FoodStageType;
	protected Edible_Base m_FoodItem;
	
	protected int m_SelectionIndex;				//visual properties
	protected int m_TextureIndex;
	protected int m_MaterialIndex;
	
	protected float m_CookingTime;
	
	// Lookup and search values
	// STRINGs are mostly used for CONFIG searches
	// INTs are mostly used for MAP searches
	static const string VISUAL_PROPERTIES 			= 	"visual_properties";
	static const string NUTRITION_PROPERTIES 		= 	"nutrition_properties";
	static const string COOKING_PROPERTIES 			= 	"cooking_properties";
	static const int 	VISUAL_PROPERTIES_HASH 		= 	VISUAL_PROPERTIES.Hash();
	static const int 	NUTRITION_PROPERTIES_HASH 	= 	NUTRITION_PROPERTIES.Hash();
	static const int 	COOKING_PROPERTIES_HASH 	= 	COOKING_PROPERTIES.Hash();
	
	// The following will be filled in constructor
	private static int 	m_StageRawHash 		= 0;
	private static int 	m_StageBakedHash 	= 0;
	private static int 	m_StageBoiledHash 	= 0;
	private static int 	m_StageDriedHash 	= 0;
	private static int 	m_StageBurnedHash 	= 0;
	private static int 	m_StageRottenHash 	= 0;
	
	// Cache food stage data for each Edible_Base
	// Used to get information for specific Edible_Base
	static ref map<int, ref map<int, ref map<int, ref array<float>>>> m_EdibleBasePropertiesMap;
	// Used to store food stage transitions for every Edible_Base
	static ref map<int, ref map<int, ref map<int, ref array<int>>>> m_EdibleBaseTransitionsMap;
	// Used to store the Hashed key of all possible food transitions ( including modded ones )
	static ref array<int> m_FoodStageTransitionKeys;
	
	//constructor
	void FoodStage( Edible_Base food_item )
	{
		//default
		m_FoodStageType = FoodStageType.RAW;
		m_FoodItem = food_item;
		
		//reset cooking time
		m_CookingTime = 0;
		
		m_FoodStageTypeClientLast = m_FoodStageType;
		
		// We fill all FoodStageHash values
		if ( m_StageRawHash == 0 )
			m_StageRawHash		= "Raw".Hash();
		if ( m_StageBakedHash == 0 )
			m_StageBakedHash	= "Baked".Hash();
		if ( m_StageBoiledHash == 0 )
			m_StageBoiledHash	= "Boiled".Hash();
		if ( m_StageDriedHash == 0 )
			m_StageDriedHash	= "Dried".Hash();
		if ( m_StageBurnedHash == 0 )
			m_StageBurnedHash	= "Burned".Hash();
		if ( m_StageRottenHash == 0 )
			m_StageRottenHash	= "Rotten".Hash();
		
		//get config data for all food stages
		SetupFoodStageMapping();
		
		// Get all config data relative to food stage transitions
		SetupFoodStageTransitionMapping();
		
		ChangeFoodStage( m_FoodStageType );
	}
	
	void SetupFoodStageMapping()
	{
		// We ensure we have the map is setup before trying to fill it
		if ( !m_EdibleBasePropertiesMap )
			m_EdibleBasePropertiesMap = new map<int, ref map<int, ref map< int, ref array<float>>>>;
		
		string foodType = m_FoodItem.GetType();
		int hashedFood = foodType.Hash();
		
		// We start to fill the map, we don't want duplicates of the same food type
		if ( !m_EdibleBasePropertiesMap.Contains( hashedFood ) )
		{
			map<int, ref map<int, ref array<float>>>foodStagesMap = new map<int, ref map<int, ref array<float>>>;
			
			for ( int i = 1; i < FoodStageType.COUNT; ++i )
			{
				map<int, ref array<float>> stagePropertiesMap = new map<int, ref array<float>>;
				
				// Insert visual properties
				array<float> visual_properties = new array<float>;
				string path = string.Format("CfgVehicles %1 Food FoodStages %2 visual_properties", foodType, GetFoodStageName(i));
				GetGame().ConfigGetFloatArray( path, visual_properties);
				
				stagePropertiesMap.Insert( VISUAL_PROPERTIES_HASH , visual_properties );
				
				// Insert nutrition properties
				array<float> nutrition_properties = new array<float>;
				path = string.Format("CfgVehicles %1 Food FoodStages %2 nutrition_properties", foodType, GetFoodStageName(i));
				GetGame().ConfigGetFloatArray( path, nutrition_properties);
				
				stagePropertiesMap.Insert( NUTRITION_PROPERTIES_HASH, nutrition_properties );
				
				// Insert cooking properties
				array<float> cooking_properties = new array<float>;
				path = string.Format("CfgVehicles %1 Food FoodStages %2 cooking_properties", foodType, GetFoodStageName(i));
				GetGame().ConfigGetFloatArray( path, cooking_properties);
				
				stagePropertiesMap.Insert( COOKING_PROPERTIES_HASH , cooking_properties );
				
				// Insert all properties for relevant food stage
				foodStagesMap.Insert( GetFoodStageNameHash( i ), stagePropertiesMap );
			}
			
			m_EdibleBasePropertiesMap.Insert( hashedFood, foodStagesMap );
		}
	}
	
	void SetupFoodStageTransitionMapping()
	{
		// We ensure we have only one map and that it does exist
		if ( !m_EdibleBaseTransitionsMap )
			m_EdibleBaseTransitionsMap = new map<int, ref map<int, ref map< int, ref array<int>>>>;
		
		// We ensure we have our key array setup
		if ( !m_FoodStageTransitionKeys )
			m_FoodStageTransitionKeys = new array<int>;
		
		string foodType = m_FoodItem.GetType();
		int hashedFood = foodType.Hash();
		
		// We start to fill the map, we don't want duplicates of the same food type
		if ( !m_EdibleBaseTransitionsMap.Contains( hashedFood ) )
		{
			map<int, ref map<int, ref array<int>>> foodStagesMap = new map<int, ref map<int, ref array<int>>>;
			
			for ( int i = 1; i < FoodStageType.COUNT; ++i )
			{
				map<int, ref array<int>> stageTransitionsMap = new map<int, ref array<int>>;
				string config_path = string.Format("CfgVehicles %1 Food FoodStageTransitions %2", foodType, GetFoodStageName( i ) );
				
				for ( int j = 0; j < GetGame().ConfigGetChildrenCount( config_path ); ++j )
				{
					array<int> stageTransition = new array<int>;
					string classCheck; // Used to get any existing transition class
					GetGame().ConfigGetChildName( config_path, j, classCheck );
					
					string transition_path = string.Format("%1 %2", config_path, classCheck );
					if ( GetGame().ConfigIsExisting( transition_path ) )
					{
						int transitionClassHash = classCheck.Hash();
						stageTransition.Insert( GetGame().ConfigGetInt( string.Format("%1 transition_to", transition_path) ) );
						stageTransition.Insert( GetGame().ConfigGetInt( string.Format("%1 cooking_method", transition_path) ) );
						stageTransitionsMap.Insert( transitionClassHash, stageTransition);
						
						// We only want one entry per key
						if ( m_FoodStageTransitionKeys.Find( transitionClassHash ) == -1 )
						{
							m_FoodStageTransitionKeys.Insert( transitionClassHash );
						}
					}
				}
				
				foodStagesMap.Insert( GetFoodStageNameHash(i), stageTransitionsMap );
			}
			
			m_EdibleBaseTransitionsMap.Insert( hashedFood, foodStagesMap );
		}
	}
	
	//Food Stage Type
	FoodStageType GetFoodStageType()
	{
		return m_FoodStageType;
	}
	
	void SetFoodStageType( FoodStageType food_stage_type )
	{
		m_FoodStageType = food_stage_type;
	}
	
	//Selection index
	int GetSelectionIndex()
	{
		return m_SelectionIndex;
	}
	void SetSelectionIndex( int index )
	{
		m_SelectionIndex = index;
	}
	
	//Texture index
	int GetTextureIndex()
	{
		return m_TextureIndex;
	}
	void SetTextureIndex( int index )
	{
		m_TextureIndex = index;
	}
	
	//Material index
	int GetMaterialIndex()
	{
		return m_MaterialIndex;
	}
	void SetMaterialIndex( int index )
	{
		m_MaterialIndex = index;
	}
		
	//Food properties
	protected static float GetNutritionPropertyFromIndex( int index, FoodStageType stage_type, FoodStage stage, string classname )
	{
		if ( stage )
		{
			stage_type = stage.m_FoodStageType;
			classname = stage.GetFoodItem().GetType();
		}
		
		string food_stage_name = GetFoodStageName( stage_type );
		int hashedStageName = GetFoodStageNameHash( stage_type );

		array<float> nutrition_properties;
		
		map<int, ref map<int, ref array<float>>>foodStagesMap;
		map<int, ref array<float>> stagePropertiesMap;
		
		if( !m_EdibleBasePropertiesMap.Find(classname.Hash(), foodStagesMap))
			return 0;
		if( !foodStagesMap.Find(hashedStageName, stagePropertiesMap))
			return 0;
		if( !stagePropertiesMap.Find(NUTRITION_PROPERTIES_HASH, nutrition_properties))
			return 0;

		if ( nutrition_properties.Count() > 0 )
		{
			if ( index > (nutrition_properties.Count() - 1) )
			{
				return 0;
			}
			else
			{
				return nutrition_properties.Get( index );
			}
		}
		//calculate nutrition properties from base stage and nutrition modifiers
		else
		{
			// Will not attempt to optimize this as it is for a setup we do not support internally
			//get modifiers class for nutrition values
			string config_path = string.Format("CfgVehicles %1 Food nutrition_modifiers_class", classname);
			
			if ( GetGame().ConfigIsExisting( config_path ) )
			{
				string nutr_mod_class;
				GetGame().ConfigGetText( config_path, nutr_mod_class );
				
				config_path = string.Format("CfgVehicles NutritionModifiers %1 base_stage", nutr_mod_class);
				string nutr_base_stage;
				GetGame().ConfigGetText( config_path, nutr_base_stage );
				
				//get nutrition values for food stage and modifiers 
				config_path = string.Format("CfgVehicles %1 Food FoodStages %2 nutrition_properties", classname, nutr_base_stage);
				array<float> base_nutr_properties = new array<float>;
				GetGame().ConfigGetFloatArray( config_path, base_nutr_properties );
				
				config_path = string.Format("CfgVehicles NutritionModifiers %1 %2 nutrition_properties", nutr_mod_class, food_stage_name);
				array<float> nutr_mod_properties = new array<float>;
				GetGame().ConfigGetFloatArray( config_path, nutr_mod_properties );
				
				//base nutrition * food stage nutrition modifier
				if ( base_nutr_properties.Count() > 0 && nutr_mod_properties.Count() > 0 )
				{
					return ( base_nutr_properties.Get( index ) * nutr_mod_properties.Get( index ) );
				}
			}
		}
		
		return 0;
	}
	
	static float GetFullnessIndex(FoodStage stage, int stage_type = -1, string classname = "")
	{
		return GetNutritionPropertyFromIndex( 0 , stage_type, stage, classname );
	}
	
	static float GetEnergy(FoodStage stage, int stage_type = -1, string classname = "")
	{
		return GetNutritionPropertyFromIndex( 1 , stage_type, stage, classname );
	}
	
	static float GetWater(FoodStage stage, int stage_type = -1, string classname = "")
	{
		return GetNutritionPropertyFromIndex( 2 , stage_type, stage, classname );
	}
	
	static float GetNutritionalIndex(FoodStage stage, int stage_type = -1, string classname = "")
	{
		return GetNutritionPropertyFromIndex( 3 , stage_type , stage, classname);
	}
	
	static float GetToxicity(FoodStage stage, int stage_type = -1, string classname = "")
	{
		return GetNutritionPropertyFromIndex( 4 , stage_type, stage, classname );
	}
	
	static int GetAgents(FoodStage stage, int stage_type = -1, string classname = "")
	{
		return GetNutritionPropertyFromIndex( 5 , stage_type, stage, classname );
	}
	
	static float GetDigestibility(FoodStage stage, int stage_type = -1, string classname = "")
	{
		return GetNutritionPropertyFromIndex( 6 , stage_type, stage, classname );
	}
	
	//Food item
	protected Edible_Base GetFoodItem()
	{
		return m_FoodItem;
	}
	
	//Cooking time
	float GetCookingTime()
	{
		return m_CookingTime;
	}
	void SetCookingTime( float time )
	{
		m_CookingTime = time;
	}
	
	static float GetCookingPropertyFromIndex( int index, FoodStageType stage_type, FoodStage stage, string classname )
	{ 
		if ( stage )
		{
			stage_type = stage.m_FoodStageType;
			classname = stage.GetFoodItem().GetType();
		}
		
		string food_stage_name = GetFoodStageName( stage_type );
		int hashedStageName = GetFoodStageNameHash( stage_type );

		array<float> cooking_properties = new array<float>;
		
		map<int, ref map<int, ref array<float>>>foodStagesMap = new map<int, ref map<int, ref array<float>>>;
		map<int, ref array<float>> stagePropertiesMap = new map<int, ref array<float>>;
		
		m_EdibleBasePropertiesMap.Find(classname.Hash(), foodStagesMap);
		foodStagesMap.Find(hashedStageName, stagePropertiesMap);
		
		stagePropertiesMap.Find(COOKING_PROPERTIES_HASH, cooking_properties);
		
		if ( cooking_properties.Count() > 0 )
		{
			if ( index > (cooking_properties.Count() - 1) )
			{
				return -1;
			}
			else
			{
				return cooking_properties.Get( index );
			}
		}
		return 0;
	}
	
	static array<float> GetAllCookingPropertiesForStage( FoodStageType stage_type, FoodStage stage, string classname )
	{
		if ( stage )
		{
			stage_type = stage.m_FoodStageType;
			classname = stage.GetFoodItem().GetType();
		}
		
		string food_stage_name = GetFoodStageName( stage_type );
		int hashedStageName = GetFoodStageNameHash( stage_type );

		array<float> cooking_properties = new array<float>;
		
		map<int, ref map<int, ref array<float>>>foodStagesMap = new map<int, ref map<int, ref array<float>>>;
		map<int, ref array<float>> stagePropertiesMap = new map<int, ref array<float>>;
		
		m_EdibleBasePropertiesMap.Find(classname.Hash(), foodStagesMap);
		foodStagesMap.Find(hashedStageName, stagePropertiesMap);
		
		stagePropertiesMap.Find(COOKING_PROPERTIES_HASH, cooking_properties);
		
		if ( cooking_properties.Count() > 0 )
		{
			return cooking_properties;
		}
		return null;
	}
	
	//********************************************/
	//	FOOD STAGE CHANGE
	//********************************************/
	//Checks if food stage can be changed to another stage
	bool CanChangeToNewStage( CookingMethodType cooking_method )
	{
		if ( GetNextFoodStageType( cooking_method ) == FoodStageType.NONE )
		{
			return false;
		}
		
		return true;
	}
	
	//returns possible food stage type according to given cooking method
	FoodStageType GetNextFoodStageType( CookingMethodType cooking_method )
	{
		array<int> food_transition = new array<int>;
		
		map<int, ref map<int, ref array<int>>> foodStagesMap = new map<int, ref map<int, ref array<int>>>;
		map<int, ref array<int>> foodTransitionsMap = new map<int, ref array<int>>;
		
		m_EdibleBaseTransitionsMap.Find(GetFoodItem().GetType().Hash(), foodStagesMap);
		foodStagesMap.Find( GetFoodStageNameHash( GetFoodStageType() ), foodTransitionsMap );
		
		// We go through the key array, checking every possible transition
		for ( int i = 0; i < m_FoodStageTransitionKeys.Count(); ++i )
		{
			// We test if a given transition is setup on this item
			foodTransitionsMap.Find(m_FoodStageTransitionKeys.Get( i ), food_transition);
			if ( food_transition )
			{
				// We now check if the given transition class is relevant
				if ( food_transition.Get( 1 ) == cooking_method )
				{
					return food_transition.Get( 0 );
				}
			}
		}
		
		return FoodStageType.BURNED; //If the item cannot transition out of current state, burn it
	}
	
	void ChangeFoodStage( FoodStageType new_stage_type )
	{
		map<int, ref map<int, ref array<float>>>foodStagesMap = new map<int, ref map<int, ref array<float>>>;
		map<int, ref array<float>> stagePropertiesMap = new map<int, ref array<float>>;
		
		//merge stages
		//food stage type
		SetFoodStageType( new_stage_type );
		
		array<float> visual_properties = new array<float>;
		
		m_EdibleBasePropertiesMap.Find( GetFoodItem().GetType().Hash(), foodStagesMap );
		foodStagesMap.Find( GetFoodStageNameHash( GetFoodStageType() ), stagePropertiesMap );
		stagePropertiesMap.Find( VISUAL_PROPERTIES_HASH, visual_properties );
		
		if ( visual_properties.Count() > 0 )
		{
			//selection index
			int index = visual_properties.Get( 0 );
			if ( index >= 0 )
			{
				SetSelectionIndex( index );
			}
			//texture index
			index = visual_properties.Get( 1 );
			if ( index >= 0 )
			{
				SetTextureIndex( index );
			}
			//material index
			index = visual_properties.Get( 2 );
			if ( index >= 0 )
			{
				SetMaterialIndex( index );
			}	
		}
		
		//refresh visual
		GetFoodItem().Synchronize();
	}	

	void UpdateVisuals()
	{
		//if item has food stages
		if ( GetFoodItem().HasFoodStage() ) 
		{
			Edible_Base food_item = GetFoodItem();
			
			array<string> config_selections	= food_item.GetHiddenSelections();
			array<string> config_textures	= food_item.GetHiddenSelectionsTextures();
			array<string> config_materials	= food_item.GetHiddenSelectionsMaterials();
			
			//selection index
			int selection_index;
			if ( GetSelectionIndex() >= 0 && config_selections.Count() > GetSelectionIndex() )
			{
				selection_index = GetSelectionIndex();
			}
			
			//texture index
			int texture_index;
			if ( GetTextureIndex() >= 0 && config_textures.Count() > GetTextureIndex() )
			{
				texture_index = GetTextureIndex();
			}
	
			//material index
			int material_index;
			if ( GetMaterialIndex() >= 0 && config_materials.Count() > GetMaterialIndex() )
			{
				material_index = GetMaterialIndex();
			}
				
			//hide all selection except the configured one
			for ( int i = 0; i < config_selections.Count(); i++ )
			{
				if ( config_selections.Get( i ) != config_selections.Get( selection_index ) )
				{
					food_item.SetAnimationPhase( config_selections.Get( i ), 1 );
				}
			}
	
			//Debug	
			//Print( "item = " + food_item.GetType() + " selection index = " + GetSelectionIndex().ToString() + " texture index = " + GetTextureIndex().ToString() );
		
			//show selection
			food_item.SetAnimationPhase( config_selections.Get( selection_index ), 0 );
			//set texture
			food_item.SetObjectTexture( selection_index, config_textures.Get( texture_index ) );
			//set materials
			food_item.SetObjectMaterial( selection_index, config_materials.Get( material_index ) );
		}
				
		m_FoodStageTypeClientLast = m_FoodStageType;
	}

	//Food States
	//check food stages
	bool IsFoodInStage( FoodStageType food_stage_type )
	{
		if ( GetFoodStageType() == food_stage_type )
		{
			return true;
		}
		
		return false;
	}
	
	bool IsFoodRaw()
	{
		return IsFoodInStage( FoodStageType.RAW );
	}
	
	bool IsFoodBaked()
	{
		return IsFoodInStage( FoodStageType.BAKED );
	}
	
	bool IsFoodBoiled()
	{
		return IsFoodInStage( FoodStageType.BOILED );
	}
	
	bool IsFoodDried()
	{
		return IsFoodInStage( FoodStageType.DRIED );
	}
	
	bool IsFoodBurned()
	{
		return IsFoodInStage( FoodStageType.BURNED );
	}

	bool IsFoodRotten()
	{
		return IsFoodInStage( FoodStageType.ROTTEN );
	}
			
	//get name of food stage type
	static string GetFoodStageName( FoodStageType food_stage_type )
	{
		switch ( food_stage_type )
		{
			case FoodStageType.RAW: 	return "Raw";
			case FoodStageType.BAKED: 	return "Baked";
			case FoodStageType.BOILED: 	return "Boiled";
			case FoodStageType.DRIED: 	return "Dried";
			case FoodStageType.BURNED: 	return "Burned";
			case FoodStageType.ROTTEN: 	return "Rotten";
		}
		
		return "Raw";
	}
	
	// Get hashed name of food stage type for quicker access
	static int GetFoodStageNameHash( FoodStageType food_stage_type )
	{
		switch ( food_stage_type )
		{
			case FoodStageType.RAW:		return m_StageRawHash;
			case FoodStageType.BAKED:	return m_StageBakedHash;
			case FoodStageType.BOILED:	return m_StageBoiledHash;
			case FoodStageType.DRIED:	return m_StageDriedHash;
			case FoodStageType.BURNED:	return m_StageBurnedHash;
			case FoodStageType.ROTTEN:	return m_StageRottenHash;
		}
		
		return m_StageRawHash;
	}
	
	//================================================================
	// SERIALIZATION
	//================================================================	
	void OnStoreSave( ParamsWriteContext ctx )
	{   
		//Food stage type 
		ctx.Write( m_FoodStageType );
		
		//Selection index
		ctx.Write( m_SelectionIndex );

		//Texture index
		ctx.Write( m_TextureIndex );

		//Material index
		ctx.Write( m_MaterialIndex );
	}
	
	bool OnStoreLoad( ParamsReadContext ctx, int version )
	{
		//Food stage type 
		if ( !ctx.Read( m_FoodStageType ) )
		{
			m_FoodStageType = FoodStageType.RAW;	//set default
			return false;
		}
		
		//Selection index
		if ( !ctx.Read( m_SelectionIndex ) )
		{
			m_SelectionIndex = 0;			//set default
			return false;
		}
		
		//Texture index
		if ( !ctx.Read( m_TextureIndex ) )
		{
			m_TextureIndex = 0;				//set default
			return false;
		}
		
		//Material index
		if ( !ctx.Read( m_MaterialIndex ) )
		{
			m_MaterialIndex = 0;			//set default			
			return false;
		}
		
		return true;
	}
}
