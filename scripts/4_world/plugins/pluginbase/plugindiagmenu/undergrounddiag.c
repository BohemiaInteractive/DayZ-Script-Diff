#ifdef DEVELOPER
#ifndef SERVER
//----------------------------------------------------------------------------------------
//! 
class UndergroundDiagData
{
}

class BoxDrawGizmo : Managed
{
	vector m_Rotation[3];
	private vector m_Position;
	private vector m_Transform[4];
	
	ref ScriptInvoker m_OnRotationChanged = new ScriptInvoker();
	
	void BoxDrawGizmo(vector pPosition)
	{
		Math3D.MatrixIdentity4(m_Transform);
		Math3D.MatrixIdentity3(m_Rotation);

		SetPosition(pPosition);
	}

	void SetPosition(vector position)
	{
		m_Position = position;
		m_Transform[3] = position;
		
		SetTransform(m_Transform);
	}
	
	vector GetPosition()
	{
		return m_Position;
	}

	void SetTransform(vector transform[4])
	{
		m_Transform = transform;
		
		for (int i = 0; i < 3; ++i)
			m_Rotation[i] = transform[i];
		
		m_Position = transform[3];
	}
	
	void GetTransform(inout vector transform[4])
	{
		transform = m_Transform;
		//Print(m_Transform);
		//Print(transform);
		/*
		Math3D.MatrixIdentity4(transform);
		Gizmo_GetWorldTransform(transform);
		*/
	}

	bool Gizmo_IsSupported()
	{
		return true;
	}
	
	void Gizmo_SetWorldTransform(vector transform[4], bool finalize)
	{
		m_OnRotationChanged.Invoke(m_Transform);
		
		//Print(transform);

		//m_Rotation[0] = transform[0];
		//m_Rotation[1] = transform[1];
		//m_Rotation[2] = transform[2];
		//m_Position = transform[3];

		/*
		Math3D.MatrixIdentity4(m_Transform);
		m_Transform[0] = m_Rotation[0];
		m_Transform[1] = m_Rotation[1];
		m_Transform[2] = m_Rotation[2];
		m_Transform[3] = m_Position;
		*/
		
		SetTransform(transform);
		
		//for (int i = 0; i < 4; ++i)
		//	m_Transform[i] = transform[i];
	}

	void Gizmo_GetWorldTransform(vector transform[4])
	{
		/*
		Math3D.MatrixIdentity4(transform);
		m_Transform[0] = m_Rotation[0];
		m_Transform[1] = m_Rotation[1];
		m_Transform[2] = m_Rotation[2];
		m_Transform[3] = m_Position;
		*/
		
		transform = m_Transform;
		//for(int i = 0; i < 4; ++i)
		//	Print("Gizmo_GetWorldTransform: " + transform[i]);
		
		/*
		//Print(m_Transform);
		
		for (int i = 0; i < 4; ++i)
			transform[i] = m_Transform[i];
		*/
	}
}

class BoxDraw
{
	private PlayerBase m_Player;
	private ref BoxDrawGizmo m_AA, m_BB, m_Global;
	
	
	//! TODO move to math3d
	vector TransformPointByMatrix(vector transform[4], vector localPoint)
	{
	    return transform[0] * localPoint[0] + transform[1] * localPoint[1] + transform[2] * localPoint[2] + transform[3];
	}

	vector InverseTransformPointByMatrix(vector transform[4], vector pointWorld)
	{
	    // Move to origin's position
	    vector relative = pointWorld - transform[3];
	
	    // Project onto basis vectors (assuming basis is orthonormal!)
	    float localX = vector.Dot(relative, transform[0].Normalized());
	    float localY = vector.Dot(relative, transform[1].Normalized());
	    float localZ = vector.Dot(relative, transform[2].Normalized());
	
	    return Vector(localX, localY, localZ);
	}
	//! ---------------

	void BoxDraw()
	{
		if (!GetGizmoApi())
		{
			ErrorEx("Gizmo API not created, crash imminent", ErrorExSeverity.ERROR);
		}

		m_Player = PlayerBase.Cast(g_Game.GetPlayer());
		vector position = m_Player.GetPosition();
		
		m_AA = new BoxDrawGizmo(position);
		m_BB = new BoxDrawGizmo(position + "1 1 1");
		m_Global = new BoxDrawGizmo(Math3D.BoxCenter(m_AA.GetPosition(), m_BB.GetPosition()));
		
		GetGizmoApi().SelectUser(m_AA);
		GetGizmoApi().SelectUser(m_BB);
		GetGizmoApi().SelectUser(m_Global);
		
		m_Global.m_OnRotationChanged.Insert(OnGlobalRotationChanged);
	}
	
	void ~BoxDraw()
	{
		m_Global.m_OnRotationChanged.Remove(OnGlobalRotationChanged);

		GetGizmoApi().DeselectTracker(m_AA);
		GetGizmoApi().DeselectTracker(m_BB);
		GetGizmoApi().DeselectTracker(m_Global);
	}
	
	void Update()
	{
		/*
		m_AA.m_Rotation[0] = m_Global.m_Transform[0];
		m_AA.m_Rotation[1] = m_Global.m_Transform[1];
		m_AA.m_Rotation[2] = m_Global.m_Transform[2];
		m_BB.m_Rotation[0] = m_Global.m_Transform[0];
		m_BB.m_Rotation[1] = m_Global.m_Transform[1];
		m_BB.m_Rotation[2] = m_Global.m_Transform[2];
		*/
		
		//m_Global.m_Position = Math3D.BoxCenter(m_AA.m_Position, m_BB.m_Position);
		
		vector boxSize = Math3D.BoxSize(m_AA.GetPosition(), m_BB.GetPosition());
		
		vector globalTransform[4];
		Math3D.MatrixIdentity4(globalTransform);
		m_Global.GetTransform(globalTransform);
		//Print(globalTransform);
		DrawRotatedBox(globalTransform, boxSize, 0xFFFF0000);

		//Print(m_AA.m_Transform[3]);
		//vector aa = m_AA.m_Transform[3];
		//vector bb = m_BB.m_Transform[3];
		//Print(m_AA.GetPosition());
	}
	
	private void OnGlobalRotationChanged(vector transform[4])
	{
		//m_Global.m_Transform = transform
		//Print(transform);
		//Print(m_AA.m_Transform);
		//m_AA.m_Transform[3] = TransformPointByMatrix(m_Global.m_Transform, InverseTransformPointByMatrix(m_Global.m_Transform, m_AA.GetPosition()));
		//m_AA.Gizmo_SetWorldTransform(m_AA.m_Transform, true);
		//m_BB.m_Position = TransformPointByMatrix(m_Global.m_Transform, InverseTransformPointByMatrix(m_Global.m_Transform, m_BB.GetPosition()));
	}

	private void DrawRotatedBox(vector transform[4], vector size, int color)
	{
	    vector localCorners[8] = {
	        Vector(-0.5, -0.5, -0.5),
	        Vector( 0.5, -0.5, -0.5),
	        Vector( 0.5,  0.5, -0.5),
	        Vector(-0.5,  0.5, -0.5),
	        Vector(-0.5, -0.5,  0.5),
	        Vector( 0.5, -0.5,  0.5),
	        Vector( 0.5,  0.5,  0.5),
	        Vector(-0.5,  0.5,  0.5),
	    };
	
	    vector worldCorners[8];
	    for (int i = 0; i < 8; ++i)
	    {
	        vector scaled = Vector(
	            localCorners[i][0] * size[0],
	            localCorners[i][1] * size[1],
	            localCorners[i][2] * size[2],
	        );
	        worldCorners[i] = TransformPointByMatrix(transform, scaled);
	    }
	
	    int edgePairs[] = {
	        0,1,  1,2,  2,3,  3,0,
	        4,5,  5,6,  6,7,  7,4,
	        0,4,  1,5,  2,6,  3,7
	    };
	
	    for (int j = 0; j < 24; j += 2)
	    {
	        Debug.DrawLine(worldCorners[edgePairs[j]], worldCorners[edgePairs[j + 1]], color, ShapeFlags.ONCE);
	    }
	}
}


//----------------------------------------------------------------------------------------
//!
class UndergroundDiag
{
	private ref UndergroundDiagData m_Data = new UndergroundDiagData();
	private ref BoxDraw m_BoxDraw;
	
	private const int COLOR_START        = 0xFF2ECC71; // Green
	private const int COLOR_END          = 0xFF4B77BE; // Blue
	private const int COLOR_END_NOATTACH = 0xFFF7CA18; // Yellow
	private const int COLOR_OBSTRUCTION  = 0xFFFF00FF; // Pink
	private const int COLOR_HIT          = 0xFFFF0000; // Red
	private const int COLOR_GUIDE        = 0xC03A3A3A; // Semi-transparent gray
	private const int COLOR_WEAPON       = 0x1FFFFFFF; // Semi-transparent white
	private const int COLOR_LAST_POS     = 0x3AF7CA18; // Semi-transparent yellow
	
	private bool m_EditModeEnabled    = true;

	//----------------------------------------------------------------------------------------
	UndergroundDiagData Data()
	{
		return m_Data;
	}
	
	//----------------------------------------------------------------------------------------
	void DrawDiag(float deltaTime)
	{
		UndergroundDiagData data = m_Data;
		if (data)
		{
			/// TITLE
			DbgUI.Begin("UndergroundDiag Diag");
			{
				/// WEAPON PROPERTIES
				int gray            = 0x80FFFFFF; // semi transparent white (gray)
				int darkObst        = gray&COLOR_OBSTRUCTION;
				int endNoAttachCol  = gray & COLOR_END_NOATTACH;

				DbgUI.Spacer(10);
				DbgUI.Check("  Edit Mode", m_EditModeEnabled);
			}
			DbgUI.End();
			
			HandleGizmos();
		}
	}
	
	private void HandleGizmos()
	{
		if (m_EditModeEnabled)
		{
			if (!m_BoxDraw)
				m_BoxDraw = new BoxDraw();
			
			m_BoxDraw.Update();
		}
		else
		{
			if (m_BoxDraw)
				delete m_BoxDraw;
		}
	}
}
#endif
#endif