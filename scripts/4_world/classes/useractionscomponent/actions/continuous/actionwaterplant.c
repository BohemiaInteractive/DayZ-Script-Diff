class ActionWaterPlantCB : ActionContinuousBaseCB
{
	private const float QUANTITY_USED_PER_SEC = 150;
	
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousWaterPlant(QUANTITY_USED_PER_SEC);
	}
};

class ActionWaterPlant: ActionContinuousBase
{
	void ActionWaterPlant()
	{
		m_CallbackClass = ActionWaterPlantCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_EMPTY_VESSEL;
		m_FullBody = true;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_LOW;
		m_Text = "#water_plant";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionTarget = new CCTNonRuined(UAMaxDistances.DEFAULT);
		m_ConditionItem = new CCINotRuinedAndEmpty;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		Object targetObject = target.GetObject();
		
		if ( targetObject != NULL && targetObject.IsInherited(PlantBase) && item != NULL && !item.IsDamageDestroyed() )
		{
			PlantBase plant = PlantBase.Cast( targetObject );
			
			if ( plant.NeedsWater() && item.GetQuantity() > 0 )
			{
				return true;
			}
		}
		
		return false;
	}
	
	override void OnFinishProgressServer( ActionData action_data )
	{
		Object targetObject = action_data.m_Target.GetObject();
		
		if ( targetObject != NULL && targetObject.IsInherited(PlantBase) )
		{
			PlantBase plant = PlantBase.Cast( targetObject );
			Param1<float> nacdata = Param1<float>.Cast( action_data.m_ActionComponent.GetACData() );
			float water = nacdata.param1;
			Slot slot = plant.GetSlot();
			water = action_data.m_Player.GetSoftSkillsManager().AddSpecialtyBonus( water, this.GetSpecialtyWeight() );
			slot.GiveWater( water );
			action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
		}
	}
	
	override void OnFinishProgressClient( ActionData action_data )
	{
		Object targetObject = action_data.m_Target.GetObject();
		
		if ( targetObject != NULL && targetObject.IsInherited(PlantBase) )
		{
			PlantBase plant = PlantBase.Cast( targetObject );
			Param1<float> nacdata = Param1<float>.Cast( action_data.m_ActionComponent.GetACData() );
			float water = nacdata.param1;
			Slot slot = plant.GetSlot();
			water = action_data.m_Player.GetSoftSkillsManager().AddSpecialtyBonus( water, this.GetSpecialtyWeight() );
			slot.GiveWater( water );
			action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
		}
	}
};