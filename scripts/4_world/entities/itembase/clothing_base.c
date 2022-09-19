// I know the filename is stupid, but it's what the other files in this structure use..
// And I can't really make it "class Clothing_Base extends Clothing"
// since that will conflict with config and other parts of script and break mods :c
class Clothing extends Clothing_Base
{
	override bool IsClothing()
	{
		return true;
	}
	
	override bool CanHaveWetness()
	{
		return true;
	}
	
	//! Used for 'glasses' (and other colored layers triggered by attach/detach in the InventorySlots.EYEWEAR)
	int GetGlassesEffectID()
	{
		return -1;
	}
	
	//----------------------------------------------------------------
	// GameplayWidgetEffects
	array<int> GetEffectWidgetTypes()
	{
		return null;
	}
	
	// Conditions	
	override bool CanPutInCargo( EntityAI parent )
	{
		if ( !super.CanPutInCargo( parent ) )
			return false;
		
		return !parent || CanPutInCargoClothingConditions( parent );
	}
	
	bool CanPutInCargoClothingConditions( EntityAI parent )
	{
		bool is_hidden_stash_exception = false;
		
		if ( parent.IsInherited( UndergroundStash ) )
			is_hidden_stash_exception = true;
		
		if ( GetNumberOfItems() == 0 || !parent || parent.IsMan() || is_hidden_stash_exception )
		{
			EntityAI cargoParent = parent.GetHierarchyParent();
			ClothingBase parentClothing = ClothingBase.Cast(parent);
			if (cargoParent)
				return !(parent.IsClothing() && cargoParent.IsClothing()) || ( parentClothing && parentClothing.SmershException(cargoParent) );
			
			return true;
		}
		
		return false;
	}
	
	override bool CanReceiveItemIntoCargo( EntityAI item )
	{
		if (!super.CanReceiveItemIntoCargo(item))
			return false;
		
		return CanReceiveItemIntoCargoClothingConditions( item );
	}
	
	bool CanReceiveItemIntoCargoClothingConditions( EntityAI item )
	{
		EntityAI hierarchyParent = GetHierarchyParent();
		return !hierarchyParent || hierarchyParent.IsMan() || SmershException(hierarchyParent);
	}
	
	//Kind of a hack but I don't have enough time left to do larger scale reworks, sorry
	bool SmershException(EntityAI hierarchyParent)
	{
		EntityAI hp = hierarchyParent.GetHierarchyParent();
		if (hp)
		{
			if (!hp.IsMan())
				return false;
		}
		
		return IsInherited(SmershBag) && hierarchyParent.IsInherited(SmershVest);
	}
	
	override bool CanLoadItemIntoCargo(EntityAI item)
	{
		if (!super.CanLoadItemIntoCargo(item))
			return false;
		
		return CanLoadItemIntoCargoClothingConditions(item);
	}
	
	bool CanLoadItemIntoCargoClothingConditions(EntityAI item)
	{
		EntityAI parent = GetHierarchyParent();
		
		if ( parent && parent.IsInherited( UndergroundStash ) )
			return true;
				
		return !parent || parent.IsMan() || SmershException(parent);
	}
	
	//Method used to specify if a piece of eyeware can be worn under a gas mask
	//! deprecated
	bool CanWearUnderMask(EntityAI parent)
	{
		return true;
	}
	
	// -------------------------------------------------------------------------
	override void EEHealthLevelChanged(int oldLevel, int newLevel, string zone)
	{
		super.EEHealthLevelChanged(oldLevel, newLevel, zone);
		if ( !GetGame().IsDedicatedServer() )
		{
			PlayerBase player_owner = PlayerBase.Cast(GetHierarchyParent());
			
			if( player_owner )
			{
				if( player_owner.m_CorpseState != 0 )
				{
					GetGame().GetCallQueue( CALL_CATEGORY_GUI ).CallLater( player_owner.UpdateCorpseState, 0, false);
				}
			}
		}
	}
	
	override void SwitchItemSelectionTextureEx(EItemManipulationContext context, Param par = null)
	{
		super.SwitchItemSelectionTextureEx(context, par);
		
		Param1<PlayerBase> data = Param1<PlayerBase>.Cast(par);
		if (!data)
		{
			return;
		}
		
		PlayerBase player = data.param1;
		
		int personality = GetHiddenSelectionIndex("personality");
		if (personality >= 0)
		{
			string tone_mat = player.m_EmptyGloves.GetHiddenSelectionsMaterials().Get(0);
			string tone_texture;
			
			if (player.m_CorpseState > PlayerConstants.CORPSE_STATE_FRESH)
			{
				tone_texture = player.m_DecayedTexture;
			}
			else
			{
				tone_texture = player.m_EmptyGloves.GetHiddenSelectionsTextures().Get(0);
			}
			
			SetObjectMaterial( personality, tone_mat );
			SetObjectTexture( personality, tone_texture );
		}
	}
};

typedef Clothing ClothingBase;