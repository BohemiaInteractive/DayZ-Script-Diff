class RepairCarEngineActionReciveData : ActionReciveData
{
	string m_DamageZoneRecived;
}

class RepairCarEngineActionData : ActionData
{
	string m_DamageZone;
}

class ActionRepairCarEngineCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime( UATimeSpent.BASEBUILDING_REPAIR_FAST );
	}
};

class ActionRepairCarEngine: ActionContinuousBase
{
	typename m_LastValidType;
	string m_CurrentDamageZone = "";
	int m_LastValidComponentIndex = -1;
	
	void ActionRepairCarEngine()
	{
		m_CallbackClass = ActionRepairTentPartCB;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
		
		//m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_ASSEMBLE;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT;// | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_FullBody = true;
		m_LockTargetOnUse = false;
		m_Text = "#repair";
	}

	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINonRuined; //To change?
		m_ConditionTarget 	= new CCTCursor(UAMaxDistances.REPAIR);
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		//Action not allowed if player has broken legs
		if (player.GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS)
			return false;
		
		//m_CurrentDamageZone = "";
		Object targetObject = target.GetObject();
		Object targetParent = target.GetParent();
		CarScript car = CarScript.Cast( targetObject );
		EntityAI carEntity = EntityAI.Cast( targetObject );
		
		if ( !car || !player )
			return false;

		 //m_MaximalActionDistance;
		//float distance = Math.AbsFloat( vector.Distance( car.GetPosition(),player.GetPosition() ));

		//if ( distance <= MAX_ACTION_DIST )	
		//{
			if ( GetGame().IsMultiplayer() && GetGame().IsServer() )
				return true;
			
			array<string> selections = new array<string>;
			PluginRepairing module_repairing;
			Class.CastTo( module_repairing, GetPlugin(PluginRepairing) );

			targetObject.GetActionComponentNameList( target.GetComponentIndex(), selections, LOD.NAME_VIEW);

			if (m_LastValidType != target.Type() || m_LastValidComponentIndex != target.GetComponentIndex() || m_CurrentDamageZone == "" )
			{
				string damageZone = "";
				string compName = "";

				for ( int s = 0; s < selections.Count(); s++ )
				{
					compName = selections[s];

					//NOTE: relevant fire geometry and view geometry selection names MUST match in order to get a valid damage zone
					if ( carEntity && DamageSystem.GetDamageZoneFromComponentName( carEntity, compName, damageZone ))
					{
						int zoneHP = car.GetHealthLevel( damageZone );
						if (zoneHP > GameConstants.STATE_WORN && zoneHP < GameConstants.STATE_RUINED)
						{
							m_CurrentDamageZone = damageZone;
							m_LastValidComponentIndex = target.GetComponentIndex();
							
							vector repairPos = carEntity.GetPosition();
					
							//Exclude all zones except engine
							if ( damageZone != "Engine")
								return false;
							else
								repairPos = car.GetEnginePosWS();
							
							/*float dist = vector.DistanceSq( repairPos, player.GetPosition() );
							if ( dist < MAX_ACTION_DIST * MAX_ACTION_DIST)*/
								return true;
						}
					}
				}
			//}
		}
		
		return false;
	}

	override void OnFinishProgressServer( ActionData action_data )
	{
		Object tgObject = action_data.m_Target.GetObject();
		ItemBase usedItem = action_data.m_MainItem;
		
		string damageZone = RepairCarPartActionData.Cast(action_data).m_DamageZone;
		if ( !GetGame().IsMultiplayer() )
			damageZone = m_CurrentDamageZone;

		if ( tgObject && damageZone != "" ) 
		{
			CarScript car = CarScript.Cast( tgObject );
			if ( car )
			{

				int newDmgLevel = car.GetHealthLevel( damageZone ) - 1;
				
				if ( newDmgLevel < 0 )
					newDmgLevel = 0;
				
				float zoneMax = car.GetMaxHealth(damageZone,"");
				
				float dmgStateValue = zoneMax * GameConstants.DAMAGE_WORN_VALUE;
				float randomValue = Math.RandomFloatInclusive( zoneMax * 0.05, zoneMax * 0.15);
				
				//TODO:: CHECK
				//GetHealthLevelValue
				switch ( newDmgLevel )
				{
					case GameConstants.STATE_BADLY_DAMAGED:
						Print( zoneMax * GameConstants.DAMAGE_RUINED_VALUE );
						car.SetHealth( damageZone, "", (zoneMax * GameConstants.DAMAGE_RUINED_VALUE) + randomValue );
						break;

					case GameConstants.STATE_DAMAGED:
						Print(zoneMax * GameConstants.DAMAGE_BADLY_DAMAGED_VALUE );
						car.SetHealth( damageZone, "", (zoneMax * GameConstants.DAMAGE_BADLY_DAMAGED_VALUE) + randomValue );
						break;

					case GameConstants.STATE_WORN:
						Print(zoneMax * GameConstants.DAMAGE_DAMAGED_VALUE );
						car.SetHealth( damageZone, "", (zoneMax * GameConstants.DAMAGE_DAMAGED_VALUE) + randomValue );
						break;
					
					case GameConstants.STATE_PRISTINE:
						Print(zoneMax * GameConstants.DAMAGE_WORN_VALUE );
						car.SetHealth( damageZone, "", (zoneMax * GameConstants.DAMAGE_WORN_VALUE) + randomValue );
						break;
					
					default:
						break;
				}

				MiscGameplayFunctions.DealAbsoluteDmg(usedItem, 35); //Placeholder until proper balancing and functions are deployed
				
			}
		}
	}

	override ActionData CreateActionData()
	{
		RepairCarPartActionData action_data = new RepairCarPartActionData;
		return action_data;
	}
	
	override void WriteToContext(ParamsWriteContext ctx, ActionData action_data)
	{
		super.WriteToContext( ctx, action_data );
		RepairCarPartActionData repair_action_data;

		if ( HasTarget() && Class.CastTo( repair_action_data,action_data ) )
		{
			repair_action_data.m_DamageZone = m_CurrentDamageZone;
			ctx.Write(repair_action_data.m_DamageZone);
		}
	}
	
	override bool ReadFromContext(ParamsReadContext ctx, out ActionReciveData action_recive_data )
	{
		if ( !action_recive_data )
			action_recive_data = new RepairCarPartActionReciveData;

		super.ReadFromContext( ctx, action_recive_data );
		RepairCarPartActionReciveData recive_data_repair = RepairCarPartActionReciveData.Cast( action_recive_data );

		if ( HasTarget() )
		{
			string zone;
			if ( !ctx.Read( zone ) )
				return false;

			recive_data_repair.m_DamageZoneRecived = zone;
		}

		return true;
	}
	
	override void HandleReciveData( ActionReciveData action_recive_data, ActionData action_data )
	{
		super.HandleReciveData( action_recive_data, action_data );

		RepairCarPartActionReciveData recive_data_repair = RepairCarPartActionReciveData.Cast( action_recive_data );
		RepairCarPartActionData.Cast( action_data ).m_DamageZone = recive_data_repair.m_DamageZoneRecived;
	}
};