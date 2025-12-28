#include "pch.h"
#include "framework.h"
#include "TaskManager.h"
#include "TaskManagerDlg.h"
#include "TaskEngine.h"
#include "ExecutorFactory.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CTaskManagerDlg::CTaskManagerDlg(CWnd* pParent)
    : CDialogEx(IDD_TASKMANAGER_DIALOG, pParent)
    , m_txtMatrix(_T(""))
    , m_txtWeb(_T(""))
    , m_txtStats(_T(""))
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTaskManagerDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_TXT_RESULT_MATRIX, m_txtMatrix);
    DDX_Text(pDX, IDC_TXT_RESULT_WEB, m_txtWeb);
    DDX_Text(pDX, IDC_TXT_RESULT_STATS, m_txtStats);
}

BEGIN_MESSAGE_MAP(CTaskManagerDlg, CDialogEx)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_MESSAGE(WM_LOG_UPDATE, &CTaskManagerDlg::OnLogUpdate)
    ON_BN_CLICKED(IDC_BTN_ENGINE_START, &CTaskManagerDlg::OnBnClickedBtnEngineStart)
    ON_BN_CLICKED(IDC_BTN_ENGINE_STOP, &CTaskManagerDlg::OnBnClickedBtnEngineStop)
    ON_BN_CLICKED(IDC_BTN_CALC_MATRIX, &CTaskManagerDlg::OnBnClickedBtnCalcMatrix)
    ON_BN_CLICKED(IDC_BTN_DO_BACKUP, &CTaskManagerDlg::OnBnClickedBtnDoBackup)
    ON_BN_CLICKED(IDC_BTN_WEB_REQUEST, &CTaskManagerDlg::OnBnClickedBtnWebRequest)
    ON_BN_CLICKED(IDC_BTN_ALERT_SVC, &CTaskManagerDlg::OnBnClickedBtnAlertSvc)
    ON_BN_CLICKED(IDC_BTN_ANALYTICS, &CTaskManagerDlg::OnBnClickedBtnAnalytics)
    ON_BN_CLICKED(IDC_BTN_DEMO_DEADLOCK, &CTaskManagerDlg::OnBnClickedBtnDemoDeadlock)
    ON_BN_CLICKED(IDC_BTN_DEMO_SAFE, &CTaskManagerDlg::OnBnClickedBtnDemoSafe)
    ON_STN_CLICKED(IDC_TXT_RESULT_WEB, &CTaskManagerDlg::OnStnClickedTxtResultWeb)
END_MESSAGE_MAP()

BOOL CTaskManagerDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);

    TaskEngine::Instance()->Subscribe(this);
    return TRUE;
}

void CTaskManagerDlg::OnMessageArrived(const std::string& message)
{
    std::string* pMsg = new std::string(message);
    PostMessage(WM_LOG_UPDATE, 0, (LPARAM)pMsg);
}

LRESULT CTaskManagerDlg::OnLogUpdate(WPARAM wParam, LPARAM lParam)
{
    std::string* pMsg = (std::string*)lParam;
    if (!pMsg) return 0;

    std::string msg = *pMsg;
    CString strMsg = CA2T(msg.c_str());

    // 更新列表框
    CListBox* pList = (CListBox*)GetDlgItem(IDC_LISTBOX_MONITOR);
    if (pList) {
        int idx = pList->AddString(strMsg);
        pList->SetCurSel(idx);
    }

    // 解析数据标签
    if (msg.find("[DATA-MATRIX]") != std::string::npos) {
        m_txtMatrix = strMsg.Mid(13);
        UpdateData(FALSE);
    }
    else if (msg.find("[DATA-WEB]") != std::string::npos) {
        m_txtWeb = strMsg.Mid(10);
        UpdateData(FALSE);
    }
    else if (msg.find("[DATA-STATS]") != std::string::npos) {
        m_txtStats = strMsg.Mid(12);
        UpdateData(FALSE);
    }

    // 死锁验证成功提示
    if (msg.find("验证通过") != std::string::npos) {
        ::MessageBox(NULL,
            _T("防死锁演示成功！\n\n防死锁机制确保了异常发生时锁的自动释放。"),
            _T("验证结果"),
            MB_OK | MB_ICONINFORMATION);
    }

    delete pMsg;
    return 0;
}

void CTaskManagerDlg::OnPaint()
{
    if (IsIconic()) {
        CPaintDC dc(this);
        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;
        dc.DrawIcon(x, y, m_hIcon);
    }
    else {
        CDialogEx::OnPaint();
    }
}

HCURSOR CTaskManagerDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CTaskManagerDlg::OnBnClickedBtnEngineStart()
{
    TaskEngine::Instance()->StartEngine();


}

void CTaskManagerDlg::OnBnClickedBtnEngineStop()
{
    TaskEngine::Instance()->StopEngine();
    AfxMessageBox(_T("任务引擎已停止"));
}

void CTaskManagerDlg::OnBnClickedBtnCalcMatrix()
{
    auto task = ExecutorFactory::Build("Matrix");
    if (task) {
        TaskEngine::Instance()->SubmitTask(task, 0, true, 5000);
    }
}

void CTaskManagerDlg::OnBnClickedBtnDoBackup()
{
    auto task = ExecutorFactory::Build("Backup");
    if (task) {
        TaskEngine::Instance()->SubmitTask(task, 1000);

    }
}

void CTaskManagerDlg::OnBnClickedBtnWebRequest()
{
    auto task = ExecutorFactory::Build("WebRequest");
    if (task) {
        TaskEngine::Instance()->SubmitTask(task, 0);
    }
}

void CTaskManagerDlg::OnBnClickedBtnAlertSvc()
{
    auto task = ExecutorFactory::Build("Alert");
    if (task) {
        TaskEngine::Instance()->SubmitTask(task, 1000, true, 60000);
    }
}

void CTaskManagerDlg::OnBnClickedBtnAnalytics()
{
    auto task = ExecutorFactory::Build("Analytics");
    if (task) {
        TaskEngine::Instance()->SubmitTask(task, 0);
    }
}

void CTaskManagerDlg::OnBnClickedBtnDemoDeadlock()
{
    TaskEngine::Instance()->SubmitTask(ExecutorFactory::Build("DeadlockDemo"), 100);
    TaskEngine::Instance()->SubmitTask(ExecutorFactory::Build("Validator"), 3000);
}

void CTaskManagerDlg::OnBnClickedBtnDemoSafe()
{
    TaskEngine::Instance()->SubmitTask(ExecutorFactory::Build("SafeDemo"), 100);
    TaskEngine::Instance()->SubmitTask(ExecutorFactory::Build("Validator"), 3000);
}

void CTaskManagerDlg::OnStnClickedTxtResultWeb()
{
    // TODO: 在此添加控件通知处理程序代码
}
