/*
* File:
*    ngx_http_ipiove_reco_dll.h
* Description:
*
* Author:
*    zqsong
* Review:
*    Ver      Author         Date        Comment
*    -------------------------------------------
*    0.0.1    zqsong         2012.3.11   Draft
*    0.0.2    chunyuan Liao  2012.3.20   first version
*	 0.0.3    mHuang		 2012.10.23  second version
**/

#ifndef __NGX_HTTP_IPIOVE_IPIOVE_RECO_DLL_H_INCLUDED__
#define __NGX_HTTP_IPIOVE_IPIOVE_RECO_DLL_H_INCLUDED__


#include <stdint.h>

#define MAX_STRING_ID_LENGTH	80
#define MAX_ERR_STRING_LENGTH   256

typedef intptr_t reco_instance_handle;

typedef struct{
	const char * collectionID;			//the unique ID of the given image collection
	const char * indexZipFilePath;		//the path  file for the collection, var name updated by mHuang on 10/23/2012
}update_index_param_t;

typedef struct{
	const char * collection;			//ID of the collection of the index images
	const char * algorithm;
	const char * dataType;				//"file": the pImageData is an image file path;
										//otherwise, pImageData is a memory block holding the actual image data and
										//dataType is the format of the image data, including  jpg, png, bmp, tif ....
    const char * imgName;
	void * pImageData;			//the pointer to the image file path or memory block
	int	 imageDataLen;			//size of the data pointed by pImagedata.either strlen(pImageData)+1 (dataType=='file') or
								//the length of the image data block
	int topN;					//the number of best matches to be returned
	float lowest_score;
}get_top_n_matches_param_t;

typedef struct{
	char		collectionID[MAX_STRING_ID_LENGTH];		//ID of the data collection. It's possible to return matched images from multiple collections
	char 		docID[MAX_STRING_ID_LENGTH];			//Unique ID of the matched document
	int			pageNum;								//Unique ID of the specific page within the docucment
	float		matchingScore;						    //recognition confidence
//	double		transformMatrix[3][3];					//the homographic transform matrix from the query image to the index image
}match_result_t;

typedef struct{
	int			nMatched;
	char		errMsg[MAX_ERR_STRING_LENGTH];
	match_result_t * pMatchList;
}get_top_n_matches_result_item_t;




#define MAX_FEATURELEN	        128
#define MAX_ATTACHED            16
typedef struct{
	unsigned short      featurelen;
	unsigned short      attachInfolen;
	float	            featurevector[MAX_FEATURELEN];
	float               attachedInfo[MAX_ATTACHED];
}feature_t;

/*
#define APPICON_FEATURE_DIM     108
typedef struct{
	char	            v[APPICON_FEATURE_DIM];
}featurevector_t;
*/

typedef int (*reco_dllapi_batch_start)(const char* configFilefolder, const char* workRootPath);
typedef void (*reco_dllapi_batch_close)();
typedef reco_instance_handle (*reco_dllapi_get_instance_handle)(const char* collectionName);
typedef int (*reco_dllapi_add_instance)(const char* configFilePath, const char * workingPath);
typedef int (*reco_dllapi_instance_remove)(const char* collectionName);
typedef int (*reco_dllapi_get_active_collections)(char*** names);
typedef int (*reco_dllapi_instance_add_image)( const reco_instance_handle instance, const get_top_n_matches_param_t * param);


//**************************************************************************************************
//Name		:  	reco_dllapi_new_instance
//Function	:	create an instance of image recognizer which implements the specified algorithm
//Return	:	a handle to the created instance. 0 for failed call
//Note		:
//***************************************************************************************************
typedef reco_instance_handle (*reco_dllapi_new_instance)(const char * configFile);


//**************************************************************************************************
//Name		:  	reco_dllapi_instance_init
//Function	:	initialize an image recognizer instance
//Return	:	#successfully loaded collection, -1 for error
//Note		:
//***************************************************************************************************
typedef int (*reco_dllapi_instance_init)(	const reco_instance_handle instance,
                                            const char * workRootPath);				//In: where log file exists



//**************************************************************************************************
//Name		:  	reco_dllapi_instance_close
//Function	:	release an image recognizer instance
//Return	:	0 success, -1 failure
//Note		:
//***************************************************************************************************
typedef int (*reco_dllapi_instance_close)(	const reco_instance_handle instance	);


//**************************************************************************************************
//Name		:  	reco_dllapi_instance_update_index
//Function	:	update index for an image collection
//Return	:	0 success, -1 failure
//Note		:
//***************************************************************************************************
typedef int (*reco_dllapi_instance_update_index)(
			const reco_instance_handle instance,	//in: the instance returned by reco_dllapi_new_instance
			update_index_param_t * param);			//in: not used any more


typedef int (*reco_dllapi_instance_get_features)(
            const reco_instance_handle instance, 		//In: the instance returned by reco_dllapi_new_instance
			const get_top_n_matches_param_t * param,	//In: input info: imageName, buffer, etc
            feature_t** pFeatures);


typedef int (*reco_dllapi_instance_search_features)(
            const   reco_instance_handle instance, 		//In: the instance returned by reco_dllapi_new_instance
			const   get_top_n_matches_param_t * param,
            const   feature_t * pFeatures,              //In: array of query image features
            int     nfeat,                              //In: count of query image features
            get_top_n_matches_result_item_t * result); 	//In/Out: address of an array to receive the matching result items



#endif //__NGX_HTTP_IPIOVE_IPIOVE_RECO_DLL_H_INCLUDED__
