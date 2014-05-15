
#ifndef __CocosTestPrj__Test001__
#define __CocosTestPrj__Test001__

#include "cocos2d.h"
#include <iostream>

USING_NS_CC; // = using namespace cocos2d;

class Test001 : public cocos2d::Layer {
public:
    virtual bool init();
    static Scene* makeScene();
    CREATE_FUNC(Test001);
    
private:
    
    Size winSize; // = Director::getInstance()->getWinSize();
    Sprite* mGirl;
    LabelTTF* mLabel;
    LabelTTF* mLabelCounter;
    int mRotateCnt;
    int mTimerCnt;
    
    void drawGirl();
    void drawText();
    void drawButton();
    void onMenuCallback(Ref* sender);
    void timerOperation(float actualInterval);
    void addEventListener(EventDispatcher* e);
};

#endif /* defined(__CocosTestPrj__Test001__) */
