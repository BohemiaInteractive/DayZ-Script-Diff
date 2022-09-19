enum CookingMethodType
{
	NONE		= 0,		//no cooking method available
	BAKING		= 1,
	BOILING		= 2,
	DRYING		= 3,
	TIME		= 4,
	
	COUNT
}

class Cooking
{
	static const float TIME_WITH_SUPPORT_MATERIAL_COEF 				= 1.0;	//! time modifier used when not using support material
	static const float TIME_WITHOUT_SUPPORT_MATERIAL_COEF			= 2.0;  //! time modifier used when using support material

	static const float COOKING_FOOD_TIME_INC_VALUE 					= 2;	//! time increase when cooking a food
	static const float COOKING_LARD_DECREASE_COEF 					= 25;	//! how many units from quantity of lard are remove at each stage
	static const float COOKING_FOOD_QUANTITY_DECREASE_AMOUNT_NONE 	= 25;	//! how many units from quantity of item are removed at each stage when support material is NOT used
	static const float COOKING_FOOD_QUANTITY_DECREASE_AMOUNT_LARD 	= 0;	//! NOT USED.

	static const float DEFAULT_COOKING_TEMPERATURE 					= 150;	//default temperature for cooking (e.g. cooking on stick)
	static const float FOOD_MAX_COOKING_TEMPERATURE					= 150;	//
	static const float PARAM_BURN_DAMAGE_COEF						= 0.05;	//value for calculating damage on items located in fireplace CargoGrid

	static const float LIQUID_BOILING_POINT 						= 150;	//boiling point for liquids
	static const float LIQUID_VAPOR_QUANTITY 						= 2;	//vapor quantity

	typename COOKING_EQUIPMENT_POT	 					= Pot;
	typename COOKING_EQUIPMENT_FRYINGPAN				= FryingPan;
	typename COOKING_EQUIPMENT_CAULDRON					= Cauldron;
	typename COOKING_INGREDIENT_LARD 					= Lard;
	
	void ProcessItemToCook(notnull ItemBase pItem, ItemBase cookingEquip, Param2<CookingMethodType, float> pCookingMethod, out Param2<bool, bool> pStateFlags)
	{
		Edible_Base item_to_cook = Edible_Base.Cast(pItem);
		
		//! state flags are in order: is_done, is_burned
		pStateFlags = new Param2<bool, bool>(false, false);
				
		if (item_to_cook && item_to_cook.CanBeCooked())
		{
			//! enable cooking sound		
			item_to_cook.MakeSoundsOnClient(true);

			//! update food
			UpdateCookingState(item_to_cook, pCookingMethod.param1, cookingEquip, pCookingMethod.param2);
			
			//check for done state for boiling and drying
			if (item_to_cook.IsFoodBoiled() || item_to_cook.IsFoodDried())
			{
				pStateFlags.param1 = true;
			}
			//! check for done state from baking (exclude Lard from baked items)
			else if (item_to_cook.IsFoodBaked() && item_to_cook.Type() != Lard)
			{
				pStateFlags.param1 = true;
			}
			//! check for burned state
			else if (item_to_cook.IsFoodBurned())
			{
				pStateFlags.param2 = true;
			}
		}
		else   
		{
			//damage item
			pItem.DecreaseHealth("", "", PARAM_BURN_DAMAGE_COEF * 100);
			
			//add temperature to item
			AddTemperatureToItem(pItem, null, 0);
		}
	}

	//COOKING PROCESS
	//--- Cooking with equipment (pot)
	//Returns 1 if the item changed its cooking stage, 0 if not
	int CookWithEquipment(ItemBase cooking_equipment, float cooking_time_coef = 1)
	{
		bool is_empty;
		
		//check cooking conditions
		if (cooking_equipment == null)
		{
			return 0;
		}
		
		if (cooking_equipment.IsRuined())
		{
			return 0;
		}
		
		//manage items in cooking equipment
		Param2<bool, bool> stateFlags = new Param2<bool, bool>(false, false); // 1st - done; 2nd - burned
		Param2<CookingMethodType, float> cookingMethodWithTime = GetCookingMethodWithTimeOverride(cooking_equipment);
		
		//! cooking time coef override
		if (cooking_time_coef != 1)
		{
			cookingMethodWithTime.param2 = cooking_time_coef;
		}
		
		CargoBase cargo = cooking_equipment.GetInventory().GetCargo();
		if (cargo)
		{
			is_empty = cargo.GetItemCount() == 0;
			
			//process items
			for (int i = 0; i < cargo.GetItemCount(); i++)
			{
				ProcessItemToCook(ItemBase.Cast(cargo.GetItem(i)), cooking_equipment, cookingMethodWithTime, stateFlags);
			}
		}
		else
		{
			ProcessItemToCook(cooking_equipment, cooking_equipment, cookingMethodWithTime, stateFlags);
		}
		
		//manage cooking equipment
		Bottle_Base bottle_base = Bottle_Base.Cast(cooking_equipment);
		if ( bottle_base )
		{
			float cooking_equipment_temp = cooking_equipment.GetTemperature();
			bool is_water_boiling;
			
			//handle water boiling
			if (cooking_equipment_temp >= LIQUID_BOILING_POINT)
			{
				//remove agents
				cooking_equipment.RemoveAllAgents();
				
				if (cooking_equipment.GetQuantity() > 0)
				{
					is_water_boiling = true;
					
					//vaporize liquid
					cooking_equipment.AddQuantity(-LIQUID_VAPOR_QUANTITY);
				};
			}
			
			//handle audio visuals
			bottle_base.RefreshAudioVisualsOnClient(cookingMethodWithTime.param1, stateFlags.param1, is_empty, stateFlags.param2);
		}

		FryingPan frying_pan = FryingPan.Cast(cooking_equipment);
		if (frying_pan && !bottle_base)
		{
			//handle audio visuals
			frying_pan.RefreshAudioVisualsOnClient(cookingMethodWithTime.param1, stateFlags.param1, is_empty, stateFlags.param2);
		}
		
		return 1;
	}
	
	//Returns 1 if the item changed its cooking stage, 0 if not
	int CookOnStick( Edible_Base item_to_cook, float cook_time_inc )
	{
		if ( item_to_cook && item_to_cook.CanBeCookedOnStick() )
		{
			//update food
			return UpdateCookingStateOnStick( item_to_cook, cook_time_inc );
		}
		
		return 0;
	}
	
	//Returns 1 if the item changed its cooking stage, 0 if not
	protected int UpdateCookingState(Edible_Base item_to_cook, CookingMethodType cooking_method, ItemBase cooking_equipment, float cooking_time_coef)
	{
		//food properties
		float food_temperature = item_to_cook.GetTemperature();
		
		//{min_temperature, time_to_cook, max_temperature (optional)}
		//get next stage name - if next stage is not defined, next stage name will be empty "" and no cooking properties (food_min_temp, food_time_to_cook, food_max_temp) will be set
		FoodStageType new_stage_type = item_to_cook.GetNextFoodStageType(cooking_method);

		float food_min_temp 	= 0;
		float food_time_to_cook = 0;
		float food_max_temp		= -1;
		
		//Set next stage cooking properties if next stage possible
		if (item_to_cook.CanChangeToNewStage(cooking_method)) // new_stage_type != NONE
		{
			array<float> next_stage_cooking_properties = new array<float>();
			next_stage_cooking_properties = FoodStage.GetAllCookingPropertiesForStage(new_stage_type, null, item_to_cook.GetType());
			
			food_min_temp = next_stage_cooking_properties.Get(eCookingPropertyIndices.MIN_TEMP);
			food_time_to_cook = next_stage_cooking_properties.Get(eCookingPropertyIndices.COOK_TIME);
			// The last element is optional and might not exist
			if (next_stage_cooking_properties.Count() > 2)
			{
				food_max_temp = next_stage_cooking_properties.Get(eCookingPropertyIndices.MAX_TEMP);
			}
		}
		
		//add temperature
		AddTemperatureToItem(item_to_cook, cooking_equipment, food_min_temp);
		
		//add cooking time if the food can be cooked by this method
		if (food_min_temp > 0 && food_temperature >= food_min_temp)
		{
			float new_cooking_time = item_to_cook.GetCookingTime() + COOKING_FOOD_TIME_INC_VALUE * cooking_time_coef;
			item_to_cook.SetCookingTime(new_cooking_time);
			
			//progress to next stage
			if (item_to_cook.GetCookingTime() >= food_time_to_cook)
			{
				//if max temp is defined check next food stage
				if (food_max_temp >= 0)
				{
					if (food_temperature > food_max_temp && item_to_cook.GetFoodStageType() != FoodStageType.BURNED)
					{
						new_stage_type = FoodStageType.BURNED;
					}
				}
				
				//! Change food stage
				item_to_cook.ChangeFoodStage(new_stage_type);
				//! Remove all modifiers
				item_to_cook.RemoveAllAgentsExcept(eAgents.BRAIN);
				
				if (cooking_equipment)
				{
					if (cooking_method == CookingMethodType.BAKING)
					{
						ItemBase lard = GetItemTypeFromCargo(COOKING_INGREDIENT_LARD, cooking_equipment);
						if (lard)
						{
							//decrease lard quantity
							float lardQuantity = lard.GetQuantity() - COOKING_LARD_DECREASE_COEF;
							lardQuantity = Math.Clamp(lardQuantity, 0, lard.GetQuantityMax());
							lard.SetQuantity(lardQuantity);
						}
						else
						{
							//! any foodstage without lard
							DecreaseCookedItemQuantity(item_to_cook, COOKING_FOOD_QUANTITY_DECREASE_AMOUNT_NONE);
						}
					}
				}
				else
				{
					//! any foodstage without lard 
					DecreaseCookedItemQuantity(item_to_cook, COOKING_FOOD_QUANTITY_DECREASE_AMOUNT_NONE);
				}
				
				//reset cooking time
				item_to_cook.SetCookingTime(0);

				return 1;
			}
		}
		
		return 0;
	}
	
	//Returns 1 if the item changed its cooking stage, 0 if not
	protected int UpdateCookingStateOnStick( Edible_Base item_to_cook, float cook_time_inc )
	{
		//food properties
		float food_temperature = item_to_cook.GetTemperature();
		
		//{min_temperature, time_to_cook, max_temperature (optional)}
		//get next stage name - if next stage is not defined, next stage name will be empty "" and no cooking properties (food_min_temp, food_time_to_cook, food_max_temp) will be set
		FoodStageType new_stage_type = item_to_cook.GetNextFoodStageType( CookingMethodType.BAKING );
		float food_min_temp = 0;
		float food_time_to_cook = 0;
		float food_max_temp = -1;
		bool is_done = false;	// baked
		bool is_burned = false;	// burned

		//Set next stage cooking properties if next stage possible
		if ( item_to_cook.CanChangeToNewStage( CookingMethodType.BAKING ) )
		{
			array<float> next_stage_cooking_properties = new array<float>;
			next_stage_cooking_properties = FoodStage.GetAllCookingPropertiesForStage( new_stage_type, null, item_to_cook.GetType() );
			
			food_min_temp = next_stage_cooking_properties.Get( eCookingPropertyIndices.MIN_TEMP );
			food_time_to_cook = next_stage_cooking_properties.Get( eCookingPropertyIndices.COOK_TIME );
			// The last element is optional and might not exist
			if ( next_stage_cooking_properties.Count() > 2 )
				food_max_temp = next_stage_cooking_properties.Get( eCookingPropertyIndices.MAX_TEMP );
		}
		
		
		// refresh audio
		if (item_to_cook.GetInventory().IsAttachment())
		{
			item_to_cook.MakeSoundsOnClient(true);
			//add temperature
			AddTemperatureToItem(item_to_cook, null, food_min_temp);
		}
		
		//add cooking time if the food can be cooked by this method
		if ( food_min_temp > 0 && food_temperature >= food_min_temp )
		{
			float new_cooking_time = item_to_cook.GetCookingTime() + cook_time_inc;
			item_to_cook.SetCookingTime( new_cooking_time );
			
			//progress to next stage
			if ( item_to_cook.GetCookingTime() >= food_time_to_cook )
			{
				//if max temp is defined check next food stage
				if ( food_max_temp >= 0 )
				{
					if ( food_temperature > food_max_temp && item_to_cook.GetFoodStageType() != FoodStageType.BURNED )
					{
						new_stage_type = FoodStageType.BURNED;
					}
				}
				
				//change food stage
				item_to_cook.ChangeFoodStage( new_stage_type );
				item_to_cook.RemoveAllAgentsExcept(eAgents.BRAIN);
				
				DecreaseCookedItemQuantity(item_to_cook, COOKING_FOOD_QUANTITY_DECREASE_AMOUNT_NONE);

				//reset cooking time
				item_to_cook.SetCookingTime( 0 );
				return 1;
			}
		}
		
		return 0;
	}
	
	void SmokeItem(Edible_Base item_to_cook, float cook_time_inc)
	{
		if (item_to_cook)
		{
			float new_cook_time = item_to_cook.GetCookingTime() + cook_time_inc;
			float drying_cook_time = FoodStage.GetCookingPropertyFromIndex(eCookingPropertyIndices.COOK_TIME, FoodStageType.DRIED, null, item_to_cook.GetType());

			switch (item_to_cook.GetFoodStageType())
			{
			case FoodStageType.RAW:
				item_to_cook.SetCookingTime(new_cook_time);

				if (item_to_cook.GetCookingTime() >= drying_cook_time)
				{
					item_to_cook.ChangeFoodStage(FoodStageType.DRIED);
					item_to_cook.RemoveAllAgentsExcept(eAgents.BRAIN);
					item_to_cook.SetCookingTime(0);
				}
			break;
			default:
				item_to_cook.SetCookingTime(new_cook_time);

				if (item_to_cook.GetCookingTime() >= drying_cook_time)
				{
					item_to_cook.ChangeFoodStage(FoodStageType.BURNED);
					item_to_cook.RemoveAllAgents();
					item_to_cook.SetCookingTime(0);
				}
			break;
			}
		}
	}
	
	void TerminateCookingSounds(ItemBase pItem)
	{
		Edible_Base edible;
		if (pItem)
		{
			if (pItem.GetInventory()) // cookware
			{
				CargoBase cargo = pItem.GetInventory().GetCargo();
				if (cargo)
				{
					for (int i = 0; i < cargo.GetItemCount(); i++)
					{
						edible = Edible_Base.Cast(cargo.GetItem(i));
						if (edible)
						{
							edible.MakeSoundsOnClient(false);
						}
					}
				}
			}
			else
			{
				edible = Edible_Base.Cast(pItem);
				if (edible)
				{
					edible.MakeSoundsOnClient(false);
				}
			}
		}
	}
	
	//! Cooking data
	protected ItemBase GetItemTypeFromCargo( typename item_type, ItemBase cooking_equipment )
	{
		CargoBase cargo = cooking_equipment.GetInventory().GetCargo();
		if (cargo)
		{
			for (int i = 0; i < cargo.GetItemCount(); i++)
			{
				EntityAI entity = cargo.GetItem(i);
				if (entity.Type() == item_type)
				{
					ItemBase item = ItemBase.Cast(entity);
		
					return item;
				}
			}
		}
		
		return null;
	}

	//! DEPREACTED
	protected CookingMethodType GetCookingMethod(ItemBase cooking_equipment)
	{
		if (cooking_equipment.Type() == COOKING_EQUIPMENT_POT || cooking_equipment.Type() == COOKING_EQUIPMENT_CAULDRON)
		{
			//has water, but not petrol dammit X)
			if (cooking_equipment.GetQuantity() > 0 && cooking_equipment.GetLiquidType() != LIQUID_GASOLINE)
			{
				return CookingMethodType.BOILING;
			}
			
			//has lard in cargo
			if (GetItemTypeFromCargo(COOKING_INGREDIENT_LARD, cooking_equipment))
			{
				return CookingMethodType.BAKING;
			}
			return CookingMethodType.DRYING;
		}
		
		if (cooking_equipment.Type() == COOKING_EQUIPMENT_FRYINGPAN)
		{
			if (GetItemTypeFromCargo(COOKING_INGREDIENT_LARD, cooking_equipment))
			{
				return CookingMethodType.BAKING;
			}
			return CookingMethodType.DRYING;
		}

		return CookingMethodType.NONE;
	}
	
	protected Param2<CookingMethodType, float> GetCookingMethodWithTimeOverride(ItemBase cooking_equipment)
	{
		Param2<CookingMethodType, float> val = new Param2<CookingMethodType, float>(CookingMethodType.NONE, TIME_WITH_SUPPORT_MATERIAL_COEF);

		switch (cooking_equipment.Type())
		{
		case COOKING_EQUIPMENT_POT:
		case COOKING_EQUIPMENT_CAULDRON:
		case COOKING_EQUIPMENT_FRYINGPAN:
			if (cooking_equipment.GetQuantity() > 0)
			{
				if (cooking_equipment.GetLiquidType() == LIQUID_GASOLINE)
				{
					//! when cooking in gasoline, jump to drying state(will be burnt then)
					val = new Param2<CookingMethodType, float>(CookingMethodType.DRYING, TIME_WITHOUT_SUPPORT_MATERIAL_COEF);
					break;
				}

				val = new Param2<CookingMethodType, float>(CookingMethodType.BOILING, TIME_WITH_SUPPORT_MATERIAL_COEF);
				break;
			}
			
			if (GetItemTypeFromCargo(COOKING_INGREDIENT_LARD, cooking_equipment))
			{
				//has lard in cargo, slower process
				val = new Param2<CookingMethodType, float>(CookingMethodType.BAKING, TIME_WITH_SUPPORT_MATERIAL_COEF);
				break;
			}
			
			val = new Param2<CookingMethodType, float>(CookingMethodType.BAKING, TIME_WITHOUT_SUPPORT_MATERIAL_COEF);
		break;
		
		default:
			val = new Param2<CookingMethodType, float>(CookingMethodType.BAKING, TIME_WITHOUT_SUPPORT_MATERIAL_COEF);
		break;
		}

		return val;
	}
	
	Edible_Base GetFoodOnStick( ItemBase stick_item )
	{
		Edible_Base food_on_stick = Edible_Base.Cast( stick_item.GetAttachmentByType( Edible_Base ) );

		return food_on_stick;
	}
	
	float GetTimeToCook( Edible_Base item_to_cook, CookingMethodType cooking_method )
	{
		FoodStageType food_stage_type = item_to_cook.GetNextFoodStageType( cooking_method );
		return FoodStage.GetCookingPropertyFromIndex( eCookingPropertyIndices.COOK_TIME, food_stage_type, null, item_to_cook.GetType());
	}

	float GetMinTempToCook( Edible_Base item_to_cook, CookingMethodType cooking_method )
	{
		FoodStageType food_stage_type = item_to_cook.GetNextFoodStageType( cooking_method );
		return FoodStage.GetCookingPropertyFromIndex( eCookingPropertyIndices.MIN_TEMP, food_stage_type, null, item_to_cook.GetType());
	}
	
	//add temperature to item
	protected void AddTemperatureToItem( ItemBase cooked_item, ItemBase cooking_equipment, float min_temperature )
	{
		if ( cooked_item.GetTemperatureMax() >= FireplaceBase.PARAM_ITEM_HEAT_MIN_TEMP )
		{
			float item_temperature = cooked_item.GetTemperature();
			
			//set actual cooking temperature
			float actual_cooking_temp = DEFAULT_COOKING_TEMPERATURE;	//default
			if ( cooking_equipment )
			{
				actual_cooking_temp = cooking_equipment.GetTemperature();
			}
			
			//add temperature
			if ( actual_cooking_temp > item_temperature )
			{
				item_temperature = actual_cooking_temp * 0.5;
				item_temperature = Math.Clamp( item_temperature, min_temperature, FOOD_MAX_COOKING_TEMPERATURE );
				
				//set new temperature
				if ( GetGame() && GetGame().IsServer() )
				{
					cooked_item.SetTemperature( item_temperature );
				}
			}
		}
	}
	
	protected void DecreaseCookedItemQuantity(notnull Edible_Base pItem, float pAmount = 0.0)
	{
		if (GetGame().IsServer())
		{
			float quantity = pItem.GetQuantity() - pAmount;
			quantity = Math.Clamp(quantity, 0, pItem.GetQuantityMax());
			pItem.SetQuantity(quantity);
		}
	}
}
