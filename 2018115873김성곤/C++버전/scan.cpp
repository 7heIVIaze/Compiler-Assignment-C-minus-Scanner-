#include <iostream>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <string>
using namespace std;

constexpr auto ID_LENGTH = 40; // ID�� ����
constexpr auto NUM_KEY = 6; // ������� ������ 6���̹Ƿ�
// #define MAX 1024 // ���ڿ� ���� ���� ����
constexpr auto NUM_SYM = 16;

int line = 1; // ������ �˱� ���Ͽ�

enum cmsymbol {
	cnull = -1, cplus, cminus, cmultiple, cmodify, cless, clesse, cmore, cmoree, cequal, cnotequal, cassign, csemicolon, ccomma, clparenthese,
	// ������� +, -, *, /, <, <=, >, >=, ==, !=, =, ;, ,, (
	crparenthese, clbracket, crbracket, clbrace, crbrace, cnumber, cidentifier, celse, cif, cint, creturn, cvoid, cwhile, ceof, cerror, cloop
	// ������� ), [, ], {, }, ����, �ĺ���, else, if, int, return, void, while, EOF, ������Ȳ, �ּ��� �� ���� ��Ȳ
};
// �ɺ� Ÿ�� ���̺�

struct tokenType {
	int type; // � Ÿ������
	char error; // ���� ��Ȳ�� ��� ���� ����
	string sym; // �ɺ��� ���� ����. 3�� ������ <=���� ���
	int number; //����
	string id; // �ĺ���
};

string keyword[NUM_KEY]{
	"else", "if", "int", "return", "void", "while"
}; // Ű���� ���̺�

enum cmsymbol keynum[NUM_KEY]{
	celse, cif, cint, creturn, cvoid, cwhile
}; // Ű���� Ÿ�� ���̺�

char symbolcase[NUM_SYM]{
	'!', '(', ')', '*', '+', ',', '-', '/', ';', '<',
	'=', '>', '[', ']', '{', '}'
}; // �ɺ� ���̺�

enum cmsymbol symnum[NUM_SYM]{
	cnotequal, clparenthese, crparenthese, cmultiple, cplus, ccomma, cminus, cmodify, csemicolon, cless, 
	cassign, cmore, clbracket, crbracket, clbrace, crbrace
}; // �ɺ� Ÿ�� ���̺�

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
} // ������ �߻����� ��� ����â�� ��� ������

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

	if (ch != '0') {// ���� ù ���ڰ� 0�� �ƴ� ���
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
} // Ű���� ���̺��� ã�� ���� ���̳ʸ� ��ġ

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
} // �ɺ� ���̺��� ã�� ���� ���̳ʸ� ��ġ

tokenType Scanner(ifstream& source)
{
	char ch;
	string id;
	int index = 0;
	tokenType token;
	token.type = (cnull);

	do
	{
		source.get(ch); // �ϴ� �� ���� �޾Ƴ���
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
			case 0: // !�� ���
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
			case 7: // /�� ���
				source.get(ch);
				if (ch == '*') { // �ּ��� ����
					do
					{
						if (source.eof()) { // ���� ������ ������ �о��ٸ�
							LexicalError(4);
							token.type = cloop;
							return token;
						}
						source.get(ch);
						while (ch != '*') {
							source.get(ch);
							if (source.eof()) { // ������ ������ �о��ٸ�
								LexicalError(4);
								token.type = cloop;
								return token;
							}
							if (ch == '\n')
								line++;
						}
						source.get(ch);
					} while (ch != '/'); // �ּ� Ż��
				}
				else { // �������� ���
					source.seekg(-1, ios::cur);
					token.type = cmodify;
					token.sym = "/";
					cin.putback(ch);
				}
				break;
			case 9: // <�� ���
				source.get(ch);
				if (ch == '=') { // <=�� ���
					token.type = clesse;
					token.sym = "<=";
				}
				else { // <�� ���
					source.seekg(-1, ios::cur);
					token.type = cless;
					token.sym = "<";
					cin.putback(ch);
				}
				break;
			case 10: // =�� ���
				source.get(ch);
				if (ch == '=') { // ==�� ���
					token.type = cequal;
					token.sym = "==";
				}
				else {  // =�� ���
					source.seekg(-1, ios::cur);
					token.type = cassign;
					token.sym = "=";
					cin.putback(ch);
				}
				break;
			case 11: // >�� ���
				source.get(ch);
				if (ch == '=') { // >=�� ���
					token.type = cmoree;
					token.sym = ">=";
				}
				else { // >�� ���
					source.seekg(-1, ios::cur);
					token.type = cmore;
					token.sym = ">";
					cin.putback(ch);
				}
				break;
			case NUM_SYM: // ����� ���̺� ���� ���
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
		// �ɺ� ���̺� ������
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
				if (ch == '*') { // �ּ��� ����
					do
					{
						if (source.eof()) { // ���� ������ ������ �о��ٸ�
							LexicalError(4);
							token.type = cloop;
							return token;
						}
						source.get(ch);
						while (ch != '*') {
							source.get(ch);
							if (source.eof()) { // ������ ������ �о��ٸ�
								LexicalError(4);
								token.type = cloop;
								return token;
							}
							if (ch == '\n')
								line++;
						}
						source.get(ch);
					} while (ch != '/'); // �ּ� Ż��
				}
				else { // �������� ���
					source.seekg(-1, ios::cur);
					token.type = cmodify;
					token.sym = "/";
					cin.putback(ch);
				}
				break;
			case '<':
				source.get(ch);
				if (ch == '=') { // <=�� ���
					token.type = clesse;
					token.sym = "<=";
				}
				else { // <�� ���
					source.seekg(-1, ios::cur);
					token.type = cless;
					token.sym = "<";
					cin.putback(ch);
				}
				break;
			case '>':
				source.get(ch);
				if (ch == '=') { // >=�� ���
					token.type = cmoree;
					token.sym = ">=";
				}
				else { // >�� ���
					source.seekg(-1, ios::cur);
					token.type = cmore;
					token.sym = ">";
					cin.putback(ch);
				}
				break;
			case '=':
				source.get(ch);
				if (ch == '=') { // ==�� ���
					token.type = cequal;
					token.sym = "==";
				}
				else {  // =�� ���
					source.seekg(-1, ios::cur);
					token.type = cassign;
					token.sym = "=";
					cin.putback(ch);
				}
				break;
			case '!':
				source.get(ch);
				if (ch == '=') { // !=�� ���
					token.type = cnotequal;
					token.sym = "!=";
				}
				else { // !�� ��� (ERROR)��
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
		// �ɺ� ���̺� �� �� ����
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

		if (a.type < 19) // �ɺ��� �������� ���
			result << "\t" << line << ": Symbol: " << a.sym << endl;
		else if (a.type == 19) // number�� �������� ���
			result << "\t" << line << ": Number, Value = " << a.number << endl;
		else if (a.type == 20) // identifier�� �������� ���
			result << "\t" << line << ": ID, Name = " << a.id << endl;
		else if (a.type > 20 && a.type < 27) // Ű����� �������� ���
			result << "\t" << line << ": Reserved word: " << a.id << endl;
		else if (a.type == 27) // EOF�� ���
			result << "\t" << line << ": EOF" << endl;
		else if (a.type == 29) // loop���� �� ������ ���
			result << "\t" << line << ": Stop before Ending the Comment" << endl;
		else
			result << "\t" << line << ": ERROR: " << a.error << endl;

	} while (!(source.eof()));

	copy.close();
	result.close();
	source.close();

	return 0;
}