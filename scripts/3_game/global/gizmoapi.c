enum GizmoTransformMode
{
	NONE,
	MOVE,
	MOVE_2D,
	ROTATE,
	SCALE,
};

enum GizmoSpaceMode
{
	WORLD,
	LOCAL,
	CAMERA,
};

/**
 * Temporary hande for a gizmo, not guaranteed to be valid after 
 * any CRUD operation within GizmoApi or on a new frame
 */
typedef int GizmoHandle;

/**
 * API for managing gizmos
 */
class GizmoApi : Managed
{
	/**
	 * Return the number of gizmos
	 */
	proto GizmoHandle GetCount();

	/**
	 * Return the instance passed in for the temporary handle
	 */
	proto Class GetInstance(GizmoHandle index);

	/**
	 * Return the tracker passed in for the temporary handle
	 */
	proto Managed GetTracker(GizmoHandle index);

	/**
	 * Returns a temporary handle 
	 */
	proto GizmoHandle FindByTracker(Managed tracker);

	/**
	 * Deselect the gizmo for the given tracker
	 */
	proto void DeselectTracker(Managed tracker);

	/**
	 * Deselect the gizmo for the temporary handle
	 */
	proto void Deselect(GizmoHandle index);

	/**
	 * Deselect all gizmos
	 */
	proto void DeselectAll();

	/**
	 * Sets the transform of the object directly. Internally functions similiar to 'SelectUser'
	 * 
	 * Tracker: The tracker is the same object
	 */
	proto void SelectObject(Object object);

	/**
	 * Applies impulses to set the position when dynamic, otherwise sets the transform in the physics scene. Doesn't work in multiplayer
	 * 
	 * Tracker: The tracker is the parent entity
	 * 
	 * Note: Currently, GizmoGetInstance doesn't work due to Script Error: 'Physics' can't be compared against 'Class'
	 */
	proto void SelectPhysics(Physics physics);

	/**
	 * Scripted controls, requires the following methods to be implemented in the class
	 * 	void Gizmo_SetWorldTransform(vector[4] transform, bool finalize)
	 * 	void Gizmo_GetWorldTransform(vector[4] transform)
	 *  bool Gizmo_IsAllowedTransformMode(GizmoTransformMode mode)
	 *  bool Gizmo_IsAllowedSpaceMode(GizmoSpaceMode mode)
	 * 
	 * Tracker: The tracker is the same instance
	 */
	proto void SelectUser(Managed instance);

	/**
	 * Set current transform mode for all gizmos
	 * 
	 * 'Gizmo_IsAllowedTransformMode' function will override the transform mode per gizmo to NONE if returned false
	 * 	
	 */
	proto void SetTransformMode(GizmoTransformMode mode);

	/**
	 * Get current transform mode for all gizmos
	 */
	proto GizmoTransformMode GetTransformMode();

	/**
	 * Set current space mode for all gizmos
	 * 
	 * 'Gizmo_IsAllowedSpaceMode' function will override the space mode per gizmo to NONE if returned false
	 */
	proto void SetSpaceMode(GizmoSpaceMode mode);

	/**
	 * Get current space mode for all gizmos
	 */
	proto GizmoSpaceMode GetSpaceMode();
	
};

proto void CreateGizmoApi();
proto void DestroyGizmoApi();

proto GizmoApi GetGizmoApi();
