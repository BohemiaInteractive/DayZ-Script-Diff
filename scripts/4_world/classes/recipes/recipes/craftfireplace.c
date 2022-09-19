class CraftFireplace extends RecipeBase
{
	override void Init()
	{
		m_Name = "#STR_CraftFireplace0";
		m_IsInstaRecipe = false;//should this recipe be performed instantly without animation
		m_AnimationLength = 1;//animation length in relative time units
		m_Specialty = 0.02;// value > 0 for roughness, value < 0 for precision
		
		
		//conditions
		m_MinDamageIngredient[0] = -1;//-1 = disable check
		m_MaxDamageIngredient[0] = 3;//-1 = disable check
		
		m_MinQuantityIngredient[0] = 1;//-1 = disable check
		m_MaxQuantityIngredient[0] = -1;//-1 = disable check
		
		m_MinDamageIngredient[1] = -1;//-1 = disable check
		m_MaxDamageIngredient[1] = 3;//-1 = disable check
		
		m_MinQuantityIngredient[1] = 1;//-1 = disable check
		m_MaxQuantityIngredient[1] = -1;//-1 = disable check
		//----------------------------------------------------------------------------------------------------------------------
		
		//INGREDIENTS
		//ingredient 1
		InsertIngredient(0,"WoodenStick");//you can insert multiple ingredients this way
		InsertIngredient(0,"Firewood");//you can insert multiple ingredients this way
		InsertIngredient(0,"Rag");//you can insert multiple ingredients this way
		InsertIngredient(0,"BandageDressing");//you can insert multiple ingredients this way
		InsertIngredient(0,"Paper");//you can insert multiple ingredients this way
		InsertIngredient(0,"ItemBook");//you can insert multiple ingredients this way
		InsertIngredient(0,"Bark_Birch");//you can insert multiple ingredients this way
		InsertIngredient(0,"Bark_Oak");//you can insert multiple ingredients this way
		
		m_IngredientAddHealth[0] = 0;// 0 = do nothing
		m_IngredientSetHealth[0] = -1; // -1 = do nothing
		m_IngredientAddQuantity[0] = 0;// 0 = do nothing
		m_IngredientDestroy[0] = false;//true = destroy, false = do nothing
		m_IngredientUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this ingredient
		
		//ingredient 2
		InsertIngredient(1,"WoodenStick");//you can insert multiple ingredients this way
		InsertIngredient(1,"Firewood");//you can insert multiple ingredients this way
		InsertIngredient(1,"Rag");//you can insert multiple ingredients this way
		InsertIngredient(1,"BandageDressing");//you can insert multiple ingredients this way
		InsertIngredient(1,"Paper");//you can insert multiple ingredients this way
		InsertIngredient(1,"GiftWrapPaper");//you can insert multiple ingredients this way
		InsertIngredient(1,"ItemBook");//you can insert multiple ingredients this way
		InsertIngredient(1,"Bark_Birch");//you can insert multiple ingredients this way
		InsertIngredient(1,"Bark_Oak");//you can insert multiple ingredients this way
		
		m_IngredientAddHealth[1] = 0;// 0 = do nothing
		m_IngredientSetHealth[1] = -1; // -1 = do nothing
		m_IngredientAddQuantity[1] = 0;// 0 = do nothing
		m_IngredientDestroy[1] = false;// false = do nothing
		m_IngredientUseSoftSkills[1] = false;// set 'true' to allow modification of the values by softskills on this ingredient
		//----------------------------------------------------------------------------------------------------------------------
		
		//result1
		AddResult("Fireplace");//add results here

		m_ResultSetFullQuantity[0] = false;//true = set full quantity, false = do nothing
		m_ResultSetQuantity[0] = -1;//-1 = do nothing
		m_ResultSetHealth[0] = -1;//-1 = do nothing
		m_ResultInheritsHealth[0] = -2;// (value) == -1 means do nothing; a (value) >= 0 means this result will inherit health from ingredient number (value);(value) == -2 means this result will inherit health from all ingredients averaged(result_health = combined_health_of_ingredients / number_of_ingredients)
		m_ResultInheritsColor[0] = -1;// (value) == -1 means do nothing; a (value) >= 0 means this result classname will be a composite of the name provided in AddResult method and config value "color" of ingredient (value)
		m_ResultToInventory[0] = -2;//(value) == -2 spawn result on the ground;(value) == -1 place anywhere in the players inventory, (value) >= 0 means switch position with ingredient number(value)
		m_ResultUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this result
		m_ResultReplacesIngredient[0] = -1;// value == -1 means do nothing; a value >= 0 means this result will transfer item propertiesvariables, attachments etc.. from an ingredient value
	}

	override bool CanDo(ItemBase ingredients[], PlayerBase player)//final check for recipe's validity
	{
		ItemBase ingredient1 = ingredients[0];
		ItemBase ingredient2 = ingredients[1];
		
		if ( ingredient1.Type() == ingredient2.Type() || ingredient1.GetInventory().GetSlotId(0) == ingredient2.GetInventory().GetSlotId(0) )
		{
			return false;
		}
		
		return true;
	}

	override void Do( ItemBase ingredients[], PlayerBase player, array<ItemBase> results, float specialty_weight )//gets called upon recipe's completion
	{
		ItemBase result = ItemBase.Cast( results.Get( 0 ) );
		ItemBase ingredient1 = ingredients[0];
		ItemBase ingredient2 = ingredients[1];
		
		//clear inventory reservation
		if ( !GetGame().IsMultiplayer() )
		{
			InventoryLocation loc = new InventoryLocation;
			ingredient1.GetInventory().GetCurrentInventoryLocation( loc );
			player.GetInventory().ClearInventoryReservationEx( ingredient1, loc );
			ingredient2.GetInventory().GetCurrentInventoryLocation( loc );
			player.GetInventory().ClearInventoryReservationEx( ingredient2, loc );			
		}
		
		//Ingredient 1
		if ( ingredient1.GetQuantity() <= 0 )
		{
			if ( GetGame().IsServer() && GetGame().IsMultiplayer() )
			{
				player.ServerTakeEntityToTargetAttachment( result, ingredient1 ); // multiplayer server side
			}
			else
			{
				player.LocalTakeEntityToTargetAttachment( result, ingredient1 ); // single player or multiplayer client side
			}
		}
		else
		{
			string ingredient1_classname = ingredient1.GetType();
			ItemBase attachment1 = ItemBase.Cast( result.GetInventory().CreateAttachment( ingredient1_classname ) );
			MiscGameplayFunctions.TransferItemProperties( ingredient1, attachment1 );
			attachment1.SetQuantity( 1 );
			attachment1.SetCleanness(0);
			
			//set quantity to ingredient
			ingredient1.AddQuantity( -1 );
		}
		
		//Ingredient 2
		if ( ingredient2.GetQuantity() <= 0 )
		{
			if ( GetGame().IsServer() && GetGame().IsMultiplayer() )
			{
				player.ServerTakeEntityToTargetAttachment( result, ingredient2 ); // multiplayer server side
			}
			else
			{
				player.LocalTakeEntityToTargetAttachment( result, ingredient2 ); // single player or multiplayer client side
			}
		}
		else
		{
			string ingredient2_classname = ingredient2.GetType();
			ItemBase attachment2 = ItemBase.Cast( result.GetInventory().CreateAttachment( ingredient2_classname ) );
			MiscGameplayFunctions.TransferItemProperties( ingredient2, attachment2 );
			attachment2.SetQuantity( 1 );
			attachment2.SetCleanness(0);
			//set quantity to ingredient
			ingredient2.AddQuantity( -1 );
		}
	}
}
