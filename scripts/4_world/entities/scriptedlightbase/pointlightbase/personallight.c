class PersonalLight extends PointLightBase
{	
	void PersonalLight()
	{
		SetVisibleDuringDaylight(false);
		SetRadiusTo( 3 );
		SetBrightnessTo(0.38);
		SetDiffuseColor(0.2, 0.23, 0.25);
		SetCastShadow(false);
		EnableSpecular(false);
		EnableLinear(true);
		SetFlareVisible(false);
	}
	
	override void OnFrameLightSource(IEntity other, float timeSlice)
	{
		if ( GetGame()  &&  IsEnabled() )
		{
			vector pos = GetGame().GetCurrentCameraPosition();
			pos += Vector( 0, -0.4, 0 );
			
			SetPosition( pos );
		}
	}
	
	// Experiment with dynamic range of Personal Light based on distance between camera and player's character.
	/*override void OnFrameLightSource(IEntity other, float timeSlice)
	{
		if ( GetGame()  &&  IsEnabled() )
		{
			vector pos = GetGame().GetCurrentCameraPosition();
			vector dir = GetGame().GetCurrentCameraDirection();
			
			SetPosition(pos);
			
			DayZPlayer plr = GetGame().GetPlayer();
			if (plr)
			{
				vector pos_plr = plr.GetPosition() + Vector(0, 1.64, 0);
				float distance = vector.Distance( pos_plr, pos );
				//Print(distance);
				float radius = 20.0 + distance;
				SetRadiusTo( radius );
			}
		}
	}*/
}