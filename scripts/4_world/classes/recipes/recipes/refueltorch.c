// This recipe adds fuel to torch

class RefuelTorch extends RecipeBase
{	
	override void Init()
	{
		m_Name = "#STR_RefuelTorch0";
		m_IsInstaRecipe = false;//should this recipe be performed instantly without animation
		m_AnimationLength = 0.25;//animation length in relative time units
		m_Specialty = 0.02;// value > 0 for roughness, value < 0 for precision
		
		//conditions
		m_MinDamageIngredient[0] = -1;//-1 = disable check
		m_MaxDamageIngredient[0] = -1;//-1 = disable check
		
		m_MinQuantityIngredient[0] = -1;//-1 = disable check
		m_MaxQuantityIngredient[0] = -1;//-1 = disable check
		
		m_MinDamageIngredient[1] = -1;//-1 = disable check
		m_MaxDamageIngredient[1] = -1;//-1 = disable check
		
		m_MinQuantityIngredient[1] = -1;//-1 = disable check
		m_MaxQuantityIngredient[1] = -1;//-1 = disable check
		//----------------------------------------------------------------------------------------------------------------------
		
		//INGREDIENTS
		//ingredient 1
		InsertIngredient(0,"Rag");//you can insert multiple ingredients this way
		
		m_IngredientAddHealth[0] = 0;// 0 = do nothing
		m_IngredientSetHealth[0] = -1; // -1 = do nothing
		m_IngredientAddQuantity[0] = 0;// 0 = do nothing
		m_IngredientDestroy[0] = false;//true = destroy, false = do nothing
		m_IngredientUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this ingredient
		
		//ingredient 2
		InsertIngredient(1,"Torch");//you can insert multiple ingredients this way
		InsertIngredient(1,"LongTorch");//you can insert multiple ingredients this way
		
		m_IngredientAddHealth[1] = 0;// 0 = do nothing
		m_IngredientSetHealth[1] = -1; // -1 = do nothing
		m_IngredientAddQuantity[1] = -1;// 0 = do nothing
		m_IngredientDestroy[1] = false;// false = do nothing
		m_IngredientUseSoftSkills[1] = false;// set 'true' to allow modification of the values by softskills on this ingredient
		//----------------------------------------------------------------------------------------------------------------------
		
		//result1
		//AddResult("Torch");//add results here

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
		Rag rag = Rag.Cast(ingredients[0]);
		Torch torch = Torch.Cast(ingredients[1]);
		
		if (!rag  ||  !torch)
			return false;
		
		Rag rag_on_torch = Rag.Cast(  torch.GetInventory().FindAttachment( rag.GetInventory().GetSlotId(0) )  );
		
		if (rag_on_torch) // Check if torch already has rag with 100% quantity
		{
			if (rag_on_torch.GetQuantity() == rag_on_torch.GetQuantityMax() )
			{
				return false;
			}
		}
		
		return true;
	}

	override void Do(ItemBase ingredients[], PlayerBase player,array<ItemBase> results, float specialty_weight)//gets called upon recipe's completion
	{
		Rag rag = Rag.Cast(ingredients[0]);
		Torch torch = Torch.Cast(ingredients[1]);
		
		if ( !GetGame().IsMultiplayer() )
		{
			InventoryLocation loc = new InventoryLocation;
			rag.GetInventory().GetCurrentInventoryLocation( loc );
			player.GetInventory().ClearInventoryReservationEx( rag, loc );
		}	
		
		Rag rag_on_torch = Rag.Cast(  torch.GetInventory().FindAttachment( rag.GetInventory().GetSlotId(0) )  );
		
		if (rag_on_torch)
		{
			rag_on_torch.CombineItems(rag, false);
		}
		else
		{
			if ( GetGame().IsServer()  &&  GetGame().IsMultiplayer() )
			{
				player.ServerTakeEntityToTargetAttachment(torch, rag); // multiplayer server side
			}
			else
			{
				player.LocalTakeEntityToTargetAttachment(torch, rag); // single player or multiplayer client side
			}
		}
	}
};
