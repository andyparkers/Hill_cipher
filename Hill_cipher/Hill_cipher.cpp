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
#include <fstream>


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

	vector<vector<int>> GetMatrix() const {
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

	static int Gcd(int a, int b, int& x, int& y) {
		if (a == 0) {
			x = 0; y = 1;
			return b;
		}
		int x1, y1;
		int d = Gcd(b % a, a, x1, y1);
		x = y1 - (b / a) * x1;
		y = x1;
		return d;
	}

	[[nodiscard]] static bool IsCoprime(int a, int b) {
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

	void restrict_by_module(int module) {
		for (auto& vec : matrix) {
			for (auto& elem : vec) {
				elem %= module;
				if (elem < 0) {
					elem += module;
				}
			}
		}
	}

	static Matrix getKey(const string& key) {
		int size_len = int(sqrt(key.length()));
		const string& alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ???????????????????????????????????????????????????????????????? ,.:!?";
		vector<vector<int>> for_push(size_len);
		for (int i = 0; i < size_len; ++i) {
			for (int k = 0; k < size_len; ++k) {
				for_push[i].push_back(alphabet.find(key[k + i * size_len]));
			}
		}
		return { size_len, size_len, for_push };
	}

	static Matrix create_vector(const string& letter) {
		vector<vector<int>> word(letter.length());
		const string& alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ???????????????????????????????????????????????????????????????? ,.:!?";
		for (size_t i = 0; i < letter.length(); ++i) {
			word[i].push_back(alphabet.find(letter[i]));
		}
		return { 1, static_cast<int>(letter.length()), word };
	}

	void inverse_matrix(const string& key) {
		vector<vector<int>> result = matrix;
		int matrix_determinant = GetDeterminant(matrix);
		int det_m = (matrix_determinant % 64 < 0
			? matrix_determinant % 64 + 64
			: matrix_determinant % 64);
		int x = 0;
		int y = 0;
		Gcd(det_m, 64, x, y);
		int rev_det = 0;
		if (det_m < 0) {
			rev_det = (x > 0 ? x : -x);
		}
		else if (det_m > 0) {
			rev_det = x + 64;
		}
		for (size_t i = 0; i < colums; ++i) {
			for (size_t k = 0; k < colums; ++k) {
				vector<vector<int>> temp;
				for (size_t l = 0; l < colums; ++l) {
					if (l != i) {
						vector<int> for_push = matrix[l];
						for_push.erase(for_push.begin() + k);
						temp.push_back(for_push);
					}
				}
				result[i][k] = int(pow(-1, (i + k) % 2)) * GetDeterminant(temp) * rev_det % 64;
				if (result[i][k] < 0) {
					result[i][k] += 64;
				}
			}
		}
		for (size_t i = 0; i < colums; ++i) {
			for (size_t k = i; k < colums; ++k) {
				swap(result[i][k], result[k][i]);
			}
		}
		matrix = result;
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
			result.emplace_back(Matrix::create_vector(open_text.substr(i, key_lenght)));
		}
		return result;
	}
};


class Hill {
private:
	string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ???????????????????????????????????????????????????????????????? ,.:!?";

	string makeString(size_t size) const {
		srand(time(nullptr));
		string random_string(size, ' ');
		for (int i = 0; i < size; ++i) {
			random_string[i] = alphabet[rand() % alphabet.length()];
		}
		return random_string;
	}

public:
	string encrypt(const string& open_text, const string& key) const {
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
				Matrix symbol = m_key * matrix_open_text[i];
				symbol.restrict_by_module(64);
				answer += alphabet[symbol.GetMatrix()[k][0]];
			}
		}
		return answer;
	}

	string decrypt(const string& cipher_text, const string& key) const {
		const vector<Matrix>& text = Matrix::make_vectors_for_enc(cipher_text, key);
		vector<Matrix> open_text(text.size());
		Matrix reverse_matrix = Matrix::getKey(key);
		reverse_matrix.inverse_matrix(key);
		for (size_t i = 0; i < text.size(); ++i) {
			open_text[i] = reverse_matrix * text[i];
		}
		string answer = "";
		for (size_t i = 0; i < open_text.size(); ++i) {
			for (int k = 0; k < static_cast<int>(sqrt(key.length())); ++k) {
				answer += alphabet[open_text[i].GetMatrix()[k][0]];
			}
		}
		return answer;
	}

	vector<string> PrintKeys(size_t key_size, int keys_quantity) const {
		Hill object_for_check;
		map<string, int> keys;
		vector<string> to_return;
		int key_numbers = 0;
		string lets_try(key_size, ' ');
		while (1) {
			lets_try = makeString(key_size);
			if (object_for_check.keyCheck(lets_try) == 1 && keys[lets_try] == 0) {
				++keys[lets_try];
				++key_numbers;
				to_return.push_back(lets_try);
				if (key_numbers == keys_quantity) {
					break;
				}
			}
		}
		return to_return;
	}

	[[nodiscard]] bool keyCheck(const string& key) const {
		if (floor(sqrt(key.length())) != sqrt(key.length()) || key.length() == 1) {
			return 0;
		}
		const Matrix& matrix = Matrix::getKey(key);
		int matrix_determinant = Matrix::GetDeterminant(matrix.GetMatrix());
		int det_matrix = (matrix_determinant % 64 < 0
			? matrix_determinant % 64 + 64
			: matrix_determinant % 64);
		if (Matrix::IsCoprime(det_matrix, 64) == 1) {
			return 1;
		}
		return 0;
	}

	[[nodiscard]] bool TextCheck(const string& text) const {
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
	cout << "Enter text: ";
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
	string action = "";
	while (1) {
		GetKeyAndText(data, key);

		cout << "Enter \"encrypt\" or \"decrypt\", according to you need: ";
		getline(cin, action);

		if (action == "encrypt") {
			string cipher_text = hill.encrypt(data, key);
			cout << "Encrypted text: " << cipher_text << endl;
		}
		else if (action == "decrypt") {
			string answer = hill.decrypt(data, key);
			cout << "Decrypted text: " << answer << endl;
		}
		else {
			cout << "Wrong query, try again please" << endl;
		}

		data.clear();
		key.clear();
	}
}
