//! The object which is in a trigger and its metadata
class TriggerInsider
{
	ref OLinkT insider; // DEPRECATED
	
	//! Object that data belongs to
	protected Object m_Object;
	
	//! Last time the object was seen in ms
	int timeStamp;
	
	//! Time the object was first seen in seconds
	float timeEntered;
	
	//! Last time the object was updated in seconds, is used for calculating deltaTime
	float lastUpdated;
	
	void TriggerInsider(Object obj)
	{
		insider = new OLinkT(obj);
		m_Object = obj;
	}
	
	void ~TriggerInsider()
    {
    }

	Object GetObject()
	{
		return m_Object;
	}
};

#ifdef DEVELOPER
typedef Param7<vector, vector, vector, vector, float, string, array<ref TriggerInsider>> DebugTriggerInfo;
#endif

//! Scripted Trigger
class Trigger : TriggerEvents
{
	//! DEPRECATED
	const int TIMEOUT = 1000;
	//! The objects and their metadata which are currently inside the Trigger
	ref array<ref TriggerInsider> m_insiders;
	
	#ifdef DEVELOPER
	bool m_Local;//is this trigger spawning on client only ?
	string 						m_DebugAreaType;
	ref array<ref TriggerInsider> m_dbgInsiders;
	#endif
	
	//! ctor
	private void Trigger()
	{
		SetEventMask(EntityEvent.INIT /*| EntityEvent.TOUCH*/ | EntityEvent.FRAME | EntityEvent.ENTER | EntityEvent.LEAVE );
		SetFlags(EntityFlags.TRIGGER, false);
		
		m_insiders = new array<ref TriggerInsider>;
	}
	
	//! dtor
	private void ~Trigger()
	{
		#ifdef DEVELOPER
		CleanupDebugShapes(dbgTargets);
		#endif
	}

	/** \name IEntity events
 		Usage of IEntity events
	*/
	//@{
	//! Set the default extents of the Trigger only once it is properly initialized
	override void EOnInit(IEntity other, int extra)
	{
		SetExtents("-2 -4 -2", "2 4 2");
	}
	
	//! When an Object touches the Trigger, we want to register it being inside the Trigger -> Replaced by EOnEnter
	/*override void EOnTouch(IEntity other, int extra)
	{
		Object obj;
		if (Class.CastTo(obj, other) && CanAddObjectAsInsider(obj))
			AddInsider(obj);
	}*/
	
	//! We want to update the insiders every frame (or do we?)
	override void EOnFrame(IEntity other, float timeSlice)
	{
		UpdateInsiders(TIMEOUT);
	}
	
	//! When an Object enters the trigger add it to Insiders
	override void EOnEnter(IEntity other, int extra)
	{
		Object obj;
		if (Class.CastTo(obj, other) && CanAddObjectAsInsider(obj))
			AddInsider(obj);
	}
	
	//! When an Object exits the trigger remove it from Insiders
	override void EOnLeave(IEntity other, int extra)
	{
		Object obj;
		if (Class.CastTo(obj, other))
			RemoveInsiderByObject(obj);
	}
	//@}
	
	
	/** \name Trigger configuration
 		Basic configuration and data API
	*/
	//@{
	//! Set the size of the Trigger, avoid using SetCollisionBox directly
	void SetExtents(vector mins, vector maxs)
	{
		SetCollisionBox(mins, maxs);
	}
	
	//! Get the radius of the CollisionBox, simply left for backwards compatibility
	float GetRadius(vector min, vector max)
	{
		return GetCollisionRadius();			
	}
	
	//! Get the current TriggerInsider array, left for backwards compatibility, moved down from ManTrigger
	array<ref TriggerInsider> GetInsiders()
	{
		return m_insiders;
	}
	
	//! Gets the TriggerInsider for the Object if it exists
	TriggerInsider GetInsiderForObject(Object object)
	{
		TriggerInsider ins;
		
		for ( int n = 0; n < m_insiders.Count(); ++n )
		{
			ins = m_insiders[n];
			if (ins.GetObject() == object)
				return ins;
		}
		
		return null;
	}
	
	//! Gets the index in m_insiders for the Object
	int GetInsiderIndexForObject(Object object)
	{
		TriggerInsider ins;
		
		for ( int n = 0; n < m_insiders.Count(); ++n )
		{
			ins = m_insiders[n];
			if (ins.GetObject() == object)
				return n;
		}
		
		return -1;
	}
	//@}
	
	
	/** \name TriggerEvents, backwards compatibility calling
 		Implementation for backwards compatibility with old Trigger system
	*/
	//@{
	override protected void OnEnterBeginEvent(TriggerInsider insider)
	{
		// Call the old event for backwards compatibility
		OnEnter(insider.GetObject());
	}
	
	override protected void OnLeaveBeginEvent(TriggerInsider insider)
	{
		// Call the old event for backwards compatibility
		OnLeave(insider.GetObject());
	}	
	//@}

	
	/** \name DEPRECATED Events
 		left for backwards compatibility
	*/
	//@{
	void OnEnter(Object obj) {}
	
	void OnLeave(Object obj) {}
	//@}
	
	
	/** \name TriggerInsider conditions
 		Conditions applied to TriggerInsider for if it can be added or should be removed
	*/
	//@{
	//! Condition whether an Object can be added as TriggerInsider (checked before calling AddInsider)
	protected bool CanAddObjectAsInsider(Object object)
	{
		return true;
	}
	
	//! Condition whether a TriggerInsider should still be updated or not (checked in update loop and before adding)
	protected bool ShouldRemoveInsider(TriggerInsider insider)
	{
		return false;
	}
	
	//! Condition whether a TriggerInsider should still be updated or not, skips OnLeaveEvent (checked in update loop and before adding)
	protected bool ShouldRemoveInsiderNoLeave(TriggerInsider insider)
	{
		return false;
	}
	//@}
	
	
	/** \name TriggerInsider processing
 		Logic and processing of adding, removing and updating a TriggerInsider
	*/
	//@{
	//! Used for easily overriding TriggerInsider creation without rewriting AddInsider
	protected TriggerInsider CreateInsider(Object obj)
	{
		return new TriggerInsider(obj);
	}
	
	//! Adding of new TriggerInsider
	protected void AddInsider(Object obj)
	{
		if ( !obj )
			return;	

		// Already in?
		if ( GetInsiderForObject( obj ) )
		{
			Error(string.Format("[WARNING] :: [Trigger] :: [%1] :: Insider (%2) is already inside.", GetDebugName(this), GetDebugName(obj)));
			return;
		}
			
		// New Object entered! Fill the data.
		TriggerInsider insider = CreateInsider(obj);
		insider.timeStamp = g_Game.GetTime();
		insider.timeEntered = g_Game.GetTickTime();
		insider.lastUpdated = insider.timeEntered;
		
		// Don't add if it is going to be removed anyways..
		if ( ShouldRemoveInsider(insider) || ShouldRemoveInsiderNoLeave(insider) )
			return;
		
		// Keep track of the Object as long as it is inside the Trigger
		int index = m_insiders.Insert(insider);
		
		// Call the enter event to signal this Object entered
		Enter(insider);
		obj.OnEnterTrigger(this);
		
		#ifdef TRIGGER_DEBUG_NORMAL
		Debug.TriggerLog(string.Format("%1: inserted at index %2", GetDebugName(obj), index), "Trigger", "", "AddInsider", GetDebugName(this));
		#endif
						
		//!DEBUG
		#ifdef DEVELOPER
		DebugSendDmgTrigger();
		#endif
	}
	
	//! Removing of TriggerInsider
	protected void RemoveInsider(TriggerInsider insider, int index = -1)
	{
		Leave(insider);
		insider.GetObject().OnLeaveTrigger(this);
		
		#ifdef TRIGGER_DEBUG_NORMAL
		Debug.TriggerLog(string.Format("%1: removing at index %2", GetDebugName(insider.GetObject()), index), "Trigger", "", "RemoveInsider", GetDebugName(this));
		#endif	
		
		if (index >= 0)
			m_insiders.Remove(index);
		else
			m_insiders.RemoveItemUnOrdered(insider);
		
		//!DEBUG
		#ifdef DEVELOPER
		DebugSendDmgTrigger();
		#endif
	}
	
	//! Removing of TriggerInsider through Object
	protected void RemoveInsiderByObject(Object object)
	{
		TriggerInsider ins;
		for ( int n = 0; n < m_insiders.Count(); ++n )
		{
			ins = m_insiders[n];
			if (ins.GetObject() == object)
			{
				RemoveInsider(ins, n);
				return;
			}
		}
		
		// As EOnLeave can call this, it is perfectly valid that this Object is not found on Script side
		// because of "ShouldRemoveInsider" and "ShouldRemoveInsiderNoLeave"
	}
	
	//! Update the current TriggerInsider inside the Trigger, timeout paramter is deprecated
	protected void UpdateInsiders(int timeout)
	{
		//!DEBUG
		#ifdef DEVELOPER
		DebugSendDmgTrigger();
		#endif
		
		// Don't do anything if there aren't any insiders
		if ( m_insiders.Count() == 0 )
			return;
		
		// Mark the beginning of the update loop
		StayStart(m_insiders.Count());
		
		// Iterate over the current insiders, backwards because we are deleting
		for ( int n = m_insiders.Count() - 1; n >= 0 ; --n)
		{
			TriggerInsider insider = m_insiders.Get(n);
			Object obj = insider.GetObject();
			
			// Check if the Object still exists or should be removed without calling OnLeaveEvent
			if ( !obj || ShouldRemoveInsiderNoLeave(insider) )
			{
				#ifdef TRIGGER_DEBUG_BASIC
				Debug.TriggerLog(string.Format("%1: removed with no Leave.", GetDebugName(obj)), "Trigger", "", "UpdateInsiders", GetDebugName(this));
				#endif
				
				m_insiders.Remove(n);
				continue;
			}
			
			// Check if Object left the Trigger or should be removed regardless
			if ( ShouldRemoveInsider(insider) )
			{
				RemoveInsider(insider, n);
				continue;
			}
			
			// Call the OnStayEvent, Object is still inside the Trigger and can be updated
			// Pass in the time since the Object was last updated (or entered)
			float currentTime = g_Game.GetTickTime();
			Stay(insider, currentTime - insider.lastUpdated);
			insider.lastUpdated = currentTime;
		}
		
		// Mark the end of the update loop
		StayFinish();
	}
	//@}
	
	/** \name DEBUGGING
		General internal debugging functionality
	*/
	//@{
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{	
		super.OnRPC(sender, rpc_type, ctx);
		#ifdef DEVELOPER
		switch ( rpc_type )
		{
			case ERPCs.RPC_AREADAMAGE_DEBUGAREA:
				DebugTriggerInfo data = new DebugTriggerInfo(vector.Zero, vector.Zero, vector.Zero, vector.Zero, 0, "", null);
			
				if ( ctx.Read( data ) )
					DebugDmgTrigger( data.param1, data.param2, data.param3, data.param4, data.param5, data.param6, data.param7 );
			break;
		}
		#endif
	}
	
#ifdef DEVELOPER
	void DebugSendDmgTrigger()
	{		
		vector minmax[2];
		GetCollisionBox(minmax);
		
		DebugTriggerInfo data = new DebugTriggerInfo(vector.Zero, vector.Zero, vector.Zero, vector.Zero, 0, "", null);
		data.param1 = GetWorldPosition();
		data.param2 = GetOrientation();		
		data.param3 = minmax[0];
		data.param4 = minmax[1];
		data.param5 = GetCollisionRadius();		
		data.param6 = m_DebugAreaType;
		data.param7 = m_insiders;
		
		if ( GetGame().IsMultiplayer() && GetGame().IsServer() )
			PluginDiagMenu.SendDataToSubscribersServer(this, ESubscriberSystems.TRIGGERS, ERPCs.RPC_AREADAMAGE_DEBUGAREA,data,false);
			//GetGame().RPCSingleParam(this, ERPCs.RPC_AREADAMAGE_DEBUGAREA, data, true);
		else if (!GetGame().IsMultiplayer() || m_Local)
			DebugDmgTrigger( data.param1, data.param2, data.param3, data.param4, data.param5, data.param6, data.param7 );
	}
	
	protected ref array<Shape> dbgTargets = new array<Shape>();
	
	void DebugDmgTrigger( vector pos, vector orientation, vector min, vector max, float radius, string dmgType, array<ref TriggerInsider> insiders)
	{
		CleanupDebugShapes( dbgTargets );
		
		bool enableDebug = DiagMenu.GetBool(DiagMenuIDs.DM_SHOW_AREADMG_TRIGGER);
		if ( enableDebug )
		{
			if ( GetGame().IsMultiplayer() && GetGame().IsServer() )
			{
				return;
			}
			
			vector w_pos, w_pos_sphr, w_pos_lend;
			
			w_pos = pos;
			// sphere pos tweaks
			w_pos_sphr = w_pos;
			// line pos tweaks
			w_pos_lend = w_pos;
			
			//Find way to change colour of box depending on ammoType in a more elegant fashion
			m_DebugAreaType = dmgType;
			Shape dbgShape;			
			
			switch ( m_DebugAreaType )
			{
				case "FireDamage":
					dbgShape = DrawDebugShape(pos, min, max, radius, COLOR_RED_A);
				break;
				
				case "BarbedWireHit":
					dbgShape = DrawDebugShape(pos, min, max, radius, COLOR_BLUE_A);
				break;
				
				default:
					dbgShape = DrawDebugShape(pos, min, max, radius, COLOR_GREEN_A);
				break;
			}
			
			if ( GetGame().IsMultiplayer() || GetGame().IsServer() )
				m_dbgInsiders = insiders;
		
			if ( m_dbgInsiders.Count() > 0 )
			{
				//Change colour to make state clearer
				dbgShape.SetColor( COLOR_YELLOW_A );
				
				for ( int i = 0; i < m_dbgInsiders.Count(); i++ )
				{
					EntityAI insider_EAI = EntityAI.Cast( m_dbgInsiders[i].GetObject() );
					if ( insider_EAI )
					{
						vector insiderPos = insider_EAI.GetWorldPosition() + "0 0.1 0";						
						dbgTargets.Insert( Debug.DrawArrow( w_pos, insiderPos ) );
					}
				}
			}
		}
	}
	
	protected Shape DrawDebugShape(vector pos, vector min, vector max, float radius, int color)
	{
		Shape dbgShape;

		switch (GetTriggerShape())
		{
		case TriggerShape.BOX:
			dbgShape = Debug.DrawBox(min, max, color);

			vector mat[4];
			GetTransform(mat);
			dbgShape.CreateMatrix(mat);
			dbgShape.SetMatrix(mat);
		break;
		case TriggerShape.CYLINDER:
			dbgShape = Debug.DrawCylinder(pos, radius, max[1], color, ShapeFlags.TRANSP|ShapeFlags.NOZWRITE);
		break;
		case TriggerShape.SPHERE:
			dbgShape = Debug.DrawSphere(pos, radius, color, ShapeFlags.TRANSP|ShapeFlags.NOZWRITE);
		break;
		default:
			ErrorEx("TriggerShape not found", ErrorExSeverity.WARNING);
		break;
		}

		dbgTargets.Insert(dbgShape);
	
		return dbgShape;
	}
	
	protected void CleanupDebugShapes(array<Shape> shapes)
	{
		for (int it = 0; it < shapes.Count(); ++it)
		{
			Debug.RemoveShape(shapes[it]);
		}

		shapes.Clear();
	}
	
#endif
	//@}
};