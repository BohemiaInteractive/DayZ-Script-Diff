class ActionPlaceOvenIndoor: ActionSingleUseBase
{
	void ActionPlaceOvenIndoor()
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
		FireplaceBase fireplace_in_hands = FireplaceBase.Cast( action_data.m_MainItem );
		
		//replace fireplace with lambda
		FireplaceToIndoorOvenLambda lambda = new FireplaceToIndoorOvenLambda( fireplace_in_hands, "OvenIndoor", action_data.m_Player, action_data.m_Player.GetLastFirePoint(), action_data.m_Target.GetObject() );
		lambda.SetTransferParams( true, true, true );
		action_data.m_Player.ServerReplaceItemInHandsWithNewElsewhere( lambda );

		//add to soft skills
		action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
	}
}


class FireplaceToIndoorOvenLambda : TurnItemIntoItemLambda
{
	int 		m_FirePointIndex;
	float		m_FireplaceRot;
	vector 		m_SmokePosition;
	
	void FireplaceToIndoorOvenLambda( EntityAI old_item, string new_item_type, PlayerBase player, vector pos, Object target )
	{
		InventoryLocation gnd = new InventoryLocation;
		vector mtx[4];
		Math3D.MatrixIdentity4( mtx );
		mtx[3] = pos;
		gnd.SetGround( NULL, mtx );
		OverrideNewLocation( gnd );
		
		//set fire point index and smoke point position in world
		m_FirePointIndex = player.GetLastFirePointIndex();
		m_FireplaceRot = player.GetLastFirePointRot();
		
		vector smoke_point_pos = target.GetSelectionPositionMS( OvenIndoor.OVENPOINT_SMOKE_POSITION + m_FirePointIndex.ToString() );
		vector smoke_point_pos_world = target.ModelToWorld( smoke_point_pos );		
		m_SmokePosition = smoke_point_pos_world;
	}

	override void CopyOldPropertiesToNew (notnull EntityAI old_item, EntityAI new_item)
	{
		super.CopyOldPropertiesToNew( old_item, new_item );
		
		OvenIndoor fireplace_indoor = OvenIndoor.Cast( new_item );
		if ( fireplace_indoor )
		{
			//set fire point index
			fireplace_indoor.SetFirePointIndex( m_FirePointIndex );

			// rotate fireplace
			vector fprot = vector.Zero;
			fprot[0] = m_FireplaceRot;
			fireplace_indoor.SetOrientation( fprot );
			Print(fireplace_indoor.GetOrientation());

			// smoke pos (chimney)
			fireplace_indoor.SetSmokePointPosition( m_SmokePosition );

			//synchronize
			fireplace_indoor.Synchronize();
		}
	}
};
