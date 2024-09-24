#include "xml.h"

xmlNodePtr get_child(xmlNodePtr parent) {
    if (parent == NULL) return NULL;
    xmlNodePtr child = parent->children;
    while (child != NULL) {
        if (child->type == XML_ELEMENT_NODE) {
            return child;
        }
        child = child->next;
    }
    return NULL;
}

xmlNodePtr get_next(xmlNodePtr node) {
    if (node == NULL) return NULL;
    xmlNodePtr next = node->next;
    while (next != NULL) {
        if (next->type == XML_ELEMENT_NODE) {
            return next;
        }
        next = next->next;
    }
    return NULL;
}

struct ast* get_list(xmlNodePtr node) {
    struct ast* list = NULL;
    node = get_child(node);
    xmlNodePtr next = node;
    list = newlist(xml2ast(node), NULL);
    next = get_next(next);
    while (next != NULL) {
        list = newlist(xml2ast(next), list);
        next = get_next(next);
    }
    return list;
}

struct ast* xml2ast(xmlNode* node) {
    struct ast* ast_node = NULL;
    if (!node)
        return NULL;
    if (!xmlStrcmp(node->name, BAD_CAST "drop")) {
        char* tabname = (char*)xmlGetProp(node, BAD_CAST "tabname");
        ast_node = newdrop(tabname);
    }
    if (!xmlStrcmp(node->name, BAD_CAST "filter")) {
        int oper = atoi((char*)xmlGetProp(node, BAD_CAST "operation"));
        struct ast* ast_l = xml2ast(get_child(node));
        struct ast* ast_r = NULL;
        if( oper != -1 )
          ast_r = xml2ast(get_next(get_child(node)));
        ast_node = newfilterlist(ast_l, oper, ast_r);
    }
    if (!xmlStrcmp(node->name, BAD_CAST "filter_expr")) {
        char* oper = (char*)xmlGetProp(node, BAD_CAST "cmp");
        struct ast* ast_l = xml2ast(get_child(node));
        struct ast* ast_r = xml2ast(get_next(get_child(node)));
        ast_node = newfilterpair(ast_l, newsign(oper), ast_r);
    }

    if (!xmlStrcmp(node->name, BAD_CAST "table_raw")) {
        char* tabname = (char*)xmlGetProp(node, BAD_CAST "tabname");
        int raw_num = atoi((char*)xmlGetProp(node, BAD_CAST "raw_num"));
        ast_node = newtablefraw(tabname, raw_num);
    }
    if (!xmlStrcmp(node->name, BAD_CAST "join")) {
        char* tabname1 = (char*)xmlGetProp(node, BAD_CAST "tabname1");
        char* tabname2 = (char*)xmlGetProp(node, BAD_CAST "tabname2");
        int raw_num1 = atoi((char*)xmlGetProp(node, BAD_CAST "raw_num1"));
        int raw_num2 = atoi((char*)xmlGetProp(node, BAD_CAST "raw_num2"));

        struct ast* filter_list = xml2ast(get_child(node));

        ast_node = newjoin(tabname1, tabname2, raw_num1, raw_num2, filter_list);
    }
    if (!xmlStrcmp(node->name, BAD_CAST "select")) {
        char* tabname = (char*)xmlGetProp(node, BAD_CAST "tabname");
        struct ast* filter_list = xml2ast(get_child(node));
        ast_node = newselect(tabname, filter_list);
    }
    if (!xmlStrcmp(node->name, BAD_CAST "delete")) {
        char* tabname = (char*)xmlGetProp(node, BAD_CAST "tabname");
        struct ast* filter_list = xml2ast(get_child(node));
        ast_node = newdelete(tabname, filter_list);
    }
    if (!xmlStrcmp(node->name, BAD_CAST "create")) {
        char* tabname = (char*)xmlGetProp(node, BAD_CAST "tabname");
        int raw_size = atoi((char*)xmlGetProp(node, BAD_CAST "raw_size"));
        ast_node = create_tableAst(tabname, raw_size);
    }
    if (!xmlStrcmp(node->name, BAD_CAST "insert")) {
        char* tabname = (char*)xmlGetProp(node, BAD_CAST "tabname");
        struct ast* list = xml2ast(get_child(node));
        ast_node = newinsert(tabname, list);
    }
    if (!xmlStrcmp(node->name, BAD_CAST "list")) {
        ast_node = get_list(node);
    }
    if (!xmlStrcmp(node->name, BAD_CAST "pair")) {
        int key = atoi((char*)xmlGetProp(node, BAD_CAST "key"));
        struct ast* ast_value = xml2ast(get_child(node));
        ast_node = newpair(key, ast_value);
    }
    if (!xmlStrcmp(node->name, BAD_CAST "int")) {
        int value = atoi((char*)xmlNodeGetContent(node->children));
        ast_node = newint(value);
    }
    if (!xmlStrcmp(node->name, BAD_CAST "string")) {
        char* value = (char*)xmlNodeGetContent(node->children);
        ast_node = newstring(value);
    }
    if (!xmlStrcmp(node->name, BAD_CAST "double")) {
        double value = atof((char*)xmlNodeGetContent(node->children));
        ast_node = newdouble(value);
    }


    return ast_node;
}


struct ast* parse_xml_to_ast(const char* xml_content, int content_len) {
    xmlDocPtr doc;
    xmlNodePtr root_element;

    doc = xmlReadMemory(xml_content, content_len, NULL, NULL, 0);
    if (doc == NULL) {
        fprintf(stderr, "Failed to parse document\n");
        return NULL;
    }

    root_element = xmlDocGetRootElement(doc);
    if (root_element == NULL) {
        fprintf(stderr, "Empty document\n");
        xmlFreeDoc(doc);
        return NULL;
    }

    xmlNodePtr first_child = get_child(root_element);

    struct ast* ast_root = xml2ast(first_child);
    xmlFreeDoc(doc);
    xmlCleanupParser();
    return ast_root;
}

char* data2xml(struct raw_sequence* ret) {
    xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
    xmlNodePtr root_element = xmlNewNode(NULL, BAD_CAST "root");
    xmlDocSetRootElement(doc, root_element);
    
    struct raw_sequence* copy = ret;
    
    
    while (copy != NULL) {
        xmlNodePtr xmlNode;
        xmlNode = xmlNewChild(root_element, NULL, BAD_CAST "raw_data", NULL);
        for (int i = 0; i < copy->len; i++) {
            char prop_name[3];
            _itoa_s(i, prop_name, 3, 10);
            if (copy->raw_data[i].flag == INT_DATA) {
                char buffer[256];
                snprintf(buffer, sizeof(buffer), "%d", copy->raw_data[i].int_data);
                xmlNewProp(xmlNode, BAD_CAST prop_name, BAD_CAST buffer);
            }
            if (copy->raw_data[i].flag == STRING_DATA) {
                char buffer[256];
                snprintf(buffer, sizeof(buffer), "%s", copy->raw_data[i].string_link.string_data);
                xmlNewProp(xmlNode, BAD_CAST prop_name, BAD_CAST buffer);
            }
            if (copy->raw_data[i].flag == DOUBLE_DATA) {
                char buffer[256];
                snprintf(buffer, sizeof(buffer), "%f", copy->raw_data[i].double_data);
                xmlNewProp(xmlNode, BAD_CAST prop_name, BAD_CAST buffer);
            }    
        }
        copy = copy->next;
    }

    xmlChar* xmlbuff;
    int buffersize;
    xmlDocDumpFormatMemory(doc, &xmlbuff, &buffersize, 1);
    // xmlSaveFormatFileEnc("-", doc, "UTF-8", 1);
    xmlFreeDoc(doc);
    xmlCleanupParser();
    return (char*)xmlbuff;
}
