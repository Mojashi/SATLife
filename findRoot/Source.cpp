#include <iostream>
#include <algorithm>
#include <fstream>
#include <windows.h>
#include <assert.h>
#include <vector>
#include "../header/CNF.h"
#include "../header/LifeUtil.h"

using namespace std;

vector<vector<int>> deathGame(vector<vector<int>> board) {

	int height = 2 + board.size();
	int width = 2 + board[0].size();

	vector<vector<Literal>> targetpat(height, vector<int>(width, CNF::False));


	vector<vector<Literal>> boardNum(vector<vector<int>>(height, vector<int>(width, CNF::False)));

	CNF cnf;

	rep(i, 1, height - 1) {
		rep(j, 1, width - 1) {
			boardNum[i][j] = cnf.getNewVar();

			if (board[i - 1][j - 1])
				targetpat[i][j] = CNF::True;
			else
				targetpat[i][j] = CNF::False;
		}
	}

	REP(i, height) {
		REP(j, width) {
			vector<Literal> tar;
			if (i > 0) tar.push_back(boardNum[i - 1][j]);
			if (j > 0) tar.push_back(boardNum[i][j - 1]);
			if (i < height - 1) tar.push_back(boardNum[i + 1][j]);
			if (j < width - 1) tar.push_back(boardNum[i][j + 1]);

			if (i > 0 && j > 0) tar.push_back(boardNum[i - 1][j - 1]);
			if (i < height - 1 && j > 0) tar.push_back(boardNum[i + 1][j - 1]);
			if (i > 0 && j < width - 1) tar.push_back(boardNum[i - 1][j + 1]);
			if (i < height - 1 && j < width - 1) tar.push_back(boardNum[i + 1][j + 1]);

			REP(a, tar.size()) {//8C4
				rep(b, a + 1, tar.size()) {
					rep(c, b + 1, tar.size()) {
						rep(d, c + 1, tar.size()) {
							vector<Literal> buf = { -tar[a],-tar[b] ,-tar[c] ,-tar[d], -targetpat[i][j] };
							cnf.addClause(buf);
						}

						vector<Literal> buf(tar);
						buf[a] *= -1;
						buf[b] *= -1;
						buf[c] *= -1;
						buf.push_back(targetpat[i][j]);
						cnf.addClause(buf);
					}

					vector<Literal> buf(tar);
					buf[a] *= -1;
					buf[b] *= -1;
					buf.push_back(-boardNum[i][j]);
					buf.push_back(targetpat[i][j]);
					cnf.addClause(buf);

					buf[buf.size() - 2] *= -1;
					buf[buf.size() - 1] *= -1;
					cnf.addClause(buf);
				}

				vector<Literal> buf(tar);
				buf.erase(buf.begin() + a);
				buf.push_back(-targetpat[i][j]);
				cnf.addClause(buf);
			}
		}
	}


	vector<int> eqs;
	rep(i, 1, height - 1) {
		rep(j, 1, width - 1) {
			eqs.push_back(-cnf.Equal(boardNum[i][j], targetpat[i][j]));
		}
	}

	cnf.addClause(eqs);



	vector<bool> ans(cnf.solve());

	vector<vector<int>> ret(height - 2, vector<int>(width - 2));
	rep(i, 1, height - 1) {
		rep(j, 1, width - 1) {
			if (boardNum[i][j] > 0)
				ret[i - 1][j - 1] = ans[abs(boardNum[i][j])];
			else
				ret[i - 1][j - 1] = 1 - ans[abs(boardNum[i][j])];
		}
	}

	if (ans[CNF::True] == false) return {};

	return ret;
}

void searchRoot(vector<vector<int>> board) {
	int yp, xp;
	cin >> yp >> xp;
	vector<vector<int>> last(board);
	while (1) {

		board = padding(board, yp, xp);
		board = deathGame(board);

		if (board == vector<vector<int>>()) break;

		last = board;
		for (auto row : board) {
			for (auto cell : row) {
				printf("%c", cell ? '*' : 'o');
			}
			printf("\n");
		}
		printf("\n");

		int cont;
		cout << "continue?[1:0]" << endl;
		cin >> cont;
		if (cont == 0)
			break;
	}

	encode(last);
}


int main() {
	string str;
	while (1) {
		char ch = getchar();
		str += ch;
		if (ch == '!') break;
	}
	searchRoot(decode(str));

	return 0;
}