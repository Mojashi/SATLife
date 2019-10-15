#pragma once
#include <vector>
#include <iostream>

using namespace std;

vector<vector<int>> padding(vector<vector<int>> board, int yp, int xp) {

	int height = yp * 2 + board.size();
	int width = xp * 2 + board[0].size();

	vector<vector<int>> ret(height, vector<int>(width));

	rep(i, yp, yp + board.size()) {
		rep(j, xp, xp + board[0].size()) {
			ret[i][j] = board[i - yp][j - xp];
		}
	}
	return ret;
}


pair<int, int> rotate90(int h, int w, pair<int, int> c) {
	return { w - c.second - 1, c.first };
}

void encode(vector<vector<int>> board) {
	printf("x = %d, y = %d, rule = B3/S23\n", board[0].size(), board.size());
	int co = 0;
	for (auto row : board) {
		int cou = 0;
		int bef = row[0];
		for (auto cell : row) {
			if (bef == cell) cou++;
			else {
				printf("%d%c", cou, bef ? 'o' : 'b');
				cou = 1;
			}
			bef = cell;
		}
		printf("%d%c", cou, bef ? 'o' : 'b');
		co++;
		if (co != board.size())
			printf("$");
		else
			printf("!\n");
	}

}
