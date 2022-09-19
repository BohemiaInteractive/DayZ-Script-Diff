
// Purpose of this file is to encapsulate Backlit Effects
// Basic implementatoin is for Razer Chroma devices, but API is made to remain universal for future purposes
//
// for specific method - watch UAInputAPI/ UAInput class
//
//


//-----------------------------------------------------------------------------
//! Input layer type
const int EUABLAYER_ALL				= 0;	// all layers
const int EUABLAYER_HINTKEY			= 1;	// showing hints
const int EUABLAYER_VISKEY			= 2;	// visualisation
const int EUABLAYER_3				= 3;	// 
const int EUABLAYER_CAR				= 4;	// 
const int EUABLAYER_HUMAN			= 5;	// 
const int EUABLAYER_6				= 6;	// 
const int EUABLAYER_CUSTOM			= 7;	// custom
const int EUABLAYER_LOADING			= 8;	// keys during loading sequence
const int EUABLAYER_MENU			= 9;	// keys during menu


//-----------------------------------------------------------------------------
//! Input backlit type
const int EUABACKLIT_NONE				= 0;	// turn off
const int EUABACKLIT_ON					= 1;	// permanent ilumination
const int EUABACKLIT_2					= 2;
const int EUABACKLIT_3					= 3;
const int EUABACKLIT_FADEOUT_SLOW		= 4;	// slow fadeout
const int EUABACKLIT_FADEOUT_FAST		= 5;	// fast fadeout
const int EUABACKLIT_FADEIN_SLOW		= 6;	// slow in -> then stay on
const int EUABACKLIT_FADEIN_FAST		= 7;	// fast in -> then stay on


// note: there should be states like:
//
//
//		LOADING/ MAIN MENU/ CONNECTING/ KEYBINDING/ OPTIONS - continuous
//		CHARACTER/ CAR/ "HELI"/ SPECTATOR/ INFECTED - continuous BASE
//		SWIMMING/ CRAWLING/ SPRINTING/ FIGHTING/ FLARE/ FLASHLIGHT - continuous CHARACTER
//
//	DoorOpen/ HitTaken/ CarCrashed/ etc. - event based
//


const int EUAB_OFF						= 0;	// all off

// logos :: anims
const int EUAB_LOGO_DAYZ				= 10;
const int EUAB_LOGO_CONNECTING			= 11;

// menus :: anims
const int EUAB_MENU_BROWSER				= 20;
const int EUAB_MENU_MAIN				= 21;
const int EUAB_MENU_KEYBINDING			= 22;
const int EUAB_MENU_OPTIONS				= 23;

// car :: basic background colour + overlay + transition anims
const int EUAB_CAR_OFF					= 100;	// sitting in car with engine off
const int EUAB_CAR_ON_NOLIGHTS			= 101;	// driving w/o lights (day)
const int EUAB_CAR_ON_LIGHTS			= 102;	// driving with headlights (night)
const int EUAB_CAR_STARTING				= 103;	// short starting animation
const int EUAB_CAR_ENTER				= 104;	// entering car
const int EUAB_CAR_LEAVE				= 105;	// leaving car
const int EUAB_CAR_CRASH				= 106;	// crashed

// character :: basic background colour + overlay + transition anims
const int EUAB_PLR_WALK					= 200;
const int EUAB_PLR_RUN					= 201;
const int EUAB_PLR_SPRINT				= 202;
const int EUAB_PLR_SWIM					= 203;
const int EUAB_PLR_HITBY				= 204;
const int EUAB_PLR_CONSUME				= 205;
const int EUAB_PLR_CRAFTING				= 206;
const int EUAB_PLR_EMOTE				= 207;	// playing emote
const int EUAB_PLR_UNCONSCIOUS			= 208;
const int EUAB_PLR_DEAD					= 209;

// modes :: these are set of background colours
const int EUAB_MODE_NIGHT				= 300;
const int EUAB_MODE_MORNING_BAD			= 301;
const int EUAB_MODE_MORNING_GOOD		= 302;
const int EUAB_MODE_DAY_FOGGY			= 303;
const int EUAB_MODE_DAY_OVERCAST		= 304;
const int EUAB_MODE_DAY_NICE			= 305;
const int EUAB_MODE_EVENING_BAD			= 306;
const int EUAB_MODE_EVENING_GOOD		= 307;

const int EUAB_MODE_FLASHLIGHT			= 320;
const int EUAB_MODE_CHEMLIGHT			= 321;


// overlay types
const int EUAB_OVERLAY_NONE				= 400;	// no overlay
const int EUAB_OVERLAY_CONTROLS			= 401;	// highlighted controls
const int EUAB_OVERLAY_STATUS			= 402;	// numpad + mouse used for health/ blood level visualisation
const int EUAB_OVERLAY_VON				= 403;	// VON status


//-----------------------------------------------------------------------------
//! Backlit effect class

class Backlit
{
//	private void ~Backlit(); // raist todo: turn lights off there?

	bool m_BacklitActive;

	void OnInit( DayZGame game )
	{
		// enable only on client/ standalone!
		if( game.IsClient() || !game.IsMultiplayer() )
			m_BacklitActive = true;
			
		if( m_BacklitActive )
			Print("... Backlit Effects Enabled");
	}

	// start infinite loading animation
	void LoadingAnim()
	{
		if( !m_BacklitActive )
			return;	// always return when backlit not present!
	
		// lighten up your desktop
		GetUApi().Backlit_KeyByName("kD",EUABLAYER_LOADING,EUABACKLIT_ON,0xffff0000);
		GetUApi().Backlit_KeyByName("kA",EUABLAYER_LOADING,EUABACKLIT_ON,0xffcf0000);
		GetUApi().Backlit_KeyByName("kY",EUABLAYER_LOADING,EUABACKLIT_ON,0xffaf0000);
		GetUApi().Backlit_KeyByName("kZ",EUABLAYER_LOADING,EUABACKLIT_ON,0xff8f0000);
		GetUApi().Backlit_Animation("Loading/",2,0xffc8c8c8,0xff080808);
		GetUApi().Backlit_ForceUpdate();
	}

	// ...
	void MainMenu_OnShow()
	{
		if( !m_BacklitActive )
			return;	// always return when backlit not present!
	
		// lighten up your desktop
		GetUApi().Backlit_Remove(EUABLAYER_ALL);
		GetUApi().Backlit_Animation("MainMenu/",2,0xffff0000,0xff080000);
		GetUApi().Backlit_ForceUpdate();
	}
	
	// ...
	void MainMenu_OnHide()
	{
		if( !m_BacklitActive )
			return;	// always return when backlit not present!
	
		GetUApi().Backlit_Remove(EUABLAYER_ALL);	
		GetUApi().Backlit_Background(0,0xff220000,0xff222222);		
	}

	// ...
	void OnEnterCar()
	{
		if( !m_BacklitActive )
			return;	// always return when backlit not present!
	
		GetUApi().Backlit_Remove(EUABLAYER_ALL);
		GetUApi().Backlit_Background(0,0xff211202,0xff110800);
	}

	// ...
	void OnLeaveCar()
	{
		if( !m_BacklitActive )
			return;	// always return when backlit not present!
	
		// force update player
		if( GetGame().GetMission().GetHud() )
		{
			GetUApi().Backlit_Remove(EUABLAYER_ALL);
			GetUApi().Backlit_Background(0,0xff220000,0xff222222);

			UpdatePlayer(true);
		}
	}

	// ...
	void OnSwimmingStart()
	{
		if( !m_BacklitActive )
			return;	// always return when backlit not present!
	
		// reset queue
		GetUApi().Backlit_EmptyQueue();
		// play hit animation
		GetUApi().Backlit_Animation("Water/",0,0xff00ffff,0xffffffff);
	}

	// ...
	void OnSwimmingStop()
	{
		if( !m_BacklitActive )
			return;	// always return when backlit not present!
	
		if( GetGame().GetMission().GetHud() )
		{		
			// enqueue background
			GetUApi().Backlit_Background(0,0xff220000,0xff222222);
			// force update player
			UpdatePlayer(true);
		}
	}

	// clear tutorial/ hint keys
	void HintClear()
	{
		if( !m_BacklitActive )
			return;	// always return when backlit not present!
	
		// remove previous backlit
		GetUApi().Backlit_Remove(EUABLAYER_HINTKEY);				
	}

	// highlight tutorial/ hint keys
	void HintShow( UAInput action )
	{
		if( !m_BacklitActive )
			return;	// always return when backlit not present!
	
		action.Backlit_Override(EUABLAYER_HINTKEY,0xffafafff);
	}

	// clear keybinding keys
	void KeybindingClear( )
	{
		if( !m_BacklitActive )
			return;	// always return when backlit not present!
	
		GetUApi().Backlit_Remove(EUABLAYER_ALL);
	}

	// clear keybinding keys
	void KeybindingShow( int keyHash )
	{
		if( !m_BacklitActive )
			return;	// always return when backlit not present!
	
		GetUApi().Backlit_KeyByHash(keyHash,EUABLAYER_VISKEY,EUABACKLIT_FADEOUT_SLOW,0xff0000ff);
	}


	void VisualiseHealth( int iLevel )
	{
		UAInputAPI ua_api = GetUApi();
			
		int aColor = 0xff1fff1f;
		int dColor = 0xff001f00;
				
		if( iLevel > 0 )
			ua_api.Backlit_KeyByName("kF1",EUABLAYER_HUMAN,EUABACKLIT_ON,aColor);
		else
			ua_api.Backlit_KeyByName("kF1",EUABLAYER_HUMAN,EUABACKLIT_ON,dColor );
		if( iLevel > 1 )
			ua_api.Backlit_KeyByName("kF2",EUABLAYER_HUMAN,EUABACKLIT_ON,aColor);
		else
			ua_api.Backlit_KeyByName("kF2",EUABLAYER_HUMAN,EUABACKLIT_ON,dColor );
		if( iLevel> 2 )
			ua_api.Backlit_KeyByName("kF3",EUABLAYER_HUMAN,EUABACKLIT_ON,aColor);
		else
			ua_api.Backlit_KeyByName("kF3",EUABLAYER_HUMAN,EUABACKLIT_ON,dColor );
		if( iLevel > 3 )
			ua_api.Backlit_KeyByName("kF4",EUABLAYER_HUMAN,EUABACKLIT_ON,aColor);
		else
			ua_api.Backlit_KeyByName("kF4",EUABLAYER_HUMAN,EUABACKLIT_ON,dColor );

/*		int lc = 0xff001f00;
		if( iLevel > 0 )
			lc = 0xff003f00;
		else if( iLevel > 1 )
			lc = 0xff007f00;
		else if( iLevel > 2 )
			lc = 0xff00af00;
		else if( iLevel > 3 )
			lc = 0xff00ff00;
			
		ua_api.Backlit_KeyByName("mBMiddle",EUABLAYER_HUMAN,EUABACKLIT_ON,lc);*/
		
	}
	
	void VisualiseBlood( int iLevel )
	{
		UAInputAPI ua_api = GetUApi();
				
		int aColor = 0xffff1f1f;
		int dColor = 0xff1f0000;
				
		if( iLevel > 0 )
			ua_api.Backlit_KeyByName("kF5",EUABLAYER_HUMAN,EUABACKLIT_ON,aColor);
		else
			ua_api.Backlit_KeyByName("kF5",EUABLAYER_HUMAN,EUABACKLIT_ON,dColor );
		if( iLevel > 1 )
			ua_api.Backlit_KeyByName("kF6",EUABLAYER_HUMAN,EUABACKLIT_ON,aColor);
		else
			ua_api.Backlit_KeyByName("kF6",EUABLAYER_HUMAN,EUABACKLIT_ON,dColor );
		if( iLevel > 2 )
			ua_api.Backlit_KeyByName("kF7",EUABLAYER_HUMAN,EUABACKLIT_ON,aColor);
		else
			ua_api.Backlit_KeyByName("kF7",EUABLAYER_HUMAN,EUABACKLIT_ON,dColor );
		if( iLevel > 3 )
			ua_api.Backlit_KeyByName("kF8",EUABLAYER_HUMAN,EUABACKLIT_ON,aColor);
		else
			ua_api.Backlit_KeyByName("kF8",EUABLAYER_HUMAN,EUABACKLIT_ON,dColor );
				
/*		int lc = 0xff1f0000;
		if( iLevel > 0 )
			lc = 0xff3f0000;
		else if( iLevel > 1 )
			lc = 0xff7f0000;
		else if( iLevel > 2 )
			lc = 0xffaf0000;
		else if( iLevel > 3 )
			lc = iLevel;
				
		ua_api.Backlit_KeyByName("mBRight",EUABLAYER_HUMAN,EUABACKLIT_ON,lc);*/
		
	}

	int m_HealthBefore = -1;
	int m_BloodBefore = -1;

	int m_HealthNew = 0;
	int m_BloodNew = 0;

	void SetHealth( float fHealth ) // range 0 .. 100
	{
		float health = fHealth * 0.01; // div 100
		m_HealthNew = health*4; // 4 stages
	}

	void SetBlood( float fBlood ) // range 0 .. 5000
	{
		float blood = fBlood * 0.0002; // div 5000
		m_BloodNew = blood*4; // 4 stages
	}

	void UpdatePlayer( bool bForce )
	{
		if( !m_BacklitActive )
			return;	// always return when backlit not present!
	
		// force update
		if( bForce )
		{
			m_HealthBefore = -1;
			m_BloodBefore = -1;
		}
	
		// do not update
		if( m_HealthNew == m_HealthBefore && m_BloodNew == m_BloodBefore )
			return;

		// remove previous layers
		GetUApi().Backlit_Remove(EUABLAYER_ALL);

		// set keys to layer CUSTOM
		GetUApi().Backlit_KeyByName("kW",EUABLAYER_CUSTOM,EUABACKLIT_ON,0xff0000ff);
		GetUApi().Backlit_KeyByName("kA",EUABLAYER_CUSTOM,EUABACKLIT_ON,0xff0000cf);
		GetUApi().Backlit_KeyByName("kS",EUABLAYER_CUSTOM,EUABACKLIT_ON,0xff0000af);
		GetUApi().Backlit_KeyByName("kD",EUABLAYER_CUSTOM,EUABACKLIT_ON,0xff00008f);

		GetUApi().Backlit_KeyByName("kX",EUABLAYER_CUSTOM,EUABACKLIT_ON,0xff3f3f3f);
		GetUApi().Backlit_KeyByName("kC",EUABLAYER_CUSTOM,EUABACKLIT_ON,0xff3f3f3f);

		GetUApi().Backlit_KeyByName("kQ",EUABLAYER_CUSTOM,EUABACKLIT_ON,0xff7f7f0f);
		GetUApi().Backlit_KeyByName("kE",EUABLAYER_CUSTOM,EUABACKLIT_ON,0xff7f7f0f);

		// health
		VisualiseHealth(m_HealthNew);
		// blood
		VisualiseBlood(m_BloodNew);

		// animation
		GetUApi().Backlit_Background(0,0xff220000,0xff222222);
		
		// save level
		m_BloodBefore = m_BloodNew;
		m_HealthBefore = m_HealthNew;
	}


	int m_GearBefore = 0;
	bool m_CELBefore = false;

	void RefreshVehicleLayout( int iGear, bool bCEL )
	{
		if( !m_BacklitActive )
			return;	// always return when backlit not present!
	
		UAInputAPI ua_api = GetUApi();

		if( m_GearBefore != iGear )
		{
			m_GearBefore = iGear;
			
			int activeColor = 0xff3fff3f;
			int dimmColor = 0xff0f3f0f;
				
			if( iGear == CarGear.REVERSE )
				ua_api.Backlit_KeyByName("kR",EUABLAYER_CAR,EUABACKLIT_ON,activeColor);
			else
				ua_api.Backlit_KeyByName("kR",EUABLAYER_CAR,EUABACKLIT_ON,dimmColor);

			if( iGear == CarGear.NEUTRAL )
				ua_api.Backlit_KeyByName("kN",EUABLAYER_CAR,EUABACKLIT_ON,activeColor);
			else
				ua_api.Backlit_KeyByName("kN",EUABLAYER_CAR,EUABACKLIT_ON,dimmColor);

			if( iGear == CarGear.FIRST )
				ua_api.Backlit_KeyByName("k1",EUABLAYER_CAR,EUABACKLIT_ON,activeColor);
			else
				ua_api.Backlit_KeyByName("k1",EUABLAYER_CAR,EUABACKLIT_ON,dimmColor);

			if( iGear == CarGear.SECOND )
				ua_api.Backlit_KeyByName("k2",EUABLAYER_CAR,EUABACKLIT_ON,activeColor);
			else
				ua_api.Backlit_KeyByName("k2",EUABLAYER_CAR,EUABACKLIT_ON,dimmColor);

			if( iGear == CarGear.THIRD )
				ua_api.Backlit_KeyByName("k3",EUABLAYER_CAR,EUABACKLIT_ON,activeColor);
			else
				ua_api.Backlit_KeyByName("k3",EUABLAYER_CAR,EUABACKLIT_ON,dimmColor);

			if( iGear == CarGear.FOURTH )
				ua_api.Backlit_KeyByName("k4",EUABLAYER_CAR,EUABACKLIT_ON,activeColor);
			else
				ua_api.Backlit_KeyByName("k4",EUABLAYER_CAR,EUABACKLIT_ON,dimmColor);

			if( iGear == CarGear.FIFTH )
				ua_api.Backlit_KeyByName("k5",EUABLAYER_CAR,EUABACKLIT_ON,activeColor);
			else
				ua_api.Backlit_KeyByName("k5",EUABLAYER_CAR,EUABACKLIT_ON,dimmColor);

		}

		if( bCEL != m_CELBefore )
		{		
			m_CELBefore = bCEL;

			if( bCEL )
				ua_api.Backlit_KeyByName("kC",EUABLAYER_CAR,EUABACKLIT_ON,0xffff0f0f);
			else
				ua_api.Backlit_KeyByName("kC",EUABLAYER_CAR,EUABACKLIT_NONE,0xff000000);
		}

	}

};



