#include "SavePDF3D.h"

#include "PdfIO.h"
#include <fstream>


#include "FusionToTK.h"

Ptr<Application> app;
Ptr<UserInterface> ui;

const std::string commandId = "Save_To_3DPDF";
const std::string commandName = "Save as 3DPDF";
const std::string commandDescription = "Save as 3DPDF (*.pdf).";


// Create the command definition.
static Ptr<CommandDefinition> createCommandDefinition()
{
	Ptr<CommandDefinitions> commandDefinitions = ui->commandDefinitions();
	if (!commandDefinitions)
		return nullptr;

	// Be fault tolerant in case the command is already added.
	Ptr<CommandDefinition> cmDef = commandDefinitions->itemById(commandId);
	if (!cmDef)
	{
		std::string resourcePath = "./resources";
		cmDef = commandDefinitions->addButtonDefinition(commandId,
			commandName,
			commandDescription,
			resourcePath);// absolute resource file path is specified
	}

	return cmDef;
}


// CommandDestroyed event handler
class OnDestroyEventHandler : public adsk::core::CommandEventHandler
{
public:
	void notify(const Ptr<CommandEventArgs>& eventArgs) override
	{
		adsk::terminate();
	}
}_TesseketorCommandDestroy;


struct TKStruct
{
	TesseKetor::Model_tk* model;
	std::string fileName;
};

static DWORD WINAPI MyThreadFunction(void* pContext)
{
	TKStruct *foo = static_cast<TKStruct*>(pContext);
	PdfIO* pdfIo = new PdfIO(foo->model, foo->fileName);
	pdfIo->Export();
	delete pdfIo;
	return true;
}


Camera_tk* GetCameraParam(Ptr<Camera> camera)
{
	double perpAngle = camera->perspectiveAngle();
	int viewOrient = camera->viewOrientation();
	Ptr<Vector3D> upvec = camera->upVector();
	Ptr<Point3D> camPosition = camera->eye();
	Ptr<Point3D> camTarget = camera->target();
	double distCam = camTarget->distanceTo(camPosition);
	upvec->normalize();

	Ptr<Vector3D> viewVec = Vector3D::create(camTarget->x() - camPosition->x(),
		camTarget->y() - camPosition->y(),
		camTarget->z() - camPosition->z());

	viewVec->normalize();
	Ptr<Vector3D> leftVec = viewVec->crossProduct(upvec);
	leftVec->normalize();

	Camera_tk* modelCam = Camera_tk::CreateCamera(leftVec->asArray().data(),
		upvec->asArray().data(),
		viewVec->asArray().data(),
		camPosition->asArray().data(),
		camTarget->asArray().data(),
		distCam);

	return modelCam;

}

void AddOtherViews(Model_tk*& model)
{
	double up[3], left[3], vw[3];
	Ptr<Viewport> vport = app->activeViewport();
	Ptr<Camera> camera = vport->camera();
	Camera_tk* camModel = GetCameraParam(camera);

	double* tar = camModel->GetCameraTarget();
	double dist = camModel->GetDistance();
	//frontView 
	up[0] = 0.0; up[1] = 1.0; up[2] = 0.0;
	left[0] = -1.0; left[1] = 0.0; left[2] = 0.0;
	vw[0] = 0.0; vw[1] = 0.0; vw[2] = -1.0;

	Ptr<Vector3D> posV = Vector3D::create(tar[0], tar[1], tar[2]);
	Ptr<Vector3D> posV1 = Vector3D::create(vw[0], vw[1], vw[2]);
	posV1->scaleBy(dist);
	posV->subtract(posV1);

	camModel->SetUpVec(up);
	camModel->SetLeftVec(left);
	camModel->SetViewVec(vw);
	camModel->SetCameraPosition(posV->asArray().data());
	camModel->SetCameraName("Front View");
	model->AddCamera(camModel);

	//LeftView 
	camModel = GetCameraParam(camera);
	up[0] = 0.0; up[1] = 1.0; up[2] = 0.0;
	left[0] = 0.0; left[1] = 0.0; left[2] = -1.0;
	vw[0] = 1.0; vw[1] = 0.0; vw[2] = 0.0;

	posV = Vector3D::create(tar[0], tar[1], tar[2]);
	posV1 = Vector3D::create(vw[0], vw[1], vw[2]);
	posV1->scaleBy(dist);
	posV->subtract(posV1);

	camModel->SetUpVec(up);
	camModel->SetLeftVec(left);
	camModel->SetViewVec(vw);
	camModel->SetCameraPosition(posV->asArray().data());
	camModel->SetCameraName("Left View");
	model->AddCamera(camModel);

	//Right View 
	camModel = GetCameraParam(camera);
	up[0] = 0.0; up[1] = 1.0; up[2] = 0.0;
	left[0] = 0.0; left[1] = 0.0; left[2] = 1.0;
	vw[0] = -1.0; vw[1] = 0.0; vw[2] = 0.0;

	posV = Vector3D::create(tar[0], tar[1], tar[2]);
	posV1 = Vector3D::create(vw[0], vw[1], vw[2]);
	posV1->scaleBy(dist);
	posV->subtract(posV1);

	camModel->SetUpVec(up);
	camModel->SetLeftVec(left);
	camModel->SetViewVec(vw);
	camModel->SetCameraPosition(posV->asArray().data());
	camModel->SetCameraName("Right View");
	model->AddCamera(camModel);

	//Back View 
	camModel = GetCameraParam(camera);
	up[0] = 0.0; up[1] = 1.0; up[2] = 0.0;
	left[0] = 1.0; left[1] = 0.0; left[2] = 0.0;
	vw[0] = 0.0; vw[1] = 0.0; vw[2] = 1.0;

	posV = Vector3D::create(tar[0], tar[1], tar[2]);
	posV1 = Vector3D::create(vw[0], vw[1], vw[2]);
	posV1->scaleBy(dist);
	posV->subtract(posV1);

	camModel->SetUpVec(up);
	camModel->SetLeftVec(left);
	camModel->SetViewVec(vw);
	camModel->SetCameraPosition(posV->asArray().data());
	camModel->SetCameraName("Back View");
	model->AddCamera(camModel);

	//Top View
	camModel = GetCameraParam(camera);
	up[0] = 0.0; up[1] = 0.0; up[2] = -1.0;
	left[0] = -1.0; left[1] = 0.0; left[2] = 0.0;
	vw[0] = 0.0; vw[1] = -1.0; vw[2] = 0.0;

	posV = Vector3D::create(tar[0], tar[1], tar[2]);
	posV1 = Vector3D::create(vw[0], vw[1], vw[2]);
	posV1->scaleBy(dist);
	posV->subtract(posV1);
	camModel->SetCameraName("Top View");
	camModel->SetUpVec(up);
	camModel->SetLeftVec(left);
	camModel->SetViewVec(vw);
	camModel->SetCameraPosition(posV->asArray().data());	
	model->AddCamera(camModel);

	//Bottom View  
	camModel = GetCameraParam(camera);
	up[0] = 0.0; up[1] = 0.0; up[2] = 1.0;
	left[0] = -1.0; left[1] = 0.0; left[2] = 0.0;
	vw[0] = 0.0; vw[1] = 1.0; vw[2] = 0.0;

	posV = Vector3D::create(tar[0], tar[1], tar[2]);
	posV1 = Vector3D::create(vw[0], vw[1], vw[2]);
	posV1->scaleBy(dist);
	posV->subtract(posV1);

	camModel->SetUpVec(up);
	camModel->SetLeftVec(left);
	camModel->SetViewVec(vw);
	camModel->SetCameraPosition(posV->asArray().data());
	camModel->SetCameraName("Bottom View");
	model->AddCamera(camModel);

}


#include <windows.h>
class GearCommandExecuteEventHandler : public adsk::core::CommandEventHandler
{
public:
	void notify(const Ptr<CommandEventArgs>& eventArgs) override
	{

		Ptr<UserInterface> ui = app->userInterface();

		//// file dialog

		Ptr<FileDialog> fileDlg = ui->createFileDialog();
		fileDlg->filter("3D PDF (*.pdf)\0(.pdf)\0");
		fileDlg->title("Save 3D PDF");
		DialogResults dlgResult = fileDlg->showSave();
		std::string fileNanme = fileDlg->filename();
		if (dlgResult == DialogOK && fileNanme.size()>0)
		{
			Ptr<ProgressDialog> prog = ui->createProgressDialog();

			prog->show("3DPDF Writing file", "Please Wait....", 0, 100);
			
			prog->progressValue(20);
			FusionToTK exporter;
			bool result = exporter.Traverse(app->activeDocument(), prog);
			if (result)
			{
				prog->progressValue(50);

				Ptr<Viewport> vport = app->activeViewport();
				//to fit the model to screen
				vport->fit();

				TKStruct modelData;
				modelData.model = exporter.GetModel();

				Ptr<Camera> camera = vport->camera();
				Camera_tk* camModel = GetCameraParam(camera);
				camModel->SetCameraName("Default");
				modelData.model->AddCamera(camModel);

				AddOtherViews(modelData.model);

				modelData.fileName = fileNanme;
				HANDLE  hThread = CreateThread(NULL, 0, MyThreadFunction, (void*)&modelData, 0L, NULL);
				int t = 0;
				prog->message("Saving...");
				do
				{
					prog->progressValue(t);
					Sleep(100);
					t = (t + 1) % 100;
				} while (WaitForSingleObject(hThread, 0));
				prog->progressValue(95);
				Sleep(100);
			}
			prog->hide();
		}
	}
}_gearCommandExecute;


// CommandCreated event handler.
class CommandCreatedEventHandler : public adsk::core::CommandCreatedEventHandler
{
public:
	void notify(const Ptr<CommandCreatedEventArgs>& eventArgs) override
	{
		if (eventArgs)
		{
			Ptr<Command> command = eventArgs->command();
			if (command)
			{

				Ptr<CommandEvent> executeEvent = command->execute();
				if (!executeEvent)
					return;

				if (!executeEvent->add(&_gearCommandExecute))
					return;

				Ptr<CommandEvent> destroyEvent = command->destroy();
				if (!executeEvent)
					return;

				if (!destroyEvent->add(&_TesseketorCommandDestroy))
					return;
			}
		}
	}

} cmdCreated_;



extern "C" XI_EXPORT bool run(const char* context)
{
	app = Application::get();
	if (!app)
		return false;

	ui = app->userInterface();
	if (!ui)
		return false;
	
	Ptr<CommandDefinition> command = createCommandDefinition();
	if (!command)
		return false;

	Ptr<ToolbarPanel> createPanel = ui->allToolbarPanels()->itemById("SolidScriptsAddinsPanel");//(//->itemById("SolidCreatePanel");
	if (!createPanel)
		return false;

	Ptr<CommandControl> colladabutton = createPanel->controls()->addCommand(command);
	if (!colladabutton)
		return false;
	colladabutton->isPromoted(true);
	Ptr<CommandCreatedEvent> commandCreatedEvent = command->commandCreated();
	if (!commandCreatedEvent)
		return false;
	commandCreatedEvent->add(&cmdCreated_);
	
	return true;
}

extern "C" XI_EXPORT bool stop(const char* context)
{
	if (ui)
	{
		Ptr<ToolbarPanel> createPanel = ui->allToolbarPanels()->itemById("SolidScriptsAddinsPanel");
		if (!createPanel)
			return false;

		Ptr<CommandControl> gearButton1 = createPanel->controls()->itemById(commandId);
		if (gearButton1)
			gearButton1->deleteMe();

		Ptr<CommandDefinition> cmdDef = ui->commandDefinitions()->itemById(commandId);
		if (cmdDef)
			cmdDef->deleteMe();

		ui = nullptr;
	}
	return true;
}


#ifdef XI_WIN



BOOL APIENTRY DllMain(HMODULE hmodule, DWORD reason, LPVOID reserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

#endif // XI_WIN

