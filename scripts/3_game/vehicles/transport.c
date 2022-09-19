/*!
	Base native class of all vehicles in game.
*/
class Transport extends EntityAI
{
	ref TIntArray m_SingleUseActions;
	ref TIntArray m_ContinuousActions;
	ref TIntArray m_InteractActions;

	void Transport()
	{
	}

	override int GetMeleeTargetType()
	{
		return EMeleeTargetType.NONALIGNABLE;
	}


	//! Synchronizes car's state in case the simulation is not running.
	proto native void Synchronize();


	//! Returns crew capacity of this vehicle.
	proto native int CrewSize();

	//! Returns crew member index based on action component index.
	//! -1 is returned when no crew position corresponds to given component index.
	proto native int CrewPositionIndex( int componentIdx );

	//! Returns crew member index based on player's instance.
	//! -1 is returned when the player is not isnide.
	proto native int CrewMemberIndex( Human player );

	//! Returns crew member based on position index.
	//! Null can be returned if no Human is present on the given position.
	proto native Human CrewMember( int posIdx );

	//! Reads entry point and direction into vehicle on given position in model space.
	proto void CrewEntry( int posIdx, out vector pos, out vector dir );

	//! Reads entry point and direction into vehicle on given position in world space.
	proto void CrewEntryWS( int posIdx, out vector pos, out vector dir );

	//! Returns crew transformation indside vehicle in model space
	proto void CrewTransform( int posIdx, out vector mat[4] );

	//! Returns crew transformation indside vehicle in world space
	proto void CrewTransformWS( int posIdx, out vector mat[4] );

	//! Performs transfer of player from world into vehicle on given position.
	proto native void CrewGetIn( Human player, int posIdx );

	//! Performs transfer of player from vehicle into world from given position.
	proto native Human CrewGetOut( int posIdx );
	
	//! Handles death of player in vehicle and awakes its physics if needed
	proto native void CrewDeath( int posIdx );


	override bool IsTransport()
	{
		return true;
	}
	
	override bool IsIgnoredByConstruction()
	{
		return false;
	}
	
	override bool IsHealthVisible()
	{
		return true;
	}

	override bool ShowZonesHealth()
	{
		return true;
	}
	
	float GetTransportCameraDistance()
	{
		return 4.0;
	}
	
	vector GetTransportCameraOffset()
	{
		return "0 1.3 0";
	}
	
	int GetAnimInstance()
	{
		Error("GetAnimInstance() not implemented");
		return 0;
	}

	int GetSeatAnimationType( int posIdx )
	{
		Error("GetSeatAnimationType() not implemented");
		return 0;
	}

	int Get3rdPersonCameraType()
	{
		Error("Get3rdPersonCameraType() not implemented");
		return 0;
	}
	
	bool CrewCanGetThrough( int posIdx )
	{
		return false;
	}
	
	bool CanReachSeatFromSeat( int currentSeat, int nextSeat )
	{
		return false;
	}
	
	bool CanReachSeatFromDoors( string pSeatSelection, vector pFromPos, float pDistance = 1.0 )
	{
		return false;		
	}

	bool CanReachDoorsFromSeat( string pDoorsSelection, int pCurrentSeat )
	{
		return false;
	}
	
	int GetSeatIndexFromDoor( string pDoorSelection )
	{
		//Potientially could be fixed some other way, currently follows the unfortunate pattern that CanReachDoorsFromSeat has created
		switch (pDoorSelection)
		{
			case "DoorsDriver":
				return 0;
				break;
			case "DoorsCoDriver":
				return 1;
				break;
			case "DoorsCargo1":
				return 2;
				break;
			case "DoorsCargo2":
				return 3;
				break;
		}
		return -1;
	}
	
	bool IsIgnoredObject( Object o )
	{
		if (!o)
			return false;		
		
		EntityAI e = EntityAI.Cast(o);			
		// CanBeSkinned means it is a dead entity which should not block the door
		return ( ( e && (e.IsZombie() || e.IsHologram()) ) || o.CanBeSkinned() || o.IsBush() || o.IsTree() );
	}
	
	bool IsAreaAtDoorFree( int currentSeat, float maxAllowedObjHeight = 0.5, float horizontalExtents = 0.5, float playerHeight = 1.7 )
	{
		vector crewPos;
		vector crewDir;
		CrewEntryWS( currentSeat, crewPos, crewDir );
		crewPos[1] = crewPos[1] + maxAllowedObjHeight + playerHeight * 0.5;
		array<Object> excluded = new array<Object>;
		array<Object> collided = new array<Object>;
		excluded.Insert(this);
		excluded.Insert(GetGame().GetPlayer());
		GetGame().IsBoxColliding(crewPos, crewDir, Vector(horizontalExtents, playerHeight, horizontalExtents), excluded, collided); 
		foreach (Object o : collided)
		{
			vector minmax[2];
			if (o.GetCollisionBox(minmax))
				return false;
		}
		return true;
	}
	
	Shape DebugFreeAreaAtDoor( int currentSeat, float maxAllowedObjHeight = 0.5, float horizontalExtents = 0.5, float playerHeight = 1.7 )
	{
		vector crewPos;
		vector crewDir;
		CrewEntryWS( currentSeat, crewPos, crewDir );
		crewPos[1] = crewPos[1] + maxAllowedObjHeight + playerHeight * 0.5;
		array<Object> excluded = new array<Object>;
		array<Object> collided = new array<Object>;
		excluded.Insert(this);
		excluded.Insert(GetGame().GetPlayer());
		GetGame().IsBoxColliding(crewPos, crewDir, Vector(horizontalExtents, playerHeight, horizontalExtents), excluded, collided); 
		int color = ARGB(100, 0, 255, 0);
		foreach (Object o : collided)
		{
			vector minmax[2];
			if (o.GetCollisionBox(minmax))
			{
				color = ARGB(100, 255, 0, 0);
			}
		}

		return Debug.DrawCylinder(crewPos, horizontalExtents, playerHeight, color);
	}
};
