
// MFCWinRTXamlButtonView.cpp : CMFCWinRTXamlButtonView クラスの実装
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS は、プレビュー、縮小版、および検索フィルター ハンドラーを実装している ATL プロジェクトで定義でき、
// そのプロジェクトとのドキュメント コードの共有を可能にします。
#ifndef SHARED_HANDLERS
#include "MFCWinRTXamlButton.h"
#endif

#include "MFCWinRTXamlButtonDoc.h"
#include "MFCWinRTXamlButtonView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCWinRTXamlButtonView

IMPLEMENT_DYNCREATE(CMFCWinRTXamlButtonView, CView)

BEGIN_MESSAGE_MAP(CMFCWinRTXamlButtonView, CView)
	// 標準印刷コマンド
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CMFCWinRTXamlButtonView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
END_MESSAGE_MAP()

// CMFCWinRTXamlButtonView コンストラクション/デストラクション

CMFCWinRTXamlButtonView::CMFCWinRTXamlButtonView() noexcept
{
	// TODO: 構築コードをここに追加します。

}

CMFCWinRTXamlButtonView::~CMFCWinRTXamlButtonView()
{
}

BOOL CMFCWinRTXamlButtonView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: この位置で CREATESTRUCT cs を修正して Window クラスまたはスタイルを
	//  修正してください。

	return CView::PreCreateWindow(cs);
}

// CMFCWinRTXamlButtonView 描画

void CMFCWinRTXamlButtonView::OnDraw(CDC* /*pDC*/)
{
	CMFCWinRTXamlButtonDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: この場所にネイティブ データ用の描画コードを追加します。
}


// CMFCWinRTXamlButtonView の印刷


void CMFCWinRTXamlButtonView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CMFCWinRTXamlButtonView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 既定の印刷準備
	return DoPreparePrinting(pInfo);
}

void CMFCWinRTXamlButtonView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 印刷前の特別な初期化処理を追加してください。
}

void CMFCWinRTXamlButtonView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 印刷後の後処理を追加してください。
}

void CMFCWinRTXamlButtonView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CMFCWinRTXamlButtonView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CMFCWinRTXamlButtonView の診断

#ifdef _DEBUG
void CMFCWinRTXamlButtonView::AssertValid() const
{
	CView::AssertValid();
}

void CMFCWinRTXamlButtonView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMFCWinRTXamlButtonDoc* CMFCWinRTXamlButtonView::GetDocument() const // デバッグ以外のバージョンはインラインです。
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFCWinRTXamlButtonDoc)));
	return (CMFCWinRTXamlButtonDoc*)m_pDocument;
}
#endif //_DEBUG


// CMFCWinRTXamlButtonView メッセージ ハンドラー


int CMFCWinRTXamlButtonView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// ハンドルを取得
	auto interop = desktopSource.as<IDesktopWindowXamlSourceNative>();

	// ウインドウへアタッチ
	winrt::check_hresult(interop->AttachToWindow(this->m_hWnd));

	//XAML用のHWNDを取得
	HWND hWndXamlIsland = nullptr;
	interop->get_WindowHandle(&hWndXamlIsland);

	//XAML contentの作成。ここにコントロールを追加して行きます
	winrt::Windows::UI::Xaml::Controls::StackPanel xamlContainer;

	//main.xamlを読み込む（MFCのクラスも使おうよ！！）
	CString line, strxaml;
	CStdioFile file;
	file.Open(_T("main.xaml"), CFile::modeRead);
	while (file.ReadString(line))
	{
		strxaml += line;
	}

	//CStringをhstringへ変換し、XamlReaderでControlsへ変換
	winrt::hstring xaml = strxaml.GetString();
	xamlContainer = winrt::Windows::UI::Xaml::Markup::XamlReader::Load(xaml).as<winrt::Windows::UI::Xaml::Controls::StackPanel>();

	//xamlContainerのStackPanelの0番目の要素のボタンのクリックをbuttonRevokerへ登録！長い！
	buttonRevoker = xamlContainer.
		as<winrt::Windows::UI::Xaml::Controls::StackPanel>().
		Children().GetAt(0).
		as<winrt::Windows::UI::Xaml::Controls::Button>().
		Click(winrt::auto_revoke, { this, &CMFCWinRTXamlButtonView::OnXamlButtonClick });


	//XamlSourceに追加して
	desktopSource.Content(xamlContainer);


	//今回はウインドウの(0,0)～(500,500)の正方形の領域にXAMLを表示しています。
	//画面全体に表示させたい場合はGetWindowRectでウインドウの大きさを取得して,それを指定してください。
	//サイズ変更に対応する場合は、OnSize()を追加して、ハンドルを取得して、StackPanelの大きさを指定してください。
	::SetWindowPos(hWndXamlIsland, NULL, 0, 0, 500, 500, SWP_SHOWWINDOW);
	xamlContainer.UpdateLayout();

	return 0;
}

void CMFCWinRTXamlButtonView::OnXamlButtonClick(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const&)
{
	auto msgdlg = winrt::Windows::UI::Popups::MessageDialog(L"Button Click!!");
	msgdlg.as<IInitializeWithWindow>()->Initialize(this->m_hWnd);
	msgdlg.Content(L"Button Click!!");
	msgdlg.ShowAsync();
}
