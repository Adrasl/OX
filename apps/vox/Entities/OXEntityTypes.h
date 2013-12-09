#ifndef _OXENTITYTYPES_
#define _OXENTITYTYPES_

#include <string>
#include <core/Export.h>
#include <vector>

namespace core
{
	namespace iprod
	{
		
		//retomar, esto debería ir en otro fichero común
		enum NatureOfEntity {
								STANDALONE		= 1,
								BOID			= 2,
								TREE			= 3};

		enum Species		 {	UNDEFINED		= 1,
								SPECIES1		= 2,
								SPECIES2		= 3};

		static enum IA_State {	BORN,
								HAPPY,
								ANGRY,
								SCARED,
								HUNGRY,
								HORNY,
								DEAD };

		static enum IA_Karma {	GOOD,
								NEUTRAL,
								EVIL};

		static enum IA_Energy{	CALM,
								EXITED};
	}
}

#endif