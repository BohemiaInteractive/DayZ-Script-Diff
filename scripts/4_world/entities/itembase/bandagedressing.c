class BandageDressing extends ItemBase
{
	//================================================================
	// IGNITION ACTION
	//================================================================
	override bool HasFlammableMaterial()
	{
		return true;
	}
	
	override bool CanBeIgnitedBy( EntityAI igniter = NULL )
	{
		if ( GetHierarchyParent() ) return false;
		
		return true;
	}
	
	override bool CanIgniteItem( EntityAI ignite_target = NULL )
	{
		return false;
	}
	
	override void OnIgnitedTarget( EntityAI ignited_item )
	{
	}
	
	override void OnIgnitedThis( EntityAI fire_source )
	{
		Fireplace.IgniteEntityAsFireplace( this, fire_source );
	}

	override bool IsThisIgnitionSuccessful( EntityAI item_source = NULL )
	{
		return Fireplace.CanIgniteEntityAsFireplace( this );
	}	
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionBandageTarget);
		AddAction(ActionBandageSelf);
		AddAction(ActionCreateIndoorFireplace);
		AddAction(ActionCreateIndoorOven);
		AddAction(ActionAttach);
		AddAction(ActionDetach);
	}
	
	override float GetBandagingEffectivity()
	{
		return 2.0;
	};
	
	override bool CanBeDisinfected()
	{
		return true;
	}

	override float GetInfectionChance(int system = 0, Param param = null)
	{
		if(m_Cleanness == 1)
		{
			return 0;
		}
		else
		{
			return 0.05;
		}
	}
	
	override void OnCombine(ItemBase other_item)
	{
		if(m_Cleanness == 1 && other_item.m_Cleanness == 0)
			SetCleanness(0);
	}
}