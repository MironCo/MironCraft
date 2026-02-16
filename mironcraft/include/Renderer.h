#pragma once
#include <vector>
#include <memory>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include "Shader.h"
#include "Player.h"
#include "ShadowMap.h"
#include "Block.h"

class Chunk;

class Renderer
{
private:
	static std::vector<std::unique_ptr<Chunk>> chunksToRender;
	static std::unique_ptr<ShadowMap> shadowMap;
	static std::unique_ptr<Shader> outlineShader;

	// Thread pool for chunk generation
	static std::vector<std::thread> workerThreads;
	static std::queue<Chunk*> workQueue;
	static std::mutex queueMutex;
	static std::condition_variable queueCV;
	static std::atomic<bool> shutdownWorkers;

	// Worker thread function
	static void WorkerThread();

public:
	static void Init();
	static void Shutdown();
	static void AddToRender(std::unique_ptr<Chunk> chunk);
	static void Draw(Shader& shader, Player& player);
	static void DrawShadowPass(Player& player);
	static void DrawOutlinePass(Player& player);

	// Queue a chunk for background generation/mesh building
	static void QueueChunkWork(Chunk* chunk);

	// Process chunks ready for GPU upload (call from main thread)
	static void ProcessReadyChunks();

	// Remove a block at world position and rebuild affected chunk
	static void RemoveBlock(int worldX, int worldY, int worldZ);

	// Add a block at world position and rebuild affected chunk
	static void AddBlock(int worldX, int worldY, int worldZ, BlockType type);

	// Rotate the sun/light
	static void RotateLight(float angle);
};
