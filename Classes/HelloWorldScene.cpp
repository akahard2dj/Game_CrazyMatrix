#include "HelloWorldScene.h"
#include "GameStageScene.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    nPages = 0;
    Size winSize = Director::getInstance()->getWinSize();
    
    snapshot_1 = Sprite::create("snapshot_4_1.png");
    snapshot_2 = Sprite::create("snapshot_4_2.png");
    snapshot_3 = Sprite::create("snapshot_4_3.png");
    snapshot_4 = Sprite::create("snapshot_4_4.png");
    snapshot_1->setPosition(winSize.width/2, winSize.height/2);
    snapshot_2->setPosition(winSize.width/2, winSize.height/2);
    snapshot_3->setPosition(winSize.width/2, winSize.height/2);
    snapshot_4->setPosition(winSize.width/2, winSize.height/2);
    snapshot_1->setVisible(true);
    snapshot_2->setVisible(false);
    snapshot_3->setVisible(false);
    snapshot_4->setVisible(false);
    this->addChild(snapshot_1);
    this->addChild(snapshot_2);
    this->addChild(snapshot_3);
    this->addChild(snapshot_4);
    
    listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    
    listener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(HelloWorld::onTouchMoved, this);
    listener->onTouchCancelled = CC_CALLBACK_2(HelloWorld::onTouchCancelled, this);
    listener->onTouchEnded = CC_CALLBACK_2(HelloWorld::onTouchEnded, this);
    
    EventDispatcher* dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    return true;
}

void HelloWorld::guide(int nPages)
{
    switch(nPages) {
        case 0:
            snapshot_1->setVisible(true);
            snapshot_2->setVisible(false);
            snapshot_3->setVisible(false);
            snapshot_4->setVisible(false);
            break;
        case 1:
            snapshot_1->setVisible(false);
            snapshot_2->setVisible(true);
            snapshot_3->setVisible(false);
            snapshot_4->setVisible(false);
            break;
        case 2:
            snapshot_1->setVisible(false);
            snapshot_2->setVisible(false);
            snapshot_3->setVisible(true);
            snapshot_4->setVisible(false);
            break;
        case 3:
            snapshot_1->setVisible(false);
            snapshot_2->setVisible(false);
            snapshot_3->setVisible(false);
            snapshot_4->setVisible(true);
            break;
        default:
            break;
    }
}

bool HelloWorld::onTouchBegan(Touch* touch, Event* unused_event)
{
    onTouch = touch->getLocation();
    
    return true;
}

void HelloWorld::onTouchMoved(Touch* touch, Event* unused_event){}

void HelloWorld::onTouchCancelled(Touch* touch, Event* unused_event){}

void HelloWorld::onTouchEnded(Touch* touch, Event* unused_event)
{
    guide(nPages);
    offTouch = touch->getLocation();
    float diff = offTouch.x - onTouch.x;
    if (diff < 0)
        nPages = (nPages + 1) % 5;
    if (diff > 0)
        nPages = (nPages - 1) % 5;
    
    if (nPages < 0)
        nPages = 0;
    

    
    if (nPages == 4) {
        Scene* game = GameStageScene::createScene();
        Director::getInstance()->replaceScene(game);
    }
}