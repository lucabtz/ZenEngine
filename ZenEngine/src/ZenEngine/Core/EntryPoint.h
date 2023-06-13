#pragma once

#include "ZenEngine/Core/Log.h"
#include "ZenEngine/Core/Game.h"

#include <filesystem>

std::unique_ptr<ZenEngine::Game> CreateGame(const ZenEngine::RuntimeInfo &inRuntimeInfo);

int main(int argc, char **argv)
{
    ZenEngine::Log::Init();

    ZenEngine::RuntimeInfo runtime{};
    runtime.CmdLine.Count = argc;
    runtime.CmdLine.Args = argv;
    runtime.WorkingDirectory = std::filesystem::current_path();

    std::unique_ptr<ZenEngine::Game> game = CreateGame(runtime);
    game->Init();

    game->Run();

    return 0;
}