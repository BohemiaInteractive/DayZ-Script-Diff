class BunkerBroadcastComponentBase : Managed
{
	protected bool 						m_IsActive;
	protected string					m_LockCode;
	protected vector					m_Position;
	protected int						m_Id;

	[NonSerialized()]
	protected EntityAI		m_Parent;
	[NonSerialized()]
	protected BunkerBroadcastHandler 	m_Handler;	
	[NonSerialized()]
	protected ref ScriptInvoker m_OnActivated;
	[NonSerialized()]
	protected ref ScriptInvoker m_OnDeactivated;
	
	void BunkerBroadcastComponentBase(notnull EntityAI parent)
	{
		m_Parent = parent;
	}
	
	void Init()
	{
		if (g_Game.IsServer())
		{
			m_OnActivated 	= new ScriptInvoker();
			m_OnDeactivated = new ScriptInvoker();

			m_Position = m_Parent.GetWorldPosition();
			
			//! id is persistent until the parent of component is moved in world
			m_Id = m_Position.ToString().Hash();

			m_Handler = BunkerBroadcastHandler.GetInstance();
			if (m_Handler)
				m_Handler.Register(this);
		}		
	}
	
	// --------------------------------------------------------------------------------
	
	ScriptInvoker GetOnActivated()
	{
		return m_OnActivated;
	}

	// --------------------------------------------------------------------------------
	
	ScriptInvoker GetOnDeactivated()
	{
		return m_OnDeactivated;
	}

	// --------------------------------------------------------------------------------
	
	void OnUpdate(float deltaTime)
	{
	}

	// --------------------------------------------------------------------------------

	int GetId()
	{
		return m_Id;
	}
	
	// --------------------------------------------------------------------------------
	
	vector GetPosition()
	{
		return m_Position;
	}

	// --------------------------------------------------------------------------------
	
	void OnActivate(string code, bool loadedAsActive = false)
	{
		m_LockCode = code;
		SetActive(true);
		m_OnActivated.Invoke(loadedAsActive, code);
	}

	// --------------------------------------------------------------------------------
	
	void OnDeactivate()
	{
		m_LockCode = "-";
		SetActive(false);
		m_OnDeactivated.Invoke();
	}

	// --------------------------------------------------------------------------------
	
	string GetLockCode()
	{
		return m_LockCode;
	}
	
	// --------------------------------------------------------------------------------

	bool IsActive()
	{
		return m_IsActive;
	}

	// --------------------------------------------------------------------------------
	
	protected void SetActive(bool state)
	{
		m_IsActive = state;

		#ifdef SERVER
		if (m_Parent)
			m_Parent.SetSynchDirty();
		#endif
	}
	
	// --------------------------------------------------------------------------------
}

#ifdef DIAG_DEVELOPER
class BunkerBroadcastComponentDebugData : Param
{
	bool m_IsActive;
	string m_LockCode;
	
	void Synch(notnull BunkerBroadcastComponentBase component)
	{
		m_IsActive = component.IsActive();
		m_LockCode = component.GetLockCode();
	}
	
	// --------------------------------------------------------------------------------
	
	override bool Serialize(Serializer ctx)
	{
		return ctx.Write(m_IsActive) && ctx.Write(m_LockCode);
	}
	
	// --------------------------------------------------------------------------------
		
	override bool Deserializer(Serializer ctx)
	{
		return ctx.Read(m_IsActive) && ctx.Read(m_LockCode);
	}
}
#endif

class BunkerBroadcastComponent : BunkerBroadcastComponentBase
{
	#ifdef DIAG_DEVELOPER
	protected ref array<Shape> m_DebugShapes = new array<Shape>();
	protected ref array<ref DebugTextWorldSpace> m_DebugTextsWS;
	#endif

	void BunkerBroadcastComponent(notnull EntityAI parent)
	{
		#ifdef DIAG_DEVELOPER
		m_DebugTextsWS = new array<ref DebugTextWorldSpace>();
		#endif
	}
	
	// --------------------------------------------------------------------------------
	
	override void OnUpdate(float deltaTime)
	{
		super.OnUpdate(deltaTime);

		#ifdef DIAG_DEVELOPER
		if (g_Game.IsServer())
		{
			BunkerBroadcastComponentDebugData data = null;
			if (m_IsActive)
				data = GetDebugData();
			GetGame().RPCSingleParam(m_Parent, ERPCs.DIAG_MISC_RADIOGAMEPLAY_DEBUG_DATA, data, true);
		}
		#endif
	}

	// --------------------------------------------------------------------------------
	
	void OnRPC(PlayerIdentity sender, int rpcType, ParamsReadContext ctx)
	{
		switch (rpcType)
		{
			#ifdef DIAG_DEVELOPER
			case ERPCs.DIAG_MISC_RADIOGAMEPLAY_DEBUG_DATA:
				BunkerBroadcastComponentDebugData data = new BunkerBroadcastComponentDebugData();
				ctx.Read(data);
				if (!DiagMenu.GetBool(DiagMenuIDs.RADIOGAMEPLAY_DEBUG) || (g_Game.GetUIManager().GetMenu() && g_Game.GetUIManager().GetMenu().GetID() >= 0))
				{
					Debug.CleanupTextsWS(m_DebugTextsWS);
					Debug.CleanupDrawShapes(m_DebugShapes);
					break;
				}
					
				ShowDebugInfo(DiagMenu.GetBool(DiagMenuIDs.RADIOGAMEPLAY_DEBUG), data);
				UpdateDebugDraws(DiagMenu.GetBool(DiagMenuIDs.RADIOGAMEPLAY_DEBUG), data);
				break;
			#endif
		}
	}
	
	// --------------------------------------------------------------------------------

	#ifdef DIAG_DEVELOPER	
	BunkerBroadcastComponentDebugData GetDebugData()
	{
		BunkerBroadcastComponentDebugData data = new BunkerBroadcastComponentDebugData();
		data.Synch(this);

		return data;
	}
	
	// --------------------------------------------------------------------------------
	
	void ShowDebugInfo(bool enabled, BunkerBroadcastComponentDebugData data)
	{
		Debug.CleanupTextsWS(m_DebugTextsWS);
		DrawDebugText(enabled, data);
	}
	
	// --------------------------------------------------------------------------------

	void UpdateDebugDraws(bool enabled, BunkerBroadcastComponentDebugData data)
	{

		Debug.CleanupDrawShapes(m_DebugShapes);
		DrawActivityDebugMarker(enabled, data);
	}
	
	// --------------------------------------------------------------------------------

	private void DrawActivityDebugMarker(bool enabled, BunkerBroadcastComponentDebugData data)
	{
		if (data && enabled)
		{
			int color = COLOR_RED;
			if (data.m_IsActive)
				color = COLOR_GREEN;
			
			vector position = GetPosition();
			position[1] = position[1] + 10.0;
	
			m_DebugShapes.Insert(Shape.CreateSphere(color, ShapeFlags.NOZWRITE|ShapeFlags.NOOUTLINE, position, 0.5));
		}
	}
	
	// --------------------------------------------------------------------------------
	
	private void DrawDebugText(bool enabled, BunkerBroadcastComponentDebugData data)
	{
		if (data && enabled)
		{
			string text = "";
			string lockCode = "unset";
			int color = 0xffff0000;
			if (data.m_IsActive)
			{
				lockCode = data.m_LockCode;
				color = 0xff00ff00;
			}

			text += string.Format("Bunker\n");
			text += string.Format("Code:     %1\n", lockCode);

			BunkerBroadcastProxy proxy = BunkerBroadcastProxy.Cast(m_Parent);
			Bunker bunker = proxy.GetBunker();
			if (bunker)
			{
				vector position = bunker.GetSelectionBasePositionLS("att_codelock");
				position = bunker.ModelToWorld(position);
				position[1] = position[1] + 0.2;
				m_DebugTextsWS.Insert(DebugTextWorldSpace.Create(text, DebugTextFlags.ONCE, position[0], position[1], position[2], 14, color, 0x00000000));
			}
		}
	}
	#endif
	
	// --------------------------------------------------------------------------------
}
