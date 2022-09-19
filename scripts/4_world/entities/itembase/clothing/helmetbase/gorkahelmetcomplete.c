class GorkaHelmet extends HelmetBase
{
	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{
		EntityAI entity;
		if ( Class.CastTo(entity, this) )
		{
			entity.GetInventory().CreateInInventory( "GorkaHelmetVisor" );
		}
	}
}