#include "tokenizer.h"

int main() {
  char *code = "int x = (5)*3; while (x != 0) { x = x + 1 }";
  Token *tokens;
  ssize_t token_count;
  printf("error code: %s\n", tokenize(code, &tokens, &token_count));
  printf("token count: %zu\n", token_count);
  for (ssize_t i = 0; i < token_count; ++i) {
    char *s = token_to_string(tokens[i]);
    printf("%zu: %s\n", i, s);
    free(s);
  }
}
