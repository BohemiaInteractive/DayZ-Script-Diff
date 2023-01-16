class DayZSpectator : Camera
{
	private float m_SpeedMultiplier = 1.0;
	
	private float m_SendUpdateAcc = 0.0;
	
	void DayZSpectator()
	{
		SetEventMask(EntityEvent.FRAME);
	}
	
	override void EOnFrame(IEntity other, float timeSlice)
	{
		if (GetUApi().GetInputByID(UACarShiftGearUp).LocalPress())
			m_SpeedMultiplier = m_SpeedMultiplier + 2;
		if (GetUApi().GetInputByID(UACarShiftGearDown).LocalPress())
			m_SpeedMultiplier = m_SpeedMultiplier - 2;
		
		float speed = 5.0 * m_SpeedMultiplier;
		if (GetUApi().GetInputByID(UATurbo).LocalValue())
			speed *= 2;
		
		float forward = GetUApi().GetInputByID(UAMoveForward).LocalValue() - GetUApi().GetInputByID(UAMoveBack).LocalValue();
		float strafe = GetUApi().GetInputByID(UAMoveRight).LocalValue() - GetUApi().GetInputByID(UAMoveLeft).LocalValue();
		
		vector direction = GetDirection();
		vector directionAside = vector.Up * direction;
		
		vector oldPos = GetPosition();
		vector forwardChange = forward * timeSlice * direction * speed;
		vector strafeChange = strafe * timeSlice * directionAside * speed;
		vector newPos = oldPos + forwardChange + strafeChange;
		float yMin = GetGame().SurfaceRoadY(newPos[0], newPos[2]);
		if (newPos[1] < yMin)
			newPos[1] = yMin;
		
		SetPosition(newPos);
		
		float yawDiff = GetUApi().GetInputByID(UAAimLeft).LocalValue() - GetUApi().GetInputByID(UAAimRight).LocalValue();
		float pitchDiff = GetUApi().GetInputByID(UAAimDown).LocalValue() - GetUApi().GetInputByID(UAAimUp).LocalValue();
		vector oldOrient = GetOrientation();
		vector newOrient = oldOrient;
		newOrient[0] = newOrient[0] - Math.RAD2DEG * yawDiff * timeSlice;
		newOrient[1] = newOrient[1] - Math.RAD2DEG * pitchDiff * timeSlice;
		if (newOrient[1] < -89)
			newOrient[1] = -89;
		if (newOrient[1] > 89)
			newOrient[1] = 89;
		
		SetOrientation(newOrient);
		
		if (m_SendUpdateAcc > 0.5)
		{
			GetGame().UpdateSpectatorPosition(newPos);
			m_SendUpdateAcc = 0;
		}
		
		m_SendUpdateAcc = m_SendUpdateAcc + timeSlice;
	}
};