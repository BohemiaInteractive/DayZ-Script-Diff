class Rangefinder extends PoweredOptic_Base
{
	static const float RANGEFINDER_MAX_DISTANCE = 913.4856; //TODO adjust maximal distance to match real life rangefinder
	
	protected ref Timer 				m_Timer;
	protected Widget					m_Root;
	protected TextWidget				m_RangeText;
	
	protected string                    m_LayoutPath;
	
	void Rangefinder()
	{
		InitRangeFinderData();
	}
	
	void ~Rangefinder()
	{
		m_IsActionActive = false;
	}
	
	protected void InitRangeFinderData()
	{
		string path = "cfgVehicles " + GetType();
		if (GetGame().ConfigIsExisting(path))
		{
			string layout;
			if (GetGame().ConfigIsExisting(path + " rangeFinderLayout"))
			{
				GetGame().ConfigGetText(path + " rangeFinderLayout", layout);
			}
			
			if (layout != "" && layout.Length() > 0)
			{
				m_LayoutPath = layout;
			}
			else
			{
				m_LayoutPath = "gui/layouts/gameplay/rangefinder_hud.layout";
			}
		}
	}
	
	// How frequently the measurement should be taken
	float GetMeasurementUpdateInterval()
	{
		return 0.5;
	}
	
	override void OnWorkStart()
	{
		if (GetGame().IsServer() && !m_IsActionActive)	// incorrectly synchronized state from EM
			StopWorkServer();
		
		if( !GetGame().IsDedicatedServer())
		{
			PlayerBase player_this = PlayerBase.Cast( GetGame().GetPlayer() );
			PlayerBase player_owner = PlayerBase.Cast( GetHierarchyRootPlayer() );
			
			if ( player_this == player_owner )
			{
				StartPeriodicMeasurement();
			}
		}
	}
	
	override void OnWorkStop()
	{
		if( !GetGame().IsDedicatedServer())
		{
			PlayerBase player_this = PlayerBase.Cast( GetGame().GetPlayer() );
			PlayerBase player_owner = PlayerBase.Cast( GetHierarchyRootPlayer() );
			
			if ( player_this == player_owner )
			{
				StopPeriodicMeasurement();
			}
		}
	}
	
	void StartPeriodicMeasurement()
	{
		if( !m_Timer )
		{
			m_Timer = new Timer( CALL_CATEGORY_GAMEPLAY );
		}
		
		m_Root = GetGame().GetWorkspace().CreateWidgets( m_LayoutPath );
		
		// Either use root as text widget directly or find the text as children, arbitrary layout is supported now.
		m_RangeText = TextWidget.Cast(m_Root);
		if (!m_RangeText)
			m_RangeText = TextWidget.Cast(m_Root.FindAnyWidget("RangeText"));
		
		m_Timer.Run( GetMeasurementUpdateInterval(), this, "DoMeasurement", null, true );
	}
	
	void StopPeriodicMeasurement()
	{
		if( m_Timer )
		{
			m_Timer.Stop();
		}
		
		if (m_Root)
		{
			delete m_Root;
		}
	}
	
	protected void SetDistanceText(TextWidget text, float dist)
	{
		dist = Math.Round(dist);
		
		if (dist < RANGEFINDER_MAX_DISTANCE)
		{
			if( dist < 10 )
				text.SetText( "00" + dist.ToString() );
			else if( dist < 100 )
				text.SetText( "0" + dist.ToString() );
			else
				text.SetText( dist.ToString() );
		}
		else
		{
			SetInvalidText(text);
		}
	}
	
	protected void SetInvalidText(TextWidget text)
	{
		text.SetText( "---" );
	}
	
	// Measures the distance and returns result in formated string
	void DoMeasurement()
	{
		PlayerBase player = GetPlayer();
		
		if ( player )
		{
			vector 		from 			= GetGame().GetCurrentCameraPosition();
			vector      fromDirection   = GetGame().GetCurrentCameraDirection();
			vector 		to 				= from + (fromDirection * RANGEFINDER_MAX_DISTANCE);
			vector 		contact_pos;
			vector 		contact_dir;
			int 		contactComponent;
			
			bool hit = DayZPhysics.RaycastRV( from, to, contact_pos, contact_dir, contactComponent, NULL , NULL, player, false, false, ObjIntersectIFire);
			
			//         (a)
			// from -> --- <- horizEnd
			//   (h)   \ |
			//    to -> \| 
			
			// Generate result
			float h = vector.Distance( from, contact_pos );

			if (hit)
				SetDistanceText( m_RangeText, h );
			else
				SetInvalidText( m_RangeText );
			
			// Horizontal distance
			TextWidget angleText = TextWidget.Cast(m_Root.FindAnyWidget("AngleText"));
			TextWidget horizText = TextWidget.Cast(m_Root.FindAnyWidget("RangeHDText"));
			
			vector horizontalTo = Vector( contact_pos[0], from[1], contact_pos[2] );
			float a = vector.Distance( from, horizontalTo );
			
			// Angle between horizontal and actual line
			float heightDiff = contact_pos[1] - from[1];
			float angle = Math.Atan( heightDiff / a ) * Math.RAD2DEG;
			angle = Math.Round(angle);
			
			if (angleText)
			{
				if (hit)
					angleText.SetText(string.Format("%1", angle));
				else
					SetInvalidText( angleText );
			}
			
			if (horizText)
			{
				if (hit)
					SetDistanceText( horizText, a );
				else
					SetInvalidText( horizText );
			}
		}
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		RemoveAction(ActionViewOptics);
		AddAction(ActionViewBinoculars);
	}
	
	override void OnDebugSpawn()
	{
		GetInventory().CreateInInventory( "Battery9V" );
	}
}