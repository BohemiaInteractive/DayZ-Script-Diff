class CAContinuousFertilizeGardenSlot : CAContinuousQuantity
{
	protected float m_SlotFertilizerNeed;
	protected float	m_TimeToComplete;
	protected float m_SpentQuantityTotal;
	protected float m_StartQuantity;
	protected Slot 	m_Slot;
	protected string m_Selection;

	void CAContinuousFertilizeGardenSlot( float quantity_used_per_second )
	{
		m_QuantityUsedPerSecond = quantity_used_per_second;
		m_TimeToComplete = 0;
	}
	
	override void Setup( ActionData action_data )
	{
		GardenBase target_GB;		
		if ( Class.CastTo(target_GB,  action_data.m_Target.GetObject() ) )
		{
			m_SpentQuantity = 0;
			m_StartQuantity = action_data.m_MainItem.GetQuantity();
			
			if ( !m_SpentUnits )
			{
				m_SpentUnits = new Param1<float>(0);
			}
			else
			{	
				m_SpentUnits.param1 = 0;
			}
			
			if ( action_data.m_MainItem ) 
				m_ItemQuantity = action_data.m_MainItem.GetQuantity();
			
			if ( target_GB ) 
			{
				/*string selection = target_GB.GetActionComponentName(action_data.m_Target.GetComponentIndex());
			
				Slot slot = target_GB.GetSlotBySelection( selection );*/
			
				array<string> selections = new array<string>;
				target_GB.GetActionComponentNameList(action_data.m_Target.GetComponentIndex(), selections);
	
				for (int s = 0; s < selections.Count(); s++)
				{
					//string selection = selections[s];
					m_Selection = selections[s];
					m_Slot = target_GB.GetSlotBySelection( m_Selection );
					if (m_Slot)
						break;
				}
				
				
				string item_type = action_data.m_MainItem.GetType();
				float consumed_quantity = GetGame().ConfigGetFloat( "cfgVehicles " + item_type + " Horticulture ConsumedQuantity" );
				
				float max = m_Slot.GetFertilizerQuantityMax();
				
				m_SlotFertilizerNeed = max - consumed_quantity;
			}
			m_TimeToComplete = (Math.Min(m_SlotFertilizerNeed,m_ItemQuantity))/m_QuantityUsedPerSecond;
		}
	}
	
	
	override int Execute( ActionData action_data  )
	{		
		if ( !action_data.m_Player )
		{
			return UA_ERROR;
		}
		
		if ( m_SpentQuantity >= m_ItemQuantity )
		{
			return UA_FINISHED;
		}
		else
		{
			if ( m_SpentQuantity <= m_ItemQuantity )
			{
				m_SpentQuantity += m_QuantityUsedPerSecond * action_data.m_Player.GetDeltaT();
				float val = action_data.m_Player.GetSoftSkillsManager().AddSpecialtyBonus( m_SpentQuantity, m_Action.GetSpecialtyWeight(), true );
				GardenBase garden_base;
				Class.CastTo(garden_base,  action_data.m_Target.GetObject() );
				//string selection = garden_base.GetActionComponentName(action_data.m_Target.GetComponentIndex());
			
				m_SpentQuantityTotal += m_SpentQuantity;
				
				if (GetGame().IsServer())
				{
					action_data.m_MainItem.AddQuantity( -m_SpentQuantity );		
				}
				
				garden_base.Fertilize( action_data.m_Player, action_data.m_MainItem, val, m_Selection );

				return UA_PROCESSING;
			}
			else
			{
				CalcAndSetQuantity( action_data );
				OnCompletePogress(action_data);
				return UA_FINISHED;
			}
		}
	}
	
	override float GetProgress()
	{	
		/*if ( m_TimeToComplete == 0) //Prevent division by 0 case
		{
			return -1;
		}*/
		return -(m_SpentQuantityTotal / m_StartQuantity);
		//return (m_SpentQuantity)/m_ItemQuantity;///(m_TimeToComplete * m_QuantityUsedPerSecond)/m_ItemQuantity;
	}
};