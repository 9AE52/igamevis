#include "FeatureExtraction/iGameOutlineCornerFilter.h"
#include "iGameFileIO.h"

#include <exception>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 3) {
        std::cerr << "Usage: testOutlineCorners <input-model> [corner-factor]\n";
        return 1;
    }

    const std::string inputFile = argv[1];
    auto input = iGame::FileIO::ReadFile(inputFile);
    if (input.IsNull()) {
        std::cerr << "Failed to read input model: " << inputFile << '\n';
        return 2;
    }

    auto filter = iGame::OutlineCornerFilter::New();
    filter->SetInput(input);
    if (argc == 3) {
        try {
            filter->SetCornerFactor(std::stof(argv[2]));
        } catch (const std::exception&) {
            std::cerr << "Invalid corner factor: " << argv[2] << '\n';
            return 3;
        }
    }

    if (!filter->Execute()) {
        std::cerr << "OutlineCornerFilter failed: " << filter->GetMessage() << '\n';
        return 4;
    }

    const auto result = filter->GetResult();
    if (result.IsNull()) {
        std::cerr << "OutlineCornerFilter returned no result.\n";
        return 5;
    }

    std::cout << "Input: " << inputFile << '\n';
    std::cout << "Corner factor: " << filter->GetCornerFactor() << '\n';
    std::cout << "Number of points: " << result->GetNumberOfPoints() << '\n';
    std::cout << "Number of line cells: " << result->GetNumberOfCells() << '\n';

    for (IGsize cornerId = 0; cornerId < 8; ++cornerId) {
        const auto& point = result->GetPoint(cornerId * 4);
        std::cout << "corner[" << cornerId << "] = ("
                  << point[0] << ", " << point[1] << ", " << point[2] << ")\n";
    }

    return 0;
}
