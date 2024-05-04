#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <raylib.h>
#include <raymath.h>


#define TILE_WIDTH 16
#define TILE_HEIGHT 16
#define MAX_TEXTURES 1

#define WORLD_WIDTH 30
#define WORLD_HEIGHT 30

typedef enum {
	TEXTURE_TILEMAP = 0
} texture_asset;

typedef enum {
	TILE_TYPE_GRASS = 0,
	TILE_TYPE_FLOWER1,
	TILE_TYPE_FLOWER2,
}tile_type;

typedef struct {
	int x;
	int y;
	int type;
} sTile;

typedef struct {
	int x;
	int y;
} sEntity;

sEntity player;

const int screenWidth = 800;
const int screenHeight = 600;

Texture2D textures[MAX_TEXTURES];

sTile world[WORLD_WIDTH][WORLD_HEIGHT];

Camera2D camera = { 0 };

// pick a random tile type with flower tiles being a much lower chance than a normal grass tile
int PickRandomGrassTile() {
	double r = (double)rand() / RAND_MAX;
	if (r < 0.05) {
		return TILE_TYPE_FLOWER1;
	}
	else if (r < 0.15) {
		return TILE_TYPE_FLOWER2;
	}
	else {
		return TILE_TYPE_GRASS;
	}
	return r;
}

// helper function to draw a tile at a given position
void DrawTile(int pos_x, int pos_y, int tile_x, int tile_y) {
	Rectangle source = { (float)(tile_x * TILE_WIDTH), (float)(tile_y * TILE_HEIGHT), (float)TILE_WIDTH, (float)TILE_HEIGHT };
	Rectangle dest = { (float)(pos_x), (float)(pos_y), TILE_WIDTH, TILE_HEIGHT };
	Vector2 origin = { 0, 0 };
	DrawTexturePro(textures[TEXTURE_TILEMAP], source, dest, origin, 0.0f, WHITE);
}


void GenerateMap() {
	for (int i = 0; i < WORLD_WIDTH; i++)
	{
		for (int j = 0; j < WORLD_HEIGHT; j++)
		{
			world[i][j] = (sTile){
				.x = i,
				.y = j,
				.type = PickRandomGrassTile()
			};
		}
	}
}

void LoadImages() {
	Image image = LoadImage("assets/tiles.png");
	textures[TEXTURE_TILEMAP] = LoadTextureFromImage(image);
	UnloadImage(image);
}

void SpawnPlayer() {

	player = (sEntity){
	.x = TILE_WIDTH * 3,
	.y = TILE_HEIGHT * 3,
	};

	camera.target = (Vector2){ player.x, player.y };
	camera.offset = (Vector2){ (float)screenWidth / 2.0f, (float)screenHeight / 2.0f };
	camera.rotation = 0.0f;
	camera.zoom = 3.0f;
}

void HandleCameraZoom() {
	float wheel = GetMouseWheelMove();
	if (wheel != 0.0f) {
		const float zoomIncrement = 0.125f;
		camera.zoom += wheel * zoomIncrement;

		if (camera.zoom < 3.0f) camera.zoom = 3.0f;
		if (camera.zoom > 8.0f) camera.zoom = 8.0f;
	}
}

void HandlePlayerMovement() {
	float x = player.x;
	float y = player.y;

	if (IsKeyPressed(KEY_W)) {
		y -= TILE_HEIGHT;
	}
	else if (IsKeyPressed(KEY_S)) {
		y += TILE_HEIGHT;
	}
	else if (IsKeyPressed(KEY_A)) {
		x -= TILE_WIDTH;
	}
	else if (IsKeyPressed(KEY_D)) {
		x += TILE_WIDTH;
	}

	player.x = x;
	player.y = y;
	camera.target = (Vector2){ player.x, player.y };
}

void DrawWorld() {
	sTile tile;
	int texture_index_x = 0;
	int texture_index_y = 0;
	for (int i = 0; i < WORLD_WIDTH; i++)
	{
		for (int j = 0; j < WORLD_HEIGHT; j++)
		{
			tile = world[i][j];

			switch (tile.type) {
			case TILE_TYPE_GRASS:

				texture_index_x = 0;
				texture_index_y = 0;
				break;
			case TILE_TYPE_FLOWER1:
				texture_index_x = 1;
				texture_index_y = 0;
				break;
			case TILE_TYPE_FLOWER2:
				texture_index_x = 2;
				texture_index_y = 0;
				break;


			}
			DrawTile(tile.x * TILE_WIDTH, tile.y * TILE_HEIGHT, texture_index_x, texture_index_y);
		}



	}
}

void DrawUI() {
	DrawRectangle(5, 5, 300, 120, Fade(SKYBLUE, 0.5f));
	DrawRectangleLines(5, 5, 300, 120, BLUE);

	DrawText(TextFormat("Camera Target: (%06.2f, %06.2f)", camera.target.x, camera.target.y), 15, 10, 14, YELLOW);
	DrawText(TextFormat("Camera Zoom: %06.2f", camera.zoom), 15, 30, 14, YELLOW);
}

void DrawPlayer() {
	DrawTile(camera.target.x, camera.target.y, 4, 0);
}

void UnloadTextures() {
	for (int i = 0; i < MAX_TEXTURES; i++) {
		UnloadTexture(textures[i]);
	}
}

void GameStartup() {
	InitAudioDevice();
	LoadImages();
	GenerateMap();
	SpawnPlayer();
}

void GameUpdate() {

	HandlePlayerMovement();
	HandleCameraZoom();
}

void GameRender() {

	BeginMode2D(camera);
	DrawWorld();
	DrawPlayer();
	EndMode2D();
	DrawUI();

}

void GameShutdown() {
	UnloadTextures();
	CloseAudioDevice();
}

int main()
{
	InitWindow(screenWidth, screenHeight, "PokeMan");
	SetTargetFPS(60);
	GameStartup();

	while (!WindowShouldClose())
	{
		GameUpdate();
		BeginDrawing();
		ClearBackground(GRAY);
		GameRender();
		EndDrawing();
	}

	GameShutdown();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
