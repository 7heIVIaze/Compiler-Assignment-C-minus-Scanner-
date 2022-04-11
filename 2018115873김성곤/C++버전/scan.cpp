#include <iostream>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <string>
using namespace std;

constexpr auto ID_LENGTH = 40; // ID의 길이
constexpr auto NUM_KEY = 6; // 예약어의 종류는 6개이므로
// #define MAX 1024 // 문자열 담을 변수 길이
constexpr auto NUM_SYM = 16;

int line = 1; // 라인을 알기 위하여

enum cmsymbol {
	cnull = -1, cplus, cminus, cmultiple, cmodify, cless, clesse, cmore, cmoree, cequal, cnotequal, cassign, csemicolon, ccomma, clparenthese,
	// 순서대로 +, -, *, /, <, <=, >, >=, ==, !=, =, ;, ,, (
	crparenthese, clbracket, crbracket, clbrace, crbrace, cnumber, cidentifier, celse, cif, cint, creturn, cvoid, cwhile, ceof, cerror, cloop
	// 순서대로 ), [, ], {, }, 숫자, 식별자, else, if, int, return, void, while, EOF, 에러상황, 주석이 안 끝난 상황
};
// 심볼 타입 테이블

struct tokenType {
	int type; // 어떤 타입인지
	char error; // 에러 상황일 경우 담을 변수
	string sym; // 심볼을 담을 변수. 3인 이유는 <=같은 경우
	int number; //숫자
	string id; // 식별자
};

string keyword[NUM_KEY]{
	"else", "if", "int", "return", "void", "while"
}; // 키워드 테이블

enum cmsymbol keynum[NUM_KEY]{
	celse, cif, cint, creturn, cvoid, cwhile
}; // 키워드 타입 테이블

char symbolcase[NUM_SYM]{
	'!', '(', ')', '*', '+', ',', '-', '/', ';', '<',
	'=', '>', '[', ']', '{', '}'
}; // 심볼 테이블

enum cmsymbol symnum[NUM_SYM]{
	cnotequal, clparenthese, crparenthese, cmultiple, cplus, ccomma, cminus, cmodify, csemicolon, cless, 
	cassign, cmore, clbracket, crbracket, clbrace, crbrace
}; // 심볼 타입 테이블

void LexicalError(int option)
{
	cout << "==== " << line << " line The Lexical Error ====" << endl;
	switch (option)
	{
	case 1: cout << "an identifier length must be less than 40" << endl; break;
	case 2: cout << "! must need =" << endl; break;
	case 3: cout << "Invalid Character" << endl; break;
	case 4: cout << "Stop before ending" << endl; break;
	}
} // 에러가 발생했을 경우 에러창에 띄울 문구들

string GetID(char firstcharacter, ifstream& source)
{
	string id;
	char ch = firstcharacter;
	int index = 0;
	do
	{
		if (index < ID_LENGTH)
			id += ch;
		source.get(ch);
	} while (isalpha(ch));
	cin.putback(ch);
	source.seekg(-1, ios::cur);

	return id;
}

int GetNumber(char firstcharacter, ifstream& source)
{
	char ch = firstcharacter;
	int number = 0;

	if (ch != '0') {// 받은 첫 글자가 0이 아닐 경우
		do
		{
			number = number * 10 + (int)(ch - '0');
			source.get(ch);
		} while (isdigit(ch));
		cin.putback(ch);
		source.seekg(-1, ios::cur);
	}
	else
		number = 0;
	return number;
}

int GetKeyword(string id)
{
	int index = 0;
	int start = 0, end = NUM_KEY - 1;

	do
	{
		index = (start + end) / 2;

		if (id == keyword[index])
			return index;
		else if (id > keyword[index])
			start = index + 1;
		else
			end = index - 1;
	} while (start<=end);

	return NUM_KEY;
} // 키워드 테이블에서 찾기 위한 바이너리 서치

int getSymbol(char sym)
{
	int index = 0;
	int start = 0, end = NUM_SYM - 1;

	do
	{
		index = (start + end) / 2;
		if (sym == symbolcase[index])
			return index;
		else if (sym > symbolcase[index])
			start = index + 1;
		else
			end = index - 1;
	} while (start<=end);

	return NUM_SYM;
} // 심볼 테이블에서 찾기 위한 바이너리 서치

tokenType Scanner(ifstream& source)
{
	char ch;
	string id;
	int index = 0;
	tokenType token;
	token.type = (cnull);

	do
	{
		source.get(ch); // 일단 한 글자 받아놓음
		while (isspace(ch)) {
			if (ch == '\n')
				line++;
			source.get(ch);
			if (source.eof()) {
				token.type = ceof;
				return token;
			}
		}

		if (isalpha(ch)) {
			id = GetID(ch, source);
			index = GetKeyword(id);
			if (index < NUM_KEY) {
				token.type = keynum[index];
				token.id = keyword[index];
				break;
			}
			else {
				token.type = cidentifier;
				token.id = id;
			}
		}

		else if (isdigit(ch)) {
			token.type = cnumber;
			token.number = GetNumber(ch, source);
		}

		else {
			index = getSymbol(ch);

			switch (index)
			{
			case 0: // !인 경우
				source.get(ch);
				if (ch == '=') {
					token.type = symnum[index];
					token.sym = "!=";
				}
				else {

					token.type = cerror;
					token.error = '!';
					source.seekg(-1, ios::cur);
					cin.putback(ch);
				}
				break;
			case 7: // /인 경우
				source.get(ch);
				if (ch == '*') { // 주석의 시작
					do
					{
						if (source.eof()) { // 만약 파일을 끝까지 읽었다면
							LexicalError(4);
							token.type = cloop;
							return token;
						}
						source.get(ch);
						while (ch != '*') {
							source.get(ch);
							if (source.eof()) { // 파일을 끝까지 읽었다면
								LexicalError(4);
								token.type = cloop;
								return token;
							}
							if (ch == '\n')
								line++;
						}
						source.get(ch);
					} while (ch != '/'); // 주석 탈출
				}
				else { // 나누기일 경우
					source.seekg(-1, ios::cur);
					token.type = cmodify;
					token.sym = "/";
					cin.putback(ch);
				}
				break;
			case 9: // <인 경우
				source.get(ch);
				if (ch == '=') { // <=일 경우
					token.type = clesse;
					token.sym = "<=";
				}
				else { // <일 경우
					source.seekg(-1, ios::cur);
					token.type = cless;
					token.sym = "<";
					cin.putback(ch);
				}
				break;
			case 10: // =인 경우
				source.get(ch);
				if (ch == '=') { // ==일 경우
					token.type = cequal;
					token.sym = "==";
				}
				else {  // =일 경우
					source.seekg(-1, ios::cur);
					token.type = cassign;
					token.sym = "=";
					cin.putback(ch);
				}
				break;
			case 11: // >인 경우
				source.get(ch);
				if (ch == '=') { // >=일 경우
					token.type = cmoree;
					token.sym = ">=";
				}
				else { // >일 경우
					source.seekg(-1, ios::cur);
					token.type = cmore;
					token.sym = ">";
					cin.putback(ch);
				}
				break;
			case NUM_SYM: // 예약어 테이블에 없는 경우
				cout << "Current Character: " << ch << endl;
				token.type = cerror;
				token.error = ch;
				LexicalError(3);
				break;
			default:
				token.type = symnum[index];
				token.sym = symbolcase[index];
				break;
			}
		}
		// 심볼 테이블 사용버전
/*
		else {
			switch (ch)
			{
			case '+':
				token.type = cplus;
				token.sym = "+";
				break;
			case '-':
				token.type = cminus;
				token.sym = "-";
				break;
			case '*':
				token.type = cmultiple;
				token.sym = "*";
				break;
			case '/':
				source.get(ch);
				if (ch == '*') { // 주석의 시작
					do
					{
						if (source.eof()) { // 만약 파일을 끝까지 읽었다면
							LexicalError(4);
							token.type = cloop;
							return token;
						}
						source.get(ch);
						while (ch != '*') {
							source.get(ch);
							if (source.eof()) { // 파일을 끝까지 읽었다면
								LexicalError(4);
								token.type = cloop;
								return token;
							}
							if (ch == '\n')
								line++;
						}
						source.get(ch);
					} while (ch != '/'); // 주석 탈출
				}
				else { // 나누기일 경우
					source.seekg(-1, ios::cur);
					token.type = cmodify;
					token.sym = "/";
					cin.putback(ch);
				}
				break;
			case '<':
				source.get(ch);
				if (ch == '=') { // <=일 경우
					token.type = clesse;
					token.sym = "<=";
				}
				else { // <일 경우
					source.seekg(-1, ios::cur);
					token.type = cless;
					token.sym = "<";
					cin.putback(ch);
				}
				break;
			case '>':
				source.get(ch);
				if (ch == '=') { // >=일 경우
					token.type = cmoree;
					token.sym = ">=";
				}
				else { // >일 경우
					source.seekg(-1, ios::cur);
					token.type = cmore;
					token.sym = ">";
					cin.putback(ch);
				}
				break;
			case '=':
				source.get(ch);
				if (ch == '=') { // ==일 경우
					token.type = cequal;
					token.sym = "==";
				}
				else {  // =일 경우
					source.seekg(-1, ios::cur);
					token.type = cassign;
					token.sym = "=";
					cin.putback(ch);
				}
				break;
			case '!':
				source.get(ch);
				if (ch == '=') { // !=일 경우
					token.type = cnotequal;
					token.sym = "!=";
				}
				else { // !일 경우 (ERROR)임
					source.seekg(-1, ios::cur);
					LexicalError(2);
					token.type = cerror;
					token.error = '!';
					cin.putback(ch);
				}
				break;
			case ';':
				token.type = csemicolon;
				token.sym = ";";
				break;
			case ',':
				token.type = ccomma;
				token.sym = ",";
				break;
			case '(':
				token.type = clparenthese;
				token.sym = "(";
				break;
			case ')':
				token.type = crparenthese;
				token.sym = ")";
				break;
			case '[':
				token.type = clbracket;
				token.sym = "[";
				break;
			case ']':
				token.type = crbracket;
				token.sym = "]";
				break;
			case '{':
				token.type = clbrace;
				token.sym = "{";
				break;
			case '}':
				token.type = crbrace;
				token.sym = "}";
				break;
			default: {
				cout << "Current Character: " << ch << endl;
				token.type = cerror;
				token.error = ch;
				LexicalError(3);
				break;
			}
			}
			//getSymbol(ch);
		}*/
		// 심볼 테이블 안 쓴 버전
	} while (token.type == cnull);

	return token;
};

int main(int argc, char* argv[])
{
	ifstream source;
	ifstream copy;
	ofstream result;

	int i;
	int copyline = 1;
	string buffer;
	tokenType a;
	if (argc != 3) {
		cout << "Usage: name <sourcefile> <resultfile>" << endl;
		exit(1);
	}

	copy.open(argv[1]);
	source.open(argv[1]);
	result.open(argv[2]);

	do
	{
		a = Scanner(source);

		while (copyline <= line) {
			getline(copy, buffer);
			result << copyline++ << ": " << buffer << endl;
		}

		if (a.type < 19) // 심볼로 밝혀졌을 경우
			result << "\t" << line << ": Symbol: " << a.sym << endl;
		else if (a.type == 19) // number로 밝혀졌을 경우
			result << "\t" << line << ": Number, Value = " << a.number << endl;
		else if (a.type == 20) // identifier로 밝혀졌을 경우
			result << "\t" << line << ": ID, Name = " << a.id << endl;
		else if (a.type > 20 && a.type < 27) // 키워드로 밝혀졌을 경우
			result << "\t" << line << ": Reserved word: " << a.id << endl;
		else if (a.type == 27) // EOF일 경우
			result << "\t" << line << ": EOF" << endl;
		else if (a.type == 29) // loop에서 못 나왔을 경우
			result << "\t" << line << ": Stop before Ending the Comment" << endl;
		else
			result << "\t" << line << ": ERROR: " << a.error << endl;

	} while (!(source.eof()));

	copy.close();
	result.close();
	source.close();

	return 0;
}