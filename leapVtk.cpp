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

class SampleListener : public Listener 
{
public:
	
	virtual void onFrame(const Controller& controller)
	{
		const auto frame = controller.frame();
		const auto hand1 = frame.hands()[0];
		const auto hand2 = frame.hands()[1];
		if (hand1.isValid())
		{
			if (hand2.isValid())
				freez = true;
			else
				freez = false;
			rotateVec = hand1.palmNormal();
			center = hand1.palmPosition();
			auto gestures = frame.gestures();
			for (auto gl = gestures.begin(); gl != gestures.end(); gl++)
			if ((*gl).isValid())
			{
				modeNumber == 2 ? modeNumber = 0 : modeNumber++;
				flashTextBackGround = true;
			}
		}
	}
};

class vtkTimerCallback : public vtkCommand
{
public:
	static vtkTimerCallback *New()
	{
		auto cb = new vtkTimerCallback;
		return cb;
	}

	virtual void Execute(vtkObject * caller, unsigned long eventId, void *vtkNotUsed(callData))
	{
		auto interactor = vtkRenderWindowInteractor::SafeDownCast(caller);

		if (!freez)
		{
			volume->RotateX((rotateVec.x - originVector.x) * 200);
			volume->RotateY((rotateVec.y - originVector.y) * 200);
			volume->RotateX((rotateVec.z - originVector.z) * 200);

			originVector = rotateVec;

			double x, y, z;
			camera->GetPosition(x, y, z);
			camera->SetPosition(x, y, center.z * 10 + 700);
		}
		
		textActor->SetInput(modes[modeNumber].c_str());

		textActor->GetTextProperty()->SetColor(1.0, 0.0, 0.0);
		if(flashTextBackGround)
			textActor->GetTextProperty()->SetColor(1.0, 1.0, 1.0);
		else
			textActor->GetTextProperty()->SetColor(1.0, 0.0, 0.0);
		flashTextBackGround = false;

		interactor->Render();
	}
};

class vtkKeyPressCallback : public vtkCallbackCommand
{
public:
	static vtkKeyPressCallback *New()
	{
		auto cb = new vtkKeyPressCallback;
		return cb;
	}

	virtual void Execute(vtkObject * caller, unsigned long eventId, void *vtkNotUsed(callData))
	{
		auto interactor = vtkRenderWindowInteractor::SafeDownCast(caller);
		auto ch = std::string(interactor->GetKeySym());
		if (ch == std::string("o") || ch == std::string("O"))
		{
			exit(0);
		}

		interactor->Render();
	}
};

int main(int argc, char *argv[])
{
	originVector.x = 0.0;
	originVector.y = 0.0;
	originVector.z = -1.0;

	rotateVec.x = 0.0;
	rotateVec.y = 0.0;
	rotateVec.z = -1.0;

	auto *reader = vtkDICOMImageReader::New();
	reader->SetDirectoryName("C:/Users/reza/Downloads/Compressed/New folder");
	reader->Update();
	
	auto mapper = vtkSmartPointer<vtkSmartVolumeMapper>::New();
	mapper->SetBlendModeToComposite();
	mapper->SetInputConnection(reader->GetOutputPort());

	auto *volumeProperty = vtkVolumeProperty::New();
	volumeProperty->SetAmbient(0.2);
	volumeProperty->SetDiffuse(0.8);
	volumeProperty->SetSpecular(0.5);
	volumeProperty->SetSpecularPower(5);
	volumeProperty->ShadeOn();
	volumeProperty->SetInterpolationType(VTK_CUBIC_INTERPOLATION);
	volumeProperty->SetInterpolationTypeToLinear();
	volumeProperty->SetScalarOpacityUnitDistance(5.0);

	auto *compositeOpacity = vtkPiecewiseFunction::New();
	compositeOpacity->AddPoint(0.0, 0.0);
	compositeOpacity->AddPoint(400, 1.0);
	volumeProperty->SetScalarOpacity(compositeOpacity);
	
	auto *color = vtkColorTransferFunction::New();
	color->AddRGBPoint(-10, 0.0, 0.0, 0.0);
	color->AddRGBPoint(200, 0.5, 0.5, 0.5);
	color->AddRGBPoint(400, 1.0, 1.0, 1.0);
	volumeProperty->SetColor(color);

	double xyz[3];
	mapper->GetCenter(xyz);

	volume = vtkVolume::New();
	volume->SetMapper(mapper);
	volume->SetProperty(volumeProperty);
	volume->SetOrigin(xyz[0], xyz[1], xyz[2]);
	volume->SetPosition(xyz[0], xyz[1], xyz[2]);
	
	auto renderer = vtkSmartPointer<vtkRenderer>::New();
	renderer->AddViewProp(volume);

	auto renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->SetSize(xSizeWindow, ySizeWindow);
	renderWindow->AddRenderer(renderer);
	
	camera = vtkSmartPointer<vtkCamera>::New();
	renderer->SetActiveCamera(camera);
	renderer->ResetCamera();
	camera->SetClippingRange(1, 5000);

	renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);
	renderWindowInteractor->SetInteractorStyle(nullptr);
	renderWindowInteractor->Initialize();
	auto callback = vtkSmartPointer<vtkTimerCallback>::New();
	auto callbackKeyPress = vtkSmartPointer<vtkKeyPressCallback>::New();

	renderWindowInteractor->AddObserver(vtkCommand::KeyPressEvent, callbackKeyPress);
	renderWindowInteractor->AddObserver(vtkCommand::TimerEvent, callback);
	renderWindowInteractor->CreateRepeatingTimer(10);

	SampleListener listener;
	Controller controller;
	controller.enableGesture(Leap::Gesture::TYPE_KEY_TAP,true);
	controller.addListener(listener);
	
	textActor = vtkSmartPointer<vtkTextActor>::New();
	textActor->SetInput(modes[0].c_str());
	textActor->SetPosition(10, 100);
	textActor->GetTextProperty()->SetFontSize(48);
	textActor->GetTextProperty()->SetColor(1.0, 0.0, 0.0);
	renderer->AddActor2D(textActor);
	
	renderWindow->Render();
	renderWindowInteractor->Start();

	controller.removeListener(listener);

	return EXIT_SUCCESS;
}