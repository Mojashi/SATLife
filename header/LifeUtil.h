#pragma once
#include <vector>
#include <iostream>

using namespace std;

template <typename T>
vector<vector<T>> padding(vector<vector<T>> board, int yp, int xp, T v) {

	int height = yp * 2 + board.size();
	int width = xp * 2 + board[0].size();

	vector<vector<T>> ret(height, vector<T>(width, v));

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

vector<vector<int>> decode(string str) {
	int w, h;
	string bef = string(str.begin(), str.begin() + str.find_first_of('\n') + 1);
	string aft = string(str.begin() + str.find_first_of('\n') + 1, str.end() - 1);
	sscanf_s(bef.c_str(), "x = %d, y = %d, rule = B3/S23\n", &w, &h);

	vector<vector<int>> ret(h, vector<int>(w));
	int len = 0, x = 0, y = 0;
	for (auto itr : aft) {
		if ('0' <= itr && itr <= '9') {
			len = len * 10 + itr - '0';
		}
		else {
			if (len == 0) len = 1;
			if (itr == '$') {
				x = 0, y+=len;
			}
			else {
				rep(i, x, len + x) {
					ret[y][i] = (itr == 'o' ? 1 : 0);
				}
				x += len;
			}
			len = 0;
		}
	}

	return ret;

}