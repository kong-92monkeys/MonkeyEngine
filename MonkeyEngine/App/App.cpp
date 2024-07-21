
// App.cpp : Defines the class behaviors for the application.
//

#include "pch.h"
#include "framework.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "App.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CApp

BEGIN_MESSAGE_MAP(CApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CApp::OnAppAbout)
END_MESSAGE_MAP()

import ntmonkeys.com.Lib.Logger;
import ntmonkeys.com.Lib.GenericBuffer;
import ntmonkeys.com.Lib.Bitmap;
import ntmonkeys.com.Engine.Mesh;
import ntmonkeys.com.Engine.DrawParam;
import ntmonkeys.com.Engine.Material;
import ntmonkeys.com.Engine.Texture;
import ntmonkeys.com.Frameworks.Vertex;
import ntmonkeys.com.Frameworks.SimpleRenderer;
import ntmonkeys.com.Frameworks.SimpleMaterial;

// CApp construction

CApp::CApp() noexcept
{

	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("NTmonkeys.MonkeyEngine"));

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CApp object

CApp theApp;


// CApp initialization

BOOL CApp::InitInstance()
{
	CWinApp::InitInstance();


	EnableTaskbarInteraction(FALSE);

	// AfxInitRichEdit2() is required to use RichEdit control
	// AfxInitRichEdit2();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("MonkeyEngine"));

	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object
	CFrameWnd* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;

	__onInitBeforeMainFrame();

	// create and load the frame with its resources
	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, nullptr,
		nullptr);

	// The one and only window has been initialized, so show and update it
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();

	return TRUE;
}

int CApp::ExitInstance()
{
	//TODO: handle additional resources you may have added
	__pRenderObject = nullptr;
	__pLayer = nullptr;

	__pRenderingEngine = nullptr;
	__pCore = nullptr;
	__pAssetManager = nullptr;

	return CWinApp::ExitInstance();
}

void CApp::addRenderTarget(Engine::RenderTarget &renderTarget) noexcept
{
	renderTarget.addLayer(__pLayer);
}

void CApp::removeRenderTarget(Engine::RenderTarget &renderTarget) noexcept
{
	renderTarget.removeLayer(__pLayer);
}

void CApp::__onInitBeforeMainFrame()
{
	__pAssetManager = std::make_unique<Lib::AssetManager>();
	__pAssetManager->setRootPath("Assets");

	__createGraphicsCore();
	__createRenderingEngine();
	__createRenderObject();

	__pLayer = std::shared_ptr<Engine::Layer>{ __pRenderingEngine->createLayer() };
	__pLayer->addRenderObject(__pRenderObject);

	const auto imageData{ __pAssetManager->readBinary("Images/wall.jpg") };
	const Lib::Bitmap bitmap{ imageData.data(), imageData.size(), 4ULL };

	const VkExtent3D vkExtent
	{
		.width		{ static_cast<uint32_t>(bitmap.getWidth()) },
		.height		{ static_cast<uint32_t>(bitmap.getHeight()) },
		.depth		{ 1U },
	};

	const std::unique_ptr<Engine::Texture> pTexture
	{
		__pRenderingEngine->createTexture(
			VkImageType::VK_IMAGE_TYPE_2D,
			VkFormat::VK_FORMAT_R8G8B8A8_SRGB,
			vkExtent, false)
	};
}

void CApp::__createGraphicsCore()
{
	try
	{
		const Engine::Core::CreateInfo createInfo
		{
			.vulkanLoaderLibName	{ "vulkan_loader_dedicated-1.dll" },
			.appName				{ "MonkeyEngineDemo" },
			.appVersion				{ 0U, 0U, 1U, 0U },
			.engineName				{ "MonkeyEngine" },
			.engineVersion			{ 0U, 0U, 1U, 0U }
		};

		__pCore = std::make_unique<Engine::Core>(createInfo);
	}
	catch (const std::runtime_error &e)
	{
		Lib::Logger::log(
			Lib::Logger::Severity::FATAL,
			std::format("Error occurred while creating graphics core: {}", e.what()));

		return;
	}

	Lib::Logger::log(Lib::Logger::Severity::INFO, "Graphics core created.");
}

void CApp::__createRenderingEngine()
{
	try
	{
		__pRenderingEngine = std::unique_ptr<Engine::RenderingEngine>{ __pCore->createEngine(*__pAssetManager) };
	}
	catch (const std::runtime_error &e)
	{
		Lib::Logger::log(
			Lib::Logger::Severity::FATAL,
			std::format("Error occurred while creating graphics engine: {}", e.what()));

		return;
	}

	Lib::Logger::log(Lib::Logger::Severity::INFO, "Graphics engine created.");
}

void CApp::__createRenderObject()
{
	const auto pRenderer{ std::shared_ptr<Frameworks::SimpleRenderer>{ __pRenderingEngine->createRenderer<Frameworks::SimpleRenderer>() } };

	Lib::GenericBuffer posBuffer;
	posBuffer.typedAdd<glm::vec3>({ -0.5f, -0.5f, 0.5f });
	posBuffer.typedAdd<glm::vec3>({ -0.5f, 0.5f, 0.5f });
	posBuffer.typedAdd<glm::vec3>({ 0.5f, 0.5f, 0.5f });
	posBuffer.typedAdd<glm::vec3>({ 0.5f, -0.5f, 0.5f });

	Lib::GenericBuffer colorBuffer;
	colorBuffer.typedAdd<glm::vec4>({ 1.0f, 0.0f, 0.0f, 1.0f });
	colorBuffer.typedAdd<glm::vec4>({ 0.0f, 1.0f, 0.0f, 1.0f });
	colorBuffer.typedAdd<glm::vec4>({ 0.0f, 0.0f, 1.0f, 1.0f });
	colorBuffer.typedAdd<glm::vec4>({ 1.0f, 0.0f, 1.0f, 1.0f });

	Lib::GenericBuffer indexBuffer;
	indexBuffer.typedAdd<uint16_t>({ 0U, 1U, 2U, 0U, 2U, 3U });

	const auto pMesh{ std::shared_ptr<Engine::Mesh>{ __pRenderingEngine->createMesh() } };
	pMesh->createVertexBuffer(Frameworks::VertexAttrib::POS_LOCATION, posBuffer.getData(), posBuffer.getSize());
	pMesh->createVertexBuffer(Frameworks::VertexAttrib::COLOR_LOCATION, colorBuffer.getData(), colorBuffer.getSize());
	pMesh->createIndexBuffer(VkIndexType::VK_INDEX_TYPE_UINT16, indexBuffer.getData(), indexBuffer.getSize());

	const auto pDrawParam{ std::make_shared<Engine::DrawParamIndexed>(6U, 0U, 0) };

	const auto pMaterial{ std::make_shared<Frameworks::SimpleMaterial>() };
	pMaterial->setColor({ 1.0f, 0.0f, 1.0f, 1.0f });

	__pRenderObject = std::shared_ptr<Engine::RenderObject>{ __pRenderingEngine->createRenderObject() };

	__pRenderObject->setRenderer(pRenderer);
	__pRenderObject->setMesh(pMesh);
	__pRenderObject->setDrawParam(pDrawParam);

	auto &materialPack{ __pRenderObject->getMaterialPack(0U) };
	materialPack.setMaterial(pMaterial);
}

// CApp message handlers


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg() noexcept;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() noexcept : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// App command to run the dialog
void CApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CApp message handlers


BOOL CApp::OnIdle(LONG lCount)
{
	// TODO: Add your specialized code here and/or call the base class
	__idleEvent.invoke();

	return CWinApp::OnIdle(lCount);
}
