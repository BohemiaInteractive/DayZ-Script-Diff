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
	static const float COOKING_FOOD_QUANTITY_DECREASE_AMOUNT_NONE 	= 25;	//! how many units from quantity of item are removed at each FoodStage change when support material is NOT used
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
	
	protected float m_UpdateTime = 1; //set by the item/system using cooking
	
	void SetCookingUpdateTime(float val)
	{
		m_UpdateTime = val;
	}
	
	void ProcessItemToCook(notnull ItemBase pItem, ItemBase cookingEquip, Param2<CookingMethodType, float> pCookingMethod, out Param2<bool, bool> pStateFlags)
	{
		Edible_Base item_to_cook = Edible_Base.Cast(pItem);
		
		//! state flags are in order: is_done, is_burned
		pStateFlags = new Param2<bool, bool>(false, false);
		
		if (item_to_cook && item_to_cook.CanBeCooked())
		{
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
			//add temperature to item
			if (pItem != cookingEquip) //already handled by the fireplace directly!
				AddTemperatureToItem(pItem, null, 0);
			
			//damage item that can actually overheat?
			if (pItem.CanItemOverheat())
			{
				if (pItem.IsItemOverheated())
				{
					if (pItem.HasQuantity() && pItem.GetQuantityNormalized() > 0)
					{
						pItem.AddQuantity(-LIQUID_VAPOR_QUANTITY,!pItem.IsLiquidContainer()); //TODO: use other constant here, or calculate from qtyMax..
					}
					else
					{
						pItem.DecreaseHealth(PARAM_BURN_DAMAGE_COEF * 100);
					}
				}
			}
			else
			{
				pItem.DecreaseHealth(PARAM_BURN_DAMAGE_COEF * 100); //pItem.IsKindOf("Grenade_Base")
			}
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
		if (bottle_base)
		{
			float cookingEquipmentTemp = bottle_base.GetTemperature();
			int liquidType = bottle_base.GetLiquidType();
			
			//handle water boiling
			if (liquidType != LIQUID_NONE && cookingEquipmentTemp >= Liquid.GetBoilThreshold(liquidType))
			{
				//remove agents
				bottle_base.RemoveAllAgentsExcept(eAgents.HEAVYMETAL);
				
				//vaporize liquid
				if (bottle_base.IsItemOverheated())
					bottle_base.AddQuantity(-LIQUID_VAPOR_QUANTITY);
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
		
		//Set next stage cooking properties if next stage possible
		if (item_to_cook.CanChangeToNewStage(cooking_method)) // new_stage_type != NONE
		{
			array<float> next_stage_cooking_properties = new array<float>();
			next_stage_cooking_properties = FoodStage.GetAllCookingPropertiesForStage(new_stage_type, null, item_to_cook.GetType());
			
			food_min_temp = next_stage_cooking_properties.Get(eCookingPropertyIndices.MIN_TEMP); //checked after temperature is changed
			food_time_to_cook = next_stage_cooking_properties.Get(eCookingPropertyIndices.COOK_TIME);
		}
		
		//add temperature
		AddTemperatureToItem(item_to_cook, cooking_equipment, food_min_temp);
		
		//decrease qty of burned items (or cookable items that can't be burned)
		if (item_to_cook.IsItemOverheated())
			DecreaseCookedItemQuantity(item_to_cook,COOKING_FOOD_QUANTITY_DECREASE_AMOUNT_NONE);
		
		//add cooking time if the food can be cooked by this method
		if (food_min_temp > 0 && food_temperature >= food_min_temp)
		{
			//! enable cooking SoundEvent
			item_to_cook.MakeSoundsOnClient(true,cooking_method);
			
			float new_cooking_time = item_to_cook.GetCookingTime() + COOKING_FOOD_TIME_INC_VALUE * cooking_time_coef;
			item_to_cook.SetCookingTime(new_cooking_time);
			
			//progress to next stage
			if (item_to_cook.GetCookingTime() >= food_time_to_cook)
			{
				//! Change food stage to new, IF DIFFERENT
				if (item_to_cook.GetFoodStageType() != new_stage_type)
				{
					item_to_cook.ChangeFoodStage(new_stage_type);
					
					if (cooking_equipment && cooking_equipment != item_to_cook)
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
				}
				
				//reset cooking time
				item_to_cook.ResetCookingTime();
				
				return 1;
			}
		}
		else
		{
			item_to_cook.MakeSoundsOnClient(false);
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
		bool is_done = false;	// baked
		bool is_burned = false;	// burned
		
		//Set next stage cooking properties if next stage possible
		if ( item_to_cook.CanChangeToNewStage( CookingMethodType.BAKING ) )
		{
			array<float> next_stage_cooking_properties = new array<float>;
			next_stage_cooking_properties = FoodStage.GetAllCookingPropertiesForStage( new_stage_type, null, item_to_cook.GetType() );
			
			food_min_temp = next_stage_cooking_properties.Get( eCookingPropertyIndices.MIN_TEMP );
			food_time_to_cook = next_stage_cooking_properties.Get( eCookingPropertyIndices.COOK_TIME );
		}
		
		if (item_to_cook.GetInventory().IsAttachment())
		{
			//add temperature
			AddTemperatureToItem(item_to_cook, null, food_min_temp);
		}
		
		//add cooking time if the food can be cooked by this method
		if (food_min_temp > 0 && food_temperature >= food_min_temp)
		{
			//refresh audio
			item_to_cook.MakeSoundsOnClient(true, CookingMethodType.BAKING);
			
			float new_cooking_time = item_to_cook.GetCookingTime() + cook_time_inc;
			item_to_cook.SetCookingTime( new_cooking_time );
			
			//progress to next stage
			if (item_to_cook.GetCookingTime() >= food_time_to_cook)
			{
				//! Change food stage to new, IF DIFFERENT
				if (item_to_cook.GetFoodStageType() != new_stage_type)
				{
					item_to_cook.ChangeFoodStage(new_stage_type);
					
					DecreaseCookedItemQuantity(item_to_cook, COOKING_FOOD_QUANTITY_DECREASE_AMOUNT_NONE);
				}
				//reset cooking time
				item_to_cook.ResetCookingTime();
				return 1;
			}
		}
		else
		{
			item_to_cook.MakeSoundsOnClient(false);
		}
		
		return 0;
	}
	
	void SmokeItem(Edible_Base item_to_cook, float cook_time_inc)
	{
		if (item_to_cook)
		{
			float new_cook_time = item_to_cook.GetCookingTime() + cook_time_inc;
			float drying_cook_time = FoodStage.GetCookingPropertyFromIndex(eCookingPropertyIndices.COOK_TIME, FoodStageType.DRIED, null, item_to_cook.GetType());
			float drying_cook_temp = FoodStage.GetCookingPropertyFromIndex(eCookingPropertyIndices.MIN_TEMP, FoodStageType.DRIED, null, item_to_cook.GetType());
			float itemTemp = item_to_cook.GetTemperature();
			
			if (itemTemp >= drying_cook_temp)
			{
				switch (item_to_cook.GetFoodStageType())
				{
					case FoodStageType.RAW:
						item_to_cook.SetCookingTime(new_cook_time);
						
						if (item_to_cook.GetCookingTime() >= drying_cook_time)
						{
							item_to_cook.ChangeFoodStage(FoodStageType.DRIED);
							item_to_cook.ResetCookingTime();
						}
					break;
					
					default:
						item_to_cook.SetCookingTime(new_cook_time);
						
						if (item_to_cook.GetCookingTime() >= drying_cook_time)
						{
							item_to_cook.ChangeFoodStage(FoodStageType.BURNED);
							item_to_cook.ResetCookingTime();
						}
					break;
				}
			}
		}
	}
	
	void TerminateCookingSounds(ItemBase pItem)
	{
		Edible_Base edible;
		if (pItem)
		{
			CargoBase cargo = pItem.GetInventory().GetCargo();
			if (cargo) // cookware
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

				if (cooking_equipment.GetInventory().GetCargo().GetItemCount() > 0)
				{
					val = new Param2<CookingMethodType, float>(CookingMethodType.BAKING, TIME_WITHOUT_SUPPORT_MATERIAL_COEF);
					break;
				}
				
				val = new Param2<CookingMethodType, float>(CookingMethodType.NONE, TIME_WITHOUT_SUPPORT_MATERIAL_COEF);
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
		if (!GetGame().IsServer())
			return;
		
		if (cooked_item == cooking_equipment) //solves direct cooking double heating
			return;
		
		if (cooked_item.CanHaveTemperature())
		{
			float itemTemp = cooked_item.GetTemperature();
			//set target temperature
			float targetTemp = DEFAULT_COOKING_TEMPERATURE;	//default for direct attachments (direct cooking slots, smoking...)
			if (cooking_equipment)
				targetTemp = cooking_equipment.GetTemperature();
			
			//adjust temperature
			if (targetTemp != itemTemp)
			{
				float heatPermCoef = 1.0;
				if (cooking_equipment)
					heatPermCoef = cooking_equipment.GetHeatPermeabilityCoef();
				heatPermCoef *= cooked_item.GetHeatPermeabilityCoef();
				float tempCoef;
				
				if (itemTemp < min_temperature && targetTemp > itemTemp) //heating 'catch-up' only
					tempCoef = GameConstants.TEMP_COEF_COOKING_CATCHUP;
				else
					tempCoef = GameConstants.TEMP_COEF_COOKING_DEFAULT;
				
				cooked_item.SetTemperatureEx(new TemperatureDataInterpolated(targetTemp,ETemperatureAccessTypes.ACCESS_COOKING,m_UpdateTime,tempCoef,heatPermCoef));
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
