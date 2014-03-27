//
//  CurlDown.h
//  muli_download
//
//  Created by vin on 14-3-24.
//
//

#ifndef __muli_download__CurlDown__
#define __muli_download__CurlDown__

#include <iostream>
#include "cocos2d.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#include "../cocos2dx/platform/third_party/win32/curl/curl.h"
#else
#include "curl/curl.h"
#endif
#include <stdio.h>
using namespace cocos2d;
using namespace std;
USING_NS_CC;

class CurlDownDelegate;

class CurlDown : public CCObject {
private:
    string mFileName; // 下载文件名称
    bool isStop;
public:
    ~CurlDown();
    CurlDown();
    CurlDown(string downUrl,string filePath);
    
    string mFilePath; // 本地存储地址
    double mFileLenth; // 下载文件大小
    string mDownloadUrl; // 下载URL
    long timeout; // 请求超时时间 为了测试用 设置超时时间为2秒 如果是发正式版本 改为20秒超时时间
    bool download(); // 下载方法
    long getDownloadFileLenth(); // 下载文件大小方法
    void downloadControler(); // 下载控制方法
    long getLocalFileLength(); // 当前下载文件大小
    
    void setStopDown();// 停止下载
    void setDelegate(CurlDownDelegate * delegate);
    CurlDownDelegate *mDelegate;
    
    
    enum ErrorCode
    {
        // Error caused by creating a file to store downloaded data
        kCreateFile,
        /** Error caused by network
         -- network unavaivable
         -- timeout
         -- ...
         */
        kNetwork,
        /** There is not a new version
         */
        kNoNewVersion,
        /** Error caused in uncompressing stage
         -- can not open zip file
         -- can not read file global information
         -- can not read file information
         -- can not create a directory
         -- ...
         */
        kUncompress,
    };
};

class CurlDownDelegate
{
public:
    /* @brief Call back function for error
     @param errorCode Type of error
     */
    virtual void onError(CurlDown::ErrorCode errorCode) {};
    /** @brief Call back function for recording downloading percent
     @param percent How much percent downloaded
     @warn This call back function just for recording downloading percent.
     AssetsManager will do some other thing after downloading, you should
     write code in onSuccess() after downloading.
     */
    virtual void onProgress(double percent, void *delegate, string filefullPath) {};
    /** @brief Call back function for success
     */
    virtual void onSuccess(string filefullPath) {};
};

#endif /* defined(__muli_download__CurlDown__) */
