#include "pch.h"
#include <iostream>
#include <windows.h>
#include <cmath>
#include <string>
#include <exception>
#include <sstream>
#include <vector>
#include <iomanip>
#include <map>
#include <ctime>


using namespace std;


class Matrix {
private:
	int colums;
	int rows;
	vector<vector<int>> matrix;
public:
	Matrix() {
		int colums = 0;
		int rows = 0;
		vector<vector<int>> matrix{ 0 };
	}

	Matrix(int colums, int rows) {
		this->colums = colums;
		this->rows = rows;
		matrix.assign(rows, vector<int>(colums));
	}

	Matrix(int colums, int rows, const vector<vector<int>>& matrix) {
		this->colums = colums;
		this->rows = rows;
		this->matrix = matrix;
	}

	~Matrix() {}

	vector<vector<int>> getMatrix() const {
		return matrix;
	}

	static int GetDeterminant(const vector<vector<int>>& matrix) {
		int determinant = 0;
		if (matrix.size() == 1) {
			return matrix[0][0];
		}
		else if (matrix.size() == 2) {
			determinant = matrix[0][0] * matrix[1][1] - matrix[1][0] * matrix[0][1];
			return determinant;
		}
		else {
			for (size_t i = 0; i < matrix.size(); ++i) {
				vector<vector<int>> preliminary_matrix;
				for (size_t k = 1; k < matrix.size(); ++k) {
					vector<int> row;
					for (size_t z = 0; z < matrix.size(); ++z) {
						if (z != i) {
							row.push_back(matrix[k][z]);
						}
					}
					preliminary_matrix.push_back(row);
				}
				determinant = determinant + matrix[0][i] * int(pow(-1, i)) * GetDeterminant(preliminary_matrix);
			}
			return determinant;
		}
	}

	static int gcd(int a, int b, int & x, int & y) {
		if (a == 0) {
			x = 0; y = 1;
			return b;
		}
		int x1, y1;
		int d = gcd(b % a, a, x1, y1);
		x = y1 - (b / a) * x1;
		y = x1;
		return d;
	}

	static bool isCoprime(int a, int b) {
		for (int gcd = a; ; gcd = b, b = a % b, a = gcd) {
			if (!b) {
				return gcd == 1;
			}
		}
	}

	friend ostream& operator<<(ostream& stream, const Matrix& matrix) {
		bool first_time = 0;
		for (const vector<int>& vec : matrix.matrix) {
			if (first_time) {
				cout << endl;
			}
			if (!first_time) {
				first_time = 1;
			}
			for (const int elem : vec) {
				cout << elem << " ";
			}
		}
		return stream;
	}

	friend Matrix operator+(const Matrix& f_matrix, const Matrix& s_matrix) {
		Matrix result(f_matrix.colums, f_matrix.rows, f_matrix.matrix);
		if (f_matrix.colums == s_matrix.colums && f_matrix.rows == s_matrix.rows) {
			for (int i = 0; i < f_matrix.rows; ++i) {
				for (int k = 0; k < f_matrix.colums; ++k) {
					result.matrix[i][k] = f_matrix.matrix[i][k] + s_matrix.matrix[i][k];
				}
			}
		}
		return result;
	}

	friend Matrix operator*(const Matrix& f_matrix, const Matrix& s_matrix) {
		Matrix result(s_matrix.colums, f_matrix.rows);
		if (f_matrix.colums == s_matrix.rows) {
			for (int i = 0; i < f_matrix.rows; ++i) {
				for (int k = 0; k < s_matrix.colums; ++k) {
					result.matrix[i][k] = 0;
					for (int z = 0; z < f_matrix.colums; ++z) {
						result.matrix[i][k] += f_matrix.matrix[i][z] * s_matrix.matrix[z][k];
					}
					result.matrix[i][k] %= 64;
				}
			}
		}
		return result;
	}

	Matrix restrict_by_module(const Matrix& matrix, int module) {
		Matrix result = matrix;
		for (auto& vec : result.matrix) {
			for (auto& elem : vec) {
				elem %= module;
			}
		}
		return result;
	}

	static Matrix getKey(const string& key) {
		int size_len = int(sqrt(key.length()));
		const string& alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZабвгдежзийклмнопрстуфхцчшщъыьэюя ,.:!?";
		vector<vector<int>> for_push(size_len);
		for (int i = 0; i < size_len; ++i) {
			for (int k = 0; k < size_len; ++k) {
				for_push[i].push_back(alphabet.find(key[k + i * size_len]));
			}
		}
		return Matrix(size_len, size_len, for_push);
	}

	static Matrix create_vector(const string& letter) {
		vector<vector<int>> word(letter.length());
		const string& alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZабвгдежзийклмнопрстуфхцчшщъыьэюя ,.:!?";
		for (size_t i = 0; i < letter.length(); ++i) {
			word[i].push_back(alphabet.find(letter[i]));
		}
		return Matrix(1, letter.length(), word);
	}

	static Matrix inverse_matrix(const Matrix& m, const string& key) {
		size_t key_lenght = size_t(sqrt(key.length()));
		vector<vector<int>> result = m.matrix;
		int matrix_determinant = GetDeterminant(m.matrix);
		for (size_t i = 0; i < key_lenght; ++i) {
			for (size_t k = 0; k < key_lenght; ++k) {
				vector<vector<int>> temp;
				for (size_t l = 0; l < key_lenght; ++l) {
					if (l != i) {
						vector<int> for_push = m.matrix[l];
						for_push.erase(for_push.begin() + k);
						temp.push_back(for_push);
					}
				}
				result[i][k] = int(pow(-1, (i + k) % 2)) * GetDeterminant(temp) % 64;
			}
		}
		int det_m = (matrix_determinant % 64 < 0
			? matrix_determinant % 64 + 64
			: matrix_determinant % 64);
		int x = 0;
		int y = 0;
		gcd(det_m, 64, x, y);
		int rev_det = 0;
		if (det_m < 0) {
			rev_det = (x > 0 ? x : -x);
		}
		else if (det_m > 0) {
			rev_det = x + 64;
		}
		for (size_t i = 0; i < key_lenght; ++i) {
			for (size_t k = i; k < key_lenght; ++k) {
				swap(result[i][k], result[k][i]);
			}
		}
		for (auto& vec : result) {
			for (auto& elem : vec) {
				elem = elem * rev_det % 64;
				if (elem < 0) {
					elem += 64;
				}
			}
		}
		return Matrix(key_lenght, key_lenght, result);
	}

	static vector<Matrix> make_vectors_for_enc(const string& text, const string& key) {
		size_t key_lenght = size_t(sqrt(key.length()));
		string open_text = text;
		vector<Matrix> result;
		for (size_t i = 0; i < open_text.length(); i += key_lenght) {
			if (open_text.length() - i < key_lenght) {
				size_t iter_q = key_lenght - open_text.length() + i;
				for (size_t k = 0; k < iter_q; ++k) {
					open_text.push_back(' ');
				}
			}
			result.push_back(Matrix::create_vector(open_text.substr(i, key_lenght)));
		}
		return result;
	}
};


class Hill {
private:
	string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZабвгдежзийклмнопрстуфхцчшщъыьэюя ,.:!?";

	string makeString(size_t size) {
		srand(time(nullptr));
		const string alphabet = Hill::GetAlphabet();
		string random_string(size, ' ');
		for (int i = 0; i < size; ++i) {
			random_string[i] = alphabet[rand() % alphabet.length()];
		}
		return random_string;
	}

public:
	Hill() {}

	~Hill() {}

	static string GetAlphabet() {
		return "ABCDEFGHIJKLMNOPQRSTUVWXYZабвгдежзийклмнопрстуфхцчшщъыьэюя ,.:!?";
	}

	string encrypt(const string& open_text, const string& key) {
		int size_len = int(sqrt(key.length()));
		vector<vector<int>> for_push(size_len);
		for (int i = 0; i < size_len; ++i) {
			for (int k = 0; k < size_len; ++k) {
				for_push[i].push_back(alphabet.find(key[k + i * size_len]));
			}
		}
		Matrix m_key(size_len, size_len, for_push);
		const vector<Matrix>& matrix_open_text = Matrix::make_vectors_for_enc(open_text, key);
		string answer = "";
		for (size_t i = 0; i < matrix_open_text.size(); ++i) {
			for (int k = 0; k < size_len; ++k) {
				answer += alphabet[m_key.restrict_by_module(m_key * matrix_open_text[i], 64).getMatrix()[k][0]];
			}
		}
		return answer;
	}

	string decrypt(const string& cipher_text, const string& key) {
		const vector<Matrix>& text = Matrix::make_vectors_for_enc(cipher_text, key);
		vector<Matrix> open_text(text.size());
		const Matrix& reverse_matrix = Matrix::inverse_matrix(Matrix::getKey(key), key);
		cout << "Reverse matrix: " << endl;
		cout << reverse_matrix << endl;
		for (size_t i = 0; i < text.size(); ++i) {
			open_text[i] = reverse_matrix * text[i];
		}
		string answer = "";
		for (size_t i = 0; i < open_text.size(); ++i) {
			for (int k = 0; k < int(sqrt(key.length())); ++k) {
				answer += alphabet[open_text[i].getMatrix()[k][0]];
			}
		}
		return answer;
	}

	void PrintKeys(size_t key_size, int keys_quantity) {
		Hill object_for_check;
		const string& alphabet = Hill::GetAlphabet();
		map<string, int> keys;
		int key_numbers = 0;
		string lets_try(key_size, ' ');
		while (1) {
			lets_try = makeString(key_size);
			if (object_for_check.keyCheck(lets_try) == 1 && keys[lets_try] == 0) {
				++keys[lets_try];
				++key_numbers;
				if (key_numbers == keys_quantity) {
					break;
				}
			}
		}
		for (const auto& key : keys) {
			cout << key.first << endl;
		}
	}

	bool keyCheck(const string& key) const {
		if (floor(sqrt(key.length())) != sqrt(key.length()) || key.length() == 1) {
			return 0;
		}
		const Matrix& matrix = Matrix::getKey(key);
		int matrix_determinant = Matrix::GetDeterminant(matrix.getMatrix());
		int det_matrix = (matrix_determinant % 64 < 0
			? matrix_determinant % 64 + 64
			: matrix_determinant % 64);
		if (Matrix::isCoprime(det_matrix, 64) == 1) {
			cout << "Key: " << endl;
			cout << matrix << endl;
			return 1;
		}
		return 0;
	}

	bool TextCheck(const string& text) {
		for (const auto& letter : text) {
			if (alphabet.find(letter) == string::npos && letter != 'A') {
				return 0;
			}
		}
		return 1;
	}
};


void GetKeyAndText(string& text, string& key) {
	Hill object_for_check;
	cout << "Enter open text: ";
	getline(cin, text);
	while (object_for_check.TextCheck(text) != 1) {
		text.erase();
		cout << "At least one letter hasn't been found in the alphabet. Please try again." << endl;
		cout << "Enter open text: ";
		getline(cin, text);
	}
	cout << "Enter a key: ";
	getline(cin, key);
	while (object_for_check.keyCheck(key) != 1) {
		key.erase();
		cout << "Wrong key. Please try again." << endl;
		cout << "Enter a key: ";
		getline(cin, key);
	}
}

int main()
{
	SetConsoleOutputCP(1251);
	SetConsoleCP(1251);
	Hill hill;
	string data = "";
	string key = "";
	while (1) {
		GetKeyAndText(data, key);
		string cipher_text = hill.encrypt(data, key);
		cout << "Encrypted text: " << cipher_text << endl;
		string answer = hill.decrypt(cipher_text, key);
		cout << "Decrypted text: " << answer << endl;
		data.clear();
		key.clear();
	}
}
