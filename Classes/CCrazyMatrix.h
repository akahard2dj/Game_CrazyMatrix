#include <stdlib.h>
#include <string.h>
#include <memory>
#include <stdio.h>
#include <time.h>

#define	MAX_SHUFFLES 	16
#define MAX_LEVEL		20

typedef enum _ShuffleType
{
	TRANSFORM_FLIP_X,
	TRANSFORM_FLIP_Y,
	TRANSFORM_ROT_CW,
	TRANSFORM_ROT_CCW,
	TRANSFORM_TRANS1,
	TRANSFORM_TRANS2,
	TRANSFORM_NUM_TYPE
} ShuffleType;

typedef struct _StageInfo
{
	int level;
    int matrixSize;
    int actionNum;
    int solutionNum;
    int TimeLimit;
    ShuffleType actions[MAX_SHUFFLES];
    int *before;
    int *after;
} StageInfo;

using namespace std;

class CCrazyMatrix
{
public:
    CCrazyMatrix();
    CCrazyMatrix(int _numPairs, int _numSides, int _numShuffles, int _level);
	CCrazyMatrix(int _level);
    ~CCrazyMatrix();
   
    StageInfo getStageInfo(int _level);
    
private:
	int level;
    int numPairs;
    int numSides;
	int numShuffles;
    int timeLimit;
	int gameSchedule[MAX_SHUFFLES];
	int **boardOriginal;
    int **board;
    int **boardCheck;
    int *orderSolution;
    int *idxX;
    int *idxY;
	StageInfo info;

	//obsolete functions
	void getBoard(int *_board);
	void getGameSchedule(int _gameSchedule[MAX_SHUFFLES]);
	
	int getNumTries();
    int getNumCorrects();
    int getNumErrors();

	int submitAnswer(int idx1, int idx2);
    int submitAnswerLight(int idx);
	void submitAnswerLight(int *_submit, int *_result, int *_status);

	// Game setting
	void setPosition();
    void setPositionLight();

	// Matrix Transformation method
    void transformFlipX();
    void transformFlipY();
    void transformRotCW();
    void transformRotCCW();
    void transformTrans1();
    void transformTrans2();

	void levelDesign();
    void gameDesign();
    void lightGameDesign();
	void lightGameShuffle();
    void boardCopy();

	void Print();
protected:
    int numTries;
    int numCorrects;
    int numErrors;
};
