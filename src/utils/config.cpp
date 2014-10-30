/**
 * author: Triones
 * date  : 2014-08-26
 */
#include <stdlib.h>
#ifndef _NI_WIN_
#include <unistd.h>
#endif
#include <string.h>
#include "config.h"

namespace triones
{

#define TYPE_SECTION        1
#define TYPE_TOKEN_SCALAR   2
#define TYPE_TOKEN_VECTOR   3
#define TYPE_COMMENT        4
#define TYPE_BLANK          5

#define INVALID_TYPE(type)  (((type) < TYPE_SECTION) || ((type) > TYPE_BLANK))
#define IS_TOKEN(type)      (((type) == TYPE_TOKEN_SCALAR) || ((type) == TYPE_TOKEN_VECTOR))
#define STRCMP(A, B)        (_case_sensitive ? strcmp((A), (B)) : strcasecmp((A), (B)))

Config::Config(const char *fname)
{
	char caBakFilename[148];

	_ready           = 0;
	_lines           = NULL;
	_sections        = NULL;
	_dirty_flag      = 0;
	_write_now       = 1;
	_case_sensitive  = 1;
	_total_sections  = 0;
	_tokens_per_line = 0;

	memset(_filename, 0, sizeof(_filename));

	if (fname == NULL)
	{
		return;
	}

	if (strlen(fname) >= sizeof(_filename))
	{
		return;
	}

	strcpy(_filename, fname);

	if (load(_filename) ==  - 1)
	{
		return;
	}

	_ready = 1;

	memset(caBakFilename, 0, sizeof(caBakFilename));
	sprintf(caBakFilename, "%s.lfbak", _filename);

	dump(caBakFilename);
}

Config::~Config()
{
	if (_ready && _dirty_flag && (dump() ==  - 1))
	{
		// What should we do?
	}

	release();
}

int Config::ready()
{
	return (_ready);
}

int Config::set_wirte_now(int writenow)
{
	int oldflag = _write_now;

	_write_now = (writenow ? 1 : 0);

	return (oldflag);
}

int Config::set_case_sensitive(int yes)
{
	int oldflag = _case_sensitive;

	_case_sensitive = (yes ? 1 : 0);

	return (oldflag);
}

int Config::set_column(int column)
{
	int oldcolumn = _tokens_per_line;

	_tokens_per_line = (column > 0 ? column : 0);

	return (oldcolumn);
}

int Config::reload(const char *path)
{
	release();

	_ready = 0;
	_lines = NULL;
	_sections = NULL;
	_dirty_flag = 0;
	_write_now = 1;
	_case_sensitive = 1;
	_total_sections = 0;

	if (load((path == NULL) ? _filename : path) ==  - 1)
	{
		return (-1);
	}

	_ready = 1;

	return (0);
}

int Config::dump()
{
	char caTempFilename[PATH_MAX];

	if (!_ready)
	{
		return (-1);
	}

	if (_filename[0] == '\0')
	{
		return (-1);
	}

	memset(caTempFilename, 0, sizeof(caTempFilename));
	sprintf(caTempFilename, "%s.lftmp", _filename);

	if (dump(caTempFilename) ==  - 1)
	{
		return (-1);
	}

	if (unlink(_filename) ==  - 1)
	{
		return (-1);
	}

	if (rename(caTempFilename, _filename) ==  - 1)
	{
		return (-1);
	}

	return (0);
}

#define fIsDelimiter(c) (((c) == ' ') || ((c) == '\t'))

#define STH_WRONG_EXIT(msg) { \
		fclose(_fp); \
		return(-1); \
}

#define ADD_LINE \
{ \
	if ((line = new Line) == NULL) \
	{ \
		STH_WRONG_EXIT("memory allocation fail"); \
	} \
\
	line->next   = NULL; \
	line->prev   = NULL; \
	line->token  = NULL; \
	line->etoken = NULL; \
\
	if (curline == NULL) \
	{ \
		_lines = curline = line; \
	} \
	else \
	{ \
		curline->next = line; \
		line->prev    = curline; \
		curline       = line; \
	} \
\
	line     = NULL; \
	curtoken = NULL; \
}

#define ADD_TOKEN(Type, Index, Name, Namelen, Value, Valuelen) \
{ \
	if ((token = new Token) == NULL) \
		STH_WRONG_EXIT("memory allocation fail"); \
\
	token->type  = (Type); \
	token->index = (Index); \
\
	if ((Name) == NULL) \
	{ \
		token->name = NULL; \
	} \
	else \
	{ \
		if((Namelen) <= 0) \
			STH_WRONG_EXIT("Illegal name"); \
\
		if((token->name  = new char[(Namelen)+1]) == NULL) \
			STH_WRONG_EXIT("memory allocation fail"); \
\
		memcpy(token->name, (Name), (Namelen)); \
		token->name[(Namelen)] = '\0'; \
	} \
\
	if((Value) == NULL) \
	{ \
		token->value = NULL; \
	} \
	else \
	{ \
		if((Valuelen) <= 0) \
			STH_WRONG_EXIT("Illegal value"); \
\
		if((token->value  = new char[(Valuelen)+1]) == NULL) \
			STH_WRONG_EXIT("memory allocation fail"); \
\
		memcpy(token->value, (Value), (Valuelen)); \
		token->value[(Valuelen)] = '\0'; \
	} \
\
	token->next  = NULL; \
	token->snext = NULL; \
	token->line  = NULL; \
\
	if(curtoken  == NULL) \
	{ \
		curline->token = curtoken = token; \
	} \
	else \
	{ \
		curtoken->next = token; \
		curtoken       = token; \
	} \
\
	token = NULL; \
}

int Config::load(const char *path)
{
	char caLine[1024], caTempLine[1024], caTempLine2[1024];
	char *pcEqualSign;
	int iEqualSignPos, iSectionDefined;
	int iTokenScalarDefined, iTokenVectorDefined;
	int i, j, k, l, m, flag, iStart, iEnd, iTokenEnd, iIndex;
	Token *token = NULL,  *curtoken = NULL,  *ptoken = NULL,  *ntoken = NULL;
	Line *line = NULL,  *curline = NULL;

	if ((_fp = fopen(path, "r")) == NULL)
	{
		if ((_fp = fopen(path, "w+")) == NULL)
		{
			return (-1);
		}

		fclose(_fp);

		if ((_fp = fopen(path, "r")) == NULL)
		{
			return (-1);
		}
	}

	curline = _lines;
	iSectionDefined = 0;
	iTokenScalarDefined = iTokenVectorDefined = 1;
	iIndex = 1;

	while (!feof(_fp))
	{
		memset(caLine, 0, sizeof(caLine));

		if (fgets(caLine, sizeof(caLine), _fp) == NULL)
		{
			continue;
		}

		iStart = 0;

		if ((iEnd = (strlen(caLine) - 1)) < 0)
		{
			STH_WRONG_EXIT("get line fail");
		}

		//判断末尾是否是换行，这一点尤其注意，windows下是回车换行，所以不能在win下编辑该配置文件
		if (caLine[iEnd] == '\n')
		{
			if (caLine[iEnd - 1] == '\r')
				iEnd--;
			caLine[iEnd--] = '\0';
		}

		if (caLine[iEnd] == '\r')
		{
			caLine[iEnd--] = '\0';
		}

		while ((iEnd >= iStart) && fIsDelimiter(caLine[iEnd]))
		{
			iEnd--;
		}

		ADD_LINE;
		//LOG1(NULL, 0, "dump new line: \"%s\"", caLine);

		// Is it a blank line?
		if (iStart > iEnd)
		{
			//LOG(NULL, 0, "TYPE_BLANK");
			char * null = NULL; // make compiler happy. see -Wnonnull
			ADD_TOKEN(TYPE_BLANK, 0, null, 0, null, 0);
			continue;
		}

		// Check whether the line is comment.
		if (caLine[0] == '#')
		{
			char * null = NULL; // make compiler happy. see -Wnonnull
			ADD_TOKEN(TYPE_COMMENT, 0, caLine, iEnd + 1, null, 0);
			continue;
		}

		while ((iStart <= iEnd) && fIsDelimiter(caLine[iStart]))
		{
			iStart++;
		}

		// Check whether the line is the definition of section.
		if ((caLine[iStart] == '[') && (caLine[iEnd] == ']'))
		{
			i = iStart + 1;
			j = iEnd - 1;

			while ((i <= iEnd) && fIsDelimiter(caLine[i]))
			{
				i++;
			}

			while ((j >= iStart) && fIsDelimiter(caLine[j]))
			{
				j--;
			}

			for (flag = 0, l = 0, k = i; k <= j; k++)
			{
				if (!fIsDelimiter(caLine[k]))
				{
					flag = 0;
					caTempLine[l++] = caLine[k];
				}
				else
				{
					if (flag == 0)
					{
						caTempLine[l++] = ' ';
						flag = 1;
					}
				}
			}

			{
				char * null = NULL; // make compiler happy. see -Wnonnull
				ADD_TOKEN(TYPE_SECTION, 0, caTempLine, l, null, 0);

				//ADD_TOKEN(TYPE_SECTION, 0, &caLine[i], j-i+1, null, 0);
			}

			if (!iTokenScalarDefined && !iTokenVectorDefined)
			{
			}

			iIndex = iSectionDefined = 1;
			iTokenScalarDefined = iTokenVectorDefined = 0;

			continue;
		}

		if (iSectionDefined != 1)
		{
			STH_WRONG_EXIT("define section first");
		}

		pcEqualSign = strchr(caLine, '=');

		if (pcEqualSign == NULL)
		{
			// no equal sign found. So this line should likes this:
			//  [idenfifier[ \t]*]*
			if (iTokenScalarDefined == 1)
			{
				STH_WRONG_EXIT("Cannot mix type scalar and vector together");
			}

			while (iStart <= iEnd)
			{
				iTokenEnd = iStart;

				while ((iTokenEnd <= iEnd) && !fIsDelimiter(caLine[iTokenEnd]))
				{
					iTokenEnd++;
				}

				char * null = NULL; // make compiler happy. see -Wnonnull
				ADD_TOKEN(TYPE_TOKEN_VECTOR, iIndex, null, 0, &caLine[iStart], iTokenEnd - iStart);
				iIndex++;

				while ((iTokenEnd <= iEnd) && fIsDelimiter(caLine[iTokenEnd]))
				{
					iTokenEnd++;
				}

				iStart = iTokenEnd;
			}

			// If everything is ok, continue to process the next
			// line.
			iTokenVectorDefined = 1;

			continue;
		}

		if (iTokenVectorDefined == 1)
		{
			STH_WRONG_EXIT("Cannot mix type scalar and vector together");
		}

		iEqualSignPos = pcEqualSign - caLine;
		pcEqualSign = strrchr(caLine, '=');

		if (pcEqualSign == NULL)
		{
			STH_WRONG_EXIT("I don't know why this situation occur");
		}

		if ((pcEqualSign - caLine) != iEqualSignPos)
		{
			STH_WRONG_EXIT("Two equal sign in one line");
		}

		i = iEqualSignPos - 1;
		j = iEqualSignPos + 1;

		while ((i >= iStart) && fIsDelimiter(caLine[i]))
		{
			i--;
		}

		while ((j <= iEnd) && fIsDelimiter(caLine[j]))
		{
			j++;
		}

		for (flag = 0, l = 0, k = iStart; k <= i; k++)
		{
			if (!fIsDelimiter(caLine[k]))
			{
				flag = 0;
				caTempLine[l++] = caLine[k];
			}
			else
			{
				if (flag == 0)
				{
					caTempLine[l++] = ' ';
					flag = 1;
				}
			}
		}

		for (flag = 0, m = 0, k = j; k <= iEnd; k++)
		{
			if (!fIsDelimiter(caLine[k]))
			{
				flag = 0;
				caTempLine2[m++] = caLine[k];
			}
			else
			{
				if (flag == 0)
				{
					caTempLine2[m++] = ' ';
					flag = 1;
				}
			}
		}

		ADD_TOKEN(TYPE_TOKEN_SCALAR, 1, caTempLine, l, caTempLine2, m);
		//ADD_TOKEN(TYPE_TOKEN_SCALAR, 1, &caLine[iStart], i-iStart+1, &caLine[j], iEnd-j+1);
		iTokenScalarDefined = 1;
	}

	fclose(_fp);

	// Let each token within a line has a pointer to the line.
	for (curline = _lines; curline != NULL; curline = curline->next)
	{
		curtoken = curline->token;

		while (curtoken != NULL)
		{
			curtoken->line = curline;
			curtoken = curtoken->next;
		}
	}

	// count the number of sections, and save the respective line pointer
	// in variable '_sections'.
	for (i = 0, curline = _lines; curline != NULL; curline = curline->next)
	{
		token = curline->token;

		if ((token != NULL) && (token->type == TYPE_SECTION))
		{
			i++;
		}
	}

	_total_sections = i;

	_sections = new Line *[_total_sections + 1];

	if (_sections == NULL)
	{
		return (-1);
	}

	for (j = 0, curline = _lines; curline != NULL; curline = curline->next)
	{
		token = curline->token;

		if ((token != NULL) && (token->type == TYPE_SECTION))
		{
			*(_sections + j) = curline;
			j++;
		}
	}

	*(_sections + j) = NULL;

	for (j = 1; j < _total_sections; j++)
	{
		for (k = 0; k < j; k++)
		{
			if (!STRCMP((*(_sections + k))->token->name, (*(_sections + j))->token->name))
			{
				return (-1);
				break;
			}
		}
	}

	// now set each section's etoken pointer.
	for (i = 0; i < _total_sections; i++)
	{
		curline = *(_sections + i);

		while ((curline != NULL) && (curline != *(_sections + i + 1)))
		{
			token = curline->token;

			if ((token == NULL) || !IS_TOKEN(token->type))
			{
				curline = curline->next;
				continue;
			}

			(*(_sections + i))->etoken = token;

			break;
		}
	}

	// now link the tokens within a section together.
	for (i = 0; i < _total_sections; i++)
	{
		curline = *(_sections + i);
		j = 1;

		while ((curline != NULL) && (curline != *(_sections + i + 1)))
		{
			token = curline->token;

			if ((token == NULL) || !IS_TOKEN(token->type))
			{
				curline = curline->next;
				continue;
			}

			if (j == 1)
			{
				ptoken = token;
				j = 0;
			}
			else
			{
				ptoken->snext = token;
				ptoken = token;
			}

			while (token->next != NULL)
			{
				token = token->next;
				ptoken->snext = token;
				ptoken = token;
			}

			curline = curline->next;
		}
	}

	// now compute the index parameter of each TYPE_TOKEN_SCALAR token.
	for (i = 0, line = *_sections; line != NULL; line = *(_sections + (++i)))
	{
		token = (*(_sections + i))->etoken;

		if ((token == NULL) || (token->type != TYPE_TOKEN_SCALAR))
		{
			continue;
		}

		while (token != NULL)
		{
			ntoken = token->snext;

			while (ntoken != NULL)
			{
				if (!STRCMP(token->name, ntoken->name))
				{
					ntoken->index++;
				}

				ntoken = ntoken->snext;
			}

			token = token->snext;
		}
	}

	return (0);
}

int Config::dump(const char *path)
{
	FILE *fpDump;
	Line *l;
	Token *t;
	int iMaxTokenLen, iTokens, len, i;

	if (!_ready)
	{
		return (-1);
	}

	if ((fpDump = fopen(path, "w")) == NULL)
	{
		return (-1);
	}

	for (l = _lines; l != NULL; l = l->next)
	{
		if ((t = l->token) == NULL)
		{
			continue;
		}

		if (t->type == TYPE_TOKEN_VECTOR)
		{
			for (iMaxTokenLen = 0; t != NULL; t = t->next)
			{
				len = strlen(t->value);
				if (iMaxTokenLen < len)
				{
					iMaxTokenLen = len;
				}
			}

			iMaxTokenLen++;

			if (_tokens_per_line > 0)
			{
				iTokens = _tokens_per_line;
			}
			else
			{
				iTokens = 70 / iMaxTokenLen;

				if (iTokens > 16)
				{
					iTokens = 16;
				}

				if (iTokens < 1)
				{
					iTokens = 1;
				}
			}

			for (t = l->token, i = 0; t != NULL; t = t->next)
			{
				fprintf(fpDump, "%-*s", iMaxTokenLen, t->value);
				if (!(++i % iTokens))
				{
					fprintf(fpDump, "\n");
					i = 0;
				}
			}

			if (i)
			{
				fprintf(fpDump, "\n");
			}

			continue;
		}
		else if (t->type == TYPE_COMMENT)
		{
			fprintf(fpDump, "%s\n", t->name);
		}
		else if (t->type == TYPE_BLANK)
		{
			fprintf(fpDump, "\n");
		}
		else if (t->type == TYPE_SECTION)
		{
			fprintf(fpDump, "[%s]\n", t->name);
		}
		else if (t->type == TYPE_TOKEN_SCALAR)
		{
			fprintf(fpDump, "%-15s = %s\n", t->name, t->value);
		}
		else
		{
			// do something.
		}
	}

	fclose(fpDump);
	_dirty_flag = 0;

	return (0);
}

void Config::release()
{
	Line *curline, *nextline;
	Token *curtoken, *nexttoken;

	curline = _lines;

	while (curline != NULL)
	{
		nextline = curline->next;
		curtoken = curline->token;

		while (curtoken != NULL)
		{
			nexttoken = curtoken->next;
			delete curtoken->name;
			delete curtoken->value;
			delete curtoken;
			curtoken = nexttoken;
		}

		delete curline;

		curline = nextline;
	}

	_lines = NULL;

	if (_sections != NULL)
	{
		delete[] _sections;
	}
}

int Config::get(const char *section, const char *id, std::string &value,
		int index)
{
	Line *line;
	Token *token;
	int i;

	if (!_ready)
	{
		return (-1);
	}

	if ((_sections == NULL) || (_lines == NULL))
	{
		return (-1);
	}

	if (section == NULL)
	{
		return (-1);
	}

	for (i = 0, line = *_sections; line != NULL; line = *(_sections + (++i)))
	{
		if (!STRCMP(section, line->token->name))
		{
			break;
		}
	}

	if (line == NULL)
	{
		return (-1);
	}

	if ((token = line->etoken) == NULL)
	{
		return (-1);
	}

	while (token != NULL)
	{
		if (id == NULL)
		{
			if ((token->type == TYPE_TOKEN_VECTOR) && (token->index == index))
			{
				value = token->value;
				return (0);
			}
		}
		else
		{
			if ((token->type == TYPE_TOKEN_SCALAR) && (token->index == index)
					&& !STRCMP(token->name, id))
			{
				value = token->value;
				return (0);
			}
		}

		token = token->snext;
	}

	return (-1);
}

int Config::set(const char *section, const char *id, const char *value,
		int index)
{
	Line *line;
	Token *token;
	int i;
	unsigned int len;

	if (!_ready)
	{
		return (-1);
	}

	if ((_sections == NULL) || (_lines == NULL))
	{
		return (-1);
	}

	if ((section == NULL) || (value == NULL))
	{
		return (-1);
	}

	for (i = 0, line = *_sections; line != NULL; line = *(_sections + (++i)))
	{
		if (!STRCMP(section, line->token->name))
		{
			break;
		}
	}

	if (line == NULL)
	{
		return (-1);
	}

	if ((token = line->etoken) == NULL)
	{
		return (-1);
	}

	while (token != NULL)
	{
		if (((id == NULL) && (token->type == TYPE_TOKEN_VECTOR)
				&& (token->index == index))
				|| ((id != NULL) && (token->type == TYPE_TOKEN_SCALAR)
						&& (token->index == index) && !STRCMP(token->name, id)))
		{
			if (!STRCMP(token->value, value))
			{
				return (0);
			}

			len = strlen(value);

			if (len <= strlen(token->value))
			{
				strcpy(token->value, value);
			}
			else
			{
				delete token->value;
				token->value = new char[len + 1];

				if (token->value == NULL)
				{
					return (-1);
				}

				memcpy(token->value, value, len);
				token->value[len] = '\0';
			}

			if (_write_now)
			{
				dump();
			}
			else
			{
				_dirty_flag = 1;
			}

			return (0);
		}

		token = token->snext;
	}

	return (-1);
}

int Config::count(const char *section, const char *id)
{
	Line *line;
	Token *token;
	int index, i;

	/* in case of non-vital error condition, we should return 0. */

	if (!_ready)
	{
		return (-1);
	}

	if ((_sections == NULL) || (_lines == NULL))
	{
		return (-1);
	}

	if (section == NULL)
	/* parameter error. */
	{
		return (-1);
	}

	for (i = 0, line = *_sections; line != NULL; line = *(_sections + (++i)))
	{
		if (!STRCMP(section, line->token->name))
		{
			break;
		}
	}

	if (line == NULL) /* section not found. */
	{
		return (0);
	}

	if ((token = line->etoken) == NULL) /* section empty. */
	{
		return (0);
	}

	if (id == NULL)
	{
		if (token->type != TYPE_TOKEN_VECTOR) /* token type confused. */
		{
			return (-1);
		}

		while (token->snext != NULL)
		{
			token = token->snext;
		}

		return (token->index);
	}
	else
	{
		if (token->type == TYPE_TOKEN_SCALAR)
		{
			for (index = 0; token != NULL; token = token->snext)
			{
				if (!STRCMP(token->name, id))
				{
					index = token->index;
				}
			}

			return (index);
		}

		if (token->type == TYPE_TOKEN_VECTOR)
		{
			for (index = 0; token != NULL; token = token->snext)
			{
				if (!STRCMP(token->value, id))
				{
					index++;
				}
			}

			return (index);
		}
	}

	return (-1);
}

int Config::index(const char *section, const char *id, int start)
{
	Line *line;
	Token *token;
	int index, i;

	if (!_ready)
	{
		return (-1);
	}

	if ((_sections == NULL) || (_lines == NULL))
	{
		return (-1);
	}

	if (section == NULL)
	{
		return (-1);
	}

	for (i = 0, line = *_sections; line != NULL; line = *(_sections + (++i)))
	{
		if (!STRCMP(section, line->token->name))
		{
			break;
		}
	}

	if (line == NULL)
	{
		return (-1);
	}

	if (id == NULL)
	{
		return (i + 1);
	}

	if ((token = line->etoken) == NULL)
	{
		return (-1);
	}

	if (token->type == TYPE_TOKEN_SCALAR)
	{
		for (index = 1; token != NULL; token = token->snext, index++)
		{
			if (!STRCMP(token->name, id) && (index > start))
			{
				return (index);
			}
		}

		return (-1);
	}

	if (token->type == TYPE_TOKEN_VECTOR)
	{
		for (index = 1; token != NULL; token = token->snext, index++)
		{
			if (!STRCMP(token->value, id) && (index > start))
			{
				return (index);
			}
		}

		return (-1);
	}

	return (-1);
}

int Config::add_token(const char *section, int where, const char *id, const char *value)
{
	Line *line, *curline, **tmpsections;
	Token *token, *curtoken;
	int i, iWhat, iWhere;

	if (!_ready)
	{
		return (-1);
	}

	if (section == NULL)
	{
		return (-1);
	}

	if (value == NULL)
	{
		iWhat = TYPE_SECTION;
	}
	else
	{
		if (id == NULL)
		{
			iWhat = TYPE_TOKEN_VECTOR;
		}
		else
		{
			iWhat = TYPE_TOKEN_SCALAR;
		}
	}

	if (iWhat == TYPE_SECTION)
	{
		i = _total_sections;
	}

	else if (iWhat == TYPE_TOKEN_VECTOR)
	{
		i = count(section);
	}

	else if (TYPE_TOKEN_SCALAR)
	{
		i = count(section, id);
	}
	else
	{
		return (-1);
	}

	if (i == -1)
	{
		iWhere = 0;
	}
	else
	{
		if ((where < 0) || (where > i))
		{
			iWhere = i;
		}
		else
		{
			iWhere = where;
		}
	}

	token = NULL;
	line = NULL;

	if ((token = new Token) == NULL)
	{
		return (-1);
	}

	token->type = iWhat;
	token->index = iWhere + 1;

	if (iWhat == TYPE_TOKEN_VECTOR)
	{
		token->name = NULL;
	}
	else
	{
		if (iWhat == TYPE_SECTION)
		{
			i = strlen(section);
		}
		else
		{
			i = strlen(id);
		}

		token->name = new char[i + 1];

		if (token->name == NULL)
		{
			delete token;
			return (-1);
		}

		memcpy(token->name, (iWhat == TYPE_SECTION) ? section : id, i);
		token->name[i] = '\0';
	}

	if (iWhat == TYPE_SECTION)
	{
		token->value = NULL;
	}
	else
	{
		i = strlen(value);
		token->value = new char[i + 1];

		if (token->value == NULL)
		{
			delete token->name;
			delete token;
			return (-1);
		}

		memcpy(token->value, value, i);
		token->value[i] = '\0';
	}

	token->next = token->snext = NULL;

	if ((line = new Line) == NULL)
	{
		delete token->name;
		delete token->value;
		delete token;
		return (-1);
	}

	line->next = NULL;
	line->prev = NULL;
	line->token = token;
	line->etoken = NULL;

	token->line = line;

#define SUCCESS_EXIT \
{ \
	if(_write_now) \
		dump(); \
	else \
		_dirty_flag = 1; \
	return(0); \
}

#define HCLERROR_EXIT(msg) \
{ \
	delete token->name; \
	delete token->value; \
	delete token; \
	delete line; \
	return(-1); \
}

	if (iWhat == TYPE_SECTION)
	{
		tmpsections = new Line *[_total_sections + 2];

		if (tmpsections == NULL)
		{
			HCLERROR_EXIT("memory allocation.");
		}

		for (i = 0; i < _total_sections; i++)
		{
			if (!STRCMP((*(_sections + i))->token->name, token->name))
			{
				delete[] tmpsections;
				HCLERROR_EXIT("Section already exists.");
			}
		}

		if (_total_sections == 0)
		{
			if (_lines == NULL)
			{
				_lines = line;
			}
			else
			{
				curline = _lines;
				while (curline->next != NULL)
				{
					curline = curline->next;
				}
				curline->next = line;
				line->prev = curline;
			}
		}
		else
		{
			if (_lines == *(_sections + iWhere))
			{
				_lines = line;
				line->next = *(_sections + iWhere);
				(*(_sections + iWhere))->prev = line;
			}
			else
			{
				curline = _lines;

				while (curline->next != *(_sections + iWhere))
				{
					curline = curline->next;
				}

				while ((curline != NULL)
						&& ((curline->token->type == TYPE_COMMENT)
								|| (curline->token->type == TYPE_BLANK)))
				{
					curline = curline->prev;
				}

				if (curline == NULL)
				{
					_lines->prev = line;
					line->next = _lines;
					_lines = line;
				}
				else
				{
					line->prev = curline;
					line->next = curline->next;

					if (curline->next != NULL)
					{
						curline->next->prev = line;
					}
					curline->next = line;
				}
			}
		}

		_total_sections++;

		for (i = 0; i < iWhere; i++)
		{
			*(tmpsections + i) = *(_sections + i);
		}

		*(tmpsections + iWhere) = line;

		for (i = iWhere + 1; i < _total_sections; i++)
		{
			*(tmpsections + i) = *(_sections + i - 1);
		}

		*(tmpsections + _total_sections) = NULL;

		delete[] _sections;

		_sections = tmpsections;
		SUCCESS_EXIT;
	}

	if ((_sections == NULL) || (_lines == NULL))
	{
		HCLERROR_EXIT("unknown error.");
	}

	for (i = 0; i < _total_sections; i++)
	{
		if (!STRCMP(section, (*(_sections + i))->token->name))
		{
			break;
		}
	}

	if (i == _total_sections)
	{
		HCLERROR_EXIT("cannot find the section.");
	}

	curline = *(_sections + i);
	curtoken = curline->etoken;

	if ((curtoken != NULL) && (curtoken->type != iWhat))
	{
		HCLERROR_EXIT("cannot mix type scalar/vector in a section.");
	}

	if ((curtoken == NULL) && (iWhere != 0))
	{
		HCLERROR_EXIT("No tokens in this section, where should be 0.");
	}

	if (iWhat == TYPE_TOKEN_VECTOR)
	{
		if (curtoken == NULL)
		{
			line->next = curline->next;
			line->prev = curline;

			if (curline->next != NULL)
			{
				curline->next->prev = line;
			}
			curline->next = line;
			curline->etoken = token;
			SUCCESS_EXIT;
		}

		if (iWhere == 0)
		{
			token->snext = curtoken;
			token->next = curtoken;
			token->line = curtoken->line;
			curtoken->line->token = token;
			curline->etoken = token;
			curtoken = token;

			while ((token = token->snext) != NULL)
			{
				token->index++;
			}

			delete line;

			SUCCESS_EXIT;
		}

		while ((curtoken != NULL) && (curtoken->index != iWhere))
		{
			curtoken = curtoken->snext;
		}

		if (curtoken == NULL)
		{
			HCLERROR_EXIT("Specified position not found.");
		}

		token->snext = curtoken->snext;
		token->next = curtoken->next;
		token->line = curtoken->line;
		curtoken->snext = token;
		curtoken->next = token;

		while ((token = token->snext) != NULL)
		{
			token->index++;
		}

		delete line;
		SUCCESS_EXIT;
	}

	if (iWhat == TYPE_TOKEN_SCALAR)
	{
		if (curtoken == NULL)
		{
			line->next = curline->next;
			line->prev = curline;

			if (curline->next != NULL)
			{
				curline->next->prev = line;
			}

			curline->next = line;
			curline->etoken = token;
			SUCCESS_EXIT;
		}

		if (iWhere == 0)
		{
			line->next = curline->next;
			line->prev = curline;

			if (curline->next != NULL)
			{
				curline->next->prev = line;
			}

			curline->next = line;
			token->snext = curline->etoken;
			curline->etoken = token;

			while ((token = token->snext) != NULL)
			{
				if (!STRCMP(id, token->name))
				{
					token->index++;
				}
			}

			SUCCESS_EXIT;
		}

		while ((curline != NULL) && (curline != *(_sections + i + 1)))
		{
			if (curline->token->type == TYPE_TOKEN_SCALAR)
			{
				curtoken = curline->token;
				if (!STRCMP(curtoken->name, id) && (curtoken->index == iWhere))
				{
					break;
				}
			}

			curline = curline->next;
		}

		if ((curline == NULL) || (curline == *(_sections + i + 1)))
		{
			HCLERROR_EXIT("Specified position not found.");
		}

		line->next = curline->next;
		line->prev = curline;

		if (curline->next != NULL)
		{
			curline->next->prev = line;
		}

		curline->next = line;
		token->snext = curtoken->snext;
		curtoken->snext = token;

		while ((token = token->snext) != NULL)
		{
			if (!STRCMP(id, token->name))
			{
				token->index++;
			}
		}

		SUCCESS_EXIT;
	}

	HCLERROR_EXIT("Can program run to here?");
	return (-1); // make editor happy.
}

int Config::add_comment(const char *comment, int where, const char *section,
		const char *id)
{
	Line *line, *curline;
	Token *token, *curtoken;
	int i;

	if (!_ready)
	{
		return (-1);
	}

	if (where < 0)
	{
		return (-1);
	}

	if ((section == NULL) && (id == NULL))
	{
		// add after the 'where' line.
		if ((_lines == NULL) || (where == 0))
		{
			curline = NULL;
		}
		else
		{
			curline = _lines;

			for (i = 1; i < where; i++)
			{
				if (curline->next == NULL)
				{
					break;
				}

				curline = curline->next;
			}
		}
	}
	else if ((section != NULL) && (id == NULL))
	{
		// add before the definition of this section.
		for (i = 0; i < _total_sections; i++)
		{
			if (!STRCMP((*(_sections + i))->token->name, section))
			{
				break;
			}
		}

		if (i == _total_sections)
		{
			return (-1);
		}

		curline = (*(_sections + i))->prev;
	}
	else if ((section != NULL) && (id != NULL))
	{
		// add before the 'where'th definition of 'id' in 'section'.
		for (i = 0; i < _total_sections; i++)
		{
			if (!STRCMP((*(_sections + i))->token->name, section))
			{
				break;
			}
		}

		if (i == _total_sections)
		{
			return (-1);
		}

		curline = *(_sections + i);
		curtoken = curline->etoken;

		if ((curtoken == NULL) || (curtoken->type != TYPE_TOKEN_SCALAR))
		{
			return (-1);
		}

		while (curtoken != NULL)
		{
			if ((curtoken->index == where) && !STRCMP(curtoken->name, id))
			{
				break;
			}

			curtoken = curtoken->snext;
		}

		if (curtoken == NULL)
		{
			return (-1);
		}

		curline = curtoken->line->prev;
	}
	else
	{
		return (-1);
	}

	if ((token = new Token) == NULL)
	{
		return (-1);
	}

	token->type = TYPE_COMMENT;
	token->index = 0;
	token->value = NULL;

	if (comment != NULL)
	{
		i = strlen(comment);
		token->name = new char[i + 2];

		if (token->name == NULL)
		{
			delete token;
			return (-1);
		}

		memcpy(&token->name[1], comment, i);
		token->name[0] = '#';
		token->name[i + 1] = '\0';
	}
	else
	{
		token->type = TYPE_BLANK;
		token->name = NULL;
	}

	token->next = token->snext = NULL;

	if ((line = new Line) == NULL)
	{
		delete token->name;
		delete token;
		return (-1);
	}

	line->token = token;
	line->etoken = NULL;
	line->next = line->prev = NULL;
	token->line = line;

	if (curline == NULL)
	{
		if (_lines == NULL)
		{
			_lines = line;
		}
		else
		{
			line->next = _lines;
			_lines->prev = line;
			_lines = line;
		}
	}
	else
	{
		line->next = curline->next;
		line->prev = curline;

		if (curline->next != NULL)
		{
			curline->next->prev = line;
		}
		curline->next = line;
	}

	if (_write_now)
	{
		dump();
	}
	else
	{
		_dirty_flag = 1;
	}

	return (0);
}

int Config::del_token(const char *section, const char *id, int index)
{
	int i;
	Token *token, *ptoken, *pltoken;
	Line *line;

	if (!_ready)
	{
		return (-1);
	}

	if ((_sections == NULL) || (_lines == NULL))
	{
		return (-1);
	}

	if (section == NULL)
	{
		return (-1);
	}

	for (i = 0; i < _total_sections; i++)
	{
		if (!STRCMP(section, (*(_sections + i))->token->name))
		{
			break;
		}
	}

	if (i == _total_sections)
	{
		return (-1);
	}

	token = (*(_sections + i))->etoken;

	if (token == NULL)
	{
		return (-1);
	}

	if (((id == NULL) && (token->type != TYPE_TOKEN_VECTOR))
			|| ((id != NULL) && (token->type != TYPE_TOKEN_SCALAR)))
	{
		return (-1);
	}

	ptoken = token;

	while (token != NULL)
	{
		if ((id == NULL) && (token->index == index))
		{
			// we find it!
			line = token->line;

			if (ptoken == token)
			{
				(*(_sections + i))->etoken = token->snext;
			}
			else
			{
				ptoken->snext = token->snext;
			}

			pltoken = line->token;

			if (pltoken == token)
			{
				line->token = token->next;
			}
			else
			{
				while (pltoken != NULL)
				{
					if (pltoken->next == token)
					{
						break;
					}
					pltoken = pltoken->next;
				}

				if (pltoken == NULL)
				{
					return (-1);
				}

				pltoken->next = token->next;
			}

			delete token->name;
			delete token->value;
			delete token;

			if (line->token == NULL)
			{
				line->prev->next = line->next;

				if (line->next != NULL)
				{
					line->next->prev = line->prev;
				}

				delete line;
			}

			for (token = ptoken->snext; token != NULL; token = token->snext)
			{
				token->index--;
			}

			if (_write_now)
			{
				dump();
			}
			else
			{
				_dirty_flag = 1;
			}

			return (0);
		}

		if ((id != NULL) && !STRCMP(token->name, id) && (token->index == index))
		{
			line = token->line;
			line->prev->next = line->next;

			if (line->next != NULL)
			{
				line->next->prev = line->prev;
			}

			if (ptoken == token)
			{
				(*(_sections + i))->etoken = token->snext;
			}
			else
			{
				ptoken->snext = token->snext;
			}

			delete token->name;
			delete token->value;
			delete token;
			delete line;

			for (token = ptoken->snext; token != NULL; token = token->snext)
			{
				if (!STRCMP(token->name, id))
				{
					token->index--;
				}
			}

			if (_write_now)
			{
				dump();
			}
			else
			{
				_dirty_flag = 1;
			}

			return (0);
		}

		ptoken = token;
		token = token->snext;
	}

	return (-1);
}

int Config::del_section(const char *section)
{
	Line *pline, *line, *nline, *stopline;
	Token *token, *ntoken;
	int i, j;

	if (!_ready)
	{
		return (-1);
	}

	if ((_sections == NULL) || (_lines == NULL))
	{
		return (-1);
	}

	if (section == NULL)
	{
		return (-1);
	}

	if (_total_sections < 1)
	{
		return (-1);
	}

	for (i = 0; i < _total_sections; i++)
	{
		if (!STRCMP(section, (*(_sections + i))->token->name))
		{
			break;
		}
	}

	if (i == _total_sections)
	{
		return (-1);
	}

	line = *(_sections + i);
	pline = line->prev;

	while ((pline != NULL)
			&& ((pline->token->type == TYPE_COMMENT)
					|| (pline->token->type == TYPE_BLANK)))
	{
		pline = pline->prev;
	}

	stopline = line;

	while (stopline->next != *(_sections + i + 1))
	{
		stopline = stopline->next;
	}

	while ((stopline != line)
			&& ((stopline->token->type == TYPE_COMMENT)
					|| (stopline->token->type == TYPE_BLANK)))
	{
		stopline = stopline->prev;
	}

	stopline = stopline->next;

	if (pline == NULL)
	{
		line = _lines;
	}
	else
	{
		line = pline->next;
	}

	while (line != stopline)
	{
		nline = line->next;
		token = line->token;

		while (token != NULL)
		{
			ntoken = token->next;
			delete token->name;
			delete token->value;
			delete token;
			token = ntoken;
		}

		delete line;
		line = nline;
	}

	if (pline != NULL)
	{
		pline->next = line;

		if (line != NULL)
		{
			line->prev = pline;
		}
	}
	else
	{
		_lines = line;

		if (line != NULL)
		{
			line->prev = NULL;
		}
	}

	for (j = i; *(_sections + j + 1) != NULL; j++)
	{
		*(_sections + j) = *(_sections + j + 1);
	}

	*(_sections + j) = NULL;

	_total_sections--;

	if (_write_now)
	{
		dump();
	}
	else
	{
		_dirty_flag = 1;
	}

	return (0);
}

int Config::exist_section(const char *section)
{
	int i;

	if (!_ready)
	{
		return (0);
	}

	if (section == NULL)
	{
		return (0);
	}

	for (i = 0; i < _total_sections; i++)
	{
		if (!STRCMP(section, (*(_sections + i))->token->name))
		{
			break;
		}
	}

	if (i == _total_sections)
	{
		return (0);
	}

	return (1);
}

int Config::test(void)
{
	Line *line;
	Token *token;
	char caType[6][32] = { "", "TYPE_SECTION", "TYPE_TOKEN_SCALAR", "TYPE_TOKEN_VECTOR", "TYPE_COMMENT", "TYPE_BLANK" };

	char caBuffer[512];

	if (!_ready)
	{
		return (-1);
	}

	for (line = _lines; line != NULL; line = line->next)
	{
		for (token = line->token; token != NULL; token = token->next)
		{
			sprintf(caBuffer,
					"Token %p (next %p, snext %p, line %p).\n(type %s, index %d)\n name %p \"%s\"\nvalue %p \"%s\"",
					token, token->next, token->snext, token->line,
					caType[token->type], token->index, token->name,
					(token->name ? token->name : ""), token->value,
					(token->value ? token->value : ""));
		}
	}

	return (0);
}

} // namespace triones

