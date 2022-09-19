class CameraToolsMenu extends UIScriptedMenu
{
	protected static CameraToolsMenu											m_Instance;
	vector 																		m_FreeflightCamTransform[4];
	protected vector															m_NextCamPosition;
	
	protected float																m_Time;
	
	protected Camera															m_Camera1;
	protected Camera															m_Camera2;
	
	protected ref array<ref Param6<vector, vector, float, float, float, int>>	m_CameraLines;
	protected bool																m_IsPlaying;
	protected int																m_NextCameraIndex = 1;
	
	protected float																m_FollowingTimeFinished;
	protected ref Param2<int, int>												m_FollowingBone;
	
	protected CTKeyframe														m_SelectedKeyframe;
	protected CTEvent															m_SelectedEvent;
	protected CTActor															m_SelectedActor;
	
	protected ref array<ref CTActor>											m_Actors;
	protected ref array<ref CTKeyframe>											m_Cameras;
	protected ref array<ref CTEvent>											m_Events;
	
	protected Widget															m_KeyframeContent;
	protected Widget															m_EventContent;
	
	protected XComboBoxWidget													m_InterpTypeSpeedCombo;
	protected XComboBoxWidget													m_InterpTypeCombo;
	protected EditBoxWidget														m_BlurEditWidget;
	protected EditBoxWidget														m_ActorTypeWidget;
	
	protected EditBoxWidget														m_ActorIdWidget;
	protected EditBoxWidget														m_ActorItemTypeWidget;
	
	protected ButtonWidget														m_AddItem;
	protected ButtonWidget														m_AddHandsItem;
	protected ButtonWidget														m_AddActor;
	protected ButtonWidget														m_Play;
	protected ButtonWidget														m_Stop;
	protected ButtonWidget														m_Save;
	protected ButtonWidget														m_Load;
	
	protected ButtonWidget														m_AddKeyframe;
	protected ButtonWidget														m_SaveKeyframe;
	protected ButtonWidget														m_DeleteKeyframe;
	protected ButtonWidget														m_ViewKeyframe;
	
	protected ButtonWidget														m_AddEvent;
	protected ButtonWidget														m_SaveEvent;
	protected ButtonWidget														m_DeleteEvent;
	protected ButtonWidget														m_ViewEvent;	
	
	protected ButtonWidget 														m_CopyButton;
	protected ButtonWidget 														m_ApplyButton;
	protected EditBoxWidget 													m_CameraEditbox;
	protected TextWidget 														m_CameraValues;
	
	void CameraToolsMenu()
	{
		m_Cameras	= new array<ref CTKeyframe>;
		m_Events	= new array<ref CTEvent>;
		m_Actors	= new array<ref CTActor>;
		
		m_Camera1	= Camera.Cast( GetGame().CreateObject( "staticcamera", GetGame().GetPlayer().GetPosition(), true ) );
		m_Camera2	= Camera.Cast( GetGame().CreateObject( "staticcamera", GetGame().GetPlayer().GetPosition(), true ) );
		
		m_Instance	= this;
	}
	
	void ~CameraToolsMenu()
	{
		m_Camera1.SetActive( false );
		m_Camera2.SetActive( false );
		
		//GetGame().ObjectDelete( m_Camera1 );
		//GetGame().ObjectDelete( m_Camera2 );
		
		DeveloperFreeCamera.DisableFreeCamera( PlayerBase.Cast( GetGame().GetPlayer() ), false );
		
		MissionGameplay ms	= MissionGameplay.Cast( GetGame().GetMission() );
		if (ms)
			ms.GetHud().Show( true );
	}
	
	override void OnShow()
	{
		super.OnShow();
		if ( !m_IsPlaying )
		{
			GetGame().GetMission().AddActiveInputExcludes({"menu"});
		}
	}
	
	override void OnHide()
	{
		super.OnHide();
		if ( !m_IsPlaying && GetGame() && GetGame().GetMission() && GetGame().GetUIManager())
		{
			GetGame().GetUIManager().ShowCursor(false);
			GetGame().GetMission().RemoveActiveInputExcludes({"menu"},true);
		}
	}
	
	void SelectKeyframe( CTKeyframe kf )
	{
		if ( m_SelectedKeyframe )
			m_SelectedKeyframe.Unselect();
		
		m_SelectedKeyframe = kf;
		m_SelectedKeyframe.Select();
	}
	
	void SelectEvent( CTEvent ev )
	{
		m_SelectedEvent = ev;
	}
	
	void SelectActor( CTActor ac )
	{
		m_SelectedActor = ac;
	}
	
	override Widget Init()
	{
		layoutRoot				= GetGame().GetWorkspace().CreateWidgets( "gui/layouts/camera_tools/camera_tools.layout" );
		
		m_KeyframeContent		= layoutRoot.FindAnyWidget( "keyframe_list_content" );
		m_EventContent			= layoutRoot.FindAnyWidget( "events_list_content" );
		m_InterpTypeCombo		= XComboBoxWidget.Cast( layoutRoot.FindAnyWidget( "interp_type" ) );
		m_InterpTypeSpeedCombo	= XComboBoxWidget.Cast( layoutRoot.FindAnyWidget( "interp_speed_type" ) );
		m_BlurEditWidget		= EditBoxWidget.Cast( layoutRoot.FindAnyWidget( "blur_value" ) );
		m_ActorTypeWidget		= EditBoxWidget.Cast( layoutRoot.FindAnyWidget( "add_actor_type" ) );
		m_ActorIdWidget			= EditBoxWidget.Cast( layoutRoot.FindAnyWidget( "add_actor_id" ) );
		m_ActorItemTypeWidget	= EditBoxWidget.Cast( layoutRoot.FindAnyWidget( "add_item_type" ) );
		
		m_AddKeyframe			= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "add_keyframe" ) );
		m_AddEvent				= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "add_event" ) );
		
		m_AddItem				= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "add_actor_item" ) );
		m_AddHandsItem			= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "add_actor_item_hands" ) );
		m_AddActor				= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "add_actor" ) );
		m_Play					= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "play" ) );
		m_Stop					= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "stop" ) );
		m_Save					= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "save" ) );
		m_Load					= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "load" ) );
		
		m_SaveKeyframe			= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "save_keyframe" ) );
		m_DeleteKeyframe		= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "delete_keyframe" ) );
		m_ViewKeyframe			= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "snap_to_keyframe" ) );
		m_SaveEvent				= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "save_event" ) );
		m_DeleteEvent			= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "delete_event" ) );
		m_ViewEvent				= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "snap_to_event" ) );		
		
		m_CopyButton			= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "CopyButton" ) );
		m_ApplyButton			= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "ApplyButton" ) );
		m_CameraEditbox			= EditBoxWidget.Cast( layoutRoot.FindAnyWidget( "CameraEditbox" ) );
		m_CameraValues			= TextWidget.Cast( layoutRoot.FindAnyWidget( "CameraValues" ) );
		
		MissionGameplay ms		= MissionGameplay.Cast( GetGame().GetMission() );
		ms.GetHud().Show( false );
		DeveloperFreeCamera.EnableFreeCameraSceneEditor( PlayerBase.Cast( GetGame().GetPlayer() ) );
		
		m_BlurEditWidget.SetText( "0.0" );
		return layoutRoot;
	}
	
	PlayerBase GetActor( int index )
	{
		if ( index > -1 && index < m_Actors.Count() )
			return PlayerBase.Cast( m_Actors.Get( index ).GetObj() );
		else
			return null;
	}
	
	static void PlayAnimOnActor( int actor, int anim )
	{
		if ( m_Instance )
		{
			PlayerBase p = m_Instance.GetActor( actor );
			if ( p )
			{
				EmoteCB cb = EmoteCB.Cast( p.StartCommand_Action( anim, EmoteCB, DayZPlayerConstants.STANCEMASK_ALL ) );
				cb.EnableCancelCondition(true);
			}
		}
	}
	
	void LoadData()
	{
		if ( !FileExist( g_Game.GetMissionFolderPath() + "/CameraTools" ) )
			MakeDirectory( g_Game.GetMissionFolderPath() + "/CameraTools" );
		
		m_Cameras.Clear();
		m_Events.Clear();
		m_Actors.Clear();
		
		CTSaveStructure save_data = new CTSaveStructure();
		JsonFileLoader<ref CTSaveStructure>.JsonLoadFile( g_Game.GetMissionFolderPath() + "/CameraTools/keyframe_data.json", save_data );
		
		m_InterpTypeCombo.SetCurrentItem( save_data.m_InterpType );
		m_InterpTypeSpeedCombo.SetCurrentItem( save_data.m_InterpSpeed );
		SetBlurValue( save_data.m_BlurValue );
		
		foreach ( Param6<vector, vector, float, float, float, int> transform : save_data.m_Cameras )
		{
			CTKeyframe kf = new CTKeyframe( m_Cameras.Count(), transform.param1, transform.param2, transform.param3, transform.param4, transform.param5, transform.param6, GetTotalTime( m_Cameras.Count() ), m_KeyframeContent, this );
			kf.SetInterpTime( transform.param3 );
			m_Cameras.Insert( kf );
		}
		
		foreach ( Param4<int, int, float, bool> event_t : save_data.m_Events )
		{
			CTEvent ev = new CTEvent( m_Events.Count(), event_t.param1, m_EventContent, event_t.param4, this );
			ev.SetEventType( event_t.param2 );
			ev.SetEventTime( event_t.param3 );
			m_Events.Insert( ev );
		}
		
		foreach( Param5<vector, vector, string, ref array<string>, string> actor_t : save_data.m_Actors )
		{
			CTActor ac = new CTActor( m_Actors.Count(), actor_t.param1, actor_t.param2, actor_t.param3, actor_t.param4, actor_t.param5, this );
			m_Actors.Insert( ac );
		}
	}
	
	void SaveData()
	{
		if ( !FileExist( g_Game.GetMissionFolderPath() + "/CameraTools" ) )
			MakeDirectory( g_Game.GetMissionFolderPath() + "/CameraTools" );
		
		CTSaveStructure save_data		= new CTSaveStructure();
		save_data.m_InterpType			= m_InterpTypeCombo.GetCurrentItem();
		save_data.m_InterpSpeed			= m_InterpTypeSpeedCombo.GetCurrentItem();
		save_data.m_BlurValue			= GetBlurValue();
		
		foreach ( CTKeyframe keyframe : m_Cameras )
		{
			Param6<vector, vector, float, float, float, int> transform = new Param6<vector, vector, float, float, float, int>( keyframe.GetPosition(), keyframe.GetOrientation(), keyframe.GetInterpTime(), keyframe.GetFOV(), keyframe.GetDOF(), keyframe.GetPin() );
			save_data.m_Cameras.Insert( transform );
		}
		
		foreach ( CTEvent event_t : m_Events )
		{
			Param4<int, int, float, bool> ev = new Param4<int, int, float, bool>( event_t.GetEventActor(), event_t.GetEventType(), event_t.GetEventTime(), event_t.GetEventWalk() );
			save_data.m_Events.Insert( ev );
		}
		
		foreach ( CTActor actor_t : m_Actors )
		{
			Param5<vector, vector, string, ref array<string>, string> ac = new Param5<vector, vector, string, ref array<string>, string>( actor_t.GetPosition(), actor_t.GetRotation(), actor_t.GetActorType(), actor_t.GetItems(), actor_t.GetHandsItem() );
			save_data.m_Actors.Insert( ac );
		}
		
		JsonFileLoader<ref CTSaveStructure>.JsonSaveFile( g_Game.GetMissionFolderPath() + "/CameraTools/keyframe_data.json", save_data );
	}
	
	float GetTotalTime( int index )
	{
		float time;
		index = Math.Clamp( index, 0, m_Cameras.Count() );
		for ( int i = 0; i < index; i++ )
		{
			time += m_Cameras.Get( i ).GetInterpTime();
		}
		return time;
	}
	
	void AddKeyframe()
	{
		if ( Camera.GetCurrentCamera() )
		{
			vector pos = Camera.GetCurrentCamera().GetWorldPosition();
			vector orient = Camera.GetCurrentCamera().GetOrientation();
			CTKeyframe kf = new CTKeyframe( m_Cameras.Count(), pos, orient, 1, 70, 50, -1, GetTotalTime( m_Cameras.Count() ), m_KeyframeContent, this );
			m_Cameras.Insert( kf );
			SelectKeyframe( kf );
		}
	}
	
	void SaveKeyframe()
	{
		if ( m_SelectedKeyframe && Camera.GetCurrentCamera() )
		{
			m_SelectedKeyframe.SetPosition( Camera.GetCurrentCamera().GetWorldPosition() );
			m_SelectedKeyframe.SetOrientation( Camera.GetCurrentCamera().GetOrientation() );
		}
	}
	
	void DeleteKeyframe()
	{
		if ( m_SelectedKeyframe )
		{
			int index = m_Cameras.Find( m_SelectedKeyframe );
			m_Cameras.Remove( index );
			index = Math.Clamp( index, 0, m_Cameras.Count() - 1 );
			if ( m_Cameras.Count() > 0 )
			{
				SelectKeyframe( m_Cameras.Get( index ) );
			}
		}
	}
	
	void ViewKeyframe()
	{
		if ( m_SelectedKeyframe && Camera.GetCurrentCamera() )
		{
			Camera.GetCurrentCamera().SetPosition( m_SelectedKeyframe.GetPosition() );
			Camera.GetCurrentCamera().SetOrientation( m_SelectedKeyframe.GetOrientation() );
		}
	}
	
	void AddActor( string actor_type )
	{
		vector pos = GetCameraLookDir();
		CTActor ac = new CTActor( m_Actors.Count(), pos, "0 0 0", actor_type, new array<string>, "", this );
		if ( ac.GetObj() )
			m_Actors.Insert( ac );
	}
	
	void AddEvent( int actor_index )
	{
		if ( actor_index > -1 )
		{
			CTEvent ev = new CTEvent( m_Events.Count(), actor_index, m_EventContent, false, this );
			m_Events.Insert( ev );
		}
	}
	
	
	void AddItem( string id, string type )
	{
		int id_int = id.ToInt();
		if ( m_Actors.Count() > id_int && id_int > -1 )
		{
			CTActor ac = m_Actors.Get( id_int );
			if ( ac )
				ac.AddItem( type );
		}
	}
	
	void AddHandsItem( string id, string type )
	{
		int id_int = id.ToInt();
		if ( m_Actors.Count() > id_int && id_int > -1 )
		{
			CTActor ac = m_Actors.Get( id_int );
			if ( ac )
				ac.SetHandsItem( type );
		}
	}
	
	void SaveEvent()
	{
		
	}
	
	void DeleteEvent()
	{
		
	}
	
	void ViewEvent()
	{
		
	}
	
	Param6<vector, vector, float, float, float, int> GetCameraLine( int index )
	{
		if ( index > -1 && index < m_CameraLines.Count() )
		{
			return m_CameraLines[index];
		}
		return null;
	}
	
	void Play()
	{
		if (Camera.GetCurrentCamera())
			Camera.GetCurrentCamera().GetTransform(m_FreeflightCamTransform);
		if ( m_Cameras.Count() > 0 )
		{
			layoutRoot.Show( false );
			GetGame().GetUIManager().ShowCursor( false );
			m_CameraLines = new array<ref Param6<vector, vector, float, float, float, int>>;
			
			foreach ( CTKeyframe keyframe : m_Cameras )
			{
				Param6<vector, vector, float, float, float, int> transform = new Param6<vector, vector, float, float, float, int>( keyframe.GetPosition(), keyframe.GetOrientation(), keyframe.GetInterpTime(), keyframe.GetFOV(), keyframe.GetDOF(), keyframe.GetPin() );
				m_CameraLines.Insert( transform );
				
			}
			
			foreach ( CTActor ac : m_Actors )
			{
				ac.Hide();
			}
			
			foreach ( CTEvent ev : m_Events )
			{
				ev.GetEventTime();
			}
			
			DeveloperFreeCamera.DisableFreeCamera( PlayerBase.Cast( GetGame().GetPlayer() ), false );
			
			int time					= SetCameraData( GetCameraLine( 0 ), GetCameraLine( 1 ) );
			
			if ( GetCameraLine( 0 ).param6 > -1 )
			{
				m_FollowingTimeFinished	= time;
				m_NextCamPosition = "0 0 0";
				m_Camera1.SetActive( true );
			}
			else
			{
				m_NextCamPosition		= m_Camera2.GetPosition();
				m_FollowingTimeFinished	= -1;
				m_Camera1.SetActive( true );
				m_Camera1.InterpolateTo( m_Camera2, time, m_InterpTypeCombo.GetCurrentItem() );
			}
			
			m_NextCameraIndex		= 1;
			m_IsPlaying				= true;
			m_Time					= 0;
		}
	}
	
	void Stop()
	{
		layoutRoot.Show( true );
		GetGame().GetUIManager().ShowCursor( true );
		
		m_Camera1.SetActive( false );
		m_Camera2.SetActive( false );
		FreeDebugCamera.GetInstance().SetActive(true);
		
		m_NextCameraIndex = 1;
		m_IsPlaying = false;
		
		foreach ( CTEvent ev : m_Events )
		{
			ev.Stop();
		}
		
		foreach ( CTActor ac : m_Actors )
		{
			ac.Reset();
			ac.Show();
		}
		
		Camera.GetCurrentCamera().SetTransform(m_FreeflightCamTransform);
	}
	
	vector GetCameraLookDir()
	{
		vector dir = FreeDebugCamera.GetInstance().GetDirection();
		vector from = FreeDebugCamera.GetInstance().GetPosition(); 
		vector to = from + ( dir * 10000 );   
		vector contact_pos;
		vector contact_dir;
		int contact_component;
		Object ev_obj;
		if ( m_SelectedEvent )
			ev_obj = GetActor( m_SelectedEvent.GetEventActor() );
		if ( DayZPhysics.RaycastRV(from, to, contact_pos, contact_dir, contact_component, null, null, ev_obj) )   
		{
			return contact_pos;
		}
		
		return "0 0 0";
	}
	
	vector GetMouseLookDir()
	{
		vector dir = GetGame().GetPointerDirection();
		vector from = FreeDebugCamera.GetInstance().GetPosition(); 
		vector to = from + ( dir * 10000 );   
		vector contact_pos;
		vector contact_dir;
		int contact_component;
		Object ev_obj;
		if ( m_SelectedEvent )
			ev_obj = GetActor( m_SelectedEvent.GetEventActor() );
		if ( DayZPhysics.RaycastRV(from, to, contact_pos, contact_dir, contact_component, null, null, ev_obj) )   
		{
			return contact_pos;
		}
		
		return "0 0 0";
	}
	
	override void Update( float timeslice )
	{
		
		string camPos = MiscGameplayFunctions.TruncateVecToS(FreeDebugCamera.GetInstance().GetPosition(),0);
		string camOri = MiscGameplayFunctions.TruncateVecToS(FreeDebugCamera.GetInstance().GetOrientation(),0);
		m_CameraValues.SetText("Pos:" +camPos + " | Rot:" + camOri);
		
		if (DiagMenu.GetBool(DiagMenuIDs.DM_ENABLE_REMOTE_CAMERA_BROADCAST))
		{
			Camera cam = Camera.GetCurrentCamera();
			
			if (!cam && m_Camera1)
				cam = m_Camera1; 
			if (cam)
			{
				Param4<vector, vector,float,float> p4 = new Param4<vector, vector,float,float>(GetGame().GetCurrentCameraPosition(),GetGame().GetCurrentCameraDirection(), cam.GetNearPlane(), cam.GetCurrentFOV());
				GetGame().RPCSingleParam(null, ERPCs.DEV_CAMERA_TOOLS_CAM_DATA, p4, false);
			}
		
		}
		if ( m_CameraLines && m_IsPlaying )
		{
			foreach ( CTEvent ev : m_Events )
			{
				if ( ev.IsTime( m_Time, m_Time + timeslice ) )
				{
					ev.Play();
				}
			}
			
			m_Time += timeslice;
			
			if ( !Camera.GetCurrentCamera() || !m_Camera1 || !m_Camera2 )
			{
				return;
			}
			
			if ( m_FollowingBone )
			{
				PlayerBase p = GetActor( m_FollowingBone.param1 );
				if ( p )
				{
					vector pos = p.GetBonePositionWS( m_FollowingBone.param2 );
								
					int bone_index = p.GetBoneIndexByName("LeftHand_Dummy");
					float bone_rot[4];
					p.GetBoneRotationWS( bone_index, bone_rot );
					
					vector new_orient = Math3D.QuatToAngles(bone_rot);
					
					//Print("Rot: "+ new_orient[0] +", "+ new_orient[1] +", "+ new_orient[2]);
					
					float temp = new_orient[1];
					new_orient[0] = new_orient[0] + 325; // vertical 
					new_orient[1] = new_orient[2] + 245;// horizontal
					new_orient[2] = temp + 290;			// tilt
					
					//Print("PostRot: "+ new_orient[0] +", "+ new_orient[1] +", "+ new_orient[2]);
					
					m_Camera1.SetPosition(pos);
					m_Camera1.SetOrientation(new_orient);
				}
			}
			
			if ( m_NextCameraIndex + 1 == m_CameraLines.Count() )
			{
				Stop();
				DeveloperFreeCamera.EnableFreeCameraSceneEditor( PlayerBase.Cast( GetGame().GetPlayer() ) );
				foreach ( CTEvent ev2 : m_Events )
				{
					ev2.Stop();
				}
				return;
			}
			
			if ( Camera.GetCurrentCamera().GetPosition() == m_NextCamPosition || ( m_FollowingTimeFinished != -1 && m_Time >= m_FollowingTimeFinished ) )
			{
				int time;
				if ( GetCameraLine( m_NextCameraIndex - 1 ).param6 > -1 )
				{
					m_NextCameraIndex--;
					GetCameraLine( m_NextCameraIndex ).param6 = -1;
					
					time			= SetCameraData( GetCameraLine( m_NextCameraIndex ), GetCameraLine( m_NextCameraIndex + 1 ) );
				}
				else
				{
					time			= SetCameraData( GetCameraLine( m_NextCameraIndex ), GetCameraLine( m_NextCameraIndex + 1 ) );
				}
				
				m_NextCameraIndex++;
				if ( GetCameraLine( m_NextCameraIndex - 1 ).param6 > -1 )
				{
					m_FollowingTimeFinished = time + m_Time;
					m_NextCamPosition = "0 0 0";
					m_Camera1.SetActive( true );
				}
				else if ( GetCameraLine( m_NextCameraIndex ) )
				{
					m_NextCamPosition	= GetCameraLine( m_NextCameraIndex ).param1;
					m_FollowingTimeFinished = -1;
					m_Camera1.SetActive( true );
					m_Camera1.InterpolateTo( m_Camera2, time, m_InterpTypeCombo.GetCurrentItem() );
				}
				else
				{
					Stop();
					DeveloperFreeCamera.EnableFreeCameraSceneEditor( PlayerBase.Cast( GetGame().GetPlayer() ) );
					FreeDebugCamera.GetInstance().SetOrientation( "0 0 0" );
					foreach( CTEvent ev3 : m_Events )
					{
						ev3.Stop();
					}
				}
			}
		}
		else
		{
			if ( m_Camera1.GetPosition() == m_NextCamPosition )
			{
				DeveloperFreeCamera.EnableFreeCameraSceneEditor( PlayerBase.Cast( GetGame().GetPlayer() ) );
			}
			
			for ( int i = 0; i < ( m_Cameras.Count() - 1 ); i++ )
			{
				vector pos0 = m_Cameras.Get( i ).GetPosition();
				vector pos1 = m_Cameras.Get( i + 1 ).GetPosition();
				Debug.DrawArrow( pos0, pos1, 2, COLOR_RED, ShapeFlags.ONCE );
			}
			
			if ( m_SelectedActor )
			{
				if ( !g_Game.IsLeftCtrlDown() )
					m_SelectedActor.SetPosition( GetMouseLookDir() );
				else
				{
					vector from	= m_SelectedActor.GetPosition();
					vector to	= GetMouseLookDir();
					Debug.DrawArrow( from, to, 0.5, COLOR_RED, ShapeFlags.ONCE );
					m_SelectedActor.SetRotation( vector.Direction( from,to ).Normalized().VectorToAngles() );
				}
					
			}
			
			foreach ( CTActor ac : m_Actors )
			{
				ac.Update( timeslice );
			}
		}
	}
	
	float SetCameraData( Param6<vector, vector, float, float, float,int> transform1, Param6<vector, vector, float, float, float, int> transform2 )
	{
		float int_value			= transform1.param3;
		if ( m_InterpTypeSpeedCombo.GetCurrentItem() == 1 )
		{
			int_value = vector.Distance( transform1.param1, transform2.param1 ) / int_value;
		}
		PlayerBase p;
		float pQuat[4];
		int b_index;
		if ( transform1.param6 > -1 )
		{
			p = GetActor( transform1.param6 );
			if ( p )
			{
				b_index = p.GetBoneIndexByName( "LeftHand_Dummy" );
				m_FollowingBone = new Param2<int, int>(transform1.param6, b_index);
			}
		}
		else
		{
			m_Camera1.SetPosition( transform1.param1 );
			m_Camera1.SetOrientation( transform1.param2 );
			m_FollowingBone = null;
		}
		m_Camera1.SetFOV( transform1.param4 * Math.DEG2RAD );
		m_Camera1.SetFocus( transform1.param5, GetBlurValue() );
		
		if ( transform2 )
		{
			if ( transform2.param6 > -1 )
			{
				pQuat[4];
				p = GetActor( transform2.param6 );
				if ( p )
				{
					b_index = p.GetBoneIndexByName( "LeftHand_Dummy" );
					if ( b_index != -1 )
					{
						m_Camera2.SetPosition( p.GetBonePositionWS( b_index ) );
				
						p.GetBoneRotationWS( b_index, pQuat);
						vector v = Math3D.QuatToAngles( pQuat );
						v[0] = fixAngle_180_180( v[0] - 180 );
						v[1] = fixAngle_180_180( v[1] - 270 );
						v[2] = fixAngle_180_180( v[2] + 90 );
						DbgUI.Text( v.ToString() );
						m_Camera2.SetYawPitchRoll( v );
					}
				}
			}
			else
			{
				m_Camera2.SetPosition( transform2.param1 );
				m_Camera2.SetOrientation( transform2.param2 );
			}
			
			m_Camera2.SetFOV( transform2.param4 * Math.DEG2RAD );
			m_Camera2.SetFocus( transform2.param5, GetBlurValue() );
		}
		
		return int_value;
	}
	
	void SetBlurValue( float blur )
	{
		m_BlurEditWidget.SetText( blur.ToString() );
	}
	
	float GetBlurValue()
	{
		return m_BlurEditWidget.GetText().ToFloat();
	}
	
	override bool OnMouseButtonDown( Widget w, int x, int y, int button )
	{
		if ( w == layoutRoot && button == MouseState.RIGHT && !m_IsPlaying )
		{
			GetGame().GetInput().ResetGameFocus();
			GetGame().GetInput().ChangeGameFocus(-1);
			GetGame().GetUIManager().ShowCursor(false);
			GetGame().GetMission().RemoveActiveInputExcludes({"menu"});
			SetFocus( layoutRoot );
			return true;
		}
		else if ( !IsFocusable( w ) )
		{
			SetFocus( layoutRoot );
		}
		return false;
	}
	
	override bool OnMouseButtonUp( Widget w, int x, int y, int button )
	{
		if ( w == layoutRoot && button == MouseState.LEFT )
		{
			SelectActor( null );
			return true;
		}
		return false;
	}
	
	override bool OnClick( Widget w, int x, int y, int button )
	{
		if ( button == MouseState.LEFT )
		{
			if ( w == m_SaveKeyframe )
			{
				SaveKeyframe();
				return true;
			}
			else if ( w == m_DeleteKeyframe )
			{
				DeleteKeyframe();
				return true;
			}
			else if ( w == m_ViewKeyframe )
			{
				ViewKeyframe();
				return true;
			}
			else if ( w == m_SaveEvent )
			{
				SaveEvent();
				return true;
			}
			else if ( w == m_DeleteEvent )
			{
				DeleteEvent();
				return true;
			}
			else if ( w == m_ViewEvent )
			{
				ViewEvent();
				return true;
			}
			else if ( w == m_AddKeyframe )
			{
				AddKeyframe();
				return true;
			}
			else if ( w == m_AddEvent )
			{
				AddEvent( 0 );
				return true;
			}
			else if ( w == m_Play )
			{
				Play();
				return true;
			}
			else if ( w == m_Stop )
			{
				Stop();
				return true;
			}
			else if ( w == m_Save )
			{
				SaveData();
				return true;
			}
			else if ( w == m_Load )
			{
				LoadData();
				return true;
			}
			else if ( w == m_AddActor )
			{
				AddActor( m_ActorTypeWidget.GetText() );
			}
			else if ( w == m_AddItem )
			{
				AddItem( m_ActorIdWidget.GetText(), m_ActorItemTypeWidget.GetText() );
			}
			else if ( w == m_AddHandsItem )
			{
				AddHandsItem( m_ActorIdWidget.GetText(), m_ActorItemTypeWidget.GetText() );
			}
			else if ( w == m_CopyButton )
			{
				string clipboardIn = FreeDebugCamera.GetInstance().GetPosition().ToString(false)+"|"+ FreeDebugCamera.GetInstance().GetOrientation().ToString(false);
				GetGame().CopyToClipboard( clipboardIn );
				
				//Print(clipboardIn);
			}
			else if ( w == m_ApplyButton )
			{
				string camText = m_CameraEditbox.GetText();
				if (camText && camText.IndexOf("|") !=-1)
				{
					int seperatorIndex = camText.IndexOf("|");
					int length = camText.Length();
					
					string posStr = camText.Substring(0,seperatorIndex);
					string oriStr = camText.Substring(seperatorIndex+1, length - seperatorIndex - 1);
					FreeDebugCamera.GetInstance().SetPosition(posStr.ToVector());
					FreeDebugCamera.GetInstance().SetOrientation(oriStr.ToVector());
				}
			}
		}
		return false;
	}
	
	
	override bool OnMouseEnter( Widget w, int x, int y )
	{
		if ( w == m_CameraEditbox)
			m_CameraEditbox.SetText("");
		return true;
	}
	
	override bool OnMouseLeave( Widget w, Widget enterW, int x, int y )
	{
		return true;
	}
	
	override bool OnFocus( Widget w, int x, int y )
	{
		if ( IsFocusable( w ) )
		{
			ColorRed( w );
			return true;
		}
		return false;
	}
	
	override bool OnFocusLost( Widget w, int x, int y )
	{
		if ( IsFocusable( w ) )
		{
			ColorWhite( w, null );
			return true;
		}
		return false;
	}
	
	bool IsFocusable( Widget w )
	{
		if ( w )
		{
			return ( w == m_SaveKeyframe  || w == m_DeleteKeyframe || w == m_ViewKeyframe || w == m_SaveEvent || w == m_DeleteEvent || w == m_ViewEvent || w == m_AddKeyframe || w == m_AddEvent || w == m_Play || w == m_Stop || w == m_Save || w == m_Load || w == m_InterpTypeCombo );
		}
		return false;
	}
	
	override bool OnKeyPress(Widget w, int x, int y, int key)
	{
		if (!w && key == KeyCode.KC_P )
		{
			if ( !m_IsPlaying )
			{
				Play();
				return true;
			}
			else
			{
				Stop();
				return true;
			}
			return false;
		}
		else if ( g_Game.IsLeftCtrlDown() && key == KeyCode.KC_S )
		{
			SaveKeyframe();
			return true;
		}
		else if ( g_Game.IsLeftCtrlDown() && key == KeyCode.KC_Q )
		{
			AddKeyframe();
			return true;
		}
		else if ( g_Game.IsLeftCtrlDown() && key == KeyCode.KC_E )
		{
			DeleteKeyframe();
			return true;
		}
		
		else if ( !w && g_Game.IsLeftCtrlDown() )
		{
			SelectActor( null );
			return true;
		}
		return false;
	}
	
	//Coloring functions (Until WidgetStyles are useful)
	void ColorRed( Widget w )
	{
		//SetFocus( w );
		if ( w.IsInherited( ButtonWidget ) )
		{
			ButtonWidget button = ButtonWidget.Cast( w );
			button.SetTextColor( ARGB( 255, 255, 0, 0 ) );
			button.SetAlpha( 0.9 );
		}
	}
	
	void ColorWhite( Widget w, Widget enterW )
	{
		if ( w.IsInherited( ButtonWidget ) )
		{
			ButtonWidget button = ButtonWidget.Cast( w );
			if (button)
			{
				button.SetTextColor( ARGB( 255, 255, 255, 255 ) );
				button.SetAlpha( 0.75 );
			}
		}
	}
}