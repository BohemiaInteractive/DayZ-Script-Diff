class Edible_Base extends ItemBase
{
	const string DIRECT_COOKING_SLOT_NAME 	= "DirectCooking";
	//Baking
	const string SOUND_BAKING_START 		= "bake";		// raw stage
	const string SOUND_BAKING_DONE 			= "bakeDone";	// baked stage
	//Burning
	const string SOUND_BURNING_DONE 		= "burned";		// burned stage

	protected bool m_MakeCookingSounds;
	protected SoundOnVehicle m_SoundCooking;
	protected string m_SoundPlaying;
	ref FoodStage m_FoodStage;
	protected float m_DecayTimer;
	protected float m_DecayDelta = 0.0;
	protected FoodStageType m_LastDecayStage = FoodStageType.NONE;
	
	void Edible_Base()
	{
		if ( HasFoodStage() )
		{
			m_FoodStage = new FoodStage( this );
			
			//synchronized variables
			RegisterNetSyncVariableInt( "m_FoodStage.m_FoodStageType", 	FoodStageType.NONE, FoodStageType.COUNT );
			RegisterNetSyncVariableInt( "m_FoodStage.m_SelectionIndex", 0, 6 );
			RegisterNetSyncVariableInt( "m_FoodStage.m_TextureIndex", 	0, 6 );
			RegisterNetSyncVariableInt( "m_FoodStage.m_MaterialIndex", 	0, 6 );
			RegisterNetSyncVariableFloat( "m_FoodStage.m_CookingTime", 	0, 600, 0 );						//min = 0; max = 0; precision = 0;

			m_SoundPlaying = "";
			RegisterNetSyncVariableBool("m_MakeCookingSounds");
		}
	}
	
	override void EEInit()
	{
		super.EEInit();
		
		//update visual
		UpdateVisuals();
	}

	override void EEDelete( EntityAI parent )
	{
		super.EEDelete( parent );
		
		// remove audio
		RemoveAudio();
	}
	
	override void EEItemLocationChanged(notnull InventoryLocation oldLoc, notnull InventoryLocation newLoc)
	{
		super.EEItemLocationChanged(oldLoc, newLoc);

		//! disable sounds (from cooking)		
		if (oldLoc.GetType() == InventoryLocationType.ATTACHMENT || oldLoc.GetType() == InventoryLocationType.CARGO)
		{
			switch (oldLoc.GetParent().GetType())
			{
				case "FryingPan":
				case "Pot":
				case "Cauldron":
				case "SharpWoodenStick":
					MakeSoundsOnClient(false);
				break;
			}
			
			//! check for DirectCooking slot name
 			if (oldLoc.GetSlot() > -1 && InventorySlots.GetSlotName(oldLoc.GetSlot()).Contains(DIRECT_COOKING_SLOT_NAME))
			{
				MakeSoundsOnClient(false);
			}
		}
	}

	void UpdateVisuals()
	{
		if ( GetFoodStage() )
		{
			GetFoodStage().UpdateVisuals();
		}
	}
	
	bool Consume(float amount, PlayerBase consumer)
	{
		AddQuantity(-amount, false, false);
		OnConsume(amount, consumer);
		return true;
	}
	
	void OnConsume(float amount, PlayerBase consumer);	
	
	//food staging
	override bool CanBeCooked()
	{
		return false;
	}
	
	override bool CanBeCookedOnStick()
	{
		return false;
	}
	
	//================================================================
	// SYNCHRONIZATION
	//================================================================	
	void Synchronize()
	{
		SetSynchDirty();
			
		if (GetGame().IsMultiplayer())
		{
			UpdateVisuals();
		}
	}
	
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		
		//update visuals
		UpdateVisuals();

		//update audio
		if ( m_MakeCookingSounds )
		{
			RefreshAudio();
		}
		else
		{
			RemoveAudio();
		}
	}

	//================================================================
	// AUDIO EFFECTS (WHEN ON DCS)
	//================================================================	
	void MakeSoundsOnClient( bool soundstate )
	{
		m_MakeCookingSounds = soundstate;

		//synchronize
		Synchronize();
	}
	protected void RefreshAudio()
	{
		string sound_name;
		int particle_id;

		switch ( GetFoodStageType() )
		{
			case FoodStageType.RAW:
				sound_name = SOUND_BAKING_START;
				break;
			case FoodStageType.BAKED:
				sound_name = SOUND_BAKING_DONE;
				break;
			case FoodStageType.BURNED:
				sound_name = SOUND_BURNING_DONE;
				break;
			default:
				sound_name = SOUND_BAKING_START;
				break;
		}

		SoundCookingStart(sound_name);
	}
	protected void RemoveAudio()
	{
		m_MakeCookingSounds = false;
		SoundCookingStop();
	}

	//================================================================
	// SERIALIZATION
	//================================================================	
	override void OnStoreSave( ParamsWriteContext ctx )
	{   
		super.OnStoreSave( ctx );

		if ( GetFoodStage() )
		{
			GetFoodStage().OnStoreSave( ctx );
		}
		
		// food decay
		ctx.Write( m_DecayTimer );
		ctx.Write( m_LastDecayStage );
	}
	
	override bool OnStoreLoad( ParamsReadContext ctx, int version )
	{
		if ( !super.OnStoreLoad( ctx, version ) )
			return false;

		if ( GetFoodStage() )
		{
			if ( !GetFoodStage().OnStoreLoad( ctx, version ) )
			return false;
		}
		
		if ( version >= 115 )
		{
			if ( !ctx.Read( m_DecayTimer ) )
			{
				m_DecayTimer = 0.0;
				return false;
			}
			if ( !ctx.Read( m_LastDecayStage ) )
			{
				m_LastDecayStage = FoodStageType.NONE;
				return false;
			}
		}
		
		return true;
	}
	
	override void AfterStoreLoad()
	{	
		super.AfterStoreLoad();
		
		//synchronize
		Synchronize();
	}	

	//get food stage
	FoodStage GetFoodStage()
	{
		return m_FoodStage;
	}
	
	//food types
	override bool IsMeat()
	{
		return false;
	}
	
	override bool IsCorpse()
	{
		return false;
	}
	
	override bool IsFruit()
	{
		return false;
	}
	
	override bool IsMushroom()
	{
		return false;
	}	
	
	//================================================================
	// NUTRITIONAL VALUES
	//================================================================	
	//food properties
	static float GetFoodTotalVolume(ItemBase item, string classname = "", int food_stage = 0)
	{
		Edible_Base food_item = Edible_Base.Cast(item);
		if (food_item && food_item.GetFoodStage())
		{
			 return FoodStage.GetFullnessIndex(food_item.GetFoodStage());
		}
		else if (classname != "" && food_stage)
		{
			return FoodStage.GetFullnessIndex(null, food_stage, classname);
		}
		string class_path = string.Format("cfgVehicles %1 Nutrition", classname);
		return GetGame().ConfigGetFloat( class_path + " fullnessIndex" );

	}
	
	static float GetFoodEnergy(ItemBase item, string classname = "", int food_stage = 0)
	{
		Edible_Base food_item = Edible_Base.Cast(item);
		if (food_item && food_item.GetFoodStage())
		{
			 return FoodStage.GetEnergy(food_item.GetFoodStage());
		}
		else if (classname != "" && food_stage)
		{
			return FoodStage.GetEnergy(null, food_stage, classname);
		}
		string class_path = string.Format("cfgVehicles %1 Nutrition", classname);
		return GetGame().ConfigGetFloat( class_path + " energy" );			
	}
	
	static float GetFoodWater(ItemBase item, string classname = "", int food_stage = 0)
	{
		Edible_Base food_item = Edible_Base.Cast(item);
		if (food_item && food_item.GetFoodStage())
		{
			return FoodStage.GetWater(food_item.GetFoodStage());
		}
		else if (classname != "" && food_stage)
		{
			return FoodStage.GetWater(null, food_stage, classname);
		}
		string class_path = string.Format("cfgVehicles %1 Nutrition", classname);
		return GetGame().ConfigGetFloat( class_path + " water" );			
	}
	
	static float GetFoodNutritionalIndex(ItemBase item, string classname = "", int food_stage = 0)
	{
		Edible_Base food_item = Edible_Base.Cast(item);
		if (food_item && food_item.GetFoodStage())
		{
			return FoodStage.GetNutritionalIndex(food_item.GetFoodStage());	
		}
		else if (classname != "" && food_stage)
		{
			return FoodStage.GetNutritionalIndex(null, food_stage, classname);
		}
		string class_path = string.Format("cfgVehicles %1 Nutrition", classname);
		return GetGame().ConfigGetFloat( class_path + " nutritionalIndex" );		
		
	}
	
	static float GetFoodToxicity(ItemBase item, string classname = "", int food_stage = 0)
	{
		Edible_Base food_item = Edible_Base.Cast(item);
		if (food_item && food_item.GetFoodStage())
		{
			return FoodStage.GetToxicity(food_item.GetFoodStage());
		}
		else if (classname != "" && food_stage)
		{
			return FoodStage.GetToxicity(null, food_stage, classname);
		}
		string class_path = string.Format("cfgVehicles %1 Nutrition", classname);
		return GetGame().ConfigGetFloat( class_path + " toxicity" );			
	}
	
	static int GetFoodAgents(ItemBase item, string classname = "", int food_stage = 0)
	{
		Edible_Base food_item = Edible_Base.Cast(item);
		if (food_item && food_item.GetFoodStage())
		{
			return FoodStage.GetAgents(food_item.GetFoodStage());
		}
		else if (classname != "" && food_stage)
		{
			return FoodStage.GetAgents(null, food_stage, classname);
		}
		string class_path = string.Format("cfgVehicles %1 Nutrition", classname);
		return GetGame().ConfigGetInt( class_path + " agents" );
	}
	
	static float GetFoodDigestibility(ItemBase item, string classname = "", int food_stage = 0)
	{
		Edible_Base food_item = Edible_Base.Cast(item);
		if (food_item && food_item.GetFoodStage())
		{
			return FoodStage.GetDigestibility(food_item.GetFoodStage());
		}
		else if (classname != "" && food_stage)
		{
			return FoodStage.GetDigestibility(null, food_stage, classname);
		}
		string class_path = string.Format("cfgVehicles %1 Nutrition", classname);
		return GetGame().ConfigGetInt( class_path + " digestibility" );
	}
	
	static NutritionalProfile GetNutritionalProfile(ItemBase item, string classname = "", int food_stage = 0)
	{
		return new NutritionalProfile(GetFoodEnergy(item, classname, food_stage),GetFoodWater(item, classname, food_stage),GetFoodNutritionalIndex(item, classname, food_stage),GetFoodTotalVolume(item, classname, food_stage), GetFoodToxicity(item, classname, food_stage),  GetFoodAgents(item, classname,food_stage), GetFoodDigestibility(item, classname,food_stage));
	}
	
	//================================================================
	// FOOD STAGING
	//================================================================
	FoodStageType GetFoodStageType()
	{
		return GetFoodStage().GetFoodStageType();
	}
	
	//food stage states
	bool IsFoodRaw()
	{
		if ( GetFoodStage() ) 
		{
			return GetFoodStage().IsFoodRaw();
		}
		
		return false;
	}

	bool IsFoodBaked()
	{
		if ( GetFoodStage() ) 
		{
			return GetFoodStage().IsFoodBaked();
		}
		
		return false;
	}
	
	bool IsFoodBoiled()
	{
		if ( GetFoodStage() ) 
		{
			return GetFoodStage().IsFoodBoiled();
		}
		
		return false;
	}
	
	bool IsFoodDried()
	{
		if ( GetFoodStage() ) 
		{
			return GetFoodStage().IsFoodDried();
		}
		
		return false;
	}
	
	bool IsFoodBurned()
	{
		if ( GetFoodStage() ) 
		{
			return GetFoodStage().IsFoodBurned();
		}
		
		return false;
	}
	
	bool IsFoodRotten()
	{
		if ( GetFoodStage() ) 
		{
			return GetFoodStage().IsFoodRotten();
		}
		
		return false;
	}				
	
	//food stage change
	void ChangeFoodStage( FoodStageType new_food_stage_type )
	{
		GetFoodStage().ChangeFoodStage( new_food_stage_type );
	}
	
	FoodStageType GetNextFoodStageType( CookingMethodType cooking_method )
	{
		return GetFoodStage().GetNextFoodStageType( cooking_method );
	}
	
	string GetFoodStageName( FoodStageType food_stage_type )
	{
		return GetFoodStage().GetFoodStageName( food_stage_type );
	}
	
	bool CanChangeToNewStage( CookingMethodType cooking_method )
	{
		return GetFoodStage().CanChangeToNewStage( cooking_method );
	}
	
	//Use this to receive food stage from another Edible_Base
	void TransferFoodStage( notnull Edible_Base source )
	{
		if ( !source.HasFoodStage())
			return;
		m_LastDecayStage = source.GetLastDecayStage();
		ChangeFoodStage(source.GetFoodStage().GetFoodStageType());
		m_DecayTimer = source.GetDecayTimer();
		m_DecayDelta = source.GetDecayDelta();
	}
	
	//================================================================
	// COOKING
	//================================================================
	//cooking time
	float GetCookingTime()
	{
		return GetFoodStage().GetCookingTime();
	}
	
	void SetCookingTime( float time )
	{
		GetFoodStage().SetCookingTime( time );
		
		//synchronize when calling on server
		Synchronize();
	}
	
	//replace edible with new item (opening cans)
	void ReplaceEdibleWithNew( string typeName )
	{
		PlayerBase player = PlayerBase.Cast(GetHierarchyRootPlayer());
		if (player)
		{
			ReplaceEdibleWithNewLambda lambda = new ReplaceEdibleWithNewLambda(this, typeName, player);
			player.ServerReplaceItemInHandsWithNew(lambda);
		}
		else
			Error("ReplaceEdibleWithNew - cannot use edible without player");
	}

	override void SetActions()
	{
		super.SetActions();

		AddAction(ActionAttach);
		AddAction(ActionDetach);
	}

	protected void SoundCookingStart( string sound_name )
	{
		if ( GetGame() && ( !GetGame().IsDedicatedServer() ) )
		{	
			if ( m_SoundPlaying != sound_name )
			{
				//stop previous sound
				SoundCookingStop();
				
				//create new
				m_SoundCooking = PlaySoundLoop( sound_name, 50 );
				m_SoundPlaying = sound_name;
			}
		}
	}
	protected void SoundCookingStop()
	{
		if ( m_SoundCooking )
		{
			GetGame().ObjectDelete( m_SoundCooking );
			m_SoundCooking = NULL;
			m_SoundPlaying = "";
		}
	}
	
	override bool CanHaveTemperature()
	{
		return true;
	}
	
	override bool CanDecay()
	{
		return false;
	}
	
	override bool CanProcessDecay()
	{
		return ( GetFoodStageType() != FoodStageType.ROTTEN );
	}
	
	override void ProcessDecay( float delta, bool hasRootAsPlayer )
	{
		delta *= DayZGame.Cast(GetGame()).GetFoodDecayModifier();
		m_DecayDelta += ( 1 + ( 1 - GetHealth01( "", "" ) ) );
		if ( hasRootAsPlayer )
			m_DecayDelta += GameConstants.DECAY_RATE_ON_PLAYER;
		
		/*Print( "-------------------------" );
		Print( this );
		Print( m_DecayTimer );
		Print( m_DecayDelta );
		Print( m_LastDecayStage );*/
		
		if ( IsFruit() || IsMushroom() )
		{
			// fruit, vegetables and mushrooms
			if ( m_LastDecayStage != GetFoodStageType() )
			{
				switch ( GetFoodStageType() )
				{
					case FoodStageType.RAW:
						m_DecayTimer = ( GameConstants.DECAY_FOOD_RAW_FRVG + ( Math.RandomFloat01() * ( GameConstants.DECAY_FOOD_RAW_FRVG * ( GameConstants.DECAY_TIMER_RANDOM_PERCENTAGE / 100.0 ) ) ) );
						m_LastDecayStage = FoodStageType.RAW;
						break;
					
					case FoodStageType.BOILED:
						m_DecayTimer = ( GameConstants.DECAY_FOOD_BOILED_FRVG + ( Math.RandomFloat01() * ( GameConstants.DECAY_FOOD_BOILED_FRVG * ( GameConstants.DECAY_TIMER_RANDOM_PERCENTAGE / 100.0 ) ) ) );
						m_LastDecayStage = FoodStageType.BOILED;
						break;
					
					case FoodStageType.BAKED:
						m_DecayTimer = ( GameConstants.DECAY_FOOD_BAKED_FRVG + ( Math.RandomFloat01() * ( GameConstants.DECAY_FOOD_BAKED_FRVG * ( GameConstants.DECAY_TIMER_RANDOM_PERCENTAGE / 100.0 ) ) ) );
						m_LastDecayStage = FoodStageType.BAKED;
						break;
					
					case FoodStageType.DRIED:
					case FoodStageType.BURNED:
					case FoodStageType.ROTTEN:
					default:
						m_DecayTimer = -1;
						m_LastDecayStage = FoodStageType.NONE;
						return;
				}
				
				//m_DecayTimer = m_DecayTimer / 1000.0;
			}
			
			m_DecayTimer -= ( delta * m_DecayDelta );
						
			if ( m_DecayTimer <= 0 ) 
			{
				if ( m_LastDecayStage != FoodStageType.NONE )
				{
					// switch to decayed stage
					if ( ( m_LastDecayStage == FoodStageType.BOILED ) || ( m_LastDecayStage == FoodStageType.BAKED ) )
					{
						ChangeFoodStage( FoodStageType.ROTTEN );
					} 
					if ( m_LastDecayStage == FoodStageType.RAW )
					{
						int rng = Math.RandomIntInclusive( 0, 100 );
						if ( rng > GameConstants.DECAY_FOOD_FRVG_DRIED_CHANCE )
						{
							ChangeFoodStage( FoodStageType.ROTTEN );
						}
						else
						{
							if ( CanChangeToNewStage( FoodStageType.DRIED ) )
							{
								ChangeFoodStage( FoodStageType.DRIED );
							}
							else
							{
								ChangeFoodStage( FoodStageType.ROTTEN );
							}
						}
					}
				}
			}

		}
		else if ( IsMeat() )
		{
			// meat
			if ( m_LastDecayStage != GetFoodStageType() )
			{
				switch ( GetFoodStageType() )
				{
					case FoodStageType.RAW:
						m_DecayTimer = ( GameConstants.DECAY_FOOD_RAW_MEAT + ( Math.RandomFloat01() * ( GameConstants.DECAY_FOOD_RAW_MEAT * ( GameConstants.DECAY_TIMER_RANDOM_PERCENTAGE / 100.0 ) ) ) );
						m_LastDecayStage = FoodStageType.RAW;
						break;
					
					case FoodStageType.BOILED:
						m_DecayTimer = ( GameConstants.DECAY_FOOD_BOILED_MEAT + ( Math.RandomFloat01() * ( GameConstants.DECAY_FOOD_BOILED_MEAT * ( GameConstants.DECAY_TIMER_RANDOM_PERCENTAGE / 100.0 ) ) ) );
						m_LastDecayStage = FoodStageType.BOILED;
						break;
					
					case FoodStageType.BAKED:
						m_DecayTimer = ( GameConstants.DECAY_FOOD_BAKED_MEAT + ( Math.RandomFloat01() * ( GameConstants.DECAY_FOOD_BAKED_MEAT * ( GameConstants.DECAY_TIMER_RANDOM_PERCENTAGE / 100.0 ) ) ) );
						m_LastDecayStage = FoodStageType.BAKED;
						break;
					
					case FoodStageType.DRIED:
						m_DecayTimer = ( GameConstants.DECAY_FOOD_DRIED_MEAT + ( Math.RandomFloat01() * ( GameConstants.DECAY_FOOD_DRIED_MEAT * ( GameConstants.DECAY_TIMER_RANDOM_PERCENTAGE / 100.0 ) ) ) );
						m_LastDecayStage = FoodStageType.DRIED;
						break;

					case FoodStageType.BURNED:
					case FoodStageType.ROTTEN:
					default:
						m_DecayTimer = -1;
						m_LastDecayStage = FoodStageType.NONE;
						return;
				}
			}
			
			m_DecayTimer -= ( delta * m_DecayDelta );
			
			if ( m_DecayTimer <= 0 ) 
			{
				if ( m_LastDecayStage != FoodStageType.NONE )
				{
					// switch to decayed stage
					if ( ( m_LastDecayStage == FoodStageType.DRIED ) || ( m_LastDecayStage == FoodStageType.RAW ) || ( m_LastDecayStage == FoodStageType.BOILED ) || ( m_LastDecayStage == FoodStageType.BAKED ) )
					{
						ChangeFoodStage( FoodStageType.ROTTEN );
					}
				}
			}
		}
		else if ( IsCorpse() )
		{
			// corpse
			if ( m_LastDecayStage != GetFoodStageType() )
			{
				switch ( GetFoodStageType() )
				{
					case FoodStageType.RAW:
						m_DecayTimer = ( GameConstants.DECAY_FOOD_RAW_CORPSE + ( Math.RandomFloat01() * ( GameConstants.DECAY_FOOD_RAW_CORPSE * ( GameConstants.DECAY_TIMER_RANDOM_PERCENTAGE / 100.0 ) ) ) );
						m_LastDecayStage = FoodStageType.RAW;
						break;

					case FoodStageType.BURNED:
					case FoodStageType.ROTTEN:
					default:
						m_DecayTimer = -1;
						m_LastDecayStage = FoodStageType.NONE;
						return;
				}
			}
			
			m_DecayTimer -= ( delta * m_DecayDelta );
			
			if ( m_DecayTimer <= 0 ) 
			{
				if ( m_LastDecayStage != FoodStageType.NONE )
				{
					// switch to decayed stage
					if ( ( m_LastDecayStage == FoodStageType.DRIED ) || ( m_LastDecayStage == FoodStageType.RAW ) || ( m_LastDecayStage == FoodStageType.BOILED ) || ( m_LastDecayStage == FoodStageType.BAKED ) )
					{
						ChangeFoodStage( FoodStageType.ROTTEN );
					}
				}
			}
		}
		else
		{
			// opened cans
			m_DecayTimer -= ( delta * m_DecayDelta );

			if ( ( m_DecayTimer <= 0 ) && ( m_LastDecayStage == FoodStageType.NONE ) )
			{
				m_DecayTimer = ( GameConstants.DECAY_FOOD_CAN_OPEN + ( Math.RandomFloat01() * ( GameConstants.DECAY_FOOD_DRIED_MEAT * ( GameConstants.DECAY_TIMER_RANDOM_PERCENTAGE / 100.0 ) ) ) );
				m_LastDecayStage = FoodStageType.RAW;
				//m_DecayTimer = m_DecayTimer / 1000.0;
			}
			else
			{
				if ( m_DecayTimer <= 0 )
				{
					InsertAgent(eAgents.FOOD_POISON, 1);
					m_DecayTimer = -1;
				}
			}
		}
		
		m_DecayDelta = 0.0;
	}
	
	override void GetDebugActions(out TSelectableActionInfoArray outputList)
	{
		super.GetDebugActions(outputList);
		
		if(HasFoodStage())
		{
			outputList.Insert(new TSelectableActionInfo(SAT_DEBUG_ACTION, EActions.FOOD_STAGE_PREV, "Food Stage Prev"));
			outputList.Insert(new TSelectableActionInfo(SAT_DEBUG_ACTION, EActions.FOOD_STAGE_NEXT, "Food Stage Next"));
		}
	}
	
	override bool OnAction(int action_id, Man player, ParamsReadContext ctx)
	{
		super.OnAction(action_id, player, ctx);
		
		if ( GetGame().IsServer() )
		{
			if ( action_id == EActions.FOOD_STAGE_PREV )
			{
				int food_stage_prev = GetFoodStageType() - 1;
				if (food_stage_prev <= 0)
				{
					food_stage_prev = FoodStageType.COUNT - 1;
				}
				ChangeFoodStage(food_stage_prev);
				return true;
			}
			else if ( action_id == EActions.FOOD_STAGE_NEXT )
			{
				int food_stage_next = GetFoodStageType() + 1;
				if (food_stage_next >= FoodStageType.COUNT )
				{
					food_stage_next = FoodStageType.RAW;
				}
				ChangeFoodStage(food_stage_next);
				return true;
			}
		}
		return false;
	}
	//================================================================
	// GENERAL GETTERS
	//================================================================
	
	float GetDecayTimer()
	{
		return m_DecayTimer;
	}
	
	float GetDecayDelta()
	{
		return m_DecayDelta;
	}
	
	FoodStageType GetLastDecayStage()
	{
		return m_LastDecayStage;
	}
}

class ReplaceEdibleWithNewLambda : TurnItemIntoItemLambda
{
	void ReplaceEdibleWithNewLambda(EntityAI old_item, string new_item_type, PlayerBase player) { }
};
