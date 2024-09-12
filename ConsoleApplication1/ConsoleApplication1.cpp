#include "raylib.h"
#include "muParser.h"
#include "Marius.hpp"
#include <string>
#include <iostream>
#include <vector>
#include <cmath> // For standard math functions

Vector2 point = { -1, -1 };  // Initialize to -1 to indicate no point selected yet

void InitializeParser(mu::Parser& parser) {
	// Register trigonometric functions
	parser.DefineFun("COS", [](double x) { return std::cos(x); });
	parser.DefineFun("SIN", [](double x) { return std::sin(x); });
	parser.DefineFun("TAN", [](double x) { return std::tan(x); });

	// Register logarithmic and power functions
	parser.DefineFun("LOG", [](double x) { return std::log(x); }); // Natural logarithm
	parser.DefineFun("LOG10", [](double x) { return std::log10(x); }); // Base-10 logarithm
	parser.DefineFun("EXP", [](double x) { return std::exp(x); }); // Exponential function

	// Register square root and absolute value functions
	parser.DefineFun("SQRT", [](double x) { return std::sqrt(x); });
	parser.DefineFun("ABS", [](double x) { return std::abs(x); });

	// Register constants
	parser.DefineConst("PI", 3.14159265358979323846);
	parser.DefineConst("E", 2.71828182845904523536);
}

void UserInput(int& phase, float& amplitude, int& yPosition, bool& updateGraph) {
	if (IsKeyDown(KEY_LEFT)) {
		phase += 5;
		updateGraph = true;
	}
	if (IsKeyDown(KEY_RIGHT)) {
		phase -= 5;
		updateGraph = true;
	}
	if (IsKeyDown(KEY_Z)) {
		amplitude += 1.5f;
		updateGraph = true;
	}
	if (IsKeyDown(KEY_X) && amplitude > 0) {
		amplitude -= 1.5f;
		updateGraph = true;
	}
	if (IsKeyDown(KEY_UP)) {
		yPosition += 2;
		updateGraph = true;
	}
	if (IsKeyDown(KEY_DOWN)) {
		yPosition -= 2;
		updateGraph = true;
	}
	if (IsKeyPressed(KEY_SPACE)) {
		// Set crosshair to the current mouse position
		point = GetMousePosition();
		point.x = static_cast<int>(point.x);
		point.y = static_cast<int>(point.y);
	}
}

void DrawGraphAxis(int yPosition, int numLabels, float amplitude, int phase) {
	// Draw X and Y axis lines (X-axis now centered based on phase)
	DrawLine(0, yPosition, 600, yPosition, WHITE);
	DrawLine(300 - phase, 0, 300 - phase, 600, WHITE);
	DrawText("@a_mariuuus", 100, 100, 10, DARKGRAY);
	// Calculate the spacing for the labels
	float xSpacing = 600.0f / numLabels;
	float ySpacing = 600.0f / numLabels;

	// Draw numbering for X and Y axes, ensuring they remain equidistant
	for (int i = 0; i <= numLabels; i++) {
		float xPos = i * xSpacing;
		float yPos = i * ySpacing;

		// X-axis numbering
		float xValue = (xPos - 300 + phase) / amplitude;
		DrawText(TextFormat("%.01f", xValue), xPos, yPosition + 5, 10, WHITE);

		// Y-axis numbering
		float yValue = (yPosition - yPos) / amplitude;
		if (i != numLabels / 2)  // Avoid overlapping at origin
			DrawText(TextFormat("%.01f", yValue), 305 - phase, yPos - 5, 10, WHITE);
	}
}

void ResetGraph(int phase, float amplitude, int yPosition, mu::Parser& parser, std::vector<Vector2>& graphPoints, std::string& Expr) {
	// Get a new expression from the user
	Expr = GetUserInputAndSetExpression();
	parser.SetExpr(Expr);

	// Clear previous graph points
	graphPoints.clear();

	// Precompute graph values
	for (float x = -600; x < 600; x += 1) {
		double graphValue = 0;
		try {
			float graphX = (x + phase) / amplitude;
			double x1 = static_cast<double>(graphX);
			parser.DefineVar("X", &x1);
			graphValue = parser.Eval() * amplitude;
		}
		catch (const mu::Parser::exception_type& e) {
			std::cerr << "Parser error: " << e.GetMsg() << std::endl;
			graphValue = 0;  // Fallback value
		}

		int xPos = static_cast<int>(300 + x);
		if (xPos >= 0 && xPos < 600) {
			graphPoints.push_back({ static_cast<float>(xPos), static_cast<float>(yPosition - static_cast<int>(graphValue)) });
		}
	}

	// Debug print to verify the reset
	std::cout << "Graph reset with expression: " << Expr << std::endl;
}

void UpdateGraph(int phase, float amplitude, int yPosition, mu::Parser& parser, std::vector<Vector2>& graphPoints, const std::string& Expr) {
	parser.SetExpr(Expr);

	// Clear previous graph point
	graphPoints.clear();

	// Precompute graph values
	for (float x = -600; x < 600; x += 1) {
		double graphValue = 0;
		try {
			float graphX = (x + phase) / amplitude;
			double x1 = static_cast<double>(graphX);
			parser.DefineVar("X", &x1);
			graphValue = parser.Eval() * amplitude;
		}
		catch (const mu::Parser::exception_type& e) {
			std::cerr << "Parser error: " << e.GetMsg() << std::endl;
			graphValue = 0;  // Fallback value
		}

		int xPos = static_cast<int>(300 + x);
		if (xPos >= 0 && xPos < 600) {
			graphPoints.push_back({ static_cast<float>(xPos), static_cast<float>(yPosition - static_cast<int>(graphValue)) });
		}
	}
}

void DrawCrosshair(Vector2 point, int yPosition, int phase) {
	if (point.x != -1) {
		// Draw crosshair at the point
		DrawCircle(static_cast<int>(point.x), static_cast<int>(point.y), 2.0f, RED);

		// Draw horizontal dotted line from y-axis to the crosshair
		if (point.x >= 300 - phase) {
			for (int i = static_cast<int>(300 - phase); i < point.x; i += 2) {
				DrawPixel(i, static_cast<int>(point.y), RED);
			}
		}
		else {
			for (int i = static_cast<int>(300 - phase); i > point.x; i -= 2) {
				DrawPixel(i, static_cast<int>(point.y), RED);
			}
		}

		// Draw vertical dotted line from x-axis to the crosshair
		if (point.y < yPosition) {
			for (int i = static_cast<int>(point.y); i < yPosition; i += 2) {
				DrawPixel(static_cast<int>(point.x), i, RED);
			}
		}
		else {
			for (int i = static_cast<int>(point.y); i > yPosition; i -= 2) {
				DrawPixel(static_cast<int>(point.x), i, RED);
			}
		}
	}
}

int main() {
	InitWindow(600, 600, "Graph with Crosshair");
	SetTargetFPS(60);

	mu::Parser parser;
	InitializeParser(parser);

	std::string Expr = GetUserInputAndSetExpression();
	int phase = 0;
	int yPosition = 300;  // Center y-axis
	float amplitude = 2;
	bool updateGraph = true;

	const int numLabels = 10;  // Number of labels along each axis
	std::vector<Vector2> graphPoints;

	// Initial graph setup
	UpdateGraph(phase, amplitude, yPosition, parser, graphPoints, Expr);

	while (!WindowShouldClose()) {
		// Input handling
		UserInput(phase, amplitude, yPosition, updateGraph);

		// Update graph only if phase, amplitude, or yPosition changed
		if (updateGraph) {
			UpdateGraph(phase, amplitude, yPosition, parser, graphPoints, Expr);
			updateGraph = false;
		}

		// Reset on 'R' key press
		if (IsKeyPressed(KEY_R)) {
			ResetGraph(phase, amplitude, yPosition, parser, graphPoints, Expr);
			updateGraph = false; // Ensure no further updates until input changes
		}

		BeginDrawing();
		ClearBackground(BLACK);

		// Draw the axis with respect to the current phase
		DrawGraphAxis(yPosition, numLabels, amplitude, phase);

		// Draw the graph using precomputed points
		for (size_t i = 1; i < graphPoints.size(); i++) {
			DrawLineV(graphPoints[i - 1], graphPoints[i], WHITE);
		}

		// Draw the crosshairs
		DrawCrosshair(point, yPosition, phase);

		EndDrawing();
	}

	CloseWindow();
	return 0;
}
