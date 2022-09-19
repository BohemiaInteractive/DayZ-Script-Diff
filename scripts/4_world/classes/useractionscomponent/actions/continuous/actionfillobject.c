class ActionFillObjectCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.DEFAULT_FILL);
	}
};

class ActionFillObject: ActionContinuousBase
{
	protected int m_ActionState;

	protected const int EMPTY 						= 0;
	protected const int FILLED 						= 1;
	
	void ActionFillObject()
	{
		m_CallbackClass = ActionFillObjectCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_DIG;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_LOW;
	}
	
	override void CreateConditionComponents()  
	{	
		
		m_ConditionTarget = new CCTNonRuined(UAMaxDistances.DEFAULT);
		m_ConditionItem = new CCINonRuined;
	}
	
	override void OnActionInfoUpdate(  PlayerBase player, ActionTarget target, ItemBase item )
	{
		HescoBox hesco = HescoBox.Cast(target.GetObject());

		if( hesco.GetState() == HescoBox.UNFOLDED )
		{
			m_Text =  "#empty";
		}
		else
		{
			m_Text =  "#fill";
		}
	}	

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if ( player.IsPlacingLocal() )
			return false;
		
		HescoBox hesco;
		if ( Class.CastTo(hesco,target.GetObject()) )
		{
			if ( hesco.CanBeFilledAtPosition( player.GetPosition() ) )
			{
				if ( hesco.GetState() == HescoBox.UNFOLDED )
				{
					m_ActionState = EMPTY;
					return true;
				}
				else if( hesco.GetState() == HescoBox.FILLED)
				{
					m_ActionState = FILLED;
					return true;					
				}
			}
		}
		
		return false;
	}

	override void OnFinishProgressServer( ActionData action_data )
	{
		HescoBox hesco;
		
		if ( Class.CastTo(hesco,action_data.m_Target.GetObject()) )
		{
			const float ITEM_DAMAGE = 0.05;
			action_data.m_MainItem.DecreaseHealth ( "", "", action_data.m_Player.GetSoftSkillsManager().SubtractSpecialtyBonus( ITEM_DAMAGE, this.GetSpecialtyWeight() )*100 );
			
			if ( hesco.GetState() == HescoBox.UNFOLDED )
			{
				hesco.Fill();
			}
			else if ( hesco.GetState() == HescoBox.FILLED )
			{
				hesco.Unfold();
			}
		}

		action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
	}
	
	override void OnFinishProgressClient( ActionData action_data )
	{
		HescoBox hesco;
		
		if ( Class.CastTo(hesco,action_data.m_Target.GetObject()) )
		{
			if ( hesco.GetState() == HescoBox.UNFOLDED )
			{
				hesco.Fill();
			}
			else if ( hesco.GetState() == HescoBox.FILLED )
			{
				hesco.Unfold();
			}
		}
	}
};