#include <iostream>
#include <algorithm>
#include <fstream>
#include <windows.h>
#include <assert.h>
#include <vector>
#include "../header/CNF.h"
#include "../header/LifeUtil.h"

using namespace std;

vector<vector<vector<int>>> searchPattern(int height, int width, int loop, int moveX = 0, int moveY = 0, int rot = 0, vector<vector<vector<int>>> avoid = {}) {

	pair<int, int> patsize = { height, width };

	assert(rot <= 3 && rot >= 0);

	height += 2 + moveY;
	width += 2 + moveX;
	loop += 1;

	vector<vector<vector<Literal>>> boardNum(loop, vector<vector<int>>(height, vector<int>(width, CNF::False)));

	CNF cnf;

	rep(k, 1, loop - 1) {
		rep(i, 1, height - 1) {
			rep(j, 1, width - 1) {
				boardNum[k][i][j] = cnf.getNewVar();
			}
		}
	}

	REP(i, patsize.first) {
		REP(j, patsize.second) {
			pair<int, int> co = { i,j };
			REP(k, rot) co = rotate90(patsize.first, patsize.second, co);

			boardNum[loop - 1][co.first + 1 + moveY][co.second + 1 + moveX] =  boardNum[0][1 + i][1 + j] = cnf.getNewVar();
		}
	}

	{
		vector<int> rowcolc;
		REP(i, patsize.second) {
			rowcolc.push_back(boardNum[0][1][i + 1]);
		}
		REP(i, patsize.first) {
			rowcolc.push_back(boardNum[0][i + 1][1]);
		}
		cnf.addClause(rowcolc);
	}

	REP(k, loop - 1) {
		REP(i,  height ) {
			REP(j , width ) {
				vector<Literal> tar;
				if (i > 0) tar.push_back(boardNum[k][i - 1][j]);
				if (j > 0) tar.push_back(boardNum[k][i][j - 1]);
				if (i < height - 1) tar.push_back(boardNum[k][i + 1][j]);
				if (j < width - 1) tar.push_back(boardNum[k][i][j + 1]);

				if (i > 0 && j > 0) tar.push_back(boardNum[k][i - 1][j - 1]);
				if (i < height - 1 && j > 0) tar.push_back(boardNum[k][i + 1][j - 1]);
				if (i > 0 && j < width - 1) tar.push_back(boardNum[k][i - 1][j + 1]);
				if (i < height - 1 && j < width - 1) tar.push_back(boardNum[k][i + 1][j + 1]);

				REP(a, tar.size()) {//8C4
					rep(b, a + 1, tar.size()) {
						rep(c, b + 1, tar.size()) {
							rep(d, c + 1, tar.size()) {
								Clause buf = {-tar[a],-tar[b] ,-tar[c] ,-tar[d], -boardNum[k + 1][i][j] };
								cnf.addClause(buf);
							}

							Clause buf(tar);
							buf[a] *= -1;
							buf[b] *= -1;
							buf[c] *= -1;
							buf.push_back(boardNum[k + 1][i][j]);
							cnf.addClause(buf);
						}

						Clause buf(tar);
						buf[a] *= -1;
						buf[b] *= -1;
						buf.push_back(-boardNum[k][i][j]);
						buf.push_back(boardNum[k + 1][i][j]);
						cnf.addClause(buf);

						buf[buf.size() - 2] *= -1;
						buf[buf.size() - 1] *= -1;
						cnf.addClause(buf);
					}

					Clause buf(tar);
					buf.erase(buf.begin() + a);
					buf.push_back(-boardNum[k + 1][i][j]);
					cnf.addClause(buf);
				}
			}
		}

		if (k != 0) {
			Clause eqs;
			rep(i, 1, height - 1) {
				rep(j, 1, width - 1) {
					eqs.push_back(-cnf.Equal(boardNum[k][i][j], boardNum[0][i][j]));
				}
			}

			cnf.addClause(eqs);
		}
	}
	for (auto abd : avoid) {
		Clause eqs;
		REP(i, abd.size()) {
			REP(j, abd[0].size()) {
				eqs.push_back(abd[i][j] ? -boardNum[0][i][j] : boardNum[0][i][j]);
			}
		}
		cnf.addClause(eqs);
	}

	vector<bool> ans(cnf.solve());

	vector<vector<vector<int>>> ret(loop, vector<vector<int>>(height, vector<int>(width)));
	REP(k, loop) {
		REP(i, height) {
			REP(j, width) {
				if (boardNum[k][i][j] > 0)
					ret[k][i][j] = ans[abs(boardNum[k][i][j])];
				else
					ret[k][i][j] = 1 - ans[abs(boardNum[k][i][j])];
			}
		}
	}
	return ret;
}

int main() {
	int w, h, loop, movex, movey, rot;
	cin >> w >> h >> loop >> movex >>  movey >> rot;
	vector<vector<vector<int>>> avoid;
	while (1) {
		vector<vector<vector<int>>> board = searchPattern(h, w, loop, movex, movey, rot, avoid);


		for (auto bd : board) {
			for (auto row : bd) {
				for (auto cell : row) {
					printf("%c", cell ? '*' : 'o');
				}
				printf("\n");
			}
			printf("\n");
		}
		cout << "continue?[1:0]" << endl;
		int cont;
		cin >> cont;
		if (cont)
			avoid.push_back(board[0]);
		else
			break;
	}
	return 0;
}