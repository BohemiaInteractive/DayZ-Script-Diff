class ActionCookOnStickCB : ActionContinuousBaseCB
{
	private const float COOKING_ON_STICK_UPDATE_TIME = 1;
	
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTimeCooking( COOKING_ON_STICK_UPDATE_TIME );
	}
};

class ActionCookOnStick: ActionContinuousBase
{
	void ActionCookOnStick()
	{
		m_CallbackClass 	= ActionCookOnStickCB;
		m_CommandUID 		= DayZPlayerConstants.CMD_GESTUREFB_CAMPFIRE;
		m_FullBody 			= true;
		m_StanceMask 		= DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_SpecialtyWeight 	= UASoftSkillsWeight.ROUGH_LOW;
		
		m_LockTargetOnUse 	= false;
		m_Text 				= "#cook_on_stick";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionTarget	= new CCTNonRuined(UAMaxDistances.DEFAULT);
		m_ConditionItem		= new CCINonRuined;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		FireplaceBase fireplace_target = FireplaceBase.Cast(target.GetObject());
		Object targetObject = target.GetObject();
		Edible_Base item_on_stick = Edible_Base.Cast(item.GetAttachmentByType(Edible_Base));
		if (fireplace_target && item_on_stick && item_on_stick.CanBeCookedOnStick())
		{
			//fireplace
			if (fireplace_target.IsBaseFireplace())
			{
				Fireplace fireplace = Fireplace.Cast(fireplace_target);
				if (!fireplace.IsOven() && fireplace_target.CanCookOnStick())
				{
					return true;
				}
			}
			
			//fireplace indoor
			if (fireplace_target.IsFireplaceIndoor())
			{
				if (fireplace_target.CanCookOnStick()) 
				{
					return true;
				}	
			}			
		}
		
		return false;
	}
	
	override void OnEndServer(ActionData action_data)
	{
		super.OnEndServer(action_data);
		
		ItemBase stick = action_data.m_MainItem;
		Edible_Base item_on_stick = Edible_Base.Cast(stick.GetAttachmentByType(Edible_Base));

		if (item_on_stick)
		{
			item_on_stick.MakeSoundsOnClient(false);
		}
	}
};