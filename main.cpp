#include<iostream>
#include<cstdio>
#include<cstring>
#include<cstdlib>
using namespace std;

#define MAX_CAILD_NODES 9

#define RED 0
#define BLUE 1
#define REDSTONE 0
#define BLUESTONE1 1
#define BLUESTONE2 2

#define MAXDEPTH 10

#define MAX 1
#define MIN -1
#define INV -1

int alpha, bate;

int preTable[2][10][5]{
	{
		//red
		{ 2, 1, INV },{ 2, 3, 0, INV },
		{ 4, 3, 1, 0, INV },{ 5, 4, 2, 1, INV },
		{ 6, 5, 3, 2, INV },{ 7, 6, 4, 3, INV },
		{ 8, 7, 5, 4, INV },{ 9, 8, 6, 5, INV },
		{ 9, 7, 6, INV },{ 8, 7, INV }
	},
	{
		//blue
		{ INV },{ 0, INV },
		{ 3, 1, 0, INV },{ 2, 1, INV },
		{ 2, 3, 5, INV },{ 3, 4, INV },
		{ 4, 5, 7, INV },{ 5, 6, INV },
		{ 6, 7, 9, INV },{ 7, 8, INV },
	}
};

//初始棋盘
int stoneIntersection[3] = { 0,8,9 };
//着法列表
int moveList[1024];
//各着法首地址
int* pList[MAXDEPTH + 2];
//当前搜索深度
int maxDepth;
//最好的
int bestRootMove;
//电脑思考次数
int boards_checked;

void DrawBoard();

//下子
int putCell(int from,int to,int wtm) {
	int i;
	int rVal = 1;

	if ((from < 0) || (from > 9))
		return 1;
	if ((to < 0) || (to > 9))
		return 1;

	if (RED == wtm && (from != stoneIntersection[REDSTONE] || to == stoneIntersection[BLUESTONE1] || to == stoneIntersection[BLUESTONE2]))
		return 1;
	else if (BLUE == wtm && ((from != stoneIntersection[BLUESTONE1] && from != stoneIntersection[BLUESTONE2]) || to == stoneIntersection[REDSTONE] || to == stoneIntersection[BLUESTONE1] || to == stoneIntersection[BLUESTONE2]))
		return 1;

	if (2 < abs(from - to))
		return 1;

	for (i = 0; i < 3; ++i) {
		if (from == stoneIntersection[i]){
			stoneIntersection[i] = to;
			rVal = -1;
		}
	}
	return rVal;
}

//生成着法
int* getMove(int color, int *mList) {
	int i = 0;
	int from;
	int to;
	int count = 0;

	if (color == RED) {
		from = stoneIntersection[REDSTONE];
		to = preTable[color][from][i];
		while (to != INV) {
			if ((to != stoneIntersection[BLUESTONE1]) && (to != stoneIntersection[BLUESTONE2])) {
				mList[count++] = to;
			}
			to = preTable[color][from][++i];
		}
	}

	else {
		from = stoneIntersection[BLUESTONE1];
		to = preTable[color][from][i];
		while (to != INV) {
			if ((to != stoneIntersection[REDSTONE]) && (to != stoneIntersection[BLUESTONE2])) {
				mList[count++] = to | 16;
			}
			to = preTable[color][from][++i];
		}

		i = 0;
		from = stoneIntersection[BLUESTONE2];
		to = preTable[color][from][i];
		while (to != INV) {
			if ((to != stoneIntersection[REDSTONE]) && (to != stoneIntersection[BLUESTONE1])) {
				mList[count++] = to | 32;
			}
			to = preTable[color][from][++i];
		}
	}

	return &mList[count];
}

int checkIsWin(int wtm,int *si) {
	if ((si[RED] == 8) || (si[RED] == 9)) {
		return ((wtm == RED) ? 1 : -1);
	}

	else if ((si[RED] == 0) && ((si[BLUESTONE1] + si[BLUESTONE2]) == 3)){
		return ((wtm == RED) ? -1 : 1);
	}
	return 0;
}

int Evaluation(int wtm, int *si) {
	int value = 0;

	if ((si[RED] > si[BLUESTONE1]) || (si[RED] > si[BLUESTONE2])){
		value = ((wtm == RED) ? MAX : MIN);
	}

	else if ((wtm == RED) && ((stoneIntersection[BLUESTONE1] == si[RED] + 1) && (stoneIntersection[BLUESTONE2] == si[RED] + 2) || (stoneIntersection[BLUESTONE1] == si[RED] + 2) && (stoneIntersection[BLUESTONE2] == si[RED] + 1)))	{
		value = MAX;
	}

	else if ((wtm == BLUE) &&((1 == si[REDSTONE] || 2 == si[REDSTONE] || 3 == si[REDSTONE]) &&((stoneIntersection[BLUESTONE1] == si[RED] + 1) &&(stoneIntersection[BLUESTONE2] == si[RED] + 2) ||(stoneIntersection[BLUESTONE1] == si[RED] + 2) &&(stoneIntersection[BLUESTONE2] == si[RED] + 1)))){
		value = MAX;
	}
	return value;
}

int MakeMove(int piece, int to) {
	int tmp = stoneIntersection[piece];
	stoneIntersection[piece] = to;
	return tmp;
}

void UnmakeMove(int piece, int from) {
	stoneIntersection[piece] = from;
}

void getHunamMove() {
	char  selection[200];
	while (1) {
		cin>>selection;
		printf("\n");
		int piece, from, to;
		from = selection[0] - '0';
		to = selection[1] - '0';
		if (from <= 9 && from >= 0 && to <= 9 && to >= 0)
			if (putCell(from, to, RED) == -1) break;
	}
}

int evaluateComputerMove(int depth, int alpha, int bate);
int evaluateHumanMove(int depth, int alpha, int bate);

int evaluateHumanMove(int depth, int alpha, int bate) {
	int move;
	int piece;
	int from;
	int ply = maxDepth - depth;
	int *pMove = pList[ply];
	int value;
	boards_checked++;
	short min = MAX + 1;
	pList[ply + 1] = getMove(RED, pList[ply]);
	if (checkIsWin(BLUE, stoneIntersection) == 1) {
		return MAX;
	}
	if (pList[ply] == pList[ply + 1] || depth == 0) {
		return Evaluation(BLUE, stoneIntersection);
	}
	for (; pMove<pList[ply + 1]; ++pMove) {
		move = *pMove;
		piece = move >> 4;
		from = MakeMove(piece, move & 15);
		value = evaluateComputerMove(depth - 1, alpha, bate);
		UnmakeMove(piece, from);
		if (value<min) {
			min = value;
		}
		if (value < bate) {
			bate = value;
		}
		if (bate <= alpha) {
			return bate;
		}
	}
	if (min == MAX + 1) {
		return 0;
	}

	return min;
}

int evaluateComputerMove(int depth, int alpha, int bate)
{
	int move;
	int piece;
	int from;
	int ply = maxDepth - depth;
	int *pMove = pList[ply];
	int value;
	boards_checked++;
	short max = MIN - 1;

	pList[ply + 1] = getMove(BLUE, pList[ply]);

	if (checkIsWin(RED, stoneIntersection) == 1)
		return MIN;
	if (pList[ply] == pList[ply + 1] || depth == 0){
		return -Evaluation(RED, stoneIntersection);
	}
	for (; pMove<pList[ply + 1]; ++pMove){
		move = *pMove;
		piece = move >> 4;

		from = MakeMove(piece, move & 15);
		value = evaluateHumanMove(depth - 1, alpha, bate);
		UnmakeMove(piece, from);

		if (value>max){
			max = value;
			if (!ply){
				bestRootMove = move;
			}
		}
		if (value>alpha)
			alpha = value;

		if (alpha >= bate)
			return alpha;
	}

	if (max == MIN - 1) {
		return 0;
	}
	return max;
}

void getComputerMove() {
	boards_checked = 0;
	evaluateComputerMove(maxDepth, -3, 3);
	int piece, from, to;
	piece = (bestRootMove & 48) >> 4;
	from = stoneIntersection[piece];
	to = bestRootMove & 15;
	cout << from << to << endl;
	putCell(from, to, piece);
	return;
}

int main() {

	cout << "这个棋叫牛角棋，初始布子如下图，一个R（红子）在顶端，两个B（蓝子）在底端，红子先走，每次沿线走1步，走入相邻空棋位。红子成功地走到8或9位红子获胜，蓝子把红子怼回0蓝子获胜。现在你是红子，电脑（PlayEgg）是蓝子。从0到1输入“01+回车”。如果行棋过程中有什么BUG关了重启。开始吧\n" << endl;
	bestRootMove = 1000;
	maxDepth = MAXDEPTH;

	pList[0] = moveList;
	int won = 0;

	DrawBoard();

	while (!won) {
		
		getHunamMove();

		won = checkIsWin(RED, stoneIntersection);

		if (!won){
			won = 0;

			getComputerMove();

			DrawBoard();

			won = checkIsWin(BLUE, stoneIntersection);
			if (won){
				printf("\nYou lose!\n");
			}
		}
		else{
			printf("\nYou win!\n");
		}
	}
	system("pause");
	return 0;
}

//灵魂画手
void DrawBoard() {
	char ch[10] = { '0' , '1' , '2' , '3' , '4' , '5' , '6' , '7' , '8' , '9' };
	ch[stoneIntersection[RED]] = 'R';
	ch[stoneIntersection[BLUESTONE1]] = 'B';
	ch[stoneIntersection[BLUESTONE2]] = 'B';

	cout << ch[0] << "\n";
	cout << "|  \\\n";
	cout << "|   " << ch[1] << endl;
	cout << "|  /  \\\n";
	cout << ch[2] << "-----" << ch[3] << endl;
	printf("∣／  ∣\n");
	printf("%c—— %c\n", ch[4], ch[5]);
	printf("∣  ／∣\n");
	printf("∣／  ∣\n");
	printf("%c—— %c\n", ch[6], ch[7]);
	printf("∣  ／∣\n");
	printf("∣／  ∣\n");
	printf("%c—— %c\n", ch[8], ch[9]);

	return;
}