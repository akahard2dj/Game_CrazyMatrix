//
//  StageClearScene.cpp
//  Crz Matrix
//
//  Created by 정 도원 on 2014. 5. 12..
//
//

#include "StageClearScene.h"
#include "SimpleAudioEngine.h"

cocos2d::Scene* StageClearScene::createScene() {
    
    auto scene = Scene::create();
    auto layer = StageClearScene::create();
    scene->addChild(layer);
    return scene;
    
}


bool StageClearScene::init() {
    
    if (!Layer::init()) {
        return false;
    }
    
    auto bgLayout = LayerColor::create(Color4B(30, 30, 30, 220));
    bgLayout->setPosition(Point(0, 0));
    bgLayout->setAnchorPoint(Point(0, 0));
    this->addChild(bgLayout);
    
    auto stageClearLabel = LabelTTF::create("Clear!", "arial", 60);
    stageClearLabel->setPosition(Point(winSize.width/2, winSize.height * 0.7));
    stageClearLabel->runAction(ScaleBy::create(2, 1.5f));
    this->addChild(stageClearLabel);
    
    auto delay1_exp = DelayTime::create(0.2);
    auto delay2_exp = DelayTime::create(0.5);
    auto exp1 = CallFunc::create( CC_CALLBACK_0(StageClearScene::explosionEffect, this, Point(128,672)) );
    auto exp2 = CallFunc::create( CC_CALLBACK_0(StageClearScene::explosionEffect, this, Point(530,622)) );
    auto exp3 = CallFunc::create( CC_CALLBACK_0(StageClearScene::explosionEffect, this, Point(350,912)) );
    this->runAction(Sequence::create(exp1, delay1_exp, exp2, delay2_exp, exp3, NULL));
    
    auto nextButton = MenuItemImage::create("f1.png", "f2.png", CC_CALLBACK_0(StageClearScene::playNextStage, this) );
    nextButton->setScale(2);
    auto menu = Menu::create(nextButton, NULL);
    menu->setPosition(Point(winSize.width/2, winSize.height * 0.4));
    this->addChild(menu);
    
    return true;
}

void StageClearScene::playNextStage() {
    Director::getInstance()->popScene();
}

void StageClearScene::explosionEffect(Point s) {
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("firework.mp3");
	ParticleSystem *particle = ParticleExplosion::createWithTotalParticles(200);
	particle->setTexture(Director::getInstance()->getTextureCache()->addImage("fire.png"));
	particle->setPosition(s);
	particle->setGravity(Point(0, -80));
    particle->setLife(4.5);
	particle->setEndColor(Color4F(0,0,0,1));
	
    this->addChild(particle,100);
}



