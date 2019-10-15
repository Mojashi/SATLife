#include <iostream>
#include <algorithm>
#include <fstream>
#include <windows.h>
#include <assert.h>
#include <vector>
#include "../header/CNF.h"
#include "../header/LifeUtil.h"

using namespace std;

vector<vector<vector<int>>> simulateLifegame(vector<vector<int>> board, int time) {

	int height = board.size() + 2;
	int width = board[0].size() + 2;

	vector<vector<vector<int>>> boardNum(time, vector<vector<int>>(height, vector<int>(width)));

	CNF cnf;

	REP(k, time) {
		REP(i, height) {
			REP(j, width) {
				if (i == height - 1 || i == 0 || j == 0 || j == width - 1) {
					boardNum[k][i][j] = FALSE;
				}
				else {
					if (k != 0) {
						boardNum[k][i][j] = cnf.getNewVar();
					}
					else {
						if (board[i - 1][j - 1])
							boardNum[0][i][j] = TRUE;
						else
							boardNum[0][i][j] = FALSE;
					}
				}
			}
		}
	}


	vector<vector<pair<vector<int>, int>>> counter(height, vector<pair<vector<int>, int>>(width));

	REP(k, time - 1) {
		rep(i, 1, height - 1) {
			rep(j, 1, width - 1) {
				vector<int> count = cnf.sum({
					boardNum[k][i - 1][j - 1],boardNum[k][i - 1][j],boardNum[k][i - 1][j + 1],
					boardNum[k][i][j - 1] ,boardNum[k][i][j + 1],
					boardNum[k][i + 1][j - 1] ,boardNum[k][i + 1][j] ,boardNum[k][i + 1][j + 1]
					});

				counter[i][j].first = count;

				int upper4 = cnf.Or(vector<int>(count.begin() + 2, count.end()));

				counter[i][j].second = upper4;

				cnf.addClause({ -count[0], -count[1], upper4, boardNum[k + 1][i][j] });
				cnf.addClause({ boardNum[k][i][j], count[0], -count[1], upper4, -boardNum[k + 1][i][j] });
				cnf.addClause({ -boardNum[k][i][j], count[0], -count[1], upper4, boardNum[k + 1][i][j] });
				cnf.addClause({ -count[0], count[1], upper4, -boardNum[k + 1][i][j] });
				cnf.addClause({ count[0], count[1], upper4, -boardNum[k + 1][i][j] });
				cnf.addClause({ -upper4, -boardNum[k + 1][i][j] });
			}
		}

	}

	vector<bool> ans(cnf.solve());

	rep(i, 1, height - 1) {
		rep(j, 1, width - 1) {
			cout << ans[counter[i][j].second] << ":";
			for (auto itr : counter[i][j].first)
				cout << ans[itr];
			cout << " ";
		}
		cout << endl;
	}


	vector<vector<vector<int>>> ret(time, vector<vector<int>>(height, vector<int>(width)));
	REP(k, time) {
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
	return 0;
}