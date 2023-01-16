// For modding, see PluginDiagMenuModding.c
// !!! MODDING DISCLAIMER: These are debug functionality files, if you are thinking about modding the vanilla ones, do so at your own risk
//                         These files will not be maintained with the thought of "what if a modder modded this" (Excluding the modding functionality of course)
//                         Which is why the modding functionality was developed with the thought of the modded ones having their own isolated safe space

class PluginDiagMenuServer : PluginDiagMenu
{
#ifdef DIAG_DEVELOPER
	static ref map<Man, int> m_Subscribers = new map<Man, int>;
	
	// A bit of a hack, because SP creates both Client and Server Plugins
	override private void RegisterDiags()
	{
		if (GetGame().IsMultiplayer())
		{
			super.RegisterDiags();
		}
	}
	
	//---------------------------------------------
	override void OnRPC(PlayerBase player, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(player, rpc_type, ctx);
		
		switch (rpc_type)
		{			
			case ERPCs.DEV_DIAGMENU_SUBSCRIBE:
			{
				if (ctx.Read(CachedObjectsParams.PARAM1_INT))
				{
					int newMask = CachedObjectsParams.PARAM1_INT.param1;
					int currentMask = m_Subscribers.Get(player);
				
					if (newMask != currentMask)
					{
						if (newMask == 0)
						{
							m_Subscribers.Remove(player);
						}
						else
						{
							m_Subscribers.Set(player, newMask);
						}
					}
				}
				break;
			}
			case ERPCs.DIAG_VEHICLE_DEBUG_OUTPUT:
			{
				if (ctx.Read(CachedObjectsParams.PARAM1_INT))
					CarScript.DEBUG_OUTPUT_TYPE = CachedObjectsParams.PARAM1_INT.param1;
				break;
			}
			
			case ERPCs.DIAG_VEHICLES_DUMP_CRASH_DATA_REQUEST:
			{
				if (ctx.Read(CachedObjectsParams.PARAM1_BOOL))
					CrashDebugData.SendData(player);
				break;
			}	
		}
	}
	
	//---------------------------------------------
	static void SendDataToSubscribersServer(Object target, ESubscriberSystems system, int rpc_type, Param data, bool guaranteed = true)
	{
		for (int i = 0; i < m_Subscribers.Count(); ++i)
		{
			Man man = m_Subscribers.GetKey(i);
			if (man)
			{
				int subscribedSystems = m_Subscribers.Get(man);
				if (system & subscribedSystems)
				{
					GetGame().RPCSingleParam( target, rpc_type, data, guaranteed, man.GetIdentity() );
				}
			}
			else
			{
				m_Subscribers.RemoveElement(i);
				i--;
			}
		}
	}
#endif
}