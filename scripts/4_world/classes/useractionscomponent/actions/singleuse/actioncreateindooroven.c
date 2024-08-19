class ActionCreateIndoorOven: ActionSingleUseBase
{
	void ActionCreateIndoorOven()
	{
		m_CommandUID        = DayZPlayerConstants.CMD_ACTIONMOD_OPENDOORFW;
		m_StanceMask        = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
		m_Text = "#place_object";
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTCursor;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if ( !target ) return false;

		Object target_object = target.GetObject();
		string action_selection = target_object.GetActionComponentName( target.GetComponentIndex() );
		BuildingWithFireplace building = BuildingWithFireplace.Cast( target_object );
		
		if ( target_object && building && action_selection.Contains( OvenIndoor.OVENPOINT_ACTION_SELECTION ) )
		{
			if ( !IsInReach(player, target, UAMaxDistances.DEFAULT) ) return false;
			
			vector fire_point_pos_world, fire_point_rot_world;
			int fire_point_index = OvenIndoor.GetFirePointIndex( action_selection );
			if ( OvenIndoor.CanPlaceFireplaceInSelectedSpot( building, fire_point_index, fire_point_pos_world, fire_point_rot_world ) )
			{
				float rot_deg = 0.0;
				if ( building.HasSelection( OvenIndoor.OVENPOINT_PLACE_ROT + fire_point_index.ToString() ) )
				{
					vector diff = fire_point_rot_world - fire_point_pos_world;
					diff[1] = 0.0;
					diff.Normalize();
					float dotp = vector.Dot( "0 0 1" , diff );
					rot_deg = Math.Acos( dotp ) * Math.RAD2DEG;
					if ( ( diff[0] < 0 ) && ( diff[2] < 0 ) )
						rot_deg = 360.0 - rot_deg;
					else if ( ( diff[0] < 0 ) && ( diff[2] > 0 ) )
						rot_deg = 360.0 - rot_deg;

					//Debug.DrawArrow( fire_point_pos_world, fire_point_pos_world + diff );
				}
				
				float fire_point_dist = vector.Distance( fire_point_pos_world, player.GetPosition() );
				if ( fire_point_dist <= 2 )
				{
					player.SetLastFirePoint( fire_point_pos_world );
					player.SetLastFirePointIndex( fire_point_index );
					player.SetLastFirePointRot( rot_deg );
					return true;
				}
			}
		}
		
		return false;
	}
		
	override void OnExecuteServer( ActionData action_data )
	{	
		Object obj = GetGame().CreateObjectEx( "OvenIndoor", action_data.m_Player.GetLastFirePoint(), ECE_PLACE_ON_SURFACE );
	
		int m_FirePointIndex = action_data.m_Player.GetLastFirePointIndex();
		float m_FireplaceRot = action_data.m_Player.GetLastFirePointRot();
		vector smoke_point_pos = action_data.m_Target.GetObject().GetSelectionPositionMS( OvenIndoor.OVENPOINT_SMOKE_POSITION + m_FirePointIndex.ToString() );
		vector smoke_point_pos_world = action_data.m_Target.GetObject().ModelToWorld( smoke_point_pos );		
		vector m_SmokePosition = smoke_point_pos_world;
		
		OvenIndoor fp_indoor = OvenIndoor.Cast( obj );
		if ( fp_indoor )
		{
			fp_indoor.SetFirePointIndex( m_FirePointIndex );
			fp_indoor.SetSmokePointPosition( m_SmokePosition );

			vector fprot = vector.Zero;
			fprot[0] = m_FireplaceRot;
			fp_indoor.SetOrientation( fprot );

			fp_indoor.Synchronize();
			
			//move item to target fireplace
			action_data.m_Player.ServerTakeEntityToTargetAttachment(fp_indoor, action_data.m_MainItem);
		}
	}
}