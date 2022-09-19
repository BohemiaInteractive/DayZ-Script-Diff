// *************************************************************************************
// ! DayZPlayerCamera1stPersonVehicle - first person in vehicle
// *************************************************************************************
class DayZPlayerCamera1stPersonVehicle extends DayZPlayerCamera1stPerson
{
	override void OnUpdate(float pDt, out DayZPlayerCameraResult pOutResult)
	{
		super.OnUpdate(pDt, pOutResult);

		pOutResult.m_fUseHeading 		= 0.0;
	}
}

// *************************************************************************************
// ! DayZPlayerCamera3rdPersonVehicle - 3rd person in vehicle (generic)
// *************************************************************************************
class DayZPlayerCamera3rdPersonVehicle extends DayZPlayerCameraBase
{
	static const float 	CONST_UD_MIN	= -85.0;		//!< down limit
	static const float 	CONST_UD_MAX	= 85.0;			//!< up limit

	static const float 	CONST_LR_MIN	= -160.0;		//!< down limit
	static const float 	CONST_LR_MAX	= 160.0;		//!< up limit

	const float CONST_ANGULAR_LAG_YAW_STRENGTH   = 4;
	const float CONST_ANGULAR_LAG_PITCH_STRENGTH = 1.5;

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
			vector 	f = pPrevCamera.GetBaseAngles();
			m_fUpDownAngle		= f[0]; 
			m_fLeftRightAngle	= f[1]; 
			m_fUpDownAngleAdd	= f[2];
		}
		
		m_LagOffsetPosition = vector.Zero;
		m_fLagOffsetVelocityX[0] = 0;
		m_fLagOffsetVelocityY[0] = 0;
		m_fLagOffsetVelocityZ[0] = 0;
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
		float udAngle		= UpdateUDAngle(m_fUpDownAngle, m_fUpDownAngleAdd, CONST_UD_MIN, CONST_UD_MAX, pDt);
		m_fLeftRightAngle	= UpdateLRAngle(m_fLeftRightAngle, CONST_LR_MIN, CONST_LR_MAX, pDt);
		m_fRoll				= 0;
				
		//! create LS lag from vehicle velocities	
		vector posDiffLS = vector.Zero;
		vector orientDiff =  vector.Zero;

		if ( vehicle )
		{
			vector posDiffWS = GetVelocity(vehicle) * pDt;
			posDiffLS = posDiffWS.InvMultiply3(playerTransformWS);

			orientDiff = dBodyGetAngularVelocity(vehicle) * pDt * Math.RAD2DEG;
		}		
			
		//! smooth it!
		m_LagOffsetPosition[0] = Math.SmoothCD(m_LagOffsetPosition[0], posDiffLS[0], m_fLagOffsetVelocityX, 0.3, 1000, pDt);
		m_LagOffsetPosition[1] = Math.SmoothCD(m_LagOffsetPosition[1], posDiffLS[1], m_fLagOffsetVelocityY, 0.3, 1000, pDt);
		m_LagOffsetPosition[2] = Math.SmoothCD(m_LagOffsetPosition[2], posDiffLS[2], m_fLagOffsetVelocityZ, 0.3, 1000, pDt);

		m_LagOffsetOrientation[0] = Math.SmoothCD(m_LagOffsetOrientation[0], orientDiff[0], m_fLagOffsetVelocityYaw, 0.3, 1000, pDt);
		m_LagOffsetOrientation[1] = Math.SmoothCD(m_LagOffsetOrientation[1], orientDiff[1], m_fLagOffsetVelocityPitch, 0.3, 1000, pDt);
		
		//! setup orientation
		vector rot;
		rot[0] = m_fLeftRightAngle + CONST_ANGULAR_LAG_YAW_STRENGTH * m_LagOffsetOrientation[0];
		rot[1] = udAngle + CONST_ANGULAR_LAG_PITCH_STRENGTH * m_LagOffsetOrientation[1];
		rot[2] = m_fRoll;
	
		Math3D.YawPitchRollMatrix(rot, pOutResult.m_CameraTM);

		//! setup position		
		pOutResult.m_CameraTM[3] = cameraPosition - m_LagOffsetPosition;
		
		//! setup rest
		pOutResult.m_fDistance 			= m_fDistance;
		pOutResult.m_fUseHeading 		= 0.0;
		pOutResult.m_fInsideCamera 		= 0.0;
		pOutResult.m_fIgnoreParentRoll 	= 1.0;
		
		StdFovUpdate(pDt, pOutResult);
		
		super.OnUpdate(pDt, pOutResult);
	}	

	override vector GetBaseAngles()
	{
		vector a;
		a[0] = m_fUpDownAngle;
		a[1] = m_fLeftRightAngle;
		a[2] = m_fUpDownAngleAdd;
		return a;
	}

	//! runtime config
	protected 	vector  m_CameraOffsetMS;	//!< model space offset
	protected 	float 	m_fDistance;		//!< distance from start
	protected 	float 	m_fRoll;			//!< camera roll
	
	//! runtime values 
	protected 	float 	m_fUpDownAngle;		//!< up down angle in rad
	protected 	float 	m_fUpDownAngleAdd;	//!< up down angle in rad
	protected 	float 	m_fLeftRightAngle;	//!< left right angle in rad (in freelook only)
	
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