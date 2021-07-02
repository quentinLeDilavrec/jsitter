#include <tree_sitter/api.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <stddef.h>
//#include "java_api.cpp"
#include <assert.h>

TSLanguage *tree_sitter_json();
TSLanguage *tree_sitter_go();

bool ts_zipper_next(TSZipper *zipper, TSZipper *res, TSLanguage *lang) {
    bool down = ts_zipper_down(zipper, res, lang);
    if (down) {
        return true;
    }
    bool right = ts_zipper_right(zipper, res);
    if (right) {
        return true;
    }
    while ((zipper = ts_zipper_up(zipper))) {
        if (ts_zipper_right(zipper, res)) {
            return true;
        }
    }
    return false;
}

void perf() {
    FILE *f = fopen("/Users/jetzajac/Projects/jsitter/testData/router_go", "r");
    fseek (f, 0, SEEK_END);
    size_t s = ftell(f);
    rewind(f);
    void *b = malloc(s);
    fread(b, s, 1, f);
    TSParser *parser = ts_parser_new();
    ts_parser_set_language(parser, tree_sitter_go());
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    TSTree *tree = ts_parser_parse_string(parser,
                                          NULL,
                                          (const char *)b,
                                          s);
    clock_gettime(CLOCK_MONOTONIC, &end);
    uint64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
    printf("took %llu\n", delta_us);
}

void ranges() {
    TSParser *parser = ts_parser_new();
    TSLanguage *lang = tree_sitter_go();
    ts_parser_set_language(parser, lang);
    const char *str_go = "func hello() { sayHello() }";
    TSTree *tree = ts_parser_parse_string(
                                          parser,
                                          NULL,
                                          str_go,
                                          strlen(str_go)
                                          );
    const char *str_go2 = "func hello() { sayHello }";
    TSInputEdit e; // + 8
    e.start_byte = 15 + 8;
    e.old_end_byte = 15 + 8 + 2;
    e.new_end_byte = 15 + 8;
    ts_tree_edit(tree, &e);
    TSTree *tree2 = ts_parser_parse_string(parser, tree, str_go2, strlen(str_go2));
    uint32_t len;
    
    
    TSRange *ranges = ts_tree_get_changed_ranges(tree, tree2, &len);
    
    char *new_string = ts_node_string(ts_tree_root_node(tree2));
    printf("New Syntax tree: %s\n", new_string);
    char *old_string = ts_node_string(ts_tree_root_node(tree));
    printf("Old Syntax tree: %s\n", old_string);
    
    const char *str_go3 = "func hello() { sayHello() }";
    TSInputEdit e2; // + 8
    e2.start_byte = 15 + 8;
    e2.new_end_byte = 15 + 8 + 2;
    e2.old_end_byte = 15 + 8;
    ts_tree_edit(tree2, &e2);
    TSTree *tree3 = ts_parser_parse_string(parser, tree2, str_go3, strlen(str_go3));
    uint32_t len2;
    
    
    TSRange *ranges2 = ts_tree_get_changed_ranges(tree, tree2, &len2);
    
    char *new_string3 = ts_node_string(ts_tree_root_node(tree3));
    printf("New Syntax tree: %s\n", new_string3);
}

void str_insert(size_t index, char c, char *str, size_t size) {
  memmove(str + index + 1, str + index, size - index);
  str[index] = c;
}

void test_crash() {
  FILE *f = fopen("/Users/jetzajac/Projects/jsitter/testData/router_go", "r");
  fseek (f, 0, SEEK_END);
  size_t s = ftell(f);
  rewind(f);
  void *b = malloc(s + 1000);
  fread(b, s, 1, f);
  TSParser *parser = ts_parser_new();
  ts_parser_set_language(parser, tree_sitter_go());
  TSTree *tree = ts_parser_parse_string(parser,
                                        NULL,
                                        (const char *)b,
                                        s);
  TSTree *copy = ts_tree_copy(tree);
  //for (int i = 0; i < 100; ++i) {
  int i = 2; {
    printf("i = %d\n", i);
    //str_insert(1000 + i, 'x', b, s + i);
    TSTree *copy = ts_tree_copy(tree);
    TSInputEdit e; // + 8
    e.start_byte = 2000;
    e.old_end_byte = 2000;
    e.new_end_byte = 2000 + i;
    ts_tree_edit(copy, &e);
    TSTree *new_tree = ts_parser_parse_string(parser, copy, b, s);
    ts_tree_delete(copy);
    ts_tree_delete(new_tree);
    ts_parser_reset(parser);
  }
}




typedef struct {
  uint32_t bytes;
  TSPoint extent;
} CLength;

typedef uint16_t TSStateId;

typedef struct {
  union {
    char *long_data;
    char short_data[24];
  };
  uint32_t length;
} CExternalScannerState;

typedef struct {
  volatile uint32_t ref_count;
  CLength padding;
  CLength size;
  uint32_t lookahead_bytes;
  uint32_t error_cost;
  uint32_t child_count;
  TSSymbol symbol;
  TSStateId parse_state;

  bool visible : 1;
  bool named : 1;
  bool extra : 1;
  bool fragile_left : 1;
  bool fragile_right : 1;
  bool has_changes : 1;
  bool has_external_tokens : 1;
  bool depends_on_column: 1;
  bool is_missing : 1;
  bool is_keyword : 1;

  union {
    // Non-terminal subtrees (`child_count > 0`)
    struct {
      uint32_t visible_child_count;
      uint32_t named_child_count;
      uint32_t node_count;
      uint32_t repeat_depth;
      int32_t dynamic_precedence;
      uint16_t production_id;
      struct {
        TSSymbol symbol;
        TSStateId parse_state;
      } first_leaf;
    };

    // External terminal subtrees (`child_count == 0 && has_external_tokens`)
    CExternalScannerState external_scanner_state;

    // Error terminal subtrees (`child_count == 0 && symbol == ts_builtin_sym_error`)
    int32_t lookahead_char;
  };
} CSubtreeHeapData;



typedef union {
  struct {
    uint8_t type;
    TSStateId state;
    bool extra;
    bool repetition;
  } shift;
  struct {
    uint8_t type;
    uint8_t child_count;
    TSSymbol symbol;
    int16_t dynamic_precedence;
    uint16_t production_id;
  } reduce;
  uint8_t type;
} TSParseAction;

typedef union {
  TSParseAction action;
  struct {
    uint8_t count;
    bool reusable;
  } entry;
} TSParseActionEntry;


typedef struct {
  TSFieldId field_id;
  uint8_t child_index;
  bool inherited;
} TSFieldMapEntry;

typedef struct {
  uint16_t index;
  uint16_t length;
} TSFieldMapSlice;

typedef struct {
  bool visible;
  bool named;
  bool supertype;
} TSSymbolMetadata;

typedef struct {
  uint32_t version;
  uint32_t symbol_count;
  uint32_t alias_count;
  uint32_t token_count;
  uint32_t external_token_count;
  uint32_t state_count;
  uint32_t large_state_count;
  uint32_t production_id_count;
  uint32_t field_count;
  uint16_t max_alias_sequence_length;
  const uint16_t *parse_table;
  const uint16_t *small_parse_table;
  const uint32_t *small_parse_table_map;
  const TSParseActionEntry *parse_actions;
  const char * const *symbol_names;
  const char * const *field_names;
  const TSFieldMapSlice *field_map_slices;
  const TSFieldMapEntry *field_map_entries;
  const TSSymbolMetadata *symbol_metadata;
  const TSSymbol *public_symbol_map;
  const uint16_t *alias_map;
  const TSSymbol *alias_sequences;
} aaa;

int memoryLayout() {

  CSubtreeHeapData * ptr = (CSubtreeHeapData *)0;

  
  //printf(sizeof(*ptr));
  printf("%d\n",((int)(&ptr->ref_count)));
  printf("%d\n",((int)(&ptr->padding))-((int)(&ptr->ref_count)));
  printf("%d\n",((int)(&ptr->size))-((int)(&ptr->padding)));
  printf("%d\n",((int)(&ptr->lookahead_bytes))-((int)(&ptr->size)));
  printf("%d\n",((int)(&ptr->error_cost))-((int)(&ptr->lookahead_bytes)));
  printf("%d\n",((int)(&ptr->child_count))-((int)(&ptr->error_cost)));
  printf("%d\n",((int)(&ptr->symbol))-((int)(&ptr->child_count)));
  printf("%d\n",((int)(&ptr->parse_state))-((int)(&ptr->symbol)));
  printf("%d\n",((int)(&ptr->visible_child_count)));
  printf("%d\n",((int)(&ptr->named_child_count))-((int)(&ptr->visible_child_count)));
  printf("inc node_count: %d\n",((int)(&ptr->node_count))-((int)(&ptr->named_child_count)));
  printf("%d\n",((int)(&ptr->repeat_depth))-((int)(&ptr->node_count)));
  printf("%d\n",((int)(&ptr->dynamic_precedence))-((int)(&ptr->repeat_depth)));
  printf("%d\n",((int)(&ptr->production_id))-((int)(&ptr->dynamic_precedence)));
  printf("production_id: %d\n",((int)(&ptr->production_id)));

  
  aaa * ptr2 = (aaa *)0;

  printf("offset alias_sequences %d\n",((int)(&ptr2->alias_sequences)));
  printf("offset max_alias_sequence_length %d\n",((int)(&ptr2->max_alias_sequence_length)));

}

int main () {
  printf("tests:\n");
  memoryLayout();
  //    ranges();
  // test_crash();
  // perf();
    return 0;
}
