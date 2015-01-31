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
	//define an streamstring to store data
	stringstream ssRecordData;
	//uint64       startTime = NOW_TIME;//start time of test//avoid third part lib
	time_t       startTime;
	time(&startTime);

	DIR* 			dirInfo;
	struct dirent * dirEntry;
	void  * pImgBuf	= new char[BUF_LEN];

    //int top1_hit = 0, topN_hit = 0;num_noReturn = 0;

    //the number of query images that exist a match,and the number of query images that there is no match
    int num_exist = 0+1, num_nomatch = 0+1;//positive and negative--incase the denominator equal 0
    int num_top1_precog = 0, num_top1_nreject = 0;//p=positve-----n=negative----means exist match and no match
    int num_topN_precog = 0, num_topN_nreject = 0;
    //int num_exist_top1_hit = 0, num_exist_top5_hit = 0;
    //int num_nomatch_top1_hit = 0, num_nomatch_top5_hit = 0;

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
					++num_exist;
				else{
					noMatch = true;
					++num_nomatch;
					
				}

				ssRecordData << queryImgID << "\t";//record query image name

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
                ssRecordData << (int)difftime(et, st) << "\t";//record time per image cost in recognition and search
                //ssRecordData << NOW_TIME - st << "\t";//record time per image cost in recognition and search

				//find no matches
                if(result.nMatched == 0) {
                	if(noMatch){
                		++num_top1_nreject;
                		++num_topN_nreject;
                	}
                    //num_noReturn ++;
                }
				else if( !noMatch && (result.nMatched>0) ){//positive sample
						COPY(queryFilePath, string(resultDir)+queryImgName);//move query image to result folder

						bool topNIncd = false;

						for(int i=0; i<result.nMatched; i++){
							string docID(result.pMatchList[i].docID);//理解为匹配的图像名

							ssRecordData << result.pMatchList[i].matchingScore << "\t";//top i score
							if(!queryImgID.compare(docID)){
								if(0 == i)
									++num_top1_precog;
								if(!topNIncd){
									++num_topN_precog;
									topNIncd = true;
								}

								ssRecordData << "True" << "\t";//correctly matched
							}
							else{
								ssRecordData << "False" << "\t";
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
				ssRecordData << "\n";//start to record next query

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
	saveFile << "\t correct recognize \t correct reject \t false positive \t false negative" << "\n";
	saveFile << "top1  " << float(num_top1_precog)/num_exist << "\t" << float(num_top1_nreject)/num_nomatch << "\t"
						 << float(num_nomatch - num_top1_nreject)/num_nomatch << "\t" << float(num_exist - num_top1_precog)/num_exist << "\n";
	saveFile << "top5  " << float(num_topN_precog)/num_exist << "\t" << float(num_topN_nreject)/num_nomatch << "\t"
						 << float(num_nomatch - num_topN_nreject)/num_nomatch << "\t" << float(num_exist - num_topN_precog)/num_exist << "\n";
	saveFile << "ImageID \t cost time \t top1_score \t top1_result \t top2_score \t top2_result \t top3_score \t top3_result \t top4_score \t top4_result \t top5_score \t top5_result";
	saveFile << ssRecordData;
	saveFile << "------------------------------------------------------------------------------------";

	saveFile.close();
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
