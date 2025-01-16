/*
Example of skinning config which should be inside animal's base class:
class Skinning
{
	// All classes in this scope are parsed, so they can have any name.
	class ObtainedSteaks 
	{
		item = "DeerSteakMeat"; // item to spawn
		count = 10; // How many items to spawn
		transferToolDamageCoef = 1; // Optional: How much tool's damage is transfered to item's damage.
		
		// Make sure to have only 1 of the following quantity paramenter
		quantity = 100; // Optional: Set item's quantity
		quantityCoef = 1; // Optional: Set item's quantity (in coefficient)
		quantityMinMax[] = {100, 200}; // Optional: Set item's quantity within min/max values
		quantityMinMaxCoef[] = {0, 1}; // Optional: Set item's quantity (in coefficient) within min/max values
	};
};
*/

class ActionSkinningCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.SKIN);
	}
}

class ActionSkinning: ActionContinuousBase
{	
	void ActionSkinning()
	{
		m_CallbackClass = ActionSkinningCB;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_MEDIUM;
		
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_ANIMALSKINNING;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
		m_FullBody = true;
		m_Text = "#skin";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem		= new CCINonRuined();
		m_ConditionTarget	= new CCTCursorNoRuinCheck();
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		Object targetObject = target.GetObject();
		
		if (targetObject.CanBeSkinned() && !targetObject.IsAlive())
		{
			EntityAI target_EAI;
			if (Class.CastTo(target_EAI, targetObject) && target_EAI.CanBeSkinnedWith(item))
				return true;
		}
		
		return false;
	}
	
	// Spawns the loot according to the Skinning class in the dead agent's config
	override void OnFinishProgressServer(ActionData action_data)
	{
		super.OnFinishProgressServer(action_data);
		
		Object targetObject = action_data.m_Target.GetObject();
		
		// Mark the body as skinned to forbid another skinning action on it
		EntityAI body = EntityAI.Cast(targetObject);
		body.SetAsSkinned();
		
		MiscGameplayFunctions.RemoveAllAttachedChildrenByTypename(body, {Bolt_Base});
		
		HandlePlayerBody(action_data);
		SpawnItems(action_data);
		
		if (body)
		{
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Call(GetGame().ObjectDelete, body);
		}
		
		MiscGameplayFunctions.DealAbsoluteDmg(action_data.m_MainItem, UADamageApplied.SKINNING);
		
		PluginLifespan moduleLifespan = PluginLifespan.Cast(GetPlugin(PluginLifespan));
		moduleLifespan.UpdateBloodyHandsVisibility(action_data.m_Player, true);
		action_data.m_Player.SetBloodyHandsPenaltyChancePerAgent(eAgents.SALMONELLA, body.GetSkinningBloodInfectionChance(eAgents.SALMONELLA));
	}
	
	// Spawns an organ defined in the given config
	ItemBase CreateOrgan(PlayerBase player, vector body_pos, string item_to_spawn, string cfg_skinning_organ_class, ItemBase tool)
	{
		// Create item from config
		ItemBase added_item;
		vector posHead;
		MiscGameplayFunctions.GetHeadBonePos(player,posHead);
		vector posRandom = MiscGameplayFunctions.GetRandomizedPositionVerified(posHead,body_pos,UAItemsSpreadRadius.NARROW,player);
		Class.CastTo(added_item,  GetGame().CreateObjectEx(item_to_spawn, posRandom, ECE_PLACE_ON_SURFACE));
		
		// Check if skinning is configured for this body
		if (!added_item)
			return null;
		
		// Set item's quantity from config, if it's defined there.
		float item_quantity = 0;
		array<float> quant_min_max = new array<float>;
		array<float> quant_min_max_coef = new array<float>;
		
		GetGame().ConfigGetFloatArray(cfg_skinning_organ_class + "quantityMinMax", quant_min_max);
		GetGame().ConfigGetFloatArray(cfg_skinning_organ_class + "quantityMinMaxCoef", quant_min_max_coef);
		
		// Read config for quantity value
		if (quant_min_max.Count() > 0)
		{
			float soft_skill_manipulated_value = (quant_min_max.Get(0)+ quant_min_max.Get(1)) / 2;
			item_quantity = Math.RandomFloat(soft_skill_manipulated_value, quant_min_max.Get(1));
		}
		
		if (quant_min_max_coef.Count() > 0)
		{
			float coef = Math.RandomFloat(quant_min_max_coef.Get(0), quant_min_max_coef.Get(1));
			item_quantity = added_item.GetQuantityMax() * coef;
		}
		
		if (GetGame().ConfigGetFloat(cfg_skinning_organ_class + "quantity") > 0)
			item_quantity = g_Game.ConfigGetFloat(cfg_skinning_organ_class + "quantity");
		
		if (GetGame().ConfigGetFloat(cfg_skinning_organ_class + "quantityCoef") > 0)
		{
			float coef2 = g_Game.ConfigGetFloat(cfg_skinning_organ_class + "quantityCoef");
			item_quantity = added_item.GetQuantityMax() * coef2;
		}
		
		if (item_quantity > 0)
		{
			item_quantity = Math.Round(item_quantity);
			added_item.SetQuantity(item_quantity, false);
		}
		
		// Transfer tool's damage to the item's condition
		float item_apply_tool_damage_coef = GetGame().ConfigGetFloat(cfg_skinning_organ_class + "transferToolDamageCoef");
		
		if (item_apply_tool_damage_coef > 0)
		{
			float tool_dmg_coef = 1 - tool.GetHealth01();
			float organ_dmg_coef = tool_dmg_coef * item_apply_tool_damage_coef;
			added_item.DecreaseHealthCoef(organ_dmg_coef);
		}
		
		added_item.InsertAgent(eAgents.SALMONELLA, 1);
		return added_item;
	}
	
	override void OnFinishProgressClient(ActionData action_data)
	{
		super.OnFinishProgressClient(action_data);
		
		if (action_data.m_Target) 
		{
			Object target_obj = action_data.m_Target.GetObject();
			
			if (target_obj.IsKindOf("Animal_CapreolusCapreolus") || target_obj.IsKindOf("Animal_CapreolusCapreolusF") || target_obj.IsKindOf("Animal_CervusElaphus") || target_obj.IsKindOf("Animal_CervusElaphusF"))
			{
				GetGame().GetAnalyticsClient().OnActionFinishedGutDeer();
			}
		}
	}
	
	//! This section drops all clothes (and attachments) from the dead player before deleting their body
	void HandlePlayerBody(ActionData action_data)
	{
		PlayerBase body;
		if (Class.CastTo(body,action_data.m_Target.GetObject()))
		{
			if (body.IsRestrained() && body.GetHumanInventory().GetEntityInHands())
				MiscGameplayFunctions.TransformRestrainItem(body.GetHumanInventory().GetEntityInHands(), null, action_data.m_Player, body);

			//Remove splint if target is wearing one
			if (body.IsWearingSplint())
			{
				EntityAI entity = action_data.m_Player.SpawnEntityOnGroundOnCursorDir("Splint", 0.5);
				ItemBase newItem = ItemBase.Cast(entity);
				EntityAI attachment = body.GetItemOnSlot("Splint_Right");
				if (attachment && attachment.GetType() == "Splint_Applied")
				{
					if (newItem)
					{
						MiscGameplayFunctions.TransferItemProperties(attachment, newItem);
						//Lower health level of splint after use
						if (newItem.GetHealthLevel() < 4)
						{
							int newDmgLevel = newItem.GetHealthLevel() + 1;
							float max = newItem.GetMaxHealth("", "");
							
							switch (newDmgLevel)
							{
								case GameConstants.STATE_BADLY_DAMAGED:
									newItem.SetHealth("", "", max * GameConstants.DAMAGE_BADLY_DAMAGED_VALUE);
									break;
			
								case GameConstants.STATE_DAMAGED:
									newItem.SetHealth("", "", max * GameConstants.DAMAGE_DAMAGED_VALUE);
									break;
			
								case GameConstants.STATE_WORN:
									newItem.SetHealth("", "", max * GameConstants.DAMAGE_WORN_VALUE);
									break;
								
								case GameConstants.STATE_RUINED:
									newItem.SetHealth("", "", max * GameConstants.DAMAGE_RUINED_VALUE);
									break;
									
								default:
									break;
							}
						}
					}

					attachment.Delete();
				}
			}
			
			int deadBodyLifetime;
			if (GetCEApi())
			{
				deadBodyLifetime = GetCEApi().GetCEGlobalInt("CleanupLifetimeDeadPlayer");
				if (deadBodyLifetime <= 0)
					deadBodyLifetime = 3600;
			}

			DropInventoryItems(body,deadBodyLifetime);
		}
	}
	
	void DropInventoryItems(PlayerBase body, float newLifetime)
	{
		array<EntityAI> children = new array<EntityAI>;
		body.GetInventory().EnumerateInventory(InventoryTraversalType.LEVELORDER, children);
		foreach (EntityAI child : children)
		{
			if (child)
			{
				child.SetLifetime(newLifetime);
				body.GetInventory().DropEntity(InventoryMode.SERVER, body, child);
			}
		}
	}
	
	void SpawnItems(ActionData action_data)
	{
		EntityAI body = EntityAI.Cast(action_data.m_Target.GetObject());
		
		// Get config path to the animal
		string cfg_animal_class_path = "cfgVehicles " + body.GetType() + " " + "Skinning ";
		vector bodyPosition = body.GetPosition();
		
		if (!g_Game.ConfigIsExisting(cfg_animal_class_path))
		{
			Debug.Log("Failed to find class 'Skinning' in the config of: " + body.GetType());
			return;
		}
		
		// Getting item type from the config
		int child_count = g_Game.ConfigGetChildrenCount(cfg_animal_class_path);
		
		string item_to_spawn;
		string cfg_skinning_organ_class;
		// Parsing of the 'Skinning' class in the config of the dead body
		for (int i1 = 0; i1 < child_count; i1++)
		{
			// To make configuration as convenient as possible, all classes are parsed and parameters are read
			g_Game.ConfigGetChildName(cfg_animal_class_path, i1, cfg_skinning_organ_class); // out cfg_skinning_organ_class
			cfg_skinning_organ_class = cfg_animal_class_path + cfg_skinning_organ_class + " ";
			g_Game.ConfigGetText(cfg_skinning_organ_class + "item", item_to_spawn); // out item_to_spawn
			
			if (item_to_spawn != "") // Makes sure to ignore incompatible parameters in the Skinning class of the agent
			{
				// Spawning items in action_data.m_Player's inventory
				int item_count = g_Game.ConfigGetInt(cfg_skinning_organ_class + "count");
				
				array<string> itemZones = new array<string>;
				array<float> itemCount = new array<float>;
				float zoneDmg = 0;
				
				GetGame().ConfigGetTextArray(cfg_skinning_organ_class + "itemZones", itemZones);
				GetGame().ConfigGetFloatArray(cfg_skinning_organ_class + "countByZone", itemCount);
				
				if (itemCount.Count() > 0)
				{
					item_count = 0;
					for (int z = 0; z < itemZones.Count(); z++)
					{
						zoneDmg = body.GetHealth01(itemZones[z], "Health");
						zoneDmg *= itemCount[z]; //just re-using variable
						item_count += Math.Floor(zoneDmg);
					}
				}
				
				for (int i2 = 0; i2 < item_count; i2++)
				{
					ItemBase spawn_result = CreateOrgan(action_data.m_Player, bodyPosition, item_to_spawn, cfg_skinning_organ_class, action_data.m_MainItem);
					
					//Damage pelts based on the average values on itemZones
					//It only works if the "quantityCoef" in the config is more than 0 
					float qtCoeff = GetGame().ConfigGetFloat(cfg_skinning_organ_class + "quantityCoef");
					if (qtCoeff > 0)
					{
						float avgDmgZones = 0;
						for (int c2 = 0; c2 < itemZones.Count(); c2++)
						{
							avgDmgZones += body.GetHealth01(itemZones[c2], "Health");
						}
						
						avgDmgZones = avgDmgZones/itemZones.Count(); // Evaluate the average Health
						
						if (spawn_result)
							spawn_result.SetHealth01("","", avgDmgZones);
					}
					
					// inherit temperature
					if (body.CanHaveTemperature() && spawn_result.CanHaveTemperature())
					{
						spawn_result.SetTemperatureDirect(body.GetTemperature());
						spawn_result.SetFrozen(body.GetIsFrozen());
					}
					
					// handle fat/guts from human bodies
					if ((item_to_spawn == "Lard") || (item_to_spawn == "Guts"))
					{
						if (body.IsKindOf("SurvivorBase"))
						{
							spawn_result.InsertAgent(eAgents.BRAIN, 1);
						}
					}
				}
			}
		}
	}
	
	//DEPRECATED
	vector GetRandomPos(vector body_pos)
	{
		return body_pos + Vector(Math.RandomFloat01() - 0.5, 0, Math.RandomFloat01() - 0.5);
	}
}
