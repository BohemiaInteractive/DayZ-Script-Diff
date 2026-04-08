//! This entity exists both client and server side
//! when it enters into player's bubble and gets instantiated client-side, it will locally spawn UndergroundTrigger, which is the actual trigger
class UndergroundTriggerCarrier : UndergroundTriggerCarrierBase
{
	ref JsonUndergroundAreaTriggerData m_Data;
	UndergroundTrigger m_Trigger;
	
	void UndergroundTriggerCarrier()
	{
		RegisterNetSyncVariableInt("m_TriggerIndex", -1, 255);
		RegisterNetSyncVariableInt("m_ParentObjectNetIdLow");
		RegisterNetSyncVariableInt("m_ParentObjectNetIdHigh");
	}
	
	void ~UndergroundTriggerCarrier()
	{
		if (m_Trigger && !m_Trigger.IsSetForDeletion() && g_Game)
		{
			m_Trigger.Delete();
		}
	}
	
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();

		if (g_Game.IsMultiplayer())
			m_ParentObject = g_Game.GetObjectByNetworkId(m_ParentObjectNetIdLow, m_ParentObjectNetIdHigh);
		
		if (!m_Trigger)
			SpawnTrigger();
	}
		
	bool CanSpawnTrigger()
	{
		return UndergroundAreaLoader.m_JsonData && m_TriggerIndex != -1;
	}
	
	void RequestDelayedTriggerSpawn()
	{
		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SpawnTrigger, 100);
	}
	
	void SpawnTrigger()
	{
		if (!CanSpawnTrigger())
		{
			RequestDelayedTriggerSpawn();
			return;
		}
		
		if (UndergroundAreaLoader.m_JsonData.Triggers && UndergroundAreaLoader.m_JsonData.Triggers.IsValidIndex(m_TriggerIndex))
		{
			JsonUndergroundAreaTriggerData data = UndergroundAreaLoader.m_JsonData.Triggers[m_TriggerIndex];
			UndergroundTrigger trigger = UndergroundTrigger.Cast(g_Game.CreateObjectEx("UndergroundTrigger", GetPosition(), ECE_LOCAL));
			if (trigger)
			{
				#ifdef DIAG_DEVELOPER
				trigger.m_Local = true;
				#endif
				trigger.SetPosition(vector.Zero);
				AddChild(trigger, -1);
				trigger.Init(data);
				trigger.SetTriggerParentObject(m_ParentObject);
				
				m_Trigger = trigger;
			}
		}
	}
}

enum EUndergroundTriggerType
{
	UNDEFINED,
	TRANSITIONING,
	OUTER,
	INNER
}

class UndergroundTrigger : ManTrigger
{
	ref JsonUndergroundAreaTriggerData m_Data;
	
	EUndergroundTriggerType m_Type;
	float m_Accommodation;
	float m_InterpolationSpeed;
	
	protected Object m_ParentObject;
	
	void Init(JsonUndergroundAreaTriggerData data)
	{
		m_Data = data;
		//Print("Spawning trigger " + this + "at pos: " + GetPosition());
		vector extMax = data.GetSize() * 0.5;
		vector extMin = -extMax;
		SetExtents(extMin, extMax);
		m_Accommodation = data.EyeAccommodation;
		m_InterpolationSpeed = data.InterpolationSpeed;

		if (data.Breadcrumbs && data.Breadcrumbs.Count() > 0) //TODO: simpler check
		{
			if (data.Breadcrumbs.Count() > 32)
			{
				ErrorEx("max 'Breadcrumb' count is 32, found: " + data.Breadcrumbs.Count());
			}
			m_Type = EUndergroundTriggerType.TRANSITIONING;
		}
		else
		{
			if (m_Accommodation == 1.0)
			{
				m_Type = EUndergroundTriggerType.OUTER;
			}
			else
			{
				m_Type = EUndergroundTriggerType.INNER;
			}
		}
	}
	
	//! sets parent "virtual" parent object that trigger is registered to, eg. building
	void SetTriggerParentObject(Object parent)
	{
		m_ParentObject = parent;
	}
	
	Object GetTriggerParentObject()
	{
		return m_ParentObject;
	}

	override protected bool CanAddObjectAsInsider(Object object)
	{
		if (!super.CanAddObjectAsInsider(object))
		{
			return false;
		}
		
		return ManBase.Cast(object).IsControlledPlayer();
	}
	
	
	override void EOnInit(IEntity other, int extra)
	{
		//disable parent behaviour
	}
	
	#ifdef DEVELOPER
	override protected void OnEnterServerEvent(TriggerInsider insider) 
	{
		#ifndef SERVER//to make it work in single during development
		OnEnterClientEvent(insider);
		#endif
	}
	override protected void OnLeaveServerEvent(TriggerInsider insider) 
	{
		#ifndef SERVER//to make it work in single during development
		OnLeaveClientEvent(insider);
		#endif
	}
	#endif
	
	override protected void OnEnterClientEvent(TriggerInsider insider) 
	{
		PlayerBase player = PlayerBase.Cast(insider.GetObject());
		if (player)
		{
			UndergroundHandlerClient handler = player.GetUndergroundHandler();
			if (handler)
			{
				handler.OnTriggerEnter(this);
			}
		}
	}
	
	override protected void OnLeaveClientEvent(TriggerInsider insider) 
	{
		PlayerBase player = PlayerBase.Cast(insider.GetObject());
		if (player)
		{
			UndergroundHandlerClient handler = player.GetUndergroundHandler();
			if (handler)
			{
				handler.OnTriggerLeave(this);
			}
		}
	}

};