#ifndef __GAMESTAGE_SCENE_H__
#define __GAMESTAGE_SCENE_H__

#include <vector>
#include "cocos2d.h"
#include "CCrazyMatrix.h"

USING_NS_CC;

class GameStageScene : public cocos2d::LayerColor
{
public:

	virtual bool init();  
	static cocos2d::Scene* createScene();
    CREATE_FUNC(GameStageScene);

private:
	Size winSize;
	std::vector<Sprite*> mTiles;
	std::vector<int> mTilesSelected;
	int lastActionTile;
	CCrazyMatrix matrix;
    StageInfo info;
	int mCurrentLevel;
	Layer* boardLayer;
    LabelBMFont* timerLabel;
    int timerCount;
    bool tileTouchEnable;
    Sprite* bgImage;
    Sprite* shareImage[3];
    Sprite* popMenuImage[4];
    Sprite* optionImage[2];
    LabelBMFont* currentStage;
    Sprite* bgCurrentStage;
    bool isPopupShowing;
    Sprite* pauseLayout;
    bool isGameFinished;
    Color4F explosion_col[7];
    EventDispatcher* eventDispatcher;
    EventListenerTouchOneByOne* buttonListener;
    int bestStage;
    std::vector<std::string> _defaultSearchPathArray;
    LabelTTF* bestStageLabel;
    bool isEffectSoundOn;
	bool isBGMOn;
    float iconRatio;
    float fontRatio;
    
    void initSound();
    void initBoard();
	void initStageButtonInfo();
    void initMenuPopup();
    void initTimerLabel();
    
    void gameStart(float dt);
	void getStageInfo();
	void drawBoard();
    void drawCurrentStageInfo();
    void drawTiles();
	void runTimer(float dt);
    void drawTimerLabel(float dt);
	void showTiles(float dt);
	void hideTiles(float dt);
	void drawSolutionTiles(int n);
	void drawBoardTiles(int n);
	void addButtonEventListener(EventDispatcher* e);
    void addTileButtonEventListener();
	void explosion(Point s);
    void flower(Point s);
    void effectShowSolution(Point s);
    void stageClear();
    void showMenuPopup(float dt);
    void hideMenuPopup();
    void writeBestStage();
    void loadBestStage();
    void playSoundEffect(std::string fileName);
    float fontCal(float fontSize);
    float speedCal(float speed);
};

#endif // __GAMESTAGE_SCENE_H__
