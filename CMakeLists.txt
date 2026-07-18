cmake_minimum_required(VERSION 3.16)
project(knurl LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(knurl
    app/main.cpp
    app/CliOptions.cpp
    app/Engine.cpp
    scanning/FileWalker.cpp
    scanning/PyImportExtractor.cpp
    scanning/LocalBindingExtractor.cpp
    scanning/UsageIndexer.cpp
    graph/ModuleResolver.cpp
    graph/DependencyGraph.cpp
    graph/CycleDetector.cpp
    graph/ImpactAnalyzer.cpp
    graph/UsageAnalyzer.cpp
    graph/EdgeTierClassifier.cpp
    graph/GraphTierAnnotator.cpp
    graph/RiskRanker.cpp
)
target_include_directories(knurl PRIVATE ${CMAKE_SOURCE_DIR})
