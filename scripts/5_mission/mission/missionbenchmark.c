//  Struct for individual benchmark locations 
class BenchmarkLocation
{
	bool m_IsDummyTeleport;
	bool m_IsDummyWait;
	float m_CamSpeedMultiplier = 1;
	string m_Name;
	vector m_StartPos;
	vector m_StartLookAtPos;
	
	void BenchmarkLocation(string name)
	{
		m_Name = name;
	}
	
	void SetPosition(vector start)
	{
		m_StartPos = start + "0 2.5 0"; // raise to head level
	}
	
	// note that vector[1] (height) is ignored during interpolation and setting it has no effect
	void SetLookAtPosition(vector start)
	{
		m_StartLookAtPos = start;
	}
	
	void SetCameraSpeedMultiplier(float multiplier)
	{
		m_CamSpeedMultiplier = multiplier;
	}
	
	void SetDummyTeleport()
	{
		m_IsDummyTeleport = true;
	}
	
	void SetDummyWait()
	{
		m_IsDummyWait = true;
	}
}

// Global settings of benchmark 
class BenchmarkConfig
{
	bool m_LogToRPT;
	bool m_DoDevPrints;
	int m_PreloadLength = 5;
	float m_TimeMultiplier = 1;
	string m_CSVName;
	
	ref array<ref BenchmarkLocation> m_Locations = new array<ref BenchmarkLocation>();
	
	void AddLocation(notnull BenchmarkLocation loc)
	{
		m_Locations.Insert(loc);
	}
	
	void AddQuickLocation(string name, vector pos, vector lookAtPos)
	{
		BenchmarkLocation loc = new BenchmarkLocation(name);
		loc.SetPosition(pos);
		loc.SetLookAtPosition(lookAtPos);
		
		m_Locations.Insert(loc);
	}
	
	void AddTeleport()
	{
		BenchmarkLocation loc = new BenchmarkLocation("Teleport");
		loc.SetDummyTeleport();
		m_Locations.Insert(loc);
	}
	
	void AddWait()
	{
		BenchmarkLocation loc = new BenchmarkLocation("Wait");
		loc.SetDummyWait();
		m_Locations.Insert(loc);
	}
	
	// false = csv, true = rpt
	void LogToRPT(bool logRPT)
	{
		m_LogToRPT = logRPT;
	}
	
	void DoDevPrints(bool doPrints)
	{
		m_DoDevPrints = doPrints;
	}
		
	// Preload time of individual locations after teleport
	void SetPreloadLengthTime(int seconds)
	{
		m_PreloadLength = seconds;
	}
	
	// Time multiplier for quickly testing flypath 
	void SetTestTimeMultiplier(float multiplier)
	{
		m_TimeMultiplier = multiplier;
	}
	
	void SetCSVName(string name)
	{
		m_CSVName = name;
	}
}

class MissionBenchmark : MissionGameplay
{	
	protected const int 		INITIAL_PRELOAD = 5; // seconds, extra preload seconds to compensate for the game launching
	protected const float 		STEP_INTERVAL = 1; 	// seconds, how much time passes between steps (fps measurements)
	
	protected bool 				m_InitialLoadDone;	// extra time is added to first preload to make up for intial loading of the game 
	protected bool 				m_IsPreloading;		// preload time between location measurements
	protected bool 				m_LocationDone;		// finished measuring current location
	protected int 				m_LocIndex;
	protected int 				m_MeasuringStep;
	protected float			 	m_MeasureStepTimer = 1;
	protected float 			m_SumFPS;
	protected float 			m_TimeCounter;
	protected float				m_MeasureLength;
	protected float				m_StepDistance;
	
	protected FileHandle 		m_CSVLog;
	protected BenchmarkLocation m_CurrentLocation;
	protected BenchmarkLocation m_NextLocation;
	
	protected ref BenchmarkConfig m_Config;
	
	protected static MissionBenchmark m_Instance;
		
	void MissionBenchmark()
	{
		m_Instance = this;
	}
	
	void ~MissionBenchmark()
	{
		m_Instance = null;
	}
	
	static MissionBenchmark GetInstance()
	{
		return m_Instance;
	}
	
	BenchmarkConfig GetConfig()
	{
		if (!m_Config)
			m_Config = new BenchmarkConfig();
		
		return m_Config;
	}
	
	void Start()
	{		
		if (!m_Config || m_Config.m_Locations.Count() <= 1)
		{
			OnBenchmarkEnd("Not enough locations defined");
			return;
		}
		
		if (!m_Config.m_LogToRPT)
			CreateCSVLog();
				
		m_IsPreloading = true;
		
		OnLocationSwitch();
	}
		
	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);

		m_TimeCounter += timeslice * m_Config.m_TimeMultiplier;
		
		if (!m_CurrentLocation)
			return;
		else if (m_IsPreloading)
			PreloadUpdate();
		else 
			MeasureUpdate(timeslice);
	}
	
	// Update logic which runs when location is being preloaded
	protected void PreloadUpdate()
	{
		if (!m_InitialLoadDone)
		{
			if (m_TimeCounter >= (m_Config.m_PreloadLength + INITIAL_PRELOAD)) // end preload
				m_InitialLoadDone = true;
		}
		else if (m_TimeCounter >= m_Config.m_PreloadLength * (1 / m_Config.m_TimeMultiplier)) // end preload
		{
			m_TimeCounter = 0;
			m_IsPreloading = false;
		}
	}
	
	// Update logic which runs when measurement is in progress
	protected void MeasureUpdate(float timeSlice)
	{		
		m_MeasureStepTimer += timeSlice;
		if (m_MeasureStepTimer >= STEP_INTERVAL)
		{
			float avgFps = GetGame().GetFps();
			
			if (m_Config.m_DoDevPrints)
				Print( string.Format("Measure step: %1 | FPS: %2" , m_MeasuringStep + 1, 1/avgFps) );

			/*if (m_MeasuringStep >= m_MeasureLength)	// end of steps
			{	}
			else*/	// next step
			
			m_MeasureStepTimer = 0;					
			m_SumFPS += ( 1/avgFps );
			m_MeasuringStep++;
			GetGame().GetPlayer().SetPosition(FreeDebugCamera.GetInstance().GetPosition() - "0 2.5 0");
		}
		
		LerpCamera();	
	}
	
 	protected void AdvanceLocation()
	{
		if (m_NextLocation.m_IsDummyWait && m_MeasuringStep < 5)
			return;
		
		string locationName = m_NextLocation.m_Name;
		FPSLog(locationName, m_SumFPS/m_MeasuringStep);
		
		if (m_Config.m_DoDevPrints)		
			Print( string.Format("%1 | Measurements: %2 | Average FPS: %3 | Elapsed time: %4 seconds" ,locationName, m_MeasuringStep, m_SumFPS/m_MeasuringStep ,m_TimeCounter) );
					
		m_LocIndex++;
		OnLocationSwitch();
	}
	
	// logic for interpolating camera between location points
	protected void LerpCamera()
	{
		float lerpX, lerpZ, lerpY;
		
		vector target = m_NextLocation.m_StartPos;
		float camSpeedAdjust = m_CurrentLocation.m_CamSpeedMultiplier * 5 * m_TimeCounter * 1/m_StepDistance;
		
		lerpX = Math.Lerp(m_CurrentLocation.m_StartPos[0], target[0], camSpeedAdjust);
		lerpZ = Math.Lerp(m_CurrentLocation.m_StartPos[1], target[1], camSpeedAdjust);
		lerpY = Math.Lerp(m_CurrentLocation.m_StartPos[2], target[2], camSpeedAdjust);
		
		if (camSpeedAdjust >= 1 || m_NextLocation.m_IsDummyWait)
		{
			AdvanceLocation();
			return;
		}
				
		FreeDebugCamera.GetInstance().SetPosition( Vector(lerpX, lerpZ, lerpY) );	
				
		target = m_NextLocation.m_StartLookAtPos;
		
		lerpX = Math.Lerp(m_CurrentLocation.m_StartLookAtPos[0], target[0], camSpeedAdjust);
		//lerpZ = Math.Lerp(m_CurrentLocation.m_StartLookAtPos[1], target[1], camSpeedAdjust); // ignored as it causes issues with lerping between look at points
		lerpY = Math.Lerp(m_CurrentLocation.m_StartLookAtPos[2], target[2], camSpeedAdjust);
		FreeDebugCamera.GetInstance().LookAt( Vector(lerpX, lerpZ, lerpY) );		
	}
	
	protected void OnLocationSwitch()
	{		
		if (m_LocIndex >= (m_Config.m_Locations.Count() - 1))
		{
			OnBenchmarkEnd("Test finished!");
			return;
		}
		
		m_MeasureStepTimer = 1; // tick first measurement straight after preload
		m_SumFPS = 0;
		m_MeasuringStep = 0;
		m_TimeCounter = 0;	
		m_CurrentLocation = m_Config.m_Locations[m_LocIndex];
		m_NextLocation = m_Config.m_Locations[m_LocIndex+1];
		m_StepDistance = vector.Distance(m_CurrentLocation.m_StartPos, m_NextLocation.m_StartPos);
		
		if (!GetGame().GetPlayer())
		{
			CreatePlayer();
			TeleportToPos(m_CurrentLocation);
		}
			
		if (m_NextLocation.m_IsDummyTeleport)	// flycam teleport
		{
			m_LocIndex += 2;
			if (m_LocIndex >= (m_Config.m_Locations.Count() - 1))
			{
				OnBenchmarkEnd("Test finished!");
				return;
			}
			else 
			{
				m_CurrentLocation = m_Config.m_Locations[m_LocIndex];
				m_NextLocation = m_Config.m_Locations[m_LocIndex+1];
				m_StepDistance = vector.Distance(m_CurrentLocation.m_StartPos, m_NextLocation.m_StartPos);
				TeleportToPos(m_CurrentLocation);
			}
		}	
		
		if (m_NextLocation.m_IsDummyWait)
		{
			m_NextLocation.m_Name = m_CurrentLocation.m_Name + " Wait";
			m_NextLocation.m_StartPos = m_CurrentLocation.m_StartPos;
			m_NextLocation.m_StartLookAtPos = m_CurrentLocation.m_StartLookAtPos;
		}
		
		if (m_Config.m_DoDevPrints)
		{
			Print(string.Format("================"));
			Print(string.Format("%1 test begin" , m_CurrentLocation.m_Name + " -> " + m_NextLocation.m_Name));
		}
	}
	
	protected void TeleportToPos(BenchmarkLocation loc)
	{
		FreeDebugCamera.GetInstance().SetPosition(loc.m_StartPos);
		vector lookAtPos = loc.m_StartLookAtPos;
		lookAtPos[1] = loc.m_StartPos[1];
		FreeDebugCamera.GetInstance().LookAt(lookAtPos);
		
		GetGame().GetPlayer().SetPosition(m_CurrentLocation.m_StartPos - "0 2.5 0");
		m_IsPreloading = true;
	}
	
	protected void OnBenchmarkEnd(string reason)
	{
		if (!m_Config.m_LogToRPT)
		{
			CloseFile( m_CSVLog );
			
			if (m_Config.m_DoDevPrints)
				Print( "Benchmark CSV file closed" );
		}
		
		if (m_Config.m_DoDevPrints)
			Print(string.Format("%1", reason));
		
		FreeDebugCamera.GetInstance().SetActive(false);
		GetGame().RequestExit( IDC_MAIN_QUIT );	// does not work on consoles ?
	}
	
	protected void CreatePlayer()
	{
		Entity playerEnt = GetGame().CreatePlayer(NULL, "SurvivorF_Eva", m_CurrentLocation.m_StartPos - "0 2.5 0", 0, "NONE");
		PlayerBase player = PlayerBase.Cast(playerEnt);
		GetGame().SelectPlayer(NULL, player);
		
		player.GetStatWater().Set(3000);
		player.GetStatEnergy().Set(3000);
		player.SetAllowDamage(false);
		player.SetCanBeDestroyed(false);
		
		FreeDebugCamera.GetInstance().SetFOV(0.72);
		FreeDebugCamera.GetInstance().SetActive(true);
	}
	
	protected void CreateCSVLog()
	{
		string fileName = "benchmark";
		if (m_Config.m_CSVName != string.Empty)
			fileName = m_Config.m_CSVName;
		
		m_CSVLog = OpenFile("$profile:" + fileName + ".csv", FileMode.WRITE);
		if ( m_CSVLog == 0 )	
			OnBenchmarkEnd("Failed to create benchmark .csv");
		
		if (m_Config.m_DoDevPrints)
			Print("Benchmark .csv created");
		
		FPrintln(m_CSVLog, "Location,FPS,Time");
	}
	
	protected void FPSLog( string position, float frames )
	{
		int year, month, day, hour, minute, second;
		GetYearMonthDay(year, month, day);
		GetHourMinuteSecondUTC(hour, minute, second);
		string timestamp = string.Format( "%1-%2-%3-%4-%5-%6", year, month, day, hour, minute, second );

		if (m_Config.m_LogToRPT)
			PrintToRPT("Average FPS: " + frames);
		else 
			FPrintln( m_CSVLog, position + "," + frames + "," + timestamp );
	}
}