/*
MIT License

Copyright (c) 2023 Jaysmito Mukherjee (jaysmito101@gmail.com)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#define CGL_LOGGING_ENABLED
#define CGL_EXCLUDE_TEXT_RENDER
#define CGL_EXCLUDE_NETWORKING
#define CGL_IMPLEMENTATION
#include "cgl.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static const char* PASS_THROUGH_VERTEX_SHADER_SOURCE = "#version 430 core\n"
"\n"
"layout (location = 0) in vec4 position;\n"
"layout (location = 1) in vec4 normal;\n"
"layout (location = 2) in vec4 texcoord;\n"
"\n"
"out vec3 Position;\n"
"out vec2 TexCoord;\n"
"\n"
"void main()\n"
"{\n"
"	gl_Position = vec4(position.xyz, 1.0f);\n"
"	Position = position.xyz;\n"
"	TexCoord = texcoord.xy;\n"
"}";

static const char* PASS_THROUGH_FRAGMENT_SHADER_SOURCE = "#version 430 core\n"
"\n"
"out vec4 FragColor;\n"
"\n"
"in vec3 Position;\n"
"in vec2 TexCoord;\n"
"\n"
"uniform sampler2D u_texMap;\n"
"uniform sampler2D u_tex;\n"
"uniform sampler2D u_texSky;\n"
"uniform float u_fov;\n"
"uniform vec2 u_ppos;\n"
"uniform float u_dir;\n"
"uniform float u_map_size;\n"
"uniform int u_rays;\n"
"\n"
"float get_angle(vec2 uv)\n"
"{\n"
"   float angle = u_dir - u_fov * 0.5f + u_fov * uv.x;\n"
"   return angle;\n"
"}\n"
"\n"
"float get_wall_uvx(vec2 uv, vec4 data)\n"
"{\n"
"	float ray_angle = get_angle(uv);\n"
"   float ray_hit_x = u_ppos.x + data.x * cos(ray_angle);\n"
"   float ray_hit_y = u_ppos.y + data.x * sin(ray_angle);\n"
"   if (data.y > 0.5f) {\n"
"       return ray_hit_x - ( floor(ray_hit_x * u_map_size) / u_map_size );\n"
"   } else {\n"
"       return ray_hit_y - ( floor(ray_hit_y * u_map_size) / u_map_size );\n"
"   }\n"
"}\n"
"\n"
"vec3 wall_color(vec4 data, vec2 uv, float lineHeight)\n"
"{\n"
"   vec3 color = vec3(0.0f);\n"
"   float yOffset = (1.0f - lineHeight) * 0.5f;\n"
"   vec2 texCoord = vec2(0.0f);\n"
"   texCoord.y = (uv.y - yOffset) / lineHeight;\n"
"   texCoord.y = texCoord.y * 0.2 + data.w;\n"
"   texCoord.x = get_wall_uvx(uv, data) * u_map_size * 0.2 + data.z;\n"
"   color = texture(u_texMap, texCoord).xyz;\n"
"   color *= (0.8 + data.y); \n // wall\n"
"   return color;\n"
"}\n"
"\n"
"vec3 floor_color(vec4 data, vec2 uv, float lineHeight)\n"
"{\n"
"   vec3 color = vec3(0.0f);\n"
"   float yOffset = (1.0f - lineHeight) * 0.5f;\n"
"   vec2 texCoord = vec2(0.0f);\n"
"   texCoord.y = (uv.y * yOffset);\n"
"   texCoord.y = texCoord.y;\n"
"   texCoord.x = get_wall_uvx(uv, data) * u_map_size * 0.2 + 0.6;\n"
"   color = texture(u_texMap, texCoord).xyz;\n"
"   color *= (0.8 + data.y); \n // wall\n"
"   return mix ( vec3(0.1f, texCoord.y + 0.2, 0.1f), color, pow(texCoord.y, 2.0f));\n"
"}\n"
"\n"
"void main()\n"
"{\n"
"   vec2 uv = vec2(TexCoord.y, 1.0f - TexCoord.x);\n"
"	vec4 data= texture(u_tex, uv);\n"
"	vec3 color = vec3(0.0f);\n"
"   float lineHeight = data.r * cos(get_angle(uv) - u_dir);\n"
"   lineHeight = 0.01 / (lineHeight  * lineHeight + 0.000001f);\n"
"   if (uv.y < 0.5f - lineHeight * 0.5f) {\n"
"       color = floor_color(data, uv, lineHeight);\n"
"   } else if (uv.y > 0.5f + lineHeight * 0.5f) {\n"
"       color = texture(u_texSky, vec2(uv.x + u_dir, uv.y)).xyz;\n"
"   } else {\n"
"       color = wall_color(data, uv, lineHeight);\n"
"   }\n"
"	FragColor = vec4(color, 1.0f);\n"
"}";


#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define WINDOW_WIDTH         1600
#define WINDOW_HEIGHT        800
#define GAME_WIDTH           800
#define GAME_HEIGHT          600
#define MAP_SIZE             30
#define MAP_EDITOR_SIZE      600
#define MAP_EDITOR_TILE_SIZE ( MAP_EDITOR_SIZE / MAP_SIZE )
#define NUM_TILE_TEXTURES    20
#define TEXTURE_TILE_WIDTH   0.2f
#define TEXTURE_TILE_HEIGHT  0.25f
#define PLAYER_FOV			 CGL_deg_to_rad( 45.0 )

#define SAMPLE_MAP(x, y)	 ( MAP[ ( ( y ) * MAP_SIZE ) + ( x ) ] )

static CGL_byte MAP[] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

static CGL_texture* g_TilemapTexture = NULL;
static CGL_texture* g_SkyTexture = NULL;
static CGL_float g_TilemapOffset[NUM_TILE_TEXTURES][2] = {
	{ TEXTURE_TILE_WIDTH * 0.0f, TEXTURE_TILE_HEIGHT * 1.2f * 0.0f},
	{ TEXTURE_TILE_WIDTH * 1.0f, TEXTURE_TILE_HEIGHT * 0.0f},
	{ TEXTURE_TILE_WIDTH * 2.0f, TEXTURE_TILE_HEIGHT * 0.0f},
	{ TEXTURE_TILE_WIDTH * 3.0f, TEXTURE_TILE_HEIGHT * 0.0f},
	{ TEXTURE_TILE_WIDTH * 4.0f, TEXTURE_TILE_HEIGHT * 0.0f},
	{ TEXTURE_TILE_WIDTH * 0.0f, TEXTURE_TILE_HEIGHT * 1.0f},
	{ TEXTURE_TILE_WIDTH * 1.0f, TEXTURE_TILE_HEIGHT * 1.0f},
	{ TEXTURE_TILE_WIDTH * 2.0f, TEXTURE_TILE_HEIGHT * 1.0f},
	{ TEXTURE_TILE_WIDTH * 3.0f, TEXTURE_TILE_HEIGHT * 1.0f},
	{ TEXTURE_TILE_WIDTH * 4.0f, TEXTURE_TILE_HEIGHT * 1.0f},
	{ TEXTURE_TILE_WIDTH * 0.0f, TEXTURE_TILE_HEIGHT * 2.0f},
	{ TEXTURE_TILE_WIDTH * 1.0f, TEXTURE_TILE_HEIGHT * 2.0f},
	{ TEXTURE_TILE_WIDTH * 2.0f, TEXTURE_TILE_HEIGHT * 2.0f},
	{ TEXTURE_TILE_WIDTH * 3.0f, TEXTURE_TILE_HEIGHT * 2.0f},
	{ TEXTURE_TILE_WIDTH * 4.0f, TEXTURE_TILE_HEIGHT * 2.0f},
	{ TEXTURE_TILE_WIDTH * 0.0f, TEXTURE_TILE_HEIGHT * 3.0f},
	{ TEXTURE_TILE_WIDTH * 1.0f, TEXTURE_TILE_HEIGHT * 3.0f},
	{ TEXTURE_TILE_WIDTH * 2.0f, TEXTURE_TILE_HEIGHT * 3.0f},
	{ TEXTURE_TILE_WIDTH * 3.0f, TEXTURE_TILE_HEIGHT * 3.0f},
	{ TEXTURE_TILE_WIDTH * 4.0f, TEXTURE_TILE_HEIGHT * 3.0f}
};

static CGL_window* g_Window = NULL;

static CGL_float g_RayCastResult[GAME_WIDTH * 4] = {0.0f};

static struct {
	CGL_int selectedTile;
	CGL_tilemap* tilemap;
} g_MapEditor;

static struct {
	CGL_vec2 position;
	CGL_float direction;
	CGL_bool mouseLocked;
	CGL_vec2 prevMousePos;

	CGL_shader* raycastShader;
	CGL_texture* raycastData;
	CGL_framebuffer* gameView;
} g_Game;


// ---------------------------------------------------------------
// ---------------------- UTILS ----------------------------------
// ---------------------------------------------------------------

CGL_texture* load_texture(const char* path)
{
	int width, height, channels;
	unsigned char* data = stbi_load(path, &width, &height, &channels, 0);
	if (data == NULL) return NULL;

	CGL_image img = {
		.bytes_per_channel = 8,
		.channels = channels,
		.height = height,
		.width = width,
		.data = data
	};

	CGL_texture* texture = CGL_texture_create(&img);

	stbi_image_free(data);

	return texture;
}



CGL_bool raycast_single(CGL_float px, CGL_float py, CGL_float dir, CGL_int* wx, CGL_int* wy, CGL_float* dst, CGL_bool* hOv)
{
	// make 

	if (dir > CGL_PI) dir -= 2.0f * CGL_PI;
	else if (dir < -CGL_PI) dir += 2.0f * CGL_PI;

	// for horizontal lines
	CGL_int wx_h = -1, wy_h = -1;
	CGL_float dst_h = 1.0f;
	// if (fasle)
	{

		CGL_float tDir = tanf(CGL_PI_2 - dir);
		CGL_bool up = dir > 0.0f;


		CGL_float nextY = floorf(py * MAP_SIZE + (up ? 1.0f : 0.0f)) / MAP_SIZE + (up ? 0.00001f : -0.0001f);
		CGL_float nextX = px + (nextY - py) * tDir;
		
		CGL_float yStep = (up ? 1.0f : -1.0f) / MAP_SIZE;
		CGL_float xStep = yStep * tDir;


		while (nextX >= 0.0f && nextX <= 1.0f && nextY >= 0.0f && nextY <= 1.0f) {
			CGL_int x = (CGL_int)(nextX * MAP_SIZE);
			CGL_int y = (CGL_int)(nextY * MAP_SIZE);

			// CGL_widgets_add_circle2fr(nextX, nextY, 0.002f, 8);

			if (SAMPLE_MAP(x, y) > 0) {
				wx_h = x;
				wy_h = y;
				dst_h = sqrtf((nextX - px) * (nextX - px) + (nextY - py) * (nextY - py));
				break;
			}

			nextX += xStep;
			nextY += yStep;
		}

	}

	// for vertical lines
	CGL_int wx_v = -1, wy_v = -1;
	CGL_float dst_v = 100000.0f;
	//if(false)
	{

        CGL_float tDir = tanf(dir);
		CGL_bool right = (dir <= CGL_PI_2 && dir >= -CGL_PI_2);

		CGL_float nextX = floorf(px * MAP_SIZE + (right ? 1.0f : 0.0f)) / MAP_SIZE + (right ? 0.00001f : -0.0001f);
		CGL_float nextY = py + (nextX - px) * tDir;
		
		CGL_float xStep = (right ? 1.0f : -1.0f) / MAP_SIZE;
		CGL_float yStep = xStep * tDir;
		
		while (nextX >= 0.0f && nextX <= 1.0f && nextY >= 0.0f && nextY <= 1.0f) {
			CGL_int x = (CGL_int)(nextX * MAP_SIZE);
			CGL_int y = (CGL_int)(nextY * MAP_SIZE);
		
			//CGL_widgets_add_circle2fr(nextX, nextY, 0.002f, 8);

			if (SAMPLE_MAP(x, y) > 0) {
				wx_v = x;
				wy_v = y;
				dst_v = sqrtf((nextX - px) * (nextX - px) + (nextY - py) * (nextY - py));
				break;
			}
			
			nextX += xStep;
			nextY += yStep;
		}

		if (wx_v == -1) {
			dst_v = sqrtf((nextX - px) * (nextX - px) + (nextY - py) * (nextY - py));
		}
	}

	if (dst_h < dst_v) {
		*wx = wx_h;
		*wy = wy_h;
		*dst = dst_h;
		if (hOv) *hOv = true;
	}
	else {
		*wx = wx_v;
		*wy = wy_v;
		*dst = dst_v;
		if (hOv) *hOv = false;
	}

	return (*wx != -1 && *wy != -1);
}

// ---------------------------------------------------------------
// ---------------------- MAP EDITOR -----------------------------
// ---------------------------------------------------------------

static CGL_void upload_tilemap_data()
{
	static CGL_vec3 s_TileColor;
	for (CGL_int y = 0; y < MAP_SIZE; y++)
	{
		for (CGL_int x = 0; x < MAP_SIZE; x++)
		{
			CGL_int tileID = SAMPLE_MAP(x, y);
			if (tileID != 0)
			{
				CGL_float* tile = g_TilemapOffset[tileID - 1];
				CGL_tilemap_set_tile_color(g_MapEditor.tilemap, x, y, s_TileColor.x, s_TileColor.y, s_TileColor.z);
				CGL_tilemap_set_tile_texture_from_tileset(g_MapEditor.tilemap, x, y, tile[0], tile[1], TEXTURE_TILE_WIDTH, TEXTURE_TILE_HEIGHT);
			}
			else {
				CGL_tilemap_set_tile_color(g_MapEditor.tilemap, x, y, 0.1f, 0.1f, 0.1f);
			}
		}
	}
	CGL_tilemap_upload(g_MapEditor.tilemap);
}

static CGL_bool setup_map()
{
	// Create tilemap
	g_TilemapTexture = load_texture("assets/doomtilemap.png"); // https://i.ibb.co/jDvNj9f/doomtilemap.png
	g_SkyTexture = load_texture("assets/skylowres.png"); // https://i.ibb.co/ZWBghVy/skylowres.png
	g_MapEditor.tilemap = CGL_tilemap_create(MAP_SIZE, MAP_SIZE, MAP_EDITOR_TILE_SIZE, MAP_EDITOR_TILE_SIZE, 1);
	CGL_tilemap_set_auto_upload(g_MapEditor.tilemap, CGL_FALSE);
	upload_tilemap_data();
	return true;
}

static void update_map_editor(CGL_float mx, CGL_float my)
{
	mx *= (CGL_float)WINDOW_WIDTH / WINDOW_HEIGHT;
	my = 1.0f - my;

	CGL_float xTM = 200.0f / WINDOW_WIDTH;
	CGL_float yTM = 200.0f / WINDOW_HEIGHT;
	CGL_float sTM = (CGL_float)MAP_EDITOR_SIZE / WINDOW_HEIGHT;

	CGL_float tileMapPosX = (mx - xTM);
	CGL_float tileMapPosY = (my - yTM);

	CGL_float tilePosX = tileMapPosX / sTM;
	CGL_float tilePosY = tileMapPosY / sTM;


	if (CGL_aabb_contains_point(CGL_vec2_init(xTM, yTM), CGL_vec2_init(xTM + sTM, yTM + sTM), CGL_vec2_init(mx, my)))
	{
		CGL_int tileX = (CGL_int)(tilePosX * MAP_SIZE);
		CGL_int tileY = (CGL_int)(tilePosY * MAP_SIZE);

		if (CGL_window_get_mouse_button(g_Window, CGL_MOUSE_BUTTON_LEFT) == CGL_PRESS)
		{
			SAMPLE_MAP(tileX, tileY) = g_MapEditor.selectedTile;
		}
		else if (CGL_window_get_mouse_button(g_Window, CGL_MOUSE_BUTTON_RIGHT) == CGL_PRESS)
		{
			SAMPLE_MAP(tileX, tileY) = 0;
		}

		if (CGL_window_get_mouse_button(g_Window, CGL_MOUSE_BUTTON_MIDDLE) == CGL_PRESS)
		{
			g_MapEditor.selectedTile = SAMPLE_MAP(tileX, tileY);
		}
	}

	CGL_float scalingFactor = ((CGL_float)MAP_EDITOR_SIZE / WINDOW_HEIGHT) * 2.0f;
	// CGL_widgets_begin_int((CGL_float)WINDOW_HEIGHT / WINDOW_WIDTH * scalingFactor, 1.0f * scalingFactor, -0.5f, 0.25f); // for [-1, -1] to [1, 1]
	CGL_widgets_begin_int((CGL_float)WINDOW_HEIGHT / WINDOW_WIDTH * scalingFactor, 1.0f * scalingFactor, -0.875f, -0.5f); // for [0, 0] to [1, 1]

	CGL_vec2 startPos = g_Game.position;
	CGL_vec2 direction = CGL_vec2_from_angle(g_Game.direction);

	CGL_int wx = -1, wy = -1;
	CGL_float dst = 1.0f;
	// add 2 more rays at +- FOV/2
	CGL_widgets_set_stroke_thickness(0.002f);
	CGL_widgets_set_fill_colorf(0.8, 0.3, 0.3, 1.0);

	CGL_float startAngle = g_Game.direction - PLAYER_FOV * 0.5f;
	CGL_int numRays = 8;
	CGL_widgets_set_stroke_colorf(0.3, 0.3, 0.3, 1.0);
	for ( CGL_int i = 0; i < numRays; i++ )
	{
		CGL_float angle = startAngle + (CGL_float)i / (CGL_float)(numRays - 1) * PLAYER_FOV;
		CGL_bool hit = raycast_single(startPos.x, startPos.y, angle, &wx, &wy, &dst, NULL);
		direction = CGL_vec2_from_angle(angle);
		CGL_vec2 endPos = CGL_vec2_add_(startPos, CGL_vec2_scale_(direction, dst));
		CGL_widgets_set_stroke_colorf(0.3, 0.3, 0.3, 1.0);
		CGL_widgets_add_line2f(startPos.x, startPos.y, endPos.x, endPos.y);
		CGL_widgets_add_circle2fr(startPos.x, startPos.y, 0.01f, 8);
	}
	
	direction = CGL_vec2_from_angle(g_Game.direction);
	CGL_bool hit = raycast_single(startPos.x, startPos.y, g_Game.direction, &wx, &wy, &dst, NULL);
	CGL_vec2 endPos = CGL_vec2_add_(startPos, CGL_vec2_scale_(direction, dst));
	CGL_widgets_set_stroke_colorf(0.8, 0.3, 0.3, 1.0);
	CGL_widgets_add_line2f(startPos.x, startPos.y, endPos.x, endPos.y);
	CGL_widgets_add_circle2fr(startPos.x, startPos.y, 0.01f, 8);


	CGL_widgets_end();
}

static void render_map_editor(CGL_float mx, CGL_float my)
{

	CGL_tilemap_render(
		g_MapEditor.tilemap,
		1.0f, 1.0f,
		100.0, 200.f,
		g_TilemapTexture);

	update_map_editor(mx, my);
	upload_tilemap_data();


	// ui for selecting texture

	CGL_widgets_begin_int((CGL_float)WINDOW_HEIGHT / WINDOW_WIDTH, 1.0f, 0.0f, 0.0f);

	mx = (mx - 0.5f) * 2.0f;
	my = (0.5f - my) * 2.0f;

	mx *= (CGL_float)WINDOW_WIDTH / WINDOW_HEIGHT;
	CGL_float xStart = -(CGL_float)WINDOW_WIDTH / WINDOW_HEIGHT + 0.05f;
	CGL_float yStart = -0.7f;

	for (int i = 0; i < 20; i++)
	{

		if (CGL_aabb_contains_point(CGL_vec2_init(xStart, yStart), CGL_vec2_init(xStart + 0.10f, yStart + 0.10f), CGL_vec2_init(mx, my))) {
			if (CGL_window_get_mouse_button(g_Window, CGL_MOUSE_BUTTON_LEFT) == CGL_PRESS) {
				g_MapEditor.selectedTile = i;
			}
			CGL_widgets_set_fill_colorf(0.9, 0.5, 0.9, 1.0);
			CGL_widgets_add_rect2f(xStart - 0.005f, yStart - 0.005f, 0.11f, 0.11f);
		}

		if (i == g_MapEditor.selectedTile) {
			CGL_widgets_set_fill_colorf(0.9, 0.9, 0.9, 1.0);
			CGL_widgets_add_rect2f(xStart - 0.005f, yStart - 0.005f, 0.11f, 0.11f);
		}

		if (i == 0) CGL_widgets_set_fill_colorf(0.6, 0.6, 0.6, 1.0);
		else {
			CGL_widgets_set_texture(g_TilemapTexture);
			CGL_float* offset = g_TilemapOffset[i - 1];
			CGL_widgets_set_texture_coordinate_so(0.2f, 0.25f, offset[0], offset[1]);
		}
		CGL_widgets_add_rect2f(xStart, yStart, 0.10f, 0.10f);


		xStart += 0.12f;

		if (xStart >= (0.0f - 0.3f)) {
			xStart = -(CGL_float)WINDOW_WIDTH / WINDOW_HEIGHT + 0.05f;
			yStart -= 0.12f;
		}
	}

	CGL_widgets_end();
}


// ---------------------------------------------------------------
// ---------------------- GAME -----------------------------------
// ---------------------------------------------------------------

CGL_bool setup_game()
{
	g_Game.position = CGL_vec2_init(0.5f, 0.5f);
	g_Game.direction = 0.0f;

	g_Game.raycastShader = CGL_shader_create(PASS_THROUGH_VERTEX_SHADER_SOURCE, PASS_THROUGH_FRAGMENT_SHADER_SOURCE, NULL);
	g_Game.gameView = CGL_framebuffer_create(GAME_WIDTH, GAME_HEIGHT);

	g_Game.raycastData = CGL_texture_create_blank(GAME_WIDTH, 1, GL_RGBA, GL_RGBA32F, GL_FLOAT);



	return true;
}

CGL_void calculate_raycast_map()
{
	CGL_float stepSize = PLAYER_FOV / (CGL_float)GAME_WIDTH;
	CGL_float angle = g_Game.direction - PLAYER_FOV * 0.5f;

	CGL_int wx = 0, wy = 0;
	CGL_float dst = 10000.0f;
	CGL_bool hOv = false;

	for (int i = 0; i < GAME_WIDTH; i++, angle += stepSize)
	{
		if (raycast_single(g_Game.position.x, g_Game.position.y, angle, &wx, &wy, &dst, &hOv))
		{
			CGL_int tileID = SAMPLE_MAP(wx, wy) - 1;
			CGL_float* offset = g_TilemapOffset[tileID];
			CGL_float* result = &g_RayCastResult[i * 4];

			// CGL_float angDiff = angle - g_Game.direction; if (angDiff < 0.0f) angDiff += 2.0f * CGL_PI; if (angDiff > CGL_PI) angDiff = angDiff - 2.0f * CGL_PI;
			//CGL_float  dstToWall = dst * cosf (angDiff);
			result[0] = dst, result[1] = hOv ? 1.0f: 0.0f, result[2] = offset[0], result[3] = offset[1];
		}
		else
		{
			g_RayCastResult[i * 4] = -1.0f;
		}
	}

	CGL_texture_set_data(g_Game.raycastData, g_RayCastResult);
}

CGL_int get_player_pos_map(CGL_float x, CGL_float y)
{
	CGL_int wx = (CGL_int)(x * (CGL_float)MAP_SIZE);
	CGL_int wy = (CGL_int)(y * (CGL_float)MAP_SIZE);

	return SAMPLE_MAP(wx, wy);
}

CGL_void update_player_pos(CGL_float sideWays, CGL_float forward)
{
	// forward
	CGL_float dx = cosf(g_Game.direction) * forward;
	CGL_float dy = sinf(g_Game.direction) * forward;
	CGL_float currX = g_Game.position.x;
	CGL_float currY = g_Game.position.y;

	const CGL_float PADDING = 50.0f;

	if (get_player_pos_map(currX + dx * PADDING, currY + dy * PADDING) == 0) {
		g_Game.position.x += dx;
		g_Game.position.y += dy;
	}
	else if (get_player_pos_map(currX + dx * PADDING, currY) == 0) {
		g_Game.position.x += dx;
	}
	else if (get_player_pos_map(currX, currY + dy * PADDING) == 0) {
		g_Game.position.y += dy;
	}

	// sideways
	dx = cosf(g_Game.direction + CGL_PI * 0.5f) * sideWays;
	dy = sinf(g_Game.direction + CGL_PI * 0.5f) * sideWays;

	if (get_player_pos_map(currX + dx * PADDING, currY + dy * PADDING) == 0) {
		g_Game.position.x += dx;
		g_Game.position.y += dy;
	}
	else if (get_player_pos_map(currX + dx * PADDING, currY) == 0) {
		g_Game.position.x += dx;
	}
	else if (get_player_pos_map(currX, currY + dy * PADDING) == 0) {
		g_Game.position.y += dy;
	}
}

CGL_void update_game()
{
	if (CGL_window_is_key_pressed(g_Window, CGL_KEY_L)) {
		CGL_double mx, my;
		CGL_window_get_mouse_position(g_Window, &mx, &my);
		g_Game.prevMousePos = CGL_vec2_init((CGL_float)mx, (CGL_float)my);
		g_Game.mouseLocked = true;
		glfwSetInputMode(CGL_window_get_glfw_handle(g_Window), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	}
	else if (CGL_window_is_key_pressed(g_Window, CGL_KEY_U)) {
		g_Game.mouseLocked = false;
		glfwSetInputMode(CGL_window_get_glfw_handle(g_Window), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	if (g_Game.mouseLocked) {
		CGL_double mx = 0.0, my = 0.0;
		CGL_window_get_mouse_position(g_Window, &mx, &my);
		CGL_vec2 mousePos = CGL_vec2_init((CGL_float)mx, (CGL_float)my);
		CGL_vec2 delta = CGL_vec2_sub(mousePos, g_Game.prevMousePos);
		// g_Game.prevMousePos = mousePos;
		glfwSetCursorPos(CGL_window_get_glfw_handle(g_Window), g_Game.prevMousePos.x, g_Game.prevMousePos.y);

		g_Game.direction -= delta.x * 0.003f;
		// make sure g_Game.direction  stays within -PI and PI
		if (g_Game.direction > CGL_PI) g_Game.direction -= 2.0f * CGL_PI;
		else if (g_Game.direction < -CGL_PI) g_Game.direction += 2.0f * CGL_PI;

		if (CGL_window_is_key_pressed(g_Window, CGL_KEY_W)) {
			update_player_pos(0.0f, 0.0008f);
		}
		else if (CGL_window_is_key_pressed(g_Window, CGL_KEY_S)) {
			update_player_pos(0.0f, -0.0008f);
		}
		if (CGL_window_is_key_pressed(g_Window, CGL_KEY_A)) {
			update_player_pos(0.0008f, 0.0f);
		}
		else if (CGL_window_is_key_pressed(g_Window, CGL_KEY_D)) {
			update_player_pos(-0.0008f, 0.0f);
		}

	}

	calculate_raycast_map();


}

CGL_void render_game()
{
	update_game();

	CGL_widgets_begin();

	CGL_float height = (CGL_float)GAME_HEIGHT / (CGL_float)GAME_WIDTH;
	height *=  (CGL_float)WINDOW_WIDTH / (CGL_float)WINDOW_HEIGHT;

	CGL_widgets_set_texture(CGL_framebuffer_get_color_texture(g_Game.gameView));
	CGL_widgets_add_rect2f(-0.05, -1.0 + height * 0.25, 1.0, height);

	CGL_widgets_end();


	CGL_framebuffer_bind (g_Game.gameView);
	CGL_gl_clear(0.0f, 0.0f, 0.1f, 1.0f);
	CGL_shader_bind(g_Game.raycastShader);
	CGL_shader_set_uniform_int(g_Game.raycastShader, CGL_shader_get_uniform_location(g_Game.raycastShader, "u_tex"), CGL_texture_bind(g_Game.raycastData, 5));
	CGL_shader_set_uniform_int(g_Game.raycastShader, CGL_shader_get_uniform_location(g_Game.raycastShader, "u_texMap"), CGL_texture_bind(g_TilemapTexture, 6));
	CGL_shader_set_uniform_int(g_Game.raycastShader, CGL_shader_get_uniform_location(g_Game.raycastShader, "u_texSky"), CGL_texture_bind(g_SkyTexture, 4));
	CGL_shader_set_uniform_int(g_Game.raycastShader, CGL_shader_get_uniform_location(g_Game.raycastShader, "u_rays"), GAME_WIDTH);
	CGL_shader_set_uniform_float(g_Game.raycastShader, CGL_shader_get_uniform_location(g_Game.raycastShader, "u_fov"), PLAYER_FOV);
	CGL_shader_set_uniform_float(g_Game.raycastShader, CGL_shader_get_uniform_location(g_Game.raycastShader, "u_dir"), g_Game.direction);
	CGL_shader_set_uniform_float ( g_Game.raycastShader, CGL_shader_get_uniform_location(g_Game.raycastShader, "u_map_size"), (CGL_float)MAP_SIZE);
	CGL_shader_set_uniform_vec2v ( g_Game.raycastShader, CGL_shader_get_uniform_location(g_Game.raycastShader, "u_ppos"), g_Game.position.x, g_Game.position.y);
	CGL_gl_render_screen_quad();

}

// ---------------------------------------------------------------
// ---------------------- MAIN -----------------------------------
// ---------------------------------------------------------------


int main(int argc, char** argv, char** envp)
{
	srand((uint32_t)time(NULL));
	CGL_init();
	g_Window = CGL_window_create(WINDOW_WIDTH, WINDOW_HEIGHT, "CGL Raycaster 3D - Jaysmito Mukherjee");
	if (g_Window == NULL) return false;
	CGL_window_make_context_current(g_Window);
	CGL_gl_init();
	CGL_widgets_init();
	CGL_framebuffer* default_framebuffer = CGL_framebuffer_create_from_default(g_Window);

	if (!setup_map()) return EXIT_FAILURE;
	if (!setup_game()) return EXIT_FAILURE;

	while (!CGL_window_should_close(g_Window))
	{
		CGL_window_set_size(g_Window, WINDOW_WIDTH, WINDOW_HEIGHT);

		static int frame_count = 0;
		frame_count++;
		int wx, wy;
		double mxp, myp;
		CGL_window_get_framebuffer_size(g_Window, &wx, &wy);
		CGL_window_get_mouse_position(g_Window, &mxp, &myp);
		mxp = (mxp / wx);
		myp = (myp / wy);

		static CGL_byte buffer[2048];

		CGL_framebuffer_bind(default_framebuffer);
		CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f);

		render_map_editor(mxp, myp);
		render_game();


		if (CGL_window_is_key_pressed(g_Window, CGL_KEY_ESCAPE)) break;

		CGL_window_swap_buffers(g_Window);
		CGL_window_poll_events(g_Window);
	}

	CGL_shader_destroy(g_Game.raycastShader);
	CGL_framebuffer_destroy(g_Game.gameView);
	CGL_texture_destroy(g_Game.raycastData);
	CGL_texture_destroy(g_TilemapTexture);
	CGL_texture_destroy(g_SkyTexture);	
	CGL_framebuffer_destroy(default_framebuffer);
	CGL_widgets_shutdown();
	CGL_gl_shutdown();
	CGL_window_destroy(g_Window);
	CGL_shutdown();
	return EXIT_SUCCESS;
}
