#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <ctime>

#include <opencv2/opencv.hpp>

//#include "util/timeUtils.h"
//#include "indexer/index_exported_interface.h"
#include "recog_exported_interface.h"
#include "ngx_http_ipiove_reco_dll.h"
//#include "util/paramMgr.h"
//#include "util/dirUtils.h"


using namespace std;
using namespace cv;

#define BUF_LEN 	    (5*1024*1024)
#define TOPN		    5
#define LOWEST_SCORE    1


string rootDir = "/home/wangbs/code/";

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


void COPY(string src, string dest){
	Mat img = imread(src);
	imwrite(dest, img); 
}


int g_nTotalQuery = 0;

void batch_query(	const char* queryImageDir,
					const string& trainImageDir,
				 	reco_instance_handle& h,
				 	int topN,
				 	const char* resultDir){
	#if PRINT_EVERY_DETAIL
	//open a file and save data in
	fstream saveFileD;
	string filePathD = "test_detail.txt";
	saveFileD.open(filePathD.c_str(), fstream::out | fstream::app);
	if(!saveFileD.is_open()){
		cout << "open file failed!" << endl;
		return;
	}
	//define an streamstring to store data
	stringstream ssRecordData;
	saveFileD << "ImageID \t cost time \t top1_score \t top1_result \t top2_score \t top2_result \t top3_score \t top3_result \t top4_score \t top4_result \t top5_score \t top5_result \n";
	saveFileD << ssRecordData;
	//saveFileD.close();
	#endif

	//uint64       startTime = NOW_TIME;//start time of test//avoid third part lib
	time_t       startTime;
	time(&startTime);

	DIR* 			dirInfo;
	struct dirent * dirEntry;
	void  * pImgBuf	= new char[BUF_LEN];

    //int top1_hit = 0, topN_hit = 0;num_noReturn = 0;

    //the number of query images that exist a match,and the number of query images that there is no match
    int inlier = 0+1, outlier = 0+1;//positive and negative--incase the denominator equal 0
    int num_top1_inrecg = 0, num_outreject = 0;//p=positve-----n=negative----means exist match and no match
    int num_topN_inrecg = 0;
    //int inlier_top1_hit = 0, inlier_top5_hit = 0;
    //int outlier_top1_hit = 0, outlier_top5_hit = 0;

	try{
		dirInfo	= opendir(queryImageDir);
		if( NULL == dirInfo ) return ;

		while( ( dirEntry = readdir(dirInfo) )!= NULL ){
			if( !strcmp(dirEntry->d_name,".") || !strcmp(dirEntry->d_name,"..") ) continue;
			try{
				string queryImgName	=  string(dirEntry->d_name); //cout << queryImgName << endl;
				//query image ID
				string queryImgID   =  queryImgName.substr(queryImgName.find("=")+1, queryImgName.find("."));
				bool noMatch = false;
				//if the image name string contain substring "nomatch" that means there is no match image in training set
				if(queryImgID.find("nomatch") == string::npos)
					++inlier;
				else{
					noMatch = true;
					++outlier;
					
				}

				#if PRINT_EVERY_DETAIL
				ssRecordData << queryImgID << "\t";//record query image name
				#endif

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

				//uint64 st = NOW_TIME;//avoid third part library
				time_t st;
				time(&st);
				get_top_n_matches_result_item_t	result;
				result.nMatched = 0;
				result.pMatchList = 0;

				feature_t* pFeatures = NULL;
				int nFeature = recognizer_dllapi_instance_get_features(h, &p,  &pFeatures);
                recognizer_dllapi_instance_search_features(h, &p, pFeatures, nFeature, &result);
                if(pFeatures) delete pFeatures;

                //record time in second, save in  txt
                time_t et;
                time(&et);
                #if PRINT_EVERY_DETAIL
                ssRecordData << (int)difftime(et, st) << "\t";//record time per image cost in recognition and search
                //ssRecordData << NOW_TIME - st << "\t";//record time per image cost in recognition and search
                #endif

				//find no matches
                if(result.nMatched == 0) {
                	if(noMatch){
                		++num_outreject;
                	}
                    //num_noReturn ++;
                }
				else if( !noMatch && (result.nMatched>0) ){//positive sample
						COPY(queryFilePath, string(resultDir)+queryImgName);//move query image to result folder

						bool topNIncd = false;

						for(int i=0; i<result.nMatched; i++){
							string docID(result.pMatchList[i].docID);//理解为匹配的图像名

							#if PRINT_EVERY_DETAIL
							ssRecordData << result.pMatchList[i].matchingScore << "\t";//top i score
							#endif

							if(!queryImgID.compare(docID)){
								if(0 == i)
									++num_top1_inrecg;
								if(!topNIncd){
									++num_topN_inrecg;
									topNIncd = true;
								}
								#if PRINT_EVERY_DETAIL
								ssRecordData << "True" << "\t";//correctly matched
								#endif
							}
							else{
								#if PRINT_EVERY_DETAIL
								ssRecordData << "False" << "\t";
								#endif
							}

							string trainImgName = docID + ".jpg";

							stringstream ss;
							ss.clear();

							ss<< queryImgID << "_" << i << "_" << result.pMatchList[i].matchingScore << "_"  << trainImgName;
							string tmp = ss.str();
							for(size_t k = 0; k<tmp.size(); k++) if(tmp[k] == '/') tmp[k] = '_';
							COPY(string(trainImageDir) + trainImgName, string(resultDir) + tmp);
						}

						delete []result.pMatchList;
				}//end if
				#if PRINT_EVERY_DETAIL
				ssRecordData << "\n";//start to record next query
				
				saveFileD << ssRecordData;
				//saveFileD.close();

				ssRecordData.clear();
				ssRecordData.str() == "";
				#endif

				g_nTotalQuery ++;

			}catch(...){}
		}// end while
	}catch(...){}


	//uint64 testTime = NOW_TIME - startTime;//avoid third part library
	time_t  endTime;
	time(&endTime);
	uint64 testTime = (int)difftime(endTime, startTime);
	//open a file and save data in
	fstream saveFile;
	//string filePath = resultDir + queryImgID + ".txt";
	string filePath = "test.txt";
	saveFile.open(filePath.c_str(), fstream::out | fstream::app);
	if(!saveFile.is_open()){
		cout << "open file failed!" << endl;
		return;
	}

	saveFile << "Test Time               :\t" << testTime << "\n";
	saveFile << "Training data directory :\t" << trainImageDir << "\n";
	saveFile << "Testing  data directory :\t" << queryImageDir << "\n";
	saveFile << "\t total recognize rate\t inlier recognize rate \t outlier reject rate" << "\n";
	saveFile << "top1  " << float( num_top1_inrecg + num_outreject )/( inlier + outlier ) << "\t"
						 << float( num_top1_inrecg )/inlier << "\t" << float( num_outreject )/outlier << "\n";
	saveFile << "top5  " << float( num_topN_inrecg + num_outreject )/( inlier + outlier ) << "\t"
						 << float( num_topN_inrecg )/inlier << "\t" << float( num_outreject )/outlier << "\n";
	
	saveFile << "------------------------------------------------------------------------------------\n";

	saveFile.close();
	#if PRINT_EVERY_DETAIL
	saveFileD.close();
	#endif
}


int main (int argc, char *argv[]){

   if( argc > 1 ){
        rootDir = string( argv[1] );
    }

    //string configfolder   =  rootDir+ "config/";
    //string rootworkingDir =  rootDir+ "log/";

    printf("query test start ...");

    vector<string> collectionNames;//什么数据据。训练？
    collectionNames.push_back("ReadBoy");

    string queryImgfolder = rootDir+ "queryImg/";
    string trainImgfolder = rootDir+ "trainImg/";
    string queryRstfolder = rootDir+ "result/";
    for(unsigned int i=0; i<collectionNames.size(); i++){
        reco_instance_handle  h = recognizer_dllapi_get_instance_handle(collectionNames[i].c_str());
      
        batch_query( queryImgfolder.c_str(),
        			 trainImgfolder + collectionNames[i] + "/",
                     h,
                     TOPN,
                     (queryRstfolder + "/").c_str());
    }


    //recognizer_dllapi_batch_close();


    return 0;
}
