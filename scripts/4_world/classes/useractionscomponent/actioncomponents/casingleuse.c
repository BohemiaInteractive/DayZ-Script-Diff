class CASingleUse : CASingleUseBase
{	
	override int Execute( ActionData action_data )
	{
		if ( !action_data.m_Player )
		{
			return UA_ERROR;
		}

		return UA_FINISHED;
	}
};