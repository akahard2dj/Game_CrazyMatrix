#ifndef __GAMESTAGE_SCENE_H__
#define __GAMESTAGE_SCENE_H__

#include <vector>
#include "cocos2d.h"
#include "CCrazyMatrix.h"
#include "extensions/GUI/CCControlExtension/CCScale9Sprite.h"

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
    NodeGrid* nodeGrid;
    LabelTTF* timerLabel;
    int timerCount;
    bool tileTouchEnable;
    Sprite* bgImage;
    Sprite* shareImage[3];
    Sprite* popMenuImage[4];
    Sprite* optionImage[4];
    LabelTTF* currentStage;
    Sprite* bgCurrentStage;
    bool isPopupShowing;
    Sprite* pauseLayout;
    bool hasOneMoreChange;
    int wrongNumberPerGame;
    bool isGameFinished;
    Color4F explosion_col[7];
    
	void gameStart(float dt);
	void getStageInfo();
	void drawInitBoard();
	void drawBoard();
    void drawCurrentStageInfo();
	void makeTimer(float dt);
    void drawTimerLabel(float dt);
	void showTiles(float dt);
	void hideTiles(float dt);
	void drawSolutionTiles(int n);
	void drawBoardTiles(int n);
	void addEventListener(EventDispatcher* e);
	void explosion(Point s);
    void flower(Point s);
    void effectShowSolution(Point s);
    void stageClear();
    void showMenuPopup(float dt);
    void hideMenuPopup();

};

#endif // __GAMESTAGE_SCENE_H__