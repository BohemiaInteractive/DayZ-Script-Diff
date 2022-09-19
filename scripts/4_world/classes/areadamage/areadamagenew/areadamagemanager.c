class AreaDamageComponentTypes
{
	const int BASE = 0;
	const int HITZONE = 1;
	const int RAYCASTED = 2;
}

class AreaDamageManager : AreaDamageEvents
{
	protected EntityAI						m_ParentObject;
	protected AreaDamageTriggerBase			m_AreaDamageTrigger;	
	
	protected ref AreaDamageComponent		m_DamageComponent;
	
	protected vector						m_ExtentMin;
	protected vector 						m_ExtentMax;
	protected vector 						m_AreaPosition;
	protected vector 						m_AreaOrientation;
	
	protected string						m_TriggerBaseClass;
	
	void AreaDamageManager(EntityAI parent)
	{
		m_ParentObject		= parent;
		
		m_DamageComponent	= new AreaDamageComponent(this);
		
		m_ExtentMin 		= vector.Zero;
		m_ExtentMax 		= vector.Zero;
		m_AreaPosition		= parent.GetPosition();
		m_AreaOrientation	= parent.GetOrientation();
		
		m_TriggerBaseClass 	= "AreaDamageTriggerBase";
	}
	
	void ~AreaDamageManager()
	{
		Destroy();
	}
	
	void SetDamageComponentType(int type)
	{
		switch (type)
		{
			case AreaDamageComponentTypes.BASE:
			{
				m_DamageComponent = new AreaDamageComponent(this);
				break;
			}
			case AreaDamageComponentTypes.HITZONE:
			{
				m_DamageComponent = new AreaDamageComponentRandomHitzone(this);
				break;
			}
			case AreaDamageComponentTypes.RAYCASTED:
			{
				m_DamageComponent = new AreaDamageComponentRaycasted(this);
				break;
			}
			
			default:
			{
				Error(string.Format("[ERROR] :: [AreaDamageManager] :: Unknown DamageComponentType: %1", type));
				break;
			}
		}
	}

	//! spawn damage trigger
	void Spawn()
	{
		if ( CreateDamageTriggerEx() )
			OnTriggerCreated();
	}
	
	//! destroy damage trigger
	void Destroy()
	{
		if ( DestroyDamageTriggerEx() )
			OnTriggerDestroyed();
	}

	/////////////////////////////////////////////////////////////
	/** @name AreaDamageEvents Implementation
 	*  Implementation for handling \ref AreaDamageEvents
	*  Pass them on to the AreaDamageComponent
	*  Calling of the events that are the same in TriggerEvents will come from AreaDamageTriggerBase
	*/
	///@{
	//! Called when a trigger is created
	override void OnTriggerCreated()
	{
		m_DamageComponent.OnTriggerCreated();
	}
	
	//! Called when a trigger is destroyed
	override void OnTriggerDestroyed()
	{
		m_DamageComponent.OnTriggerDestroyed();
	}
	
	//! Called when an object enters the trigger
	override void OnEnterBeginEvent(TriggerInsider insider)
	{
		m_DamageComponent.OnEnterBeginEvent(insider);
	}
	override void OnEnterServerEvent(TriggerInsider insider)
	{
		m_DamageComponent.OnEnterServerEvent(insider);
	}
	override void OnEnterClientEvent(TriggerInsider insider)
	{
		m_DamageComponent.OnEnterClientEvent(insider);
	}
	override void OnEnterEndEvent(TriggerInsider insider)
	{
		m_DamageComponent.OnEnterEndEvent(insider);
	}
	
	//! Called at the beginning of an update loop
	override void OnStayStartBeginEvent(int nrOfInsiders)
	{
		m_DamageComponent.OnStayStartBeginEvent(nrOfInsiders);
	}
	override void OnStayStartServerEvent(int nrOfInsiders)
	{
		m_DamageComponent.OnStayStartServerEvent(nrOfInsiders);
	}
	override void OnStayStartClientEvent(int nrOfInsiders)
	{
		m_DamageComponent.OnStayStartClientEvent(nrOfInsiders);
	}
	override void OnStayStartEndEvent(int nrOfInsiders)
	{
		m_DamageComponent.OnStayStartEndEvent(nrOfInsiders);
	}
	
	//! Called in the update loop
	override void OnStayBeginEvent(TriggerInsider insider, float deltaTime)
	{
		m_DamageComponent.OnStayBeginEvent(insider, deltaTime);
	}
	override void OnStayServerEvent(TriggerInsider insider, float deltaTime)
	{
		m_DamageComponent.OnStayServerEvent(insider, deltaTime);
	}
	override void OnStayClientEvent(TriggerInsider insider, float deltaTime)
	{
		m_DamageComponent.OnStayClientEvent(insider, deltaTime);
	}
	override void OnStayEndEvent(TriggerInsider insider, float deltaTime)
	{
		m_DamageComponent.OnStayEndEvent(insider, deltaTime);
	}
	
	//! Called at the end of an update loop
	override void OnStayFinishBeginEvent()
	{
		m_DamageComponent.OnStayFinishBeginEvent();
	}
	override void OnStayFinishServerEvent()
	{
		m_DamageComponent.OnStayFinishServerEvent();
	}
	override void OnStayFinishClientEvent()
	{
		m_DamageComponent.OnStayFinishClientEvent();
	}
	override void OnStayFinishEndEvent()
	{
		m_DamageComponent.OnStayFinishEndEvent();
	}
	
	//! Called when an object leaves the trigger
	override void OnLeaveBeginEvent(TriggerInsider insider)
	{
		m_DamageComponent.OnLeaveBeginEvent(insider);
	}
	override void OnLeaveServerEvent(TriggerInsider insider)
	{
		m_DamageComponent.OnLeaveServerEvent(insider);
	}
	override void OnLeaveClientEvent(TriggerInsider insider)
	{
		m_DamageComponent.OnLeaveClientEvent(insider);
	}
	override void OnLeaveEndEvent(TriggerInsider insider)
	{
		m_DamageComponent.OnLeaveEndEvent(insider);
	}
	
	//! Tell the AreaDamageComponent to calculate and apply the damage to the Object
	override bool OnEvaluateDamage(TriggerInsider insider, float damageCoef)
	{
		return m_DamageComponent.OnEvaluateDamage(insider, damageCoef);
	}

	//! Usually called from AreaDamageComponent
	override void PreDamageActions()
	{
		if ( m_ParentObject )
			m_ParentObject.PreAreaDamageActions();
	}

	//! Usually called from AreaDamageComponent
	override void PostDamageActions()
	{
		if ( m_ParentObject )
			m_ParentObject.PostAreaDamageActions();
	}
	///@}
	/////////////////////////////////////////////////////////////

	
	//! Some event extensions
	///@{
	//! Alternative OnEvaluateDamage, using CalculateDamageScale
	bool OnEvaluateDamageEx(TriggerInsider insider, float deltaTime)
	{
		return OnEvaluateDamage( insider, CalculateDamageScale( insider, deltaTime ) );
	}
	
	//! Helper for calculating damageCoeff when using OnEvaluateDamageEx
	protected float CalculateDamageScale(TriggerInsider insider, float deltaTime)
	{
		return deltaTime;
	}
	///@}
	
	
	//! script configuration
	///@{
	void SetExtents( vector mins, vector maxs )
	{
		m_ExtentMin = mins;
		m_ExtentMax = maxs;
		
		if ( m_AreaDamageTrigger )
		{
			m_AreaDamageTrigger.SetExtents(m_ExtentMin, m_ExtentMax);
		}
	}	
	void GetExtents( out vector mins, out vector maxs )
	{
		mins = m_ExtentMin;
		maxs = m_ExtentMax;
	}	
	void GetWorldExtents( out vector mins, out vector maxs )
	{
		GetExtents(mins, maxs);
		
		mins + m_AreaPosition;
		maxs + m_AreaPosition;
	}

	void SetAmmoName( string ammo_name )
	{
		m_DamageComponent.m_AmmoName = ammo_name;
	}
	string GetAmmoName()
	{
		return m_DamageComponent.m_AmmoName;
	}
	
	void SetDamageType( int pDamageType )
	{
		m_DamageComponent.m_DamageType = pDamageType;
	}
	int GetDamageType()
	{
		return m_DamageComponent.m_DamageType;
	}
	
    void SetParentObject( EntityAI obj )
	{
		m_ParentObject = obj;
	}
	EntityAI GetParentObject()
	{
		return m_ParentObject;
	}
	
    void SetAreaPosition( vector position )
	{
		m_AreaPosition = position;
		
		if ( m_AreaDamageTrigger )
		{
			m_AreaDamageTrigger.SetPosition(m_AreaPosition);
		}
	}
	vector GetPosition()
	{
		return m_AreaPosition;
	}
	
    void SetAreaOrientation( vector orientation )
	{
		m_AreaOrientation = orientation;
		
		if ( m_AreaDamageTrigger )
		{
			m_AreaDamageTrigger.SetOrientation(m_AreaOrientation);
		}
	}	
	vector GetOrientation()
	{
		return m_AreaOrientation;
	}
	
	void SetDamageableTypes(array<typename> damageableTypes)
	{
		m_DamageComponent.m_DamageableTypes = damageableTypes;
	}
	array<typename> GetDamageableTypes()
	{
		return m_DamageComponent.m_DamageableTypes;
	}
	
	void AddDamageableType(typename damageableType)
	{
		if (m_DamageComponent.m_DamageableTypes.Find(damageableType) != -1)
			m_DamageComponent.m_DamageableTypes.Insert(damageableType);
	}
	
	void RemoveAddDamageableType(typename damageableType)
	{
		m_DamageComponent.m_DamageableTypes.RemoveItem(damageableType);
	}
	
	void SetLoopInterval( float time )
	{
		Error("SetLoopInterval not implemented - usable in Regular(looped) area damage objects only");
	}
	
	void SetDeferDuration( float time )
	{
		Error("SetDeferDuration not implemented - usable in Deferred area damage objects only");
	}

	void SetHitZones( array<string> hitzones )
	{
		AreaDamageComponentRandomHitzone adc;
		if ( CastTo( adc, m_DamageComponent ) )
			adc.SetHitZones( hitzones );
		else
			Error("SetHitZones not implemented - override for non raycasted versions of area damage objects only");
	}

	void SetRaycastSources( array<string> raycast_sources )
	{
		AreaDamageComponentRaycasted adc;
		if ( CastTo( adc, m_DamageComponent ) )
			adc.SetRaycastSources( raycast_sources );
		else
			Error("SetRaycastSources not implemented - override for raycasted versions of area damage objects only");
	}
	
	void SetRaycastSourcesVector( array<vector> raycast_sources )
	{
		AreaDamageComponentRaycasted adc;
		if ( CastTo( adc, m_DamageComponent ) )
			adc.SetRaycastSourcesVector( raycast_sources );
		else
			Error("SetRaycastSources not implemented - override for raycasted versions of area damage objects only");
	}
	
	AreaDamageTriggerBase GetTrigger()
	{
		return m_AreaDamageTrigger;
	}
	///@}
	

	//! create damage trigger with basic set of params
	///@{
	protected bool CreateDamageTriggerEx()
	{
		int flags;

		if ( GetGame().IsMultiplayer() && GetGame().IsServer() )
			flags = ECE_CREATEPHYSICS;
		else
			flags = ECE_LOCAL;

		if (Class.CastTo(m_AreaDamageTrigger, GetGame().CreateObjectEx( m_TriggerBaseClass, m_AreaPosition, flags )))
		{
			m_AreaDamageTrigger.SetOrientation( m_AreaOrientation );
			m_AreaDamageTrigger.SetExtents( m_ExtentMin, m_ExtentMax );
			m_AreaDamageTrigger.SetAreaDamageManager(this);
			return true;
		}
		
		return false;
	}
	
	protected void CreateDamageTrigger()
	{
		CreateDamageTriggerEx();
	}
	///@}
	
	
	//! destroys damage trigger
	///@{
	protected bool DestroyDamageTriggerEx()
	{
		if ( GetGame() && m_AreaDamageTrigger ) // It's necesarry to check if the game exists. Otherwise a crash occurs while quitting.
		{
			GetGame().ObjectDelete( m_AreaDamageTrigger );
			m_AreaDamageTrigger = null;
			return true;
		}
		
		return false;
	}
	
	protected void DestroyDamageTrigger()
	{
		DestroyDamageTriggerEx();
	}
	///@}
}