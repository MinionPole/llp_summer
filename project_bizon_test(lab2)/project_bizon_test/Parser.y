%{
#include "ast.h"
#include <stdio.h>
%}

%error-verbose

%union {
    int ival;
    double fval;
    char *sval;
    struct ast *ast;
    int subtok;
}

%token <sval> VARNAME STRINGVAL
%token <ival> INTVAL
%token <ival> BOOLVAL
%token <fval> DOUBLEVAL

%left AND
%left OR


/*signs*/
%token COMA
%token DOT
%token DOUBLE_POINT

/*DDL*/
%token DROP
%token CREATE
%token FILTER
%token FOR
%token RETURN
%token REMOVE
%token UPDATE
%token SELECT
%token FROM
%token JOIN
%token ON
%token TO
%token DELETE



/*utility*/
%token OPENBR_CIR CLOSEBR_CIR
%token OPENBR_FIG CLOSEBR_FIG
%token TABLENUM RAWSIZE
%token INTO
%token DOT_COMA
%token INSERT 
%token TABLE
%token IN
%token WITH
%token EQ NEQ GREATER LESSER GREATER_EQ LESSER_EQ


%type<ast> terminal
%type<ast> drop_table create_table
%type<ast> insert_to_table doc pairs pair
%type<ast> for_part_list for_part
%type<ast> filter_pair table_raw_id final_filter_node filter_sequence
%type<ast> query for_query queries
%type<ast> constant
%type<ast> CMP_SIGN
%type<ast> conclusion return_conclusion table_raw_id_list
%type<ast> remove_conclusion remove_varname_list update_conclusion
%type<ast> select_query join_query delete_query

%locations
%parse-param { struct ast **root }
%start queries

%%
queries: query { $$ = $1; print_ast(stdout, *root, 0); $$ = NULL;};
query: terminal DOT_COMA { $$ = newlist($1, NULL); *root = $$;}
     | query terminal DOT_COMA{ $$ = newlist($2, $1); *root = $$;};
terminal: drop_table | create_table | insert_to_table | for_query | select_query | join_query | delete_query;

constant: INTVAL                         { $$ = newint($1); *root= $$; }
        | DOUBLEVAL                       { $$ = newdouble($1); *root= $$;}
        | BOOLVAL                        { $$ = newbool($1); *root= $$;}
        | STRINGVAL                      { $$ = newstring($1); *root= $$;};

CMP_SIGN: EQ                         { $$ = newsign("=="); *root= $$; }
        | NEQ                       { $$ = newsign("!="); *root= $$;}
        | GREATER                        { $$ = newsign(">"); *root= $$;}
        | LESSER                      { $$ = newsign("<"); *root= $$;}
        | GREATER_EQ                        { $$ = newsign(">="); *root= $$;}
        | LESSER_EQ                      { $$ = newsign("<="); *root= $$;} ;
 

//drop
drop_table: DROP TABLE VARNAME { $$ = newdrop($3); *root= $$; };

//create
create_table: CREATE TABLE VARNAME RAWSIZE OPENBR_CIR INTVAL CLOSEBR_CIR { $$ = create_tableAst($3, $6); *root= $$; };

//insert 

insert_to_table: INSERT OPENBR_FIG pairs CLOSEBR_FIG TO VARNAME {$$=newinsert($6, $3); *root = $$;};

doc: pairs {$$ = $1;}

pairs: pair { $$ = newlist($1, NULL);*root= $$;}
     | pairs COMA pair { $$ = newlist($3, $1);*root= $$;};

pair: INTVAL DOUBLE_POINT constant {$$ = newpair($1, $3);*root= $$;};

//select
select_query: SELECT FROM VARNAME final_filter_node{ $$ = newselect($3, $4); *root= $$; }
            | SELECT FROM VARNAME{ $$ = newselect($3, NULL); *root= $$; };


//join
join_query: JOIN VARNAME TO VARNAME ON INTVAL EQ INTVAL final_filter_node { $$ = newjoin($2, $4, $6, $8, $9); *root= $$; }
          | JOIN VARNAME TO VARNAME ON INTVAL EQ INTVAL { $$ = newjoin($2, $4, $6, $8, NULL); *root= $$; };

//delete
delete_query: DELETE FROM VARNAME final_filter_node { $$ = newdelete($3, $4); *root= $$; }
            | DELETE FROM VARNAME { $$ = newdelete($3, NULL); *root= $$; };


// for queries
for_query: for_part_list final_filter_node conclusion {$$ = newforquery($1, $2, $3);*root= $$;}
           | for_part_list conclusion {$$ = newforquery($1, NULL, $2);*root= $$;};

// for part
for_part_list: for_part {$$ = newlist($1, NULL);*root= $$;}
             | for_part_list for_part_list {$$ = newlist($1, $2);*root = $$;};

for_part: FOR VARNAME IN TABLENUM OPENBR_CIR INTVAL CLOSEBR_CIR {$$ = newforast($2, $6); *root= $$;};

// filter part
final_filter_node: FILTER filter_sequence {$$ = $2};

filter_sequence: filter_pair {$$ = newfilterlist($1, NULL, NULL);*root= $$;}
                 | filter_sequence AND filter_sequence {$$ = newfilterlist($1, 1, $3);*root= $$;}
                 | filter_sequence OR filter_sequence {$$ = newfilterlist($1, 2, $3);*root= $$;};

filter_pair: table_raw_id CMP_SIGN constant {$$ = newfilterpair($1, $2, $3); *root= $$;}
            | table_raw_id CMP_SIGN table_raw_id {$$ = newfilterpair($1, $2, $3); *root= $$;};;

table_raw_id: VARNAME DOT INTVAL {$$ = newtablefraw($1, $3); *root = $$;};


// conclusion part
conclusion: return_conclusion {$$ = $1}
           | remove_conclusion{$$ = $1}
           | update_conclusion{$$ = $1};

return_conclusion: RETURN OPENBR_FIG table_raw_id_list CLOSEBR_FIG {$$ = newreturn($3);*root = $$;}

remove_conclusion: REMOVE remove_varname_list {$$ = newremove($2);*root = $$;};

update_conclusion: UPDATE VARNAME WITH OPENBR_FIG pairs CLOSEBR_FIG {$$ = newupdate($2, $5);*root = $$;};

remove_varname_list: VARNAME {$$ = newlist(newstring($1), NULL);*root = $$;}
            | remove_varname_list remove_varname_list {$$ = newlist($1, $2);*root = $$;};

table_raw_id_list: table_raw_id {$$ = newlist($1, NULL);*root = $$;}
                 | table_raw_id_list COMA table_raw_id_list {$$ = newlist($1, $3);*root = $$;};

%%

int yyerror(char *s) {
    fprintf(stderr, "error: %s\n", s);
}

struct ast * parse_ast(){
    struct ast *root = NULL;
    yyparse(&root);
    return root;
}

/*
join varname to varname on INTVAL EQ INTVAL final_filter_node;

select table_raw_id_list from varname final_filter_node;

delete from varname final_filter_node;

update varname with pairs final_filter_node;
*/