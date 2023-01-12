CFLAGS=-g -O1 -I.. -I/usr/local/include/freetype2
LIBS=-lGL -lGLU -lglut -lglfw -lfreetype -lm
all: trail_renderer vector_field_visualizer markov_text_generation

trail_renderer: trail_renderer.c ../glad.c ../cgl.h 
	$(CC) $(CFLAGS) -o $@ trail_renderer.c ../glad.c $(LIBS) \
		-DCGL_LOGGING_ENABLED
# sudo apt-get install libopenal1 libopenal-dev
vector_field_visualizer: vector_field_visualizer.c ../glad.c ../cgl.h 
	$(CC) $(CFLAGS) -o $@ vector_field_visualizer.c ../glad.c $(LIBS) \
		-lssl -lcrypto -lopenal
markov_text_generation: markov_text_generation.c ../cgl.h
	$(CC) $(CFLAGS) -o $@ markov_text_generation.c ../glad.c -lm
