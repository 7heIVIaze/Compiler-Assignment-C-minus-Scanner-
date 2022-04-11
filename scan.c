/*
	Compiler Assignment
	Cminus Scanner in C
*/
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define NUM_KEYWORDS 6 // 예약어의 종류는 6개이므로
#define ID_LENGTH 40 // ID의 길이
#define MAX 1024 // 문자열 담을 변수 길이
#define NUM_SYMBOLS 16 // 심볼 갯수 16개

int line = 1;

enum cmsymbol {
	cnull = -1, cplus, cminus, cmultiple, cmodify, cless, clesse, cmore, cmoree, cequal, cnotequal, cassign, csemicolon, ccomma, clparenthese,
	// 순서대로 +, -, *, /, <, <=, >, >=, ==, !=, =, ;, ,, (
	// 순서대로 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13
	crparenthese, clbracket, crbracket, clbrace, crbrace, cnumber, cidentifier, celse, cif, cint, creturn, cvoid, cwhile, ceof, cerror, cloop
	// 순서대로 ), [, ], {, }, 숫자, 식별자, else, if, int, return, void, while, EOF, 에러상황, 주석이 안 끝난 상황
	// 순서대로 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29
};
// 심볼 타입 테이블

struct tokenType {
	int type; // 어떤 타입인지
	char error; // 에러 상황일 경우 담을 변수
	char sym[3]; // 심볼을 담을 변수. 3인 이유는 <=같은 경우
	int number; //숫자
	char id[ID_LENGTH]; // 식별자
};

const char *keyword[NUM_KEYWORDS] = {
	"else", "if", "int", "return", "void", "while"
}; // 키워드 테이블

enum cmsymbol keynum[NUM_KEYWORDS] = {
	celse, cif, cint, creturn, cvoid, cwhile
}; // 키워드 타입 테이블

char symbolcase[NUM_SYMBOLS] = {
	'!', '(', ')', '*', '+', ',', '-', '/', ';', '<',
	'=', '>', '[', ']', '{', '}'
}; // 심볼 테이블
/*	"!", "(", ")", "*", "+", ",", "-", "/", ";", "<",
	"=", ">", "[", "]", "{", "}"*/

enum cmsymbol symnum[NUM_SYMBOLS] = {
	cnotequal, clparenthese, crparenthese, cmultiple, cplus, ccomma, cminus, cmodify, csemicolon, cless,
	cassign, cmore, clbracket, crbracket, clbrace, crbrace
}; // 심볼 타입 테이블

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
} // 에러가 발생했을 경우 에러창에 띄울 문구들

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
} // 식별자를 구하는 함수

int GetNumber(char firstcharacter, FILE* source)
{
	char ch = firstcharacter;
	int number = 0;

	if (ch != '0') {// 받은 첫 글자가 0이 아닐 경우
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
} // 숫자를 구하는 함수

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
} // 키워드 테이블에서 찾기 위한 바이너리 서치


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
} // 심볼 테이블에서 찾기 위한 바이너리 서치


struct tokenType Scanner(FILE* source)
{
	char ch;
	char id[ID_LENGTH] = "";
	int index = 0;
	struct tokenType token;
	token.type = (cnull);

	do {
		ch = fgetc(source); // 일단 한 글자 받아놓음
		while (isspace(ch)) { 
			if (ch == '\n') // 엔터일 경우 라인 숫자를 올림
				line++;
			ch = fgetc(source);
			if (feof(source)) { // 만약 파일 끝까지 읽었을 경우 eof로 판단
				token.type = ceof;
				return token; 
			}
		} // 읽은 문자가 공백일 경우

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
			index = GetKeyword(id); // 키워드인지 바이너리 서치로 찾음
			if (index == -1) { 
				token.type = cidentifier;
				strcpy(token.id, id);
			} // 없을 경우 식별자로
			else {
				token.type = keynum[index];
				strcpy(token.id, keyword[index]);
				break;
			} // 테이블에 있을 경우 키워드로
		} // 읽은 문자가 문자였을 경우

		else if (isdigit(ch)) {
			token.type = cnumber;
			token.number = GetNumber(ch, source);
		} // 읽은 문자가 숫자였을 경우

		else {
			index = getSymbol(ch);

			switch (index)
			{
			case 0: // !인 경우
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
			case 7: // /인 경우
				ch = fgetc(source);
				if (ch == '*') { // 주석의 시작
					do
					{
						if (feof(source)) { // 만약 파일을 끝까지 읽었다면
							LexicalError(4);
							token.type = cloop;
							return token;
						}
						ch = fgetc(source);
						while (ch != '*') {
							ch = fgetc(source);
							if (feof(source)) { // 파일을 끝까지 읽었다면
								LexicalError(4);
								token.type = cloop;
								return token;
							}
							if (ch == '\n')
								line++;
						}
						ch = fgetc(source);
					} while (ch != '/'); // 주석 탈출
				}
				else { // 나누기일 경우
					fseek(source, -1, SEEK_CUR);
					token.type = cmodify;
					strcpy(token.sym, "/");
					ungetc(ch, stdin);
				}
				break;
			case 9: // <인 경우
				ch = fgetc(source);
				if (ch == '=') { // <=일 경우
					token.type = clesse;
					strcpy(token.sym, "<=");
				}
				else { // <일 경우
					fseek(source, -1, SEEK_CUR);
					token.type = cless;
					strcpy(token.sym, "<");
					ungetc(ch, stdin);
				}
				break;
			case 10: // =인 경우
				ch = fgetc(source);
				if (ch == '=') { // ==일 경우
					token.type = cequal;
					strcpy(token.sym, "==");
				}
				else {  // =일 경우
					fseek(source, -1, SEEK_CUR);
					token.type = cassign;
					strcpy(token.sym, "=");
					ungetc(ch, stdin);
				}
				break;
			case 11: // >인 경우
				ch = fgetc(source);
				if (ch == '=') { // >=일 경우
					token.type = cmoree;
					strcpy(token.sym, ">=");
				}
				else { // >일 경우
					fseek(source, -1, SEEK_CUR);
					token.type = cmore;
					strcpy(token.sym, ">");
					ungetc(ch, stdin);
				}
				break;
			case NUM_SYMBOLS: // 예약어 테이블에 없는 경우
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
		// 심볼 테이블 사용버전
		
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
				if (ch == '*') { // 주석의 시작
					do
					{
						if (feof(source)) { // 만약 파일을 끝까지 읽었다면
							LexicalError(4);
							token.type = cloop;
							return token;
						}
						ch = fgetc(source);
						while (ch != '*') {
							ch = fgetc(source);
							if (feof(source)) { // 파일을 끝까지 읽었다면
								LexicalError(4);
								token.type = cloop;
								return token;
							}
							if (ch == '\n')
								line++;
						}
						ch = fgetc(source);
					} while (ch != '/'); // 주석 탈출
				}
				else { // 나누기일 경우
					fseek(source, -1, SEEK_CUR);
					token.type = cmodify;
					strcpy(token.sym, "/");
					ungetc(ch, stdin);
				}
				break;
			case '<':
				ch = fgetc(source);
				if (ch == '=') { // <=일 경우
					token.type = clesse;
					strcpy(token.sym, "<=");
				}
				else { // <일 경우
					fseek(source, -1, SEEK_CUR);
					token.type = cless;
					strcpy(token.sym, "<");
					ungetc(ch, stdin);
				}
				break;
			case '>':
				ch = fgetc(source);
				if (ch == '=') { // >=일 경우
					token.type = cmoree;
					strcpy(token.sym, ">=");
				}
				else { // >일 경우
					fseek(source, -1, SEEK_CUR);
					token.type = cmore;
					strcpy(token.sym, ">");
					ungetc(ch, stdin);
				}
				break;
			case '=':
				ch = fgetc(source);
				if (ch == '=') { // ==일 경우
					token.type = cequal;
					strcpy(token.sym, "==");
				}
				else {  // =일 경우
					fseek(source, -1, SEEK_CUR);
					token.type = cassign;
					strcpy(token.sym, "=");
					ungetc(ch, stdin);
				}
				break;
			case '!':
				ch = fgetc(source);
				if (ch == '=') { // !=일 경우
					token.type = cnotequal;
					strcpy(token.sym, "!=");
				}
				else { // !일 경우 (ERROR)임
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
		// 심볼 테이블 안 쓴 버전
		
	} while (token.type == cnull);

	return token;
};

int main(int argc, char* argv[])
{
	FILE* source;
	FILE* copy;
	FILE* result;

	int i;
	int copyline = 1;
	char buffer[MAX];
	struct tokenType a;
	if (argc != 3) {
		fprintf(stderr, "Usage: name <sourcefile> <resultfile>\n");
		exit(1);
	}

	source = fopen(argv[1], "r");
	copy = fopen(argv[1], "r");
	result = fopen(argv[2], "w");

	do
	{

		a = Scanner(source);

		while (copyline <= line) {
			fgets(buffer, sizeof(buffer), copy);
			fprintf(result, "%d: %s", copyline++, buffer);
			buffer[0] = '\0';
		}

		if (a.type < 19) // 심볼로 밝혀졌을 경우
			fprintf(result, "\t%d: Symbol: %s\n", line, a.sym);
		else if (a.type == 19) // number로 밝혀졌을 경우
			fprintf(result, "\t%d: Number, Value = %d\n", line, a.number);
		else if (a.type == 20) // identifier로 밝혀졌을 경우
			fprintf(result, "\t%d: ID, Name = %s\n", line, a.id);
		else if (a.type > 20 && a.type < 27) // 키워드로 밝혀졌을 경우
			fprintf(result, "\t%d: Reserved word: %s\n", line, a.id);
		else if (a.type == 27) // EOF일 경우
			fprintf(result, "\n\t%d: EOF\n", line);
		else if (a.type == 29) // loop에서 못 나왔을 경우
			fprintf(result, "\n\t%d: Stop before Ending the Comment\n", line);
		else
			fprintf(result, "\t%d: ERROR: %c\n", line, a.error);

	} while (!(feof(source)));

	fclose(copy);
	fclose(result);
	fclose(source);

	return 0;
}