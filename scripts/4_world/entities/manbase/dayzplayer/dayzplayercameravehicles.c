// *************************************************************************************
// ! DayZPlayerCamera1stPersonVehicle - first person in vehicle
// *************************************************************************************
class DayZPlayerCamera1stPersonVehicle extends DayZPlayerCamera1stPerson
{
	override void OnUpdate(float pDt, out DayZPlayerCameraResult pOutResult)
	{
		super.OnUpdate(pDt, pOutResult);

		pOutResult.m_fUseHeading 		= 0.0;
		pOutResult.m_bUpdateEveryFrame 	= g_Game.IsPhysicsExtrapolationEnabled();
	}
}

// *************************************************************************************
// ! DayZPlayerCamera3rdPersonVehicle - 3rd person in vehicle (generic)
// *************************************************************************************
class DayZPlayerCamera3rdPersonVehicle extends DayZPlayerCameraBase
{
	static const float 	CONST_UD_MIN	= -85.0;		//!< down limit
	static const float 	CONST_UD_MAX	= 85.0;			//!< up limit
	const float UP_ANGLE_CAP 			= 10;			// max up angle, doesnt require gamecode sync

	static const float 	CONST_LR_MIN	= -160.0;		//!< down limit
	static const float 	CONST_LR_MAX	= 160.0;		//!< up limit
	
	const float CONST_LINEAR_VELOCITY_STRENGTH		= 0.025;
	const float CONST_ANGULAR_VELOCITY_STRENGTH		= 0.025;

	const float CONST_ANGULAR_LAG_YAW_STRENGTH		= 4.0;
	const float CONST_ANGULAR_LAG_PITCH_STRENGTH	= 1.5;
	const float CONST_ANGULAR_LAG_ROLL_STRENGTH		= 0.5;

	void DayZPlayerCamera3rdPersonVehicle( DayZPlayer pPlayer, HumanInputController pInput )
	{
		//Print("new camera: DayZPlayerCamera3rdPersonVehicle");

		m_fDistance 		= 4.0;
		m_CameraOffsetMS	= "0.0 1.3 0.0";
		
		HumanCommandVehicle vehicleCommand = pPlayer.GetCommand_Vehicle();
		if ( vehicleCommand )
		{
			Transport transport = vehicleCommand.GetTransport();
			if ( transport )
			{
				m_Transport			= transport;
				m_fDistance			= transport.GetTransportCameraDistance();
				m_CameraOffsetMS	= transport.GetTransportCameraOffset();
			}
		}
	}
	
	override void OnActivate( DayZPlayerCamera pPrevCamera, DayZPlayerCameraResult pPrevCameraResult )
	{
		if (pPrevCamera)
		{
			vector baseAngles		= pPrevCamera.GetBaseAngles();
			m_fUpDownAngle			= baseAngles[0]; 
			m_fLeftRightAngle		= baseAngles[1]; 

			vector addAngles		= pPrevCamera.GetAdditiveAngles();
			m_fUpDownAngleAdd		= addAngles[0];
			m_fLeftRightAngleAdd	= addAngles[1];
		}
				
		m_LagOffsetPosition = vector.Zero;
		m_fLagOffsetVelocityX[0] = 0;
		m_fLagOffsetVelocityY[0] = 0;
		m_fLagOffsetVelocityZ[0] = 0;
		
		m_fLagOffsetVelocityYaw[0] = 0;
		m_fLagOffsetVelocityPitch[0] = 0;
		m_fLagOffsetVelocityRoll[0] = 0;
		
		m_LagOffsetOrientation[0] = 0;
		m_LagOffsetOrientation[1] = 0;
		m_LagOffsetOrientation[2] = 0;
		
		if (m_pPlayer)
		{
			vector rotDiffLS = m_pPlayer.GetOrientation() * Math.DEG2RAD;
			m_LagOffsetOrientation[0] = rotDiffLS[0];
			m_LagOffsetOrientation[1] = rotDiffLS[1];
			m_LagOffsetOrientation[2] = rotDiffLS[2];
		}
	}

	override void OnUpdate( float pDt, out DayZPlayerCameraResult pOutResult )
	{
		//! get player transform
		vector playerTransformWS[4];
		m_pPlayer.GetTransform(playerTransformWS);

		//! get vehicle and set it as ignore entity for camera collision solver
		IEntity vehicle = m_Transport;
		pOutResult.m_CollisionIgnoreEntity = vehicle;
		
		//! update camera offset
		vector cameraPosition = vector.Zero;
		if ( vehicle )
		{
			vector vehiclePositionWS = vehicle.GetOrigin();
			vector vehiclePositionMS = vehiclePositionWS.InvMultiply4(playerTransformWS);
			cameraPosition = vehiclePositionMS + m_CameraOffsetMS;
		}

		//! orientation LS
		m_CurrentCameraYaw		= UpdateLRAngleUnlocked(m_fLeftRightAngle, m_fLeftRightAngleAdd, CONST_LR_MIN, CONST_LR_MAX, pDt);
		m_CurrentCameraPitch	= UpdateUDAngleUnlocked(m_fUpDownAngle, m_fUpDownAngleAdd, CONST_UD_MIN, CONST_UD_MAX, pDt);
		m_CurrentCameraRoll		= 0;
				
		//! create LS lag from vehicle velocities	
		vector posDiffLS = vector.Zero;
		vector rotDiffLS = vector.Zero;
		
		vector rotWS = m_pPlayer.GetOrientation();
		float maxRoll = 10;
		rotWS[2] = Math.Clamp(rotWS[2], -maxRoll, maxRoll) / maxRoll;
		rotWS[2] = rotWS[2] * rotWS[2] * rotWS[2];
		rotWS[2] = rotWS[2] * maxRoll;
		rotWS[2] = 0;
		
		rotWS = rotWS * Math.DEG2RAD;
		
		vector velocity = GetVelocity(vehicle);
		
		if (vehicle)
		{
			vector posDiffWS = velocity * CONST_LINEAR_VELOCITY_STRENGTH;
			posDiffLS = posDiffLS + posDiffWS.InvMultiply3(playerTransformWS);

			vector rotDiffWS = dBodyGetAngularVelocity(vehicle) * CONST_ANGULAR_VELOCITY_STRENGTH;
			rotDiffLS = rotDiffLS + rotDiffWS.InvMultiply3(playerTransformWS);
			
			vector tempRot1 = rotDiffLS;
			rotDiffLS[0] = tempRot1[1];
			rotDiffLS[1] = tempRot1[2];
			rotDiffLS[2] = tempRot1[0];
			
			rotWS[0] = rotWS[0] + rotDiffLS[0];
			rotWS[1] = rotWS[1] + rotDiffLS[1];
			rotWS[2] = rotWS[2] + rotDiffLS[2];
		}		
			
		//! smooth it!
		m_LagOffsetPosition[0] = Math.SmoothCD(m_LagOffsetPosition[0], posDiffLS[0], m_fLagOffsetVelocityX, 0.3, 1000, pDt);
		m_LagOffsetPosition[1] = Math.SmoothCD(m_LagOffsetPosition[1], posDiffLS[1], m_fLagOffsetVelocityY, 0.3, 1000, pDt);
		m_LagOffsetPosition[2] = Math.SmoothCD(m_LagOffsetPosition[2], posDiffLS[2], m_fLagOffsetVelocityZ, 0.3, 1000, pDt);

		m_LagOffsetOrientation[0] = Math.SmoothCDPI2PI(m_LagOffsetOrientation[0], rotWS[0], m_fLagOffsetVelocityYaw, 0.3, 1000, pDt);
		m_LagOffsetOrientation[1] = Math.SmoothCDPI2PI(m_LagOffsetOrientation[1], rotWS[1], m_fLagOffsetVelocityPitch, 0.3, 1000, pDt);
		m_LagOffsetOrientation[2] = Math.SmoothCDPI2PI(m_LagOffsetOrientation[2], rotWS[2], m_fLagOffsetVelocityRoll, 0.3, 1000, pDt);
		
		//! setup orientation
		vector rot = m_LagOffsetOrientation * Math.RAD2DEG;
		if (vehicle)
		{
			vector rotTM[3];
			Math3D.YawPitchRollMatrix(rot, rotTM);
			
			Math3D.MatrixInvMultiply3(playerTransformWS, rotTM, rotTM);
			
			// because we don't have m_fIgnoreParentYaw set
			vector tempRot = Math3D.MatrixToAngles(rotTM);
			rot[0] = tempRot[0];
		}
		
		rot[0] = rot[0] * (1.0 + (Math.InverseLerp(0.0, 1.0 + CONST_ANGULAR_LAG_YAW_STRENGTH, Math.AbsFloat(rotDiffLS[0])) * CONST_ANGULAR_LAG_YAW_STRENGTH));
		rot[1] = rot[1] * (1.0 + (Math.InverseLerp(0.0, 1.0 + CONST_ANGULAR_LAG_PITCH_STRENGTH, Math.AbsFloat(rotDiffLS[1])) * CONST_ANGULAR_LAG_PITCH_STRENGTH));
		rot[2] = rot[2] * (1.0 + (Math.InverseLerp(0.0, 1.0 + CONST_ANGULAR_LAG_ROLL_STRENGTH, Math.AbsFloat(rotDiffLS[2])) * CONST_ANGULAR_LAG_ROLL_STRENGTH));
				
		rot = GetCurrentOrientation() + rot;
		
		float distance = m_fDistance;		
		distance -= (2 * Math.Clamp(Math.InverseLerp(UP_ANGLE_CAP * 0.5, CONST_UD_MAX, rot[1]), 0, 1));
		
		if (rot[1] > UP_ANGLE_CAP)
		{
			rot[1] = UP_ANGLE_CAP;
		}
		
		Math3D.YawPitchRollMatrix(rot, pOutResult.m_CameraTM);
		
		//! setup position		
		pOutResult.m_CameraTM[3] = cameraPosition - m_LagOffsetPosition;
		
		//! setup rest
		pOutResult.m_fDistance 			= distance;
		pOutResult.m_fUseHeading 		= 0.0;
		pOutResult.m_fInsideCamera 		= 0.0;
		pOutResult.m_fIgnoreParentRoll 	= 1.0;
		pOutResult.m_fIgnoreParentPitch = 1.0;
		//pOutResult.m_fIgnoreParentYaw   = 1.0;
		pOutResult.m_bUpdateEveryFrame 	= g_Game.IsPhysicsExtrapolationEnabled();
		
		StdFovUpdate(pDt, pOutResult);
		
		super.OnUpdate(pDt, pOutResult);
	}

	override vector GetBaseAngles()
	{
		vector a;
		a[0] = m_fUpDownAngle;
		a[1] = m_fLeftRightAngle;
		a[2] = 0;
		return a;
	}

	override vector GetAdditiveAngles()
	{
		vector a;
		a[0] = m_fUpDownAngleAdd;
		a[1] = m_fLeftRightAngleAdd;
		a[2] = 0;
		return a;
	}

	//! runtime config
	protected 	vector  m_CameraOffsetMS;	//!< model space offset
	protected 	float 	m_fDistance;		//!< distance from start
	
	//! runtime values 
	protected 	float 	m_fUpDownAngle;			//!< up down angle in rad
	protected 	float 	m_fUpDownAngleAdd;		//!< up down angle in rad
	protected 	float 	m_fLeftRightAngle;		//!< left right angle in rad
	protected 	float 	m_fLeftRightAngleAdd;	//!< left right angle in rad
	
	//! lag offsets
	protected	vector	m_LagOffsetPosition;
	protected	vector	m_LagOffsetOrientation;
	protected	float	m_fLagOffsetVelocityX[1];
	protected	float	m_fLagOffsetVelocityY[1];
	protected	float	m_fLagOffsetVelocityZ[1];
	protected	float	m_fLagOffsetVelocityYaw[1];
	protected	float	m_fLagOffsetVelocityPitch[1];
	protected	float	m_fLagOffsetVelocityRoll[1];
	
	//! cache
	protected Transport m_Transport;
}