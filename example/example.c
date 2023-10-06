#include <nadeshiko.h>

typedef struct {
  Nadeshiko* instance;
  unsigned int count;
} context_t;

void increment(const char *seq, const char *req, void *arg) {
  (void)req;
  context_t *context = (context_t *)arg;
  char count_string[10] = {0};
  sprintf(count_string, "%u", ++context->count);
  char result[21] = {0};
  strcat(result, "{\"count\": ");
  strcat(result, count_string);
  strcat(result, "}");
  nadeshiko_return(context->instance, seq, 0, result);
}

int main() {
    nadeshiko_init("./web");

    Nadeshiko* instance = (Nadeshiko *)malloc(sizeof(Nadeshiko));
    instance = nadeshiko_create_window(
        "Example",
        "index.html",
        500,
        500
    );

    context_t context = {.instance = instance, .count = 0};
    nadeshiko_bind(instance, "increment", increment, &context);

    nadeshiko_run(instance);
    nadeshiko_destroy(instance);

    nadeshiko_stop();

    return 0;
}