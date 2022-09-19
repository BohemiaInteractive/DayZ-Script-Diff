class AIGroupBehaviour : Managed
{
	private void AIGroupBehaviour();
	private void ~AIGroupBehaviour();
}

class BehaviourGroupInfectedPackWaypointParams
{
	vector m_CenterPosition;
	float m_Radius;
	
	void BehaviourGroupInfectedPackWaypointParams(vector pos, float radius)
	{
		m_CenterPosition = pos;
		m_Radius = radius;
	}
}

class BehaviourGroupInfectedPack : AIGroupBehaviour
{
	/*!
	Group waypoints initialization
	/param 	[in] 	waypointDefaultIndex 		Starting waypoint. Index to array of waypoints specified by waypointParams. Acceptable range is [0, waypointParams.Count() - 1]
	/param 	[in]	forwardDirection			Waypoints traverse order.
	/param 	[in]	loop						Specifies what happens after last waypoint is reached. true - cycle waypoints, false - patrol waypoints
	*/
	proto native void SetWaypoints(array<ref BehaviourGroupInfectedPackWaypointParams> waypointParams, int waypointDefaultIndex, bool forwardDirection, bool loop);

	proto native void SetWaypointsTraverseDirection(bool forwardDirection);
	proto native void SetWaypointsLoop(bool loop);
	proto native void SetCurrentWaypoint(int waypointIndex);
}