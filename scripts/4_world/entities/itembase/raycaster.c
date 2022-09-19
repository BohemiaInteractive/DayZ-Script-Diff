// Item for raycast debugging. It's also fun to play around with at night.

class Raycaster extends ItemBase
{
	protected ref Timer 	m_Timer;
	
	override void OnWorkStart()
	{
		if ( !GetGame().IsServer()  ||  !GetGame().IsMultiplayer() ) // Client side
		{
			StartPeriodicMeasurement();
		}
	}
	
	void StartPeriodicMeasurement()
	{
		if( !m_Timer )
		{
			m_Timer = new Timer;
		}
		
		m_Timer.Run( 0.01, this, "PrepareMeasurement", null, true );
	}
	
	void PrepareMeasurement()
	{
		DoMeasurement();
		DoMeasurement();
	}
	
	void DoMeasurement()
	{
		float dispersion = 80;
		
		vector 		from 			= GetPosition() + ( GetMemoryPointPos("beamStart") + ((GetOrientation()+Vector(90,0,0)).AnglesToVector()*0.3) );
		vector 		ori 			= GetOrientation() + Vector(90,0,0) + Vector(dispersion/2 - Math.RandomFloat(0, dispersion)  ,  dispersion/2 - Math.RandomFloat(0, dispersion)  ,  dispersion/2 - Math.RandomFloat(0, dispersion)  );
		vector 		to 				= from + (ori.AnglesToVector() )* 30;
		vector 		contact_pos;
		vector 		contact_dir;
		int 		contactComponent;
		
		bool is_collision = DayZPhysics.RaycastRV( from, to, contact_pos, contact_dir, contactComponent, NULL , NULL, GetHierarchyRootPlayer(), false, false, ObjIntersectIFire);
		
		if (is_collision)
		{
			vector hit_normal = contact_dir.VectorToAngles();
			hit_normal[1] = hit_normal[1] + 90;
			
			// Generate result
			Particle p = ParticleManager.GetInstance().PlayInWorld(ParticleList.DEBUG_DOT, contact_pos);
			p.SetOrientation(hit_normal);
		}
	}
	
	void StopPeriodicMeasurement()
	{
		if( m_Timer )
		{
			m_Timer.Stop();
		}
	}

	override void OnWorkStop()
	{
		if ( !GetGame().IsServer()  ||  !GetGame().IsMultiplayer() ) // Client side
		{
			StopPeriodicMeasurement();
		}
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionTurnOnWhileInHands);
		AddAction(ActionTurnOffWhileInHands);
	}
}