#pragma once
#include<utility>
#include<vector>
#include <Windows.h>

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

//HACK: Если что поменять на int
typedef pair<size_t, size_t> MYCOORD;

class MyLine {
public:
	explicit MyLine(MYCOORD dBeg, MYCOORD dEnd, size_t width, Dir dir) :dBeg_(dBeg), dEnd_(dEnd), width_(width), dir_(dir) {
		//HACK: Скорее всего придется использовать кучу, иначе всё удалится
	}

	//Спросить для всех ли конструкторов нужен explicit
	explicit MyLine(const MyLine& lineToCopy) : dBeg_(lineToCopy.dBeg_), dEnd_(lineToCopy.dEnd_), width_(lineToCopy.width_), dir_(lineToCopy.dir_) {

	}

	bool Draw(HDC hdc) const {
		HPEN hPen = CreatePen(PS_SOLID, width_, RED); //Объявляется кисть
		SelectObject(hdc, hPen); //Объект делается текущим
		POINT pt;
		MoveToEx(hdc, dBeg_.first, dBeg_.second, &pt);
		return LineTo(hdc, dEnd_.first, dEnd_.second);
	}

	void Change(Dir dir, size_t speed) {
		//if (dir != dir_) {
		//	if (CheckOr(dir) != CheckOr(dir_)) {
		//		//Меняем координаты поворотом на 90 градусов
		//		Orient orient = CheckOr(dir_);
		//		if (orient == Orient::HOR) {
		//			int leng = dBeg_.first - dEnd_.first;
		//			dEnd_.first -= leng;
		//			dEnd_.second += leng;
		//		}
		//		else {
		//			int leng = dBeg_.second - dEnd_.second;
		//			dEnd_.second -= leng;
		//			dEnd_.first += leng;
		//		}
		//	}
		//	dir_ = dir;
		//}
		//else {
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
		//}
	}

	size_t width_;
private:
	MYCOORD dBeg_;
	MYCOORD dEnd_;
	Dir dir_;
};

class MovingLine {
public:
	explicit MovingLine(size_t speed, size_t traceL, size_t lineL, size_t dist) : speed_(speed), traceL_(traceL), dist_(dist) {
		const size_t otst = 20;
		for (int i = traceL; i > 0; i--) {
			lines_.push_back(MyLine(MYCOORD(i * dist, lineL + otst), MYCOORD(i * dist, otst), i, dir_));
		}
	}

	explicit MovingLine(const MovingLine& mLineToCopy) : lines_(mLineToCopy.lines_),  speed_(mLineToCopy.speed_), traceL_(mLineToCopy.traceL_), 
		dist_(mLineToCopy.dist_), dir_(mLineToCopy.dir_) {

	}

	void Draw(HDC hdc) {
		for (const MyLine& line : lines_) {
			line.Draw(hdc);
		}
	}

	void ChangeDir(Dir dir) {
		//HACK: доделать
		//Если ориентации одинаковые просто переставляем в обратном порядке
		if (CheckOr(dir) == CheckOr(dir_)) {
			reverse(lines_.begin(), lines_.end());
		}
		dir_ = dir;
	}

	void MoveLines() {
		for (MyLine& line : lines_) {
			line.Change(dir_, speed_);
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