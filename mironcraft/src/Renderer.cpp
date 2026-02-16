#include "Renderer.h"
#include "Chunk.h"
#include "Common.h"
#include "Collision.h"
#include <GLM/gtc/type_ptr.hpp>

std::vector<std::unique_ptr<Chunk>> Renderer::chunksToRender;
std::unique_ptr<ShadowMap> Renderer::shadowMap;
std::unique_ptr<Shader> Renderer::outlineShader;

// Thread pool static members
std::vector<std::thread> Renderer::workerThreads;
std::queue<Chunk*> Renderer::workQueue;
std::mutex Renderer::queueMutex;
std::condition_variable Renderer::queueCV;
std::atomic<bool> Renderer::shutdownWorkers{false};

void Renderer::WorkerThread()
{
	while (true)
	{
		Chunk* chunk = nullptr;

		{
			std::unique_lock<std::mutex> lock(queueMutex);
			queueCV.wait(lock, [] { return !workQueue.empty() || shutdownWorkers.load(); });

			if (shutdownWorkers.load() && workQueue.empty())
				return;

			chunk = workQueue.front();
			workQueue.pop();
		}

		if (chunk)
		{
			ChunkState currentState = chunk->GetState();

			if (currentState == ChunkState::Generating)
			{
				// Generate block data
				chunk->Generate();
				// Set state to BuildingMesh and queue (atomically set before queue)
				chunk->SetState(ChunkState::BuildingMesh);
				QueueChunkWork(chunk);
			}
			else if (currentState == ChunkState::BuildingMesh)
			{
				// Build mesh on worker thread (no OpenGL!)
				chunk->BuildMeshData();
				chunk->SetState(ChunkState::ReadyForUpload);
			}
		}
	}
}

void Renderer::Init()
{
	shadowMap = std::make_unique<ShadowMap>(2048, 2048);
	outlineShader = std::make_unique<Shader>("res/shaders/outline.vert", "res/shaders/outline.frag");

	// Start worker threads (use hardware concurrency - 1, min 1)
	unsigned int numThreads = std::max(1u, std::thread::hardware_concurrency() - 1);
	for (unsigned int i = 0; i < numThreads; i++)
	{
		workerThreads.emplace_back(WorkerThread);
	}
}

void Renderer::Shutdown()
{
	// Signal workers to stop
	shutdownWorkers.store(true);
	queueCV.notify_all();

	// Wait for all workers to finish
	for (auto& thread : workerThreads)
	{
		if (thread.joinable())
			thread.join();
	}
	workerThreads.clear();
}

void Renderer::QueueChunkWork(Chunk* chunk)
{
	{
		std::lock_guard<std::mutex> lock(queueMutex);
		workQueue.push(chunk);
	}
	queueCV.notify_one();
}

void Renderer::ProcessReadyChunks()
{
	// Process chunks that are ready for GPU upload (main thread only)
	for (auto& chunk : chunksToRender)
	{
		if (chunk->GetState() == ChunkState::ReadyForUpload)
		{
			chunk->UploadMeshToGPU();
			chunk->ApplyCollision();
			chunk->SetState(ChunkState::Ready);
			chunk->isLoaded = true;
		}
	}
}

void Renderer::AddToRender(std::unique_ptr<Chunk> chunk)
{
	chunksToRender.push_back(std::move(chunk));
}

void Renderer::DrawShadowPass(Player& player)
{
	if (!shadowMap) return;

	// Update light matrix centered on player
	shadowMap->UpdateLightSpaceMatrix(player.position, 48.0f);

	// Bind shadow FBO
	shadowMap->BindForWriting();

	// Render all chunks with shadow shader
	shadowMap->shadowShader.Activate();
	for (auto& chunk : chunksToRender)
	{
		if (chunk->isLoaded)
		{
			chunk->DrawChunk(shadowMap->shadowShader);
		}
	}

	// Restore default framebuffer and viewport
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, WindowData::framebufferWidth, WindowData::framebufferHeight);
}

void Renderer::DrawOutlinePass(Player& player)
{
	if (!outlineShader) return;

	// Enable culling just for outline pass - render front faces in black (slightly expanded)
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	outlineShader->Activate();

	// Set camera matrix
	player.Matrix(60.0f, 0.15f, 100.0f, *outlineShader, "cameraMatrix");

	for (auto& chunk : chunksToRender)
	{
		if (chunk->isLoaded)
		{
			chunk->DrawChunk(*outlineShader);
		}
	}

	// Disable culling for main pass
	glDisable(GL_CULL_FACE);
}

void Renderer::Draw(Shader& shader, Player& player)
{
	// First do shadow pass
	DrawShadowPass(player);

	// Queue chunks for background generation/mesh building
	for (auto& chunk : chunksToRender)
	{
		// Check if chunk needs to start loading
		if (chunk->ShouldStartLoading(player))
		{
			chunk->SetState(ChunkState::Generating);
			QueueChunkWork(chunk.get());
		}
		// Note: mesh building is queued by worker after generation completes

		// Update loaded state based on distance
		chunk->CheckDistanceToPlayer(player, shader);
	}

	// Process chunks ready for GPU upload (main thread)
	ProcessReadyChunks();

	// Draw outlines first (back faces, black)
	DrawOutlinePass(player);

	// Set shadow uniforms on main shader
	shader.Activate();

	// Ensure texture unit 0 is set for the block texture
	glUniform1i(glGetUniformLocation(shader.shaderProgramID, "tex0"), 0);

	// Pass camera position for specular lighting
	glUniform3fv(
		glGetUniformLocation(shader.shaderProgramID, "viewPos"),
		1, glm::value_ptr(player.position)
	);

	if (shadowMap)
	{
		glUniformMatrix4fv(
			glGetUniformLocation(shader.shaderProgramID, "lightSpaceMatrix"),
			1, GL_FALSE, glm::value_ptr(shadowMap->lightSpaceMatrix)
		);
		glUniform3fv(
			glGetUniformLocation(shader.shaderProgramID, "lightDir"),
			1, glm::value_ptr(shadowMap->lightDir)
		);

		// Bind shadow map to texture unit 1
		shadowMap->BindForReading(GL_TEXTURE1);
		glUniform1i(glGetUniformLocation(shader.shaderProgramID, "shadowMap"), 1);
	}

	// Make sure we're back on texture unit 0 for the block textures
	glActiveTexture(GL_TEXTURE0);

	// Draw opaque chunks first
	for (auto& chunk : chunksToRender)
	{
		if (chunk->isLoaded)
		{
			chunk->DrawChunk(shader);
		}
	}

	// Draw water with transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE); // Don't write to depth buffer for transparent objects

	glUniform1i(glGetUniformLocation(shader.shaderProgramID, "isWater"), 1);

	for (auto& chunk : chunksToRender)
	{
		if (chunk->isLoaded)
		{
			chunk->DrawWater(shader);
		}
	}

	glUniform1i(glGetUniformLocation(shader.shaderProgramID, "isWater"), 0);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}

void Renderer::RemoveBlock(int worldX, int worldY, int worldZ)
{
	// Remove from collision world
	g_CollisionWorld.RemoveBlock(worldX, worldY, worldZ);

	// Find the chunk containing this block and update its mesh
	for (auto& chunk : chunksToRender)
	{
		if (chunk->isLoaded && chunk->RemoveBlockAtWorld(worldX, worldY, worldZ))
		{
			chunk->RebuildMesh();
			break;
		}
	}
}

void Renderer::AddBlock(int worldX, int worldY, int worldZ, BlockType type)
{
	// Add to collision world
	g_CollisionWorld.AddBlock(worldX, worldY, worldZ);

	// Find the chunk containing this block and update its mesh
	for (auto& chunk : chunksToRender)
	{
		if (chunk->isLoaded && chunk->AddBlockAtWorld(worldX, worldY, worldZ, type))
		{
			chunk->RebuildMesh();
			break;
		}
	}
}

void Renderer::RotateLight(float angle)
{
	if (shadowMap)
	{
		shadowMap->RotateLight(angle);
	}
}
