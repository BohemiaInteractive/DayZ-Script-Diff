class PluginUniversalTemperatureSourceClient extends PluginBase
{
	const int MAX_SIMULTANEOUS_UTS = 10;

	protected float m_UTSAverageTemperature;

	protected ref array<ref UTemperatureSourceDebug> m_UTemperatureSourceDebugs;
	
	protected ref Widget m_RootWidget[MAX_SIMULTANEOUS_UTS];
	protected ref TextListboxWidget m_StatListWidgets[MAX_SIMULTANEOUS_UTS];
	
	protected PlayerBase m_Player;
	
	void PluginUniversalTemperatureSourceClient()
	{
		m_UTemperatureSourceDebugs = new array<ref UTemperatureSourceDebug>();
	}
	
	override void OnInit()
	{
		#ifndef NO_GUI
		InitWidgets();
		#endif
	}
	
	override void OnUpdate(float delta_time)
	{
		#ifndef NO_GUI
		if (!m_Player)
		{
			return;
		}

		UpdateStatWidgets();
		DrawDebugs();
		#endif
	}
	
	void InitWidgets()
	{
		for (int i = 0; i < MAX_SIMULTANEOUS_UTS; i++)
		{
			m_RootWidget[i] = GetGame().GetWorkspace().CreateWidgets("gui/layouts/debug/day_z_debug_remoteinfo.layout");
			m_StatListWidgets[i] = TextListboxWidget.Cast(m_RootWidget[i].FindAnyWidget("TextListboxWidget0"));
		}
	}
	
	void DrawDebugs()
	{
		for (int i = 0; i < m_UTemperatureSourceDebugs.Count(); i++)
		{
			UTemperatureSourceDebug utsd = m_UTemperatureSourceDebugs.Get(i);

			float fullRange = utsd.GetValue(1).ToFloat();
			float maxRange 	= utsd.GetValue(2).ToFloat();
			float temp		= utsd.GetValue(3).ToFloat();
			vector sphPos 	= utsd.GetValue(0).ToVector();

			int fullRangeColor = COLOR_RED_A;			
			int maxRangeColor = COLOR_YELLOW_A;
			if (temp < 0)
			{
				fullRangeColor = COLOR_GREEN_A;
				maxRangeColor = COLOR_BLUE_A;
			}

			Debug.DrawCylinder(sphPos, fullRange, fullRange, fullRangeColor, ShapeFlags.ONCE|ShapeFlags.TRANSP);
			Debug.DrawCylinder(sphPos, maxRange, maxRange, maxRangeColor, ShapeFlags.ONCE|ShapeFlags.TRANSP);
			Debug.DrawArrow(m_Player.GetPosition(), sphPos, 1.0, 0xffffffff, ShapeFlags.ONCE|ShapeFlags.TRANSP);
		}
		
		ProcessUniversalTemperatureSources();
		
		if (m_UTemperatureSourceDebugs.Count() > 0)
		{
			DbgUI.Begin("Universal Temp Sources", 10, 300);
			DbgUI.Text(string.Format("Lookup radius: %1m (server-side)", PluginUniversalTemperatureSourceServer.LOOKUP_RADIUS));
			DbgUI.Text(string.Format("Count: %1", m_UTemperatureSourceDebugs.Count()));
			DbgUI.Text(string.Format("Avg. temp: %1 °C", m_UTSAverageTemperature));
			DbgUI.End();
		}
	}
	
	protected void ProcessUniversalTemperatureSources()
	{
		if (m_UTemperatureSourceDebugs.Count() == 0)
		{
			m_UTSAverageTemperature = 0;

			return;
		}

		array<float> utsTemperatures = new array<float>();
		
		// get temperature from the source (based on distance), save it for min/max filtering
		for (int i = 0; i < m_UTemperatureSourceDebugs.Count(); i++)
		{
			UniversalTemperatureSourceDebug utsd = m_UTemperatureSourceDebugs[i];
			//! next temp source is too far (not cleaned on server, like in Environment, client need to know about all sources in MAXRANGE)
			if (vector.DistanceSq(m_Player.GetPosition(), utsd.GetValue(0).ToVector()) > Math.SqrFloat(utsd.GetValue(2).ToFloat()))
			{
				continue;
			}

			utsTemperatures.Insert(CalcTemperatureFromTemperatureSource(m_UTemperatureSourceDebugs[i]));
		}

		float min = MiscGameplayFunctions.GetMinValue(utsTemperatures);
		float max = MiscGameplayFunctions.GetMaxValue(utsTemperatures);
		
		if (max > 0 && min < 0)
		{
			m_UTSAverageTemperature = (max + min) * 0.5;
		}
		else
		{
			m_UTSAverageTemperature = max;
		}
		
	}
	
	protected float CalcTemperatureFromTemperatureSource(notnull UTemperatureSourceDebug utsd)
	{
		float distance 		= vector.Distance(m_Player.GetPosition(), utsd.GetValue(0).ToVector());
		distance 			= Math.Max(distance, 0.1);	//min distance cannot be 0 (division by zero)
		float temperature	= 0;

		//! heat transfer through air to player (env temperature)
		if (distance > utsd.GetValue(1).ToFloat())
		{
			float distFactor = 1 - (distance / utsd.GetValue(2).ToFloat());
			distFactor = Math.Max(distFactor, 0.0); //! dist factor minimum should be at 0
			temperature = utsd.GetValue(3).ToFloat() * distFactor;
		}
		else
		{
			temperature = utsd.GetValue(3).ToFloat();
		}
		
		//Print(temperature);

		return temperature;
	}
	
	void EnableWidgets(bool enable)
	{
		for (int i = 0; i < MAX_SIMULTANEOUS_UTS; i++)
		{
			m_RootWidget[i].Show(enable);
		}
	}
	
	void UpdateStatWidgets()
	{
		int i = 0;
		for (; i < m_UTemperatureSourceDebugs.Count(); i++)
		{
			UTemperatureSourceDebug utsd = m_UTemperatureSourceDebugs.Get(i);
			vector pos = utsd.GetValue(0).ToVector();
			vector screen_pos_stats = GetGame().GetScreenPos(pos + "0 0 0");
			vector screen_pos_damage = GetGame().GetScreenPos(pos + "0 2 0");
			m_RootWidget[i].SetPos(screen_pos_stats[0],screen_pos_stats[1]);
			
			if (screen_pos_stats[2] > 0 && screen_pos_stats[0] > 0 && screen_pos_stats[1] > 0)
			{
				m_RootWidget[i].Show(true);
				UpdateStatWidget(i, utsd);
			}
			else
			{
				m_RootWidget[i].Show(false);
			}
		}

		for (; i < MAX_SIMULTANEOUS_UTS; i++)
		{
			if (m_RootWidget[i])
			{
				m_RootWidget[i].Show(false);
			}
		}
	}
	
	void UpdateStatWidget(int rowIndex, UTemperatureSourceDebug utsd)
	{
		m_StatListWidgets[rowIndex].ClearItems();
		
		for (int i = 0; i < utsd.PairsCount(); i++)
		{
			m_StatListWidgets[rowIndex].AddItem(utsd.GetName(i), null, 0, i);
			m_StatListWidgets[rowIndex].SetItem(i, utsd.GetValue(i), null, 1);
		}
		
		int numPairs = utsd.PairsCount();
		// manually add value for distance (client only)
		m_StatListWidgets[rowIndex].AddItem("distance", null, 0, numPairs);
		m_StatListWidgets[rowIndex].SetItem(numPairs, vector.Distance(m_Player.GetPosition(), utsd.GetValue(0).ToVector()).ToString(), null, 1);
	}

	void RequestUniversalTemperatureSources(PlayerBase player, int enable)
	{
		//Debug.Log("RequestUniversalTemperatureSources called", "PluginUniversalTemperatureSourceClient");

		if (!enable)
		{
			m_UTemperatureSourceDebugs.Clear();
			m_Player = null;
			EnableWidgets(false);
			return;
		}

		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(enable);
		rpc.Send(player, ERPCs.DEV_REQUEST_UTS_DEBUG, true, player.GetIdentity());

		m_Player = player;
	}
	
	void PrintedDebug()
	{
		for (int i = 0; i < m_UTemperatureSourceDebugs.Count(); i++)
		{
			UTemperatureSourceDebug utsd = m_UTemperatureSourceDebugs.Get(i);
			PrintString("-------------------------------------");
			utsd.Debug();
			PrintString("-------------------------------------");
		}
	}
	
	void OnRPC(ParamsReadContext ctx)
	{
		//Debug.Log("OnRPC called", "PluginUniversalTemperatureSourceClient");
		//PrintedDebug();
		ctx.Read(m_UTemperatureSourceDebugs);
	}
}