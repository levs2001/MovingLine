#pragma once
#include<utility>
#include<vector>
#include <Windows.h>
#include <iterator>

#define RED RGB(255, 0, 0)

using namespace std;

enum class Dir {
	LEFT,
	RIGHT,
	DOWN,
	UP
};

enum class Orient {
	HOR,
	VERT
};

Orient CheckOr(Dir dir) {
	if (dir == Dir::LEFT || dir == Dir::RIGHT) {
		return Orient::HOR;
	}
	else return Orient::VERT;
}

typedef pair<size_t, size_t> MYCOORD;

class MyLine {
public:
	MyLine(MYCOORD dBeg, MYCOORD dEnd, size_t width, Dir dir) :dBeg_(dBeg), dEnd_(dEnd), width_(width), dir_(dir) {
	}

	MyLine(const MyLine& lineToCopy) : dBeg_(lineToCopy.dBeg_), dEnd_(lineToCopy.dEnd_), width_(lineToCopy.width_), dir_(lineToCopy.dir_) {
	}

	MyLine& operator= (const MyLine& line) {
		if (this == &line)
			return *this;

		width_ = line.width_;
		dBeg_ = line.dBeg_;
		dEnd_ = line.dEnd_;
		dir_ = line.dir_;

		return *this;
	}

	bool Draw(HDC hdc) const {
		HPEN hPen = CreatePen(PS_SOLID, width_, (dBeg_.first % 40, dEnd_.second % 255, (dEnd_.first + dBeg_.second) % 255));
		SelectObject(hdc, hPen);
		POINT pt;
		MoveToEx(hdc, dBeg_.first, dBeg_.second, &pt);
		return LineTo(hdc, dEnd_.first, dEnd_.second);
	}

	void ChangeDir(const Dir dir, size_t dist, size_t pos, bool oldRD) {
		if (CheckOr(dir) != CheckOr(dir_)) {
			int delta = 0;
			int k = oldRD ? 1 : -1;
			if (CheckOr(dir_) == Orient::HOR) {
				delta = dEnd_.second - dBeg_.second;
				dBeg_.first += (pos * dist) * k;
				dBeg_.second -= (pos * dist);
				dEnd_.first = dBeg_.first + abs(delta);
				dEnd_.second = dBeg_.second;
			}
			else {
				delta = dEnd_.first - dBeg_.first;
				dBeg_.first -= (pos * dist);
				dBeg_.second += (pos * dist) * k;
				dEnd_.first = dBeg_.first;
				dEnd_.second = dBeg_.second + abs(delta);
			}
		}
		dir_ = dir;
	}

	void Move(const size_t speed) {
		switch (dir_) {
		case Dir::RIGHT:
			dBeg_.first += speed;
			dEnd_.first += speed;
			break;
		case Dir::DOWN:
			dBeg_.second += speed;
			dEnd_.second += speed;
			break;
		case Dir::LEFT:
			dBeg_.first -= speed;
			dEnd_.first -= speed;
			break;
		case Dir::UP:
			dBeg_.second -= speed;
			dEnd_.second -= speed;
			break;
		}
	}

	size_t width_;
private:
	MYCOORD dBeg_;
	MYCOORD dEnd_;
	Dir dir_;
};

class MovingLine {
public:
	MovingLine(size_t speed, size_t traceL, size_t lineL, size_t dist) : speed_(speed), traceL_(traceL), dist_(dist) {
		const size_t otst = 20;
		for (int i = traceL; i > 0; i--) {
			lines_.push_back(MyLine(MYCOORD(i * dist, lineL + otst), MYCOORD(i * dist, otst), i, dir_));
		}
	}

	explicit MovingLine(const MovingLine& mLineToCopy) : lines_(mLineToCopy.lines_), speed_(mLineToCopy.speed_), traceL_(mLineToCopy.traceL_),
		dist_(mLineToCopy.dist_), dir_(mLineToCopy.dir_) {

	}

	void Draw(HDC hdc) {
		for (const MyLine& line : lines_) {
			line.Draw(hdc);
		}
	}

	void LinesRevert() {
		reverse(lines_.begin(), lines_.end());
		size_t wid = traceL_;
		for (MyLine& line : lines_) {
			line.width_ = wid--;
		}
	}

	void LinesChangeDir(Dir dir, bool oldRD) {
		size_t pos = 0;
		for (MyLine& line : lines_) {
			line.ChangeDir(dir, dist_, pos++, oldRD);
		}
	}

	bool ChangeDir(Dir dir) {
		if (dir_ == dir)
			return false;

		if (CheckOr(dir) == CheckOr(dir_)) {
			LinesRevert();
			LinesChangeDir(dir, true);
		}
		else if (dir_ == Dir::RIGHT || dir_ == Dir::DOWN) {
			LinesChangeDir(dir, true);
			if (dir_ == Dir::DOWN && dir == Dir::LEFT || dir_ == Dir::RIGHT && dir == Dir::UP) {
				LinesRevert();
			}
		}
		else {
			LinesChangeDir(dir, false);
			if (dir_ == Dir::UP && dir == Dir::LEFT || dir_ == Dir::LEFT && dir == Dir::UP) {
				LinesRevert();
			}
		}

		dir_ = dir;
		return true;
	}

	void MoveLines() {
		for (MyLine& line : lines_) {
			line.Move(speed_);
		}
	}

	~MovingLine() {
		lines_.clear();
	}

private:
	vector<MyLine> lines_;
	size_t speed_;
	size_t traceL_;
	size_t dist_;
	Dir dir_ = Dir::RIGHT;
};