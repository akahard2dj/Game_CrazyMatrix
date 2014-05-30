#include "AppDelegate.h"
#include "GameStageScene.h"
#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

#define GAME_PREF_FILE_NAME "game.plist"

USING_NS_CC;

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
        glview = GLView::create("My Game");
        director->setOpenGLView(glview);
    }

    // turn on display FPS
    director->setDisplayStats(false);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);

    // create a scene. it's an autorelease object
    //auto scene = GameStageScene::createScene();
    Scene* scene;
    bool status = checkFileStatus();
    if (status == true) {
        scene = GameStageScene::createScene();
    }
    if (status == false) {
        writeFile();
        scene = HelloWorld::createScene();
    }
    
    // run
    director->runWithScene(scene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    CocosDenshion::SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    CocosDenshion::SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
}

void AppDelegate::writeFile()
{
	auto rootDic = Dictionary::create();
	auto bestStageInteger = Integer::create(1);
	rootDic->setObject(bestStageInteger, "key_bestStage");
    
	std::string writablePath = FileUtils::getInstance()->getWritablePath();
    std::string fullPath = writablePath + GAME_PREF_FILE_NAME;
    if (rootDic->writeToFile(fullPath.c_str())) {
        log("write succeed at %s", fullPath.c_str());
	} else {
        log("oops! write failed");
	}
	
}

bool AppDelegate::checkFileStatus()
{
	std::string writablePath = FileUtils::getInstance()->getWritablePath();
    std::string fullPath = writablePath + GAME_PREF_FILE_NAME;
    
    return FileUtils::getInstance()->isFileExist(fullPath);
}
