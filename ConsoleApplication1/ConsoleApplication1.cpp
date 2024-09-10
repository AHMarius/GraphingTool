#include <raylib.h>
#include <iostream>
#include <vector>
#include <cmath>

struct Vector2 {
	float x, y;
	Vector2(float x = 0, float y = 0) : x(x), y(y) {}
};

struct pixel {
	unsigned char r, g, b, a;
	pixel() : r(0), g(0), b(0), a(255) {}  // Default constructor
};

struct object {
	std::vector<Vector2> vertex;
	Vector2 size;
	Vector2 minVertex;  // Minimum vertex coordinates
	Vector2 maxVertex;  // Maximum vertex coordinates
	pixel** pixelMap;

	template<typename... Args>
	void pushBackVectors(Args... args) {
		(vertex.push_back(args), ...);
	}

	// Find the bounding box size based on vertices
	void FindSize() {
		if (vertex.empty()) {
			std::cerr << "No vertices to calculate size." << std::endl;
			return;
		}

		minVertex = maxVertex = vertex[0];

		for (const auto& v : vertex) {
			if (v.x < minVertex.x) minVertex.x = v.x;
			if (v.y < minVertex.y) minVertex.y = v.y;
			if (v.x > maxVertex.x) maxVertex.x = v.x;
			if (v.y > maxVertex.y) maxVertex.y = v.y;
		}

		size = Vector2{ maxVertex.x - minVertex.x + 1, maxVertex.y - minVertex.y + 1 }; // +1 to include the edge
	}

	// Allocate memory for the pixelMap
	void AllocMat() {
		pixelMap = new(std::nothrow) pixel * [static_cast<int>(size.x)];
		if (!pixelMap) {
			std::cerr << "Memory allocation failed for PixelMap." << std::endl;
			exit(1);
		}

		for (int i = 0; i < size.x; i++) {
			pixelMap[i] = new(std::nothrow) pixel[static_cast<int>(size.y)];
			if (!pixelMap[i]) {
				std::cerr << "Memory allocation failed for PixelMap row." << std::endl;
				for (int k = 0; k < i; ++k) {
					delete[] pixelMap[k];
				}
				delete[] pixelMap;
				exit(1);
			}
		}
	}

	// Free allocated memory for the pixelMap
	void DelMat() {
		if (pixelMap) {
			for (int i = 0; i < size.x; i++) {
				delete[] pixelMap[i];
			}
			delete[] pixelMap;
		}
	}

	// Initialize the object with white pixels within the bounding box defined by vertices
	void CreateObject() {
		if (!pixelMap) {
			std::cerr << "PixelMap not allocated. Call AllocMat() first." << std::endl;
			return;
		}

		// Set all pixels to transparent initially
		for (int i = 0; i < size.x; i++) {
			for (int j = 0; j < size.y; j++) {
				pixelMap[i][j] = pixel{ 0, 0, 0, 0 };  // Fully transparent
			}
		}

		// Draw white pixels within the bounding box
		for (const auto& v : vertex) {
			// Translate vertex position to the pixel map coordinate system
			int x = static_cast<int>(v.x - minVertex.x);
			int y = static_cast<int>(v.y - minVertex.y);

			if (x >= 0 && x < size.x && y >= 0 && y < size.y) {
				pixelMap[x][y] = pixel{ 255, 255, 255, 255 };  // Set to white (opaque)
			}
		}
	}

	// Destructor to free allocated memory
	~object() {
		DelMat();
	}
};

int main() {
	InitWindow(600, 600, "Graph");
	SetTargetFPS(144);

	int phase = 0;
	int yPosition = 300;  // Center y-axis
	float amplitude = 2;
	int func = 0;

	const int numLabels = 10;  // Number of labels along each axis
	object Box;
	Box.pushBackVectors(Vector2{ 0, 0 }, Vector2{ 25, 0 }, Vector2{ 25, 25 }, Vector2{ 0, 25 });
	Box.FindSize();
	Box.AllocMat();
	Box.CreateObject();

	while (!WindowShouldClose()) {
		// Input handling
		if (IsKeyDown(KEY_LEFT)) phase--;
		if (IsKeyDown(KEY_RIGHT)) phase++;
		if (IsKeyDown(KEY_Z)) amplitude += 1.5f;
		if (IsKeyDown(KEY_X) && amplitude > 0) amplitude -= 0.5f;
		if (IsKeyDown(KEY_UP)) yPosition += 2;
		if (IsKeyDown(KEY_DOWN)) yPosition -= 2;
		if (IsKeyPressed(KEY_W) && func < 8) func++;
		if (IsKeyPressed(KEY_S) && func > 0) func--;

		BeginDrawing();
		ClearBackground(BLACK);

		// Draw X and Y axis lines
		DrawLine(0, yPosition, 600, yPosition, WHITE); // X-axis at y = yPosition
		DrawLine(300, 0, 300, 600, WHITE); // Y-axis at x = 300 (middle of window)

		// Calculate the spacing for the labels
		float xSpacing = 600.0f / numLabels;   // Divide the window width by number of labels
		float ySpacing = 600.0f / numLabels;   // Same for the height

		// Draw numbering for X and Y axes, ensuring they remain equidistant
		for (int i = 0; i <= numLabels; i++) {
			float xPos = i * xSpacing;
			float yPos = i * ySpacing;

			// X-axis numbering (scale based on amplitude, move with position)
			float xValue = (xPos - 300) / amplitude; // Center around the middle (300)
			DrawText(TextFormat("%.01f", xValue), xPos, yPosition + 5, 10, WHITE);

			// Y-axis numbering (scale based on amplitude, move with position)
			float yValue = (yPosition - yPos) / amplitude; // Adjust y-values to account for position
			if (i != numLabels / 2)  // Avoid overlapping at origin
				DrawText(TextFormat("%.01f", yValue), 305, yPos - 5, 10, WHITE);
		}

		// Drawing the graph while maintaining position with amplitude changes
		for (float x = -300; x < 300; x += 1 / (10 * amplitude)) {  // Adjust x step with amplitude
			float graphValue = 0;

			// Calculate graph output based on the selected function
			if (func != 8) {
				switch (func) {
				case 7:
					graphValue = amplitude * x;
					break;
				case 6:
					graphValue = amplitude * std::pow(x, 2);
					break;
				case 5:
					graphValue = amplitude * std::pow(2, x);
					break;
				case 4:
					graphValue = amplitude * (1 / std::tan(phase + x / amplitude));
					break;
				case 3:
					graphValue = amplitude * std::tan(phase + x / amplitude);
					break;
				case 2:
					graphValue = amplitude * std::cos(phase + x / amplitude);
					break;
				case 1:
					graphValue = amplitude * std::sin(phase + x / amplitude);
					break;
				case 0:
					graphValue = 0; // Flat line at y = yPosition
					break;
				}

				// Draw the pixel for the current x value with the calculated graph value
				DrawPixel(300 + static_cast<int>(x * amplitude), yPosition - static_cast<int>(graphValue), WHITE);  // Scale x and graphValue consistently
			}
			else {
				amplitude = 1 / 10;
				for (int y = -300; y < 300; y++) {
					// Ensure coordinates are within bounds
					int px = static_cast<int>(x + 300);
					int py = static_cast<int>(y + 300);
					if (px >= 0 && px < static_cast<int>(Box.size.x) && py >= 0 && py < static_cast<int>(Box.size.y)) {
						Color col = Color{ Box.pixelMap[px][py].r, Box.pixelMap[px][py].g, Box.pixelMap[px][py].b, Box.pixelMap[px][py].a };
						DrawPixel(300 + x, yPosition - y, col);
					}
				}
			}
		}

		EndDrawing();
		DrawText("@a_mariuuus", 100, 100, 10, GRAY);
	}

	CloseWindow();
	return 0;
}
