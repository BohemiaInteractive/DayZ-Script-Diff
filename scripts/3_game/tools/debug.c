// TODO:
// 1. Alredy exist some key in array / map
// 2. Object is null (check object & log error)
// 3. Debug version only
// 4. Destructor of static classes
// 5. Debug Console Interface:
//		- Clear Log
//		- Filter
//		- Log per frame
// 6. Per frame log ?
// 7. Zapis do fajlu

class Debug
{	
	static private const string	LOG_DEBUG					= "Debug";
	static private const string	LOG_DEBUG_ACTION			= "Action";
	static private const string	LOG_DEBUG_SYMPTOM			= "Symptom";
	static private const string	LOG_DEBUG_INV_MOVE			= "Inv Move";
	static private const string	LOG_DEBUG_INV_RESERVATION	= "Inv Rrsv";
	static private const string	LOG_DEBUG_INV_HFSM			= "HFSM";
	static private const string LOG_DEBUG_TRIGGER			= "Trigger";
	static private const string LOG_DEBUG_PARTICLE			= "Particle";
	static private const string LOG_DEBUG_TF				= "TestFramework";
	static private const string LOG_DEBUG_WEIGHT			= "Weight";
	static private const string LOG_DEBUG_MELEE				= "Melee";
	
	static private const string	LOG_INFO					= "Info";
	static private const string	LOG_WARNING					= "Warning";
	static private const string	LOG_ERROR					= "Error";
	static private const string	LOG_DEFAULT					= "n/a";
	
	static private ref array<Shape>	m_DebugShapes;
	
	static Widget m_DebugLayoutCanvas;
	static CanvasWidget m_CanvasDebug;
	
	static string GetDebugName(Managed entity)
	{
		if (!entity)
			return "";
		
		Object obj;
		if (CastTo(obj, entity))
			return obj.GetDebugNameNative();
		
		return entity.GetDebugName();
	}
	
	static void InitCanvas()
	{
		if (!m_DebugLayoutCanvas)
		{
			m_DebugLayoutCanvas = GetGame().GetWorkspace().CreateWidgets("gui/layouts/debug/day_z_debugcanvas.layout");
			m_CanvasDebug = CanvasWidget.Cast( m_DebugLayoutCanvas.FindAnyWidget( "CanvasWidget" ) );
		}
	}

	static void ClearCanvas()
	{
		if (m_CanvasDebug)
			m_CanvasDebug.Clear();
	}
	
	static void CanvasDrawLine(float x1, float y1, float x2, float y2, float width, int color)
	{
		InitCanvas();
		m_CanvasDebug.DrawLine(x1, y1, x2, y2, width, color);
	}
	
	/**
	\brief Draws a "point" on the screen at x,y coordinates
	Debug.ClearCanvas();
	for(int i = 0; i < 700;i++)
	{
		float val = i/700;
		float y = Easing.EaseInOutExpo(val);
		Debug.CanvasDrawPoint(i,y*700,ARGBF( 0.6, 1, 1, 1 ));
	}
	*/
	
	static void CanvasDrawPoint(float x1, float y1, int color)
	{
		CanvasDrawLine(x1, y1, x1+1, y1, 1, color);
	}
	
	static void	Init()
	{
		m_DebugShapes	= new array<Shape>;
	}
	
	static void DestroyAllShapes()
	{
		for ( int i = 0; i < m_DebugShapes.Count(); ++i )
		{
			if ( m_DebugShapes.Get(i) )
			{
				m_DebugShapes.Get(i).Destroy();
			}
		}
		
		m_DebugShapes.Clear();
	}
	
	static void RemoveShape(out Shape shape)
	{
		if (!shape) return;
		for ( int i = 0; i < m_DebugShapes.Count(); i++ )
		{
			Shape found_shape = m_DebugShapes.Get(i);
			
			if ( found_shape  &&  found_shape == shape )
			{
				found_shape.Destroy();
				m_DebugShapes.Remove(i); // Mandatory! Otherwise the Destroy() function causes crash!
				shape = null;
				return;
			}
		}
	}
	/**
	\brief Prints debug message with normal prio
		\param msg \p string Debug message for print
		\return \p void None
		@code
			Debug.Log("Hello World");
			
			>> [Log]: Hello World;
		@endcode
	*/
	static void	Log(string message = LOG_DEFAULT, string plugin = LOG_DEFAULT, string author = LOG_DEFAULT, string label = LOG_DEFAULT, string entity = LOG_DEFAULT)
	{
		LogMessage(LOG_DEBUG, plugin, entity, author, label, message);
	}
	
	static void	ActionLog(string message = LOG_DEFAULT, string plugin = LOG_DEFAULT, string author = LOG_DEFAULT, string label = LOG_DEFAULT, string entity = LOG_DEFAULT)
	{
		LogMessage(LOG_DEBUG_ACTION, plugin, entity, author, label, message);
	}
	
	static void	SymptomLog(string message = LOG_DEFAULT, string plugin = LOG_DEFAULT, string author = LOG_DEFAULT, string label = LOG_DEFAULT, string entity = LOG_DEFAULT)
	{
		LogMessage(LOG_DEBUG_SYMPTOM, plugin, entity, author, label, message);
	}
	
	static void	InventoryMoveLog(string message = LOG_DEFAULT, string plugin = LOG_DEFAULT, string author = LOG_DEFAULT, string label = LOG_DEFAULT, string entity = LOG_DEFAULT)
	{
		LogMessage(LOG_DEBUG_INV_MOVE, plugin, entity, author, label, message);
	}
	
	static void	InventoryReservationLog(string message = LOG_DEFAULT, string plugin = LOG_DEFAULT, string author = LOG_DEFAULT, string label = LOG_DEFAULT, string entity = LOG_DEFAULT)
	{
		LogMessage(LOG_DEBUG_INV_RESERVATION, plugin, entity, author, label, message);
	}
	
	static void	InventoryHFSMLog(string message = LOG_DEFAULT, string plugin = LOG_DEFAULT, string author = LOG_DEFAULT, string label = LOG_DEFAULT, string entity = LOG_DEFAULT)
	{
		LogMessage(LOG_DEBUG_INV_HFSM, plugin, entity, author, label, message);
	}
	
	static void	TriggerLog(string message = LOG_DEFAULT, string plugin = LOG_DEFAULT, string author = LOG_DEFAULT, string label = LOG_DEFAULT, string entity = LOG_DEFAULT)
	{
		LogMessage(LOG_DEBUG_TRIGGER, plugin, entity, author, label, message);
	}
	
	static void	ParticleLog(string message = LOG_DEFAULT, Managed caller = null, string function = "", Managed entity = null)
	{
		LogMessage(LOG_DEBUG_PARTICLE, GetDebugName(caller), GetDebugName(entity), "", function, message);
	}
	
	static void	TFLog(string message = LOG_DEFAULT, TestFramework caller = null, string function = "")
	{
		LogMessage(LOG_DEBUG_TF, GetDebugName(caller), "", "", function, message);
	}
	
	static void	WeightLog(string message = LOG_DEFAULT, Managed caller = null, string function = "", Managed entity = null)
	{
		//LogMessage(LOG_DEBUG_WEIGHT, GetDebugName(caller), GetDebugName(entity), "", function, message);
	}
	
	static void	MeleeLog(Entity entity, string message = LOG_DEFAULT, string plugin = LOG_DEFAULT, string author = LOG_DEFAULT, string label = LOG_DEFAULT)
	{
		string logMessage = string.Format("%1: %2", entity.GetSimulationTimeStamp(), message);
		LogMessage(LOG_DEBUG_MELEE, plugin, GetDebugName(entity), author, label, message);
	}
	
	/**
	\brief Prints debug message with normal prio
		\param msg \p string Debug message for print
		\return \p void None
		@code
			Debug.Log("Hello World");
			
			>> [Log]: Hello World;
		@endcode
	*/
	static void	LogInfo(string message = LOG_DEFAULT, string plugin = LOG_DEFAULT, string author = LOG_DEFAULT, string label = LOG_DEFAULT, string entity = LOG_DEFAULT)
	{
		LogMessage(LOG_INFO, plugin, entity, author, label, message);
	}
	
	/**
	\brief Prints debug message as warning message
		\param msg \p string Debug message for warning print
		\return \p void None
		@code
			Debug.LogWarning("Hello World, this is warning log");
			
			>> [Warning]: Hello World, this is warning log
		@endcode
	*/
	static void	LogWarning(string message = LOG_DEFAULT, string plugin = LOG_DEFAULT, string author = LOG_DEFAULT, string label = LOG_DEFAULT, string entity = LOG_DEFAULT)
	{
		LogMessage(LOG_WARNING, plugin, entity, author, label, message);
	}
	
	/**
	\brief Prints debug message as error message
		\param msg \p string Debug message for error print
		\return \p void None
		@code
			Debug.LogError("Hello World, this is error log");
			
			>> [Error]: Hello World, this is error log
		@endcode
	*/
	static void	LogError(string message = LOG_DEFAULT, string plugin = LOG_DEFAULT, string author = LOG_DEFAULT, string label = LOG_DEFAULT, string entity = LOG_DEFAULT)
	{
		LogMessage(LOG_ERROR, plugin, entity, author, label, message);
	}
	
	static void	LogArrayInt(array<int> arr = NULL, string plugin = LOG_DEFAULT, string author = LOG_DEFAULT, string label = LOG_DEFAULT, string entity = LOG_DEFAULT)
	{
		if (arr== NULL) return;
		for (int i = 0; i < arr.Count(); i++)
		{
			LogMessage(LOG_DEBUG, plugin, entity, author, label, arr.Get(i).ToString() );
		
		}
	}
	
	static void	LogArrayString(array<string> arr = NULL, string plugin = LOG_DEFAULT, string author = LOG_DEFAULT, string label = LOG_DEFAULT, string entity = LOG_DEFAULT)
	{
		if (arr== NULL) return;	
		for (int i = 0; i < arr.Count(); i++)
		{
			LogMessage(LOG_DEBUG, plugin, entity, author, label, arr.Get(i) );	
		}
	}
	
	static void	ReceivedLogMessageFromServer(string message)
	{
		if ( LogManager.IsLogsEnable() )
			SaveLog(message);
	}
	
	static void ClearScriptLogs()
	{
		ClearLogs();
	}

	static Shape DrawBox(vector pos1, vector pos2, int color = 0x1fff7f7f)
	{
		return DrawBoxEx(pos1, pos2, color, ShapeFlags.TRANSP|ShapeFlags.NOZWRITE);
	}
	
	static Shape DrawBoxEx(vector pos1, vector pos2, int color = 0x1fff7f7f, ShapeFlags flags = ShapeFlags.TRANSP|ShapeFlags.NOZWRITE)
	{
		Shape shape = Shape.Create(ShapeType.BBOX, color, flags, pos1, pos2);
		if (( flags & ShapeFlags.ONCE ) == 0)
			m_DebugShapes.Insert(shape);
		return shape;
	}
	
	static Shape DrawCube(vector pos, float size = 1, int color = 0x1fff7f7f)
	{
		vector min = pos;
		vector max = pos;
		
		float size_h = size * 0.5;
	
		min[0] = min[0] - size_h;
		min[1] = min[1] - size_h;
		min[2] = min[2] - size_h;
		
		max[0] = max[0] + size_h;
		max[1] = max[1] + size_h;
		max[2] = max[2] + size_h;
		
		Shape shape = Shape.Create(ShapeType.DIAMOND, color, ShapeFlags.TRANSP|ShapeFlags.NOZWRITE, min, max);
		m_DebugShapes.Insert(shape);
		return shape;
	}
	
	static Shape DrawSphere(vector pos, float size = 1, int color = 0x1fff7f7f, ShapeFlags flags = ShapeFlags.TRANSP|ShapeFlags.NOOUTLINE)
	{
		Shape shape = Shape.CreateSphere(color, flags, pos, size);
		if (( flags & ShapeFlags.ONCE ) == 0)
			m_DebugShapes.Insert(shape);
		return shape;
	}
	
	static Shape DrawFrustum(float horizontalAngle, float verticalAngle, float length, int color = 0x1fff7f7f, ShapeFlags flags = ShapeFlags.TRANSP|ShapeFlags.WIREFRAME)
	{
		Shape shape = Shape.CreateFrustum(horizontalAngle, verticalAngle, length, color, flags);
		if (( flags & ShapeFlags.ONCE ) == 0)
			m_DebugShapes.Insert(shape);
		return shape;
	}
	
	static Shape DrawCylinder(vector pos, float radius, float height = 1, int color = 0x1fff7f7f, ShapeFlags flags = ShapeFlags.TRANSP|ShapeFlags.NOOUTLINE )
	{
		Shape shape = Shape.CreateCylinder(color, flags, pos, radius, height);
		if (( flags & ShapeFlags.ONCE ) == 0)
			m_DebugShapes.Insert(shape);
		return shape;
	}
	
	static array<Shape> DrawCone(vector pos, float lenght, float halfAngle, float offsetAngle, int color = 0xFFFFFFFF, int flags = 0)
	{
		array<Shape> shapes = new array<Shape>;
		
		vector endL, endR;
		Math3D.ConePoints(pos, lenght, halfAngle, offsetAngle, endL, endR);

		// Left side
		shapes.Insert( Debug.DrawLine(pos, endL, color, flags) );
		// Rigth side
		shapes.Insert( Debug.DrawLine(pos, endR, color, flags) );
		// Top side
		shapes.Insert( Debug.DrawLine(endL, endR, color, flags) );
		// Middle (height) line		
		shapes.Insert( Debug.DrawLine(pos, pos + Vector(Math.Cos(offsetAngle), 0, Math.Sin(offsetAngle)).Normalized() * lenght, color, flags) );
		
		return shapes;
	}
	
	/**
	DrawLine
	\nFlags:
	\n ShapeFlags.NOZBUFFER
	\n ShapeFlags.NOZUPDATE
	\n ShapeFlags.DOUBLESIDE
	\n ShapeFlags.WIREFRAME
	\n ShapeFlags.TRANSP
	\n ShapeFlags.ONCE
	\n ShapeFlags.NOOUTLINE
	\n ShapeFlags.NOCULL
	*/
	static Shape DrawLine(vector from, vector to, int color = 0xFFFFFFFF, int flags = 0)
	{
		vector pts[2]
		pts[0] = from;
		pts[1] = to;
		
		Shape shape = Shape.CreateLines(color, flags, pts, 2);
		if (( flags & ShapeFlags.ONCE ) == 0)
			m_DebugShapes.Insert(shape);
		//m_DebugShapes.Debug();
		return shape;
	}

	static Shape DrawLines(vector[] positions, int count, int color = 0xFFFFFFFF, int flags = 0)
	{
		
		Shape shape = Shape.CreateLines(color, flags, positions, count);
		if (( flags & ShapeFlags.ONCE ) == 0)
			m_DebugShapes.Insert(shape);
		return shape;
	}
	
	static Shape DrawArrow(vector from, vector to, float size = 0.5, int color = 0xFFFFFFFF, int flags = 0)
	{
		Shape shape = Shape.CreateArrow(from, to, size, color, flags);
		m_DebugShapes.Insert(shape);
		return shape;
	}

	
	
	/**
	\brief Returns some of base config classes strings like CfgVehicles, CfgWeapons, etc. for searching purposes
		\param base_classes \p out TStringArray Array containing some of base config classes
	*/
	static void GetBaseConfigClasses( out TStringArray base_classes )
	{
		base_classes.Clear();
		base_classes.Insert(CFG_VEHICLESPATH);
		base_classes.Insert(CFG_WEAPONSPATH);
		base_classes.Insert(CFG_MAGAZINESPATH);
		base_classes.Insert(CFG_AMMO);
		base_classes.Insert(CFG_WORLDS);
		base_classes.Insert(CFG_SURFACES);
		base_classes.Insert(CFG_SOUND_SETS);
		base_classes.Insert(CFG_SOUND_SHADERS);
		base_classes.Insert(CFG_NONAI_VEHICLES);
	}
	
	/**
	\brief Returns config classes containing search_string in name
		\param search_string \p string String to search in class names
		\param filtered_classes \p out TStringArray Array containing filtered classes based on search_string
		\param only_public \p bool Set to true to return only public classes, set to false to return all filtered classes
	*/
	static void GetFiltredConfigClasses( string search_string, out TStringArray filtered_classes, bool only_public = true )
	{	
		TStringArray searching_in = new TStringArray;
		GetBaseConfigClasses( searching_in );
		
		filtered_classes.Clear();
		
		search_string.ToLower();
		
		for ( int s = 0; s < searching_in.Count(); ++s )
		{
			string config_path = searching_in.Get(s);
			
			int objects_count = GetGame().ConfigGetChildrenCount(config_path);
			for (int i = 0; i < objects_count; i++)
			{
				string childName;
				GetGame().ConfigGetChildName(config_path, i, childName);
	
				if ( only_public )
				{
					int scope = GetGame().ConfigGetInt( config_path + " " + childName + " scope" );
					if ( scope == 0 )
					{
						continue;
					}
				}
				
				string nchName = childName;
				nchName.ToLower();
	
				if ( nchName.Contains(search_string) != -1)
				{
					filtered_classes.Insert(childName);
				}
			}
		}
	}
	
	//---------------------------------------------------------------
	//-------private
	
	static private bool				m_EnabledLogs;
		
	static private void LogMessage(string level, string plugin, string entity, string author, string label, string message)
	{
		if (GetGame() == NULL || LogManager.IsLogsEnable() == false )
		{
			return;
		}
		
		bool is_server_log = ( GetGame().IsServer() && GetGame().IsMultiplayer() );
		
		
		// Formation output to external file
		// %date{MM-dd HH:mm:ss} | %Enviroment | %Level | %Module | %Entity | %Author | %Label | %Message	
		string date			= GetDate();
		string env			= "Client";
		string msg			= string.Empty;
		
		if ( is_server_log )
		{
			env = "Server";
		}
		
		msg = string.Format("%1 | %2 | %3 | %4 | %5 | %6 | %7", date, env, level, plugin, entity, label, message);
	
		if ( is_server_log )
		{
			SaveLog(msg);
#ifdef DEVELOPER //not sendig log to clients on stable
			Param1<string> msg_p = new Param1<string>(msg);
			CallMethod(CALL_ID_SEND_LOG, msg_p);
#endif
		}
		else
		{
			SaveLog(msg);
		}
	}
	
	static private void	SaveLog(string log_message)
	{
		Print("" + log_message);

		//Previous was saved to separate file
		/*FileHandle file_index = OpenFile(GetFileName(), FileMode.APPEND);
		
		if ( file_index == 0 )
		{
			return;
		}
			
		FPrintln(file_index, log_message);
		
		CloseFile(file_index);*/
	}
	
	static private void	ClearLogs()
	{
		if ( FileExist( GetFileName() ) )
		{
			FileHandle file_index = OpenFile( GetFileName(), FileMode.WRITE );
			
			if ( file_index == 0 )
			{
				return;
			}
				
			FPrintln( file_index, "" );
			
			CloseFile( file_index );	
		}
	}
	
	static private string GetFileName()
	{
		return CFG_FILE_SCRIPT_LOG_EXT;
	}
	
	static private string GetDate()
	{
		int year;
		int month;
		int day;
		int hour;
		int minute;
		int second;
		
		GetYearMonthDay(year, month, day);
		GetHourMinuteSecond(hour, minute, second);
		
		string date = month.ToStringLen(2) + "-" + day.ToStringLen(2) + " " + hour.ToStringLen(2) + ":" + minute.ToStringLen(2) + ":" + second.ToStringLen(2);
		
		return date;
	}
};

class LogManager
{
	static bool m_DoLogs;
	static bool m_DoActionDebugLog;
	static bool m_DoSymptomDebugLog;
	static bool m_DoInventoryMoveLog;
	static bool m_DoInventoryReservationLog;
	static bool m_DoInventoryHFSMLog;
	static bool m_DoWeaponLog;
	
	static void Init()
	{
#ifdef DEVELOPER 
		m_DoLogs = true;
#else
		m_DoLogs = false;
#endif

		m_DoActionDebugLog = IsCLIParam("doActionLog");
		m_DoSymptomDebugLog = IsCLIParam("doSymptomLog");
		m_DoInventoryMoveLog = IsCLIParam("doInvMoveLog");
		m_DoInventoryReservationLog = IsCLIParam("doInvReservLog");
		m_DoInventoryHFSMLog = IsCLIParam("doInvHFSMLog");
		m_DoWeaponLog = IsCLIParam("doWeaponLog");
	}
	
	static bool IsLogsEnable()
	{
		return m_DoLogs;
	}
	
	static void SetLogsEnabled(bool enable)
	{
		m_DoLogs = enable;
	}
	
	static bool IsActionLogEnable()
	{
		return m_DoActionDebugLog;
	}
	
	static void ActionLogEnable(bool enable)
	{
		m_DoActionDebugLog = enable;
	}
	
	static bool IsInventoryMoveLogEnable()
	{
		return m_DoInventoryMoveLog;
	}
	
	static void InventoryMoveLogEnable(bool enable)
	{
		m_DoInventoryMoveLog = enable;
	}
	
	static bool IsInventoryReservationLogEnable()
	{
		return m_DoInventoryReservationLog;
	}
	
	static void InventoryReservationLogEnable(bool enable)
	{
		m_DoInventoryReservationLog = enable;
	}
	
	static bool IsInventoryHFSMLogEnable()
	{
		return m_DoInventoryHFSMLog;
	}
	
	static void InventoryHFSMLogEnable(bool enable)
	{
		m_DoInventoryHFSMLog = enable;
	}
	
	static bool IsSymptomLogEnable()
	{
		return m_DoSymptomDebugLog;
	}
	
	static void SymptomLogEnable(bool enable)
	{
		m_DoSymptomDebugLog = enable;
	}
	
	static bool IsWeaponLogEnable()
	{
		return m_DoWeaponLog;
	}
	
	static void WeaponLogEnable(bool enable)
	{
		m_DoWeaponLog = enable;
	}
}
