class DayZIntroSceneXbox: Managed
{
	protected bool m_IsCharFemale;
	protected int m_LastPlayedCharacterID;
	
	protected ref TStringArray 	m_genderList;
	protected ref TStringArray 	m_CharPersonalityMaleList;
	protected ref TStringArray 	m_CharPersonalityFemaleList;
	protected ref TStringArray 	m_CharShirtList;
	protected ref TStringArray 	m_CharPantsList;
	protected ref TStringArray 	m_CharShoesList;
	protected ref TStringArray	m_AllCharacters;
	
	protected ref IntroSceneCharacter	m_Character;
	protected Camera					m_SceneCamera;
	protected Weather					m_Weather;
	protected Object					m_Clutter;
	protected vector					m_CharacterPos;
	protected vector					m_CharacterDir;
	protected ref TIntArray				m_Date = new TIntArray;

	protected MenuData					m_MenuData;
	
	protected ref MenuCarEngineSmoke	m_FXParticleCarSmoke;
	protected ref MenuEvaporation		m_FXParticleStreamLeft;
	protected ref MenuEvaporation		m_FXParticleStreamRight;
	
	ref Timer m_TimerUpdate = new Timer( CALL_CATEGORY_GAMEPLAY );
	ref Timer m_TimerParticle = new Timer( CALL_CATEGORY_GAMEPLAY );
	ref Timer m_TimerDate = new Timer( CALL_CATEGORY_GAMEPLAY );
	ref Timer m_TimerClientUpdate = new Timer( CALL_CATEGORY_GAMEPLAY );

	//==================================
	// DayZIntroSceneXbox
	//==================================
	void DayZIntroSceneXbox()
	{
		Print("DayZIntroSceneXbox Start");
		
		m_MenuData = g_Game.GetMenuData();
		
		m_LastPlayedCharacterID = m_MenuData.GetLastPlayedCharacter();
		m_CharacterPos = "0 0 0";
		m_CharacterDir = "0 0 0";
		
		//g_Game.m_PlayerName = "Survivor"; //default
		if ( m_MenuData.GetCharactersCount() == 0 )
		{
			m_LastPlayedCharacterID = -1;
		}
		
		if ( m_LastPlayedCharacterID > -1 )
		{
			m_MenuData.GetCharacterName(m_LastPlayedCharacterID, g_Game.GetPlayerGameName());
		}
	
		// Camera Setup
		vector camera_position;
		camera_position[0] 			= 1323.0;	// X
		camera_position[1] 			= 1.0;		// Y
		camera_position[2] 			= 1590.37;	// Z
		float camera_rotation_h		= 100;
		float camera_rotation_v		= -3;
		float camera_fov			= 0.85;
		float camera_focus_distance	= 0.0;
		float camera_focus_streght	= 0.0;
		
		// Character
		float character_distance = 2.25;
		
		// Camera Setup
		m_SceneCamera = CameraCreate(camera_position, camera_rotation_h, camera_rotation_v, camera_fov, camera_focus_distance, camera_focus_streght);
		m_SceneCamera.SetActive(true);
		
		PPEffects.Init(); //Deprecated, left in for legacy purposes only
		
		// Character Setup
		vector cam_dir = m_SceneCamera.GetDirection();
		m_CharacterPos = camera_position + ( cam_dir * character_distance );
		m_CharacterPos[1] = GetGame().SurfaceY(m_CharacterPos[0], m_CharacterPos[2]);
		m_CharacterDir = (camera_position - m_CharacterPos);
		
		m_Character = new IntroSceneCharacter();
		m_Character.LoadCharacterData(m_CharacterPos, m_CharacterDir);
		
		m_TimerParticle.Run(0.1, this, "SetupParticles", null, false);
		//m_TimerDate.Run(2.0, this, "SetupDate", null, false);
		m_TimerUpdate.Run(0.5, this, "SetupCharacter", null, true);
		
		vector clut_pos = SnapToGround( m_CharacterPos + "-1 0 0" );
		m_Clutter = GetGame().CreateObject( "ClutterCutter2x2", clut_pos, true );
		
		// Xbox check update
		CheckXboxClientUpdateLoopStart();
		
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Call(SetInitPostprocesses);
	}
	
	void ~DayZIntroSceneXbox()
	{
		if ( m_TimerUpdate )
		{
			m_TimerUpdate.Stop();
			delete m_TimerUpdate;
			m_TimerUpdate = null;
		}
		
		if ( m_TimerParticle )
		{
			m_TimerParticle.Stop();
			delete m_TimerParticle;
			m_TimerParticle = null;
		}
		
		
		if ( m_TimerDate )
		{
			m_TimerDate.Stop();
			delete m_TimerDate;
			m_TimerDate = null;
		}
		
		CheckXboxClientUpdateLoopStop();
		
		GetGame().ObjectDelete( m_SceneCamera );
		
		if ( m_MenuData )
		{
			m_MenuData.ClearCharacters();
		}
		
		SEffectManager.DestroyEffect(m_FXParticleCarSmoke);
		SEffectManager.DestroyEffect(m_FXParticleStreamLeft);
		SEffectManager.DestroyEffect(m_FXParticleStreamRight);
		
		PPEManagerStatic.GetPPEManager().StopAllEffects(PPERequesterCategory.ALL);
	}
	
	protected void SetInitPostprocesses()
	{
		PPERequester_MenuEffects requester;
		Class.CastTo(requester,PPERequesterBank.GetRequester(PPERequester_MenuEffects));
		
		requester.SetVignetteIntensity(0.3);
		PPERequesterBank.GetRequester(PPERequester_IntroChromAbb).Start();
	}
	
	//==============================================
	// GetIntroSceneCharacter
	//==============================================
	IntroSceneCharacter GetIntroCharacter()
	{
		return m_Character;
	}
	
	//==================================
	// SetupCharacter
	//==================================
	void SetupCharacter()
	{
		if ( m_Character.GetCharacterObj() )
		{
			vector v = m_Character.GetCharacterObj().GetOrientation();
			v[0] = -75;
			m_Character.GetCharacterObj().SetOrientation(v);
		}
	}
	
	//==================================
	// SetupParticles
	//==================================
	void SetupParticles()
	{
		m_FXParticleCarSmoke = new MenuCarEngineSmoke();
		//SEffectManager.PlayInWorld(m_FXParticleCarSmoke, "1330.36 2.11628 1594.31");
		//SEffectManager.PlayInWorld(m_FXParticleCarSmoke, "1333.88 1.51392 1594.88");
		SEffectManager.PlayInWorld(m_FXParticleCarSmoke, "1331.52 2.34052 1593.55");
		
		vector pos = m_SceneCamera.GetPosition() + m_SceneCamera.GetDirection() * 1.5;
		vector dir = m_SceneCamera.GetDirection();
		float temp = dir[0];
		dir[0] = dir[2];
		dir[2] = -temp;
			
		vector pos_right = pos + (dir * 1.5);
		vector pos_left = pos + (-dir * 1.5);
		
		m_FXParticleStreamLeft = new MenuEvaporation();
		SEffectManager.PlayInWorld(m_FXParticleStreamLeft, pos_right);
		
		m_FXParticleStreamRight = new MenuEvaporation();
		SEffectManager.PlayInWorld(m_FXParticleStreamRight, pos_left);
	}
	
	//==================================
	// SetupDate
	//==================================
	void SetupDate()
	{
		//g_Game.GetWorld().SetDate(m_Date.Get(0), m_Date.Get(1), m_Date.Get(2), m_Date.Get(3), m_Date.Get(4));
		//g_Game.GetWorld().SetDate(2020, 10, 15, 18, 20);
	}
	
	//==================================
	// CheckXboxClientUpdateLoopStart
	//==================================
	void CheckXboxClientUpdateLoopStart()
	{
		if ( CheckXboxClientUpdate() )
		{
			m_TimerClientUpdate.Run(30, this, "CheckXboxClientUpdate", null, true);
		}
	}
	
	//==================================
	// CheckXboxClientUpdateLoopStop
	//==================================
	void CheckXboxClientUpdateLoopStop()
	{
		if ( m_TimerClientUpdate )
		{
			m_TimerClientUpdate.Stop();
			delete m_TimerClientUpdate;
			m_TimerClientUpdate = null;
		}
	}
	
	//==================================
	// CheckXboxClientUpdate
	//==================================
	bool CheckXboxClientUpdate()
	{
		return OnlineServices.CheckUpdate();
	}
	
	//==================================
	// CameraCreate
	//==================================
	protected Camera CameraCreate(vector cam_pos, float cam_rot_h, float cam_rot_v, float cam_fov, float cam_focus_dist, float cam_focus_strg)
	{
		Camera cam = Camera.Cast( g_Game.CreateObject("staticcamera", SnapToGround(cam_pos), true));
		cam.SetOrientation( Vector( cam_rot_h, cam_rot_v, 0) );
		cam.SetFOV( cam_fov );
		cam.SetFocus(cam_focus_dist, cam_focus_strg);
		
		return cam;
	}
	
	//==================================
	// GetCamera
	//==================================
	Camera GetCamera()
	{
		return m_SceneCamera;
	}
	
	//==================================
	// ResetIntroCamera
	//==================================
	void ResetIntroCamera()
	{
		
	}
	
	//==================================
	// SetCharacterFemale
	//==================================
	void SetCharacterFemale(bool fem)
	{
		m_IsCharFemale = fem;
	}
	
	//==================================
	// IsCharacterFemale
	//==================================
	bool IsCharacterFemale()
	{
		return m_IsCharFemale;
	}
	
	//==================================
	// CreateRandomCharacter
	//==================================
	void CreateRandomCharacter()
	{
		string character_name;
		string params[2];
		
		m_IsCharFemale = Math.RandomInt(0, 2);
		
		if (m_IsCharFemale)
		{
			character_name = m_CharPersonalityFemaleList.GetRandomElement();
		}
		else
		{
			character_name = m_CharPersonalityMaleList.GetRandomElement();
		}
		
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(UpdateCharacterPos, 250);
	}
	
	void UpdateSelectedUserName()
	{
		string name;
		BiosUserManager user_manager = GetGame().GetUserManager();
		if( user_manager )
		{
			BiosUser user = user_manager.GetSelectedUser();
			if( user )
			{
				g_Game.SetPlayerGameName( user.GetName() );
				return;
			}
		}
		g_Game.SetPlayerGameName(GameConstants.DEFAULT_CHARACTER_NAME);
	}
	
	void UpdateCharacterPos()
	{
		if (m_Character.GetCharacterObj())
		{
			m_Character.GetCharacterObj().SetPosition(m_CharacterPos);
			m_Character.GetCharacterObj().SetDirection(m_CharacterDir.Normalized() );
		}
	}
	
	void SaveCharName( string name )
	{
		GetDayZGame().InitCharacterMenuDataInfo(m_MenuData.GetCharactersCount());
		
		//if (!GetDayZGame().IsNewCharacter() && m_LastPlayedCharacterID > -1)
		//{
			m_MenuData.SetCharacterName(m_LastPlayedCharacterID, name);			
		//}
		if (m_Character.IsDefaultCharacter())
		{
			GetGame().GetMenuDefaultCharacterData().SetCharacterName(name);
		}
		m_MenuData.SaveCharactersLocal();
	}
	
	// ------------------------------------------------------------
	vector SnapToGround(vector pos)
	{
		float pos_x = pos[0];
		float pos_z = pos[2];
		float pos_y = GetGame().SurfaceY(pos_x, pos_z);
		vector tmp_pos = Vector(pos_x, pos_y, pos_z);
		tmp_pos[1] = tmp_pos[1] + pos[1];
	
		return tmp_pos;
	}
};
