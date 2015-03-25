#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>

#include <opencv2/opencv.hpp>

#include "util/timeUtils.h"
#include "indexer/index_exported_interface.h"
#include "recognizer/recog_exported_interface.h"
#include "recognizer/ngx_http_ipiove_reco_dll.h"
#include "util/paramMgr.h"
#include "util/dirUtils.h"


using namespace std;
using namespace cv;

#define BUF_LEN 	    (5*1024*1024)
#define TOPN		    3
#define LOWEST_SCORE    1


//string rootDir = "/media/vol2/projects/oppoAR_inuse/Data/forest/";
string rootDir = "/home/liaocy/Projects/test-data/";

int loadImage2Buf( const string& queryFilePath, void * pImgBuf, int bufLen ){

	FILE * pFile	= fopen( queryFilePath.c_str(), "rb");

	if( pFile == NULL ) return 0;

	fseek( pFile,0, SEEK_END );
	long size	= ftell( pFile );
	if( size <= bufLen ){
		fseek( pFile, 0, SEEK_SET);
		size = fread(pImgBuf, 1, size, pFile);
	}
	else
		size = 0;

	fclose(pFile);

	return size;
}


int g_nTotalQuery = 0;

// for local test on a single recognizer
void batch_query(	const char* queryImageDir,
					const string& trainImageDir,
				 	reco_instance_handle& h,
				 	int topN,
				 	const char* resultDir){

	DIR* 			dirInfo;
	struct dirent * dirEntry;
	void  * pImgBuf	= new char[BUF_LEN];

    int top1_hit = 0, topN_hit = 0, num_noReturn = 0;
    MKDIR(string(resultDir) + "exist");
    string failfolder = string(resultDir) + "fail";
    MKDIR(failfolder);
	try{
		dirInfo	= opendir(queryImageDir);
		if( NULL == dirInfo ) return ;

		while( ( dirEntry = readdir(dirInfo) )!= NULL ){
			if( !strcmp(dirEntry->d_name,".") || !strcmp(dirEntry->d_name,"..") ) continue;
			try{
				string queryImgName	=  string(dirEntry->d_name); //cout << queryImgName << endl;
				if(queryImgName.find(".jpg")  == string::npos &&
                   queryImgName.find(".JPG")  == string::npos &&
                   queryImgName.find(".jpeg") == string::npos &&
                   queryImgName.find(".JPEG") == string::npos  ) continue;

				string queryFilePath = string(queryImageDir) + queryImgName;
				get_top_n_matches_param_t p;
				p.collection	= "";
				p.algorithm		= "";
				p.topN			= TOPN;
				p.lowest_score	= LOWEST_SCORE;
				p.dataType		= "jpg";
				p.imgName       = queryImgName.c_str();
				p.pImageData	= pImgBuf;
				p.imageDataLen 	= loadImage2Buf( queryFilePath, pImgBuf, BUF_LEN );

				uint64 st = NOW_TIME;
				get_top_n_matches_result_item_t	result;
				result.nMatched = 0;
				result.pMatchList = 0;

				feature_t* pFeatures = NULL;
				int nFeature = recognizer_dllapi_instance_get_features(h, &p,  &pFeatures);
                recognizer_dllapi_instance_search_features(h, &p, pFeatures, nFeature, &result);
                if(pFeatures) delete pFeatures;

				cout << "query" << g_nTotalQuery <<" time = " << NOW_TIME - st << endl << endl;

                if(result.nMatched == 0) {
                    num_noReturn ++;
                    COPY(queryFilePath , failfolder +"/" + queryImgName);
                }
				else if( result.nMatched>0){
						string subfolder = string(resultDir) + queryImgName.substr(0,queryImgName.find("."));

						MKDIR(subfolder);
						COPY(queryFilePath, subfolder +"/0__" + queryImgName);

						string target = queryImgName.substr(queryImgName.find("=")+1);
						target = target.substr(0, target.find("."));

						for(int i=0; i<result.nMatched; i++){
							string docID(result.pMatchList[i].docID);
							//string foundName = docID.substr(0, docID.find("."));

							char buf[64];
							sprintf(buf,"%d", 0); //result.pMatchList[i].pageNum);
							string pageNum = buf;
							string trainImgName = docID.substr(0, docID.find("."))  + ".*"; // + "-" + pageNum + ".jpg"; //cout << trainImgName << endl; exit(0);

							stringstream ss;
							ss.clear();
							ss<< i+1 << "__" << "\"" << trainImgName.substr(0, trainImgName.find(".")) << "\"" << "_" << result.pMatchList[i].matchingScore <<".jpg";
							string tmp = ss.str();
							for(size_t k = 0; k<tmp.size(); k++) if(tmp[k] == '/') tmp[k] = '_';
							COPY(string(trainImageDir) + trainImgName, subfolder + "/" + tmp);
						}

						delete []result.pMatchList;
				}//end if

				g_nTotalQuery ++;

			}catch(...){}
		}// end while
	}catch(...){}

	cout << "#TotalQuery = " << g_nTotalQuery << endl
         << "#no_return = " << num_noReturn << endl
	     << "#top1_hit = " << top1_hit << ",  percentage = " << float(top1_hit)/(g_nTotalQuery - num_noReturn) << endl
	     << "#topN_hit = " << topN_hit << ",  percentage = " << float(topN_hit)/(g_nTotalQuery - num_noReturn) << endl;

}


int main (int argc, char *argv[]){

   if( argc > 1 ){
        rootDir = string( argv[1] );
    }

    string configfolder   =  rootDir+ "config/";
    string rootworkingDir =  rootDir+ "log/";

    printf("query test start ...");

    // suppose we have a list of collection names
    vector<string> collectionNames;
    collectionNames.push_back("ReadBoy");
    //collectionNames.push_back("hdsfdx");

    string queryImgfolder = rootDir+ "queryImg/";
    string trainImgfolder = rootDir+ "trainImg/";
    string queryRstfolder = rootDir+ "result/";
    for(unsigned int i=0; i<collectionNames.size(); i++){
        reco_instance_handle  h = recognizer_dllapi_get_instance_handle(collectionNames[i].c_str());
        batch_query( queryImgfolder.c_str(),
                     trainImgfolder + collectionNames[i] + "/",
                     h,
                     TOPN,
                     (queryRstfolder+collectionNames[i] + "/").c_str());

    }


    recognizer_dllapi_batch_close();


    return 0;
}



