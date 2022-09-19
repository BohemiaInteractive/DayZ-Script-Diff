class PluginCharPlacement extends PluginBase
{
	PluginDeveloper 	m_ModuleDeveloper;
	PlayerBase		 	m_Character;
	ref 				Timer m_Timer;
	bool 				m_Initialized;
	vector 				m_cam_pos;
	vector 				m_character_pos;
	vector 				m_camera_dir;
	vector 				m_cameraTrans[4];
	vector 				m_demoPos;
	vector 				m_demoRot;
	vector 				m_camera_orientation;
	
	const float 		FOV = 0.5236;
	
	void CheckInit()
	{
		if ( m_Initialized )
		{
			Print("logging...");
			Log(FOV.ToString(),"FOV");
			Log(m_camera_dir.ToString(),"camera dir");
			Log(m_cam_pos.ToString(),"camera pos");
			Log(m_demoPos.ToString(),"character pos");
			
			return;
		}
		
		if( FreeDebugCamera.GetInstance().IsActive() ) 
			Init();
	}
	
	void Init()
	{
		if(m_Character)
			m_Character.Delete();
		
		m_Initialized = true;
		m_Character = PlayerBase.Cast( GetGame().CreateObject("SurvivorF_Judy", FreeDebugCamera.GetInstance().GetPosition(), false, false/*ai*/) );
		
		FreeDebugCamera.GetInstance().SetFOV(FOV); //default scene FOV
		m_demoRot = "0 0 0";
	}
	
	override void OnUpdate(float delta_time)
	{
		if(!m_Initialized) return;
		if(!FreeDebugCamera.GetInstance().IsActive()) return;
		if(!m_Character) return;
		m_camera_orientation = FreeDebugCamera.GetInstance().GetOrientation();
		m_camera_orientation[1] = 1;
		FreeDebugCamera.GetInstance().SetOrientation(m_camera_orientation); //remove to unlock vertical axis
		m_cam_pos = FreeDebugCamera.GetInstance().GetPosition();
		m_character_pos = m_Character.GetPosition();
		m_camera_dir = FreeDebugCamera.GetInstance().GetDirection();
		
		m_camera_dir.Normalize();
		m_Character.SetPosition(CalculatePos());//(FreeDebugCamera.GetInstance().GetPosition() + ( m_camera_dir * 2) );
		m_demoRot[0] = Math.Atan2(-m_camera_dir[0], -m_camera_dir[2]) * Math.RAD2DEG;
		m_Character.SetOrientation(m_demoRot);//(FreeDebugCamera.GetInstance().GetPosition() - m_demoPos);
	}
	
	override void OnDestroy()
	{
		FreeDebugCamera.GetInstance().SetFOV(g_Game.GetUserFOV());
	}
	
	vector CalculatePos()
	{
		vector cam_pos = FreeDebugCamera.GetInstance().GetPosition();
		Math3D.MatrixIdentity4(m_cameraTrans);
		Math3D.DirectionAndUpMatrix(m_camera_dir/* - SnapToGround(cam_pos)*/, "0 1 0", m_cameraTrans);
		m_cameraTrans[3] = cam_pos;
		m_demoPos = Vector(/*0.685547, -0.988281,*/0,0, 3.68823).Multiply4(m_cameraTrans);
		m_demoPos[1] = GetGame().SurfaceY(m_demoPos[0], m_demoPos[2]);
		
		return m_demoPos;
	}
}
