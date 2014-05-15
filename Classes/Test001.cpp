//
//  Test001.cpp
//  CocosTestPrj
//
//  Created by 정 도원 on 2014. 5. 5..
//
//

#include "Test001.h"

bool Test001::init() {
    
    if (!Layer::create()) {
        return false;
    }
    
    // make codes...
    mRotateCnt = 1;
    mLabelCounter = 0;
    mTimerCnt = 0;
    
    drawGirl();
    drawText();
    drawButton();
    
    addEventListener(_eventDispatcher);
    return true;
}

void Test001::drawGirl() {
    mGirl = Sprite::create("grossinis_sister2.png");
    mGirl->setPosition(winSize.width/2, winSize.height * 0.75);
    mGirl->setScale(2);
    this->addChild(mGirl);
};

void Test001::drawText() {
    mLabel = LabelTTF::create("anything", "fonts/Marker Felt.ttf", 40);
    mLabel->setPosition(winSize.width/2, winSize.height * 0.9);
    this->addChild(mLabel);
    
    mLabelCounter = LabelTTF::create("0", "fonts/Marker Felt.ttf", 80);
    mLabelCounter->setPosition(winSize.width/2, winSize.height * 0.5);
    this->addChild(mLabelCounter);
}

void Test001::onMenuCallback(Ref* sender) {
    // manually
    /*char buffer[4];
    std::sprintf(buffer, "%d", ++mTimerCnt);
    mLabelCounter->setString(buffer);*/
    
    // automatically goes to 100!
    schedule(schedule_selector(Test001::timerOperation), 0.05);
}

void Test001::timerOperation(float actualInterval) {
    
    if (mTimerCnt >= 100) {
        unschedule(schedule_selector(Test001::timerOperation));
        mTimerCnt = 0;
        return;
    }
    
    char buffer[4];
    std::sprintf(buffer, "%d", ++mTimerCnt);
    
    mLabelCounter->setString(buffer);
}

void Test001::drawButton() {
    
    auto menuItem = MenuItemImage::create("btn-play-normal.png",
                                          "btn-play-selected.png",
                                          CC_CALLBACK_1(Test001::onMenuCallback, this));
    auto menu = Menu::create(menuItem, NULL);
    menu->setPosition(Point(winSize.width/2, winSize.height * 0.4));
    menuItem->setPosition(Point::ZERO);
    
    addChild(menu, 1);
}

void Test001::addEventListener(EventDispatcher* e) {

    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    
    listener->onTouchBegan = [](Touch* touch, Event* event){
        
        auto target = event->getCurrentTarget();
        Point locationInNode = target->convertToNodeSpace(touch->getLocation());
        Size s = target->getContentSize();
        Rect rect = Rect(0, 0, s.width, s.height);
        
        if (rect.containsPoint(locationInNode)) {
            log("sprite began... x = %f, y = %f", locationInNode.x, locationInNode.y);
            return true;
        }
        return false;
    };
    
    listener->onTouchMoved = [](Touch* touch, Event* event){
        // nothing..
    };
    
    listener->onTouchEnded = [=](Touch* touch, Event* event){
        auto target = event->getCurrentTarget();
        if (target == mGirl) {
            mGirl->runAction(RotateBy::create(1, 180));
            
            char buffer[10];
            std::sprintf(buffer, "Try : %d", mRotateCnt++);
            mLabel->setString(std::string(buffer));
            
            // cb
            onMenuCallback(NULL);
        }
    };
    
    e->addEventListenerWithSceneGraphPriority(listener, mGirl);
    
}

// 타 scene 혹은 app 에게 scene을 전달하기 위한 목적
cocos2d::Scene* Test001::makeScene() {
    
    log("makeScene()");
    auto scene = Scene::create();
    auto layer = Test001::create();
    scene->addChild(layer);
    
    return scene;
}
