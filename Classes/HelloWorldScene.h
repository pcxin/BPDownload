#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

#include <pthread.h>
#include "CurlDown.h"

class HelloWorld : public cocos2d::CCLayer,CurlDownDelegate
{
private:
    CCLabelTTF* lblDownProgress;
    string updateStr;
    bool isStop;
public:
    CurlDown *ccc;
    virtual bool init();
    
    static cocos2d::CCScene* scene();
    
    // a selector callback
    void menuCloseCallback(CCObject* pSender);
    void menuCallback(CCObject* pSender);
    // preprocessor macro for "static create()" constructor ( node() deprecated )
    CREATE_FUNC(HelloWorld);
    
    string downFilePath;
    int threadStart();// 启动线程的方法
    static void* thread_funcation(void *arg);// 被启动的线程函数，注意必须是静态方法
    void updateUI(); // 线程任务调度方法
    
    // 断点续传 回调方法
    virtual void onError(CurlDown::ErrorCode errorCode);
    virtual void onProgress(double percent, void *delegate, string filefullPath);
    virtual void onSuccess(string filefullPath);
    
};

#endif // __HELLOWORLD_SCENE_H__
