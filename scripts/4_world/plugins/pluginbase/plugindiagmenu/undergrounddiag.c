#ifdef DIAG_DEVELOPER
//----------------------------------------------------------------------------------------

class UndergroundTriggerHandle : ItemBase
{
	BoxDraw m_Reference;
	ref JsonUndergroundAreaTriggerData m_Data;
	
	void UndergroundTriggerHandle()
	{
		m_Data = new JsonUndergroundAreaTriggerData();
	}
	
	string GetHandleName()
	{
		return m_Data.Comment;
	}
	
	void SetHandleName(string name)
	{
		m_Data.Comment = name;
	}

	void SetBoxDrawRef(BoxDraw boxReference)
	{
		m_Reference = boxReference;
	}
}

class UndergroundBreadcrumbHandle : ItemBase
{
	UndergroundTriggerHandle m_ParentTrigger;
	BreadcrumbDraw m_Reference;
	ref JsonUndergroundAreaBreadcrumb m_Data;
	
	void UndergroundBreadcrumbHandle()
	{
		m_Data = new JsonUndergroundAreaBreadcrumb();
	}
	
	string GetHandleName()
	{
		return m_Data.Comment;
	}
	
	void SetHandleName(string name)
	{
		m_Data.Comment = name;
	}

	void SetBreadcrumbDrawRef(BreadcrumbDraw breadcrumbReference)
	{
		m_Reference = breadcrumbReference;
	}
}

class DrawGizmoBase : Managed
{
	protected vector m_Rotation[3];
	protected vector m_Transform[4];
	
	bool m_TransformAllowed = false;
	
	ref ScriptInvoker m_OnTransformChanged = new ScriptInvoker();
	
	void DrawGizmoBase(vector pPosition)
	{
		Math3D.MatrixIdentity4(m_Transform);
		Math3D.MatrixIdentity3(m_Rotation);

		SetPosition(pPosition);
	}
	
	bool Gizmo_IsAllowedTransformMode(GizmoTransformMode mode)
	{
		return m_TransformAllowed;
	}

	bool Gizmo_IsAllowedSpaceMode(GizmoSpaceMode mode)
	{
		return true;
	}

	void SetPosition(vector position)
	{
		m_Transform[3] = position;
		
		SetTransform(m_Transform);
	}
	
	vector GetPosition()
	{
		return m_Transform[3];
	}

	void SetTransform(vector transform[4])
	{
		copyarray(m_Transform, transform);
	}
	
	void GetTransform(inout vector transform[4])
	{
		copyarray(transform, m_Transform);
	}

	bool Gizmo_IsSupported()
	{
		return true;
	}
	
	void Gizmo_SetWorldTransform(vector transform[4], bool finalize)
	{
		m_OnTransformChanged.Invoke(transform);

		SetTransform(transform);
	}

	void Gizmo_GetWorldTransform(vector transform[4])
	{
		copyarray(transform, m_Transform);
	}
	
	protected void IsolateYawOnly(inout vector transform[4])
	{
		vector inTransform[4];
		copyarray(inTransform, transform);
	    vector col0 = inTransform[0]; // (r00, r10, r20)
	    vector col1 = inTransform[1]; // (r01, r11, r21)
	    vector col2 = inTransform[2]; // (r02, r12, r22)
	
	    // Extract yaw from existing rotation
	    float r00 = col0[0]; // x of X basis
   		float r02 = col2[0]; // x of Z basis
    	float yaw = Math.Atan2(r02, r00);
	
	    float c = Math.Cos(yaw);
	    float s = Math.Sin(yaw);
	
	    // Pure yaw about Y axis:
  	  	// Ry = [ [  c, 0,  s ],
   	 	//        [  0, 1,  0 ],
   		//        [ -s, 0,  c ] ]
	    vector yawCol0 = Vector( c, 0, -s); // X basis column
   		vector yawCol1 = Vector( 0, 1,  0); // Y basis column
    	vector yawCol2 = Vector( s, 0,  c); // Z basis column
	
	    transform[0] = yawCol0;
	    transform[1] = yawCol1;
	    transform[2] = yawCol2;
	    transform[3] = inTransform[3];
	}
}

class BreadcrumbGizmo : DrawGizmoBase
{
	override bool Gizmo_IsAllowedTransformMode(GizmoTransformMode mode)
	{
		if (!super.Gizmo_IsAllowedTransformMode(mode))
			return false;

		if (mode == GizmoTransformMode.MOVE || mode == GizmoTransformMode.MOVE_2D)
			return true;
		
		return false;
	}
}

class BoxDrawGizmo : DrawGizmoBase {}

class BoxDrawCenterGizmo: BoxDrawGizmo
{
	override bool Gizmo_IsAllowedTransformMode(GizmoTransformMode mode)
	{
		if (!super.Gizmo_IsAllowedTransformMode(mode))
			return false;

		if (mode != GizmoTransformMode.SCALE)
			return true;
		
		return false;
	}
}

class BoxDrawCornerGizmo : BoxDrawGizmo
{
	override bool Gizmo_IsAllowedTransformMode(GizmoTransformMode mode)
	{
		if (!super.Gizmo_IsAllowedTransformMode(mode))
			return false;

		if (mode == GizmoTransformMode.MOVE || mode == GizmoTransformMode.MOVE_2D)
			return true;
		
		return false;
	}
}

class ArrayBoxDraw : array<ref BoxDraw>
{
	void Update()
	{
		for (int i = 0; i < Count(); ++i)
		{
			BoxDraw item = Get(i);
			item.Update();
		}
	}
}

class ArrayBreadcrumbDraw : array<ref BreadcrumbDraw>
{
	void Update()
	{
		for (int i = 0; i < Count(); ++i)
		{
			BreadcrumbDraw item = Get(i);
			item.Update();
		}
	}
}

class UndergroundTriggerDrawBase : Managed
{
	vector GetCenter()
	{
		return vector.Zero;
	}
	
	protected vector LocalToWorldFromMat(vector mat[4], vector localPos)
	{
	    vector right   = mat[0];
	    vector up      = mat[1];
	    vector forward = mat[2];
	    vector center  = mat[3];

	    return center + right * localPos[0] + up * localPos[1] + forward * localPos[2];
	}
	
	protected vector WorldToLocal(BoxDrawGizmo gizmo, vector worldPos)
	{
	    vector mat[4];
		gizmo.GetTransform(mat);
		
		vector right   = mat[0];
		vector up      = mat[1];
		vector forward = mat[2];
		vector center  = mat[3];

		vector v = worldPos - center;

		vector localPosition;
		localPosition[0] = vector.Dot(v, right);
		localPosition[1] = vector.Dot(v, up);
		localPosition[2] = vector.Dot(v, forward);

		return localPosition;
	}
}

class BoxDraw : UndergroundTriggerDrawBase
{
	private bool m_IsShown;
	private bool m_IsSelected;

	private vector m_LocalMin;
	private vector m_LocalMax;

	private ref BoxDrawCenterGizmo m_GizmoCenter;
	private ref BoxDrawCornerGizmo m_GizmoMin, m_GizmoMax;
	private UndergroundTriggerHandle m_HandleHelper

	private ref array<ref DebugTextWorldSpace> m_DebugTextsWS;
	
	private ref ScriptInvoker m_OnTransformChanged;
	
	void BoxDraw(vector position, vector orientation = "0 0 0", vector size = "1 1 1")
	{
	    if (!GetGizmoApi())
	        ErrorEx("Gizmo API not created, crash imminent", ErrorExSeverity.ERROR);
	
	    m_DebugTextsWS = new array<ref DebugTextWorldSpace>();
	
	    m_GizmoCenter = new BoxDrawCenterGizmo(position);
	
	    vector centerMat[4];
	    Math3D.YawPitchRollMatrix(orientation, centerMat);
	
	    centerMat[3] = position;
	    m_GizmoCenter.SetTransform(centerMat);
	
	    // Define local extents from size (rotation-independent)
	    vector he = size * 0.5;
	    m_LocalMin = Vector(-he[0], -he[1], -he[2]);
	    m_LocalMax = Vector( he[0],  he[1],  he[2]);
	
	    // Compute world positions for min/max gizmos from center transform
	    vector worldMin = LocalToWorldFromMat(centerMat, m_LocalMin);
	    vector worldMax = LocalToWorldFromMat(centerMat, m_LocalMax);
	
	    m_GizmoMin = new BoxDrawCornerGizmo(worldMin);
	    m_GizmoMax = new BoxDrawCornerGizmo(worldMax);
	
	    // sync corner's gizmo rotation to the center’s rotation
	    {
	        vector minMat[4]; copyarray(minMat, centerMat); minMat[3] = worldMin;
	        vector maxMat[4]; copyarray(maxMat, centerMat); maxMat[3] = worldMax;
	        m_GizmoMin.SetTransform(minMat);
	        m_GizmoMax.SetTransform(maxMat);
	    }
	
	    m_HandleHelper = UndergroundTriggerHandle.Cast(g_Game.CreateObjectEx("UndergroundTriggerHandle", position, ECE_LOCAL|ECE_NOLIFETIME|ECE_KEEPHEIGHT));
	    m_HandleHelper.SetBoxDrawRef(this);
	
	    UpdateMinGizmo();
	    UpdateMaxGizmo();
	
	    m_GizmoCenter.m_OnTransformChanged.Insert(OnCenterGizmoTransformChanged);
	    m_GizmoMin.m_OnTransformChanged.Insert(OnMinGizmoTransformChanged);
	    m_GizmoMax.m_OnTransformChanged.Insert(OnMaxGizmoTransformChanged);
	}
	
	void ~BoxDraw()
	{
		m_GizmoCenter.m_OnTransformChanged.Remove(OnCenterGizmoTransformChanged);
		m_GizmoMin.m_OnTransformChanged.Remove(OnMinGizmoTransformChanged);
		m_GizmoMax.m_OnTransformChanged.Remove(OnMinGizmoTransformChanged);
		
		if (m_HandleHelper)
			m_HandleHelper.Delete();
	}
	
	void Update()
	{
		vector globalTransform[4];
		m_GizmoCenter.GetTransform(globalTransform);
		DrawRotatedBox(globalTransform, 0x33ffb626);

		// update the helper handle position together with center gizmo
		m_HandleHelper.SetPosition(globalTransform[3] + "0.25 0.5 0.25");
		
		DrawDebugs(m_IsSelected);
	}
	
	UndergroundTriggerHandle GetTriggerHandle()
	{
		return m_HandleHelper;
	}
	
	ScriptInvoker Event_OnTransformChanged()
	{
		if (!m_OnTransformChanged)
			m_OnTransformChanged = new ScriptInvoker();

		return m_OnTransformChanged;	
	}
	
	void SetAllowTransform(bool state)
	{
		m_GizmoMin.m_TransformAllowed 		= state;
		m_GizmoMax.m_TransformAllowed 		= state;
		m_GizmoCenter.m_TransformAllowed 	= state;

		m_IsSelected = state;
	}
	
	void Select()
	{
		GetGizmoApi().SelectUser(m_GizmoMin);
		GetGizmoApi().SelectUser(m_GizmoMax);
		GetGizmoApi().SelectUser(m_GizmoCenter);
	}

	override vector GetCenter()
	{
		vector transform[4];
		m_GizmoCenter.GetTransform(transform);

		return transform[3];
	}
	
	vector GetOrientation()
	{
		vector transform[4];
		m_GizmoCenter.GetTransform(transform);

		return Math3D.MatrixToAngles(transform);
	}
	
	vector GetSize()
	{
		vector size = Math3D.BoxSize(m_LocalMin, m_LocalMax);
		return size;
	}

	private void UpdateMinGizmo()
	{
		vector globalTransform[4];
	    m_GizmoCenter.GetTransform(globalTransform);
		vector oldTransform[4];
		copyarray(oldTransform, globalTransform);
		vector worldMin = LocalToWorldFromMat(globalTransform, m_LocalMin);
	    vector worldMax = LocalToWorldFromMat(globalTransform, m_LocalMax);
	
        vector matMin[4];
		copyarray(matMin, globalTransform);
        matMin[3] = worldMin;
		
        m_GizmoMin.SetTransform(matMin);

		// update center position		
		vector center = Math3D.BoxCenter(worldMin, worldMax);
		globalTransform[3] = center;
		m_GizmoCenter.SetTransform(globalTransform);
		
		// recompute local extents in *new* center space
		m_LocalMin = WorldToLocal(m_GizmoCenter, worldMin);
		m_LocalMax = WorldToLocal(m_GizmoCenter, worldMax);

		Event_OnTransformChanged().Invoke(oldTransform, globalTransform);
	}
	
	private void UpdateMaxGizmo()
	{
	    vector globalTransform[4];
	    m_GizmoCenter.GetTransform(globalTransform);
		vector oldTransform[4];
		copyarray(oldTransform, globalTransform);
		vector worldMin = LocalToWorldFromMat(globalTransform, m_LocalMin);
	    vector worldMax = LocalToWorldFromMat(globalTransform, m_LocalMax);
	
        vector matMax[4];
		copyarray(matMax, globalTransform);
        matMax[3] = worldMax;

        m_GizmoMax.SetTransform(matMax);

		// update center position		
		vector center = Math3D.BoxCenter(worldMin, worldMax);
		globalTransform[3] = center;
		m_GizmoCenter.SetTransform(globalTransform);

		// recompute local extents in "new" center space
		m_LocalMin = WorldToLocal(m_GizmoCenter, worldMin);
		m_LocalMax = WorldToLocal(m_GizmoCenter, worldMax);
		
		Event_OnTransformChanged().Invoke(oldTransform, globalTransform);
	}
	
	private void OnCenterGizmoTransformChanged(vector transform[4])
	{
		vector oldTransform[4];
	    m_GizmoCenter.GetTransform(oldTransform);
	    m_GizmoCenter.SetTransform(transform);

		Event_OnTransformChanged().Invoke(oldTransform, transform);

		UpdateMinGizmo();
		UpdateMaxGizmo();
	}
	
	private void OnMinGizmoTransformChanged(vector transform[4])
	{
		Debug.DrawSphere(transform[3], 1.0, COLOR_RED_A, ShapeFlags.TRANSP|ShapeFlags.NOOUTLINE|ShapeFlags.ONCE);

		vector localPosition = WorldToLocal(m_GizmoCenter, transform[3]);
		m_LocalMin = localPosition;

		UpdateMinGizmo();
	}
	
	private void OnMaxGizmoTransformChanged(vector transform[4])
	{
		Debug.DrawSphere(transform[3], 1.0, COLOR_BLUE_A, ShapeFlags.TRANSP|ShapeFlags.NOOUTLINE|ShapeFlags.ONCE);

		vector localPosition = WorldToLocal(m_GizmoCenter, transform[3]);
		m_LocalMax = localPosition;
		
		UpdateMaxGizmo();
	}
	
	private void DrawDebugs(bool enabled)
	{
		m_DebugTextsWS.Clear();
		
		if (enabled)
		{
			vector globalTransform[4];
			m_GizmoCenter.GetTransform(globalTransform);
			vector worldMin = LocalToWorldFromMat(globalTransform, m_LocalMin);
		    vector worldMax = LocalToWorldFromMat(globalTransform, m_LocalMax);

			vector position	= worldMin;
			position[1] = position[1] + 0.2;
			m_DebugTextsWS.Insert(DebugTextWorldSpace.Create("Min", DebugTextFlags.ONCE, position[0], position[1], position[2], 20, 0xFFFFFFFF, 0x00000000));
			position	= worldMax;
			position[1] = position[1] + 0.2;
			m_DebugTextsWS.Insert(DebugTextWorldSpace.Create("Max", DebugTextFlags.ONCE, position[0], position[1], position[2], 20, 0xFFFFFFFF, 0x00000000));
		}
	}
	
	private void DrawRotatedBox(vector transform[4], int color)
	{
		vector cornersLocal[8];

		// build all 8 local corners from min/max
		cornersLocal[0] = m_LocalMin;
		cornersLocal[1] = Vector(m_LocalMax[0], m_LocalMin[1], m_LocalMin[2]);
		cornersLocal[2] = Vector(m_LocalMin[0], m_LocalMax[1], m_LocalMin[2]);
		cornersLocal[3] = Vector(m_LocalMax[0], m_LocalMax[1], m_LocalMin[2]);
		cornersLocal[4] = Vector(m_LocalMin[0], m_LocalMin[1], m_LocalMax[2]);
		cornersLocal[5] = Vector(m_LocalMax[0], m_LocalMin[1], m_LocalMax[2]);
		cornersLocal[6] = Vector(m_LocalMin[0], m_LocalMax[1], m_LocalMax[2]);
		cornersLocal[7] = m_LocalMax;

		vector c[8];
		for (int i = 0; i < 8; i++)
			c[i] = LocalToWorldFromMat(transform, cornersLocal[i]);
		
	    int edgePairs[] = {
	        0,1,  1,3,  3,2,  2,0,
	        4,5,  5,7,  7,6,  6,4,
	        0,4,  1,5,  2,6,  3,7
	    };
	
	    for (int j = 0; j < 24; j += 2)
	        Debug.DrawLine(c[edgePairs[j]], c[edgePairs[j + 1]], 0xff000000, ShapeFlags.ONCE);

		int size = 36;		
		
		int tris[] =
		{
		    // -Z
		    0, 1, 3,
		    0, 3, 2,
		
		    // +Z
		    4, 5, 7,
		    4, 7, 6,
		
		    // -X
		    0, 2, 6,
		    0, 6, 4,
		
		    // +X
		    1, 3, 7,
		    1, 7, 5,
		
		    // -Y
		    0, 1, 5,
		    0, 5, 4,
		
		    // +Y
		    2, 3, 7,
		    2, 7, 6
		};

		for (int p = 0; p < size; p += 3)
		{
			vector face[3] = {};
		    if (p + 2 >= size)
		        break; // safety check
		
		    face[0]	= c[tris[p]];
		    face[1] = c[tris[p + 1]];
		    face[2] = c[tris[p + 2]];

			Shape.CreateTris(color, ShapeFlags.ONCE|ShapeFlags.DOUBLESIDE|ShapeFlags.TRANSP, face, 3);
		}		
	}
}

class BreadcrumbDraw : UndergroundTriggerDrawBase
{
	private bool m_IsShown;
	private bool m_IsSelected;

	private vector m_LocalMin;
	private vector m_LocalMax;

	private vector m_LastTransform[4];

	private ref BreadcrumbGizmo m_GizmoCenter;

	private ref ScriptInvoker m_OnTransformChanged;

	private UndergroundBreadcrumbHandle m_HandleHelper

	private ref array<ref DebugTextWorldSpace> m_DebugTextsWS;
	
	void BreadcrumbDraw(vector position)
	{
	    if (!GetGizmoApi())
	        ErrorEx("Gizmo API not created, crash imminent", ErrorExSeverity.ERROR);
	
	    m_DebugTextsWS = new array<ref DebugTextWorldSpace>();
	
	    m_GizmoCenter = new BreadcrumbGizmo(position);
	
	    vector centerMat[4];
	
	    centerMat[3] = position;
		vector orientation = vector.Zero;
		Math3D.YawPitchRollMatrix(orientation, centerMat);
		copyarray(m_LastTransform, centerMat); // store the initial matrix in last transform cache
		m_GizmoCenter.SetTransform(centerMat);
		
	    m_HandleHelper = UndergroundBreadcrumbHandle.Cast(g_Game.CreateObjectEx("UndergroundBreadcrumbHandle", position, ECE_LOCAL|ECE_NOLIFETIME|ECE_KEEPHEIGHT));
	    m_HandleHelper.SetBreadcrumbDrawRef(this);
	
	    m_GizmoCenter.m_OnTransformChanged.Insert(OnCenterGizmoTransformChanged);
	}
	
	void ~BreadcrumbDraw()
	{
		m_GizmoCenter.m_OnTransformChanged.Remove(OnCenterGizmoTransformChanged);
		
		if (m_HandleHelper)
			m_HandleHelper.Delete();
	}
	
	UndergroundBreadcrumbHandle GetBreadcrumbHandle()
	{
		return m_HandleHelper;
	}

	ScriptInvoker Event_OnTransformChanged()
	{
		if (!m_OnTransformChanged)
			m_OnTransformChanged = new ScriptInvoker();

		return m_OnTransformChanged;
	}
	
	void ApplyTransformation(vector parentOldTransform[4], vector parentNewTransform[4])
	{
		vector parentOldTransformInv[4];
		
		 // Yaw delta from the two parent transforms
	    float cOld = parentOldTransform[2][2];
	    float sOld = parentOldTransform[2][0];
	    float cNew = parentNewTransform[2][2];
	    float sNew = parentNewTransform[2][0];
	
	    // cos/sin of (yawNew - yawOld) via angle subtraction identities
	    float cosDelta = cNew * cOld + sNew * sOld;
	    float sinDelta = sNew * cOld - cNew * sOld;
	
	    // Offset of child from old parent transform
	    vector offset = m_LastTransform[3] - parentOldTransform[3];
	
	    // Rotate offset by yaw delta around Y
		/*
	    vector rotatedOffset = Vector(
	         offset[0] * cosDelta + offset[2] * sinDelta,
	         offset[1],
	        -offset[0] * sinDelta + offset[2] * cosDelta,
	    );
		*/
		
    	// breadcrumb position based on parent's main rotation (yaw only)
		vector childNew[4];
		copyarray(childNew, parentNewTransform);
   		childNew[3] = parentNewTransform[3] + offset;

		vector globalTransform[4];
		m_GizmoCenter.GetTransform(globalTransform);
		globalTransform[3] = childNew[3];
		copyarray(m_LastTransform, globalTransform);
		m_GizmoCenter.SetTransform(globalTransform);
	}
	
	void Update()
	{
		vector globalTransform[4];
		m_GizmoCenter.GetTransform(globalTransform);
		DrawSphere(globalTransform, 0x550000cc, m_HandleHelper.m_Data.Radius);
		DrawDebugs(m_IsSelected);
		
		// update the helper handle position together with center gizmo
		m_HandleHelper.SetPosition(globalTransform[3] + "0.25 0.5 0.25");
	}
	
	void SetAllowTransform(bool state)
	{
		m_GizmoCenter.m_TransformAllowed 	= state;

		m_IsSelected = state;
	}
	
	void Select()
	{
		GetGizmoApi().SelectUser(m_GizmoCenter);
	}

	override vector GetCenter()
	{
		vector transform[4];
		m_GizmoCenter.GetTransform(transform);

		return transform[3];
	}
	
	private void OnCenterGizmoTransformChanged(vector transform[4])
	{
		vector oldTransform[4];
		m_GizmoCenter.GetTransform(oldTransform);
	    m_GizmoCenter.SetTransform(transform);

		Update();

		if (m_OnTransformChanged)
			m_OnTransformChanged.Invoke(oldTransform, transform);
	}
	
	private void DrawDebugs(bool enabled)
	{
		m_DebugTextsWS.Clear();
		
		if (enabled)
		{
			vector globalTransform[4];
			m_GizmoCenter.GetTransform(globalTransform);

			vector position	= globalTransform[3];
			position[1] = position[1] + 0.2;
			m_DebugTextsWS.Insert(DebugTextWorldSpace.Create(m_HandleHelper.GetHandleName(), DebugTextFlags.ONCE, position[0], position[1], position[2], 20, 0xFFFFFFFF, 0x00000000));
		}
	}
	
	private void DrawSphere(vector transform[4], int color, float radius = 0.15)
	{
		Shape.CreateSphere(color, ShapeFlags.ONCE|ShapeFlags.DOUBLESIDE|ShapeFlags.TRANSP, transform[3], radius);
	}
}

#endif
