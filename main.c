#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <raylib.h>
#include <raymath.h>

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 1000

#define MAX_FEATURES 40
#define MAX_VELOCITY 0.5f
#define MAX_POSITION 30.0f
#define ALPHA 2.0f

#define MAX_NEGATIVE_CONSTRAINT 4
#define MAX_POSITIVE_CONSTRAINT 10

#define MAX_ITERATIONS 100

#define MEMORY_V3_WEIGHT 0.05
/* #define BEST_V3_WEIGHT 0.5
#define INERTIA_V3_WEIGHT 0.2
#define CENTER_V3_WEIGHT 3 */

typedef struct Feature
{
	Vector3 position;
	Vector3 velocity;
	float score;
} Feature;

typedef struct Constraint
{
	Vector3 position;
} Constraint;

float random_float(float min, float max)
{
	return min + (float)rand() / (float)RAND_MAX * (max - min);
}

void randomize_feature(Feature *feature)
{
	(void)random_float(0.0f, 1.0f);
	*feature = (Feature){
		.position = (Vector3){random_float(-MAX_POSITION * 2.f, MAX_POSITION * 2.f), random_float(-MAX_POSITION * 2.f, MAX_POSITION * 2.f), random_float(-MAX_POSITION * 2.f, MAX_POSITION * 2.f)},
		.velocity = (Vector3){random_float(-MAX_VELOCITY, MAX_VELOCITY), random_float(-MAX_VELOCITY, MAX_VELOCITY), random_float(-MAX_VELOCITY, MAX_VELOCITY)},
		.score = 0.0f,
	};
}

Vector3 randomize_vector()
{
	return (Vector3){random_float(-MAX_POSITION, MAX_POSITION), random_float(-MAX_POSITION, MAX_POSITION), random_float(-MAX_POSITION, MAX_POSITION)};
}

void print_feature(Feature *feature)
{
	printf("Position: (%f, %f, %f)\n", feature->position.x, feature->position.y, feature->position.z);
	printf("Velocity: (%f, %f, %f)\n", feature->velocity.x, feature->velocity.y, feature->velocity.z);
	printf("Score: %f\n", feature->score);
}

void randomize_constraint(Constraint *constraint)
{
	(void)random_float(0.0f, 1.0f);
	*constraint = (Constraint){
		.position = (Vector3){random_float(-MAX_POSITION, MAX_POSITION), random_float(-MAX_POSITION, MAX_POSITION), random_float(-MAX_POSITION, MAX_POSITION)},
	};
}

void print_constraint(Constraint *constraint)
{
	printf("Position: (%f, %f, %f)\n", constraint->position.x, constraint->position.y, constraint->position.z);
}

float distance(Vector3 a, Vector3 b)
{
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	float dz = a.z - b.z;
	return sqrt(dx * dx + dy * dy + dz * dz);
}

Vector3 sumVector3(Vector3 a, Vector3 b)
{
	return (Vector3){a.x + b.x, a.y + b.y, a.z + b.z};
}

Vector3 subVector3(Vector3 a, Vector3 b)
{
	return (Vector3){a.x - b.x, a.y - b.y, a.z - b.z};
}

Vector3 multVector3(Vector3 a, float b)
{
	return (Vector3){a.x * b, a.y * b, a.z * b};
}

float max_distance(Feature *feature, Constraint *constraints, size_t num_constraints)
{
	float max_dist = 0.0f;

	for (size_t i = 0; i < num_constraints; i++)
	{
		float dist = distance(feature->position, constraints[i].position);
		if (dist > max_dist)
		{
			max_dist = dist;
		}
	}

	return max_dist;
}

float min_distance(Feature *feature, Constraint *constraints, size_t num_constraints)
{
	float min_dist = INFINITY;

	for (size_t i = 0; i < num_constraints; i++)
	{
		float dist = distance(feature->position, constraints[i].position);
		if (dist < min_dist)
		{
			min_dist = dist;
		}
	}

	return min_dist;
}

float objective(Feature *feature, Constraint *pc, Constraint *nc)
{
	double positive_distances = 0.0;
	double negative_distances = 0.0;

	for (int i = 0; i < MAX_POSITIVE_CONSTRAINT; i++)
	{
		positive_distances += distance(feature->position, pc[i].position);
	}

	for (int i = 0; i < MAX_NEGATIVE_CONSTRAINT; i++)
	{
		negative_distances += distance(feature->position, nc[i].position);
	}

	return negative_distances - positive_distances;
}

void print_vector3(Vector3 v)
{
	printf("(%f, %f, %f)\n", v.x, v.y, v.z);
}

bool check_bound(Feature *feature)
{
	return (feature->position.x < -MAX_POSITION || feature->position.x > MAX_POSITION ||
			feature->position.y < -MAX_POSITION || feature->position.y > MAX_POSITION ||
			feature->position.z < -MAX_POSITION || feature->position.z > MAX_POSITION);
}

int main(void)
{
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Feature Optimization");

	srand(time(NULL));

	Feature features[MAX_FEATURES];
	Constraint pc[MAX_POSITIVE_CONSTRAINT];
	Constraint nc[MAX_NEGATIVE_CONSTRAINT];

	for (int i = 0; i < MAX_FEATURES; i++)
	{
		randomize_feature(&features[i]);
	}
	for (size_t i = 0; i < MAX_POSITIVE_CONSTRAINT; i++)
	{
		randomize_constraint(&pc[i]);
	}
	for (size_t i = 0; i < MAX_NEGATIVE_CONSTRAINT; i++)
	{
		randomize_constraint(&nc[i]);
	}

	// Feature best_feature = {.position = Vector3Zero(), .velocity = Vector3Zero(), .score = -INFINITY};
	Feature best_feature = {.position = randomize_vector(), .velocity = Vector3Zero(), .score = -INFINITY};
	float prev_score;

	Camera3D camera = {0};
	camera.position = (Vector3){-100.0f, 50.0f, -100.0f};
	camera.target = (Vector3){0.0f, 0.0f, 0.0f};
	camera.up = (Vector3){0.0f, 1.0f, 0.0f};
	camera.fovy = 70.0f;
	camera.projection = CAMERA_PERSPECTIVE;

	while (!WindowShouldClose())
	{
		prev_score = best_feature.score;
		float dt = GetFrameTime();
		float angle = 0.7f * dt;
		camera.position.x = cosf(angle) * camera.position.x - sinf(angle) * camera.position.z;
		camera.position.z = sinf(angle) * camera.position.x + cosf(angle) * camera.position.z;

		if (IsKeyPressed(KEY_SPACE))
		{
			for (size_t i = 0; i < MAX_FEATURES; i++)
			{
				Vector3 w = multVector3(features[i].velocity, MEMORY_V3_WEIGHT);
				Vector3 diff = subVector3(best_feature.position, features[i].position);
				Vector3 gbest = multVector3(diff, random_float(0, 1));
				Vector3 vel = sumVector3(w, gbest);

				features[i].velocity = vel;
				features[i].position = sumVector3(features[i].position, features[i].velocity);
				// float min = min_distance(&features[i], nc, MAX_NEGATIVE_CONSTRAINT);
				// float max = max_distance(&features[i], pc, MAX_POSITIVE_CONSTRAINT);
				//// printf("%f - %f\n", min, max);
				// features[i].score = min - max;
				features[i].score = objective(&features[i], pc, nc);
				if (features[i].score > best_feature.score)
				{
					best_feature = features[i];
				}
			}
			if (best_feature.score == prev_score)
			{
				print_vector3(best_feature.position);
				printf("Score: %f\n", best_feature.score);
			}
		}

		if (IsKeyPressed(KEY_R))
		{
			best_feature.position = Vector3Zero();
			best_feature.velocity = Vector3Zero();
			best_feature.score = -INFINITY;
			for (int i = 0; i < MAX_FEATURES; i++)
			{
				randomize_feature(&features[i]);
			}

			for (size_t i = 0; i < MAX_POSITIVE_CONSTRAINT; i++)
			{
				randomize_constraint(&pc[i]);
			}

			for (size_t i = 0; i < MAX_NEGATIVE_CONSTRAINT; i++)
			{
				randomize_constraint(&nc[i]);
			}
		}

		BeginDrawing();
		ClearBackground(GetColor(0x181818FF));
		BeginMode3D(camera);

		DrawSphere((Vector3){0, 0, 0}, 1, GetColor(0xFFFFFFFF));
		DrawGrid(200, 10.0f);
		for (size_t i = 0; i < MAX_FEATURES; i++)
		{
			DrawSphere(features[i].position, 1, GetColor(0x0000FFFF));
		}
		for (size_t i = 0; i < MAX_POSITIVE_CONSTRAINT; i++)
		{
			DrawSphere(pc[i].position, 1, GetColor(0x00FF00FF));
		}
		for (size_t i = 0; i < MAX_NEGATIVE_CONSTRAINT; i++)
		{
			DrawSphere(nc[i].position, 1, GetColor(0xFF0000FF));
		}
		DrawSphere(best_feature.position, 1, GetColor(0xFF00FFFF));

		EndMode3D();
		EndDrawing();
	}
	return 0;
}