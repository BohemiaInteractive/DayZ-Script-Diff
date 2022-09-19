class PluginUniversalTemperatureSourceServer extends PluginBase
{
	const int INTERVAL			= 1;		//! [s]
	const float LOOKUP_RADIUS	= 20;		//! [m]
		
	protected float m_AccuTime;
	protected ref set<PlayerBase> m_ClientList;

	protected ref array<ref UTemperatureSourceDebug> m_UTemperatureSourceDebugs;

	void PluginUniversalTemperatureSourceServer()
	{
		m_ClientList 				= new set<PlayerBase>();
		m_UTemperatureSourceDebugs	= new array<ref UTemperatureSourceDebug>();
	}
	
	override void OnUpdate(float delta_time)
	{
		if (m_ClientList.Count() > 0)
		{
			m_AccuTime += delta_time;
			
			if (m_AccuTime > INTERVAL)
			{
				m_AccuTime = 0;
				SendDebug();
			}
		}
	}
	
	void GatherTemperatureSources(PlayerBase player)
	{
		m_UTemperatureSourceDebugs.Clear();
		
		if (!player)
		{
			return;
		}

		vector playerPos				= player.GetPosition();
		array<Object> nearestObjects	= new array<Object>;

		GetGame().GetObjectsAtPosition(playerPos, LOOKUP_RADIUS, nearestObjects, null);

		UTemperatureSource uts;
		UTemperatureSourceDebug utsd;

		for (int i = 0; i < nearestObjects.Count(); i++)
		{
			EntityAI ent = EntityAI.Cast(nearestObjects.Get(i));
			if (ent && ent.IsUniversalTemperatureSource() && ent != player)
			{
				uts = ent.GetUniversalTemperatureSource();
				utsd = new UTemperatureSourceDebug();
				utsd.Add("position", uts.GetPosition().ToString(false));
				utsd.Add("fullrange", uts.GetFullRange().ToString());
				utsd.Add("maxrange", uts.GetMaxRange().ToString());
				utsd.Add("temperature", uts.GetTemperature().ToString());
				utsd.Add("temperatureraw", uts.GetTemperatureRaw().ToString());
				utsd.Commit();

				m_UTemperatureSourceDebugs.Insert(utsd);
			}
		}

		if (player.GetItemInHands() && player.GetItemInHands().IsUniversalTemperatureSource())
		{
			uts = player.GetItemInHands().GetUniversalTemperatureSource();
			utsd = new UTemperatureSourceDebug();
			utsd.Add("position", uts.GetPosition().ToString(false));
			utsd.Add("fullrange", uts.GetFullRange().ToString());
			utsd.Add("maxrange", uts.GetMaxRange().ToString());
			utsd.Add("temperature", uts.GetTemperature().ToString());
			utsd.Add("temperatureraw", uts.GetTemperatureRaw().ToString());
			utsd.Commit();
	
			m_UTemperatureSourceDebugs.Insert(utsd);
		}
	}

	void SendDebug()
	{
		for (int i = 0; i < m_ClientList.Count(); i++)
		{
			PlayerBase player = m_ClientList.Get(i);
			if (player)
			{
				
				GatherTemperatureSources(player);
				ScriptRPC rpc = new ScriptRPC();
				rpc.Write(m_UTemperatureSourceDebugs);
				rpc.Send(player, ERPCs.DEV_UTS_DEBUG_DATA, true, player.GetIdentity());
			}
			else
			{
				m_ClientList.Remove(i);
			}
		}
	}

	void OnRequestReceived(PlayerBase player, bool enable)
	{
		int index = m_ClientList.Find(player);
		
		if (enable)
		{
			m_ClientList.Insert(player);
		}
		else if (index != -1 && m_ClientList.Count() > 0)
		{
			m_ClientList.Remove(index);
		}
	}
	
	void OnRPC(ParamsReadContext ctx, PlayerBase player)
	{
		bool enable;
		ctx.Read(enable);

		OnRequestReceived(player, enable);
	}
}