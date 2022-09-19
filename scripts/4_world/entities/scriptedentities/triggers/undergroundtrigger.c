//! This entity exists both client andserver side
//! when it enters into player's bubble and gets instantiated client-side, it will locally spawn UndergroundTrigger, which is the actual trigger
class UndergroundTriggerCarrier extends UndergroundTriggerCarrierBase
{
	ref JsonUndergroundAreaTriggerData m_Data;
	UndergroundTrigger m_Trigger;
	
	void UndergroundTriggerCarrier()
	{
		RegisterNetSyncVariableInt("m_TriggerIndex", -1, 255);
	}
	
	void ~UndergroundTriggerCarrier()
	{
		if (m_Trigger && !m_Trigger.IsSetForDeletion() && GetGame())
		{
			//RemoveChild(m_Trigger);
			m_Trigger.Delete();
		}
	}
	
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		if (!m_Trigger)
		{
			SpawnTrigger();
		}
	}
		
	bool CanSpawnTrigger()
	{
		return UndergroundAreaLoader.m_JsonData && m_TriggerIndex != -1;
	}
	
	void RequestDelayedTriggerSpawn()
	{
		//Print("RequestDelayedTriggerSpawn() " + this);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SpawnTrigger, 100);
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
			UndergroundTrigger trigger = UndergroundTrigger.Cast(GetGame().CreateObjectEx( "UndergroundTrigger", GetPosition(), ECE_LOCAL ));
			if (trigger)
			{
				#ifdef DEVELOPER
				trigger.m_Local = true;
				#endif
				trigger.SetPosition(vector.Zero);
				if (GetOrientation() != vector.Zero)
					trigger.SetOrientation(GetOrientation());
				AddChild(trigger,-1);
				trigger.Init(data);
				
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
			if (m_Accommodation == 1)
			{
				m_Type = EUndergroundTriggerType.OUTER;
			}
			else
			{
				m_Type = EUndergroundTriggerType.INNER;
			}
		}
	}

	override protected bool CanAddObjectAsInsider(Object object)
	{
		if (!super.CanAddObjectAsInsider(object))
		{
			return false;
		}
		
		PlayerBase player = PlayerBase.Cast(object);
		return player.IsControlledPlayer();
	}
	
	
	override void EOnInit(IEntity other, int extra)
	{
		//disable parent behaviour
	}
	override protected void OnEnterClientEvent(TriggerInsider insider) 
	{
		//Print("OnEnterClientEvent " + this);
		//return;
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
		//Print("OnLeaveClientEvent " + this);
		//return;
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
};