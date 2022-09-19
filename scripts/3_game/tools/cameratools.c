#ifdef DEVELOPER
class CameraToolsMenuServer
{
	ref array<Man> m_Subscribers = new array<Man>;
	void OnRPC(int rpc_type, ParamsReadContext ctx)
	{
		if (rpc_type == ERPCs.DEV_CAMERA_TOOLS_CAM_DATA)
		{
			Param4<vector, vector,float,float> p4 = new Param4<vector, vector,float,float>(vector.Zero, vector.Zero,0,0);
			if (ctx.Read(p4))
			{
				foreach (int index, Man p:m_Subscribers)
				{
					if (p)
					{
						GetGame().RPCSingleParam(p, ERPCs.DEV_CAMERA_TOOLS_CAM_DATA, p4, true, p.GetIdentity());
					}
					else
					{
						m_Subscribers.Remove(index);
					}
				}
			}
		}
		else if (rpc_type == ERPCs.DEV_CAMERA_TOOLS_CAM_SUBSCRIBE)
		{
			Param2<bool, Man> par2 = new Param2<bool, Man>(false,null);

			if (ctx.Read(par2))
			{
				bool enable = par2.param1;
				Man player = par2.param2;
				
				bool found = false;
				foreach (int i, Man m:m_Subscribers)
				{
					if (m == player)
					{
						if (!enable)
						{
							m_Subscribers.Remove(i);
							return;
						}
						found = true;
						m_Subscribers[i] = player;
					}
				}
				if (!found && enable)//not found in the array, insert it
				{
					m_Subscribers.Insert(player);
				}
			}
		}
	}
};

class CameraToolsMenuClient
{
	Shape m_DebugShape = null;
	
	void ~CameraToolsMenuClient()
	{
		if (m_DebugShape)
		{
			m_DebugShape.Destroy();
		}
	}
	
	void DelayedDestroy()
	{
		GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( DestroyNow, 2000);
	}
	
	void DestroyNow()
	{
		delete this;
	}
	
	void OnRPC(ParamsReadContext ctx)
	{
		if (m_DebugShape)
		{
			m_DebugShape.Destroy();
			m_DebugShape = null;
		}
		
		Param4<vector, vector,float,float> p4 = new Param4<vector, vector,float,float>(vector.Zero, vector.Zero,0,0);
		if ( ctx.Read( p4 ) )
		{
			vector pos = p4.param1;
			vector dir = p4.param2;
			float plane = p4.param3;
			float fov = p4.param4 * Math.RAD2DEG;
			
			
			m_DebugShape = Debug.DrawFrustum(fov, 30, 0.5);
			vector mat[4];
			Math3D.DirectionAndUpMatrix(dir,vector.Up,mat);
			mat[3] = pos;
			m_DebugShape.SetMatrix(mat);

		}
	}
};
#endif