// *************************************************************************************
// ! DayZPlayerCamera1stPerson - 1st person camera
// *************************************************************************************
class DayZPlayerCamera1stPerson extends DayZPlayerCameraBase
{
	static const float 	CONST_UD_MIN	= -85.0;		//!< down limit
	static const float 	CONST_UD_MAX	= 85.0;			//!< up limit

	static const float 	CONST_LR_MIN	= -160.0;		//!< down limit
	static const float 	CONST_LR_MAX	= 160.0;		//!< up limit


	void 	DayZPlayerCamera1stPerson(DayZPlayer pPlayer, HumanInputController pInput)
	{
		m_iBoneIndex = pPlayer.GetBoneIndexByName("Head");
		if (m_iBoneIndex == -1)
		{
			Print("DayZPlayerCamera1stPerson: main bone not found");
		}
		
		m_OffsetLS = "0.04 0.04 0"; //x - up, y - forward
	}
	

	//
	override void 		OnActivate(DayZPlayerCamera pPrevCamera, DayZPlayerCameraResult pPrevCameraResult)
	{
		super.OnActivate(pPrevCamera, pPrevCameraResult);

		if (pPrevCamera)
		{
			vector baseAngles		= pPrevCamera.GetBaseAngles();
			m_fUpDownAngle			= baseAngles[0]; 
			m_fLeftRightAngle		= baseAngles[1]; 

			vector addAngles		= pPrevCamera.GetAdditiveAngles();
			m_fUpDownAngleAdd		= addAngles[0];
			m_fLeftRightAngleAdd	= addAngles[1];
		}
	}

	//	
	override void 		OnUpdate(float pDt, out DayZPlayerCameraResult pOutResult)
	{
		//! update angles from input
		m_CurrentCameraYaw		= UpdateLRAngleUnlocked(m_fLeftRightAngle, m_fLeftRightAngleAdd, CONST_LR_MIN, CONST_LR_MAX, pDt);
		m_CurrentCameraPitch	= UpdateUDAngleUnlocked(m_fUpDownAngle, m_fUpDownAngleAdd, CONST_UD_MIN, CONST_UD_MAX, pDt);
		
		//! 
		Math3D.YawPitchRollMatrix(GetCurrentOrientation(), pOutResult.m_CameraTM);
		pOutResult.m_CameraTM[3]		= m_OffsetLS;

		pOutResult.m_iDirectBone 		= m_iBoneIndex;
		pOutResult.m_iDirectBoneMode 	= 1;
		pOutResult.m_fUseHeading 		= 1.0;
		pOutResult.m_fInsideCamera 		= 1.0;

		pOutResult.m_fNearPlane = 0.04; //0.07 default
		
		ProcessCameraShake(pDt, m_fLeftRightAngle, m_fUpDownAngleAdd);
		
		InitCameraOnPlayer();
		StdFovUpdate(pDt, pOutResult);
		UpdateCameraNV(PlayerBase.Cast(m_pPlayer));
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
	
	override string GetCameraName()
	{
		return "DayZPlayerCamera1stPerson";
	}
	
	protected int		m_iBoneIndex;			//!< main bone
	protected vector	m_OffsetLS;				//!< position offset 

	protected float		m_fUpDownAngle;			//!< up down angle in rad
	protected float		m_fUpDownAngleAdd;		//!< up down angle in rad
	protected float		m_fLeftRightAngle;		//!< left right angle in rad (in freelook only)
	protected float		m_fLeftRightAngleAdd;	//!< left right angle in rad (in freelook only)
}

// *************************************************************************************
// ! DayZPlayerCamera1stPersonUnconscious - first person only unconscious
// *************************************************************************************
class DayZPlayerCamera1stPersonUnconscious extends DayZPlayerCamera1stPerson
{
	void DayZPlayerCamera1stPersonUnconscious(DayZPlayer pPlayer, HumanInputController pInput)
	{
		m_iBoneIndex		= pPlayer.GetBoneIndexByName ("Head");
	}
	
	override void OnUpdate(float pDt, out DayZPlayerCameraResult pOutResult)
	{
		super.OnUpdate(pDt, pOutResult);

		pOutResult.m_iDirectBone 		= m_iBoneIndex;
		pOutResult.m_iDirectBoneMode 	= 4;
		pOutResult.m_fUseHeading 		= 0.0;
		
		vector rot;
		rot[0] = 0;
		rot[1] = 90;
		rot[2] = 0;
		
		Math3D.YawPitchRollMatrix(rot, pOutResult.m_CameraTM);
	}
}
