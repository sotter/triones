/**
 * author: Triones
 * date  : 2014-08-26
 */

#ifndef __TRIONES_CONFIG_H__
#define __TRIONES_CONFIG_H__

#include <limits.h>
#include <string>

namespace triones
{

/*
 * If functions declared within this class return an pointer and no further
 * explanation, then the return value of NULL pointer indicates error,
 * others indicate success. If the return value has the type of integer and no
 * further explanation, zero indicates success while -1 indicates error.
 */
class Config
{
private:

	struct Line;

	struct Token
	{
		int     type;
		int     index;
		char   *name;
		char   *value;
		Token  *next;      // point to the next token in this line
		Token  *snext;     // point to the next token within a
		                   // section. This pointer is valid only
		                   // when type is TYPE_TOKEN_*.
		Line   *line;      // point to the line which contains
		                   // this token.
	};

	struct Line
	{
		Token  *token;     // point to the first token in this line
		Token  *etoken;    // point to the first TYPE_TOKEN_*
		                   // token within a section. This pointer
		                   // is valid only when this line is the
		                   // definition of a section.
		Line   *prev;      // previous line
		Line   *next;      // next line
	};

	char        _filename[PATH_MAX];
	FILE       *_fp;

	Line       *_lines;
	Line      **_sections;

	int         _total_sections;

	// If configuration was changed by function call like 'set' since
	// the loading at very beginning, '_dirty_flag' will be set to 1. When
	// the instance is deleted, changes will be saved to file.
	// But if _write_now flag is set, your modification will be written
	// to file immediately.
	int         _dirty_flag;
	int         _write_now;

	int         _tokens_per_line;

	int         _case_sensitive;

	// This flag indicates whether the instance is properly initialized.
	int         _ready;

public:
	Config(const char *fname);
	virtual ~Config();

	int  reload(const char *path = NULL);
	int  load(const char *path);
	int  ready();
	int  set_wirte_now(int writenow = 1);
	int  set_case_sensitive(int yes = 1);
	int  set_column(int column = 0);
	int  dump(const char *path);
	int  dump();
	void release();

	/*
	 * This member function will get the value according to the id. The
	 * value will be copied to memory pointed by parameter 'value'. So
	 * be careful about the 'value' parameter.
	 *
	 * The parameter 'id' is also used to be a flag to distinguish the
	 * type of configuration item you request. If it is NULL, I consider
	 * that what you want is TYPE_TOKEN_VECTOR. This kind of token is
	 * defined like the example below.
	 *  [colors]
	 *  red orange yellow green cyan blue purple
	 * While non-NULL pointer indicates TYPE_TOKEN_SCALAR, for an example,
	 *  [server configuration]
	 *  ip = 127.0.0.1
	 *  port = 6001
	 *
	 *  [clients]
	 *  client = 192.168.1.11
	 *  client = 132.108.58.51
	 * Under this kind of circumstance, 'id' may be a pointer to a
	 * character string "ip", "port" or "client".
	 *
	 * So if you want to retrieve the ip address from the above example,
	 * you may issue a function call like this,
	 *  std::string caIpAddr;
	 *  config->get("server configuration", "ip", caIpAddr);
	 * and if you want to retrieve the second client address, you may issue
	 * the function call like this,
	 *  std::string caIpAddr;
	 *  config->get("clients", "client", caIpAddr, 2);
	 *
	 * If you want to retrieve the yellow color from colors section in the
	 * above example, be careful, it's type is different from "ip" or
	 * "client", you may issue a function call like this,
	 *  std::string caColor;
	 *  config->get("colors", NULL, caColor, 3);
	 */
	int get(const char *section, const char *id, std::string &value, int index = 1);

	/*
	 * Compared with get, set sets the value of id. The grammar
	 * is almost the same with get. Now let's reuse the example above
	 * to show this. Function call
	 *  config->set("server configuration", "ip", "192.168.1.11");
	 * sets the ip address to 192.168.1.11 in server configuration section,
	 * while function call
	 *  config->set("colors", NULL, "black", 3);
	 * sets the third item in colors section, that is yellow, to black
	 * color.
	 * And function call
	 *  config->set("clients", "client", "203.93.112.1", 2);
	 * sets the second client's address from 132.108.58.51 to 203.93.112.1.
	 */
	int set(const char *section, const char *id, const char *value, int index = 1);

	/*
	 * This function returns the number of 'id's defined in 'section'.
	 */
	int count(const char *section, const char *id = NULL);

	/*
	 * This function returns index of the first apperance of 'id' in
	 * 'section'. If 'id' is NULL, the return value indicates which
	 * section the 'section' is.
	 */
	int index(const char *section, const char *id = NULL, int start = 0);

	/*
	 * Function add_token adds a section or a token exactly after the item
	 * which has the corresponding type and the index number equals to
	 * 'where'. The newly added item will be the first one by default.
	 */
	int add_token(const char *section, int where = 0, const char *id = NULL, const char *value = NULL);

	/*
	 * Function add_comment adds comment line to configuration file. If
	 * both section and id are all NULL pointer, comment will be added
	 * after the 'where' line. If both section and id are all non-NULL
	 * pointer, comment will be added before the 'where'th definition
	 * of 'id' in 'section'. If section isn't NULL and id is NULL, comment
	 * will be added before the definition of 'section'. All other
	 * combinations of parameters will cause failure.
	 * By the way, a blank line other than comment line will be added
	 * if parameter comment is NULL pointer.
	 */
	int add_comment(const char *comment, int where = 0, const char *section = NULL, const char *id = NULL);

	/*
	 * If id is a NULL pointer, del_token deletes a TYPE_TOKEN_VECTOR token
	 * with index from the specified section. If id isn't NULL, del_token
	 * deletes the 'id = value' pattern found in the specified section.
	 */
	int del_token(const char *section, const char *id = NULL, int index = 1);

	/*
	 * To prevent you from deleting a whole section accidently, it's
	 * defined as a separate function.
	 */
	int del_section(const char *section);

	int exist_section(const char *section);

	/*
	 * Do you really need comments about this function here?
	 */
	int test(void);
};

} // namespace triones

#endif // #ifndef __TRIONES_CONFIG_H__

