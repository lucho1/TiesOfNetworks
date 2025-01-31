#ifndef __SCOREBOARD_HPP__
#define __SCOREBOARD_HPP__

#include <string>
#include <assert.h>

class Scoreboard {
private:
	std::string*	_names = nullptr;
	unsigned int*	_score = nullptr;
	unsigned int	_size;
	unsigned int	_capacity;

public:
	struct ScoreboardMember {
		ScoreboardMember(std::string& _name, unsigned int& _score) : name(_name), score(_score){}
		std::string &name;
		unsigned int &score;
	};

public:
	Scoreboard() {
		_capacity = 0;
	}

	Scoreboard(unsigned int size) {
		_capacity = size;
		_size = 0;
		_names = new std::string[size];
		_score = new unsigned int[size];
	}

	~Scoreboard() {
		if (_score)
			delete[] _score;
		if (_names)
			delete[] _names;
	}

	unsigned int Size() const{
		return _size;
	}

	void SetCapacity(unsigned int capacity) {
		if (capacity == _capacity)
			return;

		std::string* new_names_array = new std::string[capacity];
		unsigned int* new_score_array = new unsigned int[capacity];

		if (_names) {
			if (_size <= capacity)
				memcpy(new_names_array, _names, sizeof(std::string) * _size);
			else
				memcpy(new_names_array, _names, sizeof(std::string) * capacity);
			delete[] _names;
		}
		if (_score) {
			if (_size <= capacity)
				memcpy(new_score_array, _score, sizeof(unsigned int) * _size);
			else
				memcpy(new_score_array, _score, sizeof(unsigned int) * capacity);
			delete[] _score;
		}

		_names = new_names_array;
		_score = new_score_array;
		_capacity = capacity;
		if (_size > _capacity)
			_size = _capacity;
	}

	void Reserve(unsigned int size) {
		assert(size <= _capacity, "Reserved size cannot be bigger than capacity!");
		_size = size;
	}

	unsigned int Capacity() const {
		return _capacity;
	}

	void TryAdd(unsigned int score, std::string name) {
		if (_size == 0) {
			if (_capacity > 0) {
				_score[0] = score;
				_names[0] = name;
				_size++;
			}
			return;
		}

		if (score <= _score[_size - 1]) {
			if (_size == _capacity)
				return;
			else {
				_score[_size] = score;
				_names[_size] = name;
				++_size;
				return;
			}
		}

		unsigned int left, right;
		left = 0;
		right = _size - 1;

		while (right - left > 1) {
			unsigned int middle = left + (right - left) / 2;
			if (_score[middle] <= score)
				right = middle;
			else
				left = middle;
		}

		if (_score[right] < score) {
			if (_size < _capacity)
				++_size;

			for (int i = right + 1; i < _size; ++i) {
				_score[i] = _score[i - 1];
				_names[i] = _names[i - 1];
			}

			_score[right] = score;
			_names[right] = name;
		}
		else if (_size < _capacity){
			_score[_size] = score;
			_names[_size] = name;
			++_size;
		}
	}

	void Clear() {
		_size = 0;
	}

	const ScoreboardMember operator[](unsigned int position) const {
		assert(position < _size, "Position out of bounds of scorboard.");
		ScoreboardMember ret(_names[position], _score[position]);
		return ret;
	}

	ScoreboardMember operator[](unsigned int position) {
		assert(position < _size, "Position out of bounds of scorboard.");
		ScoreboardMember ret(_names[position], _score[position]);
		return ret;
	}


};


#endif