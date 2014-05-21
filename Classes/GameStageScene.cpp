#include "GameStageScene.h"
#include "SimpleAudioEngine.h"
#include "StageClearScene.h"

USING_NS_CC;

#define IMAGE_TILE_NORAML "YELLOW.png"
#define IMAGE_TILE_SELECTED "MAGENTA.png"
#define IMAGE_BG_CURRENT_STAGE "GREEN_CIRCLE.png"
#define SPEED_FOR_FLIP 0.05
#define SPEED_FOR_FLIP_DELAY 0.3
#define MSG_PLAY_AGAIN "play-again"


#define TAG_BUTTON_CURRENT_STAGE_BG 500
#define TAG_BUTTON_OPTION_BUTTON 501
#define TAG_BUTTON_SHARE_BUTTON 502
#define TAG_BUTTON_RETRY_BUTTON 503
#define TAG_BUTTON_OPTION_BGM_BUTTON 504
#define TAG_BUTTON_OPTION_EFFECT_BUTTON 505

#define Z_ORDER_POPUP 1000
#define Z_ORDER_POPUP_ICON 1001
#define Z_ORDER_POPUP_LABEL 1002
#define NUM_POPUP_MENU 4
#define NUM_SHARE_MENU 3
#define NUM_OPTION_MENU 2
#define GAME_MAIN_FONT_NAME "font_bold.ttf"

Scene* GameStageScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = GameStageScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}


bool GameStageScene::init()
{
    if (!LayerColor::initWithColor(Color4B(120,180,180,255))) {
        return false;
    }
    
    winSize = Director::getInstance()->getWinSize();
    
    bestStage = 1;
    loadBestStage();
	
    initSound();
    initBoard();
    initStageButtonInfo();
    initMenuPopup();
    initTimerLabel();
    
    explosion_col[0] = Color4F(255,0,0,1);
    explosion_col[1] = Color4F(0,255,0,1);
    explosion_col[2] = Color4F(0,0,255,1);
    explosion_col[3] = Color4F(255,127,0,1);
    explosion_col[4] = Color4F(255,255,0,1);
    explosion_col[5] = Color4F(0,255,255,1);
    explosion_col[6] = Color4F(255,0,255,1);
    
	mCurrentLevel = 1;
    isPopupShowing = false;
    tileTouchEnable = false;
	eventDispatcher = _eventDispatcher;
    addButtonEventListener(eventDispatcher);
    gameStart(0);
    
    return true;
}

void GameStageScene::initSound() {
    
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("firework.mp3");
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("es042.wav");
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("ticktock.wav");
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("pageFlip.mp3");
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("applause.mp3");
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("gameFail.wav");
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("stageBtClick.wav");
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("failFirst.wav");
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadBackgroundMusic("bgMusic.wav");
    CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("bgMusic.wav", true);
}

void GameStageScene::initBoard() {
    
    bgImage = Sprite::create("stage_bg.png");
    bgImage->setPosition(Point(winSize.width/2, winSize.height/2));
    this->addChild(bgImage, 0);
    
    int MARGIN_BOTTOM;
    if (winSize.height > 1000) {
        // 3.5 inch
        MARGIN_BOTTOM = winSize.height * 0.3;
    } else {
        // 4 inch
        MARGIN_BOTTOM = winSize.height * 0.2;
    }
    
    //boardLayer = NodeGrid::create();
	boardLayer = Layer::create();
	boardLayer->setPosition(0, MARGIN_BOTTOM);
    boardLayer->setContentSize(Size(winSize.width, winSize.width));
	boardLayer->setColor(Color3B(255, 0, 0));
    
	this->addChild(boardLayer, 1);
}

void GameStageScene::initStageButtonInfo() {
    bgCurrentStage = Sprite::create(IMAGE_BG_CURRENT_STAGE);
    bgCurrentStage->setPosition(Point(winSize.width/2, winSize.height * 0.2));
    bgCurrentStage->setTag(TAG_BUTTON_CURRENT_STAGE_BG);
    this->addChild(bgCurrentStage, 0);
    
    char stageInfo[256];
    std::sprintf(stageInfo, "%d", mCurrentLevel);
    currentStage = LabelTTF::create(stageInfo, GAME_MAIN_FONT_NAME, 80);
    currentStage->setPosition(Point(winSize.width/2, winSize.height * 0.2));
    this->addChild(currentStage, 1);
}

void GameStageScene::initMenuPopup() {
    
    pauseLayout = Sprite::create("popupBg.png", Rect(0, 0, winSize.width, winSize.height));
    pauseLayout->setPosition(Point(winSize.width/2, winSize.height/2));
    pauseLayout->setVisible(false);
    this->addChild(pauseLayout, Z_ORDER_POPUP);
    
    auto stageLabel = LabelTTF::create("Stage", GAME_MAIN_FONT_NAME, 30.0f);
    stageLabel->setPosition(Point(winSize.width/2, winSize.height * 0.3f));
    pauseLayout->addChild(stageLabel, Z_ORDER_POPUP_LABEL);
    
    // Menu item Layout
    popMenuImage[0] = Sprite::create("RecordIcon.png");
    popMenuImage[1] = Sprite::create("OptionIcon.png");
    popMenuImage[2] = Sprite::create("ShareIcon.png");
    popMenuImage[3] = Sprite::create("RetryIcon.png");
    shareImage[0] = Sprite::create("FacebookIcon.png");
    shareImage[1] = Sprite::create("MailIcon.png");
    shareImage[2] = Sprite::create("ReviewIcon.png");
    optionImage[0] = Sprite::create("BGMonIcon.png");
    optionImage[1] = Sprite::create("EffectOnIcon.png");
    //optionImage[2] = Sprite::create("BGMoffIcon.png");
    //optionImage[3] = Sprite::create("EffectOffIcon.png");
    
    float popX[NUM_POPUP_MENU] = {winSize.width/2, winSize.width*0.125f, winSize.width*0.875f,winSize.width*0.766f};
    float popY[NUM_POPUP_MENU] = {winSize.height/2, winSize.height*0.912f, winSize.height*0.912f,winSize.height*0.2f};
    float shareX[NUM_SHARE_MENU] = {winSize.width*0.875f, winSize.width*0.875f, winSize.width*0.875f};
    float shareY[NUM_SHARE_MENU] = {winSize.height*0.797f, winSize.height*0.710f, winSize.height*0.621f};
    float optionX[NUM_OPTION_MENU] = {winSize.width*0.125f, winSize.width*0.125f};
    float optionY[NUM_OPTION_MENU] = {winSize.height*0.797f, winSize.height*0.710f};
    
    for (int i=0; i<NUM_POPUP_MENU; i++) {
        popMenuImage[i]->setScale(0.7, 0.7);
        popMenuImage[i]->setPosition(Point(popX[i],popY[i]));
    }
    popMenuImage[0]->setScale(0.8, 0.8);
    
    for (int i=0; i<NUM_SHARE_MENU; i++) {
        shareImage[i]->setScale(0.5, 0.5);
        shareImage[i]->setPosition(Point(shareX[i],shareY[i]));
        shareImage[i]->setVisible(false);
    }
    
    for (int i=0; i<NUM_OPTION_MENU; i++) {
        optionImage[i]->setScale(0.5, 0.5);
        optionImage[i]->setPosition(Point(optionX[i],optionY[i]));
        optionImage[i]->setVisible(false);
    }
    
    for (int i=0; i<NUM_POPUP_MENU; i++) {
        pauseLayout->addChild(popMenuImage[i], Z_ORDER_POPUP);
    }
    
    for (int i=0; i<NUM_SHARE_MENU; i++) {
        pauseLayout->addChild(shareImage[i], Z_ORDER_POPUP);
    }
    
    for (int i=0; i<NUM_OPTION_MENU; i++) {
        pauseLayout->addChild(optionImage[i], Z_ORDER_POPUP);
    }
    
    popMenuImage[1]->setTag(TAG_BUTTON_OPTION_BUTTON);
    popMenuImage[2]->setTag(TAG_BUTTON_SHARE_BUTTON);
    popMenuImage[3]->setTag(TAG_BUTTON_RETRY_BUTTON);
    
    optionImage[0]->setTag(TAG_BUTTON_OPTION_BGM_BUTTON);
    optionImage[1]->setTag(TAG_BUTTON_OPTION_EFFECT_BUTTON);
    
    // BEST STAGE
    char best[128];
    sprintf(best, "%d", bestStage);
    bestStageLabel = LabelTTF::create(best, GAME_MAIN_FONT_NAME, 50.0f);
    bestStageLabel->setPosition(Point(winSize.width/2, winSize.height * 0.535));
    pauseLayout->addChild(bestStageLabel, Z_ORDER_POPUP_LABEL);
}

void GameStageScene::gameStart(float dt) {
    
	mTiles.clear();
	mTilesSelected.clear();
	getStageInfo();
    
    drawCurrentStageInfo();
    drawBoard();
    drawTiles();
}

void GameStageScene::drawTiles() {
    
    boardLayer->removeAllChildren();
    
    const int BOARD_SIZE = info.matrixSize;
    for (int n=0; n<BOARD_SIZE; n++) {
		for (int m=0; m<BOARD_SIZE; m++) {
            
			int idx = n * BOARD_SIZE + m;
            
			auto tile = Sprite::create(IMAGE_TILE_NORAML);
			tile->setPosition(winSize.width/(BOARD_SIZE+1)*(m+1), winSize.width/(BOARD_SIZE+1)*(n+1));
            float tileScale;
            if (info.matrixSize == 2) {
                tileScale = 1.8f;
            } else {
                tileScale = 4.0f / (float)info.matrixSize;
            }
            tile->setScale(tileScale);
			tile->setTag(idx);
			boardLayer->addChild(tile, 10);
            
			mTiles.push_back(tile);
			mTilesSelected.push_back(0);
		}
	}
    addTileButtonEventListener();
}

void GameStageScene::drawCurrentStageInfo() {
    char stageInfo[3];
    std::sprintf(stageInfo, "%d", mCurrentLevel);
    currentStage->setString(stageInfo);
}

void GameStageScene::getStageInfo() {
    info = matrix.getStageInfo(mCurrentLevel);
    log("action Num: %d, level: %d, matrixSize: %d",info.actionNum, info.level, info.matrixSize);
}

void GameStageScene::drawBoard() {
	
	//// ADD //////////
	int MARGIN_BOTTOM;
    if (winSize.height > 1000) {
        // 3.5 inch
        MARGIN_BOTTOM = winSize.height * 0.3;
    } else {
        // 4 inch
        MARGIN_BOTTOM = winSize.height * 0.2;
    }

	if (boardLayer != NULL) {
		boardLayer->removeFromParent();
	}
	boardLayer = Layer::create();
	boardLayer->setPosition(0, MARGIN_BOTTOM);
    boardLayer->setContentSize(Size(winSize.width, winSize.width));
	boardLayer->setColor(Color3B(255, 0, 0));
    
	this->addChild(boardLayer, 1);

	scheduleOnce(schedule_selector(GameStageScene::showTiles), 0.05);

	float delayTime = 1.5f * (float)info.matrixSize / 3.0f;
    float hideTime = 1.0f * (float)info.matrixSize / 3.0f;
    const float delayForRotationAnimation = 0.8f;
    const float delayForFlipAnimation = 1.5f;
    const float delayAnimationMargin = 0.5f;
    const float animationTime = (delayForRotationAnimation + delayForFlipAnimation) / 2
                                + delayAnimationMargin * info.actionNum;
    
	scheduleOnce(schedule_selector(GameStageScene::hideTiles), delayTime);
    scheduleOnce(schedule_selector(GameStageScene::runTimer), delayTime + animationTime + hideTime);
    
    FiniteTimeAction* action1 = NULL;
    FiniteTimeAction* action2 = NULL;
    FiniteTimeAction* action3 = NULL;
    FiniteTimeAction* action4 = NULL;
    FiniteTimeAction* action5 = NULL;
    
    auto delay_interval = DelayTime::create(0.4);

	float orbitOrigin = 0;

	for (int t=0; t<info.actionNum; t++) {
        ActionInterval* action;
        
		switch(info.actions[t]) {
		//switch(test[t]) {
		case TRANSFORM_FLIP_X:
			action = OrbitCamera::create(delayForFlipAnimation, 1, 0, orbitOrigin, 180, 0, 0);
			orbitOrigin = (orbitOrigin == 0) ? 180 : 0; 
			break;
		case TRANSFORM_FLIP_Y:
            action = OrbitCamera::create(delayForFlipAnimation, 1, 0, orbitOrigin, 180, 0,0);
			orbitOrigin = (orbitOrigin == 0) ? 180 : 0;
			break;
		case TRANSFORM_ROT_CW:
            action = RotateBy::create(delayForRotationAnimation, 90);
			break;
		case TRANSFORM_ROT_CCW:
            action = RotateBy::create(delayForRotationAnimation, -90);
			break;
		case TRANSFORM_TRANS1:
            action = RotateBy::create(delayForRotationAnimation, 90);
			log("trans 1");
			break;
		case TRANSFORM_TRANS2:
            //action = OrbitCamera::create(delayForFlipAnimation, 1, 0, 0, 180, 0, 0);
            action = RotateBy::create(delayForRotationAnimation, -90);
            log("trans 2");
			break;
		default:
			break;
		}

        action = EaseBackOut::create(action);
        
        if (t==0) {
            action1 = action;
        } else if(t==1) {
            action2 = action;
        } else if(t==2) {
            action3 = action;
        } else if (t==3) {
            action4 = action;
        } else if (t==4) {
            action5 = action;
        }
	}
    
    auto delay = DelayTime::create(delayTime + hideTime);
    boardLayer->runAction(Sequence::create(delay, action1, delay_interval, action2, delay_interval, action3, delay_interval, action4, delay_interval, action5, NULL));
    
}

void GameStageScene::showTiles(float dt) {
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("pageFlip.mp3");
	for (int n=0; n<mTiles.size(); n++) {

		float delayTime = n * SPEED_FOR_FLIP;
		float flipTime = SPEED_FOR_FLIP_DELAY;

		auto delay1 = DelayTime::create(delayTime);
		auto delay2 = DelayTime::create(flipTime / 2);

		auto flipActions = Sequence::create(delay1, OrbitCamera::create(flipTime, 1, 0, 0, -180, 0, 0), NULL);
		auto changeTile = CallFunc::create( CC_CALLBACK_0(GameStageScene::drawSolutionTiles, this, n) );

		mTiles.at(n)->runAction(flipActions);
		if (info.before[n] == 1) {
			mTiles.at(n)->runAction(Sequence::create(delay1, delay2, changeTile, NULL));
		}	
	}
}

void GameStageScene::hideTiles(float dt) {
	for (int n=0; n<mTiles.size(); n++) {

		float delayTime = n * SPEED_FOR_FLIP;
		float flipTime = SPEED_FOR_FLIP_DELAY;


		auto delay1 = DelayTime::create(delayTime);
		auto delay2 = DelayTime::create(flipTime / 2);

		auto flipActions = Sequence::create(delay1, OrbitCamera::create(flipTime, 1, 0, 0, 180, 0, 0), NULL);
		auto changeTile = CallFunc::create( CC_CALLBACK_0(GameStageScene::drawBoardTiles, this, n) );

		mTiles.at(n)->runAction(flipActions);
		if (info.before[n] == 1) {
			mTiles.at(n)->runAction(Sequence::create(delay1, delay2, changeTile, NULL));	
		}
	}
}

void GameStageScene::drawSolutionTiles(int n) {
	mTiles.at(n)->setTexture(IMAGE_TILE_SELECTED);
}

void GameStageScene::drawBoardTiles(int n) {
	mTiles.at(n)->setTexture(IMAGE_TILE_NORAML);
}

void GameStageScene::addButtonEventListener(EventDispatcher* e) {

    buttonListener = EventListenerTouchOneByOne::create();
    buttonListener->setSwallowTouches(false);
    
    buttonListener->onTouchBegan = [=](Touch* touch, Event* event){
        
        auto target = event->getCurrentTarget();
        int tagNum = target->getTag();
        
        Point locationInNode = target->convertToNodeSpace(touch->getLocation());
        Size s = target->getContentSize();
        const float MARGIN_TOUCH_AREA = target->getContentSize().width * 0.1;
        Rect rect = Rect(-MARGIN_TOUCH_AREA, -MARGIN_TOUCH_AREA, s.width + MARGIN_TOUCH_AREA, s.height + MARGIN_TOUCH_AREA);
        
        if (rect.containsPoint(locationInNode)) {
            auto scaledown = ScaleBy::create(0.1f, 0.9f);
            auto scaleup = ScaleBy::create(0.1f, 1.1f);
            auto scaleupdowon = RepeatForever::create(Sequence::create(scaleup, scaledown, NULL));
            switch (tagNum) {
                case TAG_BUTTON_CURRENT_STAGE_BG:
                {
                    bgCurrentStage->setScale(1.3f);
                    bgCurrentStage->runAction(scaleupdowon);
                }
                    break;
                case TAG_BUTTON_RETRY_BUTTON:
                {
                    float currentScale = popMenuImage[3]->getScale();
                    popMenuImage[3]->setScale(currentScale * 1.3);
                    //popMenuImage[3]->runAction(scaleupdowon);
                }
                    break;
                case TAG_BUTTON_OPTION_BUTTON:
                {
                    popMenuImage[1]->setScale(popMenuImage[1]->getScale() * 1.2);
                }
                    break;
                case TAG_BUTTON_SHARE_BUTTON:
                {
                    popMenuImage[2]->setScale(popMenuImage[2]->getScale() * 1.2);
                }
                    break;
                case TAG_BUTTON_OPTION_BGM_BUTTON:
                {
                    optionImage[0]->setScale(optionImage[0]->getScale() * 1.2);
                    
                }
                    break;
                case TAG_BUTTON_OPTION_EFFECT_BUTTON:
                {
                    optionImage[1]->setScale(optionImage[1]->getScale() * 1.2);
                }
                    break;
            }
            return true;
            
        }
        return false;
    };
    
    buttonListener->onTouchMoved = [](Touch* touch, Event* event){
        // nothing..
    };
    
    buttonListener->onTouchEnded = [=](Touch* touch, Event* event){
        auto target = event->getCurrentTarget();
		int tagNum = target->getTag();
 
        if (tagNum > mTiles.size()) {
            
            switch (tagNum) {
                case TAG_BUTTON_CURRENT_STAGE_BG:
                    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("stageBtClick.wav");
                    bgCurrentStage->stopAllActions();
                    bgCurrentStage->setScale(1.0f);
                    
                    if (isPopupShowing == false) {
                        showMenuPopup(0);
                    } else {
                        hideMenuPopup();
                    }
                    
                    if (isGameFinished == true) {
                        isGameFinished = false;
                        timerLabel->setString("");
                        gameStart(0);
                    }

                    break;
                        
                case TAG_BUTTON_RETRY_BUTTON:
                    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("stageBtClick.wav");
                    popMenuImage[3]->stopAllActions();
                    popMenuImage[3]->setScale(0.7f);
                    isGameFinished = false;
                    timerLabel->setString("");
                    mCurrentLevel = 1;

                    unschedule(schedule_selector(GameStageScene::showTiles));
                    unschedule(schedule_selector(GameStageScene::hideTiles));
                    unschedule(schedule_selector(GameStageScene::runTimer));
                    unschedule(schedule_selector(GameStageScene::drawTimerLabel));
                        
                        
                    gameStart(0);
                    hideMenuPopup();
      
                    break;
                        
                case TAG_BUTTON_OPTION_BUTTON:
                {
                    float currentScale1 = popMenuImage[1]->getScale();
                    popMenuImage[1]->setScale(currentScale1 / 1.2);
                    
                    if (optionImage[0]->isVisible() == true) {
                        for (int i=0; i<NUM_OPTION_MENU; i++) {
                            optionImage[i]->setVisible(false);
                        }
                    } else {
                        for (int i=0; i<NUM_OPTION_MENU; i++) {
                            optionImage[i]->setVisible(true);
                        }
                    }
                }
                    break;
                    
                case TAG_BUTTON_SHARE_BUTTON:
                {
                    float currentScale2 = popMenuImage[2]->getScale();
                    popMenuImage[2]->setScale(currentScale2 / 1.2);
                    
                    if (shareImage[0]->isVisible() == true) {
                        for (int i=0; i<NUM_SHARE_MENU; i++) {
                            shareImage[i]->setVisible(false);
                        }
                    } else {
                        for (int i=0; i<NUM_SHARE_MENU; i++) {
                            shareImage[i]->setVisible(true);
                        }
                    }
                }
                    break;
                case TAG_BUTTON_OPTION_BGM_BUTTON:
                {
                    optionImage[0]->setScale(optionImage[0]->getScale() / 1.2);
                    if (CocosDenshion::SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying()) {
                        CocosDenshion::SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
                        optionImage[0]->setTexture("BGMoffIcon.png");
                        
                    } else {
                        CocosDenshion::SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
                        optionImage[0]->setTexture("BGMonIcon.png");
                        
                    }
                    
                }
                    break;
                case TAG_BUTTON_OPTION_EFFECT_BUTTON:
                {
                    optionImage[1]->setScale(optionImage[1]->getScale() / 1.2);
                }
                    break;
                    
                default:
                break;
            }
            
            return;
        }
    };
    
    e->addEventListenerWithSceneGraphPriority(buttonListener->clone(), bgCurrentStage);
    e->addEventListenerWithSceneGraphPriority(buttonListener->clone(), currentStage);
    e->addEventListenerWithSceneGraphPriority(buttonListener->clone(), popMenuImage[1]);
    e->addEventListenerWithSceneGraphPriority(buttonListener->clone(), popMenuImage[2]);
    e->addEventListenerWithSceneGraphPriority(buttonListener->clone(), popMenuImage[3]);
    e->addEventListenerWithSceneGraphPriority(buttonListener->clone(), optionImage[0]);
    e->addEventListenerWithSceneGraphPriority(buttonListener->clone(), optionImage[1]);
    
}

void GameStageScene::addTileButtonEventListener() {
    auto tileButtonListener = EventListenerTouchOneByOne::create();
    tileButtonListener->setSwallowTouches(false);
    
    tileButtonListener->onTouchBegan = [=](Touch* touch, Event* event){
        
        auto target = event->getCurrentTarget();
        int tagNum = target->getTag();
        
        Point locationInNode = target->convertToNodeSpace(touch->getLocation());
        Size s = target->getContentSize();
        const float MARGIN_TOUCH_AREA = target->getContentSize().width * 0.1;
        Rect rect = Rect(-MARGIN_TOUCH_AREA, -MARGIN_TOUCH_AREA, s.width + MARGIN_TOUCH_AREA, s.height + MARGIN_TOUCH_AREA);
        
        if (rect.containsPoint(locationInNode)) {
            
            if (tileTouchEnable == true && isPopupShowing == false) {
                float currentScale = mTiles[tagNum]->getScale();
                mTiles[tagNum]->setScale(currentScale * 1.1);
                
                return true;
            }
            
            
        }
        return false;
    };
    
    tileButtonListener->onTouchMoved = [](Touch* touch, Event* event){
        // nothing..
    };
    
    tileButtonListener->onTouchEnded = [=](Touch* touch, Event* event){
        auto target = event->getCurrentTarget();
		int tagNum = target->getTag();
        
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("es042.wav");
        
		mTilesSelected[tagNum] = (mTilesSelected[tagNum] + 1) % 2;
		std::string image = mTilesSelected[tagNum] == 0 ? IMAGE_TILE_NORAML : IMAGE_TILE_SELECTED;
		mTiles[tagNum]->setTexture(image);
        float currentScale = mTiles[tagNum]->getScale();
        mTiles[tagNum]->setScale(currentScale / 1.1);
        //flower(touch->getLocation());
        
		int count = 0;
		for (int i=0; i<mTiles.size(); i++) {
			if (info.before[i] == mTilesSelected.at(i)) {
				count++;
			}
		}
        if (count == mTilesSelected.size()) {
			log("stage clear!");
            stageClear();
		}
    };
    
    for (int n=0; n<mTiles.size(); n++) {
        eventDispatcher->addEventListenerWithSceneGraphPriority(tileButtonListener->clone(), mTiles.at(n));
    }
    
}

void GameStageScene::stageClear() {
    
    if (mCurrentLevel > bestStage) {
        bestStage = mCurrentLevel;
        writeBestStage();
    }
    
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("applause.mp3");
    unschedule(schedule_selector(GameStageScene::drawTimerLabel));
    tileTouchEnable = false;
    mCurrentLevel += 1;
    timerLabel->setString("");
    
    auto scaleUp = ScaleBy::create(0.4, 1.2);
    auto scaleDown = scaleUp->reverse();
    bgCurrentStage->runAction(Sequence::create(scaleUp, scaleDown, scaleUp, scaleDown, scaleUp, scaleDown, NULL));
    
    float dt1 = (float)(rand()%5)/100.0;
    float dt2 = (float)(rand()%5)/100.0;
    
    int lotto = rand()%2;
    if (lotto == 1) {
        dt1 = dt1 * -1;
        dt2 = dt2 * -1;
    }
    
    auto delay1_exp = DelayTime::create(0.2 + dt1);
    auto delay2_exp = DelayTime::create(0.5 + dt2);
    
    float dx = (float)(rand()%8)/100.0;
    float dy = (float)(rand()%8)/100.0;
    
    auto exp1 = CallFunc::create( CC_CALLBACK_0(GameStageScene::explosion, this, Point(winSize.width*(0.20f+dx),winSize.height*(0.59f+dy))) );
    auto exp2 = CallFunc::create( CC_CALLBACK_0(GameStageScene::explosion, this, Point(winSize.width*(0.78f+dx),winSize.height*(0.55f+dy))) );
    auto exp3 = CallFunc::create( CC_CALLBACK_0(GameStageScene::explosion, this, Point(winSize.width*(0.55f+dx),winSize.height*(0.80f+dy))) );
    this->runAction(Sequence::create(exp1, delay1_exp, exp2, delay2_exp, exp3, NULL));
    
    scheduleOnce(schedule_selector(GameStageScene::gameStart), 3);
}

void GameStageScene::explosion(Point s) {
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("firework.mp3");
	ParticleSystem *particle = ParticleExplosion::createWithTotalParticles(200);
	particle->setTexture(Director::getInstance()->getTextureCache()->addImage("fire.png"));
	particle->setPosition(s);
	particle->setGravity(Point(0, -70));
    particle->setRadialAccel(1);
    particle->setLife(0.1);
    particle->setDuration(0.2);
    particle->setSpeed(270);
    particle->setTangentialAccel(10);
    //particle->setEmissionRate(1300);
    //particle->setStartColor(Color4F(255, 0, 0, 255));
    int lotto = rand()%7;
    particle->setStartColor(explosion_col[lotto]);
    particle->setStartSize(15);
	particle->setEndColor(Color4F(0,0,0,1));
    particle->setEndSize(0.0);
	
    this->addChild(particle,100);
}

void GameStageScene::flower(Point s)
{
    ParticleSystem *emitter = ParticleFlower::create();
    emitter->setTexture(Director::getInstance()->getTextureCache()->addImage("stars.png"));
    emitter->setPosition(s);
    emitter->setLife(0.1);
    emitter->setDuration(0.5);
    emitter->setEmissionRate(100);
    emitter->setSpeed(100);

    this->addChild(emitter,101);
}

void GameStageScene::effectShowSolution(Point s)
{
    ParticleSystem *emitter = ParticleFlower::create();
    emitter->setTexture(Director::getInstance()->getTextureCache()->addImage("stars.png"));
    emitter->setPosition(s);
    
    switch (info.matrixSize) {
        case 2:
            emitter->setSpeed(110);
            break;
        case 3:
            emitter->setSpeed(95);
            break;
        case 4:
            emitter->setSpeed(85);
            break;
        case 5:
            emitter->setSpeed(70);
            break;
        case 6:
            emitter->setSpeed(65);
            break;
        default:
            emitter->setSpeed(80);
            break;
    }
    
    boardLayer->addChild(emitter,0);
}

void GameStageScene::initTimerLabel() {
    timerLabel = LabelTTF::create("", GAME_MAIN_FONT_NAME, 80.0f);
    timerLabel->setPosition(Point(winSize.width/2, winSize.height * 0.9));
    this->addChild(timerLabel);
}

void GameStageScene::runTimer(float dt) {

    timerLabel->setString("Ready!");
    timerLabel->runAction(ScaleBy::create(0.3, 1.3f));

    timerCount = info.TimeLimit;
    schedule(schedule_selector(GameStageScene::drawTimerLabel), 1.0f);
    
}

void GameStageScene::drawTimerLabel(float dt) {
    
    if (timerCount < 0) {
        
        unschedule(schedule_selector(GameStageScene::drawTimerLabel));
        
        timerLabel->setScale(1.0f);
        timerLabel->runAction(ScaleBy::create(0.5, 1.2f));
        tileTouchEnable = false;
        
        for (int i=0; i<mTiles.size(); i++) {
            Point pt;
            if (info.before[i] == 1) {
                pt = mTiles.at(i)->getPosition();
                effectShowSolution(pt);
            }
        }
        
        // GAME OVER
        timerLabel->setString("Game Over!");
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("gameFail.wav");
        isGameFinished = true;
        scheduleOnce(schedule_selector(GameStageScene::showMenuPopup), 3.0f);

    } else {
        if (timerCount < 3) {
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("ticktock.wav");
            auto jump = JumpBy::create(0.25, Point(0, 0), 20, 1);
            boardLayer->runAction(Sequence::create(jump, jump, NULL));
            
        }
        char buffer[2];
        std::sprintf(buffer, "%d", timerCount--);
        timerLabel->setScale(1.0f);
        timerLabel->setString(buffer);
        timerLabel->runAction(ScaleBy::create(0.5, 1.5f));
        tileTouchEnable = true;
    }
}

void GameStageScene::showMenuPopup(float dt) {
    isPopupShowing = true;
    char best[128];
    sprintf(best, "%d", bestStage);
    bestStageLabel->setString(best);
    pauseLayout->setVisible(isPopupShowing);
    this->reorderChild(bgCurrentStage, Z_ORDER_POPUP_ICON);
    this->reorderChild(currentStage, Z_ORDER_POPUP_LABEL);
}

void GameStageScene::hideMenuPopup() {
    isPopupShowing = false;
    pauseLayout->setVisible(isPopupShowing);
    this->reorderChild(bgCurrentStage, 0);
    this->reorderChild(currentStage, 1);

}

void GameStageScene::writeBestStage()
{
    auto sharedFileUtils = FileUtils::getInstance();
    
    std::string ret;
    
    sharedFileUtils->purgeCachedEntries();
    _defaultSearchPathArray = sharedFileUtils->getSearchPaths();
    std::vector<std::string> searchPaths = _defaultSearchPathArray;
    std::string writablePath = sharedFileUtils->getWritablePath();
    std::string fileName = writablePath+"best_stage.txt";
    log("path : %s",writablePath.c_str());
    char szBuf[100];
    sprintf(szBuf, "%d", bestStage);
    FILE* fp = fopen(fileName.c_str(), "wb");
    if (fp)
    {
        size_t ret = fwrite(szBuf, 1, strlen(szBuf), fp);
        CCASSERT(ret != 0, "fwrite function returned zero value");
        fclose(fp);
        if (ret != 0) {
            log("succeed.");
        } else {
            log("fail2");
        }
    } else {
        log("writing file is failed");
    }
}

void GameStageScene::loadBestStage()
{
    auto sharedFileUtils = FileUtils::getInstance();
    
    std::string ret;
    
    std::string writablePath = sharedFileUtils->getWritablePath();
    //std::string fullPath = sharedFileUtils->fullPathForFilename("best_stage.txt");
    std::string fileName = writablePath+"best_stage.txt";
    log("external file path = %s", fileName.c_str());
    FILE* fp;
    
    if (fileName.length() > 0)
    {
        fp = fopen(fileName.c_str(), "rb");

        if (fp) {
            char szReadBuf[100] = {0};
            char szBuf[100];
            int read = fread(szReadBuf, 1, strlen(szBuf), fp);
            if (read > 0) {
                //log("The content of file from writable path: %s", szReadBuf);
                bestStage = std::atoi(szReadBuf);
                log("best stage : %d",bestStage);
            }
            fclose(fp);
        } else {
            fclose(fp);
            log("no file , create");
            writeBestStage();
        }
    }
}
