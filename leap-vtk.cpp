#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkRenderer.h>
#include <vtkVolumeProperty.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkDICOMImageReader.h>
#include <vtkVolumeRayCastCompositeFunction.h>
#include <vtkVolume.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkCallbackCommand.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkCamera.h>
#include "Leap.h"

#include <iostream>
#include <string.h>
#include <math.h>

using namespace Leap;

int xSizeWindow = 1000;
int ySizeWindow = 1000;

std::string modes[3] = { "Mode1", "Mode2", "Mode3" };
int modeNumber = 0;
bool flashTextBackGround = false;
bool freez = false;

vtkSmartPointer<vtkCamera> camera;
vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;
vtkSmartPointer <vtkTextActor> textActor;
vtkVolume * volume;

Vector originVector;
Vector rotateVec;
Vector center;

// Add class definitions here

void initializeVectors();
void initializeReader(vtkDICOMImageReader *reader);
void setupVolumeProperty(vtkVolumeProperty *volumeProperty, vtkSmartVolumeMapper *mapper, vtkColorTransferFunction *color, vtkPiecewiseFunction *compositeOpacity);
void setupRenderer(vtkRenderer *renderer);
void initializeRenderWindowInteractor();
void initializeController(SampleListener &listener, Controller &controller);
void initializeTextActor(vtkRenderer *renderer);
void startRendering(Controller &controller, SampleListener &listener);

int main(int argc, char *argv[])
{
    initializeVectors();

    auto *reader = vtkDICOMImageReader::New();
    initializeReader(reader);

    auto mapper = vtkSmartPointer<vtkSmartVolumeMapper>::New();
    auto *volumeProperty = vtkVolumeProperty::New();
    auto *compositeOpacity = vtkPiecewiseFunction::New();
    auto *color = vtkColorTransferFunction::New();

    setupVolumeProperty(volumeProperty, mapper, color, compositeOpacity);

    double xyz[3];
    mapper->GetCenter(xyz);

    volume = vtkVolume::New();
    volume->SetMapper(mapper);
    volume->SetProperty(volumeProperty);
    volume->SetOrigin(xyz[0], xyz[1], xyz[2]);
    volume->SetPosition(xyz[0], xyz[1], xyz[2]);

    auto renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->AddViewProp(volume);
    setupRenderer(renderer);

    initializeRenderWindowInteractor();
    auto callback = vtkSmartPointer<vtkTimerCallback>::New();
    auto callbackKeyPress = vtkSmartPointer<vtkKeyPressCallback>::New();
    renderWindowInteractor->AddObserver(vtkCommand::KeyPressEvent, callbackKeyPress);
    renderWindowInteractor->AddObserver(vtkCommand::TimerEvent, callback);
    renderWindowInteractor->CreateRepeatingTimer(10);

    SampleListener listener;
    Controller controller;
    initializeController(listener, controller);

    initializeTextActor(renderer);

    startRendering(controller, listener);

    return EXIT_SUCCESS;
}

void initializeVectors() {
    originVector.x = 0.0;
    originVector.y = 0.0;
    originVector.z = -1.0;

    rotateVec.x = 0.0;
    rotateVec.y = 0.0;
    rotateVec.z = -1.0;
}

void initializeReader(vtkDICOMImageReader *reader) {
    reader->SetDirectoryName("C:/Users/reza/Downloads/Compressed/New folder");
    reader->Update();
}

void setupVolumeProperty(vtkVolumeProperty *volumeProperty, vtkSmartVolumeMapper *mapper, vtkColorTransferFunction *color, vtkPiecewiseFunction *compositeOpacity) {
    mapper->SetBlendModeToComposite();
    mapper->SetInputConnection(reader->GetOutputPort());

    volumeProperty->SetAmbient(0.2);
    volumeProperty->SetDiffuse(0.8);
    volumeProperty->SetSpecular(0.5);
    volumeProperty->SetSpecularPower(5);
    volumeProperty->ShadeOn();
    volumeProperty->SetInterpolationType(VTK_CUBIC_INTERPOLATION);
    volumeProperty->SetInterpolationTypeToLinear();
    volumeProperty->SetScalarOpacityUnitDistance(5.0);

    compositeOpacity->AddPoint(0.0, 0.0);
    compositeOpacity->AddPoint(400, 1.0);
    volumeProperty->SetScalarOpacity(compositeOpacity);

    color->AddRGBPoint(-10, 0.0, 0.0, 0.0);
    color->AddRGBPoint(200, 0.5, 0.5, 0.5);
    color->AddRGBPoint(400, 1.0, 1.0, 1.0);
    volumeProperty->SetColor(color);
}

void setupRenderer(vtkRenderer *renderer) {
    auto renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->SetSize(xSizeWindow, ySizeWindow);
    renderWindow->AddRenderer(renderer);

    camera = vtkSmartPointer<vtkCamera>::New();
    renderer->SetActiveCamera(camera);
    renderer->ResetCamera();
    camera->SetClippingRange(1, 5000);
}

void initializeRenderWindowInteractor() {
    renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindowInteractor->SetInteractorStyle(nullptr);
    renderWindowInteractor->Initialize();
}

void initializeController(SampleListener &listener, Controller &controller) {
    controller.enableGesture(Leap::Gesture::TYPE_KEY_TAP,true);
    controller.addListener(listener);
}

void initializeTextActor(vtkRenderer *renderer) {
    textActor = vtkSmartPointer<vtkTextActor>::New();
    textActor->SetInput(modes[0].c_str());
    textActor->SetPosition(10, 100);
    textActor->GetTextProperty()->SetFontSize(48);
    textActor->GetTextProperty()->SetColor(1.0, 0.0, 0.0);
    renderer->AddActor2D(textActor);
}

void startRendering(Controller &controller, SampleListener &listener) {
    renderWindow->Render();
    renderWindowInteractor->Start();

    controller.removeListener(listener);
}
