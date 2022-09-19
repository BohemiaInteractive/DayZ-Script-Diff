class CAContinuousMineWood : CAContinuousBase
{
	protected float 				m_TimeElpased;
	protected float 				m_AdjustedTimeBetweenMaterialDrops;
	protected float 				m_CycleTimeOverride;
	protected float 				m_TimeBetweenMaterialDrops;
	protected float					m_DamageToMiningItemEachDrop;
	protected float					m_AdjustedDamageToMiningItemEachDrop;
	protected int 					m_AmountOfDrops;
	//protected string				m_Material;
	//protected float 				m_AmountOfMaterialPerDrop;
	protected ref map<string,int> 	m_MaterialAndQuantityMap;
	protected float					m_TimeToComplete;
	protected ref Param1<float>		m_SpentUnits;
	protected bool					m_DataLoaded = false;
	protected const int 			MINEDITEM_MAX = 5;
	protected ItemBase				m_MinedItem[MINEDITEM_MAX];
	protected ItemBase 				m_SecondaryItem;
	
	void CAContinuousMineWood(float time_between_drops)
	{
		m_TimeBetweenMaterialDrops = time_between_drops;
		m_CycleTimeOverride = -1.0;
	}
	
	override void Setup( ActionData action_data )
	{
		m_TimeElpased = 0;
		if ( !m_SpentUnits )
		{ 
			m_SpentUnits = new Param1<float>(0);
		}
		else
		{	
			m_SpentUnits.param1 = 0;
		}
		m_MaterialAndQuantityMap = new map<string,int>;
		m_DataLoaded = GetMiningData(action_data);
		
		/*
		for(int i = 0; i < m_MaterialAndQuantityMap.Count(); i++)
		{
			Print("material = " + m_MaterialAndQuantityMap.GetKey(i) + "; quantity = " + m_MaterialAndQuantityMap.GetElement(i));
		}
		*/
		if (m_CycleTimeOverride > -1.0)
		{
			m_TimeBetweenMaterialDrops = m_CycleTimeOverride;
			if (!action_data.m_MainItem) //hand action
				m_TimeBetweenMaterialDrops = m_TimeBetweenMaterialDrops * 3;
		}
		m_AdjustedTimeBetweenMaterialDrops = action_data.m_Player.GetSoftSkillsManager().SubtractSpecialtyBonus( m_TimeBetweenMaterialDrops, m_Action.GetSpecialtyWeight(), true );		
		m_TimeToComplete = m_AmountOfDrops * m_AdjustedTimeBetweenMaterialDrops;
	}
	
	override int Execute( ActionData action_data )
	{
		Object targetObject;
		Class.CastTo(targetObject, action_data.m_Target.GetObject());
		if ( !action_data.m_Player || !m_DataLoaded )
		{
			return UA_ERROR;
		}
		
		if ( (action_data.m_MainItem && action_data.m_MainItem.IsDamageDestroyed()) || targetObject.IsDamageDestroyed() )
		{
			return UA_FINISHED;
		}
		else
		{
			if ( m_TimeElpased < m_AdjustedTimeBetweenMaterialDrops )
			{
				m_TimeElpased += action_data.m_Player.GetDeltaT();
			}
			else
			{
				if ( GetGame().IsServer() )
				{
					float damage = 0;
					if (m_AmountOfDrops > 0)
						damage = (1 / m_AmountOfDrops) * 100;
					targetObject.DecreaseHealth("","",damage,true);
					CreatePrimaryItems(action_data);
					if (action_data.m_MainItem)
						action_data.m_MainItem.DecreaseHealth( "", "", m_AdjustedDamageToMiningItemEachDrop );
					else
					{
						DamagePlayersHands(action_data.m_Player);
					}
				}
				if ( targetObject.IsDamageDestroyed() )
				{
					if ( m_SpentUnits )
					{
						m_SpentUnits.param1 = m_TimeElpased;
						SetACData(m_SpentUnits);
					}
					
					if ( WoodBase.Cast(targetObject) )
					{
						WoodBase target_woodbase = WoodBase.Cast(targetObject);
						
						if (target_woodbase.GetSecondaryOutput() != "")
							CreateSecondaryItems(action_data,target_woodbase.GetSecondaryOutput(),target_woodbase.GetSecondaryDropsAmount());
					}
					
					if (action_data.m_MainItem)
					{
						targetObject.OnTreeCutDown( action_data.m_MainItem );
					}
					else
					{
						targetObject.OnTreeCutDown( action_data.m_Player );
					}
					OnCompletePogress(action_data);
					return UA_FINISHED;
				}
				m_TimeElpased = m_TimeElpased - m_AdjustedTimeBetweenMaterialDrops;
				OnCompletePogress(action_data);
			}
			return UA_PROCESSING;
		}
	}
	
	override float GetProgress()
	{
		//float progress = m_TimeElpased/m_AdjustedTimeBetweenMaterialDrops;
		return m_TimeElpased/m_AdjustedTimeBetweenMaterialDrops;
	}
		
	override int Cancel( ActionData action_data )
	{	
		ToolBase cut_tree_tool = ToolBase.Cast( action_data.m_MainItem );
		
		if (!cut_tree_tool)
			return super.Cancel( action_data );
		
		return super.Cancel( action_data );
	}
	
	//---------------------------------------------------------------------------
	
	bool GetMiningData(ActionData action_data )
	{
		MineActionData adata = MineActionData.Cast(action_data);
		WoodBase ntarget;
		if ( Class.CastTo(ntarget, action_data.m_Target.GetObject()) )
		{
			m_AmountOfDrops = /*Math.Max(1,*/ntarget.GetAmountOfDropsEx(action_data.m_MainItem,adata.m_HarvestType)/*)*/;
			m_CycleTimeOverride = ntarget.GetCycleTimeOverride(); //TODO
			//m_Material = ntarget.GetMaterial(action_data.m_MainItem);
			//m_AmountOfMaterialPerDrop = Math.Max(1,ntarget.GetAmountOfMaterialPerDrop(action_data.m_MainItem));
			ntarget.GetMaterialAndQuantityMapEx(action_data.m_MainItem,m_MaterialAndQuantityMap, adata.m_HarvestType);
			m_DamageToMiningItemEachDrop = ntarget.GetDamageToMiningItemEachDropEx(action_data.m_MainItem, adata.m_HarvestType );
			m_AdjustedDamageToMiningItemEachDrop = action_data.m_Player.GetSoftSkillsManager().SubtractSpecialtyBonus( m_DamageToMiningItemEachDrop, m_Action.GetSpecialtyWeight(), true );
			return true;
		}
		return false;
	}
	
	void CreatePrimaryItems(ActionData action_data)
	{
		string material;
		int increment;
		for (int i = 0; i < m_MaterialAndQuantityMap.Count(); i++)
		{
			material = m_MaterialAndQuantityMap.GetKey(i);
			
			if (material != "")
			{
				increment = m_MaterialAndQuantityMap.GetElement(i);
				
				if ( !m_MinedItem[i] )
				{
					m_MinedItem[i] = ItemBase.Cast(GetGame().CreateObjectEx(material,action_data.m_Player.GetPosition(), ECE_PLACE_ON_SURFACE));
					m_MinedItem[i].SetQuantity(increment);
					//Print("CreateItems | first stack");
				}
				else if (m_MinedItem[i].HasQuantity())
				{
					if ( m_MinedItem[i].IsFullQuantity() )
					{
						int stack_max = m_MinedItem[i].GetQuantityMax();

						//Print("CreateItems | new stack");
						increment -= stack_max - m_MinedItem[i].GetQuantity();
						if (increment >= 1.0)
						{
							//m_MinedItem[i].SetQuantity(stack_max);
							m_MinedItem[i] = ItemBase.Cast(GetGame().CreateObjectEx(material,action_data.m_Player.GetPosition(), ECE_PLACE_ON_SURFACE));
							m_MinedItem[i].SetQuantity(increment,false);
						}
					}
					else
					{
						//Print("CreateItems | adding quantity to: " + (m_MinedItem[i].GetQuantity() + increment));
						m_MinedItem[i].AddQuantity(increment,false);
					}
				}
				else
				{
					m_MinedItem[i] = ItemBase.Cast(GetGame().CreateObjectEx(material,action_data.m_Player.GetPosition(), ECE_PLACE_ON_SURFACE));
				}
			}
		}
	}
	
	void CreateSecondaryItems(ActionData action_data, string material_secondary = "", int quantity_secondary = -1)
	{
		if (material_secondary == "" || quantity_secondary <= 0)
		{
			return;
		}
		
		m_SecondaryItem = ItemBase.Cast(GetGame().CreateObjectEx(material_secondary,action_data.m_Player.GetPosition(), ECE_PLACE_ON_SURFACE));
		if ( !m_SecondaryItem.HasQuantity() )
		{
			CreateSecondaryItems(action_data,material_secondary,quantity_secondary - 1);
			return;
		}
		
		int increment = quantity_secondary;
		int stack_max = m_SecondaryItem.GetQuantityMax();
		int stacks_amount;// = Math.Ceil(increment/m_SecondaryItem.GetQuantityMax());
		
		stacks_amount = Math.Ceil(increment/stack_max);
		
		for (int i = 0; i < stacks_amount; i++)
		{
			if (increment > stack_max)
			{
				m_SecondaryItem.SetQuantity(stack_max);
				increment -= stack_max;
				m_SecondaryItem = ItemBase.Cast(GetGame().CreateObjectEx(material_secondary,action_data.m_Player.GetPosition(), ECE_PLACE_ON_SURFACE));
			}
			else
			{
				m_SecondaryItem.SetQuantity(increment);
			}
		}
	}
	
	void DamagePlayersHands(PlayerBase player)
	{
		//m_AdjustedDamageToMiningItemEachDrop
		ItemBase gloves = ItemBase.Cast(player.FindAttachmentBySlotName("Gloves"));
		if ( gloves && !gloves.IsDamageDestroyed() )
		{
			gloves.DecreaseHealth("","",m_AdjustedDamageToMiningItemEachDrop);
		}
		else
		{
			int rand = Math.RandomIntInclusive(0,9);
			//Print("rand: " + rand);
			if ( rand == 0 )
			{
				rand = Math.RandomIntInclusive(0,1);
				if ( rand == 0 && !player.GetBleedingManagerServer().AttemptAddBleedingSourceBySelection("LeftForeArmRoll") )
				{
					player.GetBleedingManagerServer().AttemptAddBleedingSourceBySelection("RightForeArmRoll");
				}
				else if ( rand == 1 && !player.GetBleedingManagerServer().AttemptAddBleedingSourceBySelection("RightForeArmRoll") )
				{
					player.GetBleedingManagerServer().AttemptAddBleedingSourceBySelection("LeftForeArmRoll");
				}
			}
		}
	}
};