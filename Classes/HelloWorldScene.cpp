#include "HelloWorldScene.h"
USING_NS_CC;

using namespace cocos2d;

static pthread_mutex_t g_downloadMutex;


CCScene* HelloWorld::scene() {
    CCScene *scene = CCScene::create();
    HelloWorld *layer = HelloWorld::create();
    scene->addChild(layer);
    return scene;
}

bool HelloWorld::init() {
    if ( !CCLayer::init() ) {
        return false;
    }
    CCSize size = CCDirector::sharedDirector()->getWinSize();
    
    CCMenu* pMenu = CCMenu::create();
    pMenu->setPosition( CCPointZero );
    this->addChild(pMenu, 1);
    //    down start
    CCMenuItemFont * btnDownStart = CCMenuItemFont::create("down start", this, menu_selector(HelloWorld::menuCallback));
    btnDownStart->setPosition(ccp(size.width / 2 - 200, size.height/2 + 50));
    btnDownStart->setTag(1);
    pMenu->addChild(btnDownStart);
    //    down stop
    CCMenuItemFont * btnDownStop = CCMenuItemFont::create("down stop", this, menu_selector(HelloWorld::menuCallback));
    btnDownStop->setPosition(ccp(size.width / 2 - 200, size.height/2 - 50));
    btnDownStop->setTag(2);
    pMenu->addChild(btnDownStop);
    //    down stop
    CCMenuItemFont * btnDownFileDel = CCMenuItemFont::create("down file del", this, menu_selector(HelloWorld::menuCallback));
    btnDownFileDel->setPosition(ccp(size.width / 2 - 200, size.height/2 - 150));
    btnDownFileDel->setTag(3);
    pMenu->addChild(btnDownFileDel);
    //    down Progress
    lblDownProgress = CCLabelTTF::create("down Progress", "Thonburi", 34);
    lblDownProgress->setPosition( ccp(size.width / 2 + 100, size.height/2) );
    lblDownProgress->setAnchorPoint(ccp(0.5, 0.5));
    this->addChild(lblDownProgress, 2);
    downFilePath = CCFileUtils::sharedFileUtils()->getWritablePath();
    CCLog("path:%s",(CCFileUtils::sharedFileUtils()->getWritablePath()+"dafa.zip").c_str());
    return true;
}

void HelloWorld::menuCallback(CCObject* pSender) {
    CCMenuItem *item = (CCMenuItem *)pSender;
    switch (item->getTag()) {
        case 1: // down start
            downFilePath = CCFileUtils::sharedFileUtils()->getWritablePath();
            CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(HelloWorld::updateUI), this, 0, false); // HttpClient中参考
            isStop = false;
            this->threadStart();
            break;
        case 2: // down stop
            isStop = true;
            break;
        case 3:
            if (isStop) {
                CCLog("downFilePath:%s",downFilePath.c_str());
                if (access(downFilePath.c_str(), 0) == 0) {
                    remove(downFilePath.c_str());
                    CCMessageBox("删除成功", "温馨提示");
                }else{
                    CCMessageBox("没有找到文件目录", "温馨提示");
                }
            }else{
                CCMessageBox("下载中或没有文件下载", "温馨提示");
            }
            
            break;
        default:
            break;
    }
}

//int ii =0;
void HelloWorld::updateUI(){
    if(updateStr.empty())return;
    lblDownProgress->setString(updateStr.c_str());
    if (updateStr == "success") {
        isStop = true;
        CCDirector::sharedDirector()->getScheduler()->unscheduleSelector(schedule_selector(HelloWorld::updateUI), this);
    }
    //    CCLog("--progress:%d-",ii++);
}

// 启动线程的方法
int HelloWorld::threadStart() {
    pthread_mutex_init(&g_downloadMutex, NULL);
    int errCode=0;
    pthread_t th_curlDown; // 线程初始化
    do {
        pthread_attr_t tAttr;
        errCode=pthread_attr_init(&tAttr);
        CC_BREAK_IF(errCode!=0);
        errCode=pthread_attr_setdetachstate(&tAttr, PTHREAD_CREATE_DETACHED);
        if(errCode!=0) {
            pthread_attr_destroy(&tAttr);
            break;
        }
        errCode=pthread_create(&th_curlDown, &tAttr, thread_funcation, this);
    } while (0);
    return errCode;
}

// 需要线程来完成的功能都写在这个函数里
void* HelloWorld::thread_funcation(void *arg) {
    CCLOG("thread started...");
    HelloWorld *hw = (HelloWorld*)arg;
    hw->ccc = new CurlDown("http://developer.baidu.com/map/static/doc/output/BaiduMap_AndroidSDK_v2.4.0_All.zip",hw->downFilePath);
    //    ccc->mDownloadUrl = "http://developer.baidu.com/map/static/doc/output/BaiduMap_AndroidSDK_v2.4.0_All.zip";
    //    int leng = ccc->getDownloadFileLenth();
    hw->ccc->setDelegate(hw);
    hw->ccc->downloadControler();
    
    return NULL;
}

void HelloWorld::onError(CurlDown::ErrorCode errorCode){
    CCLog("error");
    
    pthread_mutex_lock(&g_downloadMutex);
    updateStr = "error";
    pthread_mutex_unlock(&g_downloadMutex);
    
    CCDirector::sharedDirector()->getScheduler()->unscheduleSelector(schedule_selector(HelloWorld::updateUI), this);
}
void HelloWorld::onProgress(double percent, void *delegate, string filefullPath){ // 下载进度
    CCLog("donw progress:%.2f%%",percent);
    
    if (isStop) {
        CurlDown * cd = (CurlDown *)delegate;
        //        pthread_mutex_lock(&g_downloadMutex);
        cd->setStopDown();
        //        pthread_mutex_unlock(&g_downloadMutex);
    }
    
    pthread_mutex_lock(&g_downloadMutex);
    const char * per =CCString::createWithFormat("donw progress:%.2f%%",percent)->getCString();
    updateStr = per;
    downFilePath = filefullPath;
    pthread_mutex_unlock(&g_downloadMutex);
}
void HelloWorld::onSuccess(string filefullPath){
    CCLog("success");
    
    pthread_mutex_lock(&g_downloadMutex);
    updateStr = "success";
    downFilePath = filefullPath;
    pthread_mutex_unlock(&g_downloadMutex);
}

void HelloWorld::menuCloseCallback(CCObject* pSender)
{
    CCDirector::sharedDirector()->end();
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
