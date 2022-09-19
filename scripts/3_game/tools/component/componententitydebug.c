class ComponentEntityDebug extends Component
{
	
	protected Shape				m_DebugShapeBBox;
	protected Shape				m_DebugShapeDirection;
	protected float				m_DebugShapeDirectionDist;

	// -------------------------------------------------------------------------
	override Shape DebugBBoxDraw()
	{
		#ifndef DEVELOPER
		return null;
		#endif
		
		if ( m_DebugShapeBBox )
			m_DebugShapeBBox.Destroy();
				
		vector min_max[2];
		
		if (!m_ThisEntityAI.GetCollisionBox(min_max))
			return null;
		
		m_DebugShapeBBox = Debug.DrawBox(min_max[0], min_max[1]);
		
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(OnDrawing, 0, true);
		
		m_ThisEntityAI.SetEventMask(EntityEvent.FRAME);

		return null;
	}

	// -------------------------------------------------------------------------
	override void DebugBBoxSetColor(int color)
	{
		if ( m_DebugShapeBBox )
		{
			m_DebugShapeBBox.SetColor(color);
		}
	}

	// -------------------------------------------------------------------------
	override void DebugBBoxDelete()
	{
		#ifndef DEVELOPER
		return;
		#endif
		
		if ( m_DebugShapeBBox )
		{
			m_DebugShapeBBox.Destroy();
			m_DebugShapeBBox = null;
		}
		
		if ( !m_DebugShapeDirection && !m_DebugShapeBBox )
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(OnDrawing);
		}
	}

	// -------------------------------------------------------------------------
	override Shape DebugDirectionDraw(float distance = 1)
	{	
		#ifndef DEVELOPER
		return null;
		#endif
		
		if ( m_DebugShapeDirection )
		{
			m_DebugShapeDirection.Destroy();
		}
		
		vector p1 = "0 0 0";
		vector p2 = m_ThisEntityAI.GetDirection() * m_DebugShapeDirectionDist;
		m_DebugShapeDirectionDist = distance;
		m_DebugShapeDirection = Debug.DrawArrow(p1, p2);
		
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(OnDrawing, 0, true);
		
		m_ThisEntityAI.SetEventMask(EntityEvent.FRAME);
		return null;
	}

	// -------------------------------------------------------------------------
	override void DebugDirectionDelete()
	{
		#ifndef DEVELOPER
		return;
		#endif
		
		if ( m_DebugShapeDirection )
		{
			m_DebugShapeDirection.Destroy();
			m_DebugShapeDirection = null;
		}
		
		if ( !m_DebugShapeDirection && !m_DebugShapeBBox )
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(OnDrawing);
		}
	}

	void OnDrawing()
	{
		#ifndef DEVELOPER
		return;
		#endif
		
		if ( m_DebugShapeBBox || m_DebugShapeDirection )
		{		
			vector mat[4];
			m_ThisEntityAI.GetTransform(mat);
		
			if ( m_DebugShapeBBox )
			{
				m_DebugShapeBBox.SetMatrix(mat);
			}
			
			if ( m_DebugShapeDirection )
			{
				m_DebugShapeDirection.SetMatrix(mat);			
			}
		}
	}
}