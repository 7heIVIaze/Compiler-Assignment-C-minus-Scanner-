/*
	Compiler Assignment
	Cminus Scanner in C
*/
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define NUM_KEYWORDS 6 // ������� ������ 6���̹Ƿ�
#define ID_LENGTH 40 // ID�� ����
#define MAX 1024 // ���ڿ� ���� ���� ����
#define NUM_SYMBOLS 16 // �ɺ� ���� 16��

int line = 1;

enum cmsymbol {
	cnull = -1, cplus, cminus, cmultiple, cmodify, cless, clesse, cmore, cmoree, cequal, cnotequal, cassign, csemicolon, ccomma, clparenthese,
	// ������� +, -, *, /, <, <=, >, >=, ==, !=, =, ;, ,, (
	// ������� 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13
	crparenthese, clbracket, crbracket, clbrace, crbrace, cnumber, cidentifier, celse, cif, cint, creturn, cvoid, cwhile, ceof, cerror, cloop
	// ������� ), [, ], {, }, ����, �ĺ���, else, if, int, return, void, while, EOF, ������Ȳ, �ּ��� �� ���� ��Ȳ
	// ������� 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29
};
// �ɺ� Ÿ�� ���̺�

struct tokenType {
	int type; // � Ÿ������
	char error; // ���� ��Ȳ�� ��� ���� ����
	char sym[3]; // �ɺ��� ���� ����. 3�� ������ <=���� ���
	int number; //����
	char id[ID_LENGTH]; // �ĺ���
};

const char *keyword[NUM_KEYWORDS] = {
	"else", "if", "int", "return", "void", "while"
}; // Ű���� ���̺�

enum cmsymbol keynum[NUM_KEYWORDS] = {
	celse, cif, cint, creturn, cvoid, cwhile
}; // Ű���� Ÿ�� ���̺�

char symbolcase[NUM_SYMBOLS] = {
	'!', '(', ')', '*', '+', ',', '-', '/', ';', '<',
	'=', '>', '[', ']', '{', '}'
}; // �ɺ� ���̺�
/*	"!", "(", ")", "*", "+", ",", "-", "/", ";", "<",
	"=", ">", "[", "]", "{", "}"*/

enum cmsymbol symnum[NUM_SYMBOLS] = {
	cnotequal, clparenthese, crparenthese, cmultiple, cplus, ccomma, cminus, cmodify, csemicolon, cless,
	cassign, cmore, clbracket, crbracket, clbrace, crbrace
}; // �ɺ� Ÿ�� ���̺�

void LexicalError(int option)
{
	fprintf(stderr, "==== %d line The Lexical Error ====\n", line);
	switch (option)
	{
	case 1: fprintf(stderr, "an identifier length must be less than 40\n"); break;
	case 2: fprintf(stderr, "! must need =\n"); break;
	case 3: fprintf(stderr, "Invalid Character\n"); break;
	case 4: fprintf(stderr, "Stop before ending\n"); break;
	}
} // ������ �߻����� ��� ����â�� ��� ������

char *GetID(char firstcharacter, FILE *source)
{
	char id[ID_LENGTH];
	char ch = firstcharacter;
	int index = 0;
	do
	{
		if (index < ID_LENGTH)
			id[index++] = ch;
		ch = fgetc(source);
	} while (isalpha(ch));
	id[index] = '\0';
	ungetc(ch, stdin);
	fseek(source, -1, SEEK_CUR);

	return id;
} // �ĺ��ڸ� ���ϴ� �Լ�

int GetNumber(char firstcharacter, FILE* source)
{
	char ch = firstcharacter;
	int number = 0;

	if (ch != '0') {// ���� ù ���ڰ� 0�� �ƴ� ���
		do
		{
			number = number * 10 + (int)(ch - '0');
			ch = fgetc(source);
		} while (isdigit(ch));
		ungetc(ch, stdin);
		fseek(source, -1, SEEK_CUR);
	}
	else
		number = 0;
	return number;
} // ���ڸ� ���ϴ� �Լ�

int GetKeyword(char *id)
{
	int index = 0;
	int start = 0, end = NUM_KEYWORDS - 1;

	while (start <= end) {
		index = (start + end) / 2;

		if (strcmp(id, keyword[index]) == 0)
			return index;
		else if (strcmp(id, keyword[index]) > 0)
			start = index + 1;
		else if (strcmp(id, keyword[index]) < 0)
			end = index - 1;
	} 

	return -1;
} // Ű���� ���̺��� ã�� ���� ���̳ʸ� ��ġ


int getSymbol(char sym)
{
	int index = 0;
	int start = 0, end = NUM_SYMBOLS - 1;

	do
	{
		index = (start + end) / 2;
		if (sym == symbolcase[index])
			return index;
		else if (sym > symbolcase[index])
			start = index + 1;
		else
			end = index - 1;
	} while (start <= end);

	return NUM_SYMBOLS;
} // �ɺ� ���̺��� ã�� ���� ���̳ʸ� ��ġ


struct tokenType Scanner(FILE* source) 
{
	char ch;
	char id[ID_LENGTH] = "";
	int index = 0;
	struct tokenType token;
	token.type = (cnull);

	do {
		ch = fgetc(source); // �ϴ� �� ���� �޾Ƴ���
		while (isspace(ch)) { 
			if (ch == '\n') // ������ ��� ���� ���ڸ� �ø�
				line++;
			ch = fgetc(source);
			if (feof(source)) { // ���� ���� ������ �о��� ��� eof�� �Ǵ�
				token.type = ceof;
				return token; 
			}
		} // ���� ���ڰ� ������ ���

		if (isalpha(ch)) { 
			/*do
			{
				if (index < ID_LENGTH)
					id[i++] = ch;
				ch = fgetc(source);
			} while (isalpha(ch));
			ungetc(ch, stdin);
			fseek(source, -1, SEEK_CUR);
			*/
			strcpy(id, GetID(ch, source));
			index = GetKeyword(id); // Ű�������� ���̳ʸ� ��ġ�� ã��
			if (index == -1) { 
				token.type = cidentifier;
				strcpy(token.id, id);
			} // ���� ��� �ĺ��ڷ�
			else {
				token.type = keynum[index];
				strcpy(token.id, keyword[index]);
				break;
			} // ���̺� ���� ��� Ű�����
		} // ���� ���ڰ� ���ڿ��� ���

		else if (isdigit(ch)) {
			token.type = cnumber;
			token.number = GetNumber(ch, source);
		} // ���� ���ڰ� ���ڿ��� ���

		else {
			index = getSymbol(ch);

			switch (index)
			{
			case 0: // !�� ���
				ch = fgetc(source);
				if (ch == '=') {
					token.type = symnum[index];
					strcpy(token.sym, "!=");
				}
				else {
					token.type = cerror;
					token.error = '!';
					fseek(source, -1, SEEK_CUR);
					ungetc(ch, stdin);
				}
				break;
			case 7: // /�� ���
				ch = fgetc(source);
				if (ch == '*') { // �ּ��� ����
					do
					{
						if (feof(source)) { // ���� ������ ������ �о��ٸ�
							LexicalError(4);
							token.type = cloop;
							return token;
						}
						ch = fgetc(source);
						while (ch != '*') {
							ch = fgetc(source);
							if (feof(source)) { // ������ ������ �о��ٸ�
								LexicalError(4);
								token.type = cloop;
								return token;
							}
							if (ch == '\n')
								line++;
						}
						ch = fgetc(source);
					} while (ch != '/'); // �ּ� Ż��
				}
				else { // �������� ���
					fseek(source, -1, SEEK_CUR);
					token.type = cmodify;
					strcpy(token.sym, "/");
					ungetc(ch, stdin);
				}
				break;
			case 9: // <�� ���
				ch = fgetc(source);
				if (ch == '=') { // <=�� ���
					token.type = clesse;
					strcpy(token.sym, "<=");
				}
				else { // <�� ���
					fseek(source, -1, SEEK_CUR);
					token.type = cless;
					strcpy(token.sym, "<");
					ungetc(ch, stdin);
				}
				break;
			case 10: // =�� ���
				ch = fgetc(source);
				if (ch == '=') { // ==�� ���
					token.type = cequal;
					strcpy(token.sym, "==");
				}
				else {  // =�� ���
					fseek(source, -1, SEEK_CUR);
					token.type = cassign;
					strcpy(token.sym, "=");
					ungetc(ch, stdin);
				}
				break;
			case 11: // >�� ���
				ch = fgetc(source);
				if (ch == '=') { // >=�� ���
					token.type = cmoree;
					strcpy(token.sym, ">=");
				}
				else { // >�� ���
					fseek(source, -1, SEEK_CUR);
					token.type = cmore;
					strcpy(token.sym, ">");
					ungetc(ch, stdin);
				}
				break;
			case NUM_SYMBOLS: // ����� ���̺� ���� ���
				printf("Current Character: %c\n", ch);
				token.type = cerror;
				token.error = ch;
				LexicalError(3);
				break;
			case EOF:
				token.type = ceof;
				break;
			default:
				token.type = symnum[index];
				token.sym[0] = symbolcase[index];
				token.sym[1] = '\0';
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
				strcpy(token.sym, "+");
				break;
			case '-':
				token.type = cminus;
				strcpy(token.sym, "-");
				break;
			case '*':
				token.type = cmultiple;
				strcpy(token.sym, "*");
				break;
			case '/':
				ch = fgetc(source);
				if (ch == '*') { // �ּ��� ����
					do
					{
						if (feof(source)) { // ���� ������ ������ �о��ٸ�
							LexicalError(4);
							token.type = cloop;
							return token;
						}
						ch = fgetc(source);
						while (ch != '*') {
							ch = fgetc(source);
							if (feof(source)) { // ������ ������ �о��ٸ�
								LexicalError(4);
								token.type = cloop;
								return token;
							}
							if (ch == '\n')
								line++;
						}
						ch = fgetc(source);
					} while (ch != '/'); // �ּ� Ż��
				}
				else { // �������� ���
					fseek(source, -1, SEEK_CUR);
					token.type = cmodify;
					strcpy(token.sym, "/");
					ungetc(ch, stdin);
				}
				break;
			case '<':
				ch = fgetc(source);
				if (ch == '=') { // <=�� ���
					token.type = clesse;
					strcpy(token.sym, "<=");
				}
				else { // <�� ���
					fseek(source, -1, SEEK_CUR);
					token.type = cless;
					strcpy(token.sym, "<");
					ungetc(ch, stdin);
				}
				break;
			case '>':
				ch = fgetc(source);
				if (ch == '=') { // >=�� ���
					token.type = cmoree;
					strcpy(token.sym, ">=");
				}
				else { // >�� ���
					fseek(source, -1, SEEK_CUR);
					token.type = cmore;
					strcpy(token.sym, ">");
					ungetc(ch, stdin);
				}
				break;
			case '=':
				ch = fgetc(source);
				if (ch == '=') { // ==�� ���
					token.type = cequal;
					strcpy(token.sym, "==");
				}
				else {  // =�� ���
					fseek(source, -1, SEEK_CUR);
					token.type = cassign;
					strcpy(token.sym, "=");
					ungetc(ch, stdin);
				}
				break;
			case '!':
				ch = fgetc(source);
				if (ch == '=') { // !=�� ���
					token.type = cnotequal;
					strcpy(token.sym, "!=");
				}
				else { // !�� ��� (ERROR)��
					fseek(source, -1, SEEK_CUR);
					LexicalError(2);
					token.type = cerror;
					token.error = '!';
					ungetc(ch, stdin);
				}
				break;
			case ';':
				token.type = csemicolon;
				strcpy(token.sym, ";");
				break;
			case ',':
				token.type = ccomma;
				strcpy(token.sym, ",");
				break;
			case '(':
				token.type = clparenthese;
				strcpy(token.sym, "(");
				break;
			case ')':
				token.type = crparenthese;
				strcpy(token.sym, ")");
				break;
			case '[':
				token.type = clbracket;
				strcpy(token.sym, "[");
				break;
			case ']':
				token.type = crbracket;
				strcpy(token.sym, "]");
				break;
			case '{':
				token.type = clbrace;
				strcpy(token.sym, "{");
				break;
			case '}':
				token.type = crbrace;
				strcpy(token.sym, "}");
				break;
			default: {
				fprintf(stderr, "Current Character: %c\n", ch);
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

int getLine(char* origin)
{
	int index = 0, number = 0;
	do {
		number = number * 0 + (int)(origin[index] - '\0');
		index++;
	} while (origin[index] != ':');

	return number;
} // ���� ��ȣ �񱳿�

void FileCopy(char* finename)
{
	FILE* copy; copy = fopen("copy.txt", "w"); // ���� ���� �غ�
	FILE* source; source = fopen(finename, "r");
	int textline = 1;
	char buffer[MAX];

	do {
		fgets(buffer, sizeof(buffer), source);
		fprintf(copy, "%d: %s", textline++, buffer);
	} while (!feof(source));

	rewind(source); // Ȥ�� �𸣴� �ʱ�� ����
	fclose(copy);
	fclose(source);
} // ��ĳ�� pdf ����ó�� ������ ��Ÿ���� ���� ���� ��ȣ�� �ۼ��� ���� ī������ ����

void WriteResult(char* filename)
{
	FILE* temp; temp = fopen("temp.txt", "r"); // ������� �ӽ÷� ������ ����
	FILE* copy; copy = fopen("copy.txt", "r"); // �������Ͽ� ������ ǥ���� ī�� ����
	FILE* result; result = fopen(filename, "w"); // �����͸� ������ ���� ����
	int cur = 0;
	char t[MAX], c[MAX]; // temp���� ���� ������ copy���� ���� ����

	do {
		if (fgets(c, sizeof(c), copy) == NULL) break;
		fprintf(result, "%s", c); // ī�� ���Ͽ��� �� ������ �о ��� ���Ͽ� �ۼ���
		do {
			cur = ftell(temp); // ���� ���� ������ ��ġ
			if (fgets(t, MAX, temp) == NULL) break; // �ӽ� ���Ͽ� ������ ������ ���� ����
			if (getLine(c) == getLine(t)) // c�� t�� ���� ��ȣ�� ������
				fprintf(result,"\t%s", t); // �ش� ���� ��� ���Ͽ� �ۼ�
			else {
				if (ftell(temp) > 0) {
					rewind(temp);
					fseek(temp, cur, SEEK_CUR);
				} // �ٸ� ��� �����͸� �б� ������ �ű�
				break;
			}
		} while (!(feof(temp)));

	} while (!(feof(copy)));

	fclose(temp);
	fclose(copy);
	fclose(result);
	remove("copy.txt");
	remove("temp.txt"); // copy���ϰ� temp������ ���̻� �ʿ� �����Ƿ� ����
}

int main(int argc, char* argv[])
{
	FILE* source;
	FILE* temp;

	int i;
	struct tokenType a;
	if (argc != 3) {
		fprintf(stderr, "Usage: name <sourcefile> <resultfile>\n");
		exit(1);
	}

	FileCopy(argv[1]);
	source = fopen(argv[1], "r");
	temp = fopen("temp.txt", "w");

	do
	{
		a = Scanner(source);

		if (a.type < 19) // �ɺ��� �������� ���
			fprintf(temp, "%d: Symbol: %s\n", line, a.sym);
		else if (a.type == 19) // number�� �������� ���
			fprintf(temp, "%d: Number, Value = %d\n", line, a.number);
		else if (a.type == 20) // identifier�� �������� ���
			fprintf(temp, "%d: ID, Name = %s\n", line, a.id);
		else if (a.type > 20 && a.type < 27) // Ű����� �������� ���
			fprintf(temp, "%d: Reserved word: %s\n", line, a.id);
		else if (a.type == 27) // EOF�� ���
			fprintf(temp, "%d: EOF\n", line);
		else if (a.type == 29) // loop���� �� ������ ���
			fprintf(temp, "%d: Stop before Ending the Comment\n", line);
		else
			fprintf(temp, "%d: ERROR: %c\n", line, a.error);

	} while (!(feof(source)));

	fclose(temp);
	fclose(source);

	WriteResult(argv[2]);

	return 0;
}