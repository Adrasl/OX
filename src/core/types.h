#ifndef _CORETYPES_
#define _CORETYPES_

#include <string>
#include <core/Export.h>
#include <vector>

namespace core
{

	struct vector3F
	{
			float x;
			float y;
			float z;

			vector3F () : x(0), y(0), z(0) {}
			vector3F (float x_val, float y_val, float z_val) : x(x_val), y(z_val), z(z_val) {}
			//vector3F (vector3F &vector) : x(vector.x), y(vector.y), z(vector.z) {}
			vector3F (const vector3F &vector) : x(vector.x), y(vector.y), z(vector.z) {}
	};

	struct vector2I
	{
			int x;
			int y;
	};

	struct Rect3F
	{
	  Rect3F()  
	  { min[0] = min[1] = min[2] = 0;
		max[0] = max[1] = max[2] = 0;
	  }

	  Rect3F(float a_minX, float a_minY, float a_minZ, float a_maxX, float a_maxY, float a_maxZ)
	  { min[0] = a_minX; min[1] = a_minY; min[2] = a_minZ;
		max[0] = a_maxX; max[1] = a_maxY; max[2] = a_maxZ;
	  }

	//Rect3F(Rect3F &rec)  
	//  { min[0] = rec.min[0];
	//    min[1] = rec.min[1];
	//	min[2] = rec.min[2];
	//	max[0] = rec.max[0];
	//	max[1] = rec.max[1];
	//	max[2] = rec.max[2];
	//  }

	Rect3F(const Rect3F &rec)  
	  { min[0] = rec.min[0];
	    min[1] = rec.min[1];
		min[2] = rec.min[2];
		max[0] = rec.max[0];
		max[1] = rec.max[1];
		max[2] = rec.max[2];
	  }

	  float min[3];
	  float max[3];
	};

	class MotionElement
	{
		public:

			MotionElement() : magnitude(0) {};
			~MotionElement(){};
			MotionElement(const MotionElement& value)
			{
				rect        = value.rect;
				magnitude   = value.magnitude; 
				direction   = value.direction;
			}

			Rect3F rect;
			float magnitude;
			vector3F direction;
	};

	class _COREEXPORT_ Image
	{
		public:

			Image(char *input, int size_x, int size_y, int n_channels, int depth, int width_step) 
			{
				image = input; 
				width = size_x;
				height = size_y;
				nchannels = n_channels;
				depth_ = depth;
				widthstep = width_step;
			}
			~Image(){ };
			char *image;
			int width, height, nchannels, depth_, widthstep;
	};

	template <class info> struct corePoint2D
	{
		info x;
		info y;
		corePoint2D& corePoint2D::operator=(const corePoint2D& value) 
		{	if(this != &value)
			{	x = value.x;
				y = value.y;		 
			}
			return *this;
		}
	};

	template <class info> struct corePoint3D
	{
		info x;
		info y;
		info z;
		corePoint3D& corePoint3D::operator=(const corePoint3D& value) 
		{	if(this != &value)
			{	x = value.x;
				y = value.y;
				z = value.z;	 
			}
			return *this;
		}
	};

	/** \brief state protocol data unit */
	template <class info> struct corePDU3D
	{
		corePoint3D<info> position;
		corePoint3D<info> velocity;
		corePoint3D<info> acceleration;

		corePDU3D& corePDU3D::operator=(const corePDU3D& value) 
		{	if(this != &value)
			{	position = value.position;
				velocity = value.velocity;
				acceleration = value.acceleration;
			}
			return *this;
		}
	};

	/** \brief state protocol data unit */
	template <class infoSound, class infoSoundbuffer> struct coreSound
	{
		infoSound *sound_data;
		infoSoundbuffer *sound_buffer;
		corePoint3D<double> position;
		float pitch;
		float amplitude;
		float min_distance;
		float attetuation;
		bool loop;
		bool relative_to_listener;


		coreSound()
		{	sound_data = NULL;
			sound_buffer = NULL;
			position.x = position.y = position.z = 0.0;
			pitch = 1.0f;
			amplitude = 100.0f;
			loop = false;
			relative_to_listener = true;
			min_distance = 10.0f;
			attetuation = 0.75f;
		}

		coreSound(corePoint3D<double> position_, infoSound* sound_data_, infoSoundbuffer *sound_buffer_, 
			      bool loop_ = false, bool relative_to_listener_ = true, 
				  float pitch_ = 1.0, float amplitude_ = 100, float min_distance_ = 10.0f, float attetuation_ = 0.75f)
		{
			sound_data = sound_data_;
			sound_buffer = sound_buffer_;
			position = position_;
			pitch = pitch_;
			amplitude = amplitude_;
			loop = loop_;
			relative_to_listener = relative_to_listener_;
			min_distance = min_distance_;
			attetuation = attetuation_;
		}

		coreSound& coreSound::operator=(const coreSound& value) 
		{	if(this != &value)
			{	sound_data = value.sound_data;
				sound_buffer = value.sound_buffer;
				position = value.position;
				pitch = value.pitch;
				amplitude = value.amplitude;
				loop = value.loop;
				relative_to_listener = value.relative_to_listener;
				min_distance = value.min_distance;
				attetuation = value.attetuation;
			}
			return *this;
		}
	};



	class UserDataElement
	{
		public:

			UserDataElement() : speed(0), size(1) {};
			~UserDataElement(){};
			UserDataElement(const UserDataElement& value)
			{
				position		= value.position;
				speed			= value.speed; 
				size			= value.size; 
				speed_direction	= value.speed_direction;
			}

			UserDataElement& UserDataElement::operator=(const UserDataElement& value) 
			{	if(this != &value)
				{	position		= value.position;
					speed			= value.speed; 
					size			= value.size; 
					speed_direction	= value.speed_direction;	 
				}
				return *this;
			}

			vector3F position;
			float speed, size;
			vector3F speed_direction;
	};


	class Observer {

	  public:

		  virtual void Notified(void* callinginstance = NULL, const std::string &tag = "", const int &flag = 0) {}

	};

	class Subject {

		std::vector< class Observer* > observers;

	  public:

		int ObserversCount() 
		{
			return observers.size();
		}

		void attach(Observer *observer) 
		{
			if (observer)
			{
				bool found = false;
				for (std::vector< class Observer* >::iterator iter = observers.begin(); !found && (iter != observers.end()); iter++ )
					found = ( (*iter) == observer );

				if (!found)
					observers.push_back(observer);
			}
		}

		void detach(Observer *observer) 
		{
			if (observer)
			{
				for (std::vector< class Observer* >::iterator iter = observers.begin(); iter != observers.end();  )
				{
					if ( (*iter) == observer )
						observers.erase(iter);
					else
						iter++;
				}
			}
		}

		void detach_all() 
		{
			for (std::vector< class Observer* >::iterator iter = observers.begin(); iter != observers.end();  )
			{
				if ( (*iter) )
					observers.erase(iter);
				else
					iter++;
			}
		}

		virtual void Notify(void* callinginstance = NULL, const std::string &tag = "", const int &flag = 0) {
			for (std::vector< class Observer* >::iterator iter = observers.begin(); iter != observers.end(); iter++)
			{
				if (*iter)
					(*iter)->Notified(callinginstance, tag, flag);
			}
		}
	};

}

#endif