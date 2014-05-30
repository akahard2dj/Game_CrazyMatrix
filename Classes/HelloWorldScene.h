#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

USING_NS_CC;

class HelloWorld : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
    CREATE_FUNC(HelloWorld);
    
    //Variables
    cocos2d::EventListenerTouchOneByOne* listener;
    int nPages;
    Sprite* snapshot_1;
    Sprite* snapshot_2;
    Sprite* snapshot_3;
    Sprite* snapshot_4;
    Point onTouch;
    Point offTouch;
    
    //Functions
    void guide(int nPages);
    bool checkFileStatus();
    void writeFile();
    
    virtual bool init();
    virtual bool onTouchBegan(Touch* touch, Event* unused_event);
    virtual void onTouchMoved(Touch* touch, Event* unused_event);
    virtual void onTouchCancelled(Touch* touch, Event* unused_event);
    virtual void onTouchEnded(Touch* touch, Event *unused_event);
};


#endif // __HELLOWORLD_SCENE_H__
