#include <ipercept/video/FaceRecognition.h>

#include <debugger.h> 

using namespace core;
using namespace core::ipercept;
using namespace cv;

int SAVE_EIGENFACE_IMAGES = 1;		// Set to 0 if you dont want images to be saved
//#define USE_MAHALANOBIS_DISTANCE	// better accuracy.

#define IMAGE_WIDTH		112
#define IMAGE_HEIGHT	92


FaceRecognition::FaceRecognition(IApplicationConfiguration *appconfig): app_config(appconfig),
  face_width(IMAGE_WIDTH), face_height(IMAGE_HEIGHT), facepic_to_person_indexes(NULL), 
  average_image(NULL), eigen_values(NULL), projected_trainning_faces(NULL), projected_test_face(NULL),
  eigen_vector_pp(NULL), initialized(false)
{
	std::stringstream cascade_name;
	if(app_config)	cascade_name << app_config->GetExternDirectory() << "encara2/ENCARA2data/HaarClassifiers/haarcascade_frontalface_alt2.xml";
	else			cascade_name << "./haarcascade_frontalface_alt2.xml";
	cascade = (CvHaarClassifierCascade*)cvLoad( cascade_name.str().c_str(), 0, 0, 0 );

	LoadTrainningData();
	face_width  = IMAGE_WIDTH;
	face_height = IMAGE_HEIGHT;
	if( eigen_vector.size() > 0 )
	//{	projected_test_face = (float *)cvAlloc( eigen_vector.size()*sizeof(float) );
		initialized = true;	//};
}

FaceRecognition::~FaceRecognition()
{}

int FaceRecognition::RecognizeFromImage(char* data, const int &size_x, const int &size_y, const int &n_channels, const int &depth, const int &width_step)
{
	if (!initialized)
		return -1;

	if( !projected_test_face )
		projected_test_face = (float *)cvAlloc( eigen_vector.size()*sizeof(float) );

	CvSize size, desired_size;
	size.width			= size_x;
	size.height			= size_y;
	desired_size.width	= face_width;
	desired_size.height	= face_height;

	IplImage *zimage = cvCreateImage(size, depth, n_channels);
	char *default_data = zimage->imageData;
	zimage->imageData = data;
	IplImage *face_gray, *scaled, *equalized;
	face_gray = scaled = equalized = NULL;

	//Grayscale
	if( n_channels == 3 )
	{	face_gray = cvCreateImage( size, IPL_DEPTH_8U, 1 );
		cvCvtColor( zimage, face_gray, CV_BGR2GRAY );
	}
	else
		face_gray = NULL;

	//resize 
	scaled = cvCreateImage( desired_size, IPL_DEPTH_8U, 1 );
	if (face_width > size.width && size.height > size.height) 
		cvResize(face_gray, scaled, CV_INTER_LINEAR);// CV_INTER_CUBIC or CV_INTER_LINEAR is good for enlarging
	else
		cvResize(face_gray, scaled, CV_INTER_AREA);	 // CV_INTER_AREA is good for shrinking/decimation, but bad at enlarging.

	//equalize
	equalized = cvCreateImage( desired_size, IPL_DEPTH_8U, 1);
	cvEqualizeHist( scaled, equalized );
	
	//Recognize
	int candidate = -1; //candidate_face
	int candidate_index;//corresponding user to candidate_face 
	float likeness;
	if(equalized && (eigen_vector.size() > 0))
	{
		cvEigenDecomposite( equalized, eigen_vector.size(), eigen_vector_pp, 0, 0, average_image, projected_test_face);
		candidate		= FindBestCandidate(projected_test_face, likeness);
		candidate_index	= (candidate != -1) ? facepic_to_person_indexes->data.i[candidate] : -1;
	}

	zimage->imageData = default_data;
	cvReleaseImage(&zimage);
	cvReleaseImage(&face_gray);
	cvReleaseImage(&scaled);
	cvReleaseImage(&equalized);

	return candidate_index;
}

void FaceRecognition::AddUser(std::vector<core::Image> faces, const int &user_id)
{	
	int n_faces = faces.size();
	if (n_faces == 0) return;
	int previous_entries = 0;

	//Updates facepic_to_person_indexes
	if( facepic_to_person_indexes )
	{	previous_entries = facepic_to_person_indexes->cols;
		CvMat *aux = facepic_to_person_indexes;
		CvMat *new_mat = cvCreateMat( 1, previous_entries+n_faces, CV_32SC1 );
		for (int i = 0; i < previous_entries; i++)
			new_mat->data.i[i] = facepic_to_person_indexes->data.i[i];
		facepic_to_person_indexes = new_mat;
		cvReleaseMat(&aux);
	}
	else 
		facepic_to_person_indexes = cvCreateMat( 1, faces.size(), CV_32SC1 );

	int i = 0;
	for (std::vector<core::Image>::iterator iter= faces.begin(); iter != faces.end(); iter++)
	{
		CvSize size, desired_size;
		size.width			= (*iter).width;
		size.height			= (*iter).height;
		desired_size.width	= face_width;
		desired_size.height	= face_height;

		IplImage *zimage = cvCreateImage(size, (*iter).depth_, (*iter).nchannels);
		zimage->imageData = (*iter).image;

		IplImage *face_gray, *scaled, *equalized;
		//Grayscale
		if((*iter).nchannels == 3)
		{	face_gray = cvCreateImage( size, IPL_DEPTH_8U, 1 );
			cvCvtColor( zimage, face_gray, CV_BGR2GRAY );
		}
		else
			face_gray = NULL;

		//resize 
		scaled = cvCreateImage( desired_size, IPL_DEPTH_8U, 1 );
		if (face_width > size.width && size.height > size.height) 
			cvResize(face_gray, scaled, CV_INTER_LINEAR);// CV_INTER_CUBIC or CV_INTER_LINEAR is good for enlarging
		else
			cvResize(face_gray, scaled, CV_INTER_AREA);	 // CV_INTER_AREA is good for shrinking/decimation, but bad at enlarging.

		//equalize
		equalized = cvCreateImage( desired_size, IPL_DEPTH_8U, 1);
		cvEqualizeHist( scaled, equalized );

		//add pic to face_vector and register facepic_to_person relation
		face_vector.push_back(equalized);
		facepic_to_person_indexes->data.i[previous_entries+i] = user_id;

		cvReleaseImage(&face_gray);
		cvReleaseImage(&scaled);
		cvReleaseImage(&zimage);
		i++;
	}

	Train();
}

void FaceRecognition::Train()
{
	//CvMat *train_person;
	cvReleaseImage( &average_image );
	for (unsigned int i = 0; i < eigen_vector.size(); i++) 
		if (eigen_vector[i])
			cvReleaseImage(&eigen_vector[i]);
	//cvFree(&eigen_vector); 
	eigen_vector.clear();
	//cvFree(&facepic_to_person_indexes);

	//user_names.clear();
	cvFree(&eigen_values);
	cvFree(&projected_trainning_faces);

	//learn
	//PCA
	CvTermCriteria term_criteria;
	CvSize size;
	int n_eigen = face_vector.size()-1;
	size.width  = face_vector[0]->width;
	size.height = face_vector[0]->height;
	term_criteria = cvTermCriteria( CV_TERMCRIT_ITER, n_eigen, 1);

	//Reset data---------------------
	if (eigen_vector.size() > 0)
		for(unsigned int i = 0; i < eigen_vector.size(); i++)
			if(eigen_vector[i])	cvReleaseImage(&eigen_vector[i]);
	eigen_vector.clear();
	for(int i = 0; i < n_eigen; i++)
		eigen_vector.push_back( cvCreateImage(size, IPL_DEPTH_32F, 1));

	if(eigen_values) cvReleaseMat(&eigen_values);
	eigen_values = cvCreateMat( 1, n_eigen, CV_32FC1 );
	
	if (average_image) cvReleaseImage(&average_image);
	average_image = cvCreateImage(size, IPL_DEPTH_32F, 1);

	if (projected_test_face)
		cvFree(&projected_test_face);
	projected_test_face = NULL;
	projected_test_face = (float *)cvAlloc( eigen_vector.size()*sizeof(float) );

	//vector to pp **
	IplImage **face_vector_pp  = (IplImage**)cvAlloc(sizeof(IplImage*) * face_vector.size());
	eigen_vector_pp = (IplImage**)cvAlloc(sizeof(IplImage*) * n_eigen);
	for (unsigned int i = 0; i < face_vector.size(); i++)
		face_vector_pp[i] = cvCloneImage(face_vector[i]);
	for ( int i = 0; i < n_eigen; i++)
		eigen_vector_pp[i] = cvCreateImage(size, IPL_DEPTH_32F, 1);

	//calc eigen
	cvCalcEigenObjects( face_vector.size(), (void*)face_vector_pp,	(void*)eigen_vector_pp, CV_EIGOBJ_NO_CALLBACK, 0, 0, &term_criteria, average_image, eigen_values->data.fl);
	cvNormalize(eigen_values, eigen_values, 1, 0, CV_L1, 0);
	
	//pp to vector **
	std::stringstream face_path, eigen_path;
	if(app_config)	
	{	face_path << app_config->GetDataDirectory() << "recon/face_pic/";
		eigen_path << app_config->GetDataDirectory() << "recon/eigen_pic/";
	}
	for (unsigned int i = 0; i < face_vector.size(); i++)
	{	std::stringstream face_path_c;
		face_path_c  << face_path.str()  << i << ".pgm";
		cvSaveImage(face_path_c.str().c_str(), face_vector[i]);
		cvReleaseImage(&face_vector_pp[i]);
	}
	cvFree(&face_vector_pp);
	//------------

	projected_trainning_faces = cvCreateMat( face_vector.size(), eigen_vector.size(), CV_32FC1 );
	int offset = projected_trainning_faces->step / sizeof(float);
	for(unsigned int i=0; i < face_vector.size(); i++)
	{	float *coeffs = projected_trainning_faces->data.fl;
		coeffs +=  i*offset;
		cvEigenDecomposite( face_vector[i], eigen_vector.size(), eigen_vector_pp, 0, 0, average_image, coeffs);
	}

	//save to disk and release
	for (int i = 0; i < n_eigen; i++)
	{	std::stringstream eigen_path_c;
		eigen_path_c << eigen_path.str() << i << ".pgm";
		eigen_vector[i] = cvCloneImage(eigen_vector_pp[i]);
		IplImage *u8image = convertFloat32ToUchar8(eigen_vector[i]);
		cvSaveImage(eigen_path_c.str().c_str(), u8image);
		cvReleaseImage(&u8image);
		//cvReleaseImage(eigen_vector_pp[i]);	
	}

	std::stringstream average_path_c;
	average_path_c << eigen_path.str() << "average.pgm";
	IplImage *u8image = convertFloat32ToUchar8(average_image);
	cvSaveImage(average_path_c.str().c_str(), u8image);
	cvReleaseImage(&u8image);

	StoreTrainningData();
	//cvFree(&eigen_vector_pp);
	initialized = true;
}

int  FaceRecognition::FindBestCandidate(float *projected_test_face_, float &likeness, const bool &use_mahalanobis)
{
	double min_distance = DBL_MAX;
	int candidate = -1;
	int eigen_vector_size = eigen_vector.size();

	for(unsigned int trainning_face = 0; trainning_face < face_vector.size(); trainning_face++)
	{	double distance = 0;
		for(int eigen = 0; eigen < eigen_vector_size; eigen++)
		{	double aux_dis = projected_test_face[eigen] - projected_trainning_faces->data.fl[trainning_face*eigen_vector_size+eigen];
			if (use_mahalanobis)
				distance += aux_dis * aux_dis / eigen_values->data.fl[eigen];
			else
				distance += aux_dis * aux_dis;
		}
		if ( distance < min_distance )
		{	min_distance = distance;
			candidate = trainning_face;	}
	}

	likeness = 1.0f - sqrt( min_distance / (float)(face_vector.size() * eigen_vector_size) ) / 255.0f;
	return ((likeness > 0.65f) ? candidate : -1);
}

bool FaceRecognition::LoadTrainningData()
{
	if(eigen_vector.size() >0)
		for (std::vector<IplImage *>::iterator iter = eigen_vector.begin(); iter != eigen_vector.end(); iter++)
			cvReleaseImage(&(*iter));
	eigen_vector.clear();

	if(face_vector.size() >0)
		for (std::vector<IplImage *>::iterator iter = face_vector.begin(); iter != face_vector.end(); iter++)
			cvReleaseImage(&(*iter));
	face_vector.clear();

	CvFileStorage * file_storage;
	std::stringstream wop;
	wop << app_config->GetRootDirectory() << "facedata.xml";
	file_storage = cvOpenFileStorage( wop.str().c_str(), 0, CV_STORAGE_READ );
	if( !file_storage ) 
	{	std::cout << "No file facedata.xml.\n";
		return false;	}

	// Load data
	int face_vector_size		= cvReadIntByName(file_storage, 0, "face_vector_size", 0);
	int eigen_vector_size		= cvReadIntByName(file_storage, 0, "eigen_vector_size", 0);
	facepic_to_person_indexes	= (CvMat *)cvReadByName(file_storage, 0, "facepic_to_person_indexes", 0);
	eigen_values				= (CvMat *)cvReadByName(file_storage, 0, "eigen_values", 0);
	projected_trainning_faces	= (CvMat *)cvReadByName(file_storage, 0, "projected_trainning_faces", 0);
	average_image				= (IplImage *)cvReadByName(file_storage, 0, "average_image", 0);
	
	for(int i = 0;  i < face_vector_size; i++)
	{	std::stringstream ss_index;
		ss_index << "face_vector_" << i;
		face_vector.push_back((IplImage *)cvReadByName(file_storage, 0, ss_index.str().c_str(), 0));	}

	eigen_vector_pp = (IplImage**)cvAlloc(sizeof(IplImage*) * eigen_vector_size);

	for(int i = 0;  i < eigen_vector_size; i++)
	{	std::stringstream ss_index;
		ss_index << "eigen_vector_" << i;
		eigen_vector.push_back((IplImage *)cvReadByName(file_storage, 0, ss_index.str().c_str(), 0));	
		eigen_vector_pp[i] = cvCloneImage(eigen_vector[i]); }//MEMLEAK!

	cvReleaseFileStorage( &file_storage );
	return true;
}

void FaceRecognition::StoreTrainningData()
{
	CvFileStorage * file;
	std::stringstream wop;
	wop << app_config->GetRootDirectory() << "facedata.xml";
	file = cvOpenFileStorage( wop.str().c_str(), 0, CV_STORAGE_WRITE );

	cvWriteInt	( file, "face_vector_size",  face_vector.size() );
	cvWriteInt	( file, "eigen_vector_size", eigen_vector.size() );
	cvWrite		( file, "facepic_to_person_indexes", facepic_to_person_indexes, cvAttrList(0,0));
	cvWrite		( file, "eigen_values", eigen_values, cvAttrList(0,0));
	cvWrite		( file, "projected_trainning_faces", projected_trainning_faces, cvAttrList(0,0));
	cvWrite		( file, "average_image", average_image, cvAttrList(0,0));
	
	for(unsigned int i = 0; i < eigen_vector.size(); i++)
	{	std::stringstream ss_eigen_vector;
		ss_eigen_vector << "eigen_vector_" << i;
		cvWrite(file, ss_eigen_vector.str().c_str(), eigen_vector[i], cvAttrList(0,0));	}

	for(unsigned int i = 0; i < face_vector.size(); i++)
	{	std::stringstream ss_index;
		ss_index << "face_vector_" << i;
		cvWrite(file, ss_index.str().c_str(), face_vector[i], cvAttrList(0,0));	}

	cvReleaseFileStorage(&file);
}

IplImage* FaceRecognition::convertFloat32ToUchar8(const IplImage *src)
{
	IplImage *dts = NULL;
	if ((src) && (src->width > 0 && src->height > 0)) 
	{
		double min, max;
		cvMinMaxLoc(src, &min, &max);
		// handle NaN.
		if (cvIsNaN(min) || min < -FLT_MAX)
			min = -FLT_MAX;
		if (cvIsNaN(max) || max > FLT_MAX)
			max = FLT_MAX;
		if ( max - min == 0.0f )
			max = min + 0.001;	//avoid divide by zero

		dts = cvCreateImage(cvSize(src->width, src->height), IPL_DEPTH_8U, 1);
		cvConvertScale(src, dts, 255.0 / (max - min), - min * 255.0 / (max-min));
	}
	return dts;
}