#include <iostream>
#include <algorithm>
#include <fstream>
#include <windows.h>
#include <vector>

using namespace std;



#define rep(i,a,n) for(int i = a; n > i;i++)
#define REP(i,n) rep(i,0,n)
typedef int Literal;
typedef vector<Literal> Clause;

struct CNF {
public:
#define NONE 0
#define FALSE -1
#define TRUE 1
	vector<Clause> cls;
	int varCount = 0;

	CNF() {
		Literal t = getNewVar();
		cls.push_back({ t });
	}

	int getNewVar() {
		varCount++;
		return varCount;
	}

	void setVal(Literal a, Literal b) { // a=b
		cls.push_back({ a, -b});
		cls.push_back({ -a, b });
	}

	int NotEqual(Literal a, Literal b) {
		Literal ret = getNewVar();
		cls.push_back({ -a,-b,-ret});
		cls.push_back({ a,b,-ret });
		cls.push_back({ a,-b,ret });
		cls.push_back({ -a,b,ret });
		return ret;
	}

	int Equal(Literal a, Literal b) {
		Literal ret = getNewVar();
		cls.push_back({ -a,-b,ret });
		cls.push_back({ a,b,ret });
		cls.push_back({ a,-b,-ret });
		cls.push_back({ -a,b,-ret });
		return ret;
	}

	Literal Or(Literal a, Literal b) {
		Literal ret = getNewVar();
		cls.push_back({ -a,ret });
		cls.push_back({ -b,ret });
		cls.push_back({ a,b,-ret });
		return ret;
	}

	Literal Or(vector<Literal> vars) {
		if (vars.size() == 0) return FALSE;
		if (vars.size() == 1) return vars.front();

		Literal ret = getNewVar();
		Clause cl;
		cl.push_back(-ret);
		for (auto itr : vars) {
			cl.push_back(itr);
			cls.push_back({-itr, ret});
		}
		cls.push_back(cl);

		return ret;
	}

	pair<Literal, Literal> halfAdder(Literal a, Literal b) { // a+b return s=sum o=carry bit
		if (a == NONE) return { b, FALSE };
		if (b == NONE) return { a, TRUE };

		int s = getNewVar(), o = getNewVar();
		cls.push_back({ -a,-b,o });
		cls.push_back({ a,-o });
		cls.push_back({ b,-o });

		cls.push_back({ -a,-b, -s});
		cls.push_back({ a,b, -s });
		cls.push_back({ -a,b, s });
		cls.push_back({ a,-b, s });

		return { s,o };
	}
	pair<Literal, Literal> fullAdder(Literal a, Literal b, Literal c) {
		if (a == NONE) return halfAdder(b, c);
		if (b == NONE) return halfAdder(a, c);
		if (c == NONE) return halfAdder(a, b);

		Literal s = getNewVar(), o = getNewVar();

		cls.push_back({ -a,-b,-c,s });
		cls.push_back({ -a, b, c, s });
		cls.push_back({ a,-b,c,s });
		cls.push_back({ a,b,-c,s });
		cls.push_back({ -a,b,-c,-s });
		cls.push_back({ -a,-b,c,-s });
		cls.push_back({ a,-b,-c,-s });
		cls.push_back({ a,b,c,-s });

		cls.push_back({ a, b, -o });
		cls.push_back({ a, c, -o });
		cls.push_back({ b, c, -o });
		cls.push_back({ -a, -b, o });
		cls.push_back({ -a, -c, o });
		cls.push_back({ -b, -c, o });

		return { s,o };
	}
	Literal divTwo(vector<Literal> vars) {

	}
	
	vector<Literal> plus(vector<Literal> a, vector<Literal> b, int digit = 0) {
		if (a.size() < b.size())
			swap(a, b);

		vector<int> ret(a.size() + 1);


		b.resize(a.size(), NONE);

		int carry = NONE;

		REP(i, a.size()) {
			pair<Literal, Literal> buf = fullAdder(a[i], b[i], carry);
			
			carry = buf.second;
			ret[i] = buf.first;
		}
		ret[ret.size() - 1] = carry;
		return ret;
	}

	vector<Literal> sum(vector<Literal> vars, int digit = 0) {
		vector<Literal> ret = {FALSE};

		for (auto itr : vars) {
			ret = plus({ itr }, ret);
		}

		return ret;

	}

	void output(string filename) {
		ofstream ofs(filename.c_str());

		ofs << "p cnf " << varCount << " " << cls.size() << endl;
		for (auto cl : cls) {
			for (auto lit : cl) {
				ofs << lit << " ";
			}
			ofs << 0 << endl;
		}
		ofs.close();
	}

	vector<bool> solve() {
		vector<bool> ans(varCount + 1);
		output("cnf.dimacs");

		system("glucose-simp cnf.dimacs out.txt");

		ifstream ifs("out.txt");
		

		int num;
		while (1) {
			ifs >> num;
			if (num == 0) break;

			ans[abs(num)] = num > 0;
		}
		ifs.close();
		return ans;
	}
};


vector<vector<vector<int>>> searchPattern(int height, int width, int loop) {

	height += 2;
	width += 2;
	loop += 1;

	vector<vector<vector<Literal>>> boardNum(loop, vector<vector<int>>(height, vector<int>(width)));

	CNF cnf;

	REP(k, loop - 1) {
		REP(i, height) {
			REP(j, width) {
				if (i == height - 1 || i == 0 || j == 0 || j == width - 1) {
					boardNum[k][i][j] = FALSE;
				}
				else
					boardNum[k][i][j] = cnf.getNewVar();
			}
		}
	}

	REP(i, height) {
		REP(j, width) {
			boardNum[loop - 1][i][j] = boardNum[0][i][j];
		}
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
								vector<Literal> buf = {-tar[a],-tar[b] ,-tar[c] ,-tar[d], -boardNum[k + 1][i][j] };
								cnf.cls.push_back(buf);
							}

							vector<Literal> buf(tar);
							buf[a] *= -1;
							buf[b] *= -1;
							buf[c] *= -1;
							buf.push_back(boardNum[k + 1][i][j]);
							cnf.cls.push_back(buf);
						}

						vector<Literal> buf(tar);
						buf[a] *= -1;
						buf[b] *= -1;
						buf.push_back(-boardNum[k][i][j]);
						buf.push_back(boardNum[k + 1][i][j]);
						cnf.cls.push_back(buf);

						buf[buf.size() - 2] *= -1;
						buf[buf.size() - 1] *= -1;
						cnf.cls.push_back(buf);
					}

					vector<Literal> buf(tar);
					buf.erase(buf.begin() + a);
					buf.push_back(-boardNum[k + 1][i][j]);
					cnf.cls.push_back(buf);
				}
			}
		}

		if (k != 0) {
			vector<int> eqs;
			rep(i, 1, height - 1) {
				rep(j, 1, width - 1) {
					eqs.push_back(-cnf.Equal(boardNum[k][i][j], boardNum[0][i][j]));
				}
			}

			cnf.cls.push_back(eqs);
		}
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


	vector<vector<pair<vector<int>,int>>> counter(height, vector<pair<vector<int>,int>>(width));

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

				cnf.cls.push_back({ -count[0], -count[1], upper4, boardNum[k + 1][i][j] });
				cnf.cls.push_back({ boardNum[k][i][j], count[0], -count[1], upper4, -boardNum[k + 1][i][j] });
				cnf.cls.push_back({ -boardNum[k][i][j], count[0], -count[1], upper4, boardNum[k + 1][i][j] });
				cnf.cls.push_back({ -count[0], count[1], upper4, -boardNum[k + 1][i][j] });
				cnf.cls.push_back({ count[0], count[1], upper4, -boardNum[k + 1][i][j] });
				cnf.cls.push_back({ -upper4, -boardNum[k + 1][i][j] });
			}
		}

	}

	vector<bool> ans(cnf.solve());

	rep(i,1, height - 1) {
		rep(j,1, width - 1) {
			cout << ans[counter[i][j].second] << ":";
			for (auto itr : counter[i][j].first)
				cout <<ans[ itr];
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
	int w = 7, h = 7, loop = 3;
	cin >> w >> h >> loop;
	vector<vector<vector<int>>> board = searchPattern(h,w,loop);

	REP(k, loop + 1) {
		REP(i, h + 2) {
			REP(j, w + 2) {
				printf("%c", board[k][i][j] ? '*' : ' ');
			}
			printf("\n");
		}
	}

	/*int time = 4;
	vector<vector<vector<int>>> board = simulateLifegame({
		{0,1,0},
		{0,1,0},
		{0,1,0}
		}, time);

	REP(k, time) {
		REP(i, board[0].size()) {
			REP(j, board[0][0].size()) {
				printf("%c", board[k][i][j] ? '*' : ' ');
			}
			printf("\n");
		}
	}
*/
	/*CNF cnf;
	vector<int> a = {TRUE,FALSE,FALSE,FALSE,TRUE,TRUE,FALSE,TRUE,FALSE};

	vector<int> sum(cnf.sum(a));
	vector<int> ans(cnf.solve());

	cnf.solve();

	for (auto itr : sum)
		cout << ans[itr];*/

	/*
	int a = cnf.getNewVar(), b = cnf.getNewVar(),c = cnf.getNewVar();
	cnf.setVal(a, FALSE);
	cnf.setVal(b, TRUE);
	cnf.setVal(c, TRUE);
	int p = cnf.getNewVar(), q = cnf.getNewVar(), r = cnf.getNewVar();
	cnf.setVal(p, FALSE);
	cnf.setVal(q, FALSE);
	cnf.setVal(r, TRUE);

	vector<int> sum = cnf.plus(cnf.plus({ a,b,c }, { p,q,r }), { a,b,c });

	vector<int> ans = cnf.solve();

	cout << ans[a] << ans[b] << ans[c] << endl;
	cout << ans[p] << ans[q] << ans[r] << endl;
	for (auto itr : sum)
		cout << ans[itr];*/
	return 0;
}