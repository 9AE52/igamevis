#include <PassArrays/iGamePassArray.h>
#include <iGameFileIO.h>
#include <iGameInteractor.h>
#include <iGameRenderWindow.h>
#include <iGameScene.h>
#include <string>
#include <vector>

int main() {
    // Create a new scene
    auto scene = iGame::Scene::New();

    // Read the file and add it to the scene
    const std::string fileName = "./Models/kit.vtk";

    iGame::DataObject::Pointer obj = iGame::FileIO::ReadFile(fileName);
    if (!obj) {
        std::cout << "Failed to read file: " << fileName << std::endl;
        return 1;
    }

    std::vector<std::string> stdNames = {"v1", "u1", "w1"};

    auto filter = iGame::PassArray::New();
    filter->SetArrayNames(stdNames);
    filter->SetInput(obj);
    if (!filter->Execute()) {
        std::cout << "Filter ERROR!\n";
        return 0;
    }
    obj = filter->GetOutput();
    scene->AddModel(obj);
    // Set up the render window
    iGame::RenderWindow::Pointer window = iGame::RenderWindow::New();
    window->SetSize(1920, 1080);
    window->SetScene(scene);

    // Set up the interactor
    auto interactor = iGame::Interactor::New();
    interactor->Initialize(scene);
    interactor->CreateDefaultStyle();
    window->SetInteractor(interactor);

    // Start the render loop
    window->Show();
}