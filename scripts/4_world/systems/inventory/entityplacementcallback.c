class EntityPlacementCallback : ObjectSnapCallback
{
#ifdef DIAG_DEVELOPER
#ifndef SERVER
	ref array<Shape> m_Shapes = new array<Shape>();
#endif
#endif

	void EntityPlacementCallback()
	{
	}

	void ~EntityPlacementCallback()
	{
		ClearDebugShapes();
	}

	void ClearDebugShapes()
	{
#ifdef DIAG_DEVELOPER
#ifndef SERVER
		foreach (auto shape : m_Shapes)
		{
			shape.Destroy();
			shape = null;
		}
		m_Shapes.Clear();
#endif
#endif
	}

	override void OnSetup()
	{
#ifdef DIAG_DEVELOPER
#ifndef SERVER
		m_DebugEnabled = DiagMenu.GetBool(DiagMenuIDs.INVENTORY_ENTITY_PLACEMENT_CALLBACK_DEBUG);
#endif
#endif

		ClearDebugShapes();

		m_DirectionFunc[0] = 1.2;
		m_DirectionFunc[1] = 0.3;

		Human human;
		if (Class.CastTo(human, m_Owner))
		{
			m_OwnerPosition = human.GetBonePositionWS(human.GetBoneIndexByName("Head"));
			
			HumanInputController hic = human.GetInputController();
			if (hic)
			{
				float headingAngle = hic.GetHeadingAngle() + Math.PI_HALF;
				m_OwnerDirection[0] = Math.Cos(headingAngle);
				m_OwnerDirection[1] = 0.0;
				m_OwnerDirection[2] = Math.Sin(headingAngle);
				
				m_OwnerDirection.Normalized();
			}
			else
			{
				m_OwnerDirection = human.GetDirection();
			}
		}
		else if (m_Owner)
		{
			m_OwnerPosition = m_Owner.GetPosition();
			m_OwnerDirection = m_Owner.GetDirection();
		}
	}

	override void OnDebug(vector p0, vector p1, bool hasHit, bool found)
	{
#ifdef DIAG_DEVELOPER
#ifndef SERVER
		Shape shape;
		
		int alpha = 0x33000000;
		int colour = 0x00FF0000;
		if (!hasHit && found)
		{
			colour = 0x000000FF;
		}
		else if (!hasHit)
		{
			colour = 0x0000FF00;
			alpha = 0x11000000;
		}
		
		shape = Shape.Create(ShapeType.BBOX, colour | alpha, ShapeFlags.NOZBUFFER | ShapeFlags.TRANSP, -m_Extents, m_Extents);
		shape.SetMatrix(m_Transform);
		m_Shapes.Insert(shape);

		vector p[2];
		p[0] = p0;
		p[1] = p1;
		shape = Shape.CreateLines(colour | 0xFF000000, ShapeFlags.NOZBUFFER | ShapeFlags.TRANSP, p, 2);
		m_Shapes.Insert(shape);
#endif
#endif
	}

	override bool OnFirstContact(Object other)
	{
		if (!other.IsTransport())
		{
			return false;
		}

		return true;
	}

	override bool OnQuery(Object other)
	{
		if (other.IsScenery())
		{
			return false;
		}
		
		if (other.IsMan())
		{
			return false;
		}

		EntityAI entity;
		if (Class.CastTo(entity, other))
		{
			if (entity.IsAnimal() || entity.IsZombie())
			{
				return false;
			}
		}
		
		if (other.GetCollisionRadius() < 0.05)
		{
			return false;
		}

		return true;
	}

	override bool OnCollide(Object other)
	{		
		if (other.IsScenery())
		{
			return false;
		}
		
		if (other.IsMan())
		{
			return false;
		}
		
		if (!other.CanObstruct() && !other.IsTransport())
		{
			return false;
		}
		
		return true;
	}
};
