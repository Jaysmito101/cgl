#define CGL_IMPLEMENTATION
#define CGL_EXCLUDE_TEXT_RENDER
#define CGL_EXCLUDE_NETWORKING
#define CGL_LOG printf
#include "cgl.h"


int main(int argc, char** argv)
{
    if (argc != 4)
    {
        printf("Invalid number of arguments\n");
        return -1;
    }
    // for random output on every run
    srand((uint32_t)time(NULL));


    // constants
    size_t trainer_data_size = 0;
    size_t n_gram_size = 4;


    char* trainer_data = CGL_utils_read_file(argv[1], &trainer_data_size);
    trainer_data_size = strlen(trainer_data);
    printf("trainer_data_size: %zu\n", trainer_data_size);

    // setup context
    printf("Creating Markov Context ... ");
    CGL_markov* mk = CGL_markov_create(n_gram_size, 1);
    CGL_markov_token_function_ngram_text_context* ng_ctx = CGL_markov_token_function_ngram_text_context_create(n_gram_size);
    printf("Done\n");

    // train and build data table
    printf("Training Markov Chains ... ");
    CGL_markov_train(mk, ng_ctx, trainer_data, trainer_data_size, CGL_markov_token_function_ngram_text);
    printf("Done\n");

    // free ngram context (only required for training)
    CGL_markov_token_function_ngram_text_context_destroy(ng_ctx);

    // prepare for generation
    size_t count = atoi(argv[2]);
    static char temp_buffer[4096];
    memset(temp_buffer, 0, sizeof(temp_buffer));
    static char temp_buffer2[4];
    memset(temp_buffer2, 0, sizeof(temp_buffer2));
    sprintf(temp_buffer, argv[3]);

    const char* key = temp_buffer + (strlen(argv[3]) == n_gram_size ? 0 : (strlen(argv[3]) - n_gram_size));

    // generate data
    printf("Generating Text ... ");
    while((count--) > 0 && CGL_markov_generate(mk, key++, temp_buffer2)) strcat(temp_buffer, temp_buffer2);
    printf("Done\n");
    printf("Generated text is : \n%s", temp_buffer);

    // destroy context
    CGL_markov_destroy(mk);
    CGL_free(trainer_data);

    return 0;
}
