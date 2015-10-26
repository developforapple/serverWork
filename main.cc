#include<iostream>
#include<string>
#include<stdio.h>
#include<time.h>
#include <curl/curl.h>
#include "include/json/json.h"

using namespace std;

size_t writer(void* buffer, size_t size, size_t nmemb, void* lpVoid);
bool  CurlInit(CURL *&curl, const char* url,string &content);
bool  GetURLDataBycurl(const char* URL, string &content);
void downloadWillBegin(const char* URL,string &content);								//启动下载流程
bool down_file(string &str);
int count = 0;
time_t beginTime;

int main()
{
    const char *url = "http://cdn.dota2.com/apps/570/scripts/items/items_game.855dcf274b57a3f547cd360bff166ef38e5218cf.txt";
    string urlStr("");
    while(1)
    {
        string schemaUrlText;
        if(down_file(schemaUrlText)){
            cout<<schemaUrlText<<endl;
            
            Json::Reader reader;
            Json::Value value;
            if(reader.parse(schemaUrlText, value))
            {
                if(!value["result"].isNull())
                {
                    urlStr = value["result"]["items_game_url"].asString();
                    
                    cout<<urlStr<<endl;string content;
                    downloadWillBegin(urlStr.c_str(),content);
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
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30);//设置超时时间
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
    time(&beginTime);
    
    if ( GetURLDataBycurl(url,content))
    {
        cout<<"______________________:"<<content.length()<<endl;
        count = 0;
    }else{
        cout<<"——————————————download failed——————————"<<endl;
        count++;
        if(count!=5)
        {
            downloadWillBegin(url,content);
            cout<<"----retry count： "<<count<<"----------"<<endl;
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
    time_t currentTime;
    time(&currentTime);
    printf("%s %g / %g (%g %%) time: %lds \n", progress_data, d, t, d*100.0/t,currentTime - beginTime);
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


/*long writer(void *data, int size, int nmemb, string &content)
 {
 long sizes = size * nmemb;
 string temp(data,sizes);
 content += temp;
 return sizes;
 }*/

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
        printf("Resopnse code:  %d\n",retcode);
        printf("Responser len:    %f\n",length);
        printf("Download len:    %ld\n",len);
        time_t timep;
        time (&timep);
        printf("Check Update time: %s",ctime(&timep));
        
        FILE *updateLogFile = fopen("/root/openressty/nginx/html/download/updateLog.txt","a");
        if(updateLogFile){
            fprintf(updateLogFile,"---------------------------------------------\n%s fileTime:%ld--responseCode:%d--responseLen:%f--DownloadLen:%ld\n\n",ctime(&timep),filetime,retcode,length,len);
            fclose(updateLogFile);
        }
        
        if((long)length - len == 0)
        {
            printf("prepare save item.txt");
            
            FILE * file = fopen("/root/openressty/nginx/html/download/item.txt","wb");
            if (!file) {
                perror("fopen");
                return -1;
            }
            fseek(file,0,SEEK_SET);
            size_t writedSize = fwrite(content.c_str(),1,length,file);
            cout<<"item.txt write size: "<<writedSize<<endl;
            fclose(file);
            
            file = fopen("/root/openressty/nginx/html/updateTime/updatetime.txt","wb");
            fseek(file,0,SEEK_SET);
            fprintf(file,"%ld-%ld",filetime,len);
            fclose(file);
            printf("File did saved\n");
            curl_easy_cleanup(curl);
            return true;
        }else {
            printf("!!!!!!!!!!!");
            curl_easy_cleanup(curl);
            return false;
        }
        //struct curl_slist *list;
        //code = curl_easy_getinfo(curl,CURLINFO_COOKIELIST,&list);
        //curl_slist_free_all (list);
        
    }
    else
    {
        curl_easy_cleanup(curl);
        printf("download failed！ response code：  %d\n",retcode);
        return false;
    }
    return false;
}