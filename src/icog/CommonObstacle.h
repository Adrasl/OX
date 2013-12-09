#ifndef _COMMON_OBSTACLE_
#define _COMMON_OBSTACLE_

#include <core/IPersistence/IEntityPersistence.h>
#include <core/IProd/IEntity.h>
#include <core/types.h>

#include <RTreeTemplate/RTree.h>

#include <string>
#include <vector>
#include <iostream> 
#include <map>


namespace core
{
	namespace icog
	{

		enum TypeOfObstacle {	SPHERE	= 1		};

		class CommonObstacle 
		{
			public:

				CommonObstacle();
				virtual ~CommonObstacle();	

				float SetBoundingRadius(const float &radius_)	{ radius = radius_;		}
				float GetBoundingRadius()						{ return radius;		}

			protected:

				float radius;


		};
	}
}

#endif