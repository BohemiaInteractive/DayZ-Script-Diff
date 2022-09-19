//! A particular version of the deferred loop used to not damage players inside vehicles 
class AreaDamageLoopedDeferred_NoVehicle: AreaDamageLoopedDeferred
{
	override bool OnEvaluateDamageEx( TriggerInsider insider, float deltaTime )
	{
		// For case where player is inside vehicle 
		if ( insider.GetObject().IsMan() )
		{
			PlayerBase playerInsider = PlayerBase.Cast( insider.GetObject() );
			if ( playerInsider.GetCommand_Vehicle() )
				return false;
		}
		
		return super.OnEvaluateDamageEx( insider, deltaTime );
	}
}