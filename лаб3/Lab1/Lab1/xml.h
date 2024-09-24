#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlschemas.h>

#include "ast.h"
#include "classes.h"

struct ast* xml2ast(xmlNode* node);

struct ast* parse_xml_to_ast(const char* xml_content, int content_len);

char* data2xml(struct raw_sequence* ret);