//
//  main.cpp
//  serverWork
//
//  Created by Normal on 15/10/26.
//  Copyright © 2015年 wwwbbat. All rights reserved.
//

#include<iostream>
#include<string>
#include<string.h>
#include<stdio.h>
#include<time.h>
#include <fcntl.h>
// #include "include/gzip/gzip.h"
#include <curl/curl.h>
#include "json.h"

//#include "include/json/json.h"
#include <zlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#define kLogFilePath "/Users/Documents/GidHub/serverWork/serverWork/files/updateLog_v4.txt"
//#define kLogFilePath "/root/openressty/nginx/html/download/updateLog_v_3_0.txt"

#define kItemFilePath "/Users/Documents/GidHub/serverWork/serverWork/files/item_v4.txt"
//#define kItemFilePath "/root/openressty/nginx/html/download/item_v_3_0.txt"

#define kItemZipFilePath "/Users/Documents/GidHub/serverWork/serverWork/files/item_v4.zip"
//#define kItemZipFilePath "/root/openressty/nginx/html/download/item_v_3_0.zip"

#define kItemUpdateTimeFilePath "/Users/Documents/GidHub/serverWork/serverWork/files/updateTime_v4.txt"
//#define kItemUpdateTimeFilePath "/root/openressty/nginx/html/updateTime/updatetime_v_3_0.txt"


using namespace std;

size_t writer(void* buffer, size_t size, size_t nmemb, void* lpVoid);
bool  CurlInit(CURL *&curl, const char* url,string &content);
bool  GetURLDataBycurl(const char* URL, string &content);
void downloadWillBegin(const char* URL, string &content);								//启动下载流程
bool down_file(string &str);

long beginGZip(string &content, long length);

int aCount = 0;

int main()
{
    while(1)
    {
        std::string schemaUrlText;
        if(down_file(schemaUrlText)){
            cout<<schemaUrlText<<endl;
            
            Json::Reader reader;
            Json::Value value;
        
            if(reader.parse(schemaUrlText, value))
            {
                const Json::Value result = value["result"];
                if(!result.isNull())
                {
                    std::string items_game_url = result["items_game_url"].asString();
                    cout<<items_game_url<<endl;string content;
                    downloadWillBegin(items_game_url.c_str(),content);
                    sleep(60*60*5); //5小时后再次执行
                }
            }
            
    				}else{
                        printf("errorrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr");
                    }
    }
    return 0;
}

bool down_file(string &str)
{
    CURL *curl = NULL;
    CURLcode code;
    const char *filename = "http://api.steampowered.com/IEconItems_570/GetSchemaURL/v1/?key=CD9010FD71FA1583192F9BDB87ED8164";
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, filename); //设置下载地址
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);//设置超时时间
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);//设置写数据的函数
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&str);//设置写数据的变量
    code = curl_easy_perform(curl);//执行下载
    if(CURLE_OK != code) {
        cout<<"successed"<<endl;
        return false;//判断是否下载成功
    }
    return true;
}

void downloadWillBegin(const char* url,string &content)
{
    if ( GetURLDataBycurl(url,content))
    {
        cout<<"______________________:"<<content.length()<<endl;
        aCount = 0;
    }else{
        cout<<"——————————————download failed——————————"<<endl;
        aCount++;
        if(aCount!=5)
        {
            downloadWillBegin(url,content);
            cout<<"----retry count： "<<aCount<<"----------"<<endl;
        }else
        {
            cout<<"-----------wait 5 hours-------------"<<endl;
            
        }
    }
}

int downloadProgressFunc(char *progress_data,
                         double t, /* dltotal */
                         double d, /* dlnow */
                         double ultotal,
                         double ulnow)
{
    printf("%s %g / %g (%g %%)\n", progress_data, d, t, d*100.0/t);
    return 0;
}

bool CurlInit(CURL *&curl, const char* url,string &content)
{
    CURLcode code;
    string error;
    curl = curl_easy_init();
    if (curl == NULL)
    {
        printf( "Failed to create CURL connection\n");
        return false;
    }
    curl_easy_setopt(curl,CURLOPT_FILETIME,1L);
    code = curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error.c_str());
    if (code != CURLE_OK)
    {
        printf( "Failed to set error buffer [%d]\n", code );
        return false;
    }
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
    code = curl_easy_setopt(curl, CURLOPT_URL, url);
    if (code != CURLE_OK)
    {
        printf("Failed to set URL [%s]\n", error.c_str());
        return false;
    }
    code = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
    if (code != CURLE_OK)
    {
        printf( "Failed to set redirect option [%s]\n", error.c_str() );
        return false;
    }
    code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
    if (code != CURLE_OK)
    {
        printf( "Failed to set writer [%s]\n", error.c_str());
        return false;
    }
    code = curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&content);
    if (code != CURLE_OK)
    {
        printf( "Failed to set write data [%s]\n", error.c_str() );
        return false;
    }
    code = curl_easy_setopt(curl, CURLOPT_TIMEOUT,60*30);
    if (code != CURLE_OK)
    {
        printf( "Failed to set timeout: [%s]\n", error.c_str() );
        return false;
    }
    code = curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT,60);
    if (code != CURLE_OK)
    {
        printf( "Failed to set connect timeout: [%s]\n", error.c_str() );
        return false;
    }
    code = curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION,downloadProgressFunc);
    if (code != CURLE_OK)
    {
        printf( "Failed to set progress func: [%s]\n", error.c_str() );
        return false;
    }
    
    return true;
}

size_t writer(void* buffer, size_t size, size_t nmemb, void* lpVoid)
{
    std::string* str = dynamic_cast<std::string*>((std::string *)lpVoid);
    if( NULL == str || NULL == buffer )
    {
        return -1;
    }
    
    char* pData = (char*)buffer;
    str->append(pData, size * nmemb);
    return nmemb;
}

bool GetURLDataBycurl(const char* URL,  string &content)
{
    CURL *curl = NULL;
    CURLcode code;
    string error;
    
    code = curl_global_init(CURL_GLOBAL_DEFAULT);
    if (code != CURLE_OK)
    {
        printf( "Failed to global init default [%d]\n", code );
        return false;
    }
    
    if ( !CurlInit(curl,URL,content) )
    {
        printf( "Failed to global init default [%d]\n" );
        return false;
    }
    
    code = curl_easy_perform(curl);
    
    if (code != CURLE_OK)
    {
        printf( "Failed to get '%s' [%s]\n", URL, error.c_str());
        return false;
    }
    
    long retcode = 0;
    
    code = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE , &retcode);
    
    if ( (code == CURLE_OK) && retcode == 200 )
    {
        double length = 0;
        code = curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD , &length);
        long filetime;
        curl_easy_getinfo(curl, CURLINFO_FILETIME , &filetime);
        long len = content.length();
        
        printf("File time:     %ld \n",filetime);
        printf("Resopnse code:  %ld\n",retcode);
        printf("Responser len:    %f\n",length);
        printf("Download len:    %ld\n",len);
        time_t timep;
        time (&timep);
        printf("Check Update time: %s",ctime(&timep));
        
        FILE *updateLogFile = fopen(kLogFilePath,"a+");
        if(updateLogFile){
            fprintf(updateLogFile,"%s fileTime:%ld--responseCode:%ld--responseLen:%f--DownloadLen:%ld\n\n",ctime(&timep),filetime,retcode,length,len);
            fclose(updateLogFile);
        }
        
        if((long)length - len == 0)
        {
            printf("prepare save item.txt");
            
            FILE * file = fopen(kItemFilePath,"wb");
            if (!file) {
                perror("fopen");
                return -1;
            }
            fseek(file,0,SEEK_SET);
            size_t writedSize = fwrite(content.c_str(),1,length,file);
            cout<<"item.txt write size: "<<writedSize<<endl;
            fclose(file);
            
            long zipsize = beginGZip(content,length);
            
            file = fopen(kItemUpdateTimeFilePath,"wb");
            fseek(file,0,SEEK_SET);
            fprintf(file,"%ld-%ld",filetime,zipsize);
            fclose(file);
            printf("File did saved %ld\n",zipsize);
            curl_easy_cleanup(curl);
            return true;
        }else {
            printf("length - len != 0");
            curl_easy_cleanup(curl);
            return false;
        }
    }
    else
    {
        curl_easy_cleanup(curl);
        printf("download failed！ response code：  %ld\n",retcode);
        return false;
    }
    return false;
}

long beginGZip(string &content, long length)
{
    const char *buf = content.c_str();
    struct stat statf;
    gzFile gz_file;
    char * dstf = kItemZipFilePath;
    char * srcf = kItemFilePath;
    char * istart;
    int ifd;
    int rtn;
    
    if (stat(srcf, &statf) != 0)
        
        goto failed;
    
    ifd = open(srcf, O_RDONLY);
    
    if (ifd == -1)
        
        goto failed;
    
    istart = (char *)mmap(NULL, statf.st_size, PROT_READ, MAP_SHARED, ifd, 0);
    
    if (istart != MAP_FAILED){
        
        gz_file = gzopen(dstf, "wb");
        
        if (gz_file != NULL){
            
            rtn = gzwrite(gz_file, istart, statf.st_size);
            gzclose(gz_file);
            
            FILE *gzfile = fopen(dstf, "r");
            fseek(gzfile,0,SEEK_END);
            long longBytes=ftell(gzfile);
            printf("gz_file size: %ld\n", longBytes);
            
            if (rtn == statf.st_size){
                
                printf("compress the file %s ok\n", srcf);
                
                munmap(istart, statf.st_size);
                
                close(ifd);
                
                return longBytes;
                
            }else
                return -1;
        }
        
    }
    
    
    close(ifd);
    
failed:
    
    printf("compress the file %s failed\n", srcf);
    
    return -1;
}

