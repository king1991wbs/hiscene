#ifndef 	_RECOG_EXPORTED_INTERFACE_H_
#define		_RECOG_EXPORTED_INTERFACE_H_

#include "ngx_http_ipiove_reco_dll.h"

// exported C interfaces
extern "C" {
int recognizer_dllapi_batch_start(const char* configFilefolder,  const char* workRootPath);

void recognizer_dllapi_batch_close();

reco_instance_handle recognizer_dllapi_get_instance_handle(const char* collectionName){
      reco_instance_handle r;
      return r;
}

int recognizer_dllapi_add_instance(const char* configFilePath, const char * workingPath);

int recognizer_dllapi_instance_remove(const char* collectionName);

int recognizer_dllapi_get_active_collections(char*** names);

// for online image publish
int recognizer_dllapi_instance_add_image( const reco_instance_handle instance,
                                          const get_top_n_matches_param_t * param);


int recognizer_dllapi_instance_get_features(
            const reco_instance_handle instance, 		//In: the instance returned by reco_dllapi_new_instance
			const get_top_n_matches_param_t * param,	//In: see above
            feature_t** pFeatures
            ){
      return 3;
}

int recognizer_dllapi_instance_search_features(
            const   reco_instance_handle instance, 		//In: the instance returned by reco_dllapi_new_instance
			const   get_top_n_matches_param_t * param,
            const   feature_t * pFeatures,              //In: array of query image features
            int     nfeat,                              //In: count of query image features
            get_top_n_matches_result_item_t * result 	//In/Out: address of an array to receive the matching result items
			){
      result->nMatched = 2;
      
      result->pMatchList = new match_result_t[2];
      sprintf(result->pMatchList[0].docID,"%s","test1");
      //result->pMatchList[0].docID = "test1";
      result->pMatchList[0].matchingScore = 88;
      sprintf(result->pMatchList[1].docID,"%s","test2");
      //result->pMatchList[1].docID = "test2";
      result->pMatchList[1].matchingScore = 80;

      return 0;
}

//**************************************************************************************************
//Name		:  	reco_dllapi_new_instance
//Function	:	create an instance of image recognizer which implements the specified algorithm
//Return	:	a handle to the created instance. 0 for failed call
//Note		:
//***************************************************************************************************
reco_instance_handle recognizer_dllapi_new_instance(const char * configFile	);


//**************************************************************************************************
//Name		:  	reco_dllapi_instance_init
//Function	:	initialize an image recognizer instance
//Return	:	#successfully loaded collection, -1 for error
//Note		:
//***************************************************************************************************
int recognizer_dllapi_instance_init(const reco_instance_handle instance,
                                    const char * workRootPath);	//In: where log file exists



//**************************************************************************************************
//Name		:  	reco_dllapi_instance_close
//Function	:	release an image recognizer instance
//Return	:	0 success, -1 failure
//Note		:
//**************************************************************************************************
int recognizer_dllapi_instance_close(const reco_instance_handle instance);


//**************************************************************************************************
//Name		:  	reco_dllapi_instance_update_index
//Function	:	update index for an image collection
//Return	:	0 success, -1 failure
//Note		:
//***************************************************************************************************
int recognizer_dllapi_instance_update_index(const reco_instance_handle instance, update_index_param_t * param);
}//extern "C"


#endif
