#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlschemas.h>
#include <string.h>
#include <stdbool.h>
#include "xml.h"

void add_node(xmlNodePtr parent, struct ast* ast_vertex) {
    if (!ast_vertex)
        return;

    xmlNodePtr xmlNode;
    char buffer[256];

    switch (ast_vertex->nodetype)
    {
    case NT_DROP:
    {
        struct drop_ast* drop_ast = (struct drop_ast*)ast_vertex;
        xmlNode = xmlNewChild(parent, NULL, BAD_CAST "drop", NULL);
        snprintf(buffer, sizeof(buffer), "%s", drop_ast->num);
        xmlNewProp(xmlNode, BAD_CAST "tabname", BAD_CAST buffer);
        break;
    }
    case NT_SELECT:
    {
        struct select_ast* select_a = (struct select_ast*)ast_vertex;
        xmlNode = xmlNewChild(parent, NULL, BAD_CAST "select", NULL);
        snprintf(buffer, sizeof(buffer), "%s", select_a->name);
        xmlNewProp(xmlNode, BAD_CAST "tabname", BAD_CAST buffer);
        if (select_a->filter_part != NULL)
          add_node(xmlNode, select_a->filter_part);
        break;
    }
    case NT_DELETE:
    {
        struct delete_ast* delete_a = (struct delete_ast*)ast_vertex;
        xmlNode = xmlNewChild(parent, NULL, BAD_CAST "delete", NULL);
        snprintf(buffer, sizeof(buffer), "%s", delete_a->name);
        xmlNewProp(xmlNode, BAD_CAST "tabname", BAD_CAST buffer);
        if (delete_a->filter_part != NULL)
            add_node(xmlNode, delete_a->filter_part);
        break;
    }
    case NT_JOIN:
    {
        struct join_ast* join_a = (struct join_ast*)ast_vertex;
        xmlNode = xmlNewChild(parent, NULL, BAD_CAST "join", NULL);

        snprintf(buffer, sizeof(buffer), "%s", join_a->name1);
        xmlNewProp(xmlNode, BAD_CAST "tabname1", BAD_CAST buffer);
        snprintf(buffer, sizeof(buffer), "%s", join_a->name2);
        xmlNewProp(xmlNode, BAD_CAST "tabname2", BAD_CAST buffer);

        snprintf(buffer, sizeof(buffer), "%d", join_a->num1);
        xmlNewProp(xmlNode, BAD_CAST "raw_num1", BAD_CAST buffer);
        snprintf(buffer, sizeof(buffer), "%d", join_a->num2);
        xmlNewProp(xmlNode, BAD_CAST "raw_num2", BAD_CAST buffer);
        if(join_a->filter_part != NULL)
          add_node(xmlNode, join_a->filter_part);
        break;
    }
    case NT_LIST:
    {
        struct list_ast* list_ast = (struct list_ast*)ast_vertex;
        add_node(parent, list_ast->now);
        if(list_ast->next != NULL)
          add_node(parent, list_ast->next);
        break;
    }
    case NT_PAIR:
    {
        struct pair_ast* pair_ast = (struct pair_ast*)ast_vertex;
        xmlNode = xmlNewChild(parent, NULL, BAD_CAST "pair", NULL);
        snprintf(buffer, sizeof(buffer), "%d", pair_ast->raw_num);
        xmlNewProp(xmlNode, BAD_CAST "key", BAD_CAST buffer);
        add_node(xmlNode, pair_ast->value);
        break;
    }
    case NT_INTVAL:
    {
        struct int_ast* int_ast = (struct int_ast*)ast_vertex;
        snprintf(buffer, sizeof(buffer), "%d", int_ast->value);
        xmlNode = xmlNewChild(parent, NULL, BAD_CAST "int", BAD_CAST buffer);
        break;
    }
    case NT_DOUBLEVAL:
    {
        struct double_ast* double_a = (struct double_ast*)ast_vertex;
        snprintf(buffer, sizeof(buffer), "%4.4f", double_a->value);
        xmlNode = xmlNewChild(parent, NULL, BAD_CAST "double", BAD_CAST buffer);
        break;
    }
    case NT_STRINGVAL:
    {
        struct string_ast* string_a = (struct string_ast*)ast_vertex;
        xmlNode = xmlNewChild(parent, NULL, BAD_CAST "string", BAD_CAST string_a->value);
        break;
    }
    case NT_FILTER_EXPR:
    {
        struct filter_pair_ast* filter_expr_ast = (struct filter_pair_ast*)ast_vertex;
        xmlNode = xmlNewChild(parent, NULL, BAD_CAST "filter_expr", NULL);
        //char* cmp_str = ((struct sign_ast * )filter_expr_ast->sign)->sign;
        xmlNewProp(xmlNode, BAD_CAST "cmp", BAD_CAST((struct sign_ast*)filter_expr_ast->sign)->sign);
        add_node(xmlNode, filter_expr_ast->left_oper);
        add_node(xmlNode, filter_expr_ast->right_oper);
        break;
    }
    case NT_TABLE_RAW:
    {
        struct tablef_raw_ast* table_raw_a = (struct tablef_raw_ast*)ast_vertex;
        xmlNode = xmlNewChild(parent, NULL, BAD_CAST "table_raw", NULL);
        snprintf(buffer, sizeof(buffer), "%s", table_raw_a->varname);
        xmlNewProp(xmlNode, BAD_CAST "tabname", BAD_CAST buffer);
        snprintf(buffer, sizeof(buffer), "%d", table_raw_a->raw_num);
        xmlNewProp(xmlNode, BAD_CAST "raw_num", BAD_CAST buffer);
        break;
    }
    case NT_CREATE:
    {
        struct create_table_ast* create_ast = (struct create_table_ast*)ast_vertex;
        xmlNode = xmlNewChild(parent, NULL, BAD_CAST "create", NULL);
        snprintf(buffer, sizeof(buffer), "%s", create_ast->table_num);
        xmlNewProp(xmlNode, BAD_CAST "tabname", BAD_CAST buffer);
        snprintf(buffer, sizeof(buffer), "%d", create_ast->raw_size);
        xmlNewProp(xmlNode, BAD_CAST "raw_size", BAD_CAST buffer);
        break;
    }
    case NT_FILTER_LIST:
    {
        struct filter_list_ast* filter_a = (struct filter_list_ast*)ast_vertex;
        xmlNode = xmlNewChild(parent, NULL, BAD_CAST "filter", NULL);
        if (filter_a->op != NULL) {
            snprintf(buffer, sizeof(buffer), "%d", filter_a->op);
        }
        else
            snprintf(buffer, sizeof(buffer), "%d", -1);
        xmlNewProp(xmlNode, BAD_CAST "operation", BAD_CAST buffer);
        add_node(xmlNode, filter_a->now);
        add_node(xmlNode, filter_a->next);
        break;
    }
    case NT_INSERT:
    {
        struct insert_ast* insert_ast = (struct insert_ast*)ast_vertex;
        xmlNode = xmlNewChild(parent, NULL, BAD_CAST "insert", NULL);
        snprintf(buffer, sizeof(buffer), "%s", insert_ast->table_num);
        xmlNewProp(xmlNode, BAD_CAST "tabname", BAD_CAST buffer);
        xmlNodePtr list = xmlNewChild(xmlNode, NULL, BAD_CAST "list", NULL);
        add_node(list, insert_ast->list_ast);
        break;
    }
    default:
    {
        fprintf(stderr, "unknown nodetype");
        return;
    }

    }
}


char* ast2xml(struct ast* root)
{
    xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
    xmlNodePtr root_element = xmlNewNode(NULL, BAD_CAST "root");
    xmlDocSetRootElement(doc, root_element);
    add_node(root_element, root);

    xmlChar* xmlbuff;
    int buffersize;
    xmlDocDumpFormatMemory(doc, &xmlbuff, &buffersize, 1);
    // xmlSaveFormatFileEnc("-", doc, "UTF-8", 1);
    xmlFreeDoc(doc);
    xmlCleanupParser();
    return (char*)xmlbuff;
}
