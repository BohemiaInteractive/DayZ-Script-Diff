// TO DO: Remove this script!

class ActionDisinfectPlantBitCB : ActionSingleUseBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CASingleUseQuantity(UAQuantityConsumed.GARDEN_DISINFECT_PLANT);
	}
};


class ActionDisinfectPlantBit: ActionSingleUseBase
{	
	void ActionDisinfectPlantBit()
	{
		m_CallbackClass = ActionDisinfectPlantBitCB;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
		m_Text = "#apply";
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNonRuined(UAMaxDistances.DEFAULT);
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		Object targetObject = target.GetObject();
		
		if ( targetObject != NULL && targetObject.IsInherited(PlantBase) && !item.IsDamageDestroyed() )
		{
			PlantBase plant = PlantBase.Cast( targetObject );
			
			if ( plant.IsGrowing() && plant.NeedsSpraying() )
			{	
				if ( item.GetQuantity() > 0 )
				{
					return true;
				}
			}
		}
		
		return false;
	}

	override void OnExecuteServer( ActionData action_data )
	{
		Object targetObject = action_data.m_Target.GetObject();
		
		if ( targetObject != NULL && targetObject.IsInherited(PlantBase) )
		{
			
			PlantBase plant = PlantBase.Cast( targetObject );
			Param1<float> nacdata = Param1<float>.Cast( action_data.m_ActionComponent.GetACData() );
			SendMessageToClient(action_data.m_Player, plant.StopInfestation( nacdata.param1 ));
		}
		
		action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
	}
};